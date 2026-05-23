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
    void restart();

    void waitForAll() {
        // drain queue first
        {
            std::lock_guard lock(_mutex);
            while (!_taskQueue.empty()) _taskQueue.pop();
        }

        // then wait for running workers
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