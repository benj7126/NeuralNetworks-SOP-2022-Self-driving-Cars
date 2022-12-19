#pragma once

#include <vector>

#include "Map.h"
#include "line.h"
#include "NeuralNetwork.h"

// The Car that is driven by the neural netwrok
class Car {
private:
	float x=0; // car center position x
	float y=0; // car center position y

	Map* m = nullptr;
	int checkpoint = 0; // cur checkpoint
	int points = 0; // cur checkpoint
	int checkpointTimer = 160; // updates pr checkpoint untill suicide...

	int* bestScore; // a pointer to the best score of the car

	float lastDist = 0; // a float with the lastDist of the car, used to tell the ai if it is going towards or away from the goal

	float w = 10; // width of car
	float h = 20; // height of car

	float rayLength = 1000.0f; // the max length of the ray

	float r=0; // cur rotation of car
	float curSpeed=0; // cur speed to go in said rotation of car

	bool useAI; // if this is player controlled or ai controlled
	
	// The lines of the car, these are used both for drawing and colission
	std::vector<line> GetCarLines() {
		// makes rotation to vectors
		// one that points the same way as the car, and one that points 90 of that direction, so right of the car
		float xRot = cos(r);
		float yRot = sin(r);
		float xRot90 = cos(r + PI / 2.0f);
		float yRot90 = sin(r + PI / 2.0f);

		// calculate all 4 corners of the car
		float topLeftX = x + xRot * h / 2.0f + xRot90 * w / 2.0f;
		float topLeftY = y + yRot * h / 2.0f + yRot90 * w / 2.0f;

		float topRightX = x + xRot * h / 2.0f - xRot90 * w / 2.0f;
		float topRightY = y + yRot * h / 2.0f - yRot90 * w / 2.0f;

		float botLeftX = x - xRot * h / 2.0f + xRot90 * w / 2.0f;
		float botLeftY = y - yRot * h / 2.0f + yRot90 * w / 2.0f;

		float botRightX = x - xRot * h / 2.0f - xRot90 * w / 2.0f;
		float botRightY = y - yRot * h / 2.0f - yRot90 * w / 2.0f;

		// return the corners in form of lines
		return std::vector<line> {
			{ topLeftX, topLeftY, topRightX, topRightY },
			{ topRightX, topRightY, botRightX, botRightY },
			{ botRightX, botRightY, botLeftX, botLeftY },
			{ botLeftX, botLeftY, topLeftX, topLeftY },
		};
	}

