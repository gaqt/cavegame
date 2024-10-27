#include "cavegame.h"
#include "../lib/raylib/raylib.h"
#include "../lib/raylib/rcamera.h"
#include "player.h"
#include "world.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

float Vec3Dist(const Vector3 a, const Vector3 b) {
    return sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2) + pow(a.z - b.z, 2));
}

int main() {
    srand(time(NULL));

    InitWindow(SCREEN_W, SCREEN_H, "logemi's cavegame");
    SetTargetFPS(60);

    Image spongeImage = LoadImage("assets/sponge.png");
    Texture2D spongeTexture = LoadTextureFromImage(spongeImage);

    Mesh cube = GenMeshCube(1.0f, 1.0f, 1.0f);
    Model spongeModel = LoadModelFromMesh(cube);
    spongeModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = spongeTexture;

    InitWorld();
    Player player = InitPlayer(5, WORLD_Y - 2, 5);

    DisableCursor();
    SetTargetFPS(60);

    while (!WindowShouldClose()) {

        // ---- Update ----

        PlayerUpdateStandingState(&player);
        PlayerDoWalk(&player);
        PlayerDoMouseLook(&player);
        PlayerUpdatePos(&player);
        PlayerUpdateTargetBlock(&player);
        PlayerHandleInteractWithTargetBlock(&player);

        // ---- Draw ----

        BeginDrawing();

        ClearBackground(SKYBLUE);

        BeginMode3D(player.camera);

        RenderWorld(player.targetBlock, &player.camera, &spongeModel);

        EndMode3D();

        // Crosshair
        DrawRectangleRec((Rectangle){(float)SCREEN_W / 2 - 7,
                                     (float)SCREEN_H / 2 - 2, 14, 4},
                         WHITE);
        DrawRectangleRec((Rectangle){(float)SCREEN_W / 2 - 2,
                                     (float)SCREEN_H / 2 - 7, 4, 14},
                         WHITE);

#ifdef DEBUG
        DrawFPS(10, 10);

        Vector3 camRot = CameraRotUnit(&player);

        // Draw info boxes
        DrawRectangle(600, 5, 195, 120, Fade(SKYBLUE, 0.5f));
        DrawRectangleLines(600, 5, 195, 120, BLUE);

        DrawText("Camera status:", 610, 15, 10, BLACK);
        DrawText(TextFormat("- Position: (%06.3f, %06.3f, %06.3f)",
                            player.camera.position.x, player.camera.position.y,
                            player.camera.position.z),
                 610, 60, 10, BLACK);
        DrawText(TextFormat("- Target: (%06.3f, %06.3f, %06.3f)",
                            player.camera.target.x, player.camera.target.y,
                            player.camera.target.z),
                 610, 75, 10, BLACK);
        DrawText(TextFormat("- Up: (%06.3f, %06.3f, %06.3f)",
                            player.camera.up.x, player.camera.up.y,
                            player.camera.up.z),
                 610, 90, 10, BLACK);
        DrawText(TextFormat("- Rot: (%06.3f, %06.3f, %06.3f)", camRot.x,
                            camRot.y, camRot.z),
                 610, 105, 10, BLACK);

#endif

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    UnloadTexture(spongeTexture);

    CloseWindow();

    return 0;
}
