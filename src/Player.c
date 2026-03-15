#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

#include "raylib/raylib.h"

#include "Macros.h"
#include "Player.h"
#include "ResourceManager.h"

Player *createPlayer( float x, float y ) {

    Player *p = (Player*) malloc( sizeof( Player ) );

    p->pos.x = x;
    p->pos.y = y;
    p->dim.x = 80;
    p->dim.y = 100;
    p->texture = &rm.playerTexture;
    p->vel = (Vector2) { 0 };
    p->forwardSpeed = 150;
    p->backwardSpeed = 120;
    p->jumpSpeed = 350;
    p->state = PLAYER_STATE_IDLE;
    p->lastState = PLAYER_STATE_IDLE;
    p->jumping = false;
    p->lookingRight = true;

    //p->idleAnim.frameCount = 5;
    p->idleAnim.frameCount = 6;
    p->idleAnim.currentFrame = 0;
    p->idleAnim.frameTime = 0.065f;
    p->idleAnim.frameTimeCounter = 0.0f;
    p->idleAnim.stopAtLastFrame = false;
    createPlayerAnimationSources( &p->idleAnim, p->idleAnim.frameCount );
    p->idleAnim.sources[0] = (Rectangle) {   1, 142, -64, 96 };
    p->idleAnim.sources[1] = (Rectangle) {  66, 142, -64, 96 };
    p->idleAnim.sources[2] = (Rectangle) { 131, 142, -64, 96 };
    p->idleAnim.sources[3] = (Rectangle) { 196, 142, -64, 96 };
    p->idleAnim.sources[4] = (Rectangle) { 131, 142, -64, 96 };
    p->idleAnim.sources[5] = (Rectangle) {  66, 142, -64, 96 };

    //p->forwardAnim.frameCount = 6;
    p->forwardAnim.frameCount = 5;
    p->forwardAnim.currentFrame = 0;
    p->forwardAnim.frameTime = 0.07f;
    p->forwardAnim.frameTimeCounter = 0.0f;
    p->forwardAnim.stopAtLastFrame = false;
    createPlayerAnimationSources( &p->forwardAnim, p->forwardAnim.frameCount );
    p->forwardAnim.sources[0] = (Rectangle) {   1, 514, -80, 96 };
    p->forwardAnim.sources[1] = (Rectangle) {  82, 514, -80, 96 };
    p->forwardAnim.sources[2] = (Rectangle) { 163, 514, -80, 96 };
    p->forwardAnim.sources[3] = (Rectangle) { 244, 514, -80, 96 };
    p->forwardAnim.sources[4] = (Rectangle) { 325, 514, -80, 96 };

    p->backwardAnim.frameCount = 6;
    p->backwardAnim.currentFrame = 0;
    p->backwardAnim.frameTime = 0.08f;
    p->backwardAnim.frameTimeCounter = 0.0f;
    p->backwardAnim.stopAtLastFrame = false;
    createPlayerAnimationSources( &p->backwardAnim, p->backwardAnim.frameCount );
    p->backwardAnim.sources[0] = (Rectangle) {   1, 611, -80, 96 };
    p->backwardAnim.sources[1] = (Rectangle) {  82, 611, -80, 96 };
    p->backwardAnim.sources[2] = (Rectangle) { 163, 611, -80, 96 };
    p->backwardAnim.sources[3] = (Rectangle) { 244, 611, -80, 96 };
    p->backwardAnim.sources[4] = (Rectangle) { 325, 611, -80, 96 };
    p->backwardAnim.sources[5] = (Rectangle) { 406, 611, -80, 96 };

    p->straightJumpAnim.frameCount = 6;
    p->straightJumpAnim.currentFrame = 0;
    p->straightJumpAnim.frameTime = 0.07f;
    p->straightJumpAnim.frameTimeCounter = 0.0f;
    p->straightJumpAnim.stopAtLastFrame = true;
    createPlayerAnimationSources( &p->straightJumpAnim, p->straightJumpAnim.frameCount );
    p->straightJumpAnim.sources[0] = (Rectangle) {   1, 849, -64, 140 };
    p->straightJumpAnim.sources[1] = (Rectangle) {  66, 849, -64, 140 };
    p->straightJumpAnim.sources[2] = (Rectangle) { 131, 849, -64, 140 };
    p->straightJumpAnim.sources[3] = (Rectangle) { 196, 849, -64, 140 };
    p->straightJumpAnim.sources[4] = (Rectangle) { 261, 849, -64, 140 };
    p->straightJumpAnim.sources[5] = (Rectangle) { 326, 849, -64, 140 };

    p->forwardJumpAnim.frameCount = 7;
    p->forwardJumpAnim.currentFrame = 0;
    p->forwardJumpAnim.frameTime = 0.05f;
    p->forwardJumpAnim.frameTimeCounter = 0.0f;
    p->forwardJumpAnim.stopAtLastFrame = true;
    createPlayerAnimationSources( &p->forwardJumpAnim, p->forwardJumpAnim.frameCount );
    p->forwardJumpAnim.sources[0] = (Rectangle) {   1, 708, -128, 140 };
    p->forwardJumpAnim.sources[1] = (Rectangle) { 130, 708, -128, 140 };
    p->forwardJumpAnim.sources[2] = (Rectangle) { 259, 708, -128, 140 };
    p->forwardJumpAnim.sources[3] = (Rectangle) { 388, 708, -128, 140 };
    p->forwardJumpAnim.sources[4] = (Rectangle) { 517, 708, -128, 140 };
    p->forwardJumpAnim.sources[5] = (Rectangle) { 646, 708, -128, 140 };
    p->forwardJumpAnim.sources[6] = (Rectangle) {   1, 708, -128, 140 };

    p->backwardJumpAnim.frameCount = 7;
    p->backwardJumpAnim.currentFrame = 0;
    p->backwardJumpAnim.frameTime = 0.05f;
    p->backwardJumpAnim.frameTimeCounter = 0.0f;
    p->backwardJumpAnim.stopAtLastFrame = true;
    createPlayerAnimationSources( &p->backwardJumpAnim, p->backwardJumpAnim.frameCount );
    p->backwardJumpAnim.sources[0] = (Rectangle) {   1, 708, -128, 140 };
    p->backwardJumpAnim.sources[1] = (Rectangle) { 646, 708, -128, 140 };
    p->backwardJumpAnim.sources[2] = (Rectangle) { 517, 708, -128, 140 };
    p->backwardJumpAnim.sources[3] = (Rectangle) { 388, 708, -128, 140 };
    p->backwardJumpAnim.sources[4] = (Rectangle) { 259, 708, -128, 140 };
    p->backwardJumpAnim.sources[5] = (Rectangle) { 130, 708, -128, 140 };
    p->backwardJumpAnim.sources[6] = (Rectangle) {   1, 708, -128, 140 };

    p->animations[0] = &p->idleAnim;
    p->animations[1] = &p->forwardAnim;
    p->animations[2] = &p->backwardAnim;
    p->animations[3] = &p->straightJumpAnim;
    p->animations[4] = &p->forwardJumpAnim;
    p->animations[5] = &p->backwardJumpAnim;
    p->animationCount = 6;

    return p;

}

