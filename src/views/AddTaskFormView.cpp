
#include "views/AddTaskFormView.h"
#include "tasks/ComputationTask.h"
#include "tasks/IOTask.h"
#include "tasks/FileCheckTask.h"
#include "tasks/RenderTask.h"

AddTaskFormView::AddTaskFormView(sf::RenderWindow& window, sf::Font& font)
    : _window(window), _font(font) {}

void AddTaskFormView::clearForm() {
    _nameInput.clear();
    _depsInput.clear();
    _estTimeInput.clear();
    _selectedPriority = 2;
    _selectedType = 0;
}

void AddTaskFormView::openForAdd() {
    clearForm();
    _editMode = false;
    _visible = true;
}

void AddTaskFormView::openForEdit(const std::shared_ptr<Task>& task) {
    clearForm();
    _nameInput.value = task->getName();
    _selectedPriority = task->getPriority();

    std::string depsStr;
    for (int d : task->getDependencies())
        depsStr += std::to_string(d) + ",";
    if (!depsStr.empty()) depsStr.pop_back();
    _depsInput.value = depsStr;

    _editMode = true;
    _editTaskId = task->getId();
    _visible = true;
}

void AddTaskFormView::handleTextInput(uint32_t unicode) {
    _nameInput.handleChar(unicode);
    _depsInput.handleChar(unicode);
}

void AddTaskFormView::handleClick(sf::Vector2f mouse) {
    if (!_visible) return;
    float W = _window.getSize().x;
    float H = _window.getSize().y;

    float formW = W * 0.35f;
    float formH = H * 0.65f;
    float formX = (W - formW) * 0.5f;
    float formY = (H - formH) * 0.5f;

    float fieldX = formX + formW * 0.06f;
    float fieldW = formW * 0.88f;
    float fieldH = H * 0.048f;

    float gap = H * 0.085f;
    float labelH = H * 0.015f * 1.4f;
    float curY = formY + formH * 0.14f;

    // text input focus
    _nameInput.focused = _nameInput.bounds.contains(mouse);
    _depsInput.focused = _depsInput.bounds.contains(mouse);

    // skip to priority Y — 3 inputs down
    float prioY = curY + gap * 2 + labelH;

    // priority buttons
    float prioBtnW = fieldW / 3.f - 4.f;
    for (int i = 0; i < 3; i++) {
        sf::FloatRect r({fieldX + i * (prioBtnW + 4.f), prioY}, {prioBtnW, fieldH});
        if (r.contains(mouse)) { _selectedPriority = i + 1; return; }

    }

    // type buttons
    float typeY = prioY + gap + labelH;
    float typeBtnW = fieldW / 4.f - 4.f;
    for (int i = 0; i < 4; i++) {
        sf::FloatRect r({fieldX + i * (typeBtnW + 4.f), typeY}, {typeBtnW, fieldH});
        if (r.contains(mouse)) { _selectedType = i; return; }
    }

    if (_cancelBtn.contains(mouse)) {
        _visible = false;
        _editMode =false;
        clearForm();
        return;
    }

    if (_confirmBtn.contains(mouse) && !_nameInput.value.empty()) {
        _visible = false;
        _pendingTask = true;
    }
}

std::shared_ptr<Task> AddTaskFormView::consumeTask(int& nextId) {
    _pendingTask = false;

    std::vector<int> deps;
    std::stringstream ss(_depsInput.value);
    std::string token;
    while (std::getline(ss, token, ',')) {
        try { deps.push_back(std::stoi(token)); }
        catch (...) {}
    }


    int id = _editMode ? _editTaskId : nextId++;
    std::string name = _nameInput.value;
    int priority = _selectedPriority;

    clearForm();

    std::shared_ptr<Task> task;
    switch (_selectedType) {
        case 0: task = std::make_shared<ComputationTask>(id, name, deps);break;
        case 1: task = std::make_shared<IOTask>(id, name, deps);break;
        case 2: task = std::make_shared<FileCheckTask>(id, name, deps);break;
        case 3: task = std::make_shared<RenderTask>(id, name, deps);break;
        default: task = std::make_shared<ComputationTask>(id, name, deps);
    }
    task->setPriority(priority);

    _editMode = false;
    _editTaskId = -1;
    return task;
}

void AddTaskFormView::drawInput(const std::string& label, TextInput& input,
                                float fieldX, float fieldW, float fieldH,
                                float labelSize, float y) {
    sf::Text lbl(_font, label);
    lbl.setCharacterSize(labelSize);
    lbl.setFillColor(sf::Color(140, 140, 160));
    lbl.setPosition({fieldX, y});
    _window.draw(lbl);

    float inputY = y + labelSize * 1.4f;
    input.bounds = sf::FloatRect({fieldX, inputY}, {fieldW, fieldH});

    sf::RectangleShape bg({fieldW, fieldH});
    bg.setFillColor(sf::Color(20, 20, 35));
    bg.setOutlineThickness(1);
    bg.setOutlineColor(input.focused ? sf::Color(100, 100, 255) : sf::Color(55, 55, 80));
    bg.setPosition({fieldX, inputY});
    _window.draw(bg);

    sf::Text val(_font, input.value + (input.focused ? "|" : ""));
    val.setCharacterSize(labelSize);
    val.setFillColor(sf::Color::White);
    val.setPosition({fieldX + fieldW * 0.03f, inputY + fieldH * 0.2f});
    _window.draw(val);
}

