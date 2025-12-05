cmake_minimum_required(VERSION 3.20)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# CUDA
find_package(CUDAToolkit REQUIRED)
set(CMAKE_CUDA_ARCHITECTURES "75")

project(theDAW VERSION 0.1 LANGUAGES CXX CUDA)


# GTEST
include(FetchContent)
FetchContent_declare(
    googletest
    URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)

enable_testing()

# QT
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 COMPONENTS Core Gui Widgets REQUIRED)

include_directories(
    asiosdk_2.3.3/common
    asiosdk_2.3.3/host
    asiosdk_2.3.3/host/pc
)
    
file(GLOB SOURCES
    # project sources
    src/*.cpp
    src/gui/*.cpp
    src/backend/*.cpp

    # ASIO SDK sources
    asiosdk_2.3.3/common/asio.cpp
    asiosdk_2.3.3/host/asiodrivers.cpp
    asiosdk_2.3.3/host/pc/asiolist.cpp
)

# actual executable
add_executable(theDAW
    ${SOURCES}
)

target_link_libraries(theDAW
    PUBLIC
    CUDA::cudart
    winmm
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)

file(GLOB TEST_SOURCES
    src/math/test/*.cpp
)

# gtest executable
add_executable(theDAW_test
    ${TEST_SOURCES}
)

target_link_libraries(theDAW_test
    PRIVATE
    gtest_main
    gtest
)

target_include_directories(theDAW_test
    PRIVATE
    ${googletest_SOURCE_DIR}/googletest/include
)

include(GoogleTest)
gtest_discover_tests(theDAW_test)