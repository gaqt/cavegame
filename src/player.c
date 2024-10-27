#include "player.h"
#include "cavegame.h"
#include "world.h"
#include <math.h>

Player InitPlayer(const float x, const float y, const float z) {
    Camera camera = {0};
    camera.position = (Vector3){x, y, z};
    // camera target must be 1 unit away from position
    camera.target = (Vector3){x, y, z + 1};
    camera.up = (Vector3){0.0f, 1.0f, 0.0f};
    camera.fovy = 80.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    Vector3 velocity = {0, 0, 0};

    return (Player){camera, velocity, (Vector3){x + 1000, y, z}, false};
}

Vector3 CameraRotUnit(const Player *player) {
    Vector3 target = player->camera.target;
    Vector3 pos = player->camera.position;

    Vector3 rot =
        (Vector3){target.x - pos.x, target.y - pos.y, target.z - pos.z};

    return rot;
}

void PlayerUpdateStandingState(Player *player) {

#define POSX (player->camera.position.x + 0.5f)
#define POSY (player->camera.position.y + 0.5f)
#define POSZ (player->camera.position.z + 0.5f)

    if (player->velocity.y <= 0.01 && POSX > 0 && POSX <= WORLD_X + 1.0f &&
        POSY > 0 && POSY <= WORLD_Y + 1.0f && POSZ > 0 &&
        POSZ <= WORLD_Z + 1.0f && WorldBlock(POSX, POSY - 1.5f, POSZ)) {

        player->standing = true;
        player->velocity.y = 0;
        float delta = 0.49f - (POSY - floorf(POSY));
        player->camera.position.y += delta;
        player->camera.target.y += delta;
    } else {
        player->standing = false;
        player->velocity.y -= GRAVITY;
        if (player->velocity.y < -MAX_SPEED)
            player->velocity.y = -MAX_SPEED;
    }
}

void PlayerDoWalk(Player *player) {
    Vector3 camRot = CameraRotUnit(player);

    if (player->standing) {
        player->velocity.x = 0.0f;
        player->velocity.z = 0.0f;

        float invr =
            1.0f / cosf(asinf(fabsf(fminf(0.999f, fmaxf(-0.999f, camRot.y)))));
        float vx = invr * camRot.x;
        float vz = invr * camRot.z;

        if (IsKeyDown(KEY_W)) {
            player->velocity.x += vx * WALK_SPEED;
            player->velocity.z += vz * WALK_SPEED;
        }

        if (IsKeyDown(KEY_S)) {
            player->velocity.x += -vx * WALK_SPEED;
            player->velocity.z += -vz * WALK_SPEED;
        }

        if (IsKeyDown(KEY_A)) {
            float theta2 = atan2f(vx, vz) + M_PI / 2.0f;
            player->velocity.x += sinf(theta2) * WALK_SPEED;
            player->velocity.z += cosf(theta2) * WALK_SPEED;
        }

        if (IsKeyDown(KEY_D)) {
            float theta2 = atan2f(vx, vz) - M_PI / 2.0f;
            player->velocity.x += sinf(theta2) * WALK_SPEED;
            player->velocity.z += cosf(theta2) * WALK_SPEED;
        }

        if (IsKeyDown(KEY_SPACE)) {
            player->velocity.y += 0.20f;
        }
    }
}

void PlayerDoMouseLook(Player *player) {
    Vector3 camRot = CameraRotUnit(player);

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

    player->camera.target.x = player->camera.position.x + x;
    player->camera.target.y = player->camera.position.y + y;
    player->camera.target.z = player->camera.position.z + z;
}

void PlayerUpdatePos(Player *player) {
    player->camera.position.x += player->velocity.x;
    player->camera.position.y += player->velocity.y;
    player->camera.position.z += player->velocity.z;
    player->camera.target.x += player->velocity.x;
    player->camera.target.y += player->velocity.y;
    player->camera.target.z += player->velocity.z;
}

void PlayerUpdateTargetBlock(Player *player) {
    Vector3 camRot = CameraRotUnit(player);

    player->targetBlock = (Vector3){player->camera.position.x + 1000, 0, 0};
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
        if (WorldBlock(cx, cy, cz)) {
            player->targetBlock = (Vector3){floor(cx), floor(cy), floor(cz)};
            break;
        }
    }
}

void PlayerHandleInteractWithTargetBlock(Player *player) {
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) &&
        Vec3Dist(player->targetBlock, player->camera.position) < 5) {

        int x = player->targetBlock.x;
        int y = player->targetBlock.y;
        int z = player->targetBlock.z;
        SetWorldBlock(x, y, z, AIR);
        player->targetBlock = (Vector3){player->camera.position.x + 1000, 0, 0};
    }

    if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) &&
        Vec3Dist(player->targetBlock, player->camera.position) < 5) {

        int x = player->targetBlock.x;
        int y = player->targetBlock.y;
        int z = player->targetBlock.z;
        Ray ray = GetScreenToWorldRay(
            (Vector2){(float)SCREEN_W / 2.0f, (float)SCREEN_H / 2.0f},
            player->camera);
        RayCollision rayCollision = GetRayCollisionBox(
            ray, (BoundingBox){{x - 0.5f, y - 0.5f, z - 0.5f},
                               {x + 0.5f, y + 0.5f, z + 0.5f}});

        x += rayCollision.normal.x;
        y += rayCollision.normal.y;
        z += rayCollision.normal.z;

        if (x > 0 && x <= WORLD_X && y > 0 && y <= WORLD_Y && z > 0 &&
            z <= WORLD_Z) {

            SetWorldBlock(x, y, z, SPONGE);
            player->targetBlock =
                (Vector3){player->camera.position.x + 1000, 0, 0};
        }
    }
}
