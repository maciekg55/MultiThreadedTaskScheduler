#include "views/ThreadView.h"
#include "UIHelpers.h"

ThreadView::ThreadView(sf::RenderWindow& window, sf::Font& font, Scheduler& scheduler)
    : _window(window), _font(font), _scheduler(scheduler) {}

void ThreadView::draw() {
    float W = _window.getSize().x;
    float H = _window.getSize().y;

    float panelW  = W * 0.15f;
    float clipTop = H * 0.07f;
    float clipBot = H - 2.f;
    float rowH = H * 0.07f;
    float startY = H * 0.07f;
    float boxR = H * 0.014f;

    sf::RectangleShape bg({panelW, H});
    bg.setFillColor(sf::Color(25, 25, 38));
    bg.setPosition({0, 0});
    _window.draw(bg);

    sf::Text title(_font, "Available Threads");
    title.setCharacterSize(H * 0.018f);
    title.setFillColor(sf::Color(180, 180, 180));
    title.setPosition({panelW * 0.08f, H * 0.02f});
    _window.draw(title);

    sf::RectangleShape divider({panelW - 2, 1});
    divider.setFillColor(sf::Color(60, 60, 80));
    divider.setPosition({1, H * 0.055f});
    _window.draw(divider);

    const auto& workers = _scheduler.getWorkers();

    for (int i = 0; i < (int)workers.size(); i++) {
        const auto& worker = workers[i];
        float rowY = startY + i * rowH - _scrollOffset;

        if (rowY + rowH < clipTop || rowY > clipBot) continue;

        if (worker.currentTask) {
            sf::RectangleShape highlight({panelW - 2, rowH - 2});
            highlight.setFillColor(sf::Color(40, 40, 60));
            highlight.setPosition({1, rowY});
            _window.draw(highlight);
        }

        Task::TaskStatus status = worker.currentTask
            ? Task::TaskStatus::Running
            : Task::TaskStatus::Planned;

        sf::RectangleShape box({boxR, boxR});
        box.setFillColor(UI::statusColor(status));
        box.setPosition({panelW * 0.08f, rowY + rowH * 0.5f - boxR});
        _window.draw(box);

        sf::Text threadName(_font, "Thread " + std::to_string(i));
        threadName.setCharacterSize(H * 0.016f);
        threadName.setFillColor(sf::Color::White);
        threadName.setPosition({panelW * 0.22f, rowY + rowH * 0.15f});
        _window.draw(threadName);

        sf::Text taskLabel(_font, worker.currentTask
            ? worker.currentTask->getName()
            : "Idle");
        taskLabel.setCharacterSize(H * 0.013f);
        taskLabel.setFillColor(worker.currentTask
            ? sf::Color(180, 255, 180)
            : sf::Color(100, 100, 100));
        taskLabel.setPosition({panelW * 0.22f, rowY + rowH * 0.52f});
        _window.draw(taskLabel);

        sf::RectangleShape rowDiv({panelW - 2, 1});
        rowDiv.setFillColor(sf::Color(45, 45, 65));
        rowDiv.setPosition({1, rowY + rowH - 1});
        _window.draw(rowDiv);
    }

    _contentHeight = workers.size() * rowH;
}

void ThreadView::handleScroll(float delta, sf::Vector2f mousePos) {
    float W = _window.getSize().x;
    float H = _window.getSize().y;

    sf::FloatRect bounds({0, 0}, {W * 0.15f, H});
    if (!bounds.contains(mousePos)) return;

    _scrollOffset -= delta * 20.f;
    float maxScroll = std::max(0.f, _contentHeight - H * 0.85f);
    _scrollOffset = std::clamp(_scrollOffset, 0.f, maxScroll);
}