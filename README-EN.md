# PathFindingVisualizer

`PathFindingVisualizer` is a desktop pathfinding visualization project built with C++17, SDL2, and Dear ImGui.

It provides an interactive 20 × 20 grid environment where you can place a start point, a goal point, walls, and weighted tiles, then observe the search process through step-by-step execution or auto-run.

## Algorithms

- The complete default implementations of A*, Dijkstra, BFS, and Greedy Best-First Search are located in `algorithm/impl/`.
- `CustomPathfinder` does not provide a built-in implementation by default and is intended for your own experimentation and extension. This option can only be enabled from the ImGui Dev Options panel.
- The root-level `algorithm/*_pathfinder.cpp` files provide learning-oriented empty skeletons and are compiled only when `PATHFINDER_USE_CUSTOM_IMPLEMENTATIONS=ON`.
- The default build uses the complete implementations provided in `algorithm/impl/`.

## Features

### Default Features

- A 20 × 20 visual grid board.
- Supports placing the start point, goal point, walls, and tile weights.
- The buttons on the left allow selecting A*, Dijkstra, BFS, and Greedy.
- Supports auto-run, pause, next step, previous step, restart search, and reset board.
- When the mouse hovers over a tile, detailed information about that tile is shown in the upper-left corner.
- Displays total search steps, final path steps, and final path total cost.
- Directional textures such as Open, Current, Path, and Closed follow the parent chain and point toward the parent tile.

### Developer Features

#### Click the small wrench in the lower-left corner to open the ImGui debug window

Dev mode adds extra buttons:
- Show tile weights.
- Edit tile weight settings.
- Show the cost of each visited tile.

ImGui window:
- Select the Custom algorithm.
- Select the A* heuristic.
- Choose between 4-direction and 8-direction movement.
- In 8-direction mode, three diagonal movement policies are supported:
  - `Strict No Corner Cutting`: if either of the two side-adjacent tiles is a wall, diagonal movement is blocked.
  - `No Corner Cutting`: diagonal movement is blocked only if both side-adjacent tiles are walls.
  - `Allow Corner Cutting`: side-adjacent tiles are not checked, and diagonal movement through corners is allowed.
- Edit the weight of a single tile (1-10).
- Configure the global per-step movement cost for straight / diagonal movement. By default, straight movement is `10 * weight` and diagonal movement is `14 * weight`.
- Configure the auto-run speed.
- View global state machine information.

## Project Structure

```text
.
|-- main.cpp                         # Program entry
|-- CMakeLists.txt                   # Root CMake configuration
|-- status.h                         # Shared enums for input, algorithms, states, movement modes, heuristics, etc.
|-- application/                     # Application main loop and simulation controller
|-- Aframework/                      # Board, tiles, buttons, text textures, and basic UI components
|-- algorithm/                       # Pathfinder base class, learning skeletons, and algorithm documentation
|-- algorithm/impl/                  # Default complete algorithm implementations
|-- assets/                          # Fonts, textures, and audio resources
|-- imgui/                           # Dear ImGui source and SDL rendering backend
|-- thirdparty/                      # SDL2 headers, import libraries, and DLLs
`-- utils/                           # Common display string utilities
```

## Requirements

- The project is currently developed and tested primarily on Windows x64.
- Visual Studio 2022, or another compiler with C++17 support.
- CMake 3.12 or later.

The repository already includes the SDL2 headers, import libraries, and DLLs required for Windows builds in `thirdparty/`, so no extra SDL installation through a package manager is required for the current setup.

- Verified with both MSVC and MinGW builds.
- Non-Windows builds use pkg-config to locate SDL2 dependencies; macOS has not yet been tested.

## Build

Default mode uses the complete implementations in `algorithm/impl/`:

```powershell
cmake -S . -B build -A x64 -DPATHFINDER_USE_CUSTOM_IMPLEMENTATIONS=OFF
cmake --build build --config Debug
```

Learning mode uses the empty skeletons in the root `algorithm/*_pathfinder.cpp` files:

```powershell
cmake -S . -B build -A x64 -DPATHFINDER_USE_CUSTOM_IMPLEMENTATIONS=ON
cmake --build build --config Debug
```

Build the Release version:

```powershell
cmake --build build --config Release
```

## Run

When using a Visual Studio generator, the Debug executable is usually located at:

```powershell
.\build\Debug\PathFindingVisualizer.exe
```

The Release executable is usually located at:

```powershell
.\build\Release\PathFindingVisualizer.exe
```

The program loads fonts, textures, and audio resources from `assets/`. The root CMake configuration copies `assets/` into the build directory during configuration.

If you get a missing SDL DLL error at runtime, you can copy the following files from `thirdparty/lib/msvc/` to the same directory as the executable, or add that directory to `PATH`:

- `SDL2.dll`
- `SDL2_image.dll`
- `SDL2_mixer.dll`
- `SDL2_ttf.dll`

## Usage

1. Select `Start`, then click a tile on the board to set the start point.
2. Select `Goal`, then click a tile on the board to set the goal point.
3. Select `Wall`, then click or drag on the board to draw obstacles.
4. Select `Edit Weight`, or use the weight brush in Dev Options to paint weighted tiles.
5. Select an algorithm.
6. Click `Next Step` to advance one step, or click `Auto Run` to run automatically.
7. Use `Pause` to stop auto-run, `Prev Step` to step back once, `Restart` to clear the search process, and `Reset` to clear the entire board.

Once the search starts, board editing is locked. If you need to change tiles, the algorithm, movement mode, diagonal policy, or the A* heuristic, you must click `Restart` or `Reset` first.

## Statistics

- `Total Steps`: the number of algorithm steps already executed by the controller.
- `Path Steps`: the number of movement steps in the final path reconstructed from the parent chain.
- `Total Cost`: the final path total cost, computed by multiplying the configured straight / diagonal movement cost by tile weights along the reconstructed parent chain.

If no path is found, `Path Steps` and `Total Cost` remain `0`.

## Algorithm Notes

All pathfinding algorithms implement a shared `Pathfinder` interface:

```cpp
class Pathfinder
{
public:
    virtual void next_step() = 0;
    virtual std::unique_ptr<Pathfinder> clone() const = 0;
};
```

When an algorithm class inherits from `CloneablePathfinder<Derived>`, clone support is generated automatically so that algorithm state snapshots can be saved for rollback. `SimulationController` owns the current algorithm instance and is responsible for step-by-step execution, auto-play, history storage, previous-step rollback support, and locking board editing while the search is running.

Shared logic such as reading the start and goal, gathering neighbors, computing movement cost, computing heuristic cost, marking tiles, closing the current tile, and rebuilding the final path is implemented in the `Pathfinder` base class.

For more implementation details, see the algorithm notes:

- [algorithm/README-EN.md](algorithm/README-EN.md)

For more engineering details, architecture notes, and module breakdowns, see the [project page](https://akil0814.github.io/projects/PathFindingVisualizer/PathFindingVisualizer.html).

## Resources and Third-Party Code

- Fonts, textures, and audio resources are located in `assets/`.
- Dear ImGui source code is located in `imgui/`.
- SDL2 headers, libraries, and DLLs are located in `thirdparty/`.

See `LICENSE.txt` and `imgui/LICENSE.txt` for license information.
