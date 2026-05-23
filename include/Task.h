#pragma once
#include <string>
#include <vector>
#include <atomic>

class Task {
public:
    enum class TaskStatus {
        Planned,
        Waiting,
        Running,
        Completed,
        Cancelled,
    };

protected:
    int _id;
    std::string _name;
    int _priority;
    std::atomic<int> _progress;
    TaskStatus _status;
    std::vector<int> _dependencies; // as id of other tasks
    int _estTime = 5;
    std::chrono::steady_clock::time_point _startTime;
    std::chrono::steady_clock::time_point _endTime;
    bool _hasStarted = false;
    bool _hasEnded   = false;
    int _assignedThread = -1;
    std::atomic<bool> _cancelled = false;

public:

    Task(int id, std::string name, int priority, std::vector<int> dependencies);
    virtual ~Task() = default;
    virtual void execute() = 0;

    int getId() const { return _id; }

    std::string getName() const { return _name; }

    TaskStatus getStatus() const { return _status; }

    void setStatus(TaskStatus s) { _status = s; }

    int getProgress() const { return _progress; }

    int getPriority() const { return _priority; }

    void setEstimatedTime(int t) { _estTime = t; }

    void setPriority(int p)      { _priority = p; }

    const std::vector<int>& getDependencies() const { return _dependencies; }

    int getEstimatedTime() const { return _estTime; }

    void markStarted() {
        _startTime = std::chrono::steady_clock::now();
        _hasStarted = true;
    }
    void markEnded() {
        _endTime = std::chrono::steady_clock::now();
        _hasEnded = true;
    }
    bool hasStarted() const { return _hasStarted; }
    bool hasEnded()   const { return _hasEnded; }

    std::chrono::steady_clock::time_point getStartTime() const { return _startTime; }
    std::chrono::steady_clock::time_point getEndTime()   const { return _endTime; }

    void setAssignedThread(int threadId) { _assignedThread = threadId; }
    int  getAssignedThread() const { return _assignedThread; }

    void cancel() { _cancelled = true; }

    void setDependencies(std::vector<int> deps) { _dependencies = std::move(deps); }




};