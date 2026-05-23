#include "Scheduler.h"
#include <algorithm>

Scheduler::Scheduler() {}

Scheduler::~Scheduler() {
    stop();
}

void Scheduler::addTask(std::shared_ptr<Task> task) {
    std::lock_guard lock(_mutex);
    _tasks[task->getId()] = task;
    _queue.push_back(task);
}

void Scheduler::start() {
    if (_running) return;  // already running, ignore
    _running = true;
    _paused  = false;
    _schedulerThread = std::thread(&Scheduler::schedulerLoop, this);
}

void Scheduler::pause() {
    _paused = true;
}

void Scheduler::resume() {
    _paused = false;
}

void Scheduler::stop() {
    if (!_running) return;

    // cancel all tasks so execute() returns quickly
    {
        std::lock_guard<std::mutex> lock(_mutex);
        for (auto& [id, task] : _tasks)
            task->cancel();
    }

    _running = false;
    _paused  = false;

    if (_schedulerThread.joinable())
        _schedulerThread.join();
}

bool Scheduler::isReady(const std::shared_ptr<Task>& task) const {
    for (int depId : task->getDependencies()) {
        auto it = _tasks.find(depId);
        if (it == _tasks.end()) return false;
        if (it->second->getStatus() != Task::TaskStatus::Completed) return false;
    }
    return true;
}

std::shared_ptr<Task> Scheduler::pickNext() {
    std::shared_ptr<Task> best = nullptr;
    int bestPriority = -1;

    for (auto& task : _queue) {
        if (task->getStatus() != Task::TaskStatus::Planned) continue;
        if (!isReady(task)) continue;

        int ep = getEffectivePriority(task);
        if (ep > bestPriority) {
            bestPriority = ep;
            best = task;
        }
    }
    return best;
}
void Scheduler::schedulerLoop() {
    while (_running) {
        if (!_paused) {
            std::vector<std::shared_ptr<Task>> toSubmit;

            {
                std::lock_guard lock(_mutex);

                // cancel tasks with missing dependencies — they'll never be ready
                for (auto& task : _queue) {
                    if (task->getStatus() != Task::TaskStatus::Planned) continue;
                    for (int depId : task->getDependencies()) {
                        if (_tasks.find(depId) == _tasks.end()) {
                            task->setStatus(Task::TaskStatus::Cancelled);
                            break;
                        }
                    }
                }

                // dispatch ready tasks to free threads
                while (_threadPool.hasFreeThread()) {
                    auto task = pickNext();
                    if (!task) break;
                    task->setStatus(Task::TaskStatus::Running);
                    toSubmit.push_back(task);
                }
            }

            // submit outside the lock
            for (auto& task : toSubmit)
                _threadPool.submit(task);

            // check if all tasks are done
            bool allDone = true;
            {
                std::lock_guard lock(_mutex);
                if (_tasks.empty()) {
                    allDone = false;  // nothing to do yet
                } else {
                    for (const auto& [id, task] : _tasks) {
                        if (task->getStatus() != Task::TaskStatus::Completed &&
                            task->getStatus() != Task::TaskStatus::Cancelled) {
                            allDone = false;
                            break;
                            }
                    }
                }
            }

            if (allDone) {
                _running = false;
                break;
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

int Scheduler::getEffectivePriority(const std::shared_ptr<Task>& task) const {
    int maxPriority = task->getPriority();

    // check if any other task depends on this one
    for (const auto& [id, other] : _tasks) {
        if (other->getStatus() == Task::TaskStatus::Completed) continue;
        if (other->getStatus() == Task::TaskStatus::Cancelled) continue;

        for (int depId : other->getDependencies()) {
            if (depId == task->getId()) {
                // other task depends on us — inherit its effective priority
                int inherited = getEffectivePriority(other);
                if (inherited > maxPriority)
                    maxPriority = inherited;
            }
        }
    }
    return maxPriority;
}


