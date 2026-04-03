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

    Texture2D ryuTexture;
    Texture2D kenTexture;

    Texture2D blankaStageTexture;
    Texture2D chunliStageTexture;
    Texture2D eHondaStageTexture;
    Texture2D guileStageTexture;
    Texture2D kenStageTexture;
    Texture2D modelStageTexture;

    Texture2D effectsTexture;
    Texture2D fontsTexture;

    //Sound soundExample;
    //Music musicExample;
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