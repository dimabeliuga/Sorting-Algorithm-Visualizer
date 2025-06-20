cmake_minimum_required(VERSION 3.15)

project(SIMPLE_SORT_VISUALIZER)

find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIRECTORY})

add_executable(${CMAKE_PROJECT_NAME} src/visualizer.cpp)

target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE 
            ${CURSES_LIBRARIES}
)

