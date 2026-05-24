#pragma once
#include <SFML/Graphics.hpp>
#include "Task.h"
#include <string>

/// @brief UI utility functions for consistent color and label rendering across views.
namespace UI {
    /// @brief Return the display color associated with a task status.
    /// @param status The task lifecycle status.
    /// @return SFML color for that status.
    sf::Color statusColor(Task::TaskStatus status);

    /// @brief Return a human-readable label for a priority level.
    /// @param priority Priority value (1=Low, 2=Medium, 3+=High).
    /// @return String label.
    std::string priorityStr(int priority);

    /// @brief Return the foreground color for a priority level.
    /// @param priority Priority value.
    /// @return SFML foreground color.
    sf::Color priorityColor(int priority);

    /// @brief Return the background color for a priority level.
    /// @param priority Priority value.
    /// @return SFML background color.
    sf::Color priorityBgColor(int priority);

}
