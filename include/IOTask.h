#pragma once
#include "Task.h"

class IOTask : public Task {
public:
    IOTask(int id, std::string name, std::vector<int> dependencies)
        : Task(id, std::move(name), 2, std::move(dependencies)) {
        _estTime = 8;
    }
    void execute() override;
};