#pragma once

#include <vector>

#include "Layer.h"

class NeuralNetwork {
	std::vector<Layer> layers;


public:
	NeuralNetwork(std::vector<int> structure) {
		for (int layerIDX = 0; layerIDX < structure.size(); layerIDX++) {
			layers.push_back({ structure[layerIDX]});
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
			Layer& curL = layers[i];

			curL.ForwardsPass();
		}

		return layers[layers.size()-1].GetValues();
	}

	// assuming its the same size...
	void CopyNNIntoThis(NeuralNetwork otherNN) {
		layers.clear();
		for (int layerIDX = 0; layerIDX < otherNN.layers.size(); layerIDX++) {
			layers.push_back({ (int)otherNN.layers[layerIDX].perceptronsSize});
		}

		for (int i = 0; i < layers.size() - 1; i++) {
			Layer& curL = layers[i];
			Layer& nextL = layers[i + 1];

			curL.LayerLinkForward(&nextL);
		}

		for (int layerIDX = 0; layerIDX < layers.size(); layerIDX++) {
			Layer& l = layers[layerIDX];

			l.CopyLayerIntoThis(otherNN.layers[layerIDX], layerIDX == layers.size()-1);
		}
	}

	void Mutate() {
		for (int layerIDX = 1; layerIDX < layers.size(); layerIDX++) {
			Layer& curL = layers[layerIDX];

			curL.Mutate(layerIDX == layers.size() - 1);
		}
	}
};