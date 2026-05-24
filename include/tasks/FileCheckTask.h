#pragma once
#include "../Task.h"

/**
 * @brief Short-lived task simulating a quick file system check.
 *
 * Completes in roughly 3 seconds (100 × 30 ms), making it useful as a
 * fast prerequisite dependency in a task graph.
 */
class FileCheckTask : public Task {
public:
    /// @brief Construct a file check task with default medium priority.
    FileCheckTask(int id, std::string name, std::vector<int> dependencies);

    /// @brief Execute 100 short sleep iterations simulating file checks, updating progress each iteration.
    void execute() override;
};
