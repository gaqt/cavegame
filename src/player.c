#include "player.h"
#include "cavegame.h"
#include "world.h"
#include <math.h>

Player InitPlayer(const float x, const float y, const float z) {
    return (Player){
        .pos = {x, y, z},
        .lookRot = {0, 1},
        .velocity = {0, 0, 0},
        .targetBlock = {x + 1000, y, z},
        .standing = false,
    };
}

Vector3 PlayerGetCartesianLookRot(const Player *player) {
    float phi = player->lookRot.x;
    float theta = player->lookRot.y;

    return (Vector3){
        .x = sinf(theta) * sinf(phi),
        .y = cosf(theta),
        .z = sinf(theta) * cosf(phi),
    };
}

void PlayerUpdateStandingState(Player *player) {
    float blockPosX = player->pos.x + 0.5f;
    float blockPosY = player->pos.y + 0.5f;
    float blockPosZ = player->pos.z + 0.5f;

    if (player->velocity.y <= 0.01 && blockPosX > 0 &&
        blockPosX <= WORLD_X + 1.0f && blockPosY > 0 &&
        blockPosY <= WORLD_Y + 1.0f && blockPosZ > 0 &&
        blockPosZ <= WORLD_Z + 1.0f &&
        WorldBlock(blockPosX, blockPosY - 1.5f, blockPosZ)) {

        player->standing = true;
        player->velocity.y = 0;
        player->pos.y = -0.01f + floorf(blockPosY);
        player->pos.y = -0.01f + floorf(blockPosY);
    } else {
        player->standing = false;
        player->velocity.y -= GRAVITY;
        if (player->velocity.y < -MAX_SPEED)
            player->velocity.y = -MAX_SPEED;
    }
}

void PlayerDoWalk(Player *player) {
    if (!player->standing)
        return;

    player->velocity.x = 0.0f;
    player->velocity.z = 0.0f;

    float vx = sinf(player->lookRot.x);
    float vz = cosf(player->lookRot.x);

    if (IsKeyDown(KEY_W)) {
        player->velocity.x += vx * WALK_SPEED;
        player->velocity.z += vz * WALK_SPEED;
    }

    if (IsKeyDown(KEY_S)) {
        player->velocity.x += -vx * WALK_SPEED;
        player->velocity.z += -vz * WALK_SPEED;
    }

    if (IsKeyDown(KEY_A)) {
        float theta2 = atan2f(vx, vz) + PI / 2.0f;
        player->velocity.x += sinf(theta2) * WALK_SPEED;
        player->velocity.z += cosf(theta2) * WALK_SPEED;
    }

    if (IsKeyDown(KEY_D)) {
        float theta2 = atan2f(vx, vz) - PI / 2.0f;
        player->velocity.x += sinf(theta2) * WALK_SPEED;
        player->velocity.z += cosf(theta2) * WALK_SPEED;
    }

    if (IsKeyDown(KEY_SPACE)) {
        player->velocity.y += 0.20f;
    }
}

void PlayerDoMouseLook(Player *player) {
    Vector2 mouseDelta = GetMouseDelta();
    player->lookRot.y += mouseDelta.y * CAMERA_SENS;
    player->lookRot.x -= mouseDelta.x * CAMERA_SENS;

    if (player->lookRot.y < 0.02 * PI)
        player->lookRot.y = 0.02 * PI;
    else if (player->lookRot.y > 0.98 * PI)
        player->lookRot.y = 0.98 * PI;

    if (player->lookRot.x < 0.0f)
        player->lookRot.x += 2.0f * PI;
    else if (player->lookRot.x > 2.0f * PI)
        player->lookRot.x -= 2.0f * PI;
}

void PlayerUpdatePos(Player *player) {
    player->pos.x += player->velocity.x;
    player->pos.y += player->velocity.y;
    player->pos.z += player->velocity.z;
}

void PlayerUpdateTargetBlock(Player *player) {
    player->targetBlock = (Vector3){player->pos.x + 1000, 0, 0};
    float cx = player->pos.x + 0.5f;
    float cy = player->pos.y + 0.5f;
    float cz = player->pos.z + 0.5f;

    Vector3 lookRotC = PlayerGetCartesianLookRot(player);

    for (int i = 0; i < 120; i++) {
        cx += lookRotC.x / 20;
        cy += lookRotC.y / 20;
        cz += lookRotC.z / 20;
        if (cx < 0 || cy < 0 || cz < 0)
            break;
        if (cx > WORLD_X || cy > WORLD_Y || cz > WORLD_Z)
            break;
        if (WorldBlock(cx, cy, cz)) {
            player->targetBlock = (Vector3){floor(cx), floor(cy), floor(cz)};
            break;
        }
    }
}

void PlayerHandleInteractWithTargetBlock(Player *player) {
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) &&
        Vec3Dist(player->targetBlock, player->pos) < 5) {

        int x = player->targetBlock.x;
        int y = player->targetBlock.y;
        int z = player->targetBlock.z;
        SetWorldBlock(x, y, z, AIR);
        player->targetBlock = (Vector3){player->pos.x + 1000, 0, 0};
    }

    if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) &&
        Vec3Dist(player->targetBlock, player->pos) < 5) {

        int x = player->targetBlock.x;
        int y = player->targetBlock.y;
        int z = player->targetBlock.z;
        Ray ray = GetScreenToWorldRay(
            (Vector2){(float)SCREEN_W / 2.0f, (float)SCREEN_H / 2.0f},
            PlayerGetCamera(player));
        RayCollision rayCollision = GetRayCollisionBox(
            ray, (BoundingBox){{x - 0.5f, y - 0.5f, z - 0.5f},
                               {x + 0.5f, y + 0.5f, z + 0.5f}});

        x += rayCollision.normal.x;
        y += rayCollision.normal.y;
        z += rayCollision.normal.z;

        if (x > 0 && x <= WORLD_X && y > 0 && y <= WORLD_Y && z > 0 &&
            z <= WORLD_Z) {

            SetWorldBlock(x, y, z, SPONGE);
            player->targetBlock = (Vector3){player->pos.x + 1000, 0, 0};
        }
    }
}

Camera PlayerGetCamera(const Player *player) {

    Vector3 rotC = PlayerGetCartesianLookRot(player);
    Vector3 target = {.x = player->pos.x + rotC.x,
                      .y = player->pos.y + rotC.y,
                      .z = player->pos.z + rotC.z};

    return (Camera){.position = player->pos,
                    .target = target,
                    .up = {0.0f, 1.0f, 0.0f},
                    .fovy = 80.0f,
                    .projection = CAMERA_PERSPECTIVE};
}
