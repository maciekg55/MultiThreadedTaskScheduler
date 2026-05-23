#pragma once
#include <SFML/Graphics.hpp>
#include "../Scheduler.h"

class ThreadView {
public:
    ThreadView(sf::RenderWindow& window, sf::Font& font, Scheduler& scheduler);
    void draw();
    void handleScroll(float delta, sf::Vector2f mousePos);

private:
    sf::RenderWindow& _window;
    sf::Font&         _font;
    Scheduler&        _scheduler;

    float _scrollOffset  = 0.f;
    float _contentHeight = 0.f;
};