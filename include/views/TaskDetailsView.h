#pragma once
#include <SFML/Graphics.hpp>
#include "../Scheduler.h"

/**
 * @brief Bottom-right panel showing detailed information about the selected task.
 *
 * Automatically falls back to the first running task when no task is explicitly selected.
 * Exposes Delete and Edit buttons only for tasks in the Planned state.
 */
class TaskDetailsView {
public:
    /// @brief Construct the detail view bound to the given window, font, and scheduler.
    TaskDetailsView(sf::RenderWindow& window, sf::Font& font, Scheduler& scheduler);

    /// @brief Draw the task detail panel to the window.
    void draw();

    /// @brief Set the task to display by ID, or -1 to clear the selection.
    /// @param id Task ID to select.
    void setSelectedTask(int id) { _selectedTaskId = id; }

    /// @brief Return the ID of the currently selected task, or -1 if none.
    int  getSelectedTask() const { return _selectedTaskId; }

    /// @brief Return the bounding rect of the Delete button, or an empty rect if not shown.
    sf::FloatRect getDeleteBtn() const { return _deleteBtn; }

    /// @brief Return the bounding rect of the Edit button, or an empty rect if not shown.
    sf::FloatRect getEditBtn()   const { return _editBtn;   }

private:
    sf::RenderWindow& _window;
    sf::Font&         _font;
    Scheduler&        _scheduler;

    int           _selectedTaskId = -1;
    sf::FloatRect _deleteBtn;
    sf::FloatRect _editBtn;
};
