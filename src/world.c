#include "world.h"
#include "cavegame.h"
#include <stdlib.h>

BlockMaterial world[WORLD_X + 2][WORLD_Y + 2][WORLD_Z + 2] = {AIR};

void InitWorld() {
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
}

Color BlockMaterialColor(const BlockMaterial m) {
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

BlockMaterial WorldBlock(const int x, const int y, const int z) {
    return world[x][y][z];
}

BlockMaterial WorldBlockSafe(const int x, const int y, const int z) {
    if (x < 1 || x > WORLD_X)
        return AIR;
    if (y < 1 || y > WORLD_Y)
        return AIR;
    if (z < 1 || z > WORLD_Z)
        return AIR;

    return world[x][y][z];
}

BoundingBox WorldBlockBoundingBox(const int x, const int y, const int z) {

    if (WorldBlockSafe(x, y, z) == AIR)
        return (BoundingBox){{1000, 1000, 1000}, {1000, 1000, 1000}};

    return (BoundingBox){.min = {x, y, z},
                         .max = {
                             .x = x + 1.0f,
                             .y = y + 1.0f,
                             .z = z + 1.0f,
                         }};
}

void SetWorldBlock(const int x, const int y, const int z, BlockMaterial m) {
    world[x][y][z] = m;
}

void RenderWorld(Vector3 targetBlock, Camera *camera, Model *spongeModel) {
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
                    DrawModel(*spongeModel,
                              (Vector3){x + 0.5f, y + 0.5f, z + 0.5f}, 1,
                              WHITE);
                } else {
                    DrawCube((Vector3){x + 0.5f, y + 0.5f, z + 0.5f}, 1, 1, 1,
                             BlockMaterialColor(world[x][y][z]));
                }
            }
        }
    }

    if (Vec3Dist(targetBlock, camera->position) < 5)
        DrawCubeWires((Vector3){targetBlock.x + 0.5f, targetBlock.y + 0.5f,
                                targetBlock.z + 0.5f},
                      1, 1, 1, BLACK);

#ifdef DEBUG
    // Draw axis
    DrawLine3D((Vector3){0, 0, 0}, (Vector3){1000, 0, 0}, RED);
    DrawLine3D((Vector3){0, 0, 0}, (Vector3){0, 1000, 0}, GREEN);
    DrawLine3D((Vector3){0, 0, 0}, (Vector3){0, 0, 1000}, BLUE);
#endif
}
