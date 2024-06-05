cmake_minimum_required(VERSION 3.10)
project(clicker_lib)

set(HEADER_FILES include/clicker_data.h include/clicker.h
        include/process_handler/process_manager_factory.h
        include/process_handler/linux_process_manager.h
        include/process_handler/windows_process_manager.h
        include/process_handler/process_manager.h
        include/process_handler/keyboard_callback.h
        src/routine.cpp
        include/routine.h
)

set(SOURCE_FILES src/clicker.cpp
        src/process_handler/linux_process_manager.cpp
        src/process_handler/windows_process_manager.cpp
        src/process_handler/keyboard_callback.cpp)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../cmake-modules")
include(FindJSON)

include_directories(${JSON_INCLUDE_DIRS})
include_directories(include)

if(UNIX AND NOT APPLE)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(LIBEVDEV REQUIRED libevdev)
endif ()

add_library(clicker_lib STATIC ${SOURCE_FILES} ${HEADER_FILES})
target_include_directories(clicker_lib PUBLIC include ${json_SOURCE_DIR}/include)
target_link_libraries(clicker_lib nlohmann_json::nlohmann_json)

set_target_properties(clicker_lib PROPERTIES LINKER_LANGUAGE CXX)

if(UNIX AND NOT APPLE)
    include_directories(${LIBEVDEV_INCLUDE_DIRS})
    target_link_libraries(clicker_lib ${LIBEVDEV_LIBRARIES})
    target_compile_options(clicker_lib PUBLIC ${LIBEVDEV_CFLAGS_OTHER})
endif ()