void AddTaskFormView::draw() {
    if (!_visible) return;

    float W = _window.getSize().x;
    float H = _window.getSize().y;
    float formW = W * 0.35f;
    float formH = H * 0.65f;
    float formX = (W - formW) * 0.5f;
    float formY = (H - formH) * 0.5f;
    float fieldX = formX + formW * 0.06f;
    float fieldW = formW * 0.88f;
    float fieldH = H * 0.048f;
    float labelSize = H * 0.015f;
    float gap = H * 0.085f;
    float curY = formY + formH * 0.14f;

    // overlay
    sf::RectangleShape overlay({W, H});
    overlay.setFillColor(sf::Color(0, 0, 0, 160));
    overlay.setPosition({0, 0});
    _window.draw(overlay);

    // form box
    sf::RectangleShape formBg({formW, formH});
    formBg.setFillColor(sf::Color(28, 28, 45));
    formBg.setOutlineThickness(1);
    formBg.setOutlineColor(sf::Color(60, 60, 100));
    formBg.setPosition({formX, formY});
    _window.draw(formBg);

    sf::Text title(_font, _editMode ? "Edit Task" : "Add New Task");
    title.setCharacterSize(H * 0.022f);
    title.setFillColor(sf::Color::White);
    title.setPosition({fieldX, formY + formH * 0.04f});
    _window.draw(title);

    sf::RectangleShape titleDiv({formW - 2, 1});
    titleDiv.setFillColor(sf::Color(60, 60, 80));
    titleDiv.setPosition({formX + 1, formY + formH * 0.11f});
    _window.draw(titleDiv);

    // inputs
    drawInput("Task Name", _nameInput, fieldX, fieldW, fieldH, labelSize, curY);
    curY += gap;
    drawInput("Dependencies (comma separated IDs, e.g. 1,3)", _depsInput, fieldX, fieldW, fieldH, labelSize, curY);
    curY += gap;

    // priority
    sf::Text prioLabel(_font, "Priority");
    prioLabel.setCharacterSize(labelSize);
    prioLabel.setFillColor(sf::Color(140, 140, 160));
    prioLabel.setPosition({fieldX, curY});
    _window.draw(prioLabel);
    curY += labelSize * 1.4f;

    float prioBtnW = fieldW / 3.f - 4.f;
    float bX = fieldX;
    for (auto& [name, val] : std::vector<std::pair<std::string, int>>{{"Low", 1}, {"Medium", 2}, {"High", 3}}) {
        sf::RectangleShape btn({prioBtnW, fieldH});
        btn.setFillColor(_selectedPriority == val ? sf::Color(60, 80, 180) : sf::Color(30, 30, 50));
        btn.setOutlineThickness(1);
        btn.setOutlineColor(sf::Color(60, 60, 100));
        btn.setPosition({bX, curY});
        _window.draw(btn);

        sf::Text txt(_font, name);
        txt.setCharacterSize(labelSize);
        txt.setFillColor(sf::Color::White);
        txt.setPosition({bX + (prioBtnW - txt.getLocalBounds().size.x) * 0.5f, curY + fieldH * 0.2f});
        _window.draw(txt);
        bX += prioBtnW + 4.f;
    }
    curY += gap;

    // type
    sf::Text typeLabel(_font, "Task Type");
    typeLabel.setCharacterSize(labelSize);
    typeLabel.setFillColor(sf::Color(140, 140, 160));
    typeLabel.setPosition({fieldX, curY});
    _window.draw(typeLabel);
    curY += labelSize * 1.4f;

    float typeBtnW = fieldW / 4.f - 4.f;
    float tX = fieldX;
    for (int i = 0; i < 4; i++) {
        std::string name = std::vector<std::string>{"Computation", "IO", "FileCheck", "Render"}[i];
        sf::RectangleShape btn({typeBtnW, fieldH});
        btn.setFillColor(_selectedType == i ? sf::Color(60, 80, 180) : sf::Color(30, 30, 50));
        btn.setOutlineThickness(1);
        btn.setOutlineColor(sf::Color(60, 60, 100));
        btn.setPosition({tX, curY});
        _window.draw(btn);

        sf::Text txt(_font, name);
        txt.setCharacterSize(labelSize * 0.85f);
        txt.setFillColor(sf::Color::White);
        txt.setPosition({tX + (typeBtnW - txt.getLocalBounds().size.x) * 0.5f, curY + fieldH * 0.2f});
        _window.draw(txt);
        tX += typeBtnW + 4.f;
    }
    curY += gap;

    // cancel / confirm
    float cbtnW = fieldW * 0.45f;
    float confirmX = fieldX + cbtnW + fieldW * 0.1f;
    _cancelBtn = sf::FloatRect({fieldX, curY}, {cbtnW, fieldH});
    _confirmBtn = sf::FloatRect({confirmX, curY}, {cbtnW, fieldH});

    sf::RectangleShape cancelBg({cbtnW, fieldH});
    cancelBg.setFillColor(sf::Color(120, 40, 40));
    cancelBg.setPosition({fieldX, curY});
    _window.draw(cancelBg);
    sf::Text cancelTxt(_font, "Cancel");
    cancelTxt.setCharacterSize(labelSize);
    cancelTxt.setFillColor(sf::Color::White);
    cancelTxt.setPosition({fieldX + (cbtnW - cancelTxt.getLocalBounds().size.x) * 0.5f, curY + fieldH * 0.2f});
    _window.draw(cancelTxt);

    sf::RectangleShape confirmBg({cbtnW, fieldH});
    confirmBg.setFillColor(sf::Color(40, 120, 40));
    confirmBg.setPosition({confirmX, curY});
    _window.draw(confirmBg);
    sf::Text confirmTxt(_font, _editMode ? "Save Changes" : "Add Task");
    confirmTxt.setCharacterSize(labelSize);
    confirmTxt.setFillColor(sf::Color::White);
    confirmTxt.setPosition({confirmX + (cbtnW - confirmTxt.getLocalBounds().size.x) * 0.5f, curY + fieldH * 0.2f});
    _window.draw(confirmTxt);
}