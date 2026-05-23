#pragma once
#include <SFML/Graphics.hpp>
#include "Task.h"
#include <string>

namespace UI {
    sf::Color   statusColor    (Task::TaskStatus status);
    std::string priorityStr    (int priority);
    sf::Color   priorityColor  (int priority);
}