# PathFindingVisualizer

`PathFindingVisualizer` 是一个基于 C++17、SDL2 和 Dear ImGui 的桌面寻路算法可视化项目。

项目提供一个可交互的 20 x 20 网格。你可以放置起点、终点、墙体和权重格子，然后通过单步执行或自动执行观察寻路算法的搜索过程。

### 当前状态

- BFS 已经有完整的单步可视化实现。
- A*、Dijkstra、Greedy Best-First Search 已经接入 UI、枚举和类结构，但对应的 `next_step()` 当前还是占位实现。
- 项目已经具备棋盘编辑、路径播放、上一步回退、权重编辑、四方向/八方向移动、ImGui 调试面板等功能。

### 功能特性

- 20 x 20 可视化网格棋盘。
- 支持放置起点、终点、墙体、擦除格子和权重格子。
- 支持选择 A*、Dijkstra、BFS、Greedy 四种算法入口。
- 支持自动运行、暂停、下一步、上一步、重启搜索和重置棋盘。
- 搜索过程中会锁定棋盘编辑，避免搜索状态被破坏。
- 显示总步数和最终路径总代价。
- Dev Debug 模式支持：
  - A* 启发式切换：Manhattan、Euclidean、Octile、Chebyshev。
  - 四方向/八方向移动模式切换。
  - 自动运行速度调节。
  - 权重画笔，权重范围为 1 到 10。
  - 权重热力图显示。
  - 格子代价显示。
  - 查看当前格子的 G/H/F、weight、status 和 parent。

### 项目结构

```text
.
|-- main.cpp                         # 程序入口
|-- CMakeLists.txt                   # 根 CMake 配置
|-- status.h                         # 输入类型、算法类型、运行状态等共享枚举
|-- appliction/                      # 应用主循环和模拟控制器
|-- Aframework/                      # 棋盘、格子、按钮、文字纹理等 UI 基础组件
|-- algorithm/                       # 寻路算法接口和具体算法类
|-- assets/                          # 字体、贴图和音效资源
|-- imgui/                           # Dear ImGui 源码及 SDL 渲染后端
`-- thirdparty/                      # SDL2 头文件、导入库和 DLL
```

注意：当前目录名是 `appliction`，CMake 和源码 include 都使用了这个拼写。如果要改名，需要同步修改所有引用。

### 环境要求

- Windows x64。
- Visual Studio 2022，或其他支持 C++17 的编译器。
- CMake 3.12 或更高版本。

仓库中已经包含 Windows x64 所需的 SDL2 头文件、导入库和 DLL，位于 `thirdparty/` 下，因此当前工程不需要额外通过包管理器安装 SDL。

### 构建

在仓库根目录执行：

```powershell
cmake -S . -B build -A x64
cmake --build build --config Debug
```

构建 Release 版本：

```powershell
cmake --build build --config Release
```

### 运行

使用 Visual Studio 生成器时，Debug 可执行文件通常位于：

```powershell
.\build\Debug\PathFindingVisualizer.exe
```

Release 可执行文件通常位于：

```powershell
.\build\Release\PathFindingVisualizer.exe
```

程序会从 `assets/` 目录加载字体和贴图。根 CMake 配置会在配置阶段把 `assets/` 复制到构建目录。

如果运行时提示找不到 SDL DLL，可以把以下文件从 `thirdparty/lib/x64/` 复制到 exe 同级目录，或者把该目录加入 `PATH`：

- `SDL2.dll`
- `SDL2_image.dll`
- `SDL2_mixer.dll`
- `SDL2_ttf.dll`

### 使用方式

1. 点击 `Start`，再点击棋盘格子放置起点。
2. 点击 `Goal`，再点击棋盘格子放置终点。
3. 点击 `Wall`，在棋盘上点击或拖拽绘制障碍。
4. 选择算法。当前完整可运行的算法是 `BFS`。
5. 点击 `Next Step` 可以单步推进，点击 `Auto Run` 可以自动运行。
6. 使用 `Pause` 暂停自动运行，`Prev Step` 回退一步，`Restart` 清除搜索过程，`Reset` 清空整个棋盘。

搜索开始后，棋盘编辑会被锁定。需要修改格子、算法、移动模式或 A* 启发式时，请先点击 `Restart` 或 `Reset`。

### Dev Mode

点击左下角的小按钮可以打开或关闭 Dev Debug 面板。

Dev Mode 中可以调整自动运行速度、切换四方向/八方向移动、选择 A* 启发式、编辑权重、显示权重热力图、显示格子代价，并查看当前应用状态和鼠标悬停格子的详细信息。

### 算法实现说明

所有寻路算法都实现统一的 `Pathfinder` 接口：

```cpp
class Pathfinder
{
public:
    virtual void next_step() = 0;
    virtual std::unique_ptr<Pathfinder> clone() const = 0;
};
```

`SimulationController` 负责持有当前算法实例、推进单步、自动播放、保存历史记录、回退上一步，以及在搜索期间锁定棋盘编辑。

如果你想自己实现寻路逻辑，请阅读 [algorithm/README.md](algorithm/README.md)。那里说明了实现算法时需要同步哪些状态，例如 `Tile::Status`、`parent`、`G/H/F` 代价、frontier/open set、visited/closed set、`mark_finished(...)` 和 `clone()`。

### 资源和第三方代码

- 字体、贴图和音效位于 `assets/`。
- Dear ImGui 源码位于 `imgui/`。
- SDL2 头文件、库、DLL 以及可选 DLL 位于 `thirdparty/`。

许可证信息见 `LICENSE.txt` 和 `imgui/LICENSE.txt`。
