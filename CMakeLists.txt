cmake_minimum_required(VERSION 3.30)
project(MultithreadedTaskScheduler)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SFML 3.0.2 REQUIRED COMPONENTS Graphics Window System)

add_executable(${PROJECT_NAME}
        src/main.cpp
        src/Task.cpp
        include/Task.h
        src/tasks/ComputationTask.cpp
        include/tasks/ComputationTask.h
        src/Scheduler.cpp
        include/Scheduler.h
        src/ThreadPool.cpp
        include/ThreadPool.h
        src/Engine.cpp
        include/Engine.h
        src/Renderer.cpp
        include/Renderer.h
        src/tasks/IOTask.cpp
        include/tasks/IOTask.h
        src/tasks/FileCheckTask.cpp
        include/tasks/FileCheckTask.h
        src/tasks/RenderTask.cpp
        include/tasks/RenderTask.h
        include/views/ThreadView.h
        include/views/GanttGraphView.h
        include/views/DependenciesGraphView.h
        include/views/TaskDetailsView.h
        include/views/TaskQueueView.h
        include/views/StatsBar.h
        src/views/ThreadView.cpp
        src/views/GanttGraphView.cpp
        src/views/DependenciesGraphView.cpp
        src/views/TaskQueueView.cpp
        src/views/TaskDetailsView.cpp
        src/views/StatsBar.cpp
        src/UIHelpers.cpp
        include/UIHelpers.h
        src/views/AddTaskFormView.cpp
        include/views/AddTaskFormView.h
        src/PresetLoader.cpp
        include/PresetLoader.h
        src/views/PresetSelectorView.cpp
        include/views/PresetSelectorView.h
)


file(COPY ${CMAKE_SOURCE_DIR}/assets DESTINATION ${CMAKE_BINARY_DIR})
file(COPY ${CMAKE_SOURCE_DIR}/presets DESTINATION ${CMAKE_BINARY_DIR})

target_include_directories(${PROJECT_NAME} PRIVATE include)

target_link_libraries(${PROJECT_NAME}
        SFML::Graphics
        SFML::Window
        SFML::System
)