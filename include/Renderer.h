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

/**
 * @brief Composes all UI panels and routes input to the appropriate views.
 *
 * Owns one instance of each view and acts as a facade for the Engine,
 * exposing pending user actions (add/edit/delete, preset selection, thread changes)
 * as simple query methods.
 */
class Renderer {
public:
    /// @brief Construct all views sharing the given window, font, and scheduler.
    Renderer(sf::RenderWindow& window, Scheduler& scheduler);

    /// @brief Draw all views to the window.
    void draw();

    /// @brief Forward a scroll event to the views that handle scrolling.
    /// @param delta    Scroll amount.
    /// @param mousePos Cursor position used to identify the target panel.
    void handleScroll(float delta, sf::Vector2f mousePos);

    /// @brief Forward a mouse click to the active views.
    /// @param mousePos Cursor position in window coordinates.
    void handleClick(sf::Vector2f mousePos);

    /// @brief Forward a text character to the form view for editing.
    /// @param unicode Unicode code point of the entered character.
    void handleTextInput(uint32_t unicode);

    /// @brief Reset the Gantt chart's reference start time to now.
    void resetStartTime();

    /// @brief Return true if the form has a new task ready to be consumed.
    bool hasPendingAdd() const;

    /// @brief Return true if the form has an edited task ready to be consumed.
    bool hasPendingEdit() const;

    /// @brief Return the ID of a task pending deletion, or -1 if none.
    int hasPendingDelete();

    /// @brief Return the ID of the task currently being edited.
    int getEditTaskId() const;

    /// @brief Retrieve and clear the pending task from the form view.
    /// @return The new or edited task.
    std::shared_ptr<Task> consumeTask();

    /// @brief Return the bounding rect of the Start button.
    sf::FloatRect getStartButtonRect() const { return _statsBar.getStartBtn(); }

    /// @brief Return the bounding rect of the Pause button.
    sf::FloatRect getPauseButtonRect() const { return _statsBar.getPauseBtn(); }

    /// @brief Return the bounding rect of the Stop/Clear button.
    sf::FloatRect getStopButtonRect() const { return _statsBar.getStopBtn(); }

    /// @brief Push scheduler state to the views that use it for rendering.
    /// @param started   True if the scheduler has been started.
    /// @param paused    True if the scheduler is currently paused.
    /// @param allDone   True if all tasks have finished or been cancelled.
    /// @param taskCount Total number of registered tasks.
    void setStatsState(bool started, bool paused, bool allDone, int taskCount) {
        _statsBar.setState(started, paused, allDone, taskCount);
        _ganttView.setAllDone(allDone);
        _ganttView.setStarted(started);

    }

    /// @brief Set the next auto-assigned task ID based on the highest existing ID.
    /// @param maxExistingId Highest ID already in use; next ID will be maxExistingId+1.
    void loadNextTaskId(int maxExistingId) {
        _nextTaskId = maxExistingId + 1;
    }

    /// @brief Populate the preset selector with available preset names.
    /// @param presets List of preset names without extension.
    void setPresets(const std::vector<std::string>& presets) {
        _presetSelector.setPresets(presets);
    }

    /// @brief Return true if the user has selected a preset.
    bool hasPresetSelection() const { return _presetSelector.hasSelection(); }

    /// @brief Retrieve and clear the pending preset selection name.
    /// @return The selected preset name.
    std::string consumePresetSelection() { return _presetSelector.consumeSelection(); }

    /// @brief Forward a click to the stats bar (thread count buttons).
    /// @param mouse Cursor position in window coordinates.
    void handleStatsBarClick(sf::Vector2f mouse) { _statsBar.handleClick(mouse); }

    /// @brief Return true if there is a pending thread count change.
    bool hasThreadDelta() { return _statsBar.consumeThreadDelta() != 0; }

    /// @brief Retrieve and clear the pending thread count delta (+1 or -1).
    /// @return The thread count change requested by the user.
    int consumeThreadDelta() { return _statsBar.consumeThreadDelta(); }



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
