#pragma once

#include "Perceptron.h"

class Perceptron;
class Edge {
public:
	Perceptron* perceptronIn;
	Perceptron* perceptronOut;

	float weight;

	Edge() {
		// random number between -1 and 1
		weight = ((float)rand() / RAND_MAX * 2.0f) - 0.5f;
	}
};