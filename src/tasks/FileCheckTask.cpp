#include "tasks/FileCheckTask.h"
#include <thread>

FileCheckTask::FileCheckTask(int id, std::string name, std::vector<int> dependencies)
    : Task(id, std::move(name), 2, std::move(dependencies)) {
    _estTime = 3;
}

void FileCheckTask::execute() {
    // fast task — just a quick check
    for (int i = 0; i <= 100; i++) {
        if (_cancelled) return;
        std::this_thread::sleep_for(std::chrono::milliseconds(30));
        _progress = i;
    }
}