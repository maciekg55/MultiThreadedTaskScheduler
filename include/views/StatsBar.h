#pragma once
#include <SFML/Graphics.hpp>
#include "../Scheduler.h"

/**
 * @brief Bottom status bar displaying task statistics, thread controls, and action buttons.
 *
 * Renders live task counts, thread count controls, and the Start/Pause/Stop/Add buttons.
 * Thread count buttons are only interactive before scheduling starts.
 */
class StatsBar {
public:
    /// @brief Construct the stats bar bound to the given window, font, and scheduler.
    StatsBar(sf::RenderWindow& window, sf::Font& font, Scheduler& scheduler);

    /// @brief Draw the bar and all its controls to the window.
    void draw();

    /// @brief Return the bounding rect of the Start button.
    sf::FloatRect getStartBtn() const {return _startBtn;}

    /// @brief Return the bounding rect of the Pause/Resume button.
    sf::FloatRect getPauseBtn() const {return _pauseBtn;}

    /// @brief Return the bounding rect of the Stop/Clear button.
    sf::FloatRect getStopBtn() const {return _stopBtn;}

    /// @brief Return the bounding rect of the Add Task button.
    sf::FloatRect getAddBtn() const {return _addBtn;}

    /// @brief Update scheduler state used to render button appearances.
    /// @param started   True if the scheduler has been started.
    /// @param paused    True if currently paused.
    /// @param allDone   True if all tasks are complete or cancelled.
    /// @param taskCount Total number of registered tasks.
    void setState(bool started, bool paused, bool allDone, int taskCount) {
        _started = started;
        _paused = paused;
        _allDone = allDone;
        _taskCount = taskCount;
    }

    /// @brief Retrieve and clear the pending thread count delta.
    /// @return +1 to increase, -1 to decrease, 0 if unchanged.
    int consumeThreadDelta() {
        int d = _pendingThreadDelta;
        _pendingThreadDelta = 0;
        return d;
    }

    /// @brief Handle a mouse click, recording any thread count button presses.
    /// @param mouse Cursor position in window coordinates.
    void handleClick(sf::Vector2f mouse) {
        if (!_started) {
            if (_threadMinusBtn.contains(mouse)) _pendingThreadDelta = -1;
            if (_threadPlusBtn.contains(mouse)) _pendingThreadDelta = 1;
        }
    }

private:

    int _taskCount = 0;
    bool _started = false;
    bool _paused = false;
    bool _allDone =false;

    sf::RenderWindow& _window;
    sf::Font& _font;
    Scheduler& _scheduler;

    sf::FloatRect _startBtn;
    sf::FloatRect _pauseBtn;
    sf::FloatRect _stopBtn;
    sf::FloatRect _addBtn;

    sf::FloatRect _threadMinusBtn;
    sf::FloatRect _threadPlusBtn;
    int _pendingThreadDelta = 0;

    struct Btn {
        std::string label;
        sf::Color color;
        sf::FloatRect* rect;
    };

    std::vector<Btn> getButtons() {
        bool noTasks = _taskCount == 0;
        return {
            {
                "Start",
                (_started || noTasks) ? sf::Color(40, 80, 40) : sf::Color(0, 160, 80),
                &_startBtn
            },
            {
                _paused ? "Resume" : "Pause",
                (!_started || noTasks) ? sf::Color(60, 60, 60) : sf::Color(180, 140, 0),
                &_pauseBtn
            },
            {
                _allDone ? "Restart" : "Clear",
                (!_started || noTasks) ? sf::Color(60, 60, 60) : sf::Color(180, 40, 40),
                &_stopBtn
            },
            {
                "Add Task",
                sf::Color(60, 60, 160),
                &_addBtn
            },
        };
    }

};
