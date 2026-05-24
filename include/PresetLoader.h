#pragma once
#include <string>
#include <vector>
#include <memory>
#include "Task.h"

struct TaskPreset {
    int id;
    std::string name;
    std::string type;
    int priority;
    std::vector<int> deps;
};

class PresetLoader {
public:
    static std::vector<TaskPreset> load(const std::string& filepath);
    static std::shared_ptr<Task> createTask(const TaskPreset& preset);
    static std::vector<std::string> listPresets(const std::string& folder);

};