#pragma once
#include <SFML/Graphics.hpp>
#include "../Scheduler.h"

class TaskDetailsView {
public:
    TaskDetailsView(sf::RenderWindow& window, sf::Font& font, Scheduler& scheduler);
    void draw();

    void setSelectedTask(int id) { _selectedTaskId = id; }
    int  getSelectedTask() const { return _selectedTaskId; }

    sf::FloatRect getDeleteBtn() const { return _deleteBtn; }
    sf::FloatRect getEditBtn()   const { return _editBtn;   }

private:
    sf::RenderWindow& _window;
    sf::Font&         _font;
    Scheduler&        _scheduler;

    int           _selectedTaskId = -1;
    sf::FloatRect _deleteBtn;
    sf::FloatRect _editBtn;
};