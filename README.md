# PathFindingVisualizer

[English](README-EN.md) / [中文](README-CN.md)

`PathFindingVisualizer` is a C++17 desktop path-finding visualizer built with SDL2 and Dear ImGui.

`PathFindingVisualizer` 是一个使用 C++17、SDL2 和 Dear ImGui 构建的桌面寻路算法可视化项目。

## Quick Links / 快速链接

- Project details: [PathFindingVisualizer](https://akil0814.github.io/projects/PathFindingVisualizer/PathFindingVisualizer.html)
- Algorithm implementation guide: [English](algorithm/README-EN.md) / [中文](algorithm/README-CN.md)

## Build / 构建

```powershell
cmake -S . -B build -A x64 -DPATHFINDER_USE_CUSTOM_IMPLEMENTATIONS=OFF
cmake --build build --config Debug
```

## Run / 运行

```powershell
.\build\Debug\PathFindingVisualizer.exe
```

For full usage, build options, and implementation notes, open the language-specific README above.

完整使用方式、构建选项和实现说明请查看上方对应语言的 README。
