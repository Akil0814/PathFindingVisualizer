# Algorithm Implementation Guide / 寻路算法实现指南

## 中文

这个目录保存寻路算法的统一接口和具体算法实现。项目的架构目标是让算法学习者只关注“搜索逻辑本身”，而不用处理 SDL 渲染、按钮事件、播放速度、暂停、回退等应用层逻辑。

如果你要自己实现 A*、Dijkstra、Greedy、DFS 或其他寻路算法，最重要的是：`next_step()` 不应该一次跑完整个算法，而应该只推进一个可视化步骤，并同步棋盘上用于显示和回退的状态。

### 文件职责

- `path_finder.h`：所有算法的基类，定义 `next_step()`、`clone()`、`mark_finished(...)`、`move_mode()` 等公共约定。
- `bfs_pathfinder.*`：当前可参考的完整算法实现。
- `a_star_pathfinder.*`：A* 算法入口，目前 `next_step()` 是占位。
- `dijkstra_pathfinder.*`：Dijkstra 算法入口，目前 `next_step()` 是占位。
- `greedy_pathfinder.*`：Greedy Best-First Search 算法入口，目前 `next_step()` 是占位。

### 算法类必须遵守的接口约定

每个算法类都应该继承 `Pathfinder`：

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

`next_step()` 的职责是推进“一步”搜索，而不是完整搜索。这样 `SimulationController` 才能支持 `Next Step`、`Auto Run`、`Pause` 和 `Prev Step`。

`clone()` 必须复制算法的内部状态。`SimulationController` 会在每一步之前保存算法副本和棋盘快照，用于上一步回退。

### 需要同步的状态

#### 1. 算法完成状态

不要让算法结束后继续修改棋盘。`next_step()` 开头通常需要：

```cpp
if (is_finished())
    return;
```

找到路径时调用：

```cpp
mark_finished(true);
```

frontier/open set 被耗尽且没有找到路径时调用：

```cpp
mark_finished(false);
```

这个状态会被 `SimulationController` 用来停止自动运行、更新总路径代价，并在 UI 中显示 Finished Found 或 Finished No Path。

#### 2. 起点和终点

初始化时从棋盘读取：

```cpp
_start = current_board->get_start_point();
_goal = current_board->get_end_point();
```

然后必须检查：

```cpp
if (!current_board->in_bounds(_start) || !current_board->in_bounds(_goal))
{
    mark_finished(false);
    return;
}
```

虽然 UI 会尽量阻止缺少起点/终点的运行，但算法内部仍然要做防御性检查。

#### 3. frontier / open set / visited / closed set

这些是算法自己的内部状态，不应该放到 `Board` 里。

常见对应关系：

- BFS：`std::queue<Point>` 作为 frontier，`visited[y][x]` 记录是否访问过。
- Dijkstra：优先队列按当前最小 `g_cost` 取点。
- A*：优先队列按最小 `f_cost = g_cost + h_cost` 取点。
- Greedy：优先队列按最小 `h_cost` 取点。

这些容器必须是算法类的成员变量，这样 `clone()` 才能完整复制当前搜索进度。

#### 4. Tile::Status 可视化状态

棋盘通过 `Tile::Status` 决定格子的显示方式。算法实现需要及时维护这些状态：

- `Start`：起点。不要覆盖成 `Open`、`Closed`、`Current` 或 `Path`。
- `Goal`：终点。不要覆盖成 `Open`、`Closed`、`Current` 或 `Path`。
- `Wall`：障碍。不要加入 frontier，也不要改写状态。
- `Open`：已经发现，等待处理的格子。
- `Current`：当前这一步正在展开的格子。
- `Closed`：已经展开完成的格子。
- `Path`：最终路径上的格子，通常不包括起点和终点。
- `Empty`：普通可通行格子。

推荐流程：

1. 每一步开始时，把上一帧的 `Current` 改成 `Closed`。
2. 从 frontier/open set 中取出新的 current。
3. 如果 current 不是 `Start` 或 `Goal`，把它标记为 `Current`。
4. 遍历邻居，把新发现或被更优路径更新的格子标记为 `Open`。
5. 找到终点后，根据 `parent` 从终点回溯到起点，把中间格子标记为 `Path`。

#### 5. parent 回溯指针

每当你确认某个邻居应该通过当前节点到达时，需要同步：

```cpp
next_tile.set_parent(current);
```

