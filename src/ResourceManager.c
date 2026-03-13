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

static Texture2D loadTextureReplacingColor( const char *path, Color sourceColor, Color targetColor ) {
    Image img = LoadImage( path );
    ImageFormat( &img, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8 );
    ImageColorReplace( &img, sourceColor, targetColor );
    Texture2D texture = LoadTextureFromImage( img );
    UnloadImage( img );
    return texture;
}

void loadResourcesResourceManager( void ) {

    rm.playerTexture = loadTextureReplacingColor( 
        "resources/images/ryu.png", 
        (Color) { 85, 170, 255, 255 }, 
        BLANK
    );

    rm.stageTexture = loadTextureReplacingColor( 
        "resources/images/guile-stage.png", 
        (Color) { 48, 48, 88, 255 }, 
        BLANK
    );

}

void unloadResourcesResourceManager( void ) {
    UnloadTexture( rm.playerTexture );
    UnloadTexture( rm.stageTexture );
    //UnloadSound( rm.soundExample );
    //UnloadMusicStream( rm.musicExample );
}