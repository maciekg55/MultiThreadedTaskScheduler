#include "Renderer.h"
#include <map>
#include "ComputationTask.h"
#include "IOTask.h"
#include "RenderTask.h"
#include "FileCheckTask.h"



Renderer::Renderer(sf::RenderWindow& window, Scheduler& scheduler)
    : _window(window), _scheduler(scheduler)
{
    _font.openFromFile("assets/JetBrainsMono-Regular.ttf");
    _appStartTime = std::chrono::steady_clock::now();
}


void Renderer::draw() {
    drawThreadPanel();
    drawGanttPanel();
    drawDependencyGraph();
    drawTaskQueuePanel();
    drawTaskDetailsPanel();
    drawStatsBar();
    drawAddTaskButton();
    drawAddTaskForm();
}


void Renderer::drawThreadPanel() {
    float W = _window.getSize().x;
    float H = _window.getSize().y;

    float panelW = W * 0.15f;
    float panelH = H;
    float clipTop = H * 0.07f;
    float clipBot = panelH - 2.f;
    float rowH    = H * 0.07f;
    float startY  = H * 0.07f;
    float boxR    = H * 0.014f;

    // background
    sf::RectangleShape bg({panelW, panelH});
    bg.setFillColor(sf::Color(25, 25, 38));
    bg.setPosition({0, 0});
    _window.draw(bg);

    // title
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
        float rowY = startY + i * rowH - _threadScrollOffset;

        // skip rows outside visible area
        if (rowY + rowH < clipTop || rowY > clipBot) continue;

        // row highlight if running
        if (worker.currentTask != nullptr) {
            sf::RectangleShape highlight({panelW - 2, rowH - 2});
            highlight.setFillColor(sf::Color(40, 40, 60));
            highlight.setPosition({1, rowY});
            _window.draw(highlight);
        }

        Task::TaskStatus workerStatus = worker.currentTask
            ? Task::TaskStatus::Running
            : Task::TaskStatus::Planned;

        sf::RectangleShape box({boxR, boxR});
        box.setFillColor(getStatusColor(workerStatus));
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

    // track content height for scroll clamping
    _threadContentHeight = workers.size() * rowH;
}


