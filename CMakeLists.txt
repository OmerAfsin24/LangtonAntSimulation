cmake_minimum_required(VERSION 3.16)

project(LangtonsAntSimulation VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(CTest)

# Keep simulation logic reusable and testable without SFML.
add_library(LangtonsAntCore STATIC
    Ant.cpp
    Simulation.cpp
)
target_include_directories(LangtonsAntCore PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")

option(LANGTONS_ANT_BUILD_VISUALIZATION "Build the SFML graphical application" ON)

if(LANGTONS_ANT_BUILD_VISUALIZATION)
    # Fetch SFML only when the graphical application is enabled.
    include(FetchContent)

    FetchContent_Declare(
        SFML
        GIT_REPOSITORY https://github.com/SFML/SFML.git
        GIT_TAG 2.6.2
        GIT_SHALLOW TRUE
    )
    set(SFML_BUILD_AUDIO OFF CACHE BOOL "" FORCE)
    set(SFML_BUILD_NETWORK OFF CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(SFML)

    add_executable(LangtonsAntSimulation
        Visualization.cpp
        main.cpp
    )
    file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/assets/fonts")
    configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/assets/fonts/DejaVuSans.ttf"
        "${CMAKE_CURRENT_BINARY_DIR}/assets/fonts/DejaVuSans.ttf"
        COPYONLY
    )
    configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/assets/fonts/LICENSE-DejaVu.txt"
        "${CMAKE_CURRENT_BINARY_DIR}/assets/fonts/LICENSE-DejaVu.txt"
        COPYONLY
    )
    target_link_libraries(LangtonsAntSimulation PRIVATE
        LangtonsAntCore
        sfml-graphics
        sfml-window
        sfml-system
    )
    install(TARGETS LangtonsAntSimulation RUNTIME DESTINATION bin)
    install(
        FILES
            "${CMAKE_CURRENT_SOURCE_DIR}/assets/fonts/DejaVuSans.ttf"
            "${CMAKE_CURRENT_SOURCE_DIR}/assets/fonts/LICENSE-DejaVu.txt"
        DESTINATION share/langtons-ant-simulation/assets/fonts
    )
endif()

if(BUILD_TESTING)
    # Core tests link only the simulation library, not the renderer.
    add_executable(LangtonsAntCoreTests tests/SimulationTests.cpp)
    target_link_libraries(LangtonsAntCoreTests PRIVATE LangtonsAntCore)
    add_test(NAME LangtonsAntCoreTests COMMAND LangtonsAntCoreTests)
endif()
