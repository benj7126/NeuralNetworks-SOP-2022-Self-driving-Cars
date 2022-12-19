#pragma once

#include <vector>

#include "Map.h"
#include "line.h"
#include "NeuralNetwork.h"

class Car {
private:
	float x=0; // car center position x
	float y=0; // car center position y

	Map* m = nullptr;
	int checkpoint = 0; // cur checkpoint
	int points = 0; // cur checkpoint
	int checkpointTimer = 160; // updates pr checkpoint untill suicide...

	int* bestScore;

	float lastDist = 0;

	float w = 10; // width of car
	float h = 20; // height of car

	float rayLength = 1000.0f; // the max length of the ray

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
		std::vector<line> rays{
			{x, y, x + cos(r) * rayLength, y + sin(r) * rayLength},
			{x, y, x + cos(r + PI / 4.0f) * rayLength, y + sin(r + PI / 4.0f) * rayLength},
			{x, y, x + cos(r - PI / 4.0f) * rayLength, y + sin(r - PI / 4.0f) * rayLength},
			{x, y, x + cos(r + PI / 2.0f) * rayLength, y + sin(r + PI / 2.0f) * rayLength},
			{x, y, x + cos(r - PI / 2.0f) * rayLength, y + sin(r - PI / 2.0f) * rayLength},
		};

		std::vector<float> raysLen{ rayLength , rayLength , rayLength , rayLength , rayLength };

		int curRay = 0;
		for (line ray : rays) {
			for (line mapBorder : m->borderLines) {
				collision c = ray.checkCollision(mapBorder);

				c.collisionX -= x; // make them point from 0, 0
				c.collisionY -= y; // make them point from 0, 0

				float dist = sqrt(pow(c.collisionX, 2)+pow(c.collisionY, 2));
				if (raysLen[curRay] > dist)
					raysLen[curRay] = dist;
			}
			
			curRay++;
		}

		float distToCheckpoint = m->checkpointLines[checkpoint].DistanceFromCenter(x, y);

		std::vector<float> res { raysLen[0], raysLen[1], raysLen[2], raysLen[3], raysLen[4], curSpeed, r, distToCheckpoint - lastDist };

		lastDist = distToCheckpoint;

		return res;
	}

public:
	NeuralNetwork nn; // the brains of the car
	bool isAlive = true; // only used for ai

	// inputs:
	// 1-5: ray
	// 6: speed
	// 7: rotation
	// 8: dist to checkpoint

	//outputs: (all between 0 and 1)
	// 1: how much speed to add
	// 2: how much to rotate left
	// 3: how much to rotate right

	Car(float x, float y, bool useAI = true, int* bestScore = nullptr) : nn{ {8, 7, 4, 3} }, x{ x }, y{ y }, useAI{ useAI }, bestScore{ bestScore } {}

	void UpdateCar() {
		if (useAI)
			AIControlledUpdate();
		else
			PlayerControlledUpdate();

		// collision
		bool didCollide = false;
		for (line carBody : GetCarLines()) {
			for (line mapBorder : m->borderLines) {
				if (carBody.checkCollision(mapBorder).didCollide)
					didCollide = true;
			}
		}

		if (useAI && !didCollide) { // checkpoint mecanic
			bool hitCheckpoint = false;
			for (line carBody : GetCarLines()) {
				if (carBody.checkCollision(m->checkpointLines[checkpoint]).didCollide)
					hitCheckpoint = true;
			}

			if (hitCheckpoint) {
				checkpointTimer = 160;

				points++;
				if (bestScore != nullptr)
					if (points > *bestScore) {
						std::cout << std::endl << "New best score: " << points << std::endl;

						int laps = (int)(points - (points % m->checkpointLines.size())) / m->checkpointLines.size();

						std::cout << "For a total of: " << laps << std::endl << std::endl;

						*bestScore = points;
					}

				checkpoint++;
				if (checkpoint == m->checkpointLines.size())
					checkpoint = 0;
			}
		}

		checkpointTimer--;
		if (didCollide || checkpointTimer == 0) {
			if (useAI)
				isAlive = false;
			else
				respawn();
		}
	}

	float ReturnScore() {
		float distToCheckpoint = m->checkpointLines[checkpoint].DistanceFromCenter(x, y);
		return points * 1000.f + 1/distToCheckpoint;
	}

	void AIControlledUpdate() {
		if (!isAlive)
			return;

		std::vector<float> result = nn.ForwardsPass(GetInputs());

		curSpeed += result[0]/2.0f; // 0.5 - 0

		if (result[1] > 0.1f) // only rotate if over 0.1
			r += result[1]; // 0.1 - 0

		if (result[2] > 0.1f) // only rotate if over 0.1
			r -= result[2]; // 0.1 - 0

		curSpeed *= 0.9;

		x += cos(r) * curSpeed;
		y += sin(r) * curSpeed;
	}

	void GotoMap(Map* map) {
		m = map;
		respawn();
	}

	void respawn() {
		x = m->defaultX;
		y = m->defaultY;
		r = m->defaultR;

		isAlive = true;

		checkpointTimer = 160;
		checkpoint = 0;
		points = 0;
	}

	void PlayerControlledUpdate() { // for driving around with player...
		if (IsKeyDown(KEY_D)) { r += 0.05f; };
		if (IsKeyDown(KEY_A)) { r -= 0.05f; };
		if (IsKeyDown(KEY_W)) { curSpeed += 0.3f; };
		if (IsKeyDown(KEY_S)) { curSpeed -= 0.3f; };
		curSpeed *= 0.9;

		x += cos(r) * curSpeed;
		y += sin(r) * curSpeed;
	}

	void DrawCar() {
		if (!isAlive)
			return;

		// get lines that represent the car, and draw them
		std::vector<line> lines = GetCarLines();
		for (line l : lines) {
			l.Draw();
		}
	}
};