void Renderer::drawGanttPanel() {
    float W = _window.getSize().x;
    float H = _window.getSize().y;

    float panelX = W * 0.15f;
    float panelW = W * 0.55f;
    float panelH = H * 0.50f;

    // background
    sf::RectangleShape bg({panelW, panelH});
    bg.setFillColor(sf::Color(22, 22, 35));
    bg.setPosition({panelX, 0});
    _window.draw(bg);

    // title
    sf::Text title(_font, "Task Execution Scheduler ");
    title.setCharacterSize(H * 0.018f);
    title.setFillColor(sf::Color(180, 180, 180));
    title.setPosition({panelX + panelW * 0.03f, H * 0.02f});
    _window.draw(title);

    sf::RectangleShape divider({panelW - 2, 1});
    divider.setFillColor(sf::Color(60, 60, 80));
    divider.setPosition({panelX + 1, H * 0.055f});
    _window.draw(divider);

    // layout
    const auto& workers = _scheduler.getWorkers();
    int  numThreads = workers.size();
    float headerH   = H * 0.07f;
    float rowH      = (panelH - headerH) / std::max(numThreads, 1);
    float chartX    = panelX + panelW * 0.08f;  // left margin for thread labels
    float chartW    = panelW * 0.88f;
    float chartY    = headerH;

    // time window — show last 30 seconds
    auto  now         = std::chrono::steady_clock::now();
    float elapsed     = std::chrono::duration<float>(now - _appStartTime).count();
    float windowSecs  = std::max(elapsed + 5.f, 30.f);  // grows with time, min 30s

    // time axis labels
    int numTicks = 8;
    for (int t = 0; t <= numTicks; t++) {
        float frac  = (float)t / numTicks;
        float tickX = chartX + frac * chartW;
        float timeSec = frac * windowSecs;  // 0 to windowSecs, no offset

        // tick line
        sf::RectangleShape tick({1, panelH - headerH + H * 0.01f});
        tick.setFillColor(sf::Color(45, 45, 65));
        tick.setPosition({tickX, chartY - H * 0.01f});
        _window.draw(tick);

        // time label
        sf::Text timeLabel(_font, std::to_string((int)timeSec) + "s");
        timeLabel.setCharacterSize(H * 0.012f);
        timeLabel.setFillColor(sf::Color(80, 80, 100));
        timeLabel.setPosition({tickX - 8, chartY - H * 0.03f});
        _window.draw(timeLabel);
    }

    // thread rows
    for (int i = 0; i < numThreads; i++) {
        float rowY = chartY + i * rowH;

        // thread label
        sf::Text threadLabel(_font, "Thread " + std::to_string(i));
        threadLabel.setCharacterSize(H * 0.013f);
        threadLabel.setFillColor(sf::Color(140, 140, 140));
        threadLabel.setPosition({panelX + panelW * 0.01f, rowY + rowH * 0.35f});
        _window.draw(threadLabel);

        // row divider
        sf::RectangleShape rowDiv({chartW, 1});
        rowDiv.setFillColor(sf::Color(35, 35, 52));
        rowDiv.setPosition({chartX, rowY + rowH - 1});
        _window.draw(rowDiv);

        // draw task bars for this thread
        for (const auto& [id, task] : _scheduler.getTasks()) {
            if (!task->hasStarted()) continue;
            if (task->getAssignedThread() != i) continue;

            // calculate bar position on timeline
            float taskStart = std::chrono::duration<float>(
                task->getStartTime() - _appStartTime).count();
            float taskEnd = task->hasEnded() ? std::chrono::duration<float>(task->getEndTime() - _appStartTime).count() : elapsed;

            // map to chart coordinates
            float x1 = chartX + (taskStart / windowSecs) * chartW;
            float x2 = chartX + (taskEnd   / windowSecs) * chartW;

            x1 = std::max(x1, chartX);
            x2 = std::min(x2, chartX + chartW);
            if (x2 <= x1) continue;

            float barH  = rowH * 0.55f;
            float barY  = rowY + rowH * 0.22f;

            sf::RectangleShape bar({x2 - x1, barH});
            bar.setFillColor(getStatusColor(task->getStatus()));
            bar.setPosition({x1, barY});
            _window.draw(bar);

            // task name on bar if wide enough
            if (x2 - x1 > 30) {
                sf::Text barLabel(_font, task->getName());
                barLabel.setCharacterSize(H * 0.012f);
                barLabel.setFillColor(sf::Color::White);
                barLabel.setPosition({x1 + 4, barY + barH * 0.2f});
                _window.draw(barLabel);
            }
        }
    }

    // "now" line
    sf::RectangleShape nowLine({2, panelH - headerH});
    nowLine.setFillColor(sf::Color(255, 80, 80));
    nowLine.setPosition({chartX + chartW, chartY});
    _window.draw(nowLine);

    sf::Text nowLabel(_font, "Now");
    nowLabel.setCharacterSize(H * 0.012f);
    nowLabel.setFillColor(sf::Color(255, 80, 80));
    nowLabel.setPosition({chartX + chartW - 16, chartY - H * 0.025f});
    _window.draw(nowLabel);
}


