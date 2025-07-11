# cmake_minimum_required(VERSION 3.16)
# project(SunPoseViewer)

# set(CMAKE_CXX_STANDARD 17)
# set(CMAKE_AUTOMOC ON)
# set(CMAKE_AUTORCC ON)
# set(CMAKE_AUTOUIC ON)

# # Указываем Qt6 модули
# find_package(Qt6 REQUIRED COMPONENTS Core Widgets OpenGL Gui OpenGLWidgets)
# # find_package(Qt6 REQUIRED COMPONENTS Core Widgets OpenGL Gui)

# # Заголовочные файлы
# include_directories(include)

# # Источники
# set(SOURCES
#     src/main.cpp
#     src/SunVisualizer.cpp
#     src/SunWidget.cpp
#     src/stb_image.cpp
#     src/MainWindow.cpp
# )

# # Заголовки
# set(HEADERS
#     include/SunVisualizer.h
#     include/SunWidget.h
#     include/stb_image.h
#     src/MainWindow.h
# )

# # UI файлы
# set(UIS
#     src/MainWindow.ui
# )

# # Генерация UI
# qt6_wrap_ui(UIS_HEADERS ${UIS})

# # Создание исполняемого файла
# add_executable(SunPoseViewer
#     ${SOURCES}
#     ${HEADERS}
#     ${UIS}
# )

# # Линковка Qt6
# target_link_libraries(SunPoseViewer
#     Qt6::Core
#     Qt6::Widgets
#     Qt6::Gui
#     Qt6::OpenGL
#     Qt6::OpenGLWidgets
#     ${OPENGL_gl_LIBRARY}
#     ${OPENGL_glu_LIBRARY}  # <--- Это добавляет поддержку GLU
# )

cmake_minimum_required(VERSION 3.16)
project(SunPoseViewer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Указываем Qt6 модули
find_package(Qt6 REQUIRED COMPONENTS Core Widgets OpenGL Gui OpenGLWidgets)

# Добавляем пути для поиска OpenGL
find_package(OpenGL REQUIRED)

# Задаем пути для исходников
set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Источники
set(SOURCES
    ${SOURCE_DIR}/main.cpp
    ${SOURCE_DIR}/SunVisualizer.cpp
    ${SOURCE_DIR}/SunWidget.cpp
    ${SOURCE_DIR}/stb_image.cpp
    ${SOURCE_DIR}/MainWindow.cpp
)

# Заголовки
set(HEADERS
    ${INCLUDE_DIR}/SunVisualizer.h
    ${INCLUDE_DIR}/SunWidget.h
    ${INCLUDE_DIR}/stb_image.h
    ${INCLUDE_DIR}/MainWindow.h
)

# UI файлы
set(UI_FILES
    ${INCLUDE_DIR}/MainWindow.ui
)

# Генерация UI
qt6_wrap_ui(UI_HEADERS ${UI_FILES})

# Создание исполняемого файла
add_executable(SunPoseViewer
    ${SOURCES}
    ${HEADERS}
    ${UI_HEADERS}
)

# Устанавливаем пути для include-файлов
target_include_directories(SunPoseViewer PRIVATE
    ${INCLUDE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}  # Для сгенерированных ui_*.h файлов
)

# Линковка библиотек
target_link_libraries(SunPoseViewer
    Qt6::Core
    Qt6::Widgets
    Qt6::Gui
    Qt6::OpenGL
    Qt6::OpenGLWidgets
    OpenGL::GL
    OpenGL::GLU
)