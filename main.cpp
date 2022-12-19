#include "raylib.hpp"
#include <algorithm>
#include <vector>
#include <string>

#include "Car.h"

bool NextGen(std::vector<Car>& cars, int& gen) {
    for (Car& car : cars) {
        if (car.isAlive)
            return false;
    }

    std::sort(cars.begin(), cars.end(), [](Car a, Car b) { return a.ReturnScore() > b.ReturnScore(); });
    
    for (int i = 0; i < cars.size(); i++) {
        if (i <= cars.size() / 10) {
            // do nothin this is the preserved
        }
        else if (i <= cars.size() / 10.f*8.f) {
            cars[i].nn.CopyNNIntoThis(cars[i % 10].nn);
            cars[i].nn.Mutate();
            cars[i].respawn();
        }
        else {
            cars[i].nn.CopyNNIntoThis(cars[i % 10].nn);
            for (int _ = 0; _ < 50; _++) {
                cars[i].nn.Mutate();
            }
            cars[i].respawn();
        }
    }

    gen++;
    return true;
}

int main(void)
{
    srand(time(NULL));

    std::vector<Car> cars;

    Map map{};
    map.defaultR = -PI/2.0f;
    map.defaultX = 75;
    map.defaultY = 125;
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

    int gen = 0;
    int bestScore = 0;

    for (int i = 0; i < 100; i++) {
        cars.push_back({ 0, 0, true, &bestScore });
        cars[i].GotoMap(&map);
    }

    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "Car game");

    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        for (Car& car : cars) {
            car.UpdateCar();
        }

        NextGen(cars, gen);

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

        std::string s = "GEN: " + std::to_string(gen);
        char const* gen_text = s.c_str();

        DrawText(gen_text, 20, 10, 24, LIGHTGRAY);
        
        EndDrawing();

        if (IsKeyDown(KEY_S)) {
            int generations_left = 10;

            if (IsKeyDown(KEY_LEFT_SHIFT)) { generations_left *= 10; }
            if (IsKeyDown(KEY_LEFT_CONTROL)) { generations_left *= 10; }

            std::cout << "Skipping " << generations_left << " generations:" << std::endl << "< ";
            while (generations_left != 0) {
                for (Car& car : cars) {
                    car.UpdateCar();
                }

                if (NextGen(cars, gen)) {
                    std::cout << generations_left << (generations_left == 1 ? " >" : " | ");
                    generations_left--;
                }
            }
            std::cout << std::endl;
        }
    }
    
    CloseWindow();

    return 0;
}