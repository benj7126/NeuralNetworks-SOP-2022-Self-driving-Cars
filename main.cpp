#include "raylib.hpp"
#include <vector>

#include "Car.h"

int main(void)
{
    /*
    std::cout << "start <";
    srand(time(NULL));
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

    Car ncar{100, 100};


    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "raylib [core] example - basic window");

    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        ncar.UpdateCar();

        BeginDrawing();

        ClearBackground(RAYWHITE);

        ncar.DrawCar();

        DrawText("Congrats! You created your first window!", 190, 200, 20, LIGHTGRAY);

        EndDrawing();
    }
    
    CloseWindow();

    return 0;
}