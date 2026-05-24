#include "tasks/IOTask.h"
#include <thread>

IOTask::IOTask(int id, std::string name, std::vector<int> dependencies)
    : Task(id, std::move(name), 2, std::move(dependencies)) {
    _estTime = 20;  // 100 iterations * 200ms
}

void IOTask::execute() {
    for (int i = 0; i <= 100; i++) {
        if (_cancelled) return;
        // simulates waiting for disk/network — mostly sleeping
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        _progress = i;
    }
}