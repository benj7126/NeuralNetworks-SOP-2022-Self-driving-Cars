#pragma once

#include<stdio.h>
#include <iostream>
#include <vector>

#include "Edge.h";

class Edge;
class Perceptron {
	std::vector<Edge*> edgeIn;
	std::vector<Edge*> edgeOut;

	void Sigmoid() {
		// 1/(1+e^(-x))
		outValue = 1 / (1 + exp((double)-inValue));
	}

public:
	float inValue; // value of sum of previous layer
	float outValue; // value of inValue, after aktivation function has been run
	float bias = 0;

	Perceptron() {}

	void linkIn(Edge* e) {
		edgeIn.push_back(e);
		e->perceptronOut = this;
	}

	void linkOut(Edge* e) {
		edgeOut.push_back(e);
		e->perceptronIn = this;
	}

	void calculateInValue() {
		inValue = bias;
		
		for (Edge* e : edgeIn) {
			Perceptron* otherPerceptron = e->perceptronIn;

			inValue += otherPerceptron->outValue * e->weight;
		}

		Sigmoid();
	}
};