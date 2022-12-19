#pragma once

#include<stdio.h>
#include <iostream>
#include <vector>

#include "Edge.h";

// Writing Edge here to help cpp understand some things... it just needs to be there, dont think about it
class Edge;

// The perceptron class
class Perceptron {
	// vector/lists over Edge pointers for both sides of the perceptron
	std::vector<Edge*> edgeIn;
	std::vector<Edge*> edgeOut;

	// The sigmoid function
	void Sigmoid() {
		// 1/(1+e^(-x))
		outValue = 1 / (1 + exp((double)-inValue));
	}

public:
	float inValue; // value of sum of previous layer
	float outValue; // value of inValue, after aktivation function has been run
	float bias = 0; // value representing the importance of this perceptron

	// link an edge pointer with this perceptron on this perceptrons left side
	void linkIn(Edge* e) {
		edgeIn.push_back(e);
		e->perceptronOut = this;
	}

	// same as aboce, but on the right side
	void linkOut(Edge* e) {
		edgeOut.push_back(e);
		e->perceptronIn = this;
	}

	// Calculate the values of the perceptron using bias, activation function, and the sum of previous layer x the edges weights
	void calculateInValue() {
		inValue = bias;
		
		for (Edge* e : edgeIn) {
			Perceptron* otherPerceptron = e->perceptronIn;

			inValue += otherPerceptron->outValue * e->weight;
		}

		Sigmoid();
	}
};