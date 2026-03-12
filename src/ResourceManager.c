/**
 * @file ResourceManager.c
 * @author Prof. Dr. David Buzatto
 * @brief ResourceManager implementation.
 * 
 * @copyright Copyright (c) 2026
 */
#include <stdio.h>
#include <stdlib.h>

#include "ResourceManager.h"
#include "raylib/raylib.h"

ResourceManager rm = { 0 };

void loadResourcesResourceManager( void ) {
    rm.playerTexture = LoadTexture( "resources/images/RyuCE.png" );
    //rm.soundExample = LoadSound( "resources/sfx/powerUp.wav" );
    //rm.musicExample = LoadMusicStream( "resources/musics/overworld1.ogg" );
}

void unloadResourcesResourceManager( void ) {
    UnloadTexture( rm.playerTexture );
    //UnloadSound( rm.soundExample );
    //UnloadMusicStream( rm.musicExample );
}