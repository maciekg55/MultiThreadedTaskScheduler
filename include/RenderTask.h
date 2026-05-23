#pragma once
#include "Task.h"

class RenderTask : public Task {
public:
    RenderTask(int id, std::string name, std::vector<int> dependencies)
        : Task(id, std::move(name), 2, std::move(dependencies)) {
        _estTime = 12;
    }
    void execute() override;
};