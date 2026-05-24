#include "UIHelpers.h"

sf::Color UI::statusColor(Task::TaskStatus status) {
    switch (status) {
        case Task::TaskStatus::Running:   return sf::Color(0, 200, 100);
        case Task::TaskStatus::Queued:    return sf::Color(180, 140, 0);
        case Task::TaskStatus::Completed: return sf::Color(45, 55, 72);   // dark slate
        case Task::TaskStatus::Cancelled: return sf::Color(255, 60, 60);
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

sf::Color UI::priorityBgColor(int p) {
    return p >= 3 ? sf::Color(80, 25, 25)  :   // dark red for high
           p == 2 ? sf::Color(25, 40, 80)  :   // dark blue for medium
                    sf::Color(30, 30, 30);      // dark gray for low
}