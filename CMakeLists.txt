cmake_minimum_required(VERSION 3.16)
project(FukoEDisplay VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Svg)

qt_standard_project_setup()

add_executable(fuko_e_display
    src/main.cpp
    src/topology_types.h
    src/mock_data.h
    src/mock_data.cpp
    src/topology_scene.h
    src/topology_scene.cpp
    src/mainwindow.h
    src/mainwindow.cpp
)

target_link_libraries(fuko_e_display PRIVATE Qt6::Widgets Qt6::Svg)
