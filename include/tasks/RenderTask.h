#pragma once
#include "../Task.h"

class RenderTask : public Task {
public:
    RenderTask(int id, std::string name, std::vector<int> dependencies);
    void execute() override;
};