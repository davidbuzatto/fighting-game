#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "raylib/raylib.h"

#include "Macros.h"
#include "Player.h"

void drawPlayer( Player *player ) {

    Rectangle *sourceRectangle = NULL;

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
    

    if ( sourceRectangle != NULL ) {
        DrawTexturePro( 
            *player->texture,
            *sourceRectangle,
            (Rectangle) { 
                player->pos.x,
                player->pos.y,
                sourceRectangle->width * player->scale,
                sourceRectangle->height * player->scale
            },
            (Vector2) { 0 },
            0.0f,
            WHITE
        );
    }

}

void updatePlayer( Player *player, float delta ) {

    // animation resolution
    if ( player->state == PLAYER_STATE_IDLE ) {
        player->idleFrameTimeCounter += delta;
        if ( player->idleFrameTimeCounter >= player->idleFrameTime ) {
            player->idleFrameTimeCounter = 0.0f;
            player->idleCurrentFrame++;
        }
    } else if ( player->state == PLAYER_STATE_WALKING_FORWARD ) {
        player->forwardFrameTimeCounter += delta;
        if ( player->forwardFrameTimeCounter >= player->forwardFrameTime ) {
            player->forwardFrameTimeCounter = 0.0f;
            player->forwardCurrentFrame++;
        }
    } else if ( player->state == PLAYER_STATE_WALKING_BACKWARD ) {
        player->backwardFrameTimeCounter += delta;
        if ( player->backwardFrameTimeCounter >= player->backwardFrameTime ) {
            player->backwardFrameTimeCounter = 0.0f;
            player->backwardCurrentFrame++;
        }
    }

    // positioning and physics
    player->pos.x += player->vel.x * delta;
    player->pos.y += player->vel.y * delta;

    if ( IsKeyDown( KEY_LEFT ) ) {
        player->vel.x = -200;
        player->state = PLAYER_STATE_WALKING_BACKWARD;
    } else if ( IsKeyDown( KEY_RIGHT ) ) {
        player->vel.x = 200;
        player->state = PLAYER_STATE_WALKING_FORWARD;
    } else {
        player->vel.x = 0;
        player->state = PLAYER_STATE_IDLE;
    }

    if ( IsKeyPressed( KEY_UP ) && !player->jumping ) {
        player->vel.y = -500;
        player->jumping = true;
    }

    player->vel.y += GRAVITY * delta;
    
    if ( player->vel.y > 400 ) {
        player->vel.y = 400;
    }
    
}