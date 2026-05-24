#pragma once
#include <unordered_map>
#include <vector>
#include <memory>
#include <mutex>
#include <thread>
#include <condition_variable>
#include "Task.h"
#include "ThreadPool.h"
#include <unordered_set>

/**
 * @brief Dispatches ready tasks to a thread pool in priority order.
 *
 * Runs a background thread that continuously evaluates which tasks are
 * ready (all dependencies completed) and submits them to the ThreadPool
 * as workers become free. Supports pause/resume and priority inheritance.
 */
class Scheduler {
private:
    std::unordered_map<int, std::shared_ptr<Task>> _tasks;
    std::vector<std::shared_ptr<Task>> _queue;

    std::mutex _mutex;
    std::condition_variable _cv;
    std::thread _schedulerThread;
    std::atomic<bool> _running = false;
    std::atomic<bool> _paused = false;

    ThreadPool _threadPool;

    /// @brief Return true if all dependencies of the task have completed.
    /// @param task Task to evaluate.
    bool isReady(const std::shared_ptr<Task>& task) const;

    /// @brief Select the highest-effective-priority ready task from the queue.
    /// @return The chosen task, or nullptr if none is ready.
    std::shared_ptr<Task> pickNext();

    /// @brief Background loop that dispatches ready tasks and detects completion.
    void schedulerLoop();

public:
    Scheduler();
    ~Scheduler();

    /// @brief Register a task and add it to the dispatch queue.
    /// @param task Task to add.
    void addTask(const std::shared_ptr<Task> &task);

    /// @brief Start the scheduler background thread.
    void start();

    /// @brief Stop scheduling, cancel pending tasks, and join the background thread.
    void stop();

    /// @brief Return all registered tasks indexed by ID.
    const std::unordered_map<int, std::shared_ptr<Task>>& getTasks() const { return _tasks; }

    /// @brief Return the worker thread states from the underlying pool.
    const std::vector<WorkerThread>& getWorkers() const { return _threadPool.getWorkers(); }

    /// @brief Suspend dispatching without stopping the scheduler thread.
    void pause();

    /// @brief Resume dispatching after a pause.
    void resume();

    /// @brief Clear all tasks and the dispatch queue.
    void reset() {
        std::lock_guard lock(_mutex);
        _tasks.clear();
        _queue.clear();
    }

    /// @brief Remove a task by ID from both the registry and the dispatch queue.
    /// @param id ID of the task to remove.
    void removeTask(int id) {
        std::lock_guard lock(_mutex);
        _tasks.erase(id);
        _queue.erase(
            std::remove_if(_queue.begin(), _queue.end(),
                [id](const std::shared_ptr<Task>& t) { return t->getId() == id; }),
            _queue.end());
    }

    /// @brief Resize the underlying thread pool.
    /// @param count Desired number of worker threads.
    void setThreadCount(unsigned int count) { _threadPool.setThreadCount(count); }

    /// @brief Return the current number of worker threads.
    unsigned int getThreadCount() const { return _threadPool.getThreadCount(); }

private:
    /// @brief Recursively compute the effective priority of a task by priority inheritance.
    ///
    /// A task inherits the maximum priority of all tasks that depend on it,
    /// preventing priority inversion when high-priority tasks are blocked on prerequisites.
    /// @param task Task to evaluate.
    /// @param visited Tracks already-visited nodes to break cycles.
    /// @return Effective priority value.
    int getEffectivePriorityHelper(const std::shared_ptr<Task>& task,
                                std::unordered_set<int>& visited) const;

};
