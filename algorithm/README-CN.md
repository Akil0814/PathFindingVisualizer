# 寻路算法实现指南

这个目录保存 `Pathfinder` 基类、学习用空框架，以及默认算法实现。

项目结构的目标是让算法学习者只关注搜索逻辑本身，而不用处理 SDL 渲染、按钮事件、播放速度、暂停、重置或回退逻辑。

## 源文件选择

`algorithm/CMakeLists.txt` 会在 CMake 配置阶段选择要编译的算法源文件。

- 默认模式：`PATHFINDER_USE_CUSTOM_IMPLEMENTATIONS=OFF`
  - 如果 `algorithm/impl/*_pathfinder_impl.cpp` 存在，就编译 impl 中的完整实现。
  - 如果某个算法没有 impl 文件，就回退到根目录下的 `algorithm/*_pathfinder.cpp`。
  - `CustomPathfinder` 就是这样参与编译的：它故意没有 impl 文件。
- 学习模式：`PATHFINDER_USE_CUSTOM_IMPLEMENTATIONS=ON`
  - 编译根目录下的 `algorithm/*_pathfinder.cpp`。
  - 这些文件是有意保留的空框架，方便你自己实验。

示例：

```powershell
cmake -S . -B build -DPATHFINDER_USE_CUSTOM_IMPLEMENTATIONS=OFF
cmake --build build --config Debug
```

## 文件职责

- `path_finder.h` / `path_finder.cpp`：所有算法共用的基类和 helper。
- `a_star_pathfinder.h`、`bfs_pathfinder.h`、`dijkstra_pathfinder.h`、`greedy_pathfinder.h`：算法状态和类声明。
- `custom_pathfinder.h` / `custom_pathfinder.cpp`：故意留空的用户自定义算法。
- `*_pathfinder.cpp`：学习模式空框架，在 `PATHFINDER_USE_CUSTOM_IMPLEMENTATIONS=ON` 时编译。
- `impl/*_pathfinder_impl.cpp`：默认完整实现，在 `PATHFINDER_USE_CUSTOM_IMPLEMENTATIONS=OFF` 时编译。

## 已实现算法

- A*：优先队列按 `f = g + h` 排序，启发函数可通过 controller/UI 选择。
- Dijkstra：优先队列按当前已知最小 `g` 代价排序。
- BFS：FIFO 队列推进，找到的是最少移动步数路径，不一定是带权总代价最低路径。
- Greedy Best-First Search：优先队列按启发式距离排序，适合观察搜索方向，但不保证最优路径。
- Custom：给用户写自定义算法的空框架，只能从 ImGui Dev Options 面板选择。

## 算法类接口约定

每个算法类都应该继承 `CloneablePathfinder<YourPathfinder>`：

```cpp
class YourPathfinder final : public CloneablePathfinder<YourPathfinder>
{
public:
    void next_step() override;
};
```

`next_step()` 的职责是推进“一步”可视化搜索，而不是一次跑完整个算法。这样 `SimulationController` 才能支持 `Next Step`、`Auto Run`、`Pause` 和 `Prev Step`。

`CloneablePathfinder` 会自动复制算法内部状态。`SimulationController` 会在每一步之前保存算法副本和棋盘快照，用于上一步回退。

## Pathfinder 共用 helper

优先使用这些基类 helper，不要在每个算法里重复写棋盘逻辑：

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

这些 helper 会统一处理棋盘边界、起点终点保护、斜角策略、格子权重、可视化状态和最终路径重建。

## 需要同步的状态

### 1. 算法完成状态

不要让算法结束后继续修改棋盘。`next_step()` 开头通常需要：

```cpp
if (is_finished())
    return;
```

找到路径时调用：

```cpp
mark_finished(rebuild_path(_start, _goal));
```

frontier/open set 耗尽且没有找到路径时调用：

```cpp
mark_finished(false);
```

