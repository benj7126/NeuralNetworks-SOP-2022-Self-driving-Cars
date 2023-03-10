#pragma once

#include "raylib.hpp"

// a collision struct, that the line will return when it runs collision with another line
// just to make it easier to send "data" over functions
struct collision {
	bool didCollide;
	float collisionX = 0;
	float collisionY = 0;

	collision(bool didCollide, float collisionX=0, float collisionY=0)
		:didCollide{ didCollide }, collisionX{ collisionX }, collisionY{ collisionY } {};
};

// a line struct, used to simplyfy the porcess of multiple things in the program
struct line {
private:
	// the start and end 2d positions of the line
	float startX;
	float startY;

	float endX;
	float endY;

public:
	line(float startX, float startY, float endX, float endY)
		:startX{ startX }, startY{ startY }, endX{ endX }, endY{ endY } {};

	// get the distance from one 2D point to this lines center
	float DistanceFromCenter(float x, float y) {
		float pX = (startX + endX) / 2.0f;
		float pY = (startY + endY) / 2.0f;

		// Pythagoras:
		return sqrt(pow((x - pX), 2) + pow((y - pY), 2));
	}

	// Draw funktion from raylib, to show the line.
	void Draw(Color c = BLACK) {
		DrawLine(startX, startY, endX, endY, c);
	}

	// Collision code was taken from
	// https://www.jeffreythompson.org/collision-detection/line-line.php
	// all credit goes there
	collision checkCollision(line otherLine) {
		float x1 = startX;
		float y1 = startY;

		float x2 = endX;
		float y2 = endY;

		float x3 = otherLine.startX;
		float y3 = otherLine.startY;

		float x4 = otherLine.endX;
		float y4 = otherLine.endY;

		// calculate the distance to intersection point
		float uA = ((x4 - x3) * (y1 - y3) - (y4 - y3) * (x1 - x3)) / ((y4 - y3) * (x2 - x1) - (x4 - x3) * (y2 - y1));
		float uB = ((x2 - x1) * (y1 - y3) - (y2 - y1) * (x1 - x3)) / ((y4 - y3) * (x2 - x1) - (x4 - x3) * (y2 - y1));

		// if uA and uB are between 0-1, lines are colliding
		if (uA >= 0 && uA <= 1 && uB >= 0 && uB <= 1) {

			// optionally, draw a circle where the lines meet
			float intersectionX = x1 + (uA * (x2 - x1));
			float intersectionY = y1 + (uA * (y2 - y1));
			
			return {true, intersectionX, intersectionY};
		}
		return {false};
	}
};