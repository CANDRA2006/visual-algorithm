cmake_minimum_required(VERSION 3.16)
project(VisualAlgorithmSimulator)

set(CMAKE_CXX_STANDARD 17)

# Set SFML path explicitly
set(SFML_DIR "C:/SFML/lib/cmake/SFML")
set(CMAKE_PREFIX_PATH "C:/SFML")

find_package(SFML 3.0 COMPONENTS Graphics Window System REQUIRED)

include_directories(
    include
    ${SFML_INCLUDE_DIRS}
    "C:/SFML/include"
)

set(SOURCES
    src/main.cpp
    src/core/application.cpp
    src/core/algorithm_base.cpp
    src/visualizer/sorting_visualizer.cpp
    src/algorithms/sorting/bubble_sort.cpp
)

add_executable(VisualAlgorithmSimulator ${SOURCES})

target_link_libraries(VisualAlgorithmSimulator PRIVATE 
    sfml-graphics
    sfml-window
    sfml-system
)

set_target_properties(VisualAlgorithmSimulator PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

add_custom_command(TARGET VisualAlgorithmSimulator POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/assets
        ${CMAKE_BINARY_DIR}/bin/assets
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "C:/SFML/bin"
        ${CMAKE_BINARY_DIR}/bin
)