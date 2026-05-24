#pragma once
#include <string>
#include <vector>
#include <memory>
#include "Task.h"

/**
 * @brief Raw data parsed from a single task entry in a preset file.
 */
struct TaskPreset {
    int id;
    std::string name;
    std::string type;
    int priority;
    std::vector<int> deps;
};

/**
 * @brief Reads .preset files and instantiates the corresponding Task objects.
 */
class PresetLoader {
public:
    /// @brief Parse all task entries from a preset file.
    /// @param filepath Path to the .preset file.
    /// @return Ordered list of TaskPreset descriptors.
    static std::vector<TaskPreset> load(const std::string& filepath);

    /// @brief Instantiate the concrete Task subclass described by a preset.
    /// @param preset Parsed preset descriptor.
    /// @return Shared pointer to the newly created task.
    static std::shared_ptr<Task> createTask(const TaskPreset& preset);

    /// @brief Enumerate all .preset files in a directory by stem name.
    /// @param folder Directory to scan.
    /// @return Sorted list of preset names without the .preset extension.
    static std::vector<std::string> listPresets(const std::string& folder);

};
