# PathFindingVisualizer

`PathFindingVisualizer` is a C++17 desktop path-finding visualizer built with SDL2 and Dear ImGui.

The project provides an interactive 20 x 20 grid where you can place a start tile, a goal tile, walls, and weighted tiles, then inspect the search process step by step or run it automatically.

### Current Status

- BFS has a working step-by-step visualization.
- A*, Dijkstra, and Greedy Best-First Search are wired into the UI, enums, and class structure, but their `next_step()` implementations are currently placeholders.
- The app already includes board editing, path playback, previous-step undo, weighted tile editing, four-way/eight-way movement, and an ImGui debug panel.

### Features

- Visual 20 x 20 grid board.
- Edit tools for start, goal, wall, erase, and weighted tiles.
- Algorithm buttons for A*, Dijkstra, BFS, and Greedy.
- Auto-run, pause, next-step, previous-step, restart, and reset controls.
- Board editing is locked during a search to keep the algorithm state stable.
- Total step and final path cost counters.
- Dev Debug mode supports:
  - A* heuristic selection: Manhattan, Euclidean, Octile, Chebyshev.
  - Four-way or eight-way movement.
  - Auto-run speed control.
  - Weight brush from 1 to 10.
  - Weight heat map display.
  - Per-tile cost display.
  - Current tile inspection: G/H/F, weight, status, and parent.

### Project Layout

```text
.
|-- main.cpp                         # Program entry point
|-- CMakeLists.txt                   # Root CMake configuration
|-- status.h                         # Shared enums for input, algorithms, and state
|-- appliction/                      # Application loop and simulation controller
|-- Aframework/                      # Board, tile, button, text texture, and UI helpers
|-- algorithm/                       # Pathfinder interface and algorithm classes
|-- assets/                          # Fonts, textures, and sound assets
|-- imgui/                           # Dear ImGui source and SDL renderer backend
`-- thirdparty/                      # SDL2 headers, import libraries, and DLLs
```

Note: the current directory name is `appliction`. The CMake files and source includes use this spelling, so keep it unless you update every reference.

### Requirements

- Windows x64.
- Visual Studio 2022, or another C++17-capable compiler.
- CMake 3.12 or newer.

The repository already includes the SDL2 headers, import libraries, and x64 DLLs under `thirdparty/`, so the bundled Windows build does not require extra SDL package manager setup.

### Build

From the repository root:

```powershell
cmake -S . -B build -A x64
cmake --build build --config Debug
```

For a Release build:

```powershell
cmake --build build --config Release
```

### Run

With the Visual Studio generator, the Debug executable is usually created at:

```powershell
.\build\Debug\PathFindingVisualizer.exe
```

The Release executable is usually created at:

```powershell
.\build\Release\PathFindingVisualizer.exe
```

The app loads fonts and textures from `assets/`. The root CMake file copies `assets/` into the build directory during configuration.

If Windows cannot find an SDL DLL at launch time, copy these files from `thirdparty/lib/x64/` next to the executable, or add that directory to your `PATH`:

- `SDL2.dll`
- `SDL2_image.dll`
- `SDL2_mixer.dll`
- `SDL2_ttf.dll`

### How To Use

1. Select `Start`, then click a grid cell to place the start tile.
2. Select `Goal`, then click a grid cell to place the target tile.
3. Select `Wall` and click or drag on the grid to create blocked cells.
4. Select an algorithm. Use `BFS` for the currently complete visualization.
5. Click `Next Step` to advance manually, or `Auto Run` to run continuously.
6. Use `Pause` to stop auto-run, `Prev Step` to undo one search step, `Restart` to clear search progress, or `Reset` to clear the whole board.

Once a search starts, board editing is locked. Click `Restart` or `Reset` before changing tiles, algorithms, movement mode, or A* heuristic settings.

### Dev Mode

Click the small button in the lower-left corner to toggle the Dev Debug panel.

Dev Mode lets you adjust auto-run speed, switch four-way/eight-way movement, choose the A* heuristic, edit weights, show the weight heat map, show per-tile costs, and inspect the current app state plus the tile under the mouse.

### Algorithm Implementation Notes

All pathfinders implement the shared `Pathfinder` interface:

```cpp
class Pathfinder
{
public:
    virtual void next_step() = 0;
    virtual std::unique_ptr<Pathfinder> clone() const = 0;
};
```

`SimulationController` owns the active pathfinder, advances it one step at a time, runs auto-play, saves history, supports previous-step undo, and locks board editing during the search.

If you want to implement your own path-finding logic, read [algorithm/README.md](algorithm/README.md). It explains which states must be kept in sync, including `Tile::Status`, `parent`, `G/H/F` costs, frontier/open set, visited/closed set, `mark_finished(...)`, and `clone()`.

### Assets And Third-Party Code

- Fonts, textures, and sounds live in `assets/`.
- Dear ImGui sources live in `imgui/`.
- SDL2 headers, libraries, DLLs, and optional DLLs live in `thirdparty/`.

See `LICENSE.txt` and `imgui/LICENSE.txt` for license information included in this repository.
