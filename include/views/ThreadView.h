#pragma once
#include <SFML/Graphics.hpp>
#include "../Scheduler.h"

/**
 * @brief Left sidebar displaying the state of each worker thread.
 *
 * Shows which task (if any) each thread is executing, with status-coded indicators.
 */
class ThreadView {
public:
    /// @brief Construct the thread view bound to the given window, font, and scheduler.
    ThreadView(sf::RenderWindow& window, sf::Font& font, Scheduler& scheduler);

    /// @brief Draw the thread list panel to the window.
    void draw();

    /// @brief Scroll the thread list in response to a mouse wheel event.
    /// @param delta    Scroll direction and magnitude.
    /// @param mousePos Cursor position; scroll is ignored if outside this panel.
    void handleScroll(float delta, sf::Vector2f mousePos);

private:
    sf::RenderWindow& _window;
    sf::Font&         _font;
    Scheduler&        _scheduler;

    float _scrollOffset = 0.f;
    float _contentHeight = 0.f;
};
