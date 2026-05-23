cmake_minimum_required(VERSION 3.30)
project(MultithreadedTaskScheduler)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SFML 3.0.2 REQUIRED COMPONENTS Graphics Window System)

add_executable(${PROJECT_NAME}
        src/main.cpp
        src/Task.cpp
        include/Task.h
        src/ComputationTask.cpp
        include/ComputationTask.h
        src/Scheduler.cpp
        include/Scheduler.h
        src/ThreadPool.cpp
        include/ThreadPool.h
        src/Engine.cpp
        include/Engine.h
        src/Renderer.cpp
        include/Renderer.h
        src/IOTask.cpp
        include/IOTask.h
        src/FileCheckTask.cpp
        include/FileCheckTask.h
        src/RenderTask.cpp
        include/RenderTask.h)


file(COPY ${CMAKE_SOURCE_DIR}/assets DESTINATION ${CMAKE_BINARY_DIR})

target_include_directories(${PROJECT_NAME} PRIVATE include)

target_link_libraries(${PROJECT_NAME}
        SFML::Graphics
        SFML::Window
        SFML::System
)