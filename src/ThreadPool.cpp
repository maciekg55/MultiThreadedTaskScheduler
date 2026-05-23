#include "ThreadPool.h"

ThreadPool::ThreadPool() {
    _threadCount = std::thread::hardware_concurrency();
    if (_threadCount == 0) _threadCount = 4;

    _running = true;
    _workers.resize(_threadCount);

    for (unsigned int i = 0; i < _threadCount; i++) {
        _workers[i].id = i;
        _workers[i].thread = std::thread(&ThreadPool::workerLoop, this, i);
    }
}

ThreadPool::~ThreadPool() {
    {
        std::lock_guard lock(_mutex);
        _running = false;
    }
    _cv.notify_all();  // wake all sleeping workers so they can exit

    for (auto& worker : _workers) {
        if (worker.thread.joinable())
            worker.thread.join();
    }
}

void ThreadPool::submit(std::shared_ptr<Task> task) {
    {
        std::lock_guard<std::mutex> lock(_mutex);
        _taskQueue.push(task);
    }
    _cv.notify_one();  // wake one sleeping worker
}

bool ThreadPool::hasFreeThread() const {
    for (const auto& worker : _workers)
        if (worker.currentTask == nullptr) return true;
    return false;
}

void ThreadPool::workerLoop(int workerId) {
    while (true) {
        std::shared_ptr<Task> task;

        {
            std::unique_lock lock(_mutex);
            // sleep until there's a task OR we're shutting down
            _cv.wait(lock, [this] {
                return !_taskQueue.empty() || !_running;
            });

            if (!_running && _taskQueue.empty()) return;  // clean exit

            task = _taskQueue.front();
            _taskQueue.pop();
            _workers[workerId].currentTask = task;
            _workers[workerId].status = Task::TaskStatus::Running;
        }
        task->setAssignedThread(workerId);  // ← must be first
        task->markStarted();
        task->execute();
        task->markEnded();
        task->setStatus(Task::TaskStatus::Completed);

        {
            std::lock_guard lock(_mutex);
            _workers[workerId].currentTask = nullptr;
            _workers[workerId].status = Task::TaskStatus::Planned;  // back to idle
        }
    }
}