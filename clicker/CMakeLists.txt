project(clicker_lib)

set(HEADER_FILES include/clicker_data.h include/clicker.h
        include/process_handler/process_manager_factory.h
        include/process_handler/linux_process_manager.h
        include/process_handler/windows_process_manager.h
        include/process_handler/process_manager.h
)

set(SOURCE_FILES src/clicker.cpp
        src/process_handler/linux_process_manager.cpp
        src/process_handler/windows_process_manager.cpp)

include_directories(include)

add_library(clicker_lib STATIC ${SOURCE_FILES} ${HEADER_FILES})
target_include_directories(clicker_lib PUBLIC include)

set_target_properties(clicker_lib PROPERTIES LINKER_LANGUAGE CXX)
