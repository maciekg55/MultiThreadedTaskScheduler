#pragma once
#include "Task.h"

class FileCheckTask : public Task {
public:
    FileCheckTask(int id, std::string name, std::vector<int> dependencies)
        : Task(id, std::move(name), 2, std::move(dependencies)) {
        _estTime = 2;
    }
    void execute() override;
};