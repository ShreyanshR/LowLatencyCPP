cmake_minimum_required(VERSION 3.20)
project(LowLevelProjects)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Auto-create one executable for each project .cpp that defines main().
file(GLOB_RECURSE PROJECT_CPP CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/*.cpp")

foreach(src IN LISTS PROJECT_CPP)
    file(RELATIVE_PATH rel_path "${CMAKE_SOURCE_DIR}" "${src}")
    if(rel_path MATCHES "^build/" OR
       rel_path MATCHES "^cmake-build-" OR
       rel_path MATCHES "^\\.idea/" OR
       rel_path MATCHES "CMakeFiles/")
        continue()
    endif()

    file(READ "${src}" SRC_CONTENT)
    string(REGEX MATCH "int[ \t\r\n]+main[ \t\r\n]*\\(" HAS_MAIN "${SRC_CONTENT}")
    if(NOT HAS_MAIN)
        continue()
    endif()

    get_filename_component(src_dir "${rel_path}" DIRECTORY)
    get_filename_component(src_base "${rel_path}" NAME_WE)
    if(src_dir STREQUAL "")
        set(name "${src_base}")
    else()
        set(name "${src_dir}_${src_base}")
    endif()
    string(REPLACE "/" "_" name "${name}")
    string(REPLACE " " "_" name "${name}")
    if(NOT TARGET "${name}")
        add_executable("${name}" "${src}")
    endif()
endforeach()
