#pragma once

enum class InPutType
{
	Empty,      // Walkable tile
	Wall,       // Blocked tile
	Start,      // Starting point
	Goal,       // Target point
	Weight
};

enum class Algorithm
{
	AStart,
	Dijkstar,
	BFS,
	Greedy
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