#pragma once
#include <string>
#include <vector>
#include <atomic>

/**
 * @brief Abstract base class for all schedulable work units.
 *
 * Subclasses implement execute() with the actual work. The scheduler
 * manages lifecycle transitions and thread assignment.
 */
class Task {
public:
    /// @brief Lifecycle states a task moves through during scheduling.
    enum class TaskStatus {
        Planned,
        Queued,
        Running,
        Completed,
        Cancelled,
    };

protected:
    int _id;
    std::string _name;
    int _priority;
    std::atomic<int> _progress;
    std::atomic<TaskStatus> _status = TaskStatus::Planned;
    std::vector<int> _dependencies;
    int _estTime = 5;
    std::chrono::steady_clock::time_point _startTime;
    std::chrono::steady_clock::time_point _endTime;
    bool _hasStarted = false;
    bool _hasEnded = false;
    int _assignedThread = -1;
    std::atomic<bool> _cancelled = false;

public:

    /// @brief Construct a task with scheduling metadata.
    /// @param id           Unique identifier used for dependency references.
    /// @param name         Human-readable label shown in the UI.
    /// @param priority     Scheduling priority (1=low, 2=medium, 3=high).
    /// @param dependencies IDs of tasks that must complete before this one runs.
    Task(int id, std::string name, int priority, std::vector<int> dependencies);
    virtual ~Task() = default;

    /// @brief Execute the task's work on the calling worker thread.
    virtual void execute() = 0;

    /// @brief Return the unique task ID.
    int getId() const { return _id; }

    /// @brief Return the human-readable task name.
    std::string getName() const { return _name; }

    /// @brief Return the current lifecycle status.
    TaskStatus getStatus() const { return _status; }

    /// @brief Set the lifecycle status.
    /// @param s New status value.
    void setStatus(TaskStatus s) { _status = s; }

    /// @brief Return execution progress as a percentage (0–100).
    int getProgress() const { return _progress; }

    /// @brief Return the scheduling priority.
    int getPriority() const { return _priority; }

    /// @brief Override the scheduling priority.
    /// @param p New priority value.
    void setPriority(int p) { _priority = p; }

    /// @brief Return the IDs of tasks this task depends on.
    const std::vector<int>& getDependencies() const { return _dependencies; }

    /// @brief Return the estimated execution time in seconds.
    int getEstimatedTime() const { return _estTime; }

    /// @brief Record the wall-clock start time.
    void markStarted() {
        _startTime = std::chrono::steady_clock::now();
        _hasStarted = true;
    }

    /// @brief Record the wall-clock end time.
    void markEnded() {
        _endTime = std::chrono::steady_clock::now();
        _hasEnded = true;
    }

    /// @brief Return true if execution has begun.
    bool hasStarted() const { return _hasStarted; }

    /// @brief Return true if execution has finished.
    bool hasEnded()   const { return _hasEnded; }

    /// @brief Return the recorded start time.
    std::chrono::steady_clock::time_point getStartTime() const { return _startTime; }

    /// @brief Return the recorded end time.
    std::chrono::steady_clock::time_point getEndTime()   const { return _endTime; }

    /// @brief Record which worker thread is executing this task.
    /// @param threadId Index of the assigned worker.
    void setAssignedThread(int threadId) { _assignedThread = threadId; }

    /// @brief Return the ID of the worker thread executing this task, or -1 if unassigned.
    int  getAssignedThread() const { return _assignedThread; }

    /// @brief Signal the task to stop at its next cancellation checkpoint.
    void cancel() { _cancelled = true; }

    /// @brief Replace the dependency list.
    /// @param deps New list of dependency task IDs.
    void setDependencies(std::vector<int> deps) { _dependencies = std::move(deps); }
};
