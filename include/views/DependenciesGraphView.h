#pragma once
#include <SFML/Graphics.hpp>
#include "../Scheduler.h"

class DependenciesGraphView {
public:
    DependenciesGraphView(sf::RenderWindow& window, sf::Font& font, Scheduler& scheduler);
    void draw();

    void setSelectedTask(int id) { _selectedTaskId = id; }
    const std::unordered_map<int, sf::FloatRect>& getNodeRects() const { return _nodeRects; }

private:
    sf::RenderWindow& _window;
    sf::Font&         _font;
    Scheduler&        _scheduler;

    std::unordered_map<int, sf::FloatRect> _nodeRects;
    int _selectedTaskId = -1;

};