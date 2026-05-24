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
                _renderer.loadNextTaskId(-1);
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

    if (_renderer.hasPresetSelection() && !_started) {
        std::string name = _renderer.consumePresetSelection();
        if (name == "none") {
            _scheduler.reset();
            _renderer.loadNextTaskId(-1);
        } else {
            loadPreset("presets/" + name + ".preset");
        }
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
        if (depId == task->getId()) continue;  // skip self
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
    // === HIGH PRIORITY (5 tasks) ===
    // these should jump the queue and run first
    auto t1 = std::make_shared<ComputationTask>(1, "CriticalCalc", std::vector<int>{});
    t1->setPriority(3);
    _scheduler.addTask(t1);

    auto t2 = std::make_shared<IOTask>(2, "UrgentFetch", std::vector<int>{});
    t2->setPriority(3);
    _scheduler.addTask(t2);

    auto t3 = std::make_shared<FileCheckTask>(3, "SecurityScan", std::vector<int>{});
    t3->setPriority(3);
    _scheduler.addTask(t3);

    auto t4 = std::make_shared<RenderTask>(4, "PriorityRender", std::vector<int>{3});
    t4->setPriority(3);
    _scheduler.addTask(t4);

    auto t5 = std::make_shared<ComputationTask>(5, "CriticalProc", std::vector<int>{1});
    t5->setPriority(3);
    _scheduler.addTask(t5);

    // === MEDIUM PRIORITY (10 tasks) ===
    // mix of independent and chained
    auto t6 = std::make_shared<ComputationTask>(6, "DataProcess1", std::vector<int>{});
    _scheduler.addTask(t6);

    auto t7 = std::make_shared<IOTask>(7, "DataFetch1", std::vector<int>{});
    _scheduler.addTask(t7);

    auto t8 = std::make_shared<ComputationTask>(8, "DataProcess2", std::vector<int>{7});
    _scheduler.addTask(t8);

    auto t9 = std::make_shared<FileCheckTask>(9, "FileCheck1", std::vector<int>{});
    _scheduler.addTask(t9);

    auto t10 = std::make_shared<IOTask>(10, "DataFetch2", std::vector<int>{9});
    _scheduler.addTask(t10);

    auto t11 = std::make_shared<RenderTask>(11, "Render1", std::vector<int>{8, 10});
    _scheduler.addTask(t11);

    auto t12 = std::make_shared<ComputationTask>(12, "DataProcess3", std::vector<int>{});
    _scheduler.addTask(t12);

    auto t13 = std::make_shared<IOTask>(13, "DataFetch3", std::vector<int>{12});
    _scheduler.addTask(t13);

    auto t14 = std::make_shared<FileCheckTask>(14, "FileCheck2", std::vector<int>{});
    _scheduler.addTask(t14);

    auto t15 = std::make_shared<RenderTask>(15, "Render2", std::vector<int>{13, 14});
    _scheduler.addTask(t15);

    // === LOW PRIORITY (5 tasks) ===
    // cleanup and logging — should run last
    auto t16 = std::make_shared<FileCheckTask>(16, "Cleanup1", std::vector<int>{11});
    t16->setPriority(1);
    _scheduler.addTask(t16);

    auto t17 = std::make_shared<FileCheckTask>(17, "Cleanup2", std::vector<int>{15});
    t17->setPriority(1);
    _scheduler.addTask(t17);

    auto t18 = std::make_shared<ComputationTask>(18, "LogProcess", std::vector<int>{});
    t18->setPriority(1);
    _scheduler.addTask(t18);

    auto t19 = std::make_shared<IOTask>(19, "LogWrite", std::vector<int>{18});
    t19->setPriority(1);
    _scheduler.addTask(t19);

    auto t20 = std::make_shared<FileCheckTask>(20, "FinalCheck", std::vector<int>{16, 17, 19});
    t20->setPriority(1);
    _scheduler.addTask(t20);

    int maxId = 0;
    for (const auto& [id, task] : _scheduler.getTasks()) {
        maxId = std::max(maxId, id);
    }
    _renderer.loadNextTaskId(maxId);
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
