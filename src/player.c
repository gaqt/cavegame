#include "player.h"
#include "cavegame.h"
#include "world.h"
#include <math.h>

Player InitPlayer(const float x, const float y, const float z) {
    return (Player){
        .pos = {x, y, z},
        .lookRot = {0, PI / 2},
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

void PlayerUpdateEnvironmentForces(Player *player) {

    float dx = 0.0f;
    float dy = 0.0f;
    float dz = 0.0f;

    dx -= (!signbit(player->velocity.x) * 2.0f - 1.0f) *
          powf(player->velocity.x, 2) * AIR_DRAG;
    dy -= (!signbit(player->velocity.y) * 2.0f - 1.0f) *
          powf(player->velocity.y, 2) * AIR_DRAG;
    dz -= (!signbit(player->velocity.z) * 2.0f - 1.0f) *
          powf(player->velocity.z, 2) * AIR_DRAG;

    if (player->standing) {
        dx -= player->velocity.x * FRICTION_D;
        dz -= player->velocity.z * FRICTION_D;
    } else {
        dy -= GRAVITY;
    }

    player->velocity.x += dx;
    player->velocity.y += dy;
    player->velocity.z += dz;

    if (player->standing && sqrtf(powf(player->velocity.x, 2) +
                                  powf(player->velocity.z, 2)) < FRICTION_S) {

        player->velocity.x = 0.0f;
        player->velocity.z = 0.0f;
    }
}

void PlayerDoWalk(Player *player) {

    float vx = sinf(player->lookRot.x);
    float vz = cosf(player->lookRot.x);
    float walkX = 0.0f;
    float walkZ = 0.0f;

    if (IsKeyDown(KEY_W)) {
        walkX += vx;
        walkZ += vz;
    }

    if (IsKeyDown(KEY_S)) {
        walkX += -vx;
        walkZ += -vz;
    }

    if (IsKeyDown(KEY_A)) {
        float theta2 = atan2f(vx, vz) + PI / 2.0f;
        walkX += sinf(theta2);
        walkZ += cosf(theta2);
    }

    if (IsKeyDown(KEY_D)) {
        float theta2 = atan2f(vx, vz) - PI / 2.0f;
        walkX += sinf(theta2);
        walkZ += cosf(theta2);
    }

    float magnitude = sqrtf(powf(walkX, 2) + powf(walkZ, 2));
    if (magnitude > 1.0f) {
        walkX /= magnitude;
        walkZ /= magnitude;
    }

    if (player->standing) {
        player->velocity.x += walkX * WALK_SPEED;
        player->velocity.z += walkZ * WALK_SPEED;
        if (IsKeyDown(KEY_SPACE)) {
            player->velocity.y += 0.15f;
        }
    } else {
        player->velocity.x += walkX * AIR_SPEED;
        player->velocity.z += walkZ * AIR_SPEED;
    }
}

void PlayerUpdateCollisionsFloor(Player *player) {
    BoundingBox playerBox = PlayerGetBoundingBox(player);

    playerBox.min.z += 0.05f;
    playerBox.max.z -= 0.05f;
    playerBox.min.x += 0.05f;
    playerBox.max.x -= 0.05f;

    BoundingBox floorBox1 = WorldBlockBoundingBox(
        playerBox.min.x, playerBox.min.y, playerBox.min.z);
    BoundingBox floorBox2 = WorldBlockBoundingBox(
        playerBox.max.x, playerBox.min.y, playerBox.min.z);
    BoundingBox floorBox3 = WorldBlockBoundingBox(
        playerBox.max.x, playerBox.min.y, playerBox.max.z);
    BoundingBox floorBox4 = WorldBlockBoundingBox(
        playerBox.min.x, playerBox.min.y, playerBox.max.z);

    if ((CheckCollisionBoxes(playerBox, floorBox1) ||
         CheckCollisionBoxes(playerBox, floorBox2) ||
         CheckCollisionBoxes(playerBox, floorBox3) ||
         CheckCollisionBoxes(playerBox, floorBox4)) &&
        player->velocity.y <= 0) {

        player->standing = true;
        player->velocity.y = 0;
        player->pos.y = 0.495 + floorf(player->pos.y);
        player->pos.y = 0.495 + floorf(player->pos.y);
    } else {
        player->standing = false;
    }
}

void PlayerUpdateCollisionsCeil(Player *player) {
    BoundingBox playerBox = PlayerGetBoundingBox(player);

    playerBox.min.z += 0.05f;
    playerBox.max.z -= 0.05f;
    playerBox.min.x += 0.05f;
    playerBox.max.x -= 0.05f;

    BoundingBox ceilBox1 = WorldBlockBoundingBox(
        playerBox.min.x, playerBox.max.y, playerBox.min.z);
    BoundingBox ceilBox2 = WorldBlockBoundingBox(
        playerBox.max.x, playerBox.max.y, playerBox.min.z);
    BoundingBox ceilBox3 = WorldBlockBoundingBox(
        playerBox.max.x, playerBox.max.y, playerBox.max.z);
    BoundingBox ceilBox4 = WorldBlockBoundingBox(
        playerBox.min.x, playerBox.max.y, playerBox.max.z);

    if ((CheckCollisionBoxes(playerBox, ceilBox1) ||
         CheckCollisionBoxes(playerBox, ceilBox2) ||
         CheckCollisionBoxes(playerBox, ceilBox3) ||
         CheckCollisionBoxes(playerBox, ceilBox4)) &&
        player->velocity.y > 0) {

        player->velocity.y = 0;
        player->pos.y = 0.795 + floorf(player->pos.y);
        player->pos.y = 0.795 + floorf(player->pos.y);
    }
}

void PlayerUpdateCollisionsSides(Player *player) {
    BoundingBox playerBox = PlayerGetBoundingBox(player);

    playerBox.min.y += 0.05f;
    playerBox.max.y -= 0.05f;

    for (float dx = 0.0f; dx <= 0.4f; dx += 0.4f) {
        for (float dy = 0.0f; dy <= 2.0f; dy += 1.0f) {
            for (float dz = 0.0f; dz <= 0.4f; dz += 0.4f) {
                int blockX = playerBox.min.x + dx;
                int blockY = playerBox.min.y + dy;
                int blockZ = playerBox.min.z + dz;
                BoundingBox blockBox =
                    WorldBlockBoundingBox(blockX, blockY, blockZ);

                if (!CheckCollisionBoxes(playerBox, blockBox))
                    continue;

                float distX = (float)blockX + 0.5f - player->pos.x;
                float distZ = (float)blockZ + 0.5f - player->pos.z;

                if (fabsf(distX) > fabsf(distZ)) {
                    if (distX > 0 && player->velocity.x > 0) {
                        player->velocity.x = 0;
                        player->pos.x = -0.195f + ceilf(player->pos.x);
                        player->pos.x = -0.195f + ceilf(player->pos.x);
                    } else if (distX < 0 && player->velocity.x < 0) {
                        player->velocity.x = 0;
                        player->pos.x = +0.195f + floorf(player->pos.x);
                        player->pos.x = +0.195f + floorf(player->pos.x);
                    }
                } else {
                    if (distZ > 0 && player->velocity.z > 0) {
                        player->velocity.z = 0;
                        player->pos.z = -0.195f + ceilf(player->pos.z);
                        player->pos.z = -0.195f + ceilf(player->pos.z);
                    } else if (distZ < 0 && player->velocity.z < 0) {
                        player->velocity.z = 0;
                        player->pos.z = +0.195f + floorf(player->pos.z);
                        player->pos.z = +0.195f + floorf(player->pos.z);
                    }
                }
            }
        }
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
    float cx = player->pos.x;
    float cy = player->pos.y;
    float cz = player->pos.z;

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
            ray, (BoundingBox){{x, y, z}, {x + 1.0f, y + 1.0f, z + 1.0f}});

        x += rayCollision.normal.x;
        y += rayCollision.normal.y;
        z += rayCollision.normal.z;

        if (x < 0 || x > WORLD_X)
            return;
        if (y < 0 || y > WORLD_Y)
            return;
        if (z < 0 || z > WORLD_Z)
            return;

        BoundingBox playerBox = PlayerGetBoundingBox(player);
        playerBox.min.x += 0.05;
        playerBox.min.y += 0.05;
        playerBox.min.z += 0.05;
        playerBox.max.x -= 0.05;
        playerBox.max.y -= 0.05;
        playerBox.max.z -= 0.05;
        BoundingBox blockBox = {{x, y, z}, {x + 1.0f, y + 1.0f, z + 1.0f}};
        if (CheckCollisionBoxes(playerBox, blockBox))
            return;

        SetWorldBlock(x, y, z, SPONGE);
        player->targetBlock = (Vector3){player->pos.x + 1000, 0, 0};
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

BoundingBox PlayerGetBoundingBox(const Player *player) {

    return (BoundingBox){.min = {.x = player->pos.x - 0.20f,
                                 .y = player->pos.y - 1.5f,
                                 .z = player->pos.z - 0.20f},
                         .max = {.x = player->pos.x + 0.20f,
                                 .y = player->pos.y + 0.3f,
                                 .z = player->pos.z + 0.20f}};
}