`SimulationController` 会用这个状态停止自动运行、更新最终路径总代价和路径步数，并在 UI 中显示完成状态。

### 2. 起点和终点

初始化时使用共用 helper 读取和校验：

```cpp
if (!read_endpoints(_start, _goal))
{
    mark_finished(false);
    return;
}
```

虽然 UI 会尽量阻止缺少起点/终点时运行，但算法内部仍然应该做防御性检查。

### 3. frontier / open set / visited / best-cost 状态

这些是算法自己的内部状态，不应该放进 `Board`。

常见对应关系：

- BFS：`std::queue<Point>` 作为 frontier，配合 `visited[y][x]`。
- Dijkstra：优先队列按最小 `g_cost` 取点，配合 `best_cost[y][x]`。
- A*：优先队列按最小 `f_cost = g_cost + h_cost` 取点，配合 `best_cost[y][x]`。
- Greedy：优先队列按最小 `h_cost` 取点，配合 `visited[y][x]`。

这些容器必须是算法类的成员变量，这样自动 clone 才能完整复制当前搜索进度。

### 4. Tile::Status 可视化状态

棋盘通过 `Tile::Status` 决定格子的显示方式。算法实现需要谨慎维护这些状态：

- `Start`：起点，不要覆盖成 `Open`、`Closed`、`Current` 或 `Path`。
- `Goal`：终点，不要覆盖成 `Open`、`Closed`、`Current` 或 `Path`。
- `Wall`：障碍，不要加入 frontier，也不要改写状态。
- `Open`：已经发现，等待处理。
- `Current`：当前这一步正在展开的格子。
- `Closed`：已经展开完成的格子。
- `Path`：最终路径上的格子，通常不包括起点和终点。
- `Empty`：普通可通行格子。

推荐流程：

1. 每一步开始时调用 `close_current_tile(_current)`。
2. 从 frontier/open set 中取出新的 current。
3. 如果 current 不是起点或终点，调用 `mark_tile_current(current)`。
4. 遍历邻居，把新发现或被更优路径更新的格子用 `mark_tile_open(next)` 标记。
5. 找到终点时调用 `mark_finished(rebuild_path(_start, _goal))`。

### 5. parent 回溯指针

当某个邻居应该通过当前节点到达时，调用：

```cpp
set_tile_parent(next, current);
```

最终路径重建依赖 parent 链。带方向的纹理也会根据 parent 链指向父级格子。

起点初始化时通常应该清空路径数据：

```cpp
clear_tile_path_data(_start);
```

### 6. G/H/F 代价

`Tile` 中有三个公开代价字段：

```cpp
int _g_cost = 0;
int _h_cost = 0;
int _f_cost = 0;
```

建议约定：

- `_g_cost`：从起点走到当前格子的已知代价。
- `_h_cost`：从当前格子估计到终点的启发式代价。
- `_f_cost`：用于显示或排序的总代价，通常是 `g + h`。

使用共用 setter：

```cpp
set_tile_costs(next, next_g_cost, next_h_cost);
```

移动代价已经集中在基类中：

```cpp
const int next_g_cost = current_tile._g_cost + movement_cost(current, next);
```

移动代价由 `MovementCostConfig` 控制：

```cpp
struct MovementCostConfig
{
    int straight = 10;
    int diagonal = 14;
};
```

当前默认代价规则：

- 直走：`10 * tile_weight(destination)`。
- 斜走：`14 * tile_weight(destination)`。

不同算法可以这样使用：

- BFS：记录 `g`，`h = 0`，`f = g`，但队列顺序仍然按移动步数推进。
- Dijkstra：记录 `g`，`h = 0`，`f = g`，按最小 `g` 取点。
- A*：记录 `g`、`h`、`f = g + h`，按最小 `f` 取点。
- Greedy：记录 `g` 用于显示和路径代价，但按最小 `h` 取点。

### 7. 四方向 / 八方向移动和斜角策略

