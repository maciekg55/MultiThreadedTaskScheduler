#pragma once
#include <SFML/Graphics.hpp>
#include "../Scheduler.h"
#include <unordered_map>

class TaskQueueView {
public:
    TaskQueueView(sf::RenderWindow& window, sf::Font& font, Scheduler& scheduler);
    void draw();
    void handleScroll(float delta, sf::Vector2f mousePos);

    // returns the task ID that was clicked, -1 if none
    const std::unordered_map<int, sf::FloatRect>& getTaskRowRects() const { return _taskRowRects; }

private:
    sf::RenderWindow& _window;
    sf::Font&         _font;
    Scheduler&        _scheduler;

    float _scrollOffset  = 0.f;
    float _contentHeight = 0.f;

    std::unordered_map<int, sf::FloatRect> _taskRowRects;
};