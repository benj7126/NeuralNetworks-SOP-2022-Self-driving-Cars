#pragma once

#include <vector>
#include "line.h"

// Create Map struct containging a x positon, y position and rotaion
// And 2 vectors with the borders of the map and the checkpoints of the map
struct Map{
	float defaultX;
	float defaultY;
	float defaultR;
	std::vector<line> borderLines;
	std::vector<line> checkpointLines;
};