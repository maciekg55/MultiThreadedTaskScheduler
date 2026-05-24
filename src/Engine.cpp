#include "Engine.h"
#include <iostream>
#include <tasks/ComputationTask.h>
#include <tasks/RenderTask.h>
#include <tasks/IOTask.h>
#include <tasks/FileCheckTask.h>

Engine::Engine(bool runTest)
    :
    _window(sf::VideoMode({1280, 800}), "window",sf::Style::Default),
    _renderer(_window, _scheduler)
{

    auto presets = PresetLoader::listPresets("presets");
    _renderer.setPresets(presets);

    if (!presets.empty())
        loadPreset("presets/" + presets[0] + ".preset");



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
        if (event->is<sf::Event::Closed>()) _window.close();

        if (event->is<sf::Event::Resized>()) {
            sf::FloatRect view({0, 0}, {
                static_cast<float>(_window.getSize().x),
                static_cast<float>(_window.getSize().y)
            });
            _window.setView(sf::View(view));
        }

        if (event->is<sf::Event::MouseButtonPressed>()) {
            const sf::Vector2f mousePos = {
                static_cast<float>(sf::Mouse::getPosition(_window).x),
                static_cast<float>(sf::Mouse::getPosition(_window).y)
            };

            const bool noTasks = _scheduler.getTasks().empty();

            if (_renderer.getStartButtonRect().contains(mousePos) && !_started && !noTasks) {
                _renderer.resetStartTime();
                _scheduler.start();
                _started = true;
                _paused = false;
            }

            if (_renderer.getPauseButtonRect().contains(mousePos) && _started && !noTasks) {
                if (!_paused) { _scheduler.pause(); _paused = true; }
                else { _scheduler.resume(); _paused = false; }
            }

            if (_renderer.getStopButtonRect().contains(mousePos) && _started && !noTasks) {
                _scheduler.stop();
                _started = false;
                _paused = false;
                _allDone = false;
                _scheduler.reset();
                _renderer.loadNextTaskId(-1);
                _renderer.resetStartTime();
            }

            _renderer.handleClick(mousePos);
        }

        if (const auto* scroll = event->getIf<sf::Event::MouseWheelScrolled>()) {
            const sf::Vector2f mousePos = {
                static_cast<float>(sf::Mouse::getPosition(_window).x),
                static_cast<float>(sf::Mouse::getPosition(_window).y)
            };
            _renderer.handleScroll(scroll->delta, mousePos);
        }

        if (const auto* text = event->getIf<sf::Event::TextEntered>()) {
            _renderer.handleTextInput(text->unicode);
        }
    }
}

void Engine::update() {

    if (_started) {
        _allDone = true;
        for (const auto& [id, task] : _scheduler.getTasks()) {
            if (task->getStatus() != Task::TaskStatus::Completed &&
                task->getStatus() != Task::TaskStatus::Cancelled) {
                _allDone = false;
                break;
                }
        }
    } else {
        _allDone = false;
    }

    int delta = _renderer.consumeThreadDelta();
    if (delta != 0) {
        unsigned int current = _scheduler.getThreadCount();
        unsigned int maxT = std::max(1u, std::thread::hardware_concurrency() - 1);
        unsigned int newCount = static_cast<unsigned int>(
            std::clamp(static_cast<int>(current) + delta, 1, static_cast<int>(maxT))
        );
        _scheduler.setThreadCount(newCount);
    }

    if (_renderer.hasPresetSelection() && !_started) {
        if (const std::string name = _renderer.consumePresetSelection(); name == "none") {
            _scheduler.reset();
            _renderer.loadNextTaskId(-1);
        } else {
            loadPreset("presets/" + name + ".preset");
        }
    }

    if (_renderer.hasPendingAdd()) {
        auto task = _renderer.consumeTask();
        validateDeps(task);
        _scheduler.addTask(task);
    }

    if (_renderer.hasPendingEdit()) {
        int oldId = _renderer.getEditTaskId();
        auto task = _renderer.consumeTask();
        validateDeps(task);
        _scheduler.removeTask(oldId);
        _scheduler.addTask(task);
    }

    int deleteId = _renderer.hasPendingDelete();
    if (deleteId != -1)
        _scheduler.removeTask(deleteId);
}

void Engine::validateDeps(std::shared_ptr<Task>& task) {
    const auto& existing = _scheduler.getTasks();
    std::vector<int> validDeps;
    for (int depId : task->getDependencies()) {
        if (depId == task->getId()) continue;
        if (existing.count(depId))
            validDeps.push_back(depId);
    }
    task->setDependencies(validDeps);
}


void Engine::render() {
    _window.clear(sf::Color(18, 18, 28));
    _renderer.setStatsState(_started, _paused, _allDone,
                        static_cast<int>(_scheduler.getTasks().size()));
    _renderer.draw();
    _window.display();
}



void Engine::loadPreset(const std::string& filepath) {
    _scheduler.reset();
    auto presets = PresetLoader::load(filepath);

    int maxId = 0;
    for (const auto& preset : presets) {
        auto task = PresetLoader::createTask(preset);
        validateDeps(task);
        _scheduler.addTask(task);
        maxId = std::max(maxId, preset.id);
    }
    _renderer.loadNextTaskId(maxId);
}