void destroyPlayer( Player *player ) {
    for ( int i = 0; i < player->animationCount; i++ ) {
        destroyPlayerAnimationSources( player->animations[i] );
    }
    free( player );
}

void drawPlayer( Player *player ) {

    Rectangle *sourceRectangle = getCurrentPlayerAnimationSource( player );

    if ( sourceRectangle != NULL ) {
        DrawTexturePro( 
            *player->texture,
            *sourceRectangle,
            (Rectangle) { 
                player->pos.x - fabs( sourceRectangle->width ) / 2,
                player->pos.y + player->dim.y - sourceRectangle->height,
                sourceRectangle->width,
                sourceRectangle->height
            },
            (Vector2) { 0 },
            0.0f,
            WHITE
        );
    }

    //DrawCircle( player->pos.x, player->pos.y, 2, BLUE );
    //DrawRectangleLines( player->pos.x - player->dim.x / 2, player->pos.y, player->dim.x, player->dim.y, BLUE );

}

void updatePlayer( Player *player, float gravity, float delta ) {

    // state changes
    if ( !player->jumping ) {
        if ( IsKeyDown( KEY_RIGHT ) ) {
            player->vel.x = player->forwardSpeed;
            player->state = PLAYER_STATE_WALKING_FORWARD;
        } else if ( IsKeyDown( KEY_LEFT ) ) {
            player->vel.x = -player->backwardSpeed;
            player->state = PLAYER_STATE_WALKING_BACKWARD;
        } else {
            player->vel.x = 0.0f;
            player->state = PLAYER_STATE_IDLE;
        }
    }

    if ( IsKeyDown( KEY_UP ) && !player->jumping ) {
        if ( player->vel.x == 0.0f ) {
            player->vel.y = -player->jumpSpeed;
            player->jumping = true;
        } else if ( player->vel.x > 0 ) {
            //trace( "jumping right" );
            player->vel.y = -player->jumpSpeed;
            player->vel.x = player->forwardSpeed * 1.6f;
            player->jumping = true;
        } else {
            //trace( "jumping left" );
            player->vel.y = -player->jumpSpeed;
            player->vel.x = -player->backwardSpeed * 2.0f;
            player->jumping = true;
        }
    }

    if ( player->state != player->lastState ) {
        resetPlayerAnimations( player );
    }

    // animation resolution
    if ( player->jumping ) {
        if ( player->vel.x == 0.0f ) {
            updatePlayerAnimation( &player->straightJumpAnim, delta );
        } else if ( player->vel.x > 0.0f ) {
            updatePlayerAnimation( &player->forwardJumpAnim, delta );
        } else {
            updatePlayerAnimation( &player->backwardJumpAnim, delta );
        }
    } else {
        if ( player->state == PLAYER_STATE_IDLE ) {
            updatePlayerAnimation( &player->idleAnim, delta );
        } else if ( player->state == PLAYER_STATE_WALKING_FORWARD ) {
            updatePlayerAnimation( &player->forwardAnim, delta );
        } else if ( player->state == PLAYER_STATE_WALKING_BACKWARD ) {
            updatePlayerAnimation( &player->backwardAnim, delta );
        }
    }

    // positioning and physics
    player->pos.x += player->vel.x * delta;
    player->pos.y += player->vel.y * delta;

    player->vel.y += gravity * delta;
    
    if ( player->vel.y > gravity ) {
        player->vel.y = gravity;
    }

    player->lastState = player->state;
    
}

