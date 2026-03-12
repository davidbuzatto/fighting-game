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
#include "ResourceManager.h"
#include "Types.h"
#include "Player.h"

static void resolveCollisionPlayerFloor( GameWorld *gw );

/**
 * @brief Creates a dinamically allocated GameWorld struct instance.
 */
GameWorld* createGameWorld( void ) {

    GameWorld *gw = (GameWorld*) malloc( sizeof( GameWorld ) );

    float scale = 2.0f;

    float floorHeight = 50;

    gw->floor = (Rectangle) {
        0, GetScreenHeight() - floorHeight, GetScreenWidth(), floorHeight
    };

    Rectangle *floor = &gw->floor;

    gw->player = (Player) {
        .texture = &rm.playerTexture,
        .scale = scale,
        .vel = { 0 },
        .idleFrameCount = 5,
        .idleCurrentFrame = 0,
        .idleFrameTime = 0.1f,
        .idleFrameTimeCounter = 0.0f,
        .forwardFrameCount = 6,
        .forwardCurrentFrame = 0,
        .forwardFrameTime = 0.1f,
        .forwardFrameTimeCounter = 0.0f,
        .backwardFrameCount = 6,
        .backwardCurrentFrame = 0,
        .backwardFrameTime = 0.1f,
        .backwardFrameTimeCounter = 0.0f,
        .state = PLAYER_STATE_IDLE,
        .jumping = false
    };

    gw->player.idleSources[0] = (Rectangle) { 7, 14, 59, 90 };
    gw->player.idleSources[1] = (Rectangle) { 94, 15, 60, 89 };
    gw->player.idleSources[2] = (Rectangle) { 184, 14, 59, 90 };
    gw->player.idleSources[3] = (Rectangle) { 280, 11, 55, 93 };
    gw->player.idleSources[4] = (Rectangle) { 367, 12, 58, 92 };
    
    gw->player.forwardSources[0] = (Rectangle) { 9, 136, 53, 83 };
    gw->player.forwardSources[1] = (Rectangle) { 78, 131, 60, 87 };
    gw->player.forwardSources[2] = (Rectangle) { 162, 128, 64, 92 };
    gw->player.forwardSources[3] = (Rectangle) { 259, 128, 63, 90 };
    gw->player.forwardSources[4] = (Rectangle) { 352, 128, 54, 91 };
    gw->player.forwardSources[5] = (Rectangle) { 432, 131, 50, 89 };

    gw->player.backwardSources[0] = (Rectangle) { 542, 131, 61, 87 };
    gw->player.backwardSources[1] = (Rectangle) { 628, 129, 59, 90 };
    gw->player.backwardSources[2] = (Rectangle) { 713, 128, 57, 90 };
    gw->player.backwardSources[3] = (Rectangle) { 797, 127, 57, 90 };
    gw->player.backwardSources[4] = (Rectangle) { 883, 128, 58, 91 };
    gw->player.backwardSources[5] = (Rectangle) { 974, 129, 57, 89 };

    gw->player.pos = (Vector2) { 
        GetScreenWidth() / 2 - gw->player.idleSources[0].width / 2, 
        floor->y - gw->player.idleSources[0].height
    };

    return gw;

}

/**
 * @brief Destroys a GameWindow object and its dependecies.
 */
void destroyGameWorld( GameWorld *gw ) {
    free( gw );
}

/**
 * @brief Reads user input and updates the state of the game.
 */
void updateGameWorld( GameWorld *gw, float delta ) {
    updatePlayer( &gw->player, delta );
    resolveCollisionPlayerFloor( gw );
}

/**
 * @brief Draws the state of the game.
 */
void drawGameWorld( GameWorld *gw ) {

    BeginDrawing();
    ClearBackground( WHITE );

    DrawRectangleRec( gw->floor, BLUE );
    drawPlayer( &gw->player );

    EndDrawing();

}

static void resolveCollisionPlayerFloor( GameWorld *gw ) {

    Rectangle *sourceRectangle = NULL;
    Player *player = &gw->player;

    switch ( player->state ) {
        case PLAYER_STATE_IDLE:
            sourceRectangle = &player->idleSources[player->idleCurrentFrame%player->idleFrameCount];
            break;
        case PLAYER_STATE_WALKING_FORWARD:
            sourceRectangle = &player->forwardSources[player->forwardCurrentFrame%player->forwardFrameCount];
            break;
        case PLAYER_STATE_WALKING_BACKWARD:
            sourceRectangle = &player->backwardSources[player->backwardCurrentFrame%player->backwardFrameCount];
            break;
    }

    if ( player->pos.y + sourceRectangle->height * player->scale > gw->floor.y ) {
        player->pos.y = gw->floor.y - sourceRectangle->height * player->scale;
        player->vel.y = 0.0f;
        player->jumping = false;
    }

    /*DrawCircle( 50, player->pos.y + sourceRectangle->height * player->scale, 5, BLACK );
    DrawText( TextFormat( "%d", (int) player->vel.y ), 10, 10, 30, BLACK );*/

}