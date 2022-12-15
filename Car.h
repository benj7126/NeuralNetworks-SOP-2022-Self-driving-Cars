#pragma once

#include <vector>

#include "line.h"
#include "NeuralNetwork.h"

class Car {
private:
	NeuralNetwork nn; // the brains of the car

	float x=0; // car center position x
	float y=0; // car center position y
	float lastX = 0; // last x, sude for collision (might also not use, not sure for now)
	float lastY = 0; // last y, sude for collision (might also not use, not sure for now)

	float w = 10; // width of car
	float h = 20; // height of car

	float r=0; // cur rotation of car
	float curSpeed=0; // cur speed to go in said rotation of car

	bool useAI; // if this is player controlled or ai controlled
	
	std::vector<line> GetCarLines() {
		float xRot = cos(r);
		float yRot = sin(r);
		float xRot90 = cos(r + PI / 2.0f);
		float yRot90 = sin(r + PI / 2.0f);

		// calculate all line of the car
		float topLeftX = x + xRot * h / 2.0f + xRot90 * w / 2.0f;
		float topLeftY = y + yRot * h / 2.0f + yRot90 * w / 2.0f;

		float topRightX = x + xRot * h / 2.0f - xRot90 * w / 2.0f;
		float topRightY = y + yRot * h / 2.0f - yRot90 * w / 2.0f;

		float botLeftX = x - xRot * h / 2.0f + xRot90 * w / 2.0f;
		float botLeftY = y - yRot * h / 2.0f + yRot90 * w / 2.0f;

		float botRightX = x - xRot * h / 2.0f - xRot90 * w / 2.0f;
		float botRightY = y - yRot * h / 2.0f - yRot90 * w / 2.0f;

		return std::vector<line> {
			{ topLeftX, topLeftY, topRightX, topRightY },
			{ topRightX, topRightY, botRightX, botRightY },
			{ botRightX, botRightY, botLeftX, botLeftY },
			{ botLeftX, botLeftY, topLeftX, topLeftY },
		};
	}

	std::vector<float> GetInputs() {

	}

public:
	Car(float x, float y) : nn{ {2, 5, 2} }, x{ x }, y{ y }, lastX{ x }, lastY{ y }, useAI{ useAI } {}

	void UpdateCar() {
		if (useAI)
			AIControlledUpdate();
		else
			PlayerControlledUpdate();
	}

	void AIControlledUpdate() {
		std::vector<float> result = nn.ForwardsPass(GetInputs());
		if (IsKeyDown(KEY_D)) { r += 0.05f; };
		if (IsKeyDown(KEY_A)) { r -= 0.05f; };
		if (IsKeyDown(KEY_W)) { curSpeed += 0.3f; };
		curSpeed *= 0.9;

		x = x + cos(r) * curSpeed;
		y = y + sin(r) * curSpeed;
	}

	void PlayerControlledUpdate() {
		if (IsKeyDown(KEY_D)) { r += 0.05f; };
		if (IsKeyDown(KEY_A)) { r -= 0.05f; };
		if (IsKeyDown(KEY_W)) { curSpeed += 0.3f; };
		curSpeed *= 0.9;

		x = x + cos(r) * curSpeed;
		y = y + sin(r) * curSpeed;
	}

	void DrawCar() {
		std::vector<line> lines = GetCarLines();
		for (line l : lines) {
			l.Draw();
		}
	}
};
