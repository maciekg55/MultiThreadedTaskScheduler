#include "Engine.h"
#include <iostream>
#include <ComputationTask.h>
#include <RenderTask.h>
#include <IOTask.h>
#include <FileCheckTask.h>

Engine::Engine(bool runTest)
    :
    _window(sf::VideoMode({1280, 800}), "window",sf::Style::Default),
    _renderer(_window, _scheduler)
{
    _scheduler.reset();
    if (runTest) test();

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
            sf::Vector2f mouse = {
                (float)sf::Mouse::getPosition(_window).x,
                (float)sf::Mouse::getPosition(_window).y
            };

            bool noTasks = _scheduler.getTasks().empty();

            if (_renderer.getStartButtonRect().contains(mouse) && !_started && !noTasks) {
                _renderer.resetStartTime();
                _scheduler.start();
                _started = true;
                _paused = false;
            }

            if (_renderer.getPauseButtonRect().contains(mouse) && _started && !noTasks) {
                if (!_paused) { _scheduler.pause(); _paused = true; }
                else { _scheduler.resume(); _paused = false; }
            }

            if (_renderer.getStopButtonRect().contains(mouse) && _started && !noTasks) {
                _scheduler.stop();
                _started = false;
                _paused  = false;
                _allDone = false;
                _scheduler.reset();
                _renderer.resetStartTime();
            }

            _renderer.handleClick(mouse);
        }

        if (const auto* scroll = event->getIf<sf::Event::MouseWheelScrolled>()) {
            sf::Vector2f mouse = {
                (float)sf::Mouse::getPosition(_window).x,
                (float)sf::Mouse::getPosition(_window).y
            };
            _renderer.handleScroll(scroll->delta, mouse);
        }

        if (const auto* text = event->getIf<sf::Event::TextEntered>()) {
            _renderer.handleTextInput(text->unicode);
        }
    }
}

void Engine::update() {
    // calculate allDone
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

    // add/edit/delete tasks
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
        if (existing.count(depId))
            validDeps.push_back(depId);
    }
    task->setDependencies(validDeps);
}


void Engine::render() {
    _window.clear(sf::Color(18, 18, 28));
    _renderer.setStatsState(_started, _paused, _allDone,
                        (int)_scheduler.getTasks().size());
    _renderer.draw();
    _window.display();
}


void Engine::test() {
    // --- Chain A: High priority pipeline ---
    // T1 → T2 → T5 (SaveResults depends on both chains)
    _scheduler.addTask(std::make_shared<ComputationTask>(1, "DataLoad", std::vector<int>{}));
    _scheduler.addTask(std::make_shared<ComputationTask>(2, "DataProcess", std::vector<int>{1}));

    // --- Chain B: Normal priority pipeline ---
    // T3 → T4 → T5
    _scheduler.addTask(std::make_shared<FileCheckTask>(3, "FileCheck", std::vector<int>{}));
    _scheduler.addTask(std::make_shared<IOTask>(4, "Validate", std::vector<int>{3}));

    // --- Convergence point: depends on both chains ---
    // T5 waits for T2 and T4 — tests multi-dependency
    _scheduler.addTask(std::make_shared<RenderTask>(5, "SaveResults", std::vector<int>{2, 4}));

    // --- High priority independent task ---
    // T6 has no deps and high priority — should run first
    auto t6 = std::make_shared<ComputationTask>(6, "DataBackup", std::vector<int>{});
    t6->setPriority(3);
    _scheduler.addTask(t6);

    // --- Low priority cleanup — runs last ---
    // T7 depends on T5 and is low priority
    auto t7 = std::make_shared<FileCheckTask>(7, "Cleanup", std::vector<int>{5});
    t7->setPriority(1);
    _scheduler.addTask(t7);

    // --- Another high priority with no deps ---
    // T8 competes with T6 for first execution slot
    auto t8 = std::make_shared<IOTask>(8, "ReportGen", std::vector<int>{});
    t8->setPriority(3);
    _scheduler.addTask(t8);
}
