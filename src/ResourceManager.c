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
#include "Utils.h"
#include "raylib/raylib.h"

ResourceManager rm = { 0 };

void loadResourcesResourceManager( void ) {

    rm.ryuSpriteMapTexture = loadTextureReplacingColor( 
        "resources/images/chars/ryuSpriteMap.png", 
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

    rm.ryuSpecialMovesSpriteMapTexture = loadTextureReplacingColor( 
        "resources/images/chars/ryuSpecialMoves.png", 
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

    rm.ryuPalleteImage = LoadImage( "resources/images/chars/ryuPallete.png" );

    rm.kenSpriteMapTexture = loadTextureReplacingColor( 
        "resources/images/chars/kenSpriteMap.png", 
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

    rm.kenSpecialMovesSpriteMapTexture = loadTextureReplacingColor( 
        "resources/images/chars/kenSpecialMoves.png", 
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

    rm.kenPalleteImage = LoadImage( "resources/images/chars/kenPallete.png" );

    rm.kenStageAnchorTexture = LoadTexture( "resources/images/stages/ken/anchor.png" );
    rm.kenStageFloorTexture = LoadTexture( "resources/images/stages/ken/floor.png" );
    rm.kenStageBack01Texture = LoadTexture( "resources/images/stages/ken/back01.png" );
    rm.kenStageBack02Texture = LoadTexture( "resources/images/stages/ken/back02.png" );
    rm.kenStageBack03Texture = LoadTexture( "resources/images/stages/ken/back03.png" );
    rm.kenStageBoat01Texture = LoadTexture( "resources/images/stages/ken/boat01.png" );
    rm.kenStageBoat02Texture = LoadTexture( "resources/images/stages/ken/boat02.png" );
    rm.kenStageBoat03Texture = LoadTexture( "resources/images/stages/ken/boat03.png" );
    rm.kenStageBoat04Texture = LoadTexture( "resources/images/stages/ken/boat04.png" );
    rm.kenStageBoat05Texture = LoadTexture( "resources/images/stages/ken/boat05.png" );
    rm.modelStageTexture = LoadTexture( "resources/images/stages/model-stage-photo.png" );

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
    rm.ryuHitSound = LoadSound( "resources/sfx/hit-04.wav" );
    rm.ryuHadoukenSound = LoadSound( "resources/sfx/ryu-hadouken.wav" );
    rm.ryuShoryukenSound = LoadSound( "resources/sfx/ryu-shoryuken.wav" );
    rm.ryuTatsumakiSound = LoadSound( "resources/sfx/ryu-tatsumaki.wav" );

    rm.kenAttackLowSound = LoadSound( "resources/sfx/attack-low.wav" );
    rm.kenAttackMidSound = LoadSound( "resources/sfx/attack-mid.wav" );
    rm.kenAttackHighSound = LoadSound( "resources/sfx/attack-high.wav" );
    rm.kenHitSound = LoadSound( "resources/sfx/hit-04.wav" );
    rm.kenHadoukenSound = LoadSound( "resources/sfx/ken-hadouken.wav" );
    rm.kenShoryukenSound = LoadSound( "resources/sfx/ken-shoryuken.wav" );
    rm.kenTatsumakiSound = LoadSound( "resources/sfx/ken-tatsumaki.wav" );

    rm.kenTheme = LoadMusicStream( "resources/themes/ken.mp3" );

}

void unloadResourcesResourceManager( void ) {

    UnloadTexture( rm.ryuSpriteMapTexture );
    UnloadTexture( rm.ryuSpecialMovesSpriteMapTexture );
    UnloadImage( rm.ryuPalleteImage );

    UnloadTexture( rm.kenSpriteMapTexture );
    UnloadTexture( rm.kenSpecialMovesSpriteMapTexture );
    UnloadImage( rm.kenPalleteImage );

    UnloadTexture( rm.kenStageAnchorTexture );
    UnloadTexture( rm.kenStageFloorTexture );
    UnloadTexture( rm.kenStageBack01Texture );
    UnloadTexture( rm.kenStageBack02Texture );
    UnloadTexture( rm.kenStageBack03Texture );
    UnloadTexture( rm.kenStageBoat01Texture );
    UnloadTexture( rm.kenStageBoat02Texture );
    UnloadTexture( rm.kenStageBoat03Texture );
    UnloadTexture( rm.kenStageBoat04Texture );
    UnloadTexture( rm.kenStageBoat05Texture );
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