#pragma once
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <memory>
#include <functional>
#include "Task.h"

struct WorkerThread {
    std::thread thread;
    std::shared_ptr<Task> currentTask = nullptr;
    Task::TaskStatus status = Task::TaskStatus::Planned;  // Idle = Planned for now
    int id;
};

class ThreadPool {
private:
    std::vector<WorkerThread> _workers;
    std::queue<std::shared_ptr<Task>> _taskQueue;

    mutable std::mutex _mutex;
    std::condition_variable _cv;
    bool _running = false;

    unsigned int _threadCount;

    void workerLoop(int workerId);

public:
    ThreadPool();
    ~ThreadPool();

    void submit(std::shared_ptr<Task> task);
    bool hasFreeThread() const;

    const std::vector<WorkerThread>& getWorkers() const { return _workers; }


};