# Algorithm Guide / 算法指南

[English README](README-EN.md) / [中文 README](README-CN.md)

This folder contains the shared pathfinder interface and algorithm implementations. / 这个目录包含统一的寻路接口和具体算法实现。

Use this guide when adding or finishing a path-finding algorithm. / 添加或补全寻路算法时请参考这里的说明。

## Quick Links / 快速链接

- Implementation guide: [English](README-EN.md) / [中文](README-CN.md)
- Base interface: `path_finder.h` / 基础接口：`path_finder.h`
- Reference implementation: `bfs_pathfinder.*` / 参考实现：`bfs_pathfinder.*`

## Key Rule / 核心规则

`next_step()` should advance one visual step, not run the whole search at once. / `next_step()` 应该只推进一个可视化步骤，而不是一次跑完整个搜索。

Keep tile status, parent links, costs, frontier/open set, visited/closed state, and `clone()` in sync. / 需要同步格子状态、父节点、代价、frontier/open set、visited/closed 状态和 `clone()`。
