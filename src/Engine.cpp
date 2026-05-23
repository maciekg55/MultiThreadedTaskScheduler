#include "Engine.h"
#include <iostream>

Engine::Engine()
    :
    _window(sf::VideoMode({1280, 800}), "window",sf::Style::Default),
    _renderer(_window, _scheduler)
{
    resetTasks();

}

void Engine::run() {
    try {
        while (_window.isOpen()) {
            handleEvents();
            update();
            render();
        }
    } catch (const std::exception& e) {
        std::cerr << "CRASH: " << e.what() << std::endl;
    } catch (...) {
        std::cerr << "CRASH: unknown exception" << std::endl;
    }
}

void Engine::handleEvents() {
    while (const auto event = _window.pollEvent()) {
        if (event -> is<sf::Event::Closed>() ) _window.close();

        if (event->is<sf::Event::Resized>()) {
            sf::FloatRect view({0, 0}, {static_cast<float>(_window.getSize().x), static_cast<float>(_window.getSize().y)});
            _window.setView(sf::View(view));
        }

        if (event->is<sf::Event::MouseButtonPressed>()) {
            sf::Vector2f mousePosition = {sf::Mouse::getPosition(_window).x *1.f, sf::Mouse::getPosition(_window).y * 1.f};
            _renderer.handleFormClick(mousePosition);
            if (_renderer.getStartButtonRect().contains(mousePosition)) {
                if (!_started) {
                    _renderer.resetStartTime();
                    _scheduler.start();
                    _started = true;
                    _paused  = false;
                }
            }

            if (_renderer.getPauseButtonRect().contains(mousePosition)) {
                if (_started && !_paused) {
                    _scheduler.pause();
                    _paused = true;
                } else if (_started && _paused) {
                    _scheduler.resume();
                    _paused = false;
                }
            }

            if (_renderer.getStopButtonRect().contains(mousePosition)) {
                if (_started) {
                    _scheduler.stop();
                    _started = false;
                    _paused  = false;
                    // reset tasks for next run
                    _scheduler.clearTasks();
                    resetTasks();
                }
            }


        }

        if (const auto* scroll = event->getIf<sf::Event::MouseWheelScrolled>()) {
            sf::Vector2f mousePos = {
                (float)sf::Mouse::getPosition(_window).x,
                (float)sf::Mouse::getPosition(_window).y
            };
            _renderer.handleScroll(scroll->delta, mousePos);
        }

        if (const auto* text = event->getIf<sf::Event::TextEntered>()) {
            _renderer.handleTextInput(text->unicode);
        }
    }
}

void Engine::update() {
    if (_renderer.wantsAddTask() && !_renderer.wantsEdit()) {
        auto task = _renderer.getNewTask();
        // validate deps — remove any that don't exist
        validateDeps(task);
        _scheduler.addTask(task);
    }

    if (_renderer.wantsEdit()) {
        int oldId = _renderer.getEditTaskId();
        auto task = _renderer.getNewTask();
        validateDeps(task);
        _scheduler.removeTask(oldId);
        _scheduler.addTask(task);
    }

    int deleteId = _renderer.wantsDelete();
    if (deleteId != -1)
        _scheduler.removeTask(deleteId);
}

void Engine::validateDeps(std::shared_ptr<Task>& task) {
    const auto& existing = _scheduler.getTasks();
    std::vector<int> validDeps;
    for (int depId : task->getDependencies()) {
        if (existing.count(depId))
            validDeps.push_back(depId);
        else
            std::cerr << "Warning: dep T" << depId << " not found, ignored\n";
    }
    task->setDependencies(validDeps);
}


void Engine::render() {
    _window.clear(sf::Color(18, 18, 28));  // dark background
    _renderer.draw();
    _window.display();
}

void Engine::resetTasks() {
   _scheduler.clearTasks();

}