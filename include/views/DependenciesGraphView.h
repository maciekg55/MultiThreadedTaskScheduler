#pragma once
#include <SFML/Graphics.hpp>
#include "../Scheduler.h"

/**
 * @brief Directed acyclic graph view showing task dependency relationships.
 *
 * Tasks are arranged in columns by dependency depth. Arrows flow left to right
 * from a dependency to its dependent. The selected task is highlighted with a
 * white outline.
 */
class DependenciesGraphView {
public:
    /// @brief Construct the dependency graph view bound to the given window, font, and scheduler.
    DependenciesGraphView(sf::RenderWindow& window, sf::Font& font, Scheduler& scheduler);

    /// @brief Draw the dependency graph panel to the window.
    void draw();

    /// @brief Highlight the task with the given ID, or pass -1 to clear the selection.
    /// @param id Task ID to select.
    void setSelectedTask(int id) { _selectedTaskId = id; }

    /// @brief Return click-test rectangles for each rendered task node, keyed by task ID.
    const std::unordered_map<int, sf::FloatRect>& getNodeRects() const { return _nodeRects; }

private:
    sf::RenderWindow& _window;
    sf::Font&         _font;
    Scheduler&        _scheduler;

    std::unordered_map<int, sf::FloatRect> _nodeRects;
    int _selectedTaskId = -1;

};
