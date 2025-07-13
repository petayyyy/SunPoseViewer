cmake_minimum_required(VERSION 3.16)
project(SunPoseViewer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Явно указываем использовать Qt5
set(QT_VERSION_MAJOR 5)  # Форсируем использование Qt5

# Указываем Qt5 модули
# find_package(Qt5 REQUIRED COMPONENTS Core Widgets OpenGL Gui OpenGLWidgets Positioning)
# find_package(Qt5 REQUIRED COMPONENTS Core Widgets OpenGL Gui OpenGLWidgets)
find_package(Qt5 REQUIRED COMPONENTS Core Widgets OpenGL Gui)

# Добавляем пути для поиска OpenGL
find_package(OpenGL REQUIRED)

# Задаем пути для исходников
set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/libs)
set(MODEL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/models)

# Источники
set(SOURCES
    ${SOURCE_DIR}/controllers/main.cpp

    ${SOURCE_DIR}/views/ResultsWindow.cpp
    ${SOURCE_DIR}/views/SunVisualizer.cpp
    ${SOURCE_DIR}/views/SunWidget.cpp
    ${SOURCE_DIR}/views/stb_image.cpp
    ${SOURCE_DIR}/views/MainWindow.cpp

    ${MODEL_DIR}/SunPositionCalculator.cpp

    ${LIB_DIR}/APC_VecMat3D.cpp
    ${LIB_DIR}/APC_Cheb.cpp
    ${LIB_DIR}/APC_DE.cpp
    ${LIB_DIR}/APC_IO.cpp
    ${LIB_DIR}/APC_Kepler.cpp
    ${LIB_DIR}/APC_Math.cpp
    ${LIB_DIR}/APC_Moon.cpp
    ${LIB_DIR}/APC_Phys.cpp
    ${LIB_DIR}/APC_Planets.cpp
    ${LIB_DIR}/APC_PrecNut.cpp
    ${LIB_DIR}/APC_Spheric.cpp
    ${LIB_DIR}/APC_Sun.cpp
    ${LIB_DIR}/APC_Time.cpp
    # ${LIB_DIR}/MockPositioner.cpp
)

# Заголовки
set(HEADERS
    ${INCLUDE_DIR}/ResultsWindow.h
    ${INCLUDE_DIR}/SunVisualizer.h
    ${INCLUDE_DIR}/SunWidget.h
    ${INCLUDE_DIR}/stb_image.h
    ${INCLUDE_DIR}/MainWindow.h

    ${MODEL_DIR}/SunPositionCalculator.h

    ${LIB_DIR}/APC_VecMat3D.h
    ${LIB_DIR}/APC_Cheb.h
    ${LIB_DIR}/APC_Const.h
    ${LIB_DIR}/APC_DE.h
    ${LIB_DIR}/APC_IO.h
    ${LIB_DIR}/APC_Kepler.h
    ${LIB_DIR}/APC_Math.h
    ${LIB_DIR}/APC_Moon.h
    ${LIB_DIR}/APC_Phys.h
    ${LIB_DIR}/APC_Planets.h
    ${LIB_DIR}/APC_PrecNut.h
    ${LIB_DIR}/APC_Spheric.h
    ${LIB_DIR}/APC_Sun.h
    ${LIB_DIR}/APC_Time.h
    ${LIB_DIR}/GNU_iomanip.h
    # ${LIB_DIR}/Positioner.h
)

# UI файлы
set(UI_FILES
    ${INCLUDE_DIR}/MainWindow.ui
)

# Генерация UI
Qt5_wrap_ui(UI_HEADERS ${UI_FILES})

# Создание исполняемого файла
add_executable(SunPoseViewer
    ${SOURCES}
    ${HEADERS}
    ${UI_FILES}
    # ${LIB_DIR}
    # ${MODEL_DIR}
)

# Устанавливаем пути для include-файлов
target_include_directories(SunPoseViewer PRIVATE
    ${INCLUDE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${LIB_DIR}
    ${MODEL_DIR}
)

# Линковка библиотек
target_link_libraries(SunPoseViewer
    Qt5::Core
    Qt5::Widgets
    Qt5::Gui
    Qt5::OpenGL
    # Qt5::OpenGLWidgets
    # Qt5::Positioning
    OpenGL::GL
    OpenGL::GLU
)