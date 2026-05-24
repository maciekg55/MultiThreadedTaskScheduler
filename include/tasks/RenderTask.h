#pragma once
#include "../Task.h"

/**
 * @brief Mixed CPU and time-bound task simulating a rendering workload.
 *
 * Each of the 100 iterations performs a small arithmetic loop then sleeps
 * for 120 ms, combining CPU work with a fixed frame budget to model
 * GPU-bound rendering pipelines.
 */
class RenderTask : public Task {
public:
    /// @brief Construct a render task with default medium priority.
    RenderTask(int id, std::string name, std::vector<int> dependencies);

    /// @brief Execute 100 compute-and-sleep iterations simulating frame rendering, updating progress each iteration.
    void execute() override;
};
