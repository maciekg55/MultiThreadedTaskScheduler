#include "../include/RenderTask.h"
#include <thread>
void RenderTask::execute() {
    for (int i = 0; i <= 100; i++) {
        if (_cancelled) return;
        // CPU intensive like computation but longer
        volatile long sum = 0;
        for (int j = 0; j < 2000000; j++) sum += j;
        _progress = i;
        std::this_thread::sleep_for(std::chrono::milliseconds(120));
    }
}