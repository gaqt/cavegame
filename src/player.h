#pragma once
#include "../lib/raylib/raylib.h"

#define GRAVITY 0.01f
#define MAX_SPEED 3.0f
#define WALK_SPEED 0.09f
#define CAMERA_SENS 0.002f

typedef struct {
    Vector3 pos;
    Vector2 lookRot;
    Vector3 velocity;
    Vector3 targetBlock;
    bool standing;
} Player;

Player InitPlayer(const float x, const float y, const float z);
// Vector3 CameraRotUnit(const Player *player);
Vector3 PlayerGetCartesianLookRot(const Player *player);
void PlayerUpdateStandingState(Player *player);
void PlayerDoWalk(Player *player);
void PlayerDoMouseLook(Player *player);
void PlayerUpdatePos(Player *player);
void PlayerUpdateTargetBlock(Player *player);
void PlayerHandleInteractWithTargetBlock(Player *player);
Camera PlayerGetCamera(const Player *player);
