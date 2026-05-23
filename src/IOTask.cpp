#include "../include/IOTask.h"
#include <thread>
void IOTask::execute() {
    for (int i = 0; i <= 100; i++) {
        if (_cancelled) return;
        // simulates waiting for disk/network — mostly sleeping
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        _progress = i;
    }
}