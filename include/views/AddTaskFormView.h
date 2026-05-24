#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include <sstream>
#include "Task.h"

/**
 * @brief Modal overlay for creating a new task or editing an existing one.
 *
 * Renders a centered form with name, dependencies, priority, and type fields.
 * When confirmed, the result is retrievable via consumeTask().
 */
class AddTaskFormView {
public:
    /// @brief Construct the form bound to the given window and font.
    AddTaskFormView(sf::RenderWindow& window, sf::Font& font);

    /// @brief Draw the form overlay if visible.
    void draw();

    /// @brief Route a mouse click to form controls.
    /// @param mouse Cursor position in window coordinates.
    void handleClick(sf::Vector2f mouse);

    /// @brief Route a text character to the focused text input.
    /// @param unicode Unicode code point of the entered character.
    void handleTextInput(uint32_t unicode);

    /// @brief Open the form in add mode with blank fields.
    void openForAdd();

    /// @brief Open the form in edit mode pre-populated with an existing task's data.
    /// @param task Task whose fields will be loaded into the form.
    void openForEdit(const std::shared_ptr<Task>& task);

    /// @brief Return true if the form is currently displayed.
    bool isVisible()      const { return _visible; }

    /// @brief Return true if a completed task is waiting to be consumed.
    bool hasPendingTask() const { return _pendingTask; }

    /// @brief Return true if the form is in edit rather than add mode.
    bool isEditMode()     const { return _editMode; }

    /// @brief Return the ID of the task being edited, or -1 in add mode.
    int  getEditTaskId()  const { return _editTaskId; }

    /// @brief Retrieve and clear the pending task, assigning or reusing an ID.
    /// @param nextId Auto-increment counter; incremented only in add mode.
    /// @return The newly created or edited task.
    std::shared_ptr<Task> consumeTask(int& nextId);

private:
    sf::RenderWindow& _window;
    sf::Font& _font;

    bool _visible = false;
    bool _pendingTask = false;
    bool _editMode = false;
    int _editTaskId = -1;
    int _selectedPriority = 2;
    int _selectedType = 0;

    /// @brief Single-line text field with focus tracking and keyboard editing.
    struct TextInput {
        std::string value;
        bool focused = false;
        sf::FloatRect bounds;

        /// @brief Process a key code: backspace deletes the last character, printable chars are appended.
        /// @param unicode Unicode code point.
        void handleChar(uint32_t unicode) {
            if (!focused) return;
            if (unicode == 8 && !value.empty()) value.pop_back();
            else if (unicode >= 32 && unicode < 127) value += static_cast<char>(unicode);
        }

        /// @brief Clear the value and remove focus.
        void clear() { value.clear(); focused = false; }
    };


    TextInput _nameInput;
    TextInput _depsInput;
    TextInput _estTimeInput;

    sf::FloatRect _confirmBtn;
    sf::FloatRect _cancelBtn;

    /// @brief Reset all form fields to defaults.
    void clearForm();

    /// @brief Draw a labeled text input field.
    /// @param label     Field label shown above the input box.
    /// @param input     TextInput state to render.
    /// @param fieldX    Left edge of the input box.
    /// @param fieldW    Width of the input box.
    /// @param fieldH    Height of the input box.
    /// @param labelSize Font size for label and value text.
    /// @param y         Top edge of the label.
    void drawInput(const std::string& label, TextInput& input, float fieldX,
                   float fieldW, float fieldH, float labelSize, float y);
};
