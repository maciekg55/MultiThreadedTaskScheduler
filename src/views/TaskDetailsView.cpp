// views/TaskDetailsView.cpp
#include "views/TaskDetailsView.h"
#include "UIHelpers.h"
#include <unordered_map>

TaskDetailsView::TaskDetailsView(sf::RenderWindow& window, sf::Font& font, Scheduler& scheduler)
    : _window(window), _font(font), _scheduler(scheduler) {}

void TaskDetailsView::draw() {
    float W = _window.getSize().x;
    float H = _window.getSize().y;

    float panelX    = W * 0.70f;
    float panelW    = W * 0.30f;
    float panelY    = H * 0.55f;
    float panelH    = H - panelY - H * 0.07f;
    float contentX  = panelX + panelW * 0.06f;
    float labelSize = H * 0.013f;
    float lineH     = H * 0.033f;

    sf::RectangleShape bg({panelW, panelH});
    bg.setFillColor(sf::Color(25, 25, 38));
    bg.setPosition({panelX, panelY});
    _window.draw(bg);

    sf::RectangleShape topDiv({panelW, 1});
    topDiv.setFillColor(sf::Color(60, 60, 80));
    topDiv.setPosition({panelX, panelY});
    _window.draw(topDiv);

    sf::Text title(_font, "Task Details");
    title.setCharacterSize(H * 0.016f);
    title.setFillColor(sf::Color(180, 180, 180));
    title.setPosition({contentX, panelY + H * 0.015f});
    _window.draw(title);

    sf::RectangleShape divider({panelW - 2, 1});
    divider.setFillColor(sf::Color(60, 60, 80));
    divider.setPosition({panelX + 1, panelY + H * 0.044f});
    _window.draw(divider);

    // find selected
    std::shared_ptr<Task> selected = nullptr;
    const auto& allTasks = _scheduler.getTasks();

    if (_selectedTaskId != -1) {
        auto it = allTasks.find(_selectedTaskId);
        if (it != allTasks.end()) selected = it->second;
    }
    if (!selected) {
        for (const auto& [id, task] : allTasks) {
            if (task->getStatus() == Task::TaskStatus::Running) {
                selected = task;
                break;
            }
        }
    }

    if (!selected) {
        sf::Text none(_font, "No task selected");
        none.setCharacterSize(H * 0.014f);
        none.setFillColor(sf::Color(80, 80, 80));
        none.setPosition({contentX, panelY + H * 0.08f});
        _window.draw(none);
        _deleteBtn = {};
        _editBtn   = {};
        return;
    }

    auto drawRow = [&](const std::string& key, const std::string& value,
                       sf::Color valueColor, float y) {
        sf::Text keyText(_font, key);
        keyText.setCharacterSize(labelSize);
        keyText.setFillColor(sf::Color(120, 120, 120));
        keyText.setPosition({contentX, y});
        _window.draw(keyText);

        sf::Text valText(_font, value);
        valText.setCharacterSize(labelSize);
        valText.setFillColor(valueColor);
        valText.setPosition({contentX + panelW * 0.42f, y});
        _window.draw(valText);
    };

    float y = panelY + H * 0.055f;

    sf::Text header(_font, "Task ID: " + std::to_string(selected->getId()) + " Name: " + selected->getName());
    header.setCharacterSize(H * 0.015f);
    header.setFillColor(sf::Color::White);
    header.setPosition({contentX, y});
    _window.draw(header);
    y += lineH;

    drawRow("Priority:",   UI::priorityStr(selected->getPriority()),
            UI::priorityColor(selected->getPriority()), y);  y += lineH;

    drawRow("Est. Time:",  std::to_string(selected->getEstimatedTime()) + "s",
            sf::Color::White, y);                            y += lineH;

    std::string depsStr;
    for (int depId : selected->getDependencies())
        depsStr += "ID" + std::to_string(depId) + " ";
    if (depsStr.empty()) depsStr = "None";
    drawRow("Depends on:", depsStr, sf::Color(180, 180, 255), y); y += lineH;

    static const std::unordered_map<Task::TaskStatus, std::string> statusLabels{
        {Task::TaskStatus::Planned, "Planned"},
        {Task::TaskStatus::Running,"Running"},
        {Task::TaskStatus::Queued,    "Queued"},
        {Task::TaskStatus::Completed,"Completed"},
        {Task::TaskStatus::Cancelled,"Cancelled"},
    };

    auto statusIt = statusLabels.find(selected->getStatus());
    std::string statusStr = statusIt != statusLabels.end() ? statusIt->second : "Unknown";
    drawRow("Status:", statusStr, UI::statusColor(selected->getStatus()), y); y += lineH;

    // progress
    sf::Text progressLabel(_font, "Progress:");
    progressLabel.setCharacterSize(labelSize);
    progressLabel.setFillColor(sf::Color(120, 120, 120));
    progressLabel.setPosition({contentX, y});
    _window.draw(progressLabel);

    float barY = y + lineH * 0.55f;
    float barW = panelW * 0.72f;
    float barH = H * 0.010f;

    sf::RectangleShape barBg({barW, barH});
    barBg.setFillColor(sf::Color(50, 50, 70));
    barBg.setPosition({contentX, barY});
    _window.draw(barBg);

    float fillW = barW * (selected->getProgress() / 100.f);
    if (fillW > 0) {
        sf::RectangleShape barFill({fillW, barH});
        barFill.setFillColor(UI::statusColor(selected->getStatus()));
        barFill.setPosition({contentX, barY});
        _window.draw(barFill);
    }

    sf::Text pct(_font, std::to_string(selected->getProgress()) + "%");
    pct.setCharacterSize(labelSize);
    pct.setFillColor(sf::Color::White);
    pct.setPosition({contentX + barW + panelW * 0.02f, barY - lineH * 0.1f});
    _window.draw(pct);
    y += lineH * 1.4f;

    // thread
    const auto& workers = _scheduler.getWorkers();
    std::string threadStr = "None";
    for (const auto& worker : workers) {
        if (worker.currentTask && worker.currentTask->getId() == selected->getId()) {
            threadStr = "Thread " + std::to_string(worker.id);
            break;
        }
    }
    drawRow("Thread:", threadStr, sf::Color(180, 255, 180), y);
    y += lineH * 1.2f;

    // edit/delete only for planned
    if (selected->getStatus() == Task::TaskStatus::Planned) {
        float btnW = panelW * 0.38f;
        float btnH = lineH * 0.95f;

        _deleteBtn = sf::FloatRect({contentX, y}, {btnW, btnH});
        sf::RectangleShape delBg({btnW, btnH});
        delBg.setFillColor(sf::Color(140, 40, 40));
        delBg.setPosition({contentX, y});
        _window.draw(delBg);
        sf::Text delTxt(_font, "Delete");
        delTxt.setCharacterSize(labelSize);
        delTxt.setFillColor(sf::Color::White);
        delTxt.setPosition({contentX + (btnW - delTxt.getLocalBounds().size.x) * 0.5f,
                            y + btnH * 0.15f});
        _window.draw(delTxt);

        float editX  = contentX + btnW + panelW * 0.04f;
        _editBtn = sf::FloatRect({editX, y}, {btnW, btnH});
        sf::RectangleShape editBg({btnW, btnH});
        editBg.setFillColor(sf::Color(40, 80, 140));
        editBg.setPosition({editX, y});
        _window.draw(editBg);
        sf::Text editTxt(_font, "Edit");
        editTxt.setCharacterSize(labelSize);
        editTxt.setFillColor(sf::Color::White);
        editTxt.setPosition({editX + (btnW - editTxt.getLocalBounds().size.x) * 0.5f,
                             y + btnH * 0.15f});
        _window.draw(editTxt);
    } else {
        _deleteBtn = {};
        _editBtn   = {};
    }
}