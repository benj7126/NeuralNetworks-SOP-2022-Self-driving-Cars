#pragma once

#include "Perceptron.h"

// Writing Perceptron here to help cpp understand some things... it just needs to be there, dont think about it
class Perceptron;

// the Edge class that is between 2 perceptrons
class Edge {
public:
	// the perceptron on the left side and the one on the right side
	Perceptron* perceptronIn;
	Perceptron* perceptronOut;

	// the weight of the connection, how important it is for the netwrok
	float weight;

	// Contruct of edge
	Edge() {
		// random number between -1 and 1 for when the edge is made
		weight = ((float)rand() / RAND_MAX * 2.0f) - 0.5f;
	}
};