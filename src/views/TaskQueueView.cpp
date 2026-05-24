#include "views/TaskQueueView.h"
#include "UIHelpers.h"
#include <algorithm>

TaskQueueView::TaskQueueView(sf::RenderWindow& window, sf::Font& font, Scheduler& scheduler)
    : _window(window), _font(font), _scheduler(scheduler) {}

void TaskQueueView::draw() {
    float W = _window.getSize().x;
    float H = _window.getSize().y;

    float panelX = W * 0.70f;
    float panelW = W * 0.30f;
    float panelH = H * 0.55f;

    sf::RectangleShape bg({panelW, panelH});
    bg.setFillColor(sf::Color(25, 25, 38));
    bg.setPosition({panelX, 0});
    _window.draw(bg);

    sf::Text title(_font, "Task Queue");
    title.setCharacterSize(H * 0.018f);
    title.setFillColor(sf::Color(180, 180, 180));
    title.setPosition({panelX + panelW * 0.06f, H * 0.02f});
    _window.draw(title);

    sf::RectangleShape divider({panelW - 2, 1});
    divider.setFillColor(sf::Color(60, 60, 80));
    divider.setPosition({panelX + 1, H * 0.055f});
    _window.draw(divider);

    const auto& tasks = _scheduler.getTasks();
    std::vector<std::shared_ptr<Task>> high, normal, low;
    for (const auto& [id, task] : tasks) {
        int p = task->getPriority();
        if (p >= 3)      high.push_back(task);
        else if (p == 2) normal.push_back(task);
        else             low.push_back(task);
    }

    float currentY = H * 0.07f - _scrollOffset;
    float contentStartY = H * 0.07f;
    float rowH = H * 0.055f;
    float labelSize = H * 0.014f;
    float clipTop = H * 0.07f;
    float clipBot = panelH - 2.f;

    auto inBounds = [&](float y) {
        return y + rowH > clipTop && y < clipBot;
    };

    _taskRowRects.clear();

    auto drawGroup = [&](const std::string& groupName,
                         const std::vector<std::shared_ptr<Task>>& groupTasks,
                         sf::Color groupColor) {
        if (inBounds(currentY)) {
            sf::RectangleShape headerBg({panelW - 2, rowH * 0.85f});
            headerBg.setFillColor(sf::Color(35, 35, 52));
            headerBg.setPosition({panelX + 1, currentY});
            _window.draw(headerBg);

            sf::RectangleShape accent({3, rowH * 0.85f});
            accent.setFillColor(groupColor);
            accent.setPosition({panelX + 1, currentY});
            _window.draw(accent);

            sf::Text groupLabel(_font, groupName + " (" + std::to_string(groupTasks.size()) + ")");
            groupLabel.setCharacterSize(labelSize);
            groupLabel.setFillColor(groupColor);
            groupLabel.setPosition({panelX + panelW * 0.06f, currentY + rowH * 0.18f});
            _window.draw(groupLabel);
        }
        currentY += rowH;

        for (const auto& task : groupTasks) {
            _taskRowRects[task->getId()] = sf::FloatRect({panelX + 1, currentY}, {panelW - 2, rowH - 1});

            if (inBounds(currentY)) {
                sf::RectangleShape rowBg({panelW - 2, rowH - 1});
                rowBg.setFillColor(sf::Color(30, 30, 45));
                rowBg.setPosition({panelX + 1, currentY});
                _window.draw(rowBg);

                float dotR = H * 0.008f;
                sf::CircleShape dot(dotR);
                dot.setFillColor(UI::statusColor(task->getStatus()));
                dot.setPosition({panelX + panelW * 0.06f, currentY + rowH * 0.5f - dotR});
                _window.draw(dot);

                sf::Text taskName(_font, "ID: " + std::to_string(task->getId()) + " Name: " + task->getName());
                taskName.setCharacterSize(labelSize);
                taskName.setFillColor(sf::Color::White);
                taskName.setPosition({panelX + panelW * 0.15f, currentY + rowH * 0.18f});
                _window.draw(taskName);

                sf::Text estTime(_font, "Est. " + std::to_string(task->getEstimatedTime()) + "s");
                estTime.setCharacterSize(labelSize * 0.85f);
                estTime.setFillColor(sf::Color(120, 120, 120));
                estTime.setPosition({panelX + panelW * 0.70f, currentY + rowH * 0.18f});
                _window.draw(estTime);

                float barW = panelW * 0.85f;
                float barH = 2;
                sf::RectangleShape barBg({barW, barH});
                barBg.setFillColor(sf::Color(50, 50, 70));
                barBg.setPosition({panelX + panelW * 0.06f, currentY + rowH - barH - 1});
                _window.draw(barBg);

                sf::RectangleShape barFill({barW * (task->getProgress() / 100.f), barH});
                barFill.setFillColor(UI::statusColor(task->getStatus()));
                barFill.setPosition({panelX + panelW * 0.06f, currentY + rowH - barH - 1});
                _window.draw(barFill);

                sf::RectangleShape rowDiv({panelW - 2, 1});
                rowDiv.setFillColor(sf::Color(45, 45, 65));
                rowDiv.setPosition({panelX + 1, currentY + rowH - 1});
                _window.draw(rowDiv);
            }
            currentY += rowH;
        }
        currentY += rowH * 0.3f;
    };

    drawGroup("High Priority",   high,   sf::Color(255, 80,  80));
    drawGroup("Normal Priority", normal, sf::Color(80,  180, 255));
    drawGroup("Low Priority",    low,    sf::Color(120, 120, 120));

    _contentHeight = currentY + _scrollOffset - contentStartY;
}

void TaskQueueView::handleScroll(float delta, sf::Vector2f mousePos) {
    float W = _window.getSize().x;
    float H = _window.getSize().y;

    sf::FloatRect bounds({W * 0.70f, 0}, {W * 0.30f, H * 0.55f});
    if (!bounds.contains(mousePos)) return;

    _scrollOffset -= delta * 20.f;
    float maxScroll = std::max(0.f, _contentHeight - H * 0.55f * 0.85f);
    _scrollOffset = std::clamp(_scrollOffset, 0.f, maxScroll);
}