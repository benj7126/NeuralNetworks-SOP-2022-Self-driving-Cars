#include "raylib.hpp"
#include <vector>
#include <string>

#include "Car.h"

int main(void)
{
    srand(time(NULL));

    /*
    std::cout << "start <";
    NeuralNetwork nn = {{2, 3, 1}};
    // if 1 > 0.2 and 2 > 0.2
    // you need to open the door (return > 0.5)

    float d1 = (float)rand() / RAND_MAX;
    float d2 = (float)rand() / RAND_MAX;

    std::vector<float> result = nn.ForwardsPass({ d1, d2 });

    
    bool passed = false;
    if (result[0] > 0.5) {
        if (d1 > 0.2 && d2 > 0.2) {
            passed = true;
        }
    }

    std::cout << passed << std::endl;
    */

    std::vector<Car> cars;

    Map map{};
    map.defaultR = -PI/2.0f;
    map.defaultX = 75;
    map.defaultY = 125;
    map.borderLines = {
        // outer
        {50, 50, 750, 50},
        {750, 50, 750, 200},
        {750, 200, 550, 400},
        {550, 400, 50, 400},
        {50, 400, 50, 300},
        {50, 300, 250, 300},
        {250, 300, 250, 200},
        {250, 200, 50, 200},
        {50, 200, 50, 50},

        // inner
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

    for (int i = 0; i < 100; i++) {
        cars.push_back({ 0, 0 });
        cars[i].GotoMap(&map);
    }

    int gen = 0;

    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "Car game");

    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        // Car stuff
        for (Car& car : cars) {
            car.UpdateCar();
        }

        //NN Stuff
        bool sampleAlive = false;
        for (Car& car : cars) {
            if (car.isAlive)
                sampleAlive = true;
        }

        // if no cars are alive, do cheap genetic algorithm
        if (!sampleAlive) {
            float bestScore = -1;
            int bestScoreIDX = -1;
            for (int carIDX = 0; carIDX < cars.size(); carIDX++) {
                Car car = cars[carIDX];
                float score = car.ReturnScore();

                if (score > bestScore) {
                    bestScore = score;
                    bestScoreIDX = carIDX;
                }
            }

            if (bestScoreIDX != -1) {
                for (int carIDX = 0; carIDX < cars.size(); carIDX++) {
                    Car &car = cars[carIDX];

                    if (carIDX != bestScoreIDX) {
                        if (carIDX > 10) {
                            car.nn = { { 8, 7, 4, 3 } };
                        } else {
                            car.nn.CopyNNIntoThis(cars[bestScoreIDX].nn);
                            car.nn.Mutate();
                        }

                        car.respawn();
                    }
                }
            }

            gen++;
        }

        // Visual stuff
        BeginDrawing();

        ClearBackground(RAYWHITE);

        for (line l : map.borderLines) {
            l.Draw();
        }

        for (line l : map.checkpointLines) {
            l.Draw(DARKBLUE);
        }

        for (Car car : cars) {
            car.DrawCar();
        }

        /*
        std::string s = std::to_string(gen);
        char const* gen = "GEN: " + s.c_str();

        DrawText(gen, 109, 10, 10, LIGHTGRAY);
        */

        EndDrawing();

        if (IsKeyDown(KEY_S)) {
            for (int i = 0; i < 160; i++) {
                for (Car& car : cars) {
                    car.UpdateCar();
                }
            }
        }
    }
    
    CloseWindow();

    return 0;
}