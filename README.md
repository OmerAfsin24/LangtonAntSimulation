# Langton's Ant Simulation

Langton's Ant Simulation is a C++17 simulation with an SFML graphical interface. The simulation core is independent of SFML: grid positions are expressed as `(row, column)`, cells start dark, and ants wrap around grid edges.

## Project structure

```text
.
├── Ant.cpp / Ant.h                 Ant behavior
├── Simulation.cpp / Simulation.h   Grid and simulation logic
├── Visualization.cpp / Visualization.h  SFML graphical interface
├── main.cpp                        Application entry point
├── assets/fonts/                   Bundled DejaVu Sans font and license
├── tests/SimulationTests.cpp       Simulation core tests
├── LICENSE                         MIT license for the project code
├── CMakeLists.txt                  Build configuration
├── CMakePresets.json               Default Ninja build preset
└── .github/workflows/release.yml   Package and publish tagged releases
```

`out/`, `build/`, and `build-core/` are generated build directories and are not part of the source tree.

## Screenshots

Screenshots will be added here.

## Supported operating systems

The release workflow builds packages for:

- **Windows 10 and 11, 64-bit:** portable ZIP containing the application, SFML runtime libraries, and bundled font.
- **Ubuntu 24.04 LTS, x86_64:** Debian package (`.deb`) containing the application and SFML runtime libraries.

Other Linux distributions and macOS are not currently covered by prebuilt packages. They may be built from source, but are not part of the release support matrix.

## Install a release build

When a release is published, download the package for your system from [GitHub Releases](https://github.com/OmerAfsin24/LangtonAntSimulation/releases).

**Windows:** extract the ZIP, then launch `bin\LangtonsAntSimulation.exe` from the extracted folder.

**Ubuntu 24.04:** install the downloaded package with:

```sh
sudo apt install ./langtons-ant-simulation_*_amd64.deb
```

The executable is installed as `LangtonsAntSimulation` and can be launched from a terminal with:

```sh
LangtonsAntSimulation
```

## Build from source

The graphical application requires:

- A C++17 compiler (GCC, Clang, or MSVC).
- CMake 3.21 or newer for the project; CMake 3.27 or newer and Ninja to use the included configure preset (version 8).
- Git and an internet connection for the first configure; CMake downloads SFML 2.6.2.
- The platform's normal graphics/window-system development dependencies. On Linux, install your distribution's C++ build tools, Ninja, Git, OpenGL, udev, and X11 development libraries.

Examples for installing the build tools:

- **Ubuntu / Debian:** `sudo apt install build-essential cmake ninja-build git libgl1-mesa-dev libudev-dev libx11-dev libxcursor-dev libxi-dev libxinerama-dev libxrandr-dev libxxf86vm-dev`
- **macOS:** install Xcode Command Line Tools with `xcode-select --install`, then install CMake, Ninja, and Git (for example, with Homebrew).
- **Windows:** install Visual Studio 2022 or Build Tools with the **Desktop development with C++** workload, plus CMake 3.27+ and Ninja. Run the commands below from a Visual Studio Developer PowerShell so the MSVC compiler is available.

Then clone the repository and enter its directory:

```sh
git clone https://github.com/OmerAfsin24/LangtonAntSimulation.git
cd LangtonAntSimulation
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

## License

The project code is licensed under the MIT License; see [LICENSE](./LICENSE). The bundled DejaVu Sans font is separately licensed; see [assets/fonts/LICENSE-DejaVu.txt](./assets/fonts/LICENSE-DejaVu.txt). SFML is fetched as a third-party dependency during the graphical build and is not covered by this project's MIT license. Review SFML's license and any applicable platform dependency licenses when redistributing a built application.

Release packages are built and tested by GitHub Actions when a version tag is pushed. Update the version in the `project(... VERSION ...)` declaration in `CMakeLists.txt`, then push the matching tag (for example, `v0.1.0` for version `0.1.0`):

```sh
git tag v0.1.0
git push origin v0.1.0
```

The workflow verifies the tag matches the project version, runs the test suite, creates the Windows ZIP and Ubuntu `.deb`, and publishes them as GitHub Release assets.

## Known limitations

- Release packages are currently limited to 64-bit Windows 10/11 and Ubuntu 24.04 LTS on x86_64. There is no macOS package.
- The application requires a graphical desktop session. The simulation core tests can run without a display.
- The renderer is limited to 60 FPS and advances at most one simulation step per frame, so the effective simulation speed is capped at 60 steps per second.
