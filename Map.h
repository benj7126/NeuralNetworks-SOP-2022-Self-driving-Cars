#pragma once

#include <vector>

#include "line.h"

struct Map{
	float defaultX;
	float defaultY;
	float defaultR;
	std::vector<line> borderLines;
	std::vector<line> checkpointLines;
};