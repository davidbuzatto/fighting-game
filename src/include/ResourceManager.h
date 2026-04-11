/**
 * @file ResourceManager.h
 * @author Prof. Dr. David Buzatto
 * @brief ResourceManager struct and function declarations.
 * 
 * @copyright Copyright (c) 2026
 */
#pragma once

#include "raylib/raylib.h"

typedef struct ResourceManager {

    Texture2D ryuSpriteMapTexture;
    Texture2D ryuSpecialMovesSpriteMapTexture;
    Image ryuPalleteImage;

    Texture2D kenSpriteMapTexture;
    Texture2D kenSpecialMovesSpriteMapTexture;
    Image kenPalleteImage;

    Texture2D kenStageAnchorTexture;
    Texture2D kenStageFloorTexture;
    Texture2D kenStageBack01Texture;
    Texture2D kenStageBack02Texture;
    Texture2D kenStageBack03Texture;
    Texture2D kenStageBoat01Texture;
    Texture2D kenStageBoat02Texture;
    Texture2D kenStageBoat03Texture;
    Texture2D kenStageBoat04Texture;
    Texture2D kenStageBoat05Texture;

    Texture2D modelStageTexture;

    Texture2D effectsTexture;
    Texture2D fontsTexture;
    Texture2D inputIconsTexture;

    Sound ryuAttackLowSound;
    Sound ryuAttackMidSound;
    Sound ryuAttackHighSound;
    Sound ryuHitSound;
    Sound ryuHadoukenSound;
    Sound ryuShoryukenSound;
    Sound ryuTatsumakiSound;

    Sound kenAttackLowSound;
    Sound kenAttackMidSound;
    Sound kenAttackHighSound;
    Sound kenHitSound;
    Sound kenHadoukenSound;
    Sound kenShoryukenSound;
    Sound kenTatsumakiSound;

    Music kenTheme;

} ResourceManager;

/**
 * @brief Global ResourceManager instance.
 */
extern ResourceManager rm;

/**
 * @brief Load global game resources, linking them in the global instance of
 * ResourceManager called rm.
 */
void loadResourcesResourceManager( void );

/**
 * @brief Unload global game resources.
 */
void unloadResourcesResourceManager( void );