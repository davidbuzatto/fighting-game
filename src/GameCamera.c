/**
 * @file GameCamera.c
 * @author Prof. Dr. David Buzatto
 * @brief Game camera update implementation.
 *
 * @copyright Copyright (c) 2026
 */
#include <math.h>

#include "raylib/raylib.h"

#include "GameCamera.h"
#include "Types.h"

void updateCameraPlaying( GameWorld *gw ) {

    gw->camera.target.x = fabs( ( gw->player1->pos.x + gw->player2->pos.x ) / 2 );

    float worldWidth = gw->back01Texture->width;
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

void updateCameraEditing( GameWorld *gw ) {

    gw->camera.target.x = fabs( ( gw->player1->pos.x + gw->player2->pos.x ) / 2 );
    //gw->camera.target.x = GetScreenWidth() / 2 - 100;

    float worldWidth = gw->back01Texture->width;
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
