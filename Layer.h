#pragma once

#include <vector>
#include <stdio.h>

#include "Perceptron.h"
#include "Edge.h"

// The layer class that is filled with perceptrons and is supposed to be within a NN
class Layer {
public:
	// The array of perceptrons made with a pointer, because vectors didnt wanna work out...
	// The same with the edges, this list of edges that is to the left of this layer, if that makes sense...
	Perceptron* perceptrons;
	Edge* edges;

	// the size of the 2 pointer/array/lists (needed because of how they are made)
	size_t perceptronsSize;
	size_t edgesSize;

	// Constructor for the layer, taking the amount of perceptrons it needs to contain
	Layer(int perceptronCount) {
		perceptronsSize = perceptronCount;
		perceptrons = new Perceptron[perceptronsSize];

		// Initialize perceptrons
		for (int i = 0; i < perceptronCount; i++) {
			// Create perceptron and add to vector
			perceptrons[i] = {};
		}
	}

	// Links this layer and another, assuming this is behind the one we link to
	void LayerLinkForward(Layer* nextLayer) {
		// Make a list of edges that is the size needed to fit all neccecary edges
		// Based on the perceptrons in this and the next layer
		edgesSize = perceptronsSize * nextLayer->perceptronsSize;
		edges = new Edge[edgesSize];

		// loop over both layers of perceptrons
		for (int thisLayerIDX = 0; thisLayerIDX < perceptronsSize; thisLayerIDX++) {
			for (int nextLayerIDX = 0; nextLayerIDX < nextLayer->perceptronsSize; nextLayerIDX++) {
				// Add an edges with the right index
				edges[thisLayerIDX * nextLayer->perceptronsSize + nextLayerIDX] = {};
				// make it into a var for convenience
				Edge* edge = &edges[thisLayerIDX * nextLayer->perceptronsSize + nextLayerIDX];

				// variable for convenience
				Perceptron& thisPerceptron = perceptrons[thisLayerIDX];
				Perceptron& nextPerceptron = nextLayer->perceptrons[nextLayerIDX];

				// link them (pointers n such)
				thisPerceptron.linkOut(edge);
				nextPerceptron.linkIn(edge);
			}
		}
	}

	// set the values of perceptrons in this layer (mainly used for input layer)
	void SetValues(std::vector<float> newVal) {
		// loop input or the max size of perceptrons in layer, because we cant set nr 10 when we only have 5...
		for (int i = 0; i < std::min(newVal.size(), perceptronsSize); i++) {
			perceptrons[i].outValue = newVal[i];
		}
	}

	// get the values of perceptrons in this layer (mainly used for output layer)
	std::vector<float> GetValues() {
		std::vector<float> retVal;

		// loop perceptrons and add them to the end of vector
		for (int i = 0; i < perceptronsSize; i++) {
			retVal.push_back(perceptrons[i].outValue);
		}

		return retVal;
	}

	// Do the forward pass... Just tells the perceptrons to do the forward pass...
	void ForwardsPass() {
		for (int perceptronIDX = 0; perceptronIDX < perceptronsSize; perceptronIDX++) {
			Perceptron& myPerceptron = perceptrons[perceptronIDX];
			myPerceptron.calculateInValue();
		}
	}

	// Copies a layers bias and weights into this layer
	void CopyLayerIntoThis(Layer otherLayer, bool isLastLayer) {
		// loops biases
		for (int perceptronIDX = 0; perceptronIDX < perceptronsSize; perceptronIDX++) {
			// sets the balues of the biases
			Perceptron& myPerceptron = perceptrons[perceptronIDX];
			myPerceptron.bias = otherLayer.perceptrons[perceptronIDX].bias;
		}

		// only loop edges if this is not the first layer, since the first layer contains no edges...
		if (!isLastLayer) {
			// loops edges
			for (int edgesIDX = 0; edgesIDX < edgesSize; edgesIDX++) {
				// sets the edges of the biases
				Edge& myEdge = edges[edgesIDX];
				myEdge.weight = otherLayer.edges[edgesIDX].weight;
			}
		}
	}

	// mutate the perceptron, adding a little to bias and weight
	void Mutate(bool isLastLayer) {
		// loop perceptrons
		for (int perceptronIDX = 0; perceptronIDX < perceptronsSize; perceptronIDX++) {
			Perceptron& myPerceptron = perceptrons[perceptronIDX];
			// add random amount to bias
			myPerceptron.bias += (((float)rand() / RAND_MAX) - 0.5f)/100.0f;
		}

		// same as above ^^^^^^
		if (!isLastLayer) {
			// loop edges
			for (int edgesIDX = 0; edgesIDX < edgesSize; edgesIDX++) {
				Edge& myEdge = edges[edgesIDX];
				// add random amount to edges
				myEdge.weight += (((float)rand() / RAND_MAX) - 0.5f)/5.0f;
			}
		}
	}
};