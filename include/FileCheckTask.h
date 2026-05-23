#pragma once
#include "Task.h"

class FileCheckTask : public Task {
public:
    FileCheckTask(int id, std::string name, std::vector<int> dependencies);
    void execute() override;
};