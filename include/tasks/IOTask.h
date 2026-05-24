#pragma once
#include "../Task.h"

class IOTask : public Task {
public:
    IOTask(int id, std::string name, std::vector<int> dependencies);
    void execute() override;
};