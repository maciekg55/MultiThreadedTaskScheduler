#include "Renderer.h"

Renderer::Renderer(sf::RenderWindow& window, Scheduler& scheduler)
    :
    _window(window),
    _scheduler(scheduler),
    _threadView(window, _font, scheduler),
    _ganttView(window, _font, scheduler),
    _dependencyView(window, _font, scheduler),
    _taskQueueView(window, _font, scheduler),
    _taskDetailsView(window, _font, scheduler),
    _statsBar(window, _font, scheduler),
    _formView(window, _font),
    _presetSelector(window, _font)

{
    _font.openFromFile("assets/JetBrainsMono-Regular.ttf");
}

void Renderer::draw() {
    _threadView.draw();
    _ganttView.draw();
    _dependencyView.draw();
    _taskQueueView.draw();
    _taskDetailsView.draw();
    _statsBar.draw();
    _presetSelector.draw();
    _formView.draw();
}

void Renderer::resetStartTime() {
    _ganttView.resetStartTime();
}

void Renderer::handleScroll(float delta, sf::Vector2f mousePos) {
    _threadView.handleScroll(delta, mousePos);
    _taskQueueView.handleScroll(delta, mousePos);
}

void Renderer::handleTextInput(uint32_t unicode) {
    _formView.handleTextInput(unicode);
}

void Renderer::handleClick(sf::Vector2f mousePos) {

    _presetSelector.handleClick(mousePos);

    if (_formView.isVisible()) {
        _formView.handleClick(mousePos);
        return;
    }



    // helper to select/deselect a task in both detail and graph views
    auto selectTask = [&](int id) {
        int current = _taskDetailsView.getSelectedTask();
        int newId = (current == id) ? -1 : id;
        _taskDetailsView.setSelectedTask(newId);
        _dependencyView.setSelectedTask(newId);
    };

    // task row selection in queue panel
    for (const auto& [id, rect] : _taskQueueView.getTaskRowRects()) {
        if (rect.contains(mousePos)) { selectTask(id); return; }
    }

    // node click in dependency graph
    for (const auto& [id, rect] : _dependencyView.getNodeRects()) {
        if (rect.contains(mousePos)) { selectTask(id); return; }
    }

    // delete button
    if (_taskDetailsView.getDeleteBtn() != sf::FloatRect{} &&
        _taskDetailsView.getDeleteBtn().contains(mousePos)) {
        _deleteTaskId = _taskDetailsView.getSelectedTask();
        _pendingDelete = true;
        _taskDetailsView.setSelectedTask(-1);
        _dependencyView.setSelectedTask(-1);
        return;
        }

    // edit button
    if (_taskDetailsView.getEditBtn() != sf::FloatRect{} &&
        _taskDetailsView.getEditBtn().contains(mousePos)) {
        int selectedId = _taskDetailsView.getSelectedTask();
        auto it = _scheduler.getTasks().find(selectedId);
        if (it != _scheduler.getTasks().end())
            _formView.openForEdit(it->second);
        return;
        }

    // add task button
    if (_statsBar.getAddBtn().contains(mousePos)) {
        _formView.openForAdd();
        return;
    }
}

bool Renderer::hasPendingAdd() const {
    return _formView.hasPendingTask() && !_formView.isEditMode();
}

bool Renderer::hasPendingEdit() const {
    return _formView.hasPendingTask() && _formView.isEditMode();
}

int Renderer::hasPendingDelete() {
    if (!_pendingDelete) return -1;
    _pendingDelete = false;
    return _deleteTaskId;
}

int Renderer::getEditTaskId() const {
    return _formView.getEditTaskId();
}

std::shared_ptr<Task> Renderer::consumeTask() {
    return _formView.consumeTask(_nextTaskId);
}