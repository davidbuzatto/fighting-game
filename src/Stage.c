/**
 * @file Stage.c
 * @author Prof. Dr. David Buzatto
 * @brief Stage rendering and update implementation.
 *
 * @copyright Copyright (c) 2026
 */
#include <math.h>

#include "raylib/raylib.h"

#include "Stage.h"
#include "Types.h"

#define BACK_TEXTURES_COUNT 3
#define BOAT_TEXTURES_COUNT 5

static Texture2D *backTextures[BACK_TEXTURES_COUNT];
static Texture2D *boatTextures[BOAT_TEXTURES_COUNT];
static float stageTextureChangeTime = 0.2f;
static float stageTextureChangeCounter = 0.0f;
static int currentBackTexture = 0;
static int currentBoatTexture = 0;
static float boatTextureDrawOffsetY = 5;

void initStage( GameWorld *gw ) {

    backTextures[0] = gw->back01Texture;
    backTextures[1] = gw->back02Texture;
    backTextures[2] = gw->back03Texture;

    boatTextures[0] = gw->boat01Texture;
    boatTextures[1] = gw->boat02Texture;
    boatTextures[2] = gw->boat03Texture;
    boatTextures[3] = gw->boat04Texture;
    boatTextures[4] = gw->boat05Texture;

}

void updateStage( float delta ) {

    stageTextureChangeCounter += delta;
    if ( stageTextureChangeCounter >= stageTextureChangeTime ) {
        stageTextureChangeCounter = 0;
        currentBackTexture++;
        currentBoatTexture++;
    }

}

void drawStageBackground( GameWorld *gw ) {

    if ( !SHOW_MODEL_STAGE_TEXTURE ) {
        DrawTexture( *(backTextures[currentBackTexture%BACK_TEXTURES_COUNT]), 0, GetScreenHeight() - backTextures[currentBackTexture%BACK_TEXTURES_COUNT]->height, WHITE );
        DrawTexture(
            *(boatTextures[currentBoatTexture%BOAT_TEXTURES_COUNT]),
            0,
            GetScreenHeight() - boatTextures[currentBoatTexture%BOAT_TEXTURES_COUNT]->height + boatTextureDrawOffsetY * sinf( DEG2RAD * currentBoatTexture * 10 ) + boatTextureDrawOffsetY,
            WHITE
        );
        DrawTexture( *gw->floorTexture, 0, GetScreenHeight() - gw->floorTexture->height, WHITE );
    }

}

void drawStageForeground( GameWorld *gw ) {

    if ( !SHOW_MODEL_STAGE_TEXTURE ) {
        DrawTexture( *gw->anchorTexture, 0, GetScreenHeight() - gw->anchorTexture->height, WHITE );
    }

}