void flipPlayerSide( Player *player ) {
    for ( int i = 0; i < player->animationCount; i++ ) {
        Animation *a = player->animations[i];
        for ( int j = 0; j < a->frameCount; j++ ) {
            a->sources[j].width = -a->sources[j].width;
        }
    }
    player->lookingRight = !player->lookingRight;
}

void createPlayerAnimationSources( Animation *pa, int frameCount ) {
    pa->sources = (Rectangle*) malloc( sizeof( Rectangle ) * frameCount );
}

void destroyPlayerAnimationSources( Animation *pa ) {
    free( pa->sources );
}

void updatePlayerAnimation( Animation *pa, float delta ) {

    if ( pa->stopAtLastFrame && pa->currentFrame % pa->frameCount == pa->frameCount - 1 ) {
        return;
    }

    pa->frameTimeCounter += delta;

    if ( pa->frameTimeCounter >= pa->frameTime ) {
        pa->frameTimeCounter = 0.0f;
        pa->currentFrame++;
    }

}

int getPlayerAnimationFrame( Animation *pa ) {
    return pa->currentFrame % pa->frameCount;
}

int getCurrentPlayerAnimationFrame( Player *player ) {

    if ( player->jumping ) {
        if ( player->vel.x == 0.0f ) {
            return getPlayerAnimationFrame( &player->straightJumpAnim );
        } else if ( player->vel.x > 0.0f ) {
            return getPlayerAnimationFrame( &player->forwardJumpAnim );
        } else {
            return getPlayerAnimationFrame( &player->backwardJumpAnim );
        }
    }

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

Rectangle *getPlayerAnimationSource( Animation *pa ) {
    return &pa->sources[getPlayerAnimationFrame(pa)];
}

Rectangle *getCurrentPlayerAnimationSource( Player *player ) {

    if ( player->jumping ) {
        if ( player->vel.x == 0.0f ) {
            return getPlayerAnimationSource( &player->straightJumpAnim );
        } else if ( player->vel.x > 0.0f ) {
            return getPlayerAnimationSource( &player->forwardJumpAnim );
        } else {
            return getPlayerAnimationSource( &player->backwardJumpAnim );
        }
    }

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
    for ( int i = 0; i < player->animationCount; i++ ) {
        Animation *a = player->animations[i];
        a->currentFrame = 0;
        a->frameTimeCounter = 0.0f;
        a->currentFrame = 0;
    }
}
