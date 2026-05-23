#pragma once
#include <SFML/Graphics.hpp>
#include "../Scheduler.h"
#include <chrono>

class GanttGraphView {
public:
    GanttGraphView(sf::RenderWindow& window, sf::Font& font, Scheduler& scheduler);
    void draw();
    void resetStartTime() { _appStartTime = std::chrono::steady_clock::now(); }

private:
    sf::RenderWindow& _window;
    sf::Font&         _font;
    Scheduler&        _scheduler;

    std::chrono::steady_clock::time_point _appStartTime;
};