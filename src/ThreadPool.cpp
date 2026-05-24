#include "ThreadPool.h"

ThreadPool::ThreadPool() {
    unsigned int hw = std::thread::hardware_concurrency();
    _threadCount = hw > 1 ? hw - 1 : 1;  // reserve one for main/SFML thread

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
    _cv.notify_all();

    for (auto& worker : _workers) {
        if (worker.thread.joinable())
            worker.thread.join();
    }
}

void ThreadPool::submit(const std::shared_ptr<Task> &task) {
    {
        std::lock_guard lock(_mutex);
        _taskQueue.push(task);
    }
    _cv.notify_one();
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
            _cv.wait(lock, [this, workerId] {
                return !_taskQueue.empty() || !_running || workerId >= (int)_threadCount;
            });
            if ((!_running && _taskQueue.empty()) || workerId >= (int)_threadCount)
                return;
            task = _taskQueue.front();
            _taskQueue.pop();
            _workers[workerId].currentTask = task;
            _workers[workerId].status = Task::TaskStatus::Running;
        }

        task->setAssignedThread(workerId);
        task->markStarted();
        task->setStatus(Task::TaskStatus::Running);
        task->execute();
        task->markEnded();
        task->setStatus(Task::TaskStatus::Completed);

        {
            std::lock_guard lock(_mutex);
            _workers[workerId].currentTask = nullptr;
            _workers[workerId].status = Task::TaskStatus::Planned;
        }
        // release shared_ptr AFTER nulling currentTask
        task.reset();
    }
}

void ThreadPool::restart() {
    {
        std::lock_guard lock(_mutex);
        _running = false;
    }
    _cv.notify_all();
    for (auto& worker : _workers) {
        if (worker.thread.joinable())
            worker.thread.join();
    }

    while (!_taskQueue.empty()) _taskQueue.pop();
    for (auto& w : _workers) w.currentTask = nullptr;

    _running = true;
    for (unsigned int i = 0; i < _threadCount; i++) {
        _workers[i].thread = std::thread(&ThreadPool::workerLoop, this, i);
    }
}


void ThreadPool::setThreadCount(unsigned int count) {
    unsigned int maxThreads = std::max(1u, std::thread::hardware_concurrency() - 1);
    count = std::clamp(count, 1u, maxThreads);
    if (count == _threadCount) return;

    if (count < _threadCount) {
        {
            std::lock_guard lock(_mutex);
            _threadCount = count;
        }
        _cv.notify_all();
        for (unsigned int i = count; i < _workers.size(); i++) {
            if (_workers[i].thread.joinable())
                _workers[i].thread.join();
        }
        _workers.resize(count);
    } else {
        unsigned int oldCount = _threadCount;
        _threadCount = count;
        _workers.resize(count);
        for (unsigned int i = oldCount; i < count; i++) {
            _workers[i].id = i;
            _workers[i].currentTask = nullptr;
            _workers[i].status = Task::TaskStatus::Planned;
            _workers[i].thread = std::thread(&ThreadPool::workerLoop, this, i);
        }
    }
}



