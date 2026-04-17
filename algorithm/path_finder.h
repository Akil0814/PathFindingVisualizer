#pragma once
#include "../status.h"

class Pathfinder
{
public:
	Pathfinder();
	~Pathfinder();

	void set_algorithm(Algorithm type);
	Algorithm get_current_algorithm();

private:
	Algorithm _current_algorithm = Algorithm::AStart;
};