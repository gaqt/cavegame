#pragma once
#include "../lib/raylib/raylib.h"

#define WORLD_X 32
#define WORLD_Y 24
#define WORLD_Z 32

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
void SetWorldBlock(const int x, const int y, const int z, BlockMaterial m);
void RenderWorld(Vector3 targetBlock, Camera *camera, Model *spongeModel);
