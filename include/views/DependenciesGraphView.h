#pragma once
#include <SFML/Graphics.hpp>
#include "../Scheduler.h"

class DependenciesGraphView {
public:
    DependenciesGraphView(sf::RenderWindow& window, sf::Font& font, Scheduler& scheduler);
    void draw();

private:
    sf::RenderWindow& _window;
    sf::Font&         _font;
    Scheduler&        _scheduler;
};