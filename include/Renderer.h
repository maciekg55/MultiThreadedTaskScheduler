#pragma once
#include <SFML/Graphics.hpp>
#include "Scheduler.h"
#include "views/ThreadView.h"
#include "views/GanttGraphView.h"
#include "views/DependenciesGraphView.h"
#include "views/TaskQueueView.h"
#include "views/TaskDetailsView.h"
#include "views/StatsBar.h"
#include "views/AddTaskFormView.h"
#include "views/PresetSelectorView.h"

class Renderer {
public:
    Renderer(sf::RenderWindow& window, Scheduler& scheduler);

    void draw();
    void handleScroll(float delta, sf::Vector2f mousePos);
    void handleClick(sf::Vector2f mousePos);
    void handleTextInput(uint32_t unicode);
    void resetStartTime();

    bool hasPendingAdd() const;
    bool hasPendingEdit() const;
    int hasPendingDelete();
    int getEditTaskId() const;
    std::shared_ptr<Task> consumeTask();

    sf::FloatRect getStartButtonRect() const { return _statsBar.getStartBtn(); }
    sf::FloatRect getPauseButtonRect() const { return _statsBar.getPauseBtn(); }
    sf::FloatRect getStopButtonRect() const { return _statsBar.getStopBtn(); }

    void setStatsState(bool started, bool paused, bool allDone, int taskCount) {
        _statsBar.setState(started, paused, allDone, taskCount);
        _ganttView.setAllDone(allDone);
        _ganttView.setStarted(started);

    }

    void loadNextTaskId(int maxExistingId) {
        _nextTaskId = maxExistingId + 1;
    }

    void setPresets(const std::vector<std::string>& presets) {
        _presetSelector.setPresets(presets);
    }

    bool hasPresetSelection() const { return _presetSelector.hasSelection(); }
    std::string consumePresetSelection() { return _presetSelector.consumeSelection(); }



private:
    sf::RenderWindow& _window;
    Scheduler& _scheduler;
    sf::Font _font;

    ThreadView _threadView;
    GanttGraphView _ganttView;
    DependenciesGraphView _dependencyView;
    TaskQueueView _taskQueueView;
    TaskDetailsView _taskDetailsView;
    StatsBar _statsBar;
    AddTaskFormView _formView;
    PresetSelectorView _presetSelector;


    bool _pendingDelete = false;
    int _deleteTaskId = -1;
    int _nextTaskId = 0;


};