# PathFindingVisualizer

`PathFindingVisualizer` is a C++17 desktop path-finding visualizer built with SDL2 and Dear ImGui.

The app provides an interactive 20 x 20 grid where you can place a start tile, a goal tile, walls, and weighted tiles, then watch several path-finding algorithms advance step by step or run automatically.

## Current Status

- A*, Dijkstra, BFS, and Greedy Best-First Search have complete built-in implementations under `algorithm/impl/`.
- `CustomPathfinder` is intentionally empty and is meant for user experiments. It can be selected from the ImGui Dev Options panel.
- The root `algorithm/*_pathfinder.cpp` files are learning stubs. They compile only when `PATHFINDER_USE_CUSTOM_IMPLEMENTATIONS=ON`.
- The default build uses the complete implementations in `algorithm/impl/`.

## Features

- Visual 20 x 20 grid board.
- Edit tools for start, goal, wall, erase, and weighted tiles.
- Algorithm buttons for A*, Dijkstra, BFS, and Greedy.
- Custom algorithm selection from the ImGui Dev Options panel.
- Auto-run, pause, next-step, previous-step, restart, and reset controls.
- Board editing is locked while a search is running.
- Four-way and eight-way movement.
- Three diagonal movement policies:
  - `Strict No Corner Cutting`: block a diagonal if either side tile is blocked.
  - `No Corner Cutting`: block a diagonal only if both side tiles are blocked.
  - `Allow Corner Cutting`: ignore side tiles for diagonal movement.
- Weighted tiles from 1 to 10.
- Movement cost uses `10 * weight` for straight moves and `14 * weight` for diagonal moves.
- Counters for total search steps, final path steps, and final path cost.
- Directional open/current/path/closed textures use parent links to point toward the previous tile.
- Dev Options panel supports algorithm selection, A* heuristic selection, movement mode, diagonal policy, auto-run speed, weight brush, state inspection, and mouse/tile debugging.

## Project Layout

```text
.
|-- main.cpp                         # Program entry point
|-- CMakeLists.txt                   # Root CMake configuration
|-- status.h                         # Shared enums for input, algorithms, state, movement, and heuristics
|-- appliction/                      # Application loop and simulation controller
|-- Aframework/                      # Board, tile, button, text texture, and UI helpers
|-- algorithm/                       # Pathfinder base class, learning stubs, and implementation guide
|-- algorithm/impl/                  # Default complete algorithm implementations
|-- assets/                          # Fonts, textures, and sound assets
|-- imgui/                           # Dear ImGui source and SDL renderer backend
|-- thirdparty/                      # SDL2 headers, import libraries, and DLLs
`-- utils/                           # Shared display-string helpers
```

Note: the current directory name is `appliction`. The CMake files and source includes use this spelling, so keep it unless you update every reference.

## Requirements

- Windows x64.
- Visual Studio 2022, or another C++17-capable compiler.
- CMake 3.12 or newer.

The repository already includes the SDL2 headers, import libraries, and DLLs under `thirdparty/`, so the bundled Windows build does not require extra SDL package manager setup.

## Build

Default mode uses the complete implementations in `algorithm/impl/`:

```powershell
cmake -S . -B build -A x64 -DPATHFINDER_USE_CUSTOM_IMPLEMENTATIONS=OFF
cmake --build build --config Debug
```

Learning mode uses the root `algorithm/*_pathfinder.cpp` stubs instead:

```powershell
cmake -S . -B build -A x64 -DPATHFINDER_USE_CUSTOM_IMPLEMENTATIONS=ON
cmake --build build --config Debug
```

For a Release build:

```powershell
cmake --build build --config Release
```

## Run

With the Visual Studio generator, the Debug executable is usually created at:

```powershell
.\build\Debug\PathFindingVisualizer.exe
```

The Release executable is usually created at:

```powershell
.\build\Release\PathFindingVisualizer.exe
```

The app loads fonts, textures, and sounds from `assets/`. The root CMake file copies `assets/` into the build directory during configuration.

If Windows cannot find an SDL DLL at launch time, copy these files from `thirdparty/lib/msvc/` next to the executable, or add that directory to your `PATH`:

- `SDL2.dll`
- `SDL2_image.dll`
- `SDL2_mixer.dll`
- `SDL2_ttf.dll`

## How To Use

1. Select `Start`, then click a grid cell to place the start tile.
2. Select `Goal`, then click a grid cell to place the target tile.
3. Select `Wall` and click or drag on the grid to create blocked cells.
4. Select `Edit Weight` or use the Dev Options weight brush to paint weighted tiles.
5. Select an algorithm.
6. Click `Next Step` to advance manually, or `Auto Run` to run continuously.
7. Use `Pause` to stop auto-run, `Prev Step` to undo one search step, `Restart` to clear search progress, or `Reset` to clear the whole board.

Once a search starts, board editing is locked. Click `Restart` or `Reset` before changing tiles, algorithms, movement mode, diagonal policy, or A* heuristic settings.

## Dev Options

Click the small button in the lower-left corner to toggle the Dev Options panel.

Dev Options lets you:

- Select A*, Dijkstra, BFS, Greedy, or Custom.
- Change A* heuristic mode: Manhattan, Euclidean, Octile, or Chebyshev.
- Switch between four-way and eight-way movement.
- Choose the diagonal corner-cutting policy.
- Adjust auto-run speed.
- Set the weight brush value.
- Inspect simulation state, play mode, edit lock, found-path state, and mouse position.

## Counters

- `Total Steps`: number of algorithm steps executed by the controller.
- `Path Steps`: number of moves in the final reconstructed path.
- `Total Cost`: final reconstructed path cost, using `10 * weight` for straight moves and `14 * weight` for diagonal moves.

If no path is found, `Path Steps` and `Total Cost` stay at `0`.

## Algorithm Notes

All pathfinders implement the shared `Pathfinder` interface:

```cpp
class Pathfinder
{
public:
    virtual void next_step() = 0;
    virtual std::unique_ptr<Pathfinder> clone() const = 0;
};
```

Algorithm classes inherit from `CloneablePathfinder<Derived>`, so clone behavior is automatic. `SimulationController` owns the active pathfinder, advances it one step at a time, saves history, supports previous-step undo, and locks board editing during the search.

Common behavior such as reading endpoints, neighbor lookup, movement cost, heuristic cost, tile marking, closing the current tile, and rebuilding the final path lives in the `Pathfinder` base class.

For implementation details, read the algorithm guide:

- [algorithm/README-EN.md](algorithm/README-EN.md)
- [algorithm/README-CN.md](algorithm/README-CN.md)

## Assets And Third-Party Code

- Fonts, textures, and sounds live in `assets/`.
- Dear ImGui sources live in `imgui/`.
- SDL2 headers, libraries, DLLs, and optional DLLs live in `thirdparty/`.

See `LICENSE.txt` and `imgui/LICENSE.txt` for license information included in this repository.
