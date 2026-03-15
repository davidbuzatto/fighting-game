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

static void resolveCollisionPlayerStage( Player *player, GameWorld *gw );
static void updateCamera( GameWorld *gw );

/**
 * @brief Creates a dinamically allocated GameWorld struct instance.
 */
GameWorld* createGameWorld( void ) {

    GameWorld *gw = (GameWorld*) malloc( sizeof( GameWorld ) );

    float floorHeight = 30;

    gw->floor = (Rectangle) {
        0, GetScreenHeight() - floorHeight, GetScreenWidth(), floorHeight
    };
    gw->stageTexture = &rm.kenStageTexture;

    gw->gravity = 1500;

    gw->camera = (Camera2D) {
        .offset = { GetScreenWidth() / 2, 0 },
        .rotation = 0.0f,
        .target = {
            0,
            425
        },
        .zoom = 2.75f
    };

    Player *player1 = createPlayer();
    Player *player2 = createPlayer();

    initializePlayerRyu( gw->stageTexture->width / 2 - 78, 326, player1 );
    initializePlayerKen( gw->stageTexture->width / 2 + 50, 326, player2 );
    flipPlayerSide( player2 );

    player1->kb = (PlayerKeyBindings) {
        .left = KEY_LEFT,
        .right = KEY_RIGHT,
        .up = KEY_UP,
        .down = KEY_DOWN,
    };

    player2->kb = (PlayerKeyBindings) {
        .left = KEY_A,
        .right = KEY_D,
        .up = KEY_W,
        .down = KEY_S,
    };

    gw->player1 = player1;
    gw->player2 = player2;

    return gw;

}

/**
 * @brief Destroys a GameWindow object and its dependecies.
 */
void destroyGameWorld( GameWorld *gw ) {
    destroyPlayer( gw->player1 );
    destroyPlayer( gw->player2 );
    free( gw );
}

/**
 * @brief Reads user input and updates the state of the game.
 */
void updateGameWorld( GameWorld *gw, float delta ) {

    if ( IsKeyPressed( KEY_ONE ) ) {
        gw->stageTexture = &rm.kenStageTexture;
    }
    if ( IsKeyPressed( KEY_TWO ) ) {
        gw->stageTexture = &rm.blankaStageTexture;
    }
    if ( IsKeyPressed( KEY_THREE ) ) {
        gw->stageTexture = &rm.chunliStageTexture;
    }
    if ( IsKeyPressed( KEY_FOUR ) ) {
        gw->stageTexture = &rm.eHondaStageTexture;
    }
    if ( IsKeyPressed( KEY_FIVE ) ) {
        gw->stageTexture = &rm.guileStageTexture;
    }

    if ( IsKeyPressed( KEY_F ) ) {
        flipPlayerSide( gw->player1 );
    }

    updatePlayer( gw->player1, gw->gravity, delta );
    updatePlayer( gw->player2, gw->gravity, delta );

    resolveCollisionPlayerStage( gw->player1, gw );
    resolveCollisionPlayerStage( gw->player2, gw );

    updateCamera( gw );

}

/**
 * @brief Draws the state of the game.
 */
void drawGameWorld( GameWorld *gw ) {

    BeginDrawing();
    ClearBackground( WHITE );
    
    BeginMode2D( gw->camera );

    DrawTexture( *gw->stageTexture, 0, GetScreenHeight() - gw->stageTexture->height, WHITE );
    drawPlayer( gw->player1 );
    drawPlayer( gw->player2 );

    EndMode2D();
    EndDrawing();

}

static void resolveCollisionPlayerStage( Player *player, GameWorld *gw ) {

    if ( player->pos.x - player->dim.x / 2 < 0 ) {
        player->pos.x = player->dim.x / 2;
    } else if ( player->pos.x + player->dim.x / 2 > gw->stageTexture->width ) {
        player->pos.x = gw->stageTexture->width - player->dim.x / 2;
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

    gw->camera.target.x = gw->player1->pos.x;

    float worldWidth = gw->stageTexture->width;
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