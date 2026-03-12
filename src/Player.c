#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "raylib/raylib.h"

#include "Macros.h"
#include "Player.h"
#include "ResourceManager.h"

Player *createPlayer( float x, float y, float scale ) {

    Player *p = (Player*) malloc( sizeof( Player ) );

    p->pos.x = x;
    p->pos.y = y;
    p->texture = &rm.playerTexture;
    p->scale = scale;
    p->vel = (Vector2) { 0 };
    p->state = PLAYER_STATE_IDLE;
    p->lastState = PLAYER_STATE_IDLE;
    p->jumping = false;

    p->idleAnim.frameCount = 5;
    p->idleAnim.currentFrame = 0;
    p->idleAnim.frameTime = 0.1f;
    p->idleAnim.frameTimeCounter = 0.0f;
    createPlayerAnimationSources( &p->idleAnim, p->idleAnim.frameCount );
    p->idleAnim.sources[0] = (Rectangle) { 7, 14, 59, 90 };
    p->idleAnim.sources[1] = (Rectangle) { 94, 15, 60, 89 };
    p->idleAnim.sources[2] = (Rectangle) { 184, 14, 59, 90 };
    p->idleAnim.sources[3] = (Rectangle) { 280, 11, 55, 93 };
    p->idleAnim.sources[4] = (Rectangle) { 367, 12, 58, 92 };

    p->forwardAnim.frameCount = 6;
    p->forwardAnim.currentFrame = 0;
    p->forwardAnim.frameTime = 0.1f;
    p->forwardAnim.frameTimeCounter = 0.0f;
    createPlayerAnimationSources( &p->forwardAnim, p->forwardAnim.frameCount );
    p->forwardAnim.sources[0] = (Rectangle) { 9, 136, 53, 83 };
    p->forwardAnim.sources[1] = (Rectangle) { 78, 131, 60, 87 };
    p->forwardAnim.sources[2] = (Rectangle) { 162, 128, 64, 92 };
    p->forwardAnim.sources[3] = (Rectangle) { 259, 128, 63, 90 };
    p->forwardAnim.sources[4] = (Rectangle) { 352, 128, 54, 91 };
    p->forwardAnim.sources[5] = (Rectangle) { 432, 131, 50, 89 };

    p->backwardAnim.frameCount = 6;
    p->backwardAnim.currentFrame = 0;
    p->backwardAnim.frameTime = 0.1f;
    p->backwardAnim.frameTimeCounter = 0.0f;
    createPlayerAnimationSources( &p->backwardAnim, p->backwardAnim.frameCount );
    p->backwardAnim.sources[0] = (Rectangle) { 542, 131, 61, 87 };
    p->backwardAnim.sources[1] = (Rectangle) { 628, 129, 59, 90 };
    p->backwardAnim.sources[2] = (Rectangle) { 713, 128, 57, 90 };
    p->backwardAnim.sources[3] = (Rectangle) { 797, 127, 57, 90 };
    p->backwardAnim.sources[4] = (Rectangle) { 883, 128, 58, 91 };
    p->backwardAnim.sources[5] = (Rectangle) { 974, 129, 57, 89 };

    return p;

}

void destroyPlayer( Player *player ) {
    destroyPlayerAnimationSources( &player->idleAnim );
    destroyPlayerAnimationSources( &player->forwardAnim );
    destroyPlayerAnimationSources( &player->backwardAnim );
    free( player );
}

void drawPlayer( Player *player ) {

    Rectangle *sourceRectangle = getCurrentPlayerAnimationSource( player );

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

    // state changes
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

    if ( player->state != player->lastState ) {
        resetPlayerAnimations( player );
    }

    // animation resolution
    if ( player->state == PLAYER_STATE_IDLE ) {
        updatePlayerAnimation( &player->idleAnim, delta );
    } else if ( player->state == PLAYER_STATE_WALKING_FORWARD ) {
        updatePlayerAnimation( &player->forwardAnim, delta );
    } else if ( player->state == PLAYER_STATE_WALKING_BACKWARD ) {
        updatePlayerAnimation( &player->backwardAnim, delta );
    }

    // positioning and physics
    player->pos.x += player->vel.x * delta;
    player->pos.y += player->vel.y * delta;

    player->vel.y += GRAVITY * delta;
    
    if ( player->vel.y > 400 ) {
        player->vel.y = 400;
    }

    player->lastState = player->state;
    
}

void createPlayerAnimationSources( PlayerAnimation *pa, int frameCount ) {
    pa->sources = (Rectangle*) malloc( sizeof( Rectangle ) * frameCount );
}

void destroyPlayerAnimationSources( PlayerAnimation *pa ) {
    free( pa->sources );
}

void updatePlayerAnimation( PlayerAnimation *pa, float delta ) {
    pa->frameTimeCounter += delta;
    if ( pa->frameTimeCounter >= pa->frameTime ) {
        pa->frameTimeCounter = 0.0f;
        pa->currentFrame++;
    }
}

int getPlayerAnimationFrame( PlayerAnimation *pa ) {
    return pa->currentFrame % pa->frameCount;
}

int getCurrentPlayerAnimationFrame( Player *player ) {
    switch ( player->state ) {
        case PLAYER_STATE_IDLE:
            return getPlayerAnimationFrame( &player->idleAnim );
        case PLAYER_STATE_WALKING_FORWARD:
            return getPlayerAnimationFrame( &player->forwardAnim );
        case PLAYER_STATE_WALKING_BACKWARD:
            return getPlayerAnimationFrame( &player->backwardAnim );
    }
    return -1;
}

Rectangle *getPlayerAnimationSource( PlayerAnimation *pa ) {
    return &pa->sources[getPlayerAnimationFrame(pa)];
}

Rectangle *getCurrentPlayerAnimationSource( Player *player ) {
    switch ( player->state ) {
        case PLAYER_STATE_IDLE:
            return getPlayerAnimationSource( &player->idleAnim );
        case PLAYER_STATE_WALKING_FORWARD:
            return getPlayerAnimationSource( &player->forwardAnim );
        case PLAYER_STATE_WALKING_BACKWARD:
            return getPlayerAnimationSource( &player->backwardAnim );
    }
    return NULL;
}

void resetPlayerAnimations( Player *player ) {
    player->idleAnim.currentFrame = 0;
    player->idleAnim.frameTimeCounter = 0.0f;
    player->forwardAnim.currentFrame = 0;
    player->forwardAnim.frameTimeCounter = 0.0f;
    player->backwardAnim.currentFrame = 0;
    player->backwardAnim.frameTimeCounter = 0.0f;
}