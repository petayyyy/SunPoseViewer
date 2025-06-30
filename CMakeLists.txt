cmake_minimum_required(VERSION 3.16)
project(SunPoseViewer VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# find_package(Qt6 REQUIRED COMPONENTS Core Widgets OpenGL Gui)
find_package(Qt6 REQUIRED COMPONENTS Core Widgets OpenGL)
find_package(OpenGL REQUIRED)

qt_standard_project_setup()
# qt6_wrap_ui(UISrcs ui/MainWindow.ui)

qt_add_executable(${PROJECT_NAME}
    src/main.cpp
    src/MainWindow.cpp
    src/AstronomyCalculator.cpp
    src/SunWidget.cpp
    ui/MainWindow.ui
)

target_link_libraries(${PROJECT_NAME}
    # PRIVATE Qt6::Core Qt6::Widgets Qt6::Gui Qt6::OpenGL OpenGL::GL
    PRIVATE Qt6::Core Qt6::Widgets Qt6::OpenGL OpenGL::GL
)