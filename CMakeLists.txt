cmake_minimum_required(VERSION 3.16)
project(SunPoseViewer VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories(${PROJECT_SOURCE_DIR}/src)

set(HEADER_FILES
    src/libs/APC_Cheb.h
    src/libs/APC_Const.h
    src/libs/APC_DE.h
    src/libs/APC_IO.h
    src/libs/APC_Kepler.h
    src/libs/APC_Math.h
    src/libs/APC_Moon.h
    src/libs/APC_Phys.h
    src/libs/APC_Planets.h
    src/libs/APC_PrecNut.h
    src/libs/APC_Spheric.h
    src/libs/APC_Sun.h
    src/libs/APC_Time.h
    src/libs/GNU_iomanip.h
    src/libs/APC_VecMat3D.h
    src/models/SunPositionCalculator.h
    src/tests/SunCalculationTests.h
)

# Исходные файлы
set(SOURCE_FILES  
    src/libs/APC_VecMat3D.cpp
    src/libs/APC_Cheb.cpp
    src/libs/APC_DE.cpp
    src/libs/APC_IO.cpp
    src/libs/APC_Kepler.cpp
    src/libs/APC_Math.cpp
    src/libs/APC_Moon.cpp
    src/libs/APC_Phys.cpp
    src/libs/APC_Planets.cpp
    src/libs/APC_PrecNut.cpp
    src/libs/APC_Spheric.cpp
    src/libs/APC_Sun.cpp
    src/libs/APC_Time.cpp
    src/libs/APC_Spheric.cpp
    src/libs/APC_VecMat3D.cpp
    src/models/SunPositionCalculator.cpp
    src/tests/SunCalculationTests.cpp
    src/tests/runTests.cpp
)

add_executable(runTests ${HEADER_FILES} ${SOURCE_FILES})