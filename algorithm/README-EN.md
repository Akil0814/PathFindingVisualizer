# Algorithm Implementation Guide

This directory contains the shared pathfinder base class, learning stubs, and default algorithm implementations.

The architecture is designed so algorithm learners can focus on search logic instead of SDL rendering, button events, playback speed, pause, reset, or undo behavior.

## Source Selection

`algorithm/CMakeLists.txt` selects algorithm source files at configure time.

- Default mode: `PATHFINDER_USE_CUSTOM_IMPLEMENTATIONS=OFF`
  - Builds `algorithm/impl/*_pathfinder_impl.cpp` when an implementation exists.
  - Falls back to the root `algorithm/*_pathfinder.cpp` file when no impl file exists.
  - This is why `CustomPathfinder` still builds: it intentionally has no `impl` file.
- Learning mode: `PATHFINDER_USE_CUSTOM_IMPLEMENTATIONS=ON`
  - Builds the root `algorithm/*_pathfinder.cpp` files.
  - These files are intentionally small empty frameworks for experiments.

Example:

```powershell
cmake -S . -B build -DPATHFINDER_USE_CUSTOM_IMPLEMENTATIONS=OFF
cmake --build build --config Debug
```

## File Responsibilities

- `path_finder.h` / `path_finder.cpp`: shared base class and helper functions used by every algorithm.
- `a_star_pathfinder.h`, `bfs_pathfinder.h`, `dijkstra_pathfinder.h`, `greedy_pathfinder.h`: algorithm state and public class declarations.
- `custom_pathfinder.h` / `custom_pathfinder.cpp`: intentionally empty user-defined algorithm.
- `*_pathfinder.cpp`: learning-mode stubs compiled when `PATHFINDER_USE_CUSTOM_IMPLEMENTATIONS=ON`.
- `impl/*_pathfinder_impl.cpp`: default complete implementations compiled when `PATHFINDER_USE_CUSTOM_IMPLEMENTATIONS=OFF`.

## Implemented Algorithms

- A*: priority queue ordered by `f = g + h`. It supports selectable heuristics through the controller.
- Dijkstra: priority queue ordered by the lowest known `g` cost.
- BFS: FIFO frontier. It finds the shortest path by step count, not necessarily the lowest weighted cost.
- Greedy Best-First Search: priority queue ordered by heuristic distance. It is fast to visualize but not guaranteed to find the optimal path.
- Custom: empty framework for user code, selectable from the ImGui Dev Options panel.

## Required Interface Contract

Every algorithm class should inherit from `CloneablePathfinder<YourPathfinder>`:

```cpp
class YourPathfinder final : public CloneablePathfinder<YourPathfinder>
{
public:
    void next_step() override;
};
```

`next_step()` advances one visual search step, not the whole search. This is what allows `SimulationController` to support `Next Step`, `Auto Run`, `Pause`, and `Prev Step`.

`CloneablePathfinder` copies the algorithm state automatically. `SimulationController` saves an algorithm clone and a board snapshot before every step so it can undo one step.

## Shared Pathfinder Helpers

Prefer these base-class helpers instead of duplicating board logic in each algorithm:

```cpp
bool read_endpoints(Point& start, Point& goal) const;
std::vector<Point> neighbors(Point point) const;
bool same_point(Point lhs, Point rhs) const;
bool is_start_or_goal(Point point) const;
int tile_weight(Point point) const;
int movement_cost(Point from, Point to) const;
int heuristic_cost(Point from, Point to, HeuristicMode mode) const;

void clear_tile_path_data(Point point);
void set_tile_parent(Point child, Point parent);
void set_tile_costs(Point point, int g_cost, int h_cost);
void mark_tile_current(Point point);
void mark_tile_open(Point point);
void mark_tile_closed(Point point);
void mark_tile_path(Point point);
void close_current_tile(Point& current);
bool rebuild_path(Point start, Point goal);
```

These helpers keep all algorithms consistent with board bounds, start/goal protection, diagonal movement policy, tile weights, visual states, and final path reconstruction.

## States You Must Keep In Sync

### 1. Algorithm completion state

Do not keep mutating the board after the algorithm has finished. A typical `next_step()` starts with:

```cpp
if (is_finished())
    return;
```

When a path is found, call:

```cpp
mark_finished(rebuild_path(_start, _goal));
```

When the frontier/open set is exhausted without finding a path, call:

```cpp
mark_finished(false);
```

`SimulationController` uses this state to stop auto-run, update final path cost and path steps, and show the finished state in the UI.

### 2. Start and goal

Use the shared endpoint helper during initialization:

```cpp
if (!read_endpoints(_start, _goal))
{
    mark_finished(false);
    return;
}
```

The UI tries to prevent running without a start or goal, but each algorithm should still be defensive.

### 3. Frontier / open set / visited / best-cost state

These are internal algorithm states and should not live in `Board`.

Common mappings:

- BFS: `std::queue<Point>` frontier plus `visited[y][x]`.
- Dijkstra: priority queue ordered by smallest `g_cost`, plus `best_cost[y][x]`.
- A*: priority queue ordered by smallest `f_cost = g_cost + h_cost`, plus `best_cost[y][x]`.
- Greedy: priority queue ordered by smallest `h_cost`, plus `visited[y][x]`.

These containers must be member variables so automatic cloning can copy the current search progress.

### 4. Tile::Status visualization state

The board renders each cell according to `Tile::Status`. Algorithm implementations must update these states carefully:

- `Start`: starting tile. Do not overwrite it with `Open`, `Closed`, `Current`, or `Path`.
- `Goal`: target tile. Do not overwrite it with `Open`, `Closed`, `Current`, or `Path`.
- `Wall`: blocked tile. Do not add it to the frontier, and do not overwrite it.
- `Open`: discovered and waiting to be processed.
- `Current`: the tile expanded in the current visual step.
- `Closed`: a tile that has already been expanded.
- `Path`: a tile on the final path, usually excluding start and goal.
- `Empty`: a normal walkable tile.

Recommended flow:

1. At the start of each step, call `close_current_tile(_current)`.
2. Pop or select the next current tile from the frontier/open set.
3. If current is not start or goal, call `mark_tile_current(current)`.
4. Visit neighbors and mark newly discovered or improved tiles with `mark_tile_open(next)`.
5. When the goal is found, call `mark_finished(rebuild_path(_start, _goal))`.

### 5. Parent links

Whenever a neighbor should be reached through the current node, call:

```cpp
set_tile_parent(next, current);
```

Final path reconstruction depends on parent links. Directional textures also use parent links to point toward the parent tile.

The start tile should usually clear its path data during initialization:

```cpp
clear_tile_path_data(_start);
```

### 6. G/H/F costs

`Tile` exposes three cost fields:

```cpp
int _g_cost = 0;
int _h_cost = 0;
int _f_cost = 0;
```

Recommended meaning:

- `_g_cost`: known cost from the start tile to this tile.
- `_h_cost`: heuristic estimate from this tile to the goal.
- `_f_cost`: display or ordering cost, usually `g + h`.

Use the shared setter:

```cpp
set_tile_costs(next, next_g_cost, next_h_cost);
```

Movement cost is centralized:

```cpp
const int next_g_cost = current_tile._g_cost + movement_cost(current, next);
```

Current cost scale:

- Straight move: `10 * tile_weight(destination)`.
- Diagonal move: `14 * tile_weight(destination)`.

Different algorithms use costs like this:

- BFS: records `g`, `h = 0`, `f = g`, but queue order is still by step count.
- Dijkstra: records `g`, `h = 0`, `f = g`, and orders by smallest `g`.
- A*: records `g`, `h`, `f = g + h`, and orders by smallest `f`.
- Greedy: records `g` for display and path cost, but orders by smallest `h`.

### 7. Four-way / eight-way movement and diagonal policy

Do not hard-code neighbor directions in an algorithm. Use:

```cpp
for (const Point next : neighbors(current))
{
    // ...
}
```

`Pathfinder::neighbors(...)` delegates to `Board::neighbors(point, move_mode(), diagonal_policy())`.

`Board::neighbors(...)` filters out-of-bounds cells, walls, and diagonal moves according to the active `DiagonalMovePolicy`:

- `BlockIfEitherSideBlocked`: strict no corner cutting.
- `BlockIfBothSidesBlocked`: looser no corner cutting.
- `IgnoreSideBlocks`: allow corner cutting.

The algorithm still needs to handle its own visited, closed, or better-path checks.

### 8. Heuristics

`Pathfinder::heuristic_cost(...)` uses the same 10/14 scale as movement cost:

- Manhattan: `10 * (dx + dy)`.
- Euclidean: `round(10 * sqrt(dx*dx + dy*dy))`.
- Octile: `14 * min(dx, dy) + 10 * (max(dx, dy) - min(dx, dy))`.
- Chebyshev: `10 * max(dx, dy)`.

For eight-way A*, Octile is usually the best match for the current 10/14 movement model.

### 9. State required for previous-step undo

`Prev Step` depends on two snapshots:

- `Board::save_snapshot()` stores board cell state.
- `CloneablePathfinder` stores internal algorithm state through automatic `clone()`.

Therefore, every piece of data that affects search progress must be a copyable member variable of the algorithm class, for example:

- Whether the algorithm has been initialized.
- Frontier/open set.
- Visited or best-cost arrays.
- Current node.
- Start and goal.
- Algorithm-specific priority queues or tie-breaker data.

Do not store essential state in local static variables inside `next_step()`, and do not depend on external objects that cannot be copied.

### 10. What algorithms should not handle

Algorithm implementations should not:

- Handle SDL events.
- Render anything directly.
- Manage buttons, windows, or ImGui.
- Call `Board::save_snapshot()`.
- Lock or unlock board editing.
- Control auto-run speed.
- Reset the whole board.

Those responsibilities belong to `Application`, `SimulationController`, and `Board`.

## Recommended Implementation Steps

1. Add member variables such as `_initialized`, `_frontier` or `_open_set`, `_visited` or `_best_cost`, `_start`, `_goal`, and `_current`.
2. On the first `next_step()`, initialize start, goal, frontier/open set, and costs.
3. At each step, call `close_current_tile(_current)`.
4. If the frontier/open set is empty, call `mark_finished(false)`.
5. Select the current node and update its visualization state.
6. Iterate through `neighbors(current)`.
7. According to the algorithm rules, update each neighbor's parent, costs, status, and search container.
8. When the goal is found, call `mark_finished(rebuild_path(_start, _goal))`.
9. Confirm that automatic cloning preserves all search state, then test `Prev Step`.

## Common Pitfalls

- Overwriting `Start` or `Goal` with ordinary search statuses.
- Updating only the priority queue but not tile parent and `g/h/f`, which breaks path reconstruction or cost display.
- Forgetting `mark_finished(...)` after reaching the goal, so auto-run does not stop correctly.
- Forgetting `mark_finished(false)` when the frontier is empty, so the simulation gets stuck in a running state.
- Keeping search state outside member variables, causing board state and algorithm state to diverge after `Prev Step`.
- Using Manhattan heuristic in eight-way A*. Octile usually matches the current movement model better.
