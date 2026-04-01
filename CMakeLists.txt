cmake_minimum_required(VERSION 3.20)
project(LowLevelProjects)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Auto-create one executable for each project .cpp that defines main().
set(PROJECT_CPP "")

file(GLOB ROOT_CPP CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/*.cpp")
list(APPEND PROJECT_CPP ${ROOT_CPP})

foreach(source_dir IN ITEMS Concurrency GetCracked MemoryManagment OS "wc tool")
    if(EXISTS "${CMAKE_SOURCE_DIR}/${source_dir}")
        file(GLOB DIR_CPP CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/${source_dir}/*.cpp")
        list(APPEND PROJECT_CPP ${DIR_CPP})
    endif()
endforeach()

foreach(src IN LISTS PROJECT_CPP)
    file(RELATIVE_PATH rel_path "${CMAKE_SOURCE_DIR}" "${src}")
    if(rel_path MATCHES "^build/" OR
       rel_path MATCHES "^cmake-build-" OR
       rel_path MATCHES "^\\.idea/" OR
       rel_path MATCHES "CMakeFiles/")
        continue()
    endif()

    file(READ "${src}" SRC_CONTENT)
    string(REGEX REPLACE "/\\*([^*]|\\*+[^*/]|[\r\n])*\\*+/" "" SRC_NO_BLOCK_COMMENTS "${SRC_CONTENT}")
    string(REGEX REPLACE "//[^\r\n]*" "" SRC_NO_COMMENTS "${SRC_NO_BLOCK_COMMENTS}")
    string(REGEX MATCH "int[ \t\r\n]+main[ \t\r\n]*\\(" HAS_MAIN "${SRC_NO_COMMENTS}")
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
