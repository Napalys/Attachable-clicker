find_package(nlohmann_json QUIET)
if(nlohmann_json_FOUND)
    message("nlohmann_json found")
else()
    message("nlohmann_json not found, starting to download...")
    include(FetchContent)
    FetchContent_Declare(json
            GIT_REPOSITORY https://github.com/nlohmann/json.git
            GIT_TAG v3.9.1)

    FetchContent_GetProperties(json)
    if (NOT json_POPULATED)
        FetchContent_Populate(json)
        add_subdirectory(${json_SOURCE_DIR} ${json_BINARY_DIR} EXCLUDE_FROM_ALL)
    endif ()
endif ()