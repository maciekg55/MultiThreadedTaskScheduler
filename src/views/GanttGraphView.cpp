#include "views/GanttGraphView.h"
#include "UIHelpers.h"
#include <algorithm>
#include <cmath>

GanttGraphView::GanttGraphView(sf::RenderWindow& window, sf::Font& font, Scheduler& scheduler)
    : _window(window), _font(font), _scheduler(scheduler)
{
    _appStartTime = std::chrono::steady_clock::now();
}

void GanttGraphView::draw() {
    float W = _window.getSize().x;
    float H = _window.getSize().y;

    float panelX = W * 0.15f;
    float panelW = W * 0.55f;
    float panelH = H * 0.50f;

    sf::RectangleShape bg({panelW, panelH});
    bg.setFillColor(sf::Color(22, 22, 35));
    bg.setPosition({panelX, 0});
    _window.draw(bg);

    sf::Text title(_font, "Task Execution Scheduler");
    title.setCharacterSize(H * 0.018f);
    title.setFillColor(sf::Color(180, 180, 180));
    title.setPosition({panelX + panelW * 0.03f, H * 0.02f});
    _window.draw(title);

    sf::RectangleShape divider({panelW - 2, 1});
    divider.setFillColor(sf::Color(60, 60, 80));
    divider.setPosition({panelX + 1, H * 0.055f});
    _window.draw(divider);

    if (!_started) return;

    const auto& workers = _scheduler.getWorkers();
    int   numThreads = workers.size();
    float headerH = H * 0.07f;
    float rowH = (panelH - headerH) / std::max(numThreads, 1);
    float chartX = panelX + panelW * 0.08f;
    float chartW = panelW * 0.88f;
    float chartY = headerH;

    auto now = std::chrono::steady_clock::now();
    float elapsed = std::chrono::duration<float>(now - _appStartTime).count();

    // freeze the timeline at the last task's end time once all tasks are done
    if (_allDone && !_scheduler.getTasks().empty()) {
        float lastEnd = 0.f;
        for (const auto& [id, task] : _scheduler.getTasks()) {
            if (task->hasEnded()) {
                float taskEnd = std::chrono::duration<float>(
                    task->getEndTime() - _appStartTime).count();
                lastEnd = std::max(lastEnd, taskEnd);
            }
        }
        elapsed = lastEnd;
    }

    float windowSecs = std::max(elapsed + 5.f, 5.f);

    int numTicks = 8;
    for (int t = 0; t <= numTicks; t++) {
        float frac = (float)t / numTicks;
        float tickX = chartX + frac * chartW;
        float timeSec = frac * windowSecs;

        sf::RectangleShape tick({1, panelH - headerH + H * 0.01f});
        tick.setFillColor(sf::Color(45, 45, 65));
        tick.setPosition({tickX, chartY - H * 0.01f});
        _window.draw(tick);

        sf::Text timeLabel(_font, std::to_string((int)timeSec) + "s");
        timeLabel.setCharacterSize(H * 0.012f);
        timeLabel.setFillColor(sf::Color(80, 80, 100));
        timeLabel.setPosition({tickX - 8, chartY - H * 0.03f});
        _window.draw(timeLabel);
    }

    for (int i = 0; i < numThreads; i++) {
        float rowY = chartY + i * rowH;

        sf::Text threadLabel(_font, "Thread " + std::to_string(i));
        threadLabel.setCharacterSize(H * 0.013f);
        threadLabel.setFillColor(sf::Color(140, 140, 140));
        threadLabel.setPosition({panelX + panelW * 0.01f, rowY + rowH * 0.35f});
        _window.draw(threadLabel);

        sf::RectangleShape rowDiv({chartW, 1});
        rowDiv.setFillColor(sf::Color(35, 35, 52));
        rowDiv.setPosition({chartX, rowY + rowH - 1});
        _window.draw(rowDiv);

        for (const auto& [id, task] : _scheduler.getTasks()) {
            if (!task->hasStarted()) continue;
            if (task->getAssignedThread() != i) continue;

            float taskStart = std::chrono::duration<float>(
                task->getStartTime() - _appStartTime).count();
            float taskEnd = task->hasEnded()
                ? std::chrono::duration<float>(task->getEndTime() - _appStartTime).count()
                : elapsed;

            float x1 = chartX +(taskStart/windowSecs) * chartW;
            float x2 = chartX +(taskEnd/windowSecs) * chartW;
            x1 = std::max(x1, chartX);
            x2 = std::min(x2, chartX + chartW);
            if (x2 <= x1) continue;

            float barH = rowH * 0.55f;
            float barY = rowY + rowH * 0.22f;

            sf::RectangleShape bar({x2 - x1, barH});
            bar.setFillColor(UI::priorityBgColor(task->getPriority()));
            bar.setOutlineThickness(1);
            bar.setOutlineColor(UI::statusColor(task->getStatus()));
            bar.setPosition({x1, barY});
            _window.draw(bar);

            sf::RectangleShape stripe({3, barH});
            stripe.setFillColor(UI::statusColor(task->getStatus()));
            stripe.setPosition({x1, barY});
            _window.draw(stripe);

            if (x2 - x1 > 30) {
                sf::Text barLabel(_font, task->getName());
                barLabel.setCharacterSize(H * 0.012f);
                barLabel.setFillColor(sf::Color::White);
                barLabel.setPosition({x1 + 4, barY + barH * 0.2f});
                _window.draw(barLabel);
            }
        }
    }

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
