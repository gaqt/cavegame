#include "../lib/raylib/raylib.h"
#include "../lib/raylib/rcamera.h"
#include <math.h>
#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// #define RAYGUI_IMPLEMENTATION
// #include "../raygui/src/raygui.h"

#define WORLD_X 32
#define WORLD_Y 24
#define WORLD_Z 32
#define GRAVITY 0.01f
#define MAX_SPEED 3.0f
#define WALK_SPEED 0.09f
#define CAMERA_SENS 0.002f

typedef enum {
    AIR = 0,
    ROCK,
    ROCK_2,
    ROCK_3,
    GRASS,
    GRASS_2,
    GRASS_3,
    SPONGE,
} BlockMaterial;

__attribute__((const)) Color BlockMaterialColor(const BlockMaterial m) {
    switch (m) {
    case AIR:
        return (Color){0, 0, 0, 0};
    case ROCK:
        return (Color){130, 130, 130, 255};
    case ROCK_2:
        return (Color){120, 120, 120, 255};
    case ROCK_3:
        return (Color){110, 110, 110, 255};
    case GRASS:
        return (Color){0, 228, 48, 255};
    case GRASS_2:
        return (Color){0, 217, 44, 255};
    case GRASS_3:
        return (Color){0, 210, 40, 255};
    case SPONGE:
        return YELLOW;
    }

    // should never happen
    exit(-1);
}

__attribute__((const)) float Vec3Dist(const Vector3 a, const Vector3 b) {
    return sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2) + pow(a.z - b.z, 2));
}

__attribute__((const)) Vector3 CameraRotUnit(const Vector3 pos,
                                             const Vector3 target) {
    Vector3 rot =
        (Vector3){target.x - pos.x, target.y - pos.y, target.z - pos.z};

    return rot;
}

