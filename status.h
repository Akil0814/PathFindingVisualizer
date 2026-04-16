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
	BFS
};