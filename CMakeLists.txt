cmake_minimum_required(VERSION 3.10)
project(injectionClicker)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 COMPONENTS Widgets REQUIRED)

add_subdirectory(clicker)

add_executable(${PROJECT_NAME} main.cpp
        gui/mainwindow.cpp
        gui/mainwindow.ui
        gui/delegates/non_editable_delegate.hpp
        gui/delegates/numeric_delegate.hpp
        gui/delegates/action_delegate.hpp
        gui/dialogs/clicker_data_dialog.cpp
        gui/dialogs/clicker_data_dialog.h
)

target_include_directories(${PROJECT_NAME} PRIVATE gui)
target_link_libraries(${PROJECT_NAME} PRIVATE
        Qt5::Widgets
        clicker_lib
)

set_target_properties(${PROJECT_NAME} PROPERTIES
        LINKER_LANGUAGE CXX
        CXX_EXTENSIONS OFF
)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(${PROJECT_NAME} PRIVATE DEBUG)
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_definitions(${PROJECT_NAME} PRIVATE RELEASE)
endif()

target_compile_options(${PROJECT_NAME} PRIVATE
        $<$<CXX_COMPILER_ID:GNU>:-Wall -Wextra -Wpedantic -Werror>
        $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
)
