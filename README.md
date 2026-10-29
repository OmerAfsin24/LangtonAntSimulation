# Langton's Ant Simulation

Langton's Ant Simulation is a C++17 simulation with an SFML graphical interface. The simulation core is independent of SFML: grid positions are expressed as `(row, column)`, cells start dark, and ants wrap around grid edges.

## Project structure

```text
.
├── Ant.cpp / Ant.h                 Ant behavior
├── Simulation.cpp / Simulation.h   Grid and simulation logic
├── Visualization.cpp / Visualization.h  SFML graphical interface
├── main.cpp                        Application entry point
├── rules.txt                       Simulation rule description
├── assets/fonts/                   Bundled DejaVu Sans font and license
├── tests/SimulationTests.cpp       Simulation core tests
├── CMakeLists.txt                  Build configuration
└── CMakePresets.json               Default Ninja build preset
```

`out/` and `build/` are generated build directories and are not part of the source tree.

## Run from source

The graphical application requires:

- A C++17 compiler (GCC, Clang, or MSVC).
- CMake 3.27 or newer and Ninja. The included configure preset uses CMake preset schema version 8.
- Git and an internet connection for the first configure; CMake downloads SFML 2.6.2.
- The platform's normal graphics/window-system development dependencies. On Linux, install your distribution's C++ build tools, Ninja, Git, OpenGL development files, and X11 development libraries.

Examples for installing the build tools:

- **Ubuntu / Debian:** `sudo apt install build-essential cmake ninja-build git libgl1-mesa-dev libx11-dev libxrandr-dev libxcursor-dev libxi-dev`
- **macOS:** install Xcode Command Line Tools with `xcode-select --install`, then install CMake, Ninja, and Git (for example, with Homebrew).
- **Windows:** install Visual Studio 2022 or Build Tools with the **Desktop development with C++** workload, plus CMake 3.27+ and Ninja. Run the commands below from a Visual Studio Developer PowerShell so the MSVC compiler is available.

Then clone the repository and enter its directory:

```sh
git clone <repository-url>
cd LangtonAnt
```

Configure and compile the graphical application:

```sh
cmake --preset LangtonsAntSimulationPreset
cmake --build out/build/LangtonsAntSimulationPreset
```

Run it from the repository root with the command for your terminal:

**Linux / macOS (Bash, Zsh):**

```sh
./out/build/LangtonsAntSimulationPreset/LangtonsAntSimulation
```

**Windows (PowerShell):**

```powershell
.\out\build\LangtonsAntSimulationPreset\LangtonsAntSimulation.exe
```

The preset uses Ninja and a Debug build. Run the configure and build commands again after pulling changes to rebuild. The generated executable and CMake files stay under `out/build/LangtonsAntSimulationPreset/`.

To build and test only the simulation core, without fetching or linking SFML:

```sh
cmake -S . -B build-core -DLANGTONS_ANT_BUILD_VISUALIZATION=OFF
cmake --build build-core
ctest --test-dir build-core --output-on-failure
```

At launch, the setup menu asks for rows, columns, and cell size. Rows and columns can each be from 1 to 500; cell size can be from 4 to 40 pixels. The selected cell size is used when the grid fits, and is reduced as needed to fit the grid within the display area.

The graphical application opens with the simulation running. Use the controls or shortcuts:

- **Space**: play or pause.
- **N**: advance one step while paused.
- **R**: reset the grid, ant, and turn count; reset leaves the simulation paused.
- **Escape**: close the window.
- **Add ant**: activate a new ant on a randomly selected unoccupied cell. New ants start facing up; if every cell is occupied, the button reports that the grid is full.

All ants share the same grid and each simulation tick advances them in insertion order. Each ant sees cell changes made by earlier ants in that tick. Reset removes the added ants and restores the original center ant.

The numeric speed field supports values from 1 to 60 steps per second. Typing a valid value applies it immediately; press **Enter** to finish editing. The render loop is limited to 60 FPS and advances at most one simulation step per frame, so a delayed frame will not trigger a burst of catch-up steps.

The ant is drawn from SFML shapes rather than a downloaded image. This keeps it crisp at different grid sizes and avoids third-party ant artwork with licensing or transparency concerns. The interface font is DejaVu Sans, bundled under its Bitstream Vera license in `assets/fonts/`. CMake copies it beside the local executable; installed builds look for it under `share/langtons-ant-simulation/assets/fonts/`, relative to the executable. The application locates itself on Linux and Windows, so launching it from another working directory does not depend on the current directory.

To install the graphical application and its font under the configured install prefix:

```sh
cmake --install out/build/LangtonsAntSimulationPreset
```

This stages the executable and font; it does not yet bundle SFML's shared libraries or all platform system dependencies. A distributable GitHub Release will need to provide those runtime dependencies for each target platform.

## Running a prebuilt application

Prebuilt GitHub Release downloads are not available yet. Once published, ordinary users will be able to download a build for their operating system without installing a C++ compiler or CMake. Release builds must include or otherwise provide the runtime libraries required by SFML; the source-build dependency setup alone does not package those runtime files.