int main() {
    srand(time(NULL));

    const int width = 1000;
    const int height = 800;

    InitWindow(width, height, "logemi's cavegame");
    SetTargetFPS(60);

    Image spongeImage = LoadImage("assets/sponge.png");
    Texture2D spongeTexture = LoadTextureFromImage(spongeImage);

    Mesh cube = GenMeshCube(1.0f, 1.0f, 1.0f);
    Model spongeModel = LoadModelFromMesh(cube);
    spongeModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = spongeTexture;

    BlockMaterial world[WORLD_X + 2][WORLD_Y + 2][WORLD_Z + 2] = {AIR};
    for (int x = 1; x <= WORLD_X; x++) {
        for (int y = 1; y <= WORLD_Y - 6; y++) {
            for (int z = 1; z <= WORLD_Z; z++) {
                world[x][y][z] = ROCK + rand() % 3;
            }
        }
        for (int z = 1; z <= WORLD_Z; z++) {
            world[x][WORLD_Y - 5][z] = GRASS + rand() % 3;
        }
    }

    Camera camera = {0};
    camera.position = (Vector3){5.0f, WORLD_Y - 2, 5.0f};
    // camera target must be 1 unit away from position
    camera.target = (Vector3){5.0f, WORLD_Y - 2, 6.0f};
    camera.up = (Vector3){0.0f, 1.0f, 0.0f};
    camera.fovy = 80.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    Vector3 velocity = {0, 0, 0};

    DisableCursor();
    SetTargetFPS(60);

    while (!WindowShouldClose()) {

        // ---- Update ----

#define POSX (camera.position.x + 0.5f)
#define POSY (camera.position.y + 0.5f)
#define POSZ (camera.position.z + 0.5f)

        // check if standing on ground
        bool standing;
        if (velocity.y <= 0.01 && POSX > 0 && POSX <= WORLD_X + 1.0f &&
            POSY > 0 && POSY <= WORLD_Y + 1.0f && POSZ > 0 &&
            POSZ <= WORLD_Z + 1.0f &&
            world[(int)POSX][(int)(POSY - 1.5f)][(int)POSZ]) {

            standing = true;
            velocity.y = 0;
            float delta = 0.49f - (POSY - floor(POSY));
            camera.position.y += delta;
            camera.target.y += delta;
        } else {
            standing = false;
            velocity.y -= GRAVITY;
            if (velocity.y < -MAX_SPEED)
                velocity.y = -MAX_SPEED;
        }

        Vector3 camRot = CameraRotUnit(camera.position, camera.target);

        if (standing) {
            velocity.x = 0.0f;
            velocity.z = 0.0f;

            float invr =
                1.0f /
                cosf(asinf(fabsf(fminf(0.999f, fmaxf(-0.999f, camRot.y)))));
            float vx = invr * camRot.x;
            float vz = invr * camRot.z;

            if (IsKeyDown(KEY_W)) {
                velocity.x += vx * WALK_SPEED;
                velocity.z += vz * WALK_SPEED;
            }

            if (IsKeyDown(KEY_S)) {
                velocity.x += -vx * WALK_SPEED;
                velocity.z += -vz * WALK_SPEED;
            }

            if (IsKeyDown(KEY_A)) {
                float theta2 = atan2f(vx, vz) + M_PI / 2.0f;
                velocity.x += sinf(theta2) * WALK_SPEED;
                velocity.z += cosf(theta2) * WALK_SPEED;
            }

            if (IsKeyDown(KEY_D)) {
                float theta2 = atan2f(vx, vz) - M_PI / 2.0f;
                velocity.x += sinf(theta2) * WALK_SPEED;
                velocity.z += cosf(theta2) * WALK_SPEED;
            }

            if (IsKeyDown(KEY_SPACE)) {
                velocity.y += 0.20f;
            }
        }

        {
            Vector2 mouseDelta = GetMouseDelta();
            float deltaRotX = mouseDelta.x * CAMERA_SENS;
            float deltaRotY = mouseDelta.y * CAMERA_SENS;
            float x = camRot.x, y = camRot.y, z = camRot.z;
            float phi = atan2f(x, z);
            float h = sqrtf(powf(x, 2) + powf(z, 2));
            float theta = atan2f(h, y);

            theta += deltaRotY;
            phi -= deltaRotX;

            if (theta < 0.02 * M_PI)
                theta = 0.02 * M_PI;
            else if (theta > 0.98 * M_PI)
                theta = 0.98 * M_PI;

            y = cosf(theta);
            h = sinf(theta);
            z = h * cosf(phi);
            x = h * sinf(phi);

            camera.target.x = camera.position.x + x;
            camera.target.y = camera.position.y + y;
            camera.target.z = camera.position.z + z;
        }

        // update velocity
        camera.position.x += velocity.x;
        camera.position.y += velocity.y;
        camera.position.z += velocity.z;
        camera.target.x += velocity.x;
        camera.target.y += velocity.y;
        camera.target.z += velocity.z;

        // ray march to check for target block
        Vector3 targetBlock = {camera.position.x + 1000, 0, 0};
        float cx = POSX;
        float cy = POSY;
        float cz = POSZ;

        for (int i = 0; i < 120; i++) {
            cx += camRot.x / 20;
            cy += camRot.y / 20;
            cz += camRot.z / 20;
            if (cx < 0 || cy < 0 || cz < 0)
                break;
            if (cx > WORLD_X || cy > WORLD_Y || cz > WORLD_Z)
                break;
            if (world[(int)cx][(int)cy][(int)cz]) {
                targetBlock = (Vector3){floor(cx), floor(cy), floor(cz)};
                break;
            }
        }

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) &&
            Vec3Dist(targetBlock, camera.position) < 5) {

            int x = targetBlock.x;
            int y = targetBlock.y;
            int z = targetBlock.z;
            world[x][y][z] = AIR;
            targetBlock = (Vector3){camera.position.x + 1000, 0, 0};
        }

        if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) &&
            Vec3Dist(targetBlock, camera.position) < 5) {

            int x = targetBlock.x;
            int y = targetBlock.y;
            int z = targetBlock.z;
            Ray ray = GetScreenToWorldRay(
                (Vector2){(float)width / 2.0f, (float)height / 2.0f}, camera);
            RayCollision rayCollision = GetRayCollisionBox(
                ray, (BoundingBox){{x - 0.5f, y - 0.5f, z - 0.5f},
                                   {x + 0.5f, y + 0.5f, z + 0.5f}});

            x += rayCollision.normal.x;
            y += rayCollision.normal.y;
            z += rayCollision.normal.z;

            if (x > 0 && x <= WORLD_X && y > 0 && y <= WORLD_Y && z > 0 &&
                z <= WORLD_Z) {

                world[x][y][z] = SPONGE;
                targetBlock = (Vector3){camera.position.x + 1000, 0, 0};
            }
        }

        // ---- Draw ----

        BeginDrawing();

        ClearBackground(SKYBLUE);

        BeginMode3D(camera);

        Ray cameraRay = GetScreenToWorldRay(
            (Vector2){(float)width / 2, (float)height / 2}, camera);

        for (int x = 1; x <= WORLD_X; x++) {
            for (int y = 1; y <= WORLD_Y; y++) {
                for (int z = 1; z <= WORLD_Z; z++) {
                    if (!world[x][y][z])
                        continue;
                    if (world[x - 1][y][z] && world[x + 1][y][z] &&
                        world[x][y - 1][z] && world[x][y + 1][z] &&
                        world[x][y][z - 1] && world[x][y][z + 1])
                        continue;

                    if (world[x][y][z] == SPONGE) {
                        DrawModel(spongeModel, (Vector3){x,y,z}, 1, WHITE);
                    } else {
                        DrawCube((Vector3){x, y, z}, 1, 1, 1,
                                 BlockMaterialColor(world[x][y][z]));
                    }
                }
            }
        }

        if (Vec3Dist(targetBlock, camera.position) < 5)
            DrawCubeWires(targetBlock, 1, 1, 1, BLACK);

#ifdef DEBUG
        // Draw axis
        DrawLine3D((Vector3){0, 0, 0}, (Vector3){1000, 0, 0}, RED);
        DrawLine3D((Vector3){0, 0, 0}, (Vector3){0, 1000, 0}, GREEN);
        DrawLine3D((Vector3){0, 0, 0}, (Vector3){0, 0, 1000}, BLUE);
#endif

        EndMode3D();

        // Crosshair
        DrawRectangleRec(
            (Rectangle){(float)width / 2 - 7, (float)height / 2 - 2, 14, 4},
            WHITE);
        DrawRectangleRec(
            (Rectangle){(float)width / 2 - 2, (float)height / 2 - 7, 4, 14},
            WHITE);

#ifdef DEBUG
        DrawFPS(10, 10);

        // Draw info boxes
        DrawRectangle(600, 5, 195, 120, Fade(SKYBLUE, 0.5f));
        DrawRectangleLines(600, 5, 195, 120, BLUE);

        DrawText("Camera status:", 610, 15, 10, BLACK);
        DrawText(TextFormat("- Position: (%06.3f, %06.3f, %06.3f)",
                            camera.position.x, camera.position.y,
                            camera.position.z),
                 610, 60, 10, BLACK);
        DrawText(TextFormat("- Target: (%06.3f, %06.3f, %06.3f)",
                            camera.target.x, camera.target.y, camera.target.z),
                 610, 75, 10, BLACK);
        DrawText(TextFormat("- Up: (%06.3f, %06.3f, %06.3f)", camera.up.x,
                            camera.up.y, camera.up.z),
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
