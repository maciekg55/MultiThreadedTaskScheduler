#include "UIHelpers.h"

sf::Color UI::statusColor(Task::TaskStatus status) {
    switch (status) {
        case Task::TaskStatus::Running:   return sf::Color(0,   200, 100);
        case Task::TaskStatus::Completed: return sf::Color(100, 100, 255);
        case Task::TaskStatus::Cancelled: return sf::Color(255, 60,  60);
        case Task::TaskStatus::Planned:   return sf::Color(120, 120, 120);
        default:                          return sf::Color::White;
    }
}

std::string UI::priorityStr(int p) {
    return p >= 3 ? "High" : p == 2 ? "Medium" : "Low";
}

sf::Color UI::priorityColor(int p) {
    return p >= 3 ? sf::Color(255, 80,  80)  :
           p == 2 ? sf::Color(80,  180, 255) :
                    sf::Color(120, 120, 120);
}