	// Calculates theinput to give to the Neural Network
	std::vector<float> GetInputs() {
		// creates a lot of lines that are to be used as the eyes of the car
		std::vector<line> rays{
			{x, y, x + cos(r) * rayLength, y + sin(r) * rayLength},
			{x, y, x + cos(r + PI / 4.0f) * rayLength, y + sin(r + PI / 4.0f) * rayLength},
			{x, y, x + cos(r - PI / 4.0f) * rayLength, y + sin(r - PI / 4.0f) * rayLength},
			{x, y, x + cos(r + PI / 2.0f) * rayLength, y + sin(r + PI / 2.0f) * rayLength},
			{x, y, x + cos(r - PI / 2.0f) * rayLength, y + sin(r - PI / 2.0f) * rayLength},
		};

		// a lits of lengths that the car see, starting at max length (so very far away)
		std::vector<float> raysLen{ rayLength , rayLength , rayLength , rayLength , rayLength };

		// int to keep track of loop because i was too lazy to write it the right way
		int curRay = 0;
		for (line ray : rays) {
			// loop over all lines in the border of the current map
			for (line mapBorder : m->borderLines) {
				// calculate collision of border line and this ray
				collision c = ray.checkCollision(mapBorder);

				// if no collision, skip what is below
				if (!c.didCollide)
					continue;

				c.collisionX -= x; // make them point from 0, 0
				c.collisionY -= y; // make them point from 0, 0

				// use pythagoras to get length of lines
				float dist = sqrt(pow(c.collisionX, 2)+pow(c.collisionY, 2));

				// update distance of current ray, if it was the shortest distance
				if (raysLen[curRay] > dist)
					raysLen[curRay] = dist;
			}
			
			// add 1 to cur ray, so that we loop em all (lazy n such)
			curRay++;
		}

		// Get distance to next checkpoint
		float distToCheckpoint = m->checkpointLines[checkpoint].DistanceFromCenter(x, y);

		// make the list that is to be returned to the neural netowrk containing:
		// 0-4: the raycasts
		// 5: the current speed of the car
		// 6: the currnet rotatiton of the car
		// 7: the difference in position from the car to the goal center, based on last frame
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
	
	// constructor of car making a nn with 4 layers and in- / outputs as described above
	Car(float x, float y, bool useAI = true, int* bestScore = nullptr) : nn{ {8, 7, 4, 3} }, x{ x }, y{ y }, useAI{ useAI }, bestScore{ bestScore } {}

	// the function to update the car
	void UpdateCar() {
		// either use player or ai controls (defaults to ai in the constructor)
		if (useAI)
			AIControlledUpdate();
		else
			PlayerControlledUpdate();

		// do collision
		bool didCollide = false;
		// for each line in car loop each line in map, and check if therewas a collision
		for (line carBody : GetCarLines()) {
			for (line mapBorder : m->borderLines) {
				if (carBody.checkCollision(mapBorder).didCollide)
					didCollide = true;
			}
		}

		// if there was no collision and the ai is in control do checkpoint things
		if (useAI && !didCollide) { // checkpoint mecanic
			
			// loop each line in car, and check if any of the collide with the next ckeckpoint
			bool hitCheckpoint = false;
			for (line carBody : GetCarLines()) {
				if (carBody.checkCollision(m->checkpointLines[checkpoint]).didCollide)
					hitCheckpoint = true;
			}

			// if they collide
			if (hitCheckpoint) {
				// reset the checkpoint timer (time untill auto death)
				checkpointTimer = 160;

				// add a point for point calcualtions
				points++;
				// if the bestScore pointer has been set, check if the new point amount is a new record
				if (bestScore != nullptr)
					if (points > *bestScore) {
						// write it to console
						std::cout << std::endl << "New best score of: " << points << std::endl;

						int laps = (int)(points - (points % m->checkpointLines.size())) / m->checkpointLines.size();

						std::cout << "For a total of " << laps << " laps." << std::endl << std::endl;

						// update the value of the pointer
						*bestScore = points;
					}

				// update checkpoint seperaetly from points, so that it can be resat for when a loop occurs
				checkpoint++;
				if (checkpoint == m->checkpointLines.size())
					checkpoint = 0;

				// also update the last distance to the new checkpoint
				lastDist = m->checkpointLines[checkpoint].DistanceFromCenter(x, y);
			}
		}

		// -1 to the checkpoint timer, and kill the car, if it is using ai, let it "die", but if it is controlled by player, respawn it
		checkpointTimer--;
		if (didCollide || checkpointTimer == 0) {
			if (useAI)
				isAlive = false;
			else
				respawn();
		}
	}

	// the score used to sort the cars
	float ReturnScore() {
		// distance from checkpoint so that it is "better" the closer it is
		float distToCheckpoint = m->checkpointLines[checkpoint].DistanceFromCenter(x, y);

		// points*1000 because points are worth more than distance to next checkpoint (also limit distance to checkpoint to 999)
		return points * 1000.f + std::min(999.0f, 1 / distToCheckpoint);
	}

	// update the car with nn
	void AIControlledUpdate() {
		if (!isAlive)
			return;

		// get inputs of the pass and do the pass, then get results, and handle them
		std::vector<float> result = nn.ForwardsPass(GetInputs());

		// Add speed to the car based on what the NN decides
		curSpeed += result[0]/2.0f; // 0.5 - 0

		// rotate the car based on what the NN decides (the first is right the second is left)
		if (result[1] > 0.1f) // only rotate if over 0.1
			r += result[1]; // 0.1 - 0
		if (result[2] > 0.1f) // only rotate if over 0.1
			r -= result[2]; // 0.1 - 0

		// multiply speed with 0.9 so as to simulate accelaratin and such
		curSpeed *= 0.9;

		// move it towards its r by its speed
		x += cos(r) * curSpeed;
		y += sin(r) * curSpeed;
	}

	// add a map to the car
	void GotoMap(Map* map) {
		m = map;
		respawn();
	}

	// respawn the car, setting it to map positions and resetting ai relevant feilds
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
		// if keys are pressed, move like that... (works a little differently than ai, but... it makes sense)
		if (IsKeyDown(KEY_D)) { r += 0.05f; };
		if (IsKeyDown(KEY_A)) { r -= 0.05f; };
		if (IsKeyDown(KEY_W)) { curSpeed += 0.3f; };
		if (IsKeyDown(KEY_S)) { curSpeed -= 0.3f; };
		curSpeed *= 0.9;

		x += cos(r) * curSpeed;
		y += sin(r) * curSpeed;
	}

	// draw the car, but only if it is alive
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
