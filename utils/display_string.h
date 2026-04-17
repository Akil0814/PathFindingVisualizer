#pragma once

#include <SDL.h>

#include "../Aframework/tile.h"
#include "../status.h"

namespace DisplayString
{
	inline const char* input_type(InPutType input)
	{
		switch (input)
		{
		case InPutType::Empty: return "Empty";
		case InPutType::Wall: return "Wall";
		case InPutType::Start: return "Start";
		case InPutType::Goal: return "Goal";
		case InPutType::Weight: return "Weight";

		default: return "Unknown";
		}
	}

	inline const char* edit_mode(InPutType input)
	{
		switch (input)
		{
		case InPutType::Empty: return "Erase";
		case InPutType::Wall: return "Wall";
		case InPutType::Start: return "Start";
		case InPutType::Goal: return "Goal";
		case InPutType::Weight: return "Weight";

		default: return "Unknown";
		}
	}

	inline const char* algorithm(Algorithm algorithm)
	{
		switch (algorithm)
		{
		case Algorithm::AStart: return "A*";
		case Algorithm::Dijkstar: return "Dijkstra";
		case Algorithm::BFS: return "BFS";
		case Algorithm::Greedy: return "Greedy";

		default: return "Unknown";
		}
	}

	inline const char* tile_status(Tile::Status status)
	{
		switch (status)
		{
		case Tile::Status::Empty: return "Empty";
		case Tile::Status::Wall: return "Wall";
		case Tile::Status::Start: return "Start";
		case Tile::Status::Goal: return "Goal";
		case Tile::Status::Open: return "Open";
		case Tile::Status::Closed: return "Closed";
		case Tile::Status::Path: return "Path";

		default: return "Unknown";
		}
	}
}