void Renderer::drawDependencyGraph() {
    float W = _window.getSize().x;
    float H = _window.getSize().y;

    float panelX = W * 0.15f;
    float panelW = W * 0.55f;
    float panelY = H * 0.50f;
    float panelH = H * 0.43f;  // leaves room for stats bar

    // background
    sf::RectangleShape bg({panelW, panelH});
    bg.setFillColor(sf::Color(20, 20, 32));
    bg.setPosition({panelX, panelY});
    _window.draw(bg);

    // top divider
    sf::RectangleShape divider({panelW, 1});
    divider.setFillColor(sf::Color(60, 60, 80));
    divider.setPosition({panelX, panelY});
    _window.draw(divider);

    // title
    sf::Text title(_font, "TASK DEPENDENCY GRAPH");
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

    // --- layout: arrange nodes in columns based on dependency depth ---
    // depth 0 = no deps, depth 1 = depends on depth 0, etc.
    std::unordered_map<int, int> depth;
    for (const auto& [id, task] : tasks)
        depth[id] = 0;

    // simple iterative depth calculation
    bool changed = true;
    while (changed) {
        changed = false;
        for (const auto& [id, task] : tasks) {
            for (int depId : task->getDependencies()) {
                if (depth.count(depId) && depth[depId] + 1 > depth[id]) {
                    depth[id] = depth[depId] + 1;
                    changed = true;
                }
            }
        }
    }

    // group tasks by depth column
    std::map<int, std::vector<int>> columns;
    for (const auto& [id, d] : depth)
        columns[d].push_back(id);

    int numCols = columns.size();

    float graphX = panelX + panelW * 0.04f;
    float graphY = panelY + H * 0.06f;
    float graphW = panelW * 0.92f;
    float graphH = panelH - H * 0.08f;

    float nodeW  = std::min(graphW / (numCols * 2.0f), W * 0.08f);
    float nodeH  = H * 0.055f;

    // calculate node screen positions
    std::unordered_map<int, sf::Vector2f> nodePos;

    for (auto& [col, ids] : columns) {
        int   numRows = ids.size();
        float colX    = graphX + (col + 0.5f) * (graphW / numCols) - nodeW * 0.5f;

        for (int row = 0; row < numRows; row++) {
            float colY = graphY + (row + 0.5f) * (graphH / numRows) - nodeH * 0.5f;
            nodePos[ids[row]] = {colX, colY};
        }
    }

    // --- draw arrows first (behind nodes) ---
    for (const auto& [id, task] : tasks) {
        for (int depId : task->getDependencies()) {
            if (!nodePos.count(id) || !nodePos.count(depId)) continue;

            sf::Vector2f from = nodePos[depId];
            sf::Vector2f to   = nodePos[id];

            // arrow from right edge of dep node to left edge of this node
            float x1 = from.x + nodeW;
            float y1 = from.y + nodeH * 0.5f;
            float x2 = to.x;
            float y2 = to.y + nodeH * 0.5f;

            // line
            float dx   = x2 - x1;
            float dy   = y2 - y1;
            float len  = std::sqrt(dx*dx + dy*dy);
            float angle = std::atan2(dy, dx) * 180.f / 3.14159f;

            sf::RectangleShape line({len, 1.5f});
            line.setFillColor(sf::Color(80, 80, 120));
            line.setPosition({x1, y1});
            line.setRotation(sf::degrees(angle));
            _window.draw(line);

            // arrowhead triangle
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

    // --- draw nodes ---
    for (const auto& [id, task] : tasks) {
        if (!nodePos.count(id)) continue;
        sf::Vector2f pos = nodePos[id];

        // node background
        sf::RectangleShape node({nodeW, nodeH});
        node.setFillColor(sf::Color(35, 35, 55));
        node.setOutlineThickness(1.5f);
        node.setOutlineColor(getStatusColor(task->getStatus()));
        node.setPosition(pos);
        _window.draw(node);

        // task id
        sf::Text idText(_font, "ID: " + std::to_string(id));
        idText.setCharacterSize(H * 0.013f);
        idText.setFillColor(getStatusColor(task->getStatus()));
        idText.setPosition({pos.x + nodeW * 0.08f, pos.y + nodeH * 0.08f});
        _window.draw(idText);

        // task name — truncate if too long
        std::string name = task->getName();
        if (name.size() > 10) name = name.substr(0, 9) + ".";
        sf::Text nameText(_font, name);
        nameText.setCharacterSize(H * 0.012f);
        nameText.setFillColor(sf::Color::White);
        nameText.setPosition({pos.x + nodeW * 0.08f, pos.y + nodeH * 0.50f});
        _window.draw(nameText);
    }
}


void Renderer::drawTaskQueuePanel() {
    float W = _window.getSize().x;
    float H = _window.getSize().y;

    float panelX = W * 0.70f;
    float panelW = W * 0.30f;
    float panelH = H * 0.55f;

    // background
    sf::RectangleShape bg({panelW, panelH});
    bg.setFillColor(sf::Color(25, 25, 38));
    bg.setPosition({panelX, 0});
    _window.draw(bg);

    // title
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

    float currentY  = H * 0.07f - _queueScrollOffset;
    float contentStartY = H * 0.07f;
    float rowH      = H * 0.055f;
    float labelSize = H * 0.014f;
    float clipTop   = H * 0.07f;       // don't draw above title divider
    float clipBot   = panelH - 2.f;    // don't draw below panel

    auto inBounds = [&](float y) {
        return y + rowH > clipTop && y < clipBot;
    };

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

                if (task->getId() == _selectedTaskId) {
                    sf::RectangleShape sel({panelW - 2, rowH - 1});
                    sel.setFillColor(sf::Color(50, 50, 80));
                    sel.setPosition({panelX + 1, currentY});
                    _window.draw(sel);
                }

                sf::RectangleShape rowBg({panelW - 2, rowH - 1});
                rowBg.setFillColor(sf::Color(30, 30, 45));
                rowBg.setPosition({panelX + 1, currentY});
                _window.draw(rowBg);

                float dotR = H * 0.008f;
                sf::CircleShape dot(dotR);
                dot.setFillColor(getStatusColor(task->getStatus()));
                dot.setPosition({panelX + panelW * 0.06f, currentY + rowH * 0.5f - dotR});
                _window.draw(dot);

                sf::Text taskName(_font, "ID: " + std::to_string(task->getId()) + " - " + task->getName());
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
                barFill.setFillColor(getStatusColor(task->getStatus()));
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

    _taskRowRects.clear();
    drawGroup("High Priority",   high,   sf::Color(255, 80,  80));
    drawGroup("Normal Priority", normal, sf::Color(80,  180, 255));
    drawGroup("Low Priority",    low,    sf::Color(120, 120, 120));

    _queueContentHeight = currentY + _queueScrollOffset - contentStartY;
}


void Renderer::drawTaskDetailsPanel() {
    float W = _window.getSize().x;
    float H = _window.getSize().y;

    float panelX    = W * 0.70f;
    float panelW    = W * 0.30f;
    float panelY    = H * 0.55f;
    float panelH    = H - panelY - H * 0.07f;  // fills to stats bar exactly
    float contentX  = panelX + panelW * 0.06f;
    float labelSize = H * 0.013f;
    float lineH     = H * 0.033f;  // tighter line spacing

    // background
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

    // find selected task
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

    // header
    sf::Text header(_font, "ID: " + std::to_string(selected->getId()) + " - " + selected->getName());
    header.setCharacterSize(H * 0.015f);
    header.setFillColor(sf::Color::White);
    header.setPosition({contentX, y});
    _window.draw(header);
    y += lineH;

    // priority
    std::string priorityStr =
        selected->getPriority() >= 3 ? "High" :
        selected->getPriority() == 2 ? "Medium" : "Low";
    sf::Color priorityColor =
        selected->getPriority() >= 3 ? sf::Color(255, 80,  80)  :
        selected->getPriority() == 2 ? sf::Color(80,  180, 255) :
                                       sf::Color(120, 120, 120);
    drawRow("Priority:",   priorityStr, priorityColor, y); y += lineH;

    drawRow("Est. Time:",  std::to_string(selected->getEstimatedTime()) + "s",
            sf::Color::White, y);                          y += lineH;

    std::string depsStr;
    for (int depId : selected->getDependencies())
        depsStr += "Task ID: " + std::to_string(depId) + " ";
    if (depsStr.empty()) depsStr = "None";
    drawRow("Depends on:", depsStr, sf::Color(180, 180, 255), y); y += lineH;

    static const std::unordered_map<Task::TaskStatus, std::string> statusLabels = {
        {Task::TaskStatus::Planned,   "Planned"},
        {Task::TaskStatus::Waiting,   "Waiting"},
        {Task::TaskStatus::Running,   "Running"},
        {Task::TaskStatus::Completed, "Completed"},
        {Task::TaskStatus::Cancelled, "Cancelled"},
    };
    drawRow("Status:", statusLabels.at(selected->getStatus()),
            getStatusColor(selected->getStatus()), y);     y += lineH;

    // progress bar
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
        barFill.setFillColor(getStatusColor(selected->getStatus()));
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

    // edit / delete — only for planned tasks
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

        float editX = contentX + btnW + panelW * 0.04f;
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


void Renderer::drawStatsBar() {
    float W = _window.getSize().x;
    float H = _window.getSize().y;

    float barH = H * 0.07f;
    float barY = H - barH;

    // background
    sf::RectangleShape bg({W, barH});
    bg.setFillColor(sf::Color(18, 18, 30));
    bg.setPosition({0, barY});
    _window.draw(bg);

    // top divider
    sf::RectangleShape divider({W, 1});
    divider.setFillColor(sf::Color(60, 60, 80));
    divider.setPosition({0, barY});
    _window.draw(divider);

    // count tasks by status
    const auto& tasks = _scheduler.getTasks();
    int total = tasks.size();
    int running = 0, waiting = 0, completed = 0, planned = 0;

    for (const auto& [id, task] : tasks) {
        switch (task->getStatus()) {
            case Task::TaskStatus::Running:   running++;   break;
            case Task::TaskStatus::Waiting:   waiting++;   break;
            case Task::TaskStatus::Completed: completed++; break;
            case Task::TaskStatus::Planned:   planned++;   break;
            default: break;
        }
    }

    float labelSize = H * 0.015f;
    float centerY   = barY + barH * 0.5f - labelSize * 0.5f;

    // helper to draw one stat block
    float cursorX = W * 0.02f;
    auto drawStat = [&](const std::string& label, int value, sf::Color color) {
        sf::Text lbl(_font, label + ": ");
        lbl.setCharacterSize(labelSize);
        lbl.setFillColor(sf::Color(120, 120, 120));
        lbl.setPosition({cursorX, centerY});
        _window.draw(lbl);
        cursorX += lbl.getLocalBounds().size.x + 2;

        sf::Text val(_font, std::to_string(value));
        val.setCharacterSize(labelSize);
        val.setFillColor(color);
        val.setPosition({cursorX, centerY});
        _window.draw(val);
        cursorX += val.getLocalBounds().size.x + W * 0.03f;
    };

    drawStat("Total",     total,     sf::Color::White);
    drawStat("Running",   running,   sf::Color(0, 200, 100));
    drawStat("Waiting",   waiting,   sf::Color(255, 165, 0));
    drawStat("Completed", completed, sf::Color(100, 100, 255));
    drawStat("Planned",   planned,   sf::Color(120, 120, 120));

    float btnW = W * 0.07f;
    float btnH = barH * 0.60f;
    float btnY = barY + barH * 0.20f;
    float btnX = W - (btnW + W * 0.01f) * 3 - W * 0.02f;

    struct Button { std::string label; sf::Color color; sf::FloatRect* rect; };
    std::vector<Button> buttons = {
        {"Start", sf::Color(0,   160, 80),  &_startBtn},
        {"Pause", sf::Color(180, 140, 0),   &_pauseBtn},
        {"Stop",  sf::Color(180, 40,  40),  &_stopBtn },
    };

    for (auto& btn : buttons) {
        // save the rect so Engine can hit-test it
        *btn.rect = sf::FloatRect({btnX, btnY}, {btnW, btnH});

        sf::RectangleShape btnBg({btnW, btnH});
        btnBg.setFillColor(btn.color);
        btnBg.setPosition({btnX, btnY});
        _window.draw(btnBg);

        sf::Text btnLabel(_font, btn.label);
        btnLabel.setCharacterSize(labelSize);
        btnLabel.setFillColor(sf::Color::White);
        float textX = btnX + (btnW - btnLabel.getLocalBounds().size.x) * 0.5f;
        float textY = btnY + (btnH - labelSize) * 0.5f - labelSize * 0.1f;
        btnLabel.setPosition({textX, textY});
        _window.draw(btnLabel);

        btnX += btnW + W * 0.01f;
    }
}


sf::Color Renderer::getStatusColor(Task::TaskStatus status) {
    switch (status) {
        case Task::TaskStatus::Running:   return sf::Color(0, 200, 100);    // green
        case Task::TaskStatus::Waiting:   return sf::Color(255, 165, 0);    // orange
        case Task::TaskStatus::Completed: return sf::Color(100, 100, 255);  // blue
        case Task::TaskStatus::Cancelled: return sf::Color(255, 60, 60);    // red
        case Task::TaskStatus::Planned:   return sf::Color(120, 120, 120);  // gray
        default:                          return sf::Color::White;
    }
}


void Renderer::handleScroll(float delta, sf::Vector2f mousePos) {
    float W = _window.getSize().x;
    float H = _window.getSize().y;

    // task queue panel
    sf::FloatRect queueBounds({W * 0.70f, 0}, {W * 0.30f, H * 0.55f});
    if (queueBounds.contains(mousePos)) {
        _queueScrollOffset -= delta * 20.f;
        float maxScroll = std::max(0.f, _queueContentHeight - H * 0.55f * 0.85f);
        _queueScrollOffset = std::clamp(_queueScrollOffset, 0.f, maxScroll);
        return;
    }

    // thread panel
    sf::FloatRect threadBounds({0, 0}, {W * 0.15f, H});
    if (threadBounds.contains(mousePos)) {
        _threadScrollOffset -= delta * 20.f;
        float maxScroll = std::max(0.f, _threadContentHeight - H * 0.85f);
        _threadScrollOffset = std::clamp(_threadScrollOffset, 0.f, maxScroll);
        return;
    }
}


void Renderer::drawAddTaskButton() {
    float W = _window.getSize().x;
    float H = _window.getSize().y;

    float barH = H * 0.07f;
    float barY = H - barH;
    float btnW = W * 0.07f;
    float btnH = barH * 0.60f;
    float btnY = barY + barH * 0.20f;
    // sits to the left of Start button
    float btnX = W - (btnW + W * 0.01f) * 4 - W * 0.02f;

    _addTaskForm.addTaskBtn = sf::FloatRect({btnX, btnY}, {btnW, btnH});

    sf::RectangleShape btn({btnW, btnH});
    btn.setFillColor(sf::Color(60, 60, 160));
    btn.setPosition({btnX, btnY});
    _window.draw(btn);

    sf::Text label(_font, "Add Task");
    label.setCharacterSize(H * 0.015f);
    label.setFillColor(sf::Color::White);
    float textX = btnX + (btnW - label.getLocalBounds().size.x) * 0.5f;
    float textY = btnY + (btnH - H * 0.015f) * 0.5f;
    label.setPosition({textX, textY});
    _window.draw(label);
}


void Renderer::drawAddTaskForm() {
    if (!_addTaskForm.visible) return;

    float W = _window.getSize().x;
    float H = _window.getSize().y;

    // modal background overlay
    sf::RectangleShape overlay({W, H});
    overlay.setFillColor(sf::Color(0, 0, 0, 160));
    overlay.setPosition({0, 0});
    _window.draw(overlay);

    // form box
    float formW = W * 0.35f;
    float formH = H * 0.65f;
    float formX = (W - formW) * 0.5f;
    float formY = (H - formH) * 0.5f;

    sf::RectangleShape formBg({formW, formH});
    formBg.setFillColor(sf::Color(28, 28, 45));
    formBg.setOutlineThickness(1);
    formBg.setOutlineColor(sf::Color(60, 60, 100));
    formBg.setPosition({formX, formY});
    _window.draw(formBg);

    // title
    sf::Text title(_font, "Add New Task");
    title.setCharacterSize(H * 0.022f);
    title.setFillColor(sf::Color::White);
    title.setPosition({formX + formW * 0.06f, formY + formH * 0.04f});
    _window.draw(title);

    sf::RectangleShape titleDiv({formW - 2, 1});
    titleDiv.setFillColor(sf::Color(60, 60, 80));
    titleDiv.setPosition({formX + 1, formY + formH * 0.11f});
    _window.draw(titleDiv);

    float fieldX    = formX + formW * 0.06f;
    float fieldW    = formW * 0.88f;
    float fieldH    = H * 0.048f;
    float labelSize = H * 0.015f;
    float curY      = formY + formH * 0.14f;
    float gap       = H * 0.085f;

    // helper to draw a labeled text input
    auto drawInput = [&](const std::string& label, TextInput& input, float y) {
        sf::Text lbl(_font, label);
        lbl.setCharacterSize(labelSize);
        lbl.setFillColor(sf::Color(140, 140, 160));
        lbl.setPosition({fieldX, y});
        _window.draw(lbl);

        float inputY = y + labelSize * 1.4f;
        input.bounds = sf::FloatRect({fieldX, inputY}, {fieldW, fieldH});

        sf::RectangleShape inputBg({fieldW, fieldH});
        inputBg.setFillColor(sf::Color(20, 20, 35));
        inputBg.setOutlineThickness(1);
        inputBg.setOutlineColor(input.focused
            ? sf::Color(100, 100, 255)
            : sf::Color(55, 55, 80));
        inputBg.setPosition({fieldX, inputY});
        _window.draw(inputBg);

        sf::Text val(_font, input.value + (input.focused ? "|" : ""));
        val.setCharacterSize(labelSize);
        val.setFillColor(sf::Color::White);
        val.setPosition({fieldX + fieldW * 0.03f, inputY + fieldH * 0.2f});
        _window.draw(val);
    };

    drawInput("Task Name", _addTaskForm.nameInput, curY);
    curY += gap;

    drawInput("Dependencies (comma separated IDs, e.g. 1,3)", _addTaskForm.depsInput, curY);
    curY += gap;

    drawInput("Estimated Time (seconds)", _addTaskForm.estTimeInput, curY);
    curY += gap;

    // priority selector
    sf::Text prioLabel(_font, "Priority");
    prioLabel.setCharacterSize(labelSize);
    prioLabel.setFillColor(sf::Color(140, 140, 160));
    prioLabel.setPosition({fieldX, curY});
    _window.draw(prioLabel);
    curY += labelSize * 1.4f;

    std::vector<std::pair<std::string, int>> priorities = {
        {"Low", 1}, {"Medium", 2}, {"High", 3}
    };
    float btnW = fieldW / 3.f - 4.f;
    float bX   = fieldX;
    for (auto& [name, val] : priorities) {
        bool selected = _addTaskForm.selectedPriority == val;
        sf::RectangleShape btn({btnW, fieldH});
        btn.setFillColor(selected ? sf::Color(60, 80, 180) : sf::Color(30, 30, 50));
        btn.setOutlineThickness(1);
        btn.setOutlineColor(sf::Color(60, 60, 100));
        btn.setPosition({bX, curY});
        _window.draw(btn);

        sf::Text btnTxt(_font, name);
        btnTxt.setCharacterSize(labelSize);
        btnTxt.setFillColor(sf::Color::White);
        btnTxt.setPosition({bX + (btnW - btnTxt.getLocalBounds().size.x) * 0.5f,
                            curY + fieldH * 0.2f});
        _window.draw(btnTxt);
        bX += btnW + 4.f;
    }
    curY += gap;

    // type selector
    sf::Text typeLabel(_font, "Task Type");
    typeLabel.setCharacterSize(labelSize);
    typeLabel.setFillColor(sf::Color(140, 140, 160));
    typeLabel.setPosition({fieldX, curY});
    _window.draw(typeLabel);
    curY += labelSize * 1.4f;

    std::vector<std::string> types = {"Computation", "IO", "FileCheck", "Render"};
    float typeBtnW = fieldW / 4.f - 4.f;
    float tX = fieldX;
    for (int i = 0; i < (int)types.size(); i++) {
        bool selected = _addTaskForm.selectedType == i;
        sf::RectangleShape btn({typeBtnW, fieldH});
        btn.setFillColor(selected ? sf::Color(60, 80, 180) : sf::Color(30, 30, 50));
        btn.setOutlineThickness(1);
        btn.setOutlineColor(sf::Color(60, 60, 100));
        btn.setPosition({tX, curY});
        _window.draw(btn);

        sf::Text btnTxt(_font, types[i]);
        btnTxt.setCharacterSize(labelSize * 0.85f);
        btnTxt.setFillColor(sf::Color::White);
        btnTxt.setPosition({tX + (typeBtnW - btnTxt.getLocalBounds().size.x) * 0.5f,
                            curY + fieldH * 0.2f});
        _window.draw(btnTxt);
        tX += typeBtnW + 4.f;
    }
    curY += gap;

    // confirm / cancel
    float cbtnW = fieldW * 0.45f;
    _addTaskForm.cancelBtn  = sf::FloatRect({fieldX, curY}, {cbtnW, fieldH});
    _addTaskForm.confirmBtn = sf::FloatRect({fieldX + cbtnW + fieldW * 0.1f, curY}, {cbtnW, fieldH});

    sf::RectangleShape cancelBg({cbtnW, fieldH});
    cancelBg.setFillColor(sf::Color(120, 40, 40));
    cancelBg.setPosition({fieldX, curY});
    _window.draw(cancelBg);
    sf::Text cancelTxt(_font, "Cancel");
    cancelTxt.setCharacterSize(labelSize);
    cancelTxt.setFillColor(sf::Color::White);
    cancelTxt.setPosition({fieldX + (cbtnW - cancelTxt.getLocalBounds().size.x) * 0.5f,
                           curY + fieldH * 0.2f});
    _window.draw(cancelTxt);

    sf::RectangleShape confirmBg({cbtnW, fieldH});
    confirmBg.setFillColor(sf::Color(40, 120, 40));
    confirmBg.setPosition({fieldX + cbtnW + fieldW * 0.1f, curY});
    _window.draw(confirmBg);
    sf::Text confirmTxt(_font, "Add Task");
    confirmTxt.setCharacterSize(labelSize);
    confirmTxt.setFillColor(sf::Color::White);
    confirmTxt.setPosition({fieldX + cbtnW + fieldW * 0.1f + (cbtnW - confirmTxt.getLocalBounds().size.x) * 0.5f,
                            curY + fieldH * 0.2f});
    _window.draw(confirmTxt);
}

void Renderer::handleTextInput(uint32_t unicode) {
    _addTaskForm.nameInput.handleChar(unicode);
    _addTaskForm.depsInput.handleChar(unicode);
    _addTaskForm.estTimeInput.handleChar(unicode);
}

void Renderer::handleFormClick(sf::Vector2f mouse) {
    // task row selection — always check regardless of form state
    for (const auto& [id, rect] : _taskRowRects) {
        if (rect.contains(mouse)) {
            _selectedTaskId = (_selectedTaskId == id) ? -1 : id;
            return;
        }
    }

    // delete button
    if (_deleteBtn != sf::FloatRect{} && _deleteBtn.contains(mouse)) {
        _pendingDelete  = _selectedTaskId;
        _selectedTaskId = -1;
        return;
    }

    // edit button — open form pre-filled
    if (_editBtn != sf::FloatRect{} && _editBtn.contains(mouse)) {
        auto it = _scheduler.getTasks().find(_selectedTaskId);
        if (it != _scheduler.getTasks().end()) {
            auto& task = it->second;
            _addTaskForm.nameInput.value    = task->getName();
            _addTaskForm.estTimeInput.value = std::to_string(task->getEstimatedTime());
            _addTaskForm.selectedPriority   = task->getPriority();

            std::string depsStr;
            for (int d : task->getDependencies())
                depsStr += std::to_string(d) + ",";
            if (!depsStr.empty()) depsStr.pop_back();
            _addTaskForm.depsInput.value = depsStr;

            _editMode            = true;
            _editTaskId          = _selectedTaskId;
            _addTaskForm.visible = true;
        }
        return;
    }

    // open form via Add Task button
    if (_addTaskForm.addTaskBtn.contains(mouse)) {
        _editMode            = false;
        _addTaskForm.visible = true;
        return;
    }

    // everything below only runs when form is open
    if (!_addTaskForm.visible) return;

    // text input focus
    _addTaskForm.nameInput.focused    = _addTaskForm.nameInput.bounds.contains(mouse);
    _addTaskForm.depsInput.focused    = _addTaskForm.depsInput.bounds.contains(mouse);
    _addTaskForm.estTimeInput.focused = _addTaskForm.estTimeInput.bounds.contains(mouse);

    // recalculate form layout to hit-test buttons
    float W      = _window.getSize().x;
    float H      = _window.getSize().y;
    float formW  = W * 0.35f;
    float formX  = (W - formW) * 0.5f;
    float formY  = (H - H * 0.65f) * 0.5f;
    float fieldX = formX + formW * 0.06f;
    float fieldW = formW * 0.88f;
    float fieldH = H * 0.048f;
    float gap    = H * 0.085f;
    float labelH = H * 0.015f * 1.4f;

    // priority buttons — 3 fields + labels down from form top
    float prioY = formY + H * 0.65f * 0.14f + labelH + gap * 2 + labelH;
    float prioBtnW = fieldW / 3.f - 4.f;
    for (int i = 0; i < 3; i++) {
        sf::FloatRect r({fieldX + i * (prioBtnW + 4.f), prioY}, {prioBtnW, fieldH});
        if (r.contains(mouse)) { _addTaskForm.selectedPriority = i + 1; return; }
    }

    // type buttons
    float typeY    = prioY + gap + labelH;
    float typeBtnW = fieldW / 4.f - 4.f;
    for (int i = 0; i < 4; i++) {
        sf::FloatRect r({fieldX + i * (typeBtnW + 4.f), typeY}, {typeBtnW, fieldH});
        if (r.contains(mouse)) { _addTaskForm.selectedType = i; return; }
    }

    // cancel
    if (_addTaskForm.cancelBtn.contains(mouse)) {
        _addTaskForm.visible = false;
        _editMode            = false;
        _addTaskForm.nameInput.value.clear();
        _addTaskForm.depsInput.value.clear();
        _addTaskForm.estTimeInput.value.clear();
        return;
    }

    // confirm
    if (_addTaskForm.confirmBtn.contains(mouse) && !_addTaskForm.nameInput.value.empty()) {
        _addTaskForm.visible = false;
        _pendingTask         = true;
    }
}

bool Renderer::wantsAddTask() {
    return _pendingTask;
}

std::shared_ptr<Task> Renderer::getNewTask() {
    _pendingTask = false;

    // parse dependencies
    std::vector<int> deps;
    std::stringstream ss(_addTaskForm.depsInput.value);
    std::string token;
    while (std::getline(ss, token, ',')) {
        try { deps.push_back(std::stoi(token)); }
        catch (...) {}
    }

    // parse est time
    int estTime = 5;
    try { estTime = std::stoi(_addTaskForm.estTimeInput.value); }
    catch (...) {}

    std::string name = _addTaskForm.nameInput.value;
    int priority = _addTaskForm.selectedPriority;
    int id = _editMode ? _editTaskId : _nextTaskId++;

    // clear form
    _addTaskForm.nameInput.value.clear();
    _addTaskForm.depsInput.value.clear();
    _addTaskForm.estTimeInput.value.clear();

    // create correct task type
    std::shared_ptr<Task> task;
    switch (_addTaskForm.selectedType) {
        case 0: task = std::make_shared<ComputationTask>(id, name, deps); break;
        case 1: task = std::make_shared<IOTask>(id, name, deps);          break;
        case 2: task = std::make_shared<FileCheckTask>(id, name, deps);   break;
        case 3: task = std::make_shared<RenderTask>(id, name, deps);      break;
        default: task = std::make_shared<ComputationTask>(id, name, deps);
    }
    task->setEstimatedTime(estTime);
    task->setPriority(priority);
    return task;
}
