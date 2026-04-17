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

	inline const char* move_mode(MoveMode move_mode)
	{
		switch (move_mode)
		{
		case MoveMode::FourWay: return "Four Way";
		case MoveMode::EightWay: return "Eight Way";

		default: return "Unknown";
		}
	}

	inline const char* play_mode(PlayMode play_mode)
	{
		switch (play_mode)
		{
		case PlayMode::Idle: return "Idle";
		case PlayMode::AutoRun: return "Auto Run";
		case PlayMode::Pause: return "Pause";

		default: return "Unknown";
		}
	}

	inline const char* sim_state(SimState sim_state)
	{
		switch (sim_state)
		{
		case SimState::Editing: return "Editing";
		case SimState::Running: return "Running";
		case SimState::Finished: return "Finished";

		default: return "Unknown";
		}
	}

	inline const char* a_star_heuristic(HeuristicMode heuristic_mode)
	{
		switch (heuristic_mode)
		{
		case HeuristicMode::Manhattan: return "Manhattan";
		case HeuristicMode::Euclidean: return "Euclidean";
		case HeuristicMode::Octile: return "Octile";
		case HeuristicMode::Chebyshev: return "Chebyshev";

		default: return "Unknown";
		}
	}

	inline const char* a_star_heuristic_formula(HeuristicMode heuristic_mode)
	{
		switch (heuristic_mode)
		{
		case HeuristicMode::Manhattan: return "h = abs(dx) + abs(dy)";
		case HeuristicMode::Euclidean: return "h = sqrt(dx*dx + dy*dy)";
		case HeuristicMode::Octile: return "h = max(dx, dy) + (sqrt(2)-1)*min(dx, dy)";
		case HeuristicMode::Chebyshev: return "h = max(abs(dx), abs(dy))";

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
		case Tile::Status::Current: return "Current";
		case Tile::Status::Open: return "Open";
		case Tile::Status::Closed: return "Closed";
		case Tile::Status::Path: return "Path";

		default: return "Unknown";
		}
	}
}
