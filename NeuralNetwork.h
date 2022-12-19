#pragma once

#include <vector>

#include "Layer.h"

// THE Neural network
class NeuralNetwork {
	// All the layers within the neural netwrok
	std::vector<Layer> layers;


public:
	// Constructor taking in a vector that represent the amount of layers and the amount of perceptron in each layer
	NeuralNetwork(std::vector<int> structure) {
		// Create all layers and give amount of perceptrons
		for (int layerIDX = 0; layerIDX < structure.size(); layerIDX++) {
			layers.push_back({ structure[layerIDX]});
		}

		// Link all the layers in the correct way
		for (int i = 0; i < layers.size()-1; i++) {
			Layer& curL = layers[i];
			Layer& nextL = layers[i+1];

			curL.LayerLinkForward(&nextL);
		}
	}

	// The forward pass, the thing that makes the ai know what to do
	std::vector<float> ForwardsPass(std::vector<float> inputLayer) {
		// Set the first layers values to the input, since that is all that it is here for
		layers[0].SetValues(inputLayer);
		for (int i = 1; i < layers.size(); i++) {
			Layer& curL = layers[i];

			// run forwards pass on each layer, excluding the first (the input layer)
			curL.ForwardsPass();
		}

		// return the values of the output layer
		return layers[layers.size()-1].GetValues();
	}

	// Copy one nn into this nn
	void CopyNNIntoThis(NeuralNetwork otherNN) {
		// clear all the layers in the current nn
		layers.clear();
		// loop all layers of ther nn that is to be copied into this and add layers with the right amount of perceptrons
		for (int layerIDX = 0; layerIDX < otherNN.layers.size(); layerIDX++) {
			layers.push_back({ (int)otherNN.layers[layerIDX].perceptronsSize});
		}

		// Link the layers corectly
		for (int i = 0; i < layers.size() - 1; i++) {
			Layer& curL = layers[i];
			Layer& nextL = layers[i + 1];

			curL.LayerLinkForward(&nextL);
		}

		// copy the weights and biases of the layers in the other nn into this nn
		for (int layerIDX = 0; layerIDX < layers.size(); layerIDX++) {
			Layer& l = layers[layerIDX];

			l.CopyLayerIntoThis(otherNN.layers[layerIDX], layerIDX == layers.size()-1);
		}
	}

	// This is the mastermind
	void Mutate() {
		// Run the mutate function on every layer
		for (int layerIDX = 1; layerIDX < layers.size(); layerIDX++) {
			Layer& curL = layers[layerIDX];

			curL.Mutate(layerIDX == layers.size() - 1);
		}
	}
};