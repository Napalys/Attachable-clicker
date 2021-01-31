cmake_minimum_required(VERSION 3.10)
project(injectionClicker)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_PREFIX_PATH "D:\\Qt\\Qt5.12.10\\5.12.10\\mingw73_64\\")


find_package(Qt5Widgets REQUIRED)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

include_directories(clicker)
add_subdirectory(clicker)

add_executable(${PROJECT_NAME} main.cpp gui/mainwindow.cpp gui/mainwindow.h gui/mainwindow.ui)
set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(${PROJECT_NAME} Qt5::Widgets)
target_link_libraries(${PROJECT_NAME} clicker_lib)

#Add macro for debug and release in order to optimize the release version
if (CMAKE_BUILD_TYPE MATCHES Debug)
    target_compile_definitions(${PROJECT_NAME} PUBLIC DEBUG)
elseif (CMAKE_BUILD_TYPE MATCHES Release)
    target_compile_definitions(${PROJECT_NAME} PUBLIC RELEASE)
endif ()