不要在算法里手写邻居方向，统一使用：

```cpp
for (const Point next : neighbors(current))
{
    // ...
}
```

`Pathfinder::neighbors(...)` 会转交给 `Board::neighbors(point, move_mode(), diagonal_policy())`。

`Board::neighbors(...)` 会过滤越界格子、墙体，以及根据当前 `DiagonalMovePolicy` 过滤斜角移动：

- `BlockIfEitherSideBlocked`：严格防止斜角穿墙。
- `BlockIfBothSidesBlocked`：较宽松地防止斜角穿墙。
- `IgnoreSideBlocks`：不管侧边墙体，允许斜角穿过。

算法仍然需要处理自己的 visited、closed 或更优路径判断。

### 8. 启发函数

`Pathfinder::heuristic_cost(...)` 使用当前配置的移动代价尺度：

- Manhattan：`straight * (dx + dy)`。
- Euclidean：`round(straight * sqrt(dx*dx + dy*dy))`。
- Octile：`effective_diagonal * min(dx, dy) + straight * (max(dx, dy) - min(dx, dy))`。
- Chebyshev：`straight * max(dx, dy)`。

`effective_diagonal` 最多会取到 `2 * straight`，这样在用户实验奇怪数值时启发函数更安全。对于八方向 A*，Octile 通常最贴合默认的 10/14 移动模型。

### 9. 上一步回退所需状态

`Prev Step` 依赖两个快照：

- `Board::save_snapshot()` 保存棋盘格子状态。
- `CloneablePathfinder` 通过自动 `clone()` 保存算法内部状态。

因此，任何影响搜索进度的数据都必须放在算法类成员变量中，并且能被拷贝，例如：

- 是否已经初始化。
- frontier/open set。
- visited 或 best-cost 数组。
- 当前节点。
- 起点和终点。
- 算法特有的优先队列或 tie-breaker 信息。

不要把关键状态放在 `next_step()` 的局部静态变量中，也不要依赖无法复制的外部对象。

### 10. 哪些事情不应该由算法处理

算法实现不要负责这些事情：

- 不要处理 SDL 事件。
- 不要直接渲染。
- 不要管理按钮、窗口或 ImGui。
- 不要调用 `Board::save_snapshot()`。
- 不要锁定或解锁棋盘编辑。
- 不要控制自动播放速度。
- 不要重置整张棋盘。

这些由 `Application`、`SimulationController` 和 `Board` 负责。

## 推荐实现步骤

1. 在算法类中添加成员变量，例如 `_initialized`、`_frontier` 或 `_open_set`、`_visited` 或 `_best_cost`、`_start`、`_goal`、`_current`。
2. 第一次进入 `next_step()` 时初始化起点、终点、frontier/open set 和 cost。
3. 每一步先调用 `close_current_tile(_current)`。
4. 如果 frontier/open set 为空，调用 `mark_finished(false)`。
5. 取出当前节点，更新它的可视化状态。
6. 遍历 `neighbors(current)`。
7. 根据算法规则更新邻居的 parent、cost、状态和搜索容器。
8. 找到终点后调用 `mark_finished(rebuild_path(_start, _goal))`。
9. 确认自动 clone 能保留所有搜索状态，然后测试 `Prev Step`。

## 常见坑

- 覆盖了 `Start` 或 `Goal`，导致起点终点被染成普通搜索状态。
- 只更新优先队列，没有同步 tile 的 parent 和 `g/h/f`，导致路径回溯或代价显示错误。
- 找到终点后没有调用 `mark_finished(...)`，自动运行不会正确停止。
- frontier 为空时没有调用 `mark_finished(false)`，搜索会卡在运行状态。
- 内部状态没有放到成员变量，导致 `Prev Step` 回退后算法状态和棋盘状态不同步。
- 八方向 A* 继续使用 Manhattan。Octile 通常更贴合当前移动模型。
