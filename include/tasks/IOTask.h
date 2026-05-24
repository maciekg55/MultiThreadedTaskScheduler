#pragma once
#include "../Task.h"

/**
 * @brief I/O-bound task that simulates waiting for disk or network access.
 *
 * Each of the 100 iterations sleeps for 200 ms, modelling a task that
 * spends most of its time blocked rather than consuming CPU.
 */
class IOTask : public Task {
public:
    /// @brief Construct an I/O task with default medium priority.
    IOTask(int id, std::string name, std::vector<int> dependencies);

    /// @brief Execute 100 sleep iterations simulating blocked I/O, updating progress each iteration.
    void execute() override;
};
