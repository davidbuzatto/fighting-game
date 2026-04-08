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

static Texture2D loadTextureReplacingColor( const char *path, Color *sourceColors, Color *targetColors, int colorCount ) {
    Image img = LoadImage( path );
    ImageFormat( &img, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8 );
    for ( int i = 0; i < colorCount; i++ ) {
        ImageColorReplace( &img, sourceColors[i], targetColors[i] );
    }
    Texture2D texture = LoadTextureFromImage( img );
    UnloadImage( img );
    return texture;
}

void loadResourcesResourceManager( void ) {

    rm.ryuTexture = loadTextureReplacingColor( 
        "resources/images/chars/ryu.png", 
        (Color[]) { 
            (Color) { 85, 170, 255, 255 },
            (Color) { 0, 85, 127, 255 }
        },
        (Color[]) { 
            BLANK,
            BLANK
        },
        2
    );

    rm.kenTexture = loadTextureReplacingColor( 
        "resources/images/chars/ken.png", 
        (Color[]) { 
            (Color) { 85, 170, 255, 255 },
            (Color) { 0, 85, 127, 255 }
        }, 
        (Color[]) { 
            BLANK,
            BLANK
        },
        2
    );

    rm.blankaStageTexture = LoadTexture( "resources/images/stages/blanka-stage.png" );
    rm.chunliStageTexture = LoadTexture( "resources/images/stages/chunli-stage.png" );
    rm.eHondaStageTexture = LoadTexture( "resources/images/stages/ehonda-stage.png" );
    rm.guileStageTexture = LoadTexture( "resources/images/stages/guile-stage.png" );
    rm.kenStageTexture = LoadTexture( "resources/images/stages/ken-stage.png" );
    rm.modelStageTexture = LoadTexture( "resources/images/stages/model-stage.png" );

    rm.effectsTexture = loadTextureReplacingColor( 
        "resources/images/misc/effects.png", 
        (Color[]) { 
            (Color) { 0, 85, 127, 255 }
        }, 
        (Color[]) { 
            BLANK
        },
        1
    );

    rm.fontsTexture = LoadTexture( "resources/images/misc/fonts.png" );

    rm.inputIconsTexture = loadTextureReplacingColor( 
        "resources/images/misc/input-icons.png", 
        (Color[]) { 
            (Color) { 32, 0, 223, 255 }
        }, 
        (Color[]) { 
            BLANK
        },
        1
    );

    rm.ryuAttackLowSound = LoadSound( "resources/sfx/attack-low.wav" );
    rm.ryuAttackMidSound = LoadSound( "resources/sfx/attack-mid.wav" );
    rm.ryuAttackHighSound = LoadSound( "resources/sfx/attack-high.wav" );
    rm.ryuHitSound = LoadSound( "resources/sfx/hit.wav" );
    rm.ryuHadoukenSound = LoadSound( "resources/sfx/ryu-hadouken.wav" );
    rm.ryuShoryukenSound = LoadSound( "resources/sfx/ryu-shoryuken.wav" );
    rm.ryuTatsumakiSound = LoadSound( "resources/sfx/ryu-tatsumaki.wav" );

    rm.kenAttackLowSound = LoadSound( "resources/sfx/attack-low.wav" );
    rm.kenAttackMidSound = LoadSound( "resources/sfx/attack-mid.wav" );
    rm.kenAttackHighSound = LoadSound( "resources/sfx/attack-high.wav" );
    rm.kenHitSound = LoadSound( "resources/sfx/hit.wav" );
    rm.kenHadoukenSound = LoadSound( "resources/sfx/ken-hadouken.wav" );
    rm.kenShoryukenSound = LoadSound( "resources/sfx/ken-shoryuken.wav" );
    rm.kenTatsumakiSound = LoadSound( "resources/sfx/ken-tatsumaki.wav" );

    rm.kenTheme = LoadMusicStream( "resources/themes/ken.mp3" );

}

void unloadResourcesResourceManager( void ) {

    UnloadTexture( rm.ryuTexture );
    UnloadTexture( rm.kenTexture );

    UnloadTexture( rm.blankaStageTexture );
    UnloadTexture( rm.chunliStageTexture );
    UnloadTexture( rm.eHondaStageTexture );
    UnloadTexture( rm.guileStageTexture );
    UnloadTexture( rm.kenStageTexture );
    UnloadTexture( rm.modelStageTexture );

    UnloadTexture( rm.effectsTexture );
    UnloadTexture( rm.fontsTexture );
    UnloadTexture( rm.inputIconsTexture );

    UnloadSound( rm.ryuAttackLowSound );
    UnloadSound( rm.ryuAttackMidSound );
    UnloadSound( rm.ryuAttackHighSound );
    UnloadSound( rm.ryuHitSound );
    UnloadSound( rm.ryuHadoukenSound );
    UnloadSound( rm.ryuShoryukenSound );
    UnloadSound( rm.ryuTatsumakiSound );
    UnloadSound( rm.kenAttackLowSound );
    UnloadSound( rm.kenAttackMidSound );
    UnloadSound( rm.kenAttackHighSound );
    UnloadSound( rm.kenHitSound );
    UnloadSound( rm.kenHadoukenSound );
    UnloadSound( rm.kenShoryukenSound );
    UnloadSound( rm.kenTatsumakiSound );

    UnloadMusicStream( rm.kenTheme );

}