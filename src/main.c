#include "button.h"
#include <raylib.h>
#include <stdio.h>

// #define RAYGUI_IMPLEMENTATION
// #include "../raygui/src/raygui.h"

char* screenText = "0";

int main() {

    const int width = 400;
    const int height = 600;

    Button testButton =
        NewButton((Rectangle){10, 80, 120, 60}, "click me", NULL);

    InitWindow(width, height, "logalc");
    SetTargetFPS(30);

    // GuiLoadStyle("assets/style_dark.rgs");

    while (!WindowShouldClose()) {

        // --- Handle Actions ---

        Vector2 mousePos = GetMousePosition();
        HandleButtonCollision(&testButton, mousePos);
        HandleButtonAction(&testButton);

        // --- Handle Drawing ---

        BeginDrawing();
        ClearBackground(DARKGRAY);
        RenderButton(&testButton);
        DrawRectangle(10, 10, 380, 50, LIGHTGRAY);
        DrawText(screenText, 25, 25, 20, BLACK);
        EndDrawing();
    }

    CloseWindow();

    return 0;
}
