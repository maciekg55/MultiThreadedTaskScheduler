#pragma once
#include "Renderer.h"

class Engine {
private:
    sf::RenderWindow _window;
    Scheduler _scheduler;
    Renderer _renderer;

    bool _started = false;
    bool _paused  = false;
public:
    Engine();
    void run();

private:
    void handleEvents();
    void update();
    void render();
    void resetTasks();
    void validateDeps(std::shared_ptr<Task>& task);

};