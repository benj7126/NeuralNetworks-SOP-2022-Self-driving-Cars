#pragma once

#include <vector>

#include "Layer.h"

class NeuralNetwork {
	std::vector<Layer> layers;


public:
	NeuralNetwork(std::vector<int> structure) {
		for (int perceptronCount : structure) {
			layers.push_back({ perceptronCount });
		}

		for (int i = 0; i < layers.size()-1; i++) {
			Layer& curL = layers[i];
			Layer& nextL = layers[i+1];

			curL.LayerLinkForward(&nextL);
		}
	}

	std::vector<float> ForwardsPass(std::vector<float> inputLayer) {
		layers[0].SetValues(inputLayer);
		for (int i = 1; i < layers.size(); i++) {
			Layer curL = layers[i];

			curL.ForwardsPass();
		}

		return layers[layers.size()-1].GetValues();
	}
};