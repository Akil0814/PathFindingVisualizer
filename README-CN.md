# PathFindingVisualizer

`PathFindingVisualizer` 是一个使用 C++17、SDL2 和 Dear ImGui 开发的桌面寻路算法可视化项目。

在 20 x 20 的网格中，你可以放置起点、终点、墙体，设置格子权重，并通过单步执行或自动运行观察算法如何搜索路径。

## 算法相关

- A*、Dijkstra、BFS、Greedy Best-First Search 的完整默认实现位于 `algorithm/impl/`。
- `CustomPathfinder` 未提供任何内置实现，可用于你自己实验算法。该算法只能从 ImGui 的 Dev Options 面板中选择启用。
- 根目录下的 `algorithm/*_pathfinder.cpp` 是学习用空框架，只在 `PATHFINDER_USE_CUSTOM_IMPLEMENTATIONS=ON` 时参与编译。
- 默认构建会使用 `algorithm/impl/` 中提供的算法的完整实现。

## 功能

### 默认支持

- 20 x 20 可视化网格棋盘。
- 支持设置起点、终点、墙体、以及棋盘格权重。
- 左侧按钮支持选择 A*、Dijkstra、BFS、Greedy。
- 支持自动运行、暂停、下一步、上一步、重启搜索和重置棋盘。
- 当鼠标悬停于某一格子上时会在左上角显示当前格子详细信息。
- 显示总搜索步数、最终路径步数和最终路径总代价。
- Open、Current、Path、Closed 等方向纹理会根据 parent 链指向父级格子。

### 开发者功能

#### 点击左下角小扳手可呼出 ImGui 调试窗口

Dev 模式会增加按钮：
- 支持显示棋盘格权重。
- 支持编辑棋盘格权重设置。
- 支持显示每个访问过的棋盘格的代价。

ImGui 窗口：
- 支持选择 Custom 自定义算法。
- 支持选择 A* 启发函数。
- 支持四方向和八方向移动的选择。
- 八方向移动时支持三种斜角策略：
  - `Strict No Corner Cutting`：只要两个侧边格子中有一个是墙，就禁止斜走。
  - `No Corner Cutting`：只有两个侧边格子都是墙时，才禁止斜走。
  - `Allow Corner Cutting`：不检查侧边格子，允许斜角穿过。
- 可编辑单格权重数值（1-10）。
- 可设置全局单步移动代价（直走/斜走），默认直走 `10 * weight`，斜走 `14 * weight`。
- 可设置自动运行速度。
- 可查看全局状态机信息。

## 项目结构

```text
.
|-- main.cpp                         # 程序入口
|-- CMakeLists.txt                   # 根 CMake 配置
|-- status.h                         # 输入、算法、状态、移动模式、启发函数等共享枚举
|-- application/                      # 应用主循环和模拟控制器
|-- Aframework/                      # 棋盘、格子、按钮、文字纹理和 UI 基础组件
|-- algorithm/                       # Pathfinder 基类、学习空框架和算法说明
|-- algorithm/impl/                  # 默认完整算法实现
|-- assets/                          # 字体、贴图和音效资源
|-- imgui/                           # Dear ImGui 源码和 SDL 渲染后端
|-- thirdparty/                      # SDL2 头文件、导入库和 DLL
`-- utils/                           # 通用显示字符串工具
```

## 环境要求

- Windows x64。
- Visual Studio 2022，或其他支持 C++17 的编译器。
- CMake 3.12 或更高版本。

仓库已经在 `thirdparty/` 中包含 Windows 构建需要的 SDL2 头文件、导入库和 DLL，因此当前工程不需要额外通过包管理器安装 SDL。

- 已通过 MSVC 与 MinGW 编译测试。

- 非 Windows 构建会通过 pkg-config 查找 SDL2 依赖；macOS 尚未进行编译测试。

## 构建

默认模式使用 `algorithm/impl/` 中的完整实现：

```powershell
cmake -S . -B build -A x64 -DPATHFINDER_USE_CUSTOM_IMPLEMENTATIONS=OFF
cmake --build build --config Debug
```

学习模式使用根目录下的 `algorithm/*_pathfinder.cpp` 空框架：

```powershell
cmake -S . -B build -A x64 -DPATHFINDER_USE_CUSTOM_IMPLEMENTATIONS=ON
cmake --build build --config Debug
```

构建 Release 版本：

```powershell
cmake --build build --config Release
```

## 运行

使用 Visual Studio 生成器时，Debug 可执行文件通常位于：

```powershell
.\build\Debug\PathFindingVisualizer.exe
```

Release 可执行文件通常位于：

```powershell
.\build\Release\PathFindingVisualizer.exe
```

程序会从 `assets/` 加载字体、贴图和音效。根 CMake 配置会在配置阶段把 `assets/` 复制到构建目录。

如果运行时提示找不到 SDL DLL，可以把以下文件从 `thirdparty/lib/msvc/` 复制到 exe 同级目录，或者把该目录加入 `PATH`：

- `SDL2.dll`
- `SDL2_image.dll`
- `SDL2_mixer.dll`
- `SDL2_ttf.dll`

## 使用方式

1. 选择 `Start`，再点击棋盘格子设置起点。
2. 选择 `Goal`，再点击棋盘格子设置终点。
3. 选择 `Wall`，在棋盘上点击或拖拽绘制障碍。
4. 选择 `Edit Weight`，或者使用 Dev Options 中的权重画笔绘制权重格子。
5. 选择算法。
6. 点击 `Next Step` 单步推进，或者点击 `Auto Run` 自动运行。
7. 使用 `Pause` 暂停自动运行，`Prev Step` 回退一步，`Restart` 清除搜索过程，`Reset` 清空整个棋盘。

搜索开始后，棋盘编辑会被锁定。需要修改格子、算法、移动模式、斜角策略或 A* 启发函数时，需先点击 `Restart` 或 `Reset`。

## 统计数字

- `Total Steps`：控制器已经执行的算法步数。
- `Path Steps`：最终 parent 链路径包含的移动步数。
- `Total Cost`：最终 parent 链路径总代价，使用当前配置的直走/斜走移动代价乘以格子权重。

如果没有找到路径，`Path Steps` 和 `Total Cost` 会保持为 `0`。

## 算法说明

所有寻路算法都实现统一的 `Pathfinder` 接口：

```cpp
class Pathfinder
{
public:
    virtual void next_step() = 0;
    virtual std::unique_ptr<Pathfinder> clone() const = 0;
};
```

算法类继承 `CloneablePathfinder<Derived>` 后，会自动完成 clone 操作，以保存算法状态快照用于回退。`SimulationController` 持有当前算法实例，负责单步推进、自动播放、保存历史、支持上一步回退，并在搜索期间锁定棋盘编辑。

读取起点终点、获取邻居、移动代价、启发式代价、标记格子、关闭当前格子、重建最终路径等公共逻辑都放在 `Pathfinder` 基类中。

更多实现细节请阅读算法说明：

- [algorithm/README-EN.md](algorithm/README-EN.md)
- [algorithm/README-CN.md](algorithm/README-CN.md)

## 资源和第三方代码

- 字体、贴图和音效位于 `assets/`。
- Dear ImGui 源码位于 `imgui/`。
- SDL2 头文件、库、DLL 等位于 `thirdparty/`。

许可证信息见 `LICENSE.txt` 和 `imgui/LICENSE.txt`。
