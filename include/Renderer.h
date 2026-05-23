#pragma once
#include <SFML/Graphics.hpp>
#include "Scheduler.h"

class Renderer {
private:
    std::chrono::steady_clock::time_point _appStartTime;

    sf::RenderWindow& _window;
    Scheduler& _scheduler;
    sf::Font _font;
    sf::FloatRect _startBtn, _pauseBtn, _stopBtn;

    float _queueScrollOffset = 0.f;
    float _queueContentHeight = 0.f;

    float _threadScrollOffset  = 0.f;
    float _threadContentHeight = 0.f;

    bool _pendingTask = false;

    int _pendingDelete = -1;
    int _editTaskId    = -1;

    std::unordered_map<int, sf::FloatRect> _taskRowRects;

    int _selectedTaskId = -1;
    sf::FloatRect _deleteBtn;
    sf::FloatRect _editBtn;
    bool _editMode = false;


public:

    Renderer(sf::RenderWindow& window, Scheduler& scheduler);
    void draw();

    sf::FloatRect getStartButtonRect() const { return _startBtn; }
    sf::FloatRect getPauseButtonRect() const { return _pauseBtn; }
    sf::FloatRect getStopButtonRect()  const { return _stopBtn;  }

    void resetStartTime() { _appStartTime = std::chrono::steady_clock::now(); }
    void handleScroll(float delta, sf::Vector2f mousePosition);

    void handleTextInput(uint32_t unicode);
    void handleFormClick(sf::Vector2f mousePos);
    bool wantsAddTask();                          // Engine polls this
    std::shared_ptr<Task> getNewTask();

    int  wantsDelete() { int id = _pendingDelete; _pendingDelete = -1; return id; }
    bool wantsEdit()   { return _editMode && _pendingTask; }
    int  getEditTaskId() { return _editTaskId; }


private:

    void drawThreadPanel();
    void drawGanttPanel();
    void drawDependencyGraph();
    void drawTaskQueuePanel();
    void drawTaskDetailsPanel();
    void drawStatsBar();
    void drawAddTaskForm();
    void drawAddTaskButton();

    sf::Color getStatusColor(Task::TaskStatus status);

    struct TextInput {
        std::string value;
        bool focused = false;
        sf::FloatRect bounds;

        void handleChar(uint32_t unicode) {
            if (!focused) return;
            if (unicode == 8 && !value.empty())  // backspace
                value.pop_back();
            else if (unicode >= 32 && unicode < 127)  // printable
                value += static_cast<char>(unicode);
        }
    };

    struct AddTaskForm {
        bool visible = false;

        TextInput nameInput;
        TextInput depsInput;
        TextInput estTimeInput;

        int selectedPriority = 2;   // 1=Low 2=Medium 3=High
        int selectedType     = 0;   // 0=Computation 1=IO 2=FileCheck 3=Render

        sf::FloatRect confirmBtn;
        sf::FloatRect cancelBtn;
        sf::FloatRect addTaskBtn;   // the button that opens the form
    };

    AddTaskForm _addTaskForm;
    int _nextTaskId = 0;


};