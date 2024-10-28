#pragma once
#include "../lib/raylib/raylib.h"

#define WORLD_X 32
#define WORLD_Y 24
#define WORLD_Z 32
#define GRAVITY 0.01f
#define AIR_DRAG 0.005f
#define FRICTION_D 0.15f
#define FRICTION_S 0.001f

typedef enum: char {
    AIR = 0,
    ROCK,
    ROCK_2,
    ROCK_3,
    GRASS,
    GRASS_2,
    GRASS_3,
    SPONGE,
} BlockMaterial;

void InitWorld();
Color BlockMaterialColor(const BlockMaterial m);
BlockMaterial WorldBlock(const int x, const int y, const int z);
BlockMaterial WorldBlockSafe(const int x, const int y, const int z);
BoundingBox WorldBlockBoundingBox(const int x, const int y, const int z);
void SetWorldBlock(const int x, const int y, const int z, BlockMaterial m);
void RenderWorld(Vector3 targetBlock, Camera *camera, Model *spongeModel);
