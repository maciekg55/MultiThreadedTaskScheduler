
#include "views/DependenciesGraphView.h"
#include "UIHelpers.h"
#include <map>
#include <cmath>
#include <algorithm>

DependenciesGraphView::DependenciesGraphView(sf::RenderWindow& window, sf::Font& font, Scheduler& scheduler)
    : _window(window), _font(font), _scheduler(scheduler) {}

void DependenciesGraphView::draw() {
    float W = _window.getSize().x;
    float H = _window.getSize().y;

    float panelX = W * 0.15f;
    float panelW = W * 0.55f;
    float panelY = H * 0.50f;
    float panelH = H * 0.43f;

    sf::RectangleShape bg({panelW, panelH});
    bg.setFillColor(sf::Color(20, 20, 32));
    bg.setPosition({panelX, panelY});
    _window.draw(bg);

    sf::RectangleShape divider({panelW, 1});
    divider.setFillColor(sf::Color(60, 60, 80));
    divider.setPosition({panelX, panelY});
    _window.draw(divider);

    sf::Text title(_font, "Task Dependency Graph");
    title.setCharacterSize(H * 0.018f);
    title.setFillColor(sf::Color(180, 180, 180));
    title.setPosition({panelX + panelW * 0.03f, panelY + H * 0.015f});
    _window.draw(title);

    sf::RectangleShape titleDiv({panelW - 2, 1});
    titleDiv.setFillColor(sf::Color(60, 60, 80));
    titleDiv.setPosition({panelX + 1, panelY + H * 0.05f});
    _window.draw(titleDiv);

    const auto& tasks = _scheduler.getTasks();
    if (tasks.empty()) return;

    // assign each task a column equal to its longest dependency chain length
    std::unordered_map<int, int> depth;
    for (const auto& [id, task] :tasks) depth[id] = 0;

    bool changed = true;
    while (changed) {
        changed = false;
        for (const auto& [id, task] :tasks) {
            for (int depId : task->getDependencies()) {
                if (depth.count(depId) && depth[depId] + 1 > depth[id]) {
                    depth[id] = depth[depId] + 1;
                    changed = true;
                }
            }
        }
    }

    std::map<int, std::vector<int>> columns;
    for (const auto& [id, d] :depth) columns[d].push_back(id);
    int numCols = columns.size();

    float graphX = panelX+ panelW * 0.04f;
    float graphY = panelY +H * 0.06f;
    float graphW = panelW * 0.92f;
    float graphH = panelH - H * 0.08f;
    float nodeW = std::min(graphW /(numCols *2.0f),W * 0.08f);
    float nodeH = H * 0.055f;

    std::unordered_map<int, sf::Vector2f> nodePos;
    for (auto& [col, ids] : columns) {
        int numRows = ids.size();
        float colX = graphX +(col + 0.5f) * (graphW / numCols) - nodeW * 0.5f;
        for (int row = 0; row< numRows; row++) {
            float colY = graphY + (row + 0.5f) * (graphH / numRows) - nodeH * 0.5f;
            nodePos[ids[row]] ={colX, colY};
        }
    }

    for (const auto& [id, task] : tasks) {
        for (int depId :task->getDependencies()) {
            if (!nodePos.count(id) || !nodePos.count(depId)) continue;

            sf::Vector2f from = nodePos[depId];
            sf::Vector2f to = nodePos[id];
            float x1 = from.x + nodeW;
            float y1 = from.y + nodeH * 0.5f;
            float x2 = to.x;
            float y2 = to.y + nodeH * 0.5f;

            float dx = x2 - x1;
            float dy = y2 - y1;
            float len = std::sqrt(dx*dx + dy*dy);
            float angle = std::atan2(dy, dx) * 180.f / 3.14159f;

            sf::RectangleShape line({len, 1.5f});
            line.setFillColor(sf::Color(80, 80, 120));
            line.setPosition({x1, y1});
            line.setRotation(sf::degrees(angle));
            _window.draw(line);

            float arrowSize = H * 0.012f;
            sf::ConvexShape arrow;
            arrow.setPointCount(3);
            arrow.setPoint(0, {0, 0});
            arrow.setPoint(1, {-arrowSize, -arrowSize * 0.5f});
            arrow.setPoint(2, {-arrowSize,  arrowSize * 0.5f});
            arrow.setFillColor(sf::Color(80, 80, 120));
            arrow.setPosition({x2, y2});
            arrow.setRotation(sf::degrees(angle));
            _window.draw(arrow);
        }
    }


    _nodeRects.clear();
    for (const auto& [id, task] :tasks) {
        if (!nodePos.count(id)) continue;
        sf::Vector2f pos = nodePos[id];
        _nodeRects[id] = sf::FloatRect(pos, {nodeW, nodeH});

        sf::RectangleShape node({nodeW, nodeH});
        node.setFillColor(UI::priorityBgColor(task->getPriority()));
        node.setOutlineThickness(id == _selectedTaskId ? 2.5f : 1.5f);
        node.setOutlineColor(id == _selectedTaskId
            ? sf::Color::White
            : UI::statusColor(task->getStatus()));
        node.setPosition(pos);
        _window.draw(node);

        sf::Text idText(_font, "Task ID: " + std::to_string(id));
        idText.setCharacterSize(H * 0.013f);
        idText.setFillColor(UI::statusColor(task->getStatus()));
        idText.setPosition({pos.x + nodeW * 0.08f, pos.y + nodeH * 0.08f});
        _window.draw(idText);

        std::string name = task->getName();
        if (name.size() >10) name = name.substr(0, 9) +".";
        sf::Text nameText(_font, name);
        nameText.setCharacterSize(H * 0.012f);
        nameText.setFillColor(sf::Color::White);
        nameText.setPosition({pos.x + nodeW * 0.08f, pos.y + nodeH * 0.50f});
        _window.draw(nameText);
    }
}
