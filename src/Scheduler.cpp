#include "Scheduler.h"
#include <algorithm>

Scheduler::Scheduler() {}

Scheduler::~Scheduler() {
    stop();
}

void Scheduler::addTask(const std::shared_ptr<Task> &task) {
    std::lock_guard lock(_mutex);
    _tasks[task->getId()] = task;
    _queue.push_back(task);
}

void Scheduler::start() {
    if (_running) return;
    _running = true;
    _paused = false;
    _schedulerThread = std::thread(&Scheduler::schedulerLoop, this);
}

void Scheduler::pause() {
    _paused = true;
}

void Scheduler::resume() {
    _paused = false;
}

void Scheduler::stop() {

    {
        std::lock_guard lock(_mutex);
        for (auto& [id, task] : _tasks)
            task->cancel();
    }

    _running = false;
    _paused= false;

    if (_schedulerThread.joinable())
        _schedulerThread.join();

    _threadPool.waitForAll();
}

bool Scheduler::isReady(const std::shared_ptr<Task>& task) const {
    for (int depId : task->getDependencies()) {
        auto it = _tasks.find(depId);
        if (it ==_tasks.end()) return false;
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

        std::unordered_set<int> visited;
        int ep = getEffectivePriorityHelper(task, visited);
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

                while (_threadPool.hasFreeThread()) {
                    auto task = pickNext();
                    if (!task) break;
                    task->setStatus(Task::TaskStatus::Queued);
                    toSubmit.push_back(task);
                }
            }

            for (auto& task : toSubmit)
                _threadPool.submit(task);

            bool allDone = true;
            {
                std::lock_guard lock(_mutex);
                if (_tasks.empty()) {
                    allDone = false;  // nothing to do yet
                } else {
                    for (const auto& [id, task] : _tasks) {
                        if (task->getStatus() != Task::TaskStatus::Completed &&
                            task->getStatus()!=Task::TaskStatus::Cancelled) {
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

int Scheduler::getEffectivePriorityHelper(const std::shared_ptr<Task>& task,
                                           std::unordered_set<int>& visited) const {
    if (visited.count(task->getId())) return task->getPriority();
    visited.insert(task->getId());

    int maxPriority = task->getPriority();
    for (const auto& [id, other] : _tasks) {
        if (other->getStatus() == Task::TaskStatus::Completed) continue;
        if (other->getStatus() == Task::TaskStatus::Cancelled) continue;
        for (int depId : other->getDependencies()) {
            if (depId == task->getId()) {
                int inherited = getEffectivePriorityHelper(other, visited);
                if (inherited > maxPriority) maxPriority = inherited;
            }
        }
    }
    return maxPriority;
}


