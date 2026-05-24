#include "views/StatsBar.h"
#include "UIHelpers.h"

StatsBar::StatsBar(sf::RenderWindow& window, sf::Font& font, Scheduler& scheduler)
    : _window(window), _font(font), _scheduler(scheduler) {}

void StatsBar::draw() {
    float W = _window.getSize().x;
    float H = _window.getSize().y;

    float barH = H * 0.07f;
    float barY = H - barH;

    sf::RectangleShape bg({W, barH});
    bg.setFillColor(sf::Color(18, 18, 30));
    bg.setPosition({0, barY});
    _window.draw(bg);

    sf::RectangleShape divider({W, 1});
    divider.setFillColor(sf::Color(60, 60, 80));
    divider.setPosition({0, barY});
    _window.draw(divider);

    const auto& tasks = _scheduler.getTasks();

    int total = tasks.size();
    int running = 0;
    int queued = 0;
    int completed = 0;
    int planned = 0;

    for (const auto& [id, task] : tasks) {
        switch (task->getStatus()) {
            case Task::TaskStatus::Running: running++;break;
            case Task::TaskStatus::Completed: completed++;break;
            case Task::TaskStatus::Planned: planned++; break;
            case Task::TaskStatus::Queued: queued++; break;

            default: break;
        }
    }

    float labelSize = H * 0.015f;
    float centerY= barY + barH * 0.5f - labelSize * 0.5f;
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
        cursorX += val.getLocalBounds().size.x + W * 0.015f;

    };

    drawStat("Total",total, sf::Color::White);
    drawStat("Running",running, sf::Color(0,   200, 100));
    drawStat("Queued",queued,sf::Color(255, 165, 0));
    drawStat("Completed",completed,sf::Color(100, 100, 255));
    drawStat("Planned",planned,sf::Color(120, 120, 120));

    unsigned int maxThreads = std::max(1u, std::thread::hardware_concurrency() - 1);
    unsigned int curThreads = _scheduler.getWorkers().size();

    sf::Text threadLabel(_font, "Threads: " + std::to_string(curThreads));
    threadLabel.setCharacterSize(labelSize);
    threadLabel.setFillColor(sf::Color(120, 120, 120));
    threadLabel.setPosition({cursorX, centerY});
    _window.draw(threadLabel);
    cursorX += threadLabel.getLocalBounds().size.x + 8;

    float smallBtnW = H * 0.035f;
    float smallBtnH = barH * 0.55f;
    float smallBtnY = barY + barH * 0.22f;

    bool canDecrease = !_started && curThreads > 1;
    bool canIncrease = !_started && curThreads < maxThreads;

    _threadMinusBtn = sf::FloatRect({cursorX, smallBtnY}, {smallBtnW, smallBtnH});
    sf::RectangleShape minusBg({smallBtnW, smallBtnH});
    minusBg.setFillColor(canDecrease ? sf::Color(80, 40, 40) : sf::Color(40, 40, 40));
    minusBg.setPosition({cursorX, smallBtnY});
    _window.draw(minusBg);
    sf::Text minusTxt(_font, "-");
    minusTxt.setCharacterSize(labelSize);
    minusTxt.setFillColor(canDecrease ? sf::Color::White : sf::Color(80, 80, 80));
    minusTxt.setPosition({cursorX + (smallBtnW - minusTxt.getLocalBounds().size.x) * 0.5f,
                          smallBtnY + (smallBtnH - labelSize) * 0.4f});
    _window.draw(minusTxt);
    cursorX += smallBtnW + 2;
    _threadPlusBtn = sf::FloatRect({cursorX, smallBtnY}, {smallBtnW, smallBtnH});
    sf::RectangleShape plusBg({smallBtnW, smallBtnH});
    plusBg.setFillColor(canIncrease ? sf::Color(40, 80, 40) : sf::Color(40, 40, 40));
    plusBg.setPosition({cursorX, smallBtnY});
    _window.draw(plusBg);
    sf::Text plusTxt(_font, "+");
    plusTxt.setCharacterSize(labelSize);
    plusTxt.setFillColor(canIncrease ? sf::Color::White : sf::Color(80, 80, 80));
    plusTxt.setPosition({cursorX + (smallBtnW - plusTxt.getLocalBounds().size.x) * 0.5f,
                         smallBtnY + (smallBtnH - labelSize) * 0.4f});
    _window.draw(plusTxt);
    cursorX += smallBtnW + W * 0.02f;

    float btnW = W * 0.07f;
    float btnH = barH * 0.60f;
    float btnY = barY + barH * 0.20f;
    float btnX = W - (btnW + W * 0.01f) * 4 - W * 0.02f;

    struct Btn { std::string label; sf::Color color; sf::FloatRect* rect; };
    std::vector<Btn> buttons = {
        {"Add Task", sf::Color(60, 60, 160), &_addBtn},
        {"Start", sf::Color(0, 160, 80), &_startBtn},
        {"Pause", sf::Color(180, 140, 0), &_pauseBtn},
        {"Stop", sf::Color(180, 40, 40), &_stopBtn},
    };

    for (auto& btn : getButtons()) {
        *btn.rect = sf::FloatRect({btnX, btnY}, {btnW, btnH});

        sf::RectangleShape bg({btnW, btnH});
        bg.setFillColor(btn.color);
        bg.setPosition({btnX, btnY});
        _window.draw(bg);

        sf::Text label(_font, btn.label);
        label.setCharacterSize(labelSize * 0.85f);
        label.setFillColor(sf::Color::White);
        label.setPosition({btnX + (btnW - label.getLocalBounds().size.x) * 0.5f,
                           btnY + (btnH - labelSize) * 0.5f});
        _window.draw(label);

        btnX += btnW + W * 0.01f;
    }
}
