#include "Task.h"

Task::Task(const int id, std::string name, const int priority, std::vector<int> dependencies)
    :
    _id(id),
    _name(std::move(name)),
    _priority(priority),
    _dependencies(std::move(dependencies))
{}
