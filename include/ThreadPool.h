#pragma once
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <memory>
#include <functional>
#include "Task.h"

/**
 * @brief Bundles a thread with its runtime state for the pool.
 */
struct WorkerThread {
    std::thread thread;
    std::shared_ptr<Task> currentTask = nullptr;
    Task::TaskStatus status = Task::TaskStatus::Planned;
    int id;
};

/**
 * @brief Fixed-size pool of worker threads that execute tasks from a shared queue.
 *
 * Workers block on a condition variable when idle and wake when a task is submitted.
 * The thread count can be adjusted at runtime while the pool is running.
 */
class ThreadPool {
private:
    std::vector<WorkerThread> _workers;
    std::queue<std::shared_ptr<Task>> _taskQueue;

    mutable std::mutex _mutex;
    std::condition_variable _cv;
    bool _running = false;

    unsigned int _threadCount;

    /// @brief Worker thread body: dequeues and executes tasks until stopped or resized out.
    /// @param workerId Index into _workers for this thread.
    void workerLoop(int workerId);

public:
    /// @brief Construct the pool and spawn worker threads sized to hardware concurrency.
    ThreadPool();
    ~ThreadPool();

    /// @brief Enqueue a task for execution by the next free worker.
    /// @param task Task to submit.
    void submit(const std::shared_ptr<Task> &task);

    /// @brief Return true if at least one worker has no current task.
    bool hasFreeThread() const;

    /// @brief Return read-only access to all workers and their state.
    const std::vector<WorkerThread>& getWorkers() const { return _workers; }

    /// @brief Stop all threads, clear the queue, and restart fresh workers.
    void restart();

    /// @brief Resize the pool to the given number of workers.
    /// @param count Desired thread count, clamped to hardware concurrency minus one.
    void setThreadCount(unsigned int count);

    /// @brief Return the current number of worker threads.
    unsigned int getThreadCount() const { return _threadCount; }

    /// @brief Drain the pending queue and block until all running workers become idle.
    void waitForAll() {
        {
            std::lock_guard lock(_mutex);
            while (!_taskQueue.empty()) _taskQueue.pop();
        }

        while (true) {
            {
                std::lock_guard lock(_mutex);
                bool anyRunning = false;
                for (const auto& w : _workers)
                    if (w.currentTask != nullptr) { anyRunning = true; break; }
                if (!anyRunning) return;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
    }

};
