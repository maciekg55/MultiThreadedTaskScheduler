#include "../include/FileCheckTask.h"
#include <thread>
void FileCheckTask::execute() {
    // fast task — just a quick check
    for (int i = 0; i <= 100; i++) {
        if (_cancelled) return;
        std::this_thread::sleep_for(std::chrono::milliseconds(30));
        _progress = i;
    }
}