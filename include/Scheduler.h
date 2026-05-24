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

class Scheduler {
private:
    std::unordered_map<int, std::shared_ptr<Task>> _tasks;
    std::vector<std::shared_ptr<Task>> _queue;

    std::mutex _mutex;
    std::condition_variable _cv;
    std::thread _schedulerThread;
    std::atomic<bool> _running = false;
    std::atomic<bool> _paused  = false;

    ThreadPool _threadPool;

    bool isReady(const std::shared_ptr<Task>& task) const;
    std::shared_ptr<Task> pickNext();
    void schedulerLoop();

public:
    Scheduler();
    ~Scheduler();

    void addTask(const std::shared_ptr<Task> &task);
    void start();
    void stop();

    const std::unordered_map<int, std::shared_ptr<Task>>& getTasks() const { return _tasks; }
    const std::vector<WorkerThread>& getWorkers() const { return _threadPool.getWorkers(); }

    void pause();
    void resume();

    void reset() {
        std::lock_guard lock(_mutex);
        _tasks.clear();
        _queue.clear();
    }

    void removeTask(int id) {
        std::lock_guard lock(_mutex);
        _tasks.erase(id);
        _queue.erase(
            std::remove_if(_queue.begin(), _queue.end(),
                [id](const std::shared_ptr<Task>& t) { return t->getId() == id; }),
            _queue.end());
    }
private:
    int getEffectivePriorityHelper(const std::shared_ptr<Task>& task,
                                std::unordered_set<int>& visited) const;

};