最终路径重建依赖 `parent`。棋盘的方向箭头也依赖 `parent` 来决定贴图旋转方向。

起点初始化时建议清空 parent：

```cpp
start_tile.clear_parent();
```

#### 6. G/H/F 代价

`Tile` 中有三个公开代价字段：

```cpp
int _g_cost = 0;
int _h_cost = 0;
int _f_cost = 0;
```

建议约定：

- `_g_cost`：从起点走到当前格子的已知代价。
- `_h_cost`：从当前格子估计到终点的启发式代价。
- `_f_cost`：排序或显示用的总代价，通常是 `g + h`。

不同算法可以这样使用：

- BFS：`h = 0`，`f = g`。注意 BFS 的队列顺序按层数推进，不保证带权最短路。
- Dijkstra：`h = 0`，`f = g`，优先队列按 `g` 或 `f` 最小取点。
- A*：`f = g + h`，优先队列按 `f` 最小取点。
- Greedy：通常按 `h` 最小取点，可以仍然记录 `g` 方便显示路径成本。

权重来自：

```cpp
next_tile._weight
```

当前 BFS 示例使用：

```cpp
next_tile._g_cost = current_tile._g_cost + next_tile._weight;
next_tile._h_cost = 0;
next_tile._f_cost = next_tile._g_cost;
```

#### 7. 四方向/八方向移动

不要在算法中手写邻居方向，统一使用：

```cpp
for (const Point next : current_board->neighbors(current, move_mode()))
{
    // ...
}
```

这样 Dev Mode 中的 Four Way / Eight Way 切换才会对所有算法生效。

当前 `Board::neighbors(...)` 负责过滤越界格子和墙体。算法仍然需要处理自己的 visited、closed 或更优路径判断。

#### 8. 上一步回退所需状态

`Prev Step` 依赖两个快照：

- `Board::save_snapshot()` 保存棋盘格子状态。
- `Pathfinder::clone()` 保存算法内部状态。

因此，任何影响搜索进度的数据都必须放在算法类成员变量中，并且能被拷贝构造正确复制，例如：

- 是否已经初始化。
- frontier/open set。
- visited/closed 数组。
- 当前节点。
- 起点、终点。
- 当前算法特有的代价表、优先队列、tie-breaker 信息。

不要把关键状态放在 `next_step()` 的局部静态变量中，也不要依赖无法复制的外部对象。

#### 9. 哪些事情不应该由算法处理

算法实现不要负责这些事情：

- 不要处理 SDL 事件。
- 不要直接渲染。
- 不要管理按钮、窗口或 ImGui。
- 不要调用 `Board::save_snapshot()`。
- 不要锁定或解锁棋盘编辑。
- 不要控制自动播放速度。
- 不要重置整张棋盘。

这些都由 `Application`、`SimulationController` 和 `Board` 负责。

### 推荐实现步骤

1. 在算法类中添加成员变量，例如 `_initialized`、`_frontier`、`_visited`、`_start`、`_goal`、`_current`。
2. 第一次进入 `next_step()` 时初始化起点、终点、frontier 和 cost。
3. 每一步先关闭上一轮的 `Current`。
4. 如果 frontier 为空，调用 `mark_finished(false)`。
5. 取出当前节点，更新它的可视化状态。
6. 遍历 `Board::neighbors(current, move_mode())`。
7. 根据算法规则判断是否更新邻居的 `parent`、`g/h/f` 和状态。
8. 找到终点后回溯 `parent`，标记 `Path`，再调用 `mark_finished(true)`。
9. 确认 `clone()` 能复制所有搜索状态，然后测试 `Prev Step`。

### 常见坑

- 忘记保护 `Start` 和 `Goal`，导致起点终点被染成普通搜索状态。
- 只更新了优先队列，没有同步 `Tile` 的 `parent` 和 `g/h/f`，导致路径回溯或成本显示错误。
- 找到终点后没有调用 `mark_finished(true)`，自动运行不会正确停止。
- frontier 为空时没有调用 `mark_finished(false)`，搜索会卡在运行状态。
- 内部状态没有放到成员变量，导致 `Prev Step` 回退后算法状态和棋盘状态不同步。
- 八方向移动时沿用了 Manhattan 启发式，可能影响 A* 的搜索性质。八方向通常更适合 Octile、Chebyshev 或 Euclidean。