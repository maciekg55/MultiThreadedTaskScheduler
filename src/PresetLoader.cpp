#include "PresetLoader.h"
#include "tasks/ComputationTask.h"
#include "tasks/IOTask.h"
#include "tasks/FileCheckTask.h"
#include "tasks/RenderTask.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <filesystem>


std::vector<TaskPreset> PresetLoader::load(const std::string& filepath) {
    std::vector<TaskPreset> presets;
    std::ifstream file(filepath);

    if (!file.is_open()) {
        std::cerr << "PresetLoader: could not open " << filepath << "\n";
        return presets;
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;

        std::istringstream ss(line);
        std::string keyword;
        ss >> keyword;
        if (keyword != "task") continue;

        TaskPreset p;
        std::string priorityStr;

        ss >> p.id >> p.name >> p.type >> priorityStr;

        if (priorityStr == "high") p.priority = 3;
        else if (priorityStr == "medium") p.priority = 2;
        else p.priority = 1;

        int dep;
        while (ss >> dep){ p.deps.push_back(dep); }

        presets.push_back(p);
    }
    return presets;
}

std::shared_ptr<Task> PresetLoader::createTask(const TaskPreset& preset) {
    std::shared_ptr<Task> task;

    if (preset.type == "computation") {
        task = std::make_shared<ComputationTask>(preset.id, preset.name, preset.deps);
    }
    else if (preset.type == "io") {
        task = std::make_shared<IOTask>(preset.id, preset.name, preset.deps);
    }
    else if (preset.type == "filecheck") {
        task = std::make_shared<FileCheckTask>(preset.id, preset.name, preset.deps);
    }
    else if (preset.type == "render") {
        task = std::make_shared<RenderTask>(preset.id, preset.name, preset.deps);
    }
    else {
        task = std::make_shared<ComputationTask>(preset.id, preset.name, preset.deps);
    }

    task->setPriority(preset.priority);
    return task;
}

std::vector<std::string> PresetLoader::listPresets(const std::string& folder) {
    std::vector<std::string> names;
    for (const auto& entry : std::filesystem::directory_iterator(folder)) {
        if (entry.path().extension() == ".preset")
            names.push_back(entry.path().stem().string());
    }
    std::sort(names.begin(), names.end());
    return names;
}
