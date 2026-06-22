cmake_minimum_required(VERSION 3.20)
project(LowLevelProjects)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# --- GoogleTest via FetchContent (no manual install needed) ---
include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        v1.15.2
)
# (Windows) keep gtest linking the shared CRT; harmless elsewhere.
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()
include(GoogleTest)

# Explicitly add memory_order_demo
add_executable(memory_order_demo Concurrency/memory_order_demo.cpp)

# Auto-create one executable for each project .cpp that defines main().
set(PROJECT_CPP "")

file(GLOB ROOT_CPP CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/*.cpp")
list(APPEND PROJECT_CPP ${ROOT_CPP})

foreach(source_dir IN ITEMS Concurrency GetCracked LeetCode MemoryManagment OS "wc tool")
    if(EXISTS "${CMAKE_SOURCE_DIR}/${source_dir}")
        file(GLOB_RECURSE DIR_CPP CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/${source_dir}/*.cpp")
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

    get_filename_component(src_dir "${rel_path}" DIRECTORY)
    get_filename_component(src_base "${rel_path}" NAME_WE)
    if(src_dir STREQUAL "")
        set(name "${src_base}")
    else()
        set(name "${src_dir}_${src_base}")
    endif()
    string(REPLACE "/" "_" name "${name}")
    string(REPLACE " " "_" name "${name}")

    # Files named *_test.cpp become GoogleTest targets (gtest_main supplies main()).
    if(rel_path MATCHES "_test\\.cpp$")
        if(NOT TARGET "${name}")
            add_executable("${name}" "${src}")
            target_link_libraries("${name}" PRIVATE GTest::gtest_main)
            gtest_discover_tests("${name}")
        endif()
        continue()
    endif()

    # Everything else: build an executable only if it defines main().
    file(READ "${src}" SRC_CONTENT)
    string(REGEX REPLACE "/\\*([^*]|\\*+[^*/]|[\\r\\n])*\\*+/" "" SRC_NO_BLOCK_COMMENTS "${SRC_CONTENT}")
    string(REGEX REPLACE "//[^\\r\\n]*" "" SRC_NO_COMMENTS "${SRC_NO_BLOCK_COMMENTS}")
    string(REGEX MATCH "int[ \t\r\n]+main[ \t\r\n]*\\(" HAS_MAIN "${SRC_NO_COMMENTS}")
    if(NOT HAS_MAIN)
        continue()
    endif()

    if(NOT TARGET "${name}")
        add_executable("${name}" "${src}")
    endif()
endforeach()
