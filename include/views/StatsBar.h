#pragma once
#include <SFML/Graphics.hpp>
#include "../Scheduler.h"

class StatsBar {
public:
    StatsBar(sf::RenderWindow& window, sf::Font& font, Scheduler& scheduler);
    void draw();

    sf::FloatRect getStartBtn() const {return _startBtn;}
    sf::FloatRect getPauseBtn() const {return _pauseBtn;}
    sf::FloatRect getStopBtn() const {return _stopBtn;}
    sf::FloatRect getAddBtn() const {return _addBtn;}

    void setState(bool started, bool paused, bool allDone, int taskCount) {
        _started = started;
        _paused  = paused;
        _allDone = allDone;
        _taskCount = taskCount;
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