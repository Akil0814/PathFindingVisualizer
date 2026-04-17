# PathFindingVisualizer

A C++17 desktop path-finding visualizer built with SDL2 and Dear ImGui.

The project provides an interactive 20 x 20 grid where you can place a start
tile, a goal tile, walls, and weighted tiles, then step through the search
process or let it run automatically.

## Current Status

- BFS has a working step-by-step implementation.
- A*, Dijkstra, and Greedy Best-First Search are wired into the UI and class
  structure, but their `next_step()` implementations are currently placeholders.
- The app includes board editing, path playback, previous-step undo, weighted
  tile editing, four-way/eight-way movement settings, and an ImGui debug panel.

## Features

- Visual 20 x 20 grid board.
- Edit tools for start, goal, wall, erase, and weight tiles.
- Algorithm selection buttons for A*, Dijkstra, BFS, and Greedy.
- Auto-run, pause, next-step, previous-step, restart, and reset controls.
- Board locking while a simulation is running to keep the search state stable.
- Total step and path cost counters.
- Dev/debug mode with:
  - A* heuristic selector: Manhattan, Euclidean, Octile, Chebyshev.
  - Four-way or eight-way movement.
  - Auto-run speed slider.
  - Weight brush value from 1 to 10.
  - Cost and weight visualization toggles.
  - Current tile information: G/H/F cost, weight, status, and parent.

## Project Layout

```text
.
|-- main.cpp                         # Program entry point
|-- CMakeLists.txt                   # Root CMake project
|-- status.h                         # Shared enums for input, algorithms, state
|-- appliction/                      # Application loop and simulation controller
|-- Aframework/                      # Board, tile, button, text, and UI helpers
|-- algorithm/                       # Pathfinder interface and algorithm classes
|-- assets/                          # Fonts, textures, and sound assets
|-- imgui/                           # Dear ImGui source and SDL renderer backend
`-- thirdparty/                      # SDL2 headers, import libs, and DLLs
```

Note: the `appliction` directory name is used by the current CMake files and
source includes, so keep that spelling unless you update all references.

## Requirements

- Windows x64.
- Visual Studio 2022 or another C++17-capable compiler.
- CMake 3.12 or newer.

The repository already contains the SDL2 headers, import libraries, and x64
DLLs under `thirdparty/`, so no package manager setup is required for the
bundled Windows build.

## Build

From the repository root:

```powershell
cmake -S . -B build -A x64
cmake --build build --config Debug
```

For a Release build:

```powershell
cmake --build build --config Release
```

## Run

With the Visual Studio generator, the executable is typically created at:

```powershell
.\build\Debug\PathFindingVisualizer.exe
```

or:

```powershell
.\build\Release\PathFindingVisualizer.exe
```

The app loads assets from the `assets/` directory. The root CMake file copies
`assets/` into the build directory during configuration.

If Windows cannot find an SDL DLL when launching the executable, copy these
files from `thirdparty/lib/x64/` next to the executable or add that directory to
your `PATH`:

- `SDL2.dll`
- `SDL2_image.dll`
- `SDL2_mixer.dll`
- `SDL2_ttf.dll`

## How To Use

1. Select `Start`, then click a grid cell to place the start tile.
2. Select `Goal`, then click a grid cell to place the target tile.
3. Select `Wall` and drag/click on the grid to create blocked cells.
4. Select an algorithm. Use `BFS` for the currently working visualization.
5. Click `Next Step` to advance manually, or `Auto Run` to run continuously.
6. Use `Pause` to stop auto-run, `Prev Step` to undo one search step,
   `Restart` to clear only search progress, or `Reset` to clear the board.

During a search, board editing is locked. Use `Restart` or `Reset` before
changing tiles, algorithms, movement mode, or A* heuristic settings.

## Dev Mode

Click the small button in the lower-left corner to toggle the Dev Debug panel.

Dev mode exposes additional controls:

- Switch movement between four-way and eight-way neighbors.
- Change the auto-run speed.
- Edit tile weights with the weight brush.
- Show the weight heat map.
- Show per-tile cost values.
- Inspect current app state and selected tile data.

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

`SimulationController` owns the active pathfinder, advances it one step at a
time, saves history for undo, and locks/unlocks board editing around the search
lifecycle.

To add or finish an algorithm:

1. Implement `next_step()` in the corresponding file under `algorithm/`.
2. Preserve enough internal state for `clone()` to support previous-step undo.
3. Use `Board::neighbors(...)` so four-way/eight-way movement works
   consistently.
4. Set tile parents and G/H/F costs so the board can render direction arrows,
   costs, and final path reconstruction.
5. Call `mark_finished(true)` when a path is found, or `mark_finished(false)`
   when the frontier is exhausted.

## Assets And Third-Party Code

- Textures, sounds, and fonts live in `assets/`.
- Dear ImGui sources live in `imgui/`.
- SDL2 headers, libraries, DLLs, and related optional DLLs live in
  `thirdparty/`.

See `LICENSE.txt` and `imgui/LICENSE.txt` for license information included in
this repository.
