#pragma once
#include <SFML/Graphics.hpp>
#include "../Scheduler.h"
#include <chrono>

/**
 * @brief Timeline chart mapping tasks to their worker threads over time.
 *
 * Each row represents a thread; bars show start/end times per task, color-coded
 * by priority (background fill) and status (border). Once all tasks finish, the
 * timeline freezes at the last task's end time rather than continuing to advance.
 */
class GanttGraphView {
public:
    /// @brief Construct the Gantt view bound to the given window, font, and scheduler.
    GanttGraphView(sf::RenderWindow& window, sf::Font& font, Scheduler& scheduler);

    /// @brief Draw the Gantt chart panel to the window.
    void draw();

    /// @brief Reset the chart's reference start time to the current moment.
    void resetStartTime() { _appStartTime = std::chrono::steady_clock::now(); }

    /// @brief Notify the view that all tasks have completed so the timeline can freeze.
    /// @param allDone True when all tasks are finished or cancelled.
    void setAllDone(bool allDone) { _allDone = allDone; }

    /// @brief Notify the view that the scheduler has started so rendering begins.
    /// @param started True once the scheduler has been started.
    void setStarted(bool started) { _started = started; }

private:
    sf::RenderWindow& _window;
    sf::Font& _font;
    Scheduler&_scheduler;

    std::chrono::steady_clock::time_point _appStartTime;
    bool _allDone = false;
    bool _started = false;
};
