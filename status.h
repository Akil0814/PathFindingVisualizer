#pragma once

enum class InputType
{
	Empty,      // Walkable tile
	Wall,       // Blocked tile
	Start,      // Starting point
	Goal,       // Target point
	Weight
};

enum class Algorithm
{
	AStar,
	Dijkstra,
	BFS,
	Greedy,
	Custom
};

enum class PlayMode
{
	Idle,
	AutoRun,
	Pause
};

enum class SimState
{
	Editing,
	Running,
	Finished
};

enum class MoveMode
{
	FourWay,
	EightWay
};

enum class HeuristicMode
{
	Manhattan,
	Euclidean,
	Octile,
	Chebyshev
};

enum class DiagonalMovePolicy
{
	BlockIfEitherSideBlocked,
	BlockIfBothSidesBlocked,
	IgnoreSideBlocks
};

