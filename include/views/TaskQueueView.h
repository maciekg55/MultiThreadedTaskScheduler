#pragma once
#include <SFML/Graphics.hpp>
#include "../Scheduler.h"
#include <unordered_map>

/**
 * @brief Right panel listing all tasks grouped by priority.
 *
 * Each row shows task name, estimated time, and a progress bar.
 * Provides hit rectangles for click-to-select interaction.
 */
class TaskQueueView {
public:
    /// @brief Construct the queue view bound to the given window, font, and scheduler.
    TaskQueueView(sf::RenderWindow& window, sf::Font& font, Scheduler& scheduler);

    /// @brief Draw the task queue panel to the window.
    void draw();

    /// @brief Scroll the task list in response to a mouse wheel event.
    /// @param delta    Scroll direction and magnitude.
    /// @param mousePos Cursor position; scroll is ignored if outside this panel.
    void handleScroll(float delta, sf::Vector2f mousePos);

    /// @brief Return click-test rectangles for each visible task row, keyed by task ID.
    const std::unordered_map<int, sf::FloatRect>& getTaskRowRects() const { return _taskRowRects; }

private:
    sf::RenderWindow& _window;
    sf::Font&         _font;
    Scheduler&        _scheduler;

    float _scrollOffset = 0.f;
    float _contentHeight = 0.f;

    std::unordered_map<int, sf::FloatRect> _taskRowRects;
};
