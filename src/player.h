#pragma once
#include <raylib.h>

#define WALK_SPEED 0.015f
#define CAMERA_SENS 0.002f
#define AIR_SPEED 0.0013f

typedef struct {
    Vector3 pos;
    Vector2 lookRot;
    Vector3 velocity;
    Vector3 targetBlock;
    bool standing;
} Player;

Player InitPlayer(const float x, const float y, const float z);
Vector3 PlayerGetCartesianLookRot(const Player *player);
void PlayerUpdateEnvironmentForces(Player *player);
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
