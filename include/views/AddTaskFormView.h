#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include <sstream>
#include "Task.h"

class AddTaskFormView {
public:
    AddTaskFormView(sf::RenderWindow& window, sf::Font& font);

    void draw();
    void handleClick(sf::Vector2f mouse);
    void handleTextInput(uint32_t unicode);

    void openForAdd();
    void openForEdit(const std::shared_ptr<Task>& task);

    bool isVisible() const { return _visible; }
    bool hasPendingTask() const { return _pendingTask; }
    bool isEditMode() const { return _editMode; }
    int getEditTaskId() const { return _editTaskId; }

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

    struct TextInput {
        std::string value;
        bool focused = false;
        sf::FloatRect bounds;

        void handleChar(uint32_t unicode) {
            if (!focused) return;
            if (unicode == 8 && !value.empty()) value.pop_back();
            else if (unicode >= 32 && unicode < 127) value += static_cast<char>(unicode);
        }
        void clear() { value.clear(); focused = false; }
    };


    TextInput _nameInput;
    TextInput _depsInput;
    TextInput _estTimeInput;

    sf::FloatRect _confirmBtn;
    sf::FloatRect _cancelBtn;

    void clearForm();
    void drawInput(const std::string& label, TextInput& input, float fieldX,
                   float fieldW, float fieldH, float labelSize, float y);
};