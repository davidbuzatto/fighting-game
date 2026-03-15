/**
 * @file GameWorld.h
 * @author Prof. Dr. David Buzatto
 * @brief GameWorld implementation.
 * 
 * @copyright Copyright (c) 2026
 */
#include <stdio.h>
#include <stdlib.h>

#include "raylib/raylib.h"
//#include "raylib/raymath.h"
//#define RAYGUI_IMPLEMENTATION    // to use raygui, comment these three lines.
//#include "raylib/raygui.h"       // other compilation units must only include
//#undef RAYGUI_IMPLEMENTATION     // raygui.h

#include "GameWorld.h"
#include "Macros.h"
#include "ResourceManager.h"
#include "Types.h"
#include "Player.h"

static void resolveCollisionPlayerStage( GameWorld *gw );
static void updateCamera( GameWorld *gw );

/**
 * @brief Creates a dinamically allocated GameWorld struct instance.
 */
GameWorld* createGameWorld( void ) {

    GameWorld *gw = (GameWorld*) malloc( sizeof( GameWorld ) );

    float floorHeight = 30;

    gw->player = createPlayer( rm.stageTexture.width / 2 - 50, 326 );
    gw->floor = (Rectangle) {
        0, GetScreenHeight() - floorHeight, GetScreenWidth(), floorHeight
    };
    gw->gravity = 1500;

    gw->camera = (Camera2D) {
        .offset = { GetScreenWidth() / 2, 0 },
        .rotation = 0.0f,
        .target = {
            gw->player->pos.x,
            425
        },
        .zoom = 2.75f
    };

    return gw;

}

/**
 * @brief Destroys a GameWindow object and its dependecies.
 */
void destroyGameWorld( GameWorld *gw ) {
    destroyPlayer( gw->player );
    free( gw );
}

/**
 * @brief Reads user input and updates the state of the game.
 */
void updateGameWorld( GameWorld *gw, float delta ) {

    if ( IsKeyPressed( KEY_F ) ) {
        flipPlayerSide( gw->player );
    }

    updatePlayer( gw->player, gw->gravity, delta );
    resolveCollisionPlayerStage( gw );


    updateCamera( gw );

}

/**
 * @brief Draws the state of the game.
 */
void drawGameWorld( GameWorld *gw ) {

    BeginDrawing();
    ClearBackground( WHITE );
    
    BeginMode2D( gw->camera );

    DrawTexture( rm.stageTexture, 0, GetScreenHeight() - rm.stageTexture.height, WHITE );
    drawPlayer( gw->player );

    EndMode2D();
    EndDrawing();

}

static void resolveCollisionPlayerStage( GameWorld *gw ) {

    Player *player = gw->player;

    if ( player->pos.x - player->dim.x / 2 < 0 ) {
        player->pos.x = player->dim.x / 2;
    } else if ( player->pos.x + player->dim.x / 2 > rm.stageTexture.width ) {
        player->pos.x = rm.stageTexture.width - player->dim.x / 2;
    }

    if ( player->pos.y + player->dim.y > gw->floor.y ) {
        player->pos.y = gw->floor.y - player->dim.y;
        player->vel.y = 0.0f;
        if ( player->jumping ) {
            player->jumping = false;
            resetPlayerAnimations( player );
        }
    }

}

static void updateCamera( GameWorld *gw ) {

    gw->camera.target.x = gw->player->pos.x;

    float worldWidth = rm.stageTexture.width;
    float zoom = gw->camera.zoom;
    float offsetX = gw->camera.offset.x;
    float screenWidth = GetScreenWidth();

    float minTargetX = offsetX / zoom;
    float maxTargetX = worldWidth - ( screenWidth - offsetX ) / zoom;

    if ( gw->camera.target.x < minTargetX ) {
        gw->camera.target.x = minTargetX;
    } else if ( gw->camera.target.x > maxTargetX ) {
        gw->camera.target.x = maxTargetX;
    }

}