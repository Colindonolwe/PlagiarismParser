cmake_minimum_required(VERSION 3.13)
project(qtCPPchecker)

set(CMAKE_CXX_STANDARD 11)

add_executable(qtCPPchecker main.cpp)
# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed
set(CMAKE_AUTOMOC ON)
# Find the QtWidgets library
find_package(Qt5Core REQUIRED)
find_package(Qt5Widgets REQUIRED)
# find_package(QRegExp CONFIG REQUIRED)
include_directories(${Qt5Widgets_INCLUDE_DIRS})

target_link_libraries(qtCPPchecker Qt5::Widgets)