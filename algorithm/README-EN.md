## English

This directory contains the shared pathfinder interface and concrete algorithm implementations. The architecture is designed so algorithm learners can focus on search logic instead of SDL rendering, button events, playback speed, pause, or undo behavior.

If you want to implement A*, Dijkstra, Greedy, DFS, or another path-finding algorithm, the most important rule is: `next_step()` should not run the whole algorithm at once. It should advance one visual step and keep the board state synchronized for rendering and undo.

### File Responsibilities

- `path_finder.h`: base class for every algorithm. It defines `next_step()`, `clone()`, `mark_finished(...)`, `move_mode()`, and the shared contract.
- `bfs_pathfinder.*`: the current complete reference implementation.
- `a_star_pathfinder.*`: A* entry point. `next_step()` is currently a placeholder.
- `dijkstra_pathfinder.*`: Dijkstra entry point. `next_step()` is currently a placeholder.
- `greedy_pathfinder.*`: Greedy Best-First Search entry point. `next_step()` is currently a placeholder.

### Required Interface Contract

Every algorithm class should inherit from `Pathfinder`:

```cpp
class YourPathfinder final : public Pathfinder
{
public:
    void next_step() override;
    [[nodiscard]] std::unique_ptr<Pathfinder> clone() const override
    {
        return std::make_unique<YourPathfinder>(*this);
    }
};
```

`next_step()` advances one search step, not the entire search. This is what allows `SimulationController` to support `Next Step`, `Auto Run`, `Pause`, and `Prev Step`.

`clone()` must copy the algorithm's internal state. `SimulationController` saves an algorithm clone and a board snapshot before every step so it can undo one step.

### States You Must Keep In Sync

#### 1. Algorithm completion state

Do not keep mutating the board after the algorithm has finished. A typical `next_step()` starts with:

```cpp
if (is_finished())
    return;
```

When a path is found, call:

```cpp
mark_finished(true);
```

When the frontier/open set is exhausted without finding a path, call:

```cpp
mark_finished(false);
```

`SimulationController` uses this state to stop auto-run, update final path cost, and show Finished Found or Finished No Path in the UI.

#### 2. Start and goal

Read them from the board during initialization:

```cpp
_start = current_board->get_start_point();
_goal = current_board->get_end_point();
```

Then validate them:

```cpp
if (!current_board->in_bounds(_start) || !current_board->in_bounds(_goal))
{
    mark_finished(false);
    return;
}
```

The UI tries to prevent running without a start or goal, but each algorithm should still be defensive.

#### 3. Frontier / open set / visited / closed set

These are internal algorithm states and should not live in `Board`.

Common mappings:

- BFS: `std::queue<Point>` as frontier, plus `visited[y][x]`.
- Dijkstra: priority queue ordered by the smallest current `g_cost`.
- A*: priority queue ordered by the smallest `f_cost = g_cost + h_cost`.
- Greedy: priority queue ordered by the smallest `h_cost`.

These containers must be member variables so `clone()` can copy the current search progress.

#### 4. Tile::Status visualization state

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

1. At the start of each step, change the previous `Current` tile to `Closed`.
2. Pop or select the next current tile from the frontier/open set.
3. If current is not `Start` or `Goal`, mark it as `Current`.
4. Visit neighbors and mark newly discovered or improved tiles as `Open`.
5. When the goal is found, follow `parent` links from goal to start and mark intermediate tiles as `Path`.

#### 5. Parent links

Whenever a neighbor should be reached through the current node, update:

```cpp
next_tile.set_parent(current);
```

Final path reconstruction depends on `parent`. Direction arrows also use `parent` to determine texture rotation.

The start tile should usually clear its parent during initialization:

```cpp
start_tile.clear_parent();
```

#### 6. G/H/F costs

`Tile` exposes three cost fields:

```cpp
int _g_cost = 0;
int _h_cost = 0;
int _f_cost = 0;
```

Recommended meaning:

- `_g_cost`: known cost from the start tile to this tile.
- `_h_cost`: heuristic estimate from this tile to the goal.
- `_f_cost`: total ordering/display cost, usually `g + h`.

Different algorithms can use them like this:

- BFS: `h = 0`, `f = g`. Note that BFS queue order is by depth, so it is not guaranteed to find the lowest-cost weighted path.
- Dijkstra: `h = 0`, `f = g`, priority queue ordered by the smallest `g` or `f`.
- A*: `f = g + h`, priority queue ordered by the smallest `f`.
- Greedy: usually ordered by the smallest `h`, while still recording `g` for path cost display.

Tile weight comes from:

```cpp
next_tile._weight
```

The current BFS example uses:

```cpp
next_tile._g_cost = current_tile._g_cost + next_tile._weight;
next_tile._h_cost = 0;
next_tile._f_cost = next_tile._g_cost;
```

#### 7. Four-way / eight-way movement

Do not hard-code neighbor directions in the algorithm. Use:

```cpp
for (const Point next : current_board->neighbors(current, move_mode()))
{
    // ...
}
```

This keeps the Dev Mode Four Way / Eight Way setting working for every algorithm.

`Board::neighbors(...)` filters out-of-bounds cells and walls. The algorithm still needs to handle its own visited, closed, or better-path checks.

#### 8. State required for previous-step undo

`Prev Step` depends on two snapshots:

- `Board::save_snapshot()` stores board cell state.
- `Pathfinder::clone()` stores internal algorithm state.

Therefore, every piece of data that affects search progress must be a copyable member variable of the algorithm class, for example:

- Whether the algorithm has been initialized.
- Frontier/open set.
- Visited/closed arrays.
- Current node.
- Start and goal.
- Algorithm-specific cost tables, priority queues, or tie-breaker data.

Do not store essential state in local static variables inside `next_step()`, and do not depend on external objects that cannot be copied.

#### 9. What algorithms should not handle

Algorithm implementations should not:

- Handle SDL events.
- Render anything directly.
- Manage buttons, windows, or ImGui.
- Call `Board::save_snapshot()`.
- Lock or unlock board editing.
- Control auto-run speed.
- Reset the whole board.

Those responsibilities belong to `Application`, `SimulationController`, and `Board`.

### Recommended Implementation Steps

1. Add member variables such as `_initialized`, `_frontier`, `_visited`, `_start`, `_goal`, and `_current`.
2. On the first `next_step()`, initialize start, goal, frontier, and costs.
3. At each step, close the previous `Current` tile.
4. If the frontier is empty, call `mark_finished(false)`.
5. Select the current node and update its visualization state.
6. Iterate through `Board::neighbors(current, move_mode())`.
7. According to the algorithm rules, update each neighbor's `parent`, `g/h/f`, and status.
8. When the goal is found, rebuild the path through `parent`, mark `Path`, and call `mark_finished(true)`.
9. Confirm that `clone()` copies all search state, then test `Prev Step`.

### Common Pitfalls

- Overwriting `Start` or `Goal` with ordinary search statuses.
- Updating only the priority queue but not `Tile` parent and `g/h/f`, which breaks path reconstruction or cost display.
- Forgetting `mark_finished(true)` after reaching the goal, so auto-run does not stop correctly.
- Forgetting `mark_finished(false)` when the frontier is empty, so the simulation gets stuck in a running state.
- Keeping search state outside member variables, causing board state and algorithm state to diverge after `Prev Step`.
- Using Manhattan heuristic in eight-way movement. For eight-way A*, Octile, Chebyshev, or Euclidean is usually a better fit.
