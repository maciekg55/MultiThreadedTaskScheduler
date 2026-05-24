#pragma once
#include "Task.h"

/**
 * @brief CPU-bound task that performs a busy-loop computation.
 *
 * Simulates a heavy compute workload with 100 iterations of arithmetic,
 * making it suitable for demonstrating CPU saturation across threads.
 */
class ComputationTask: public Task {
public:
    /// @brief Construct a computation task with default medium priority.
    ComputationTask(int id, std::string name, std::vector<int> deps);

    /// @brief Execute 100 iterations of CPU-bound arithmetic, updating progress each iteration.
    void execute() override;
};
