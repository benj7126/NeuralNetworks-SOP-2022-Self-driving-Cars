#pragma once

#include <vector>
#include <stdio.h>

#include "Perceptron.h"
#include "Edge.h"

class Layer {
	Perceptron* perceptrons;
	Edge* edges;

	size_t perceptronsSize;
	size_t edgesSize;
	
public:
	/// <summary> Constructor for the Layer class</summary>
	/// <param name="perceptronCount"> The amount of perceptrons to have in this layer </param>
	/// <returns>  </returns>
	Layer(int perceptronCount) {
		perceptronsSize = perceptronCount;
		perceptrons = new Perceptron[perceptronsSize];

		// Initialize perceptrons
		for (int i = 0; i < perceptronCount; i++) {
			// Create perceptron and add to vector
			perceptrons[i] = {};
		}
	}

	// links this layer and another, assuming this is behind the one we link to
	void LayerLinkForward(Layer* nextLayer) {
		edgesSize = perceptronsSize * nextLayer->perceptronsSize;
		edges = new Edge[edgesSize];

		for (int thisLayerIDX = 0; thisLayerIDX < perceptronsSize; thisLayerIDX++) {
			for (int nextLayerIDX = 0; nextLayerIDX < nextLayer->perceptronsSize; nextLayerIDX++) {
				edges[thisLayerIDX * nextLayer->perceptronsSize + nextLayerIDX] = {};
				Edge* edge = &edges[thisLayerIDX * nextLayer->perceptronsSize + nextLayerIDX];

				Perceptron& thisPerceptron = perceptrons[thisLayerIDX];
				Perceptron& nextPerceptron = nextLayer->perceptrons[nextLayerIDX];

				thisPerceptron.linkOut(edge);
				nextPerceptron.linkIn(edge);
			}
		}
	}

	void SetValues(std::vector<float> newVal) {
		for (int i = 0; i < std::min(newVal.size(), perceptronsSize); i++) {
			perceptrons[i].outValue = newVal[i];
		}
	}

	std::vector<float> GetValues() {
		std::vector<float> retVal;
		
		for (int i = 0; i < perceptronsSize; i++) {
			retVal.push_back(perceptrons[i].outValue);
		}

		return retVal;
	}

	void ForwardsPass() {
		for (int perceptronIDX = 0; perceptronIDX < perceptronsSize; perceptronIDX++) {
			Perceptron& myPerceptron = perceptrons[perceptronIDX];
			myPerceptron.calculateInValue();
		}
	}
};