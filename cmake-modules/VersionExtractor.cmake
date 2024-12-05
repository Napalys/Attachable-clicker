execute_process(
        COMMAND git rev-parse --is-inside-work-tree
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE IS_GIT_REPO
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET
)

if(IS_GIT_REPO)
    execute_process(
            COMMAND git describe --tags --abbrev=0
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            OUTPUT_VARIABLE GIT_TAG
            OUTPUT_STRIP_TRAILING_WHITESPACE
    )

    execute_process(
            COMMAND git rev-parse HEAD
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            OUTPUT_VARIABLE GIT_SHA
            OUTPUT_STRIP_TRAILING_WHITESPACE
    )

    if(GIT_TAG)
        string(REGEX MATCH "^v?([0-9]+)\\.([0-9]+)\\.([0-9]+)(-(.+))?" VERSION_MATCH ${GIT_TAG})
        if(VERSION_MATCH)
            set(VERSION_MAJOR "${CMAKE_MATCH_1}")
            set(VERSION_MINOR "${CMAKE_MATCH_2}")
            set(VERSION_PATCH "${CMAKE_MATCH_3}")
            set(VERSION_TWEAK "0")
            set(VERSION_STRING "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")
        else()
            message(FATAL_ERROR "Invalid version tag format: ${GIT_TAG}")
        endif()
    else()
        message(FATAL_ERROR "Failed to get Git tag")
    endif()
else()
    message(WARNING "Not inside a Git repository, defaulting version to 0.0.0")
    set(VERSION_MAJOR "0")
    set(VERSION_MINOR "0")
    set(VERSION_PATCH "0")
    set(VERSION_TWEAK "0")
    set(VERSION_STRING "0.0.0")
    set(GIT_SHA "unknown")
endif()

set(PROJECT_VERSION ${VERSION_STRING})
set(PROJECT_VERSION_MAJOR ${VERSION_MAJOR})
set(PROJECT_VERSION_MINOR ${VERSION_MINOR})
set(PROJECT_VERSION_PATCH ${VERSION_PATCH})
set(PROJECT_VERSION_TWEAK ${VERSION_TWEAK})
set(GIT_SHA ${GIT_SHA})
message(STATUS "Project version: ${PROJECT_VERSION}")
