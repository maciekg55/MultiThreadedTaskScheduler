#pragma once
#include "Renderer.h"
#include "PresetLoader.h"

/**
 * @brief Top-level application loop that ties together the window, scheduler, and renderer.
 *
 * Owns the SFML window, the Scheduler, and the Renderer. Processes events,
 * drives the update/render cycle, and translates UI actions into scheduler calls.
 */
class Engine {
private:
    sf::RenderWindow _window;
    Scheduler _scheduler;
    Renderer _renderer;

    bool _started = false;
    bool _paused = false;
    bool _allDone = false;
public:
    /// @brief Initialize the window, load the first available preset, and set up the renderer.
    /// @param runTest Reserved for future test mode; currently unused.
    Engine(bool runTest);

    /// @brief Enter the main loop, blocking until the window is closed.
    void run();

private:
    /// @brief Poll SFML events and translate them into scheduler and renderer actions.
    void handleEvents();

    /// @brief Advance application state: detect completion, thread changes, task CRUD.
    void update();

    /// @brief Clear the window, update view state, and present the frame.
    void render();

    /// @brief Strip dependency IDs that do not exist in the scheduler or create a self-reference.
    /// @param task Task whose dependency list will be filtered in place.
    void validateDeps(std::shared_ptr<Task>& task);

    /// @brief Placeholder for automated test scenarios.
    void test();

    /// @brief Load a preset file, replacing all current tasks.
    /// @param filepath Path to the .preset file.
    void loadPreset(const std::string& filepath);
};
