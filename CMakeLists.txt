cmake_minimum_required(VERSION 3.16)
project(VisualAlgorithmSimulator VERSION 2.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Build type" FORCE)
endif()

if(WIN32)
    if(NOT DEFINED SFML_DIR)
        set(SFML_DIR "C:/SFML/lib/cmake/SFML")
        set(CMAKE_PREFIX_PATH "C:/SFML")
    endif()
elseif(APPLE)
    find_package(SFML 3.0 COMPONENTS Graphics Window System Audio QUIET)
    if(NOT SFML_FOUND)
        set(CMAKE_PREFIX_PATH "/opt/homebrew;/usr/local")
    endif()
endif()

find_package(SFML 3.0 COMPONENTS Graphics Window System Audio REQUIRED)

set(SOURCES
    src/main.cpp
    src/core/application.cpp
    src/core/algorithm_base.cpp
    src/core/logger.cpp
    src/core/audio_manager.cpp
    src/visualizer/sorting_visualizer.cpp
    src/algorithms/sorting/bubble_sort.cpp
    src/algorithms/sorting/selection_sort.cpp
    src/algorithms/sorting/insertion_sort.cpp
    src/algorithms/sorting/merge_sort.cpp
    src/algorithms/sorting/quick_sort.cpp
    src/algorithms/sorting/heap_sort.cpp
    src/algorithms/sorting/shell_sort.cpp
    src/algorithms/sorting/counting_sort.cpp
    src/algorithms/sorting/radix_sort.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE include)
target_link_libraries(${PROJECT_NAME} PRIVATE
    SFML::Graphics SFML::Window SFML::System SFML::Audio)

if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4 /WX-)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)
endif()

set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/assets"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>/assets"
    COMMENT "Copying assets...")

if(WIN32)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "C:/SFML/bin" "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
        COMMENT "Copying SFML DLLs...")
endif()

message(STATUS "Build type   : ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ standard : ${CMAKE_CXX_STANDARD}")
message(STATUS "VAS version  : ${PROJECT_VERSION}")