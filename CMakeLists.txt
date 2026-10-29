cmake_minimum_required(VERSION 3.21)

project(LangtonsAntSimulation VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(CTest)
include(GNUInstallDirs)

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
    if(UNIX AND NOT APPLE)
        set_target_properties(LangtonsAntSimulation PROPERTIES
            INSTALL_RPATH "$ORIGIN/../${CMAKE_INSTALL_LIBDIR}"
        )
        set_target_properties(sfml-graphics sfml-window sfml-system PROPERTIES
            INSTALL_RPATH "$ORIGIN"
        )
    endif()
    install(TARGETS LangtonsAntSimulation
        RUNTIME_DEPENDENCY_SET LangtonsAntRuntimeDependencies
        RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
    )
    if(WIN32 OR CMAKE_SYSTEM_NAME STREQUAL "Linux")
        install(RUNTIME_DEPENDENCY_SET LangtonsAntRuntimeDependencies
            LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
            RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
            DIRECTORIES
                "$<TARGET_FILE_DIR:sfml-graphics>"
                "$<TARGET_FILE_DIR:sfml-window>"
                "$<TARGET_FILE_DIR:sfml-system>"
            PRE_EXCLUDE_REGEXES
                "^api-ms-"
                "^ext-ms-"
            POST_EXCLUDE_REGEXES
                "^/lib/"
                "^/usr/lib/"
                ".*[/\\\\]Windows[/\\\\].*"
        )
    endif()
    install(
        FILES
            "${CMAKE_CURRENT_SOURCE_DIR}/assets/fonts/DejaVuSans.ttf"
            "${CMAKE_CURRENT_SOURCE_DIR}/assets/fonts/LICENSE-DejaVu.txt"
        DESTINATION "${CMAKE_INSTALL_DATADIR}/langtons-ant-simulation/assets/fonts"
    )
    install(
        FILES
            "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE"
            "${sfml_SOURCE_DIR}/license.md"
        DESTINATION "${CMAKE_INSTALL_DATADIR}/doc/langtons-ant-simulation"
    )
endif()

if(BUILD_TESTING)
    # Core tests link only the simulation library, not the renderer.
    add_executable(LangtonsAntCoreTests tests/SimulationTests.cpp)
    target_link_libraries(LangtonsAntCoreTests PRIVATE LangtonsAntCore)
    add_test(NAME LangtonsAntCoreTests COMMAND LangtonsAntCoreTests)
endif()

set(CPACK_PACKAGE_NAME "langtons-ant-simulation")
set(CPACK_PACKAGE_VENDOR "OmerAfsin24")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Interactive Langton's Ant simulation")
set(CPACK_PACKAGE_HOMEPAGE_URL "https://github.com/OmerAfsin24/LangtonAntSimulation")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_FILE_NAME
    "LangtonsAntSimulation-${PROJECT_VERSION}-${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")

if(WIN32)
    set(CPACK_GENERATOR "ZIP")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(CPACK_GENERATOR "DEB")
    set(CPACK_DEBIAN_PACKAGE_MAINTAINER "OmerAfsin24")
    set(CPACK_DEBIAN_PACKAGE_SECTION "games")
    set(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")
    set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
    set(CPACK_DEBIAN_FILE_NAME "DEB-DEFAULT")
endif()

include(CPack)
