#pragma once
#include "Task.h"

class ComputationTask: public Task {
public:
    ComputationTask(int id, std::string name, std::vector<int> deps);

    void execute() override;
};