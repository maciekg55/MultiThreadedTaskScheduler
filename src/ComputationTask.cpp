#include "ComputationTask.h"
#include <thread>
#include <chrono>

ComputationTask::ComputationTask(int id, std::string name, std::vector<int> dependencies)
    : Task(id, std::move(name), 2, std::move(dependencies)) {
    _estTime = 10;  // 100 iterations * 100ms
}

void ComputationTask::execute() {
    for (int i = 0; i <= 100; i++) {
        if (_cancelled) return;  // ← exit early if cancelled
        volatile long sum = 0;
        for (int j = 0; j < 20000000; j++) sum += j;
        _progress = i;
    }
}
