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

    rm.ryuTexture = loadTextureReplacingColor( 
        "resources/images/chars/ryu.png", 
        (Color) { 85, 170, 255, 255 }, 
        BLANK
    );

    rm.kenTexture = loadTextureReplacingColor( 
        "resources/images/chars/ken.png", 
        (Color) { 85, 170, 255, 255 }, 
        BLANK
    );

    rm.blankaStageTexture = LoadTexture( "resources/images/stages/blanka-stage.png" );
    rm.chunliStageTexture = LoadTexture( "resources/images/stages/chunli-stage.png" );
    rm.eHondaStageTexture = LoadTexture( "resources/images/stages/ehonda-stage.png" );
    rm.guileStageTexture = LoadTexture( "resources/images/stages/guile-stage.png" );
    rm.kenStageTexture = LoadTexture( "resources/images/stages/ken-stage.png" );

}

void unloadResourcesResourceManager( void ) {

    UnloadTexture( rm.ryuTexture );
    UnloadTexture( rm.kenTexture );

    UnloadTexture( rm.blankaStageTexture );
    UnloadTexture( rm.chunliStageTexture );
    UnloadTexture( rm.eHondaStageTexture );
    UnloadTexture( rm.guileStageTexture );
    UnloadTexture( rm.kenStageTexture );

    //UnloadSound( rm.soundExample );
    //UnloadMusicStream( rm.musicExample );
}