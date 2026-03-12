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
    gw->player = createPlayer( 0, 0, scale );

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
    updatePlayer( gw->player, delta );
    resolveCollisionPlayerFloor( gw );
}

/**
 * @brief Draws the state of the game.
 */
void drawGameWorld( GameWorld *gw ) {

    BeginDrawing();
    ClearBackground( WHITE );

    DrawRectangleRec( gw->floor, BLUE );
    drawPlayer( gw->player );

    EndDrawing();

}

static void resolveCollisionPlayerFloor( GameWorld *gw ) {

    Player *player = gw->player;
    Rectangle *sourceRectangle = getCurrentPlayerAnimationSource( player );

    if ( player->pos.y + sourceRectangle->height * player->scale > gw->floor.y ) {
        player->pos.y = gw->floor.y - sourceRectangle->height * player->scale;
        player->vel.y = 0.0f;
        player->jumping = false;
    }

    /*DrawCircle( 50, player->pos.y + sourceRectangle->height * player->scale, 5, BLACK );
    DrawText( TextFormat( "%d", (int) player->vel.y ), 10, 10, 30, BLACK );*/

}