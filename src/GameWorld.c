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

    float scaleX = 2.5;
    float scaleY = 3.0f;
    float floorHeight = 58;

    gw->floor = (Rectangle) {
        0, GetScreenHeight() - floorHeight, GetScreenWidth(), floorHeight
    };
    gw->player = createPlayer( 70, 326, scaleX, scaleY );

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

    updatePlayer( gw->player, delta );
    resolveCollisionPlayerFloor( gw );

}

/**
 * @brief Draws the state of the game.
 */
void drawGameWorld( GameWorld *gw ) {

    BeginDrawing();
    ClearBackground( WHITE );

    // TODO: refactor
    float bgScaleX = 2.3f;
    //float bgScaleY = 2.8f;
    float bgSourceW = 1024;
    float bgSourceH = 240;

    DrawTexturePro(
        rm.stageTexture,
        (Rectangle) { 8, 296, 768, 184 },
        (Rectangle) { 
            GetScreenWidth() / 2 - ( 768 * 2.33 ) / 2, 
            0,
            768 * 2.33,
            224 * 2.3
        },
        (Vector2) { 0 },
        0.0f,
        WHITE
    );

    DrawTexturePro(
        rm.stageTexture,
        (Rectangle) { 8, 8, bgSourceW, bgSourceH },
        (Rectangle) { 
            GetScreenWidth() / 2 - ( bgSourceW * bgScaleX ) / 2, 
            -90,
            bgSourceW * bgScaleX,
            GetScreenHeight() * 1.15
        },
        (Vector2) { 0 },
        0.0f,
        WHITE
    );

    //DrawRectangleRec( gw->floor, GREEN );
    drawPlayer( gw->player );
    //DrawRectangle( 0, 0, GetScreenWidth(), 95, YELLOW );

    EndDrawing();

}

static void resolveCollisionPlayerFloor( GameWorld *gw ) {

    Player *player = gw->player;
    Rectangle *sourceRectangle = getCurrentPlayerAnimationSource( player );

    //if ( player->pos.y + sourceRectangle->height * player->scale.y > gw->floor.y ) {
    if ( player->pos.y + player->dim.y > gw->floor.y ) {
        //player->pos.y = gw->floor.y - sourceRectangle->height * player->scale.y;
        player->pos.y = gw->floor.y - player->dim.y;
        player->vel.y = 0.0f;
        if ( player->jumping ) {
            player->jumping = false;
            resetPlayerAnimations( player );
        }
    }

    /*DrawCircle( 50, player->pos.y + sourceRectangle->height * player->scale, 5, BLACK );
    DrawText( TextFormat( "%d", (int) player->vel.y ), 10, 10, 30, BLACK );*/

}