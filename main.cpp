#include "raylib.hpp"
#include <algorithm>
#include <vector>
#include <string>

#include "Car.h"

bool NextGen(std::vector<Car>& cars, int& gen) {
    // Check if any of the cars in the population are still alive.
    // If any are, return false to indicate that the generation should not be advanced yet.
    for (Car& car : cars) {
        if (car.isAlive)
            return false;
    }

    // Sort the cars by their scores in descending order.
    std::sort(cars.begin(), cars.end(), [](Car a, Car b) { return a.ReturnScore() > b.ReturnScore(); });

    // Iterate over the cars in the population.
    for (int i = 0; i < cars.size(); i++) {
        // For the top 10 cars, do nothing (these cars are "preserved" for the next generation).
        if (i <= 10) {
            // do nothing
        }
        // For the next 80% of the cars, copy the neural network of a randomly selected car from the top 10 and mutate it.
        else if (i <= cars.size() / 10.f * 8.f) {
            cars[i].nn.CopyNNIntoThis(cars[i % (int)(cars.size() / 10)].nn);
            cars[i].nn.Mutate();
            cars[i].respawn();
        }
        // For the remaining 10% of the cars, copy the neural network of a randomly selected car from the top 10 and mutate it 50 times.
        else {
            cars[i].nn.CopyNNIntoThis(cars[i % (int)(cars.size() / 10)].nn);
            for (int _ = 0; _ < 50; _++) {
                cars[i].nn.Mutate();
            }
            cars[i].respawn();
        }
    }

    // Increment the generation count and return true to indicate that the generation has been advanced.
    gen++;
    return true;
}

int main(void)
{
    // Seed the random number generator with the current time.
    srand(time(NULL));

    // Create a vector to hold the "cars" in the simulation.
    std::vector<Car> cars;

    // Create a "map" object and set its default starting position and orientation,
    // as well as its border and checkpoint lines.
    Map map{};
    map.defaultR = -PI / 2.0f; // default orientation
    map.defaultX = 75; // default x position
    map.defaultY = 125; // default y position
    map.borderLines = {
        {50, 50, 750, 50},
        {750, 50, 750, 200},
        {750, 200, 550, 400},
        {550, 400, 50, 400},
        {50, 400, 50, 300},
        {50, 300, 250, 300},
        {250, 300, 250, 200},
        {250, 200, 50, 200},
        {50, 200, 50, 50},

        {100, 350, 300, 350},
        {300, 350, 300, 150},
        {300, 150, 100, 150},
        {100, 150, 100, 100},
        {100, 100, 700, 100},
        {700, 100, 700, 175},
        {700, 175, 525, 350},
        {525, 350, 100, 350},
    };
    map.checkpointLines = {
        {400, 50, 400, 100},
        {700, 100, 750, 50},
        {700, 175, 750, 200},
        {525, 350, 550, 400},
        {100, 350, 50, 350},
        {250, 250, 300, 250},
        {50, 125, 100, 125},
    };

    // Initialize the generation counter and the best score.
    int gen = 0;
    int bestScore = 0;

    // Create 100 "car" objects and add them to the vector.
    for (int i = 0; i < 100; i++) {
        cars.push_back({ 0, 0, true, &bestScore });
        cars[i].GotoMap(&map);
    }

    // Set the screen dimensions and create the window.
    const int screenWidth = 800;
    const int screenHeight = 450;
    InitWindow(screenWidth, screenHeight, "Car game");

    // Set the target frame rate to 60 FPS.
    SetTargetFPS(60);

    // Main loop.
    while (!WindowShouldClose())
    {
        // Update the position and orientation of each car.
        for (Car& car : cars) {
            car.UpdateCar();
        }

        // Advance to the next generation.
        NextGen(cars, gen);

        // Begin drawing to the screen.
        BeginDrawing();

        // Clear the screen to white.
        ClearBackground(RAYWHITE);

        // Draw the border lines of the map.
        for (line l : map.borderLines) {
            l.Draw();
        }

        // Draw the checkpoint lines of the map.
        for (line l : map.checkpointLines) {
            l.Draw(DARKBLUE);
        }

        // Draw the cars.
        for (Car car : cars) {
            car.DrawCar();
        }

        // Convert the generation count to a string and draw it to the screen.
        std::string s = "GEN: " + std::to_string(gen);
        char const* gen_text = s.c_str();
        DrawText(gen_text, 20, 10, 24, LIGHTGRAY);

        // End drawing to the screen.
        EndDrawing();

        // If the "S" key is pressed, skip the specified number of generations.
        if (IsKeyDown(KEY_S)) {
            int generations_left = 10;

            // Modify the number of generations to skip based on whether the shift or control keys are also pressed.
            if (IsKeyDown(KEY_LEFT_SHIFT)) { generations_left *= 10; }
            if (IsKeyDown(KEY_LEFT_CONTROL)) { generations_left *= 10; }

            // Print a message indicating the number of generations that will be skipped.
            std::cout << "Skipping " << generations_left << " generations:" << std::endl << "< ";

            // Skip the specified number of generations.
            while (generations_left != 0) {
                for (Car& car : cars) {
                    car.UpdateCar();
                }

                if (NextGen(cars, gen)) {
                    // Print a message indicating the number of generations left to skip.
                    std::cout << generations_left << (generations_left == 1 ? " >" : " | ");
                    generations_left--;
                }
            }
            std::cout << std::endl;
        }
    }

    // Close the window.
    CloseWindow();

    return 0;
}