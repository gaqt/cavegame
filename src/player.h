#pragma once
#include "../lib/raylib/raylib.h"

#define GRAVITY 0.01f
#define MAX_SPEED 3.0f
#define WALK_SPEED 0.07f
#define CAMERA_SENS 0.002f

typedef struct {
    Vector3 pos;
    Vector2 lookRot;
    Vector3 velocity;
    Vector3 targetBlock;
    bool standing;
} Player;

Player InitPlayer(const float x, const float y, const float z);
Vector3 PlayerGetCartesianLookRot(const Player *player);
void PlayerDoWalk(Player *player);
void PlayerUpdateCollisionsFloor(Player *player);
void PlayerUpdateCollisionsCeil(Player *player);
void PlayerUpdateCollisionsSides(Player *player);
void PlayerDoMouseLook(Player *player);
void PlayerUpdatePos(Player *player);
void PlayerUpdateTargetBlock(Player *player);
void PlayerHandleInteractWithTargetBlock(Player *player);
Camera PlayerGetCamera(const Player *player);
BoundingBox PlayerGetBoundingBox(const Player *player);
