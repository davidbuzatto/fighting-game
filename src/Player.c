#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

#include "raylib/raylib.h"

#include "Animation.h"
#include "Macros.h"
#include "Player.h"
#include "ResourceManager.h"
#include "Types.h"

Player *createPlayer() {
    Player *p = (Player*) malloc( sizeof( Player ) );
    return p;
}

void initializePlayerRyu( float x, float y, Player *p ) {

    p->pos.x = x;
    p->pos.y = y;
    p->dim.x = 80;
    p->dim.y = 100;
    p->texture = &rm.ryuTexture;
    p->vel = (Vector2) { 0 };
    p->forwardSpeed = 150;
    p->backwardSpeed = 120;
    p->jumpSpeed = 350;
    p->state = PLAYER_STATE_IDLE;
    p->lastState = PLAYER_STATE_IDLE;
    p->jumping = false;
    p->lookingRight = true;

    p->idleAnim.frameCount = 6;
    p->idleAnim.currentFrame = 0;
    p->idleAnim.frameTimeCounter = 0.0f;
    p->idleAnim.stopAtLastFrame = false;
    createAnimationFrames( &p->idleAnim, p->idleAnim.frameCount );
    p->idleAnim.frames[0] = (AnimationFrame) { (Rectangle) {   1, 142, -64, 96 }, 65 };
    p->idleAnim.frames[1] = (AnimationFrame) { (Rectangle) {  66, 142, -64, 96 }, 65 };
    p->idleAnim.frames[2] = (AnimationFrame) { (Rectangle) { 131, 142, -64, 96 }, 65 };
    p->idleAnim.frames[3] = (AnimationFrame) { (Rectangle) { 196, 142, -64, 96 }, 65 };
    p->idleAnim.frames[4] = (AnimationFrame) { (Rectangle) { 131, 142, -64, 96 }, 65 };
    p->idleAnim.frames[5] = (AnimationFrame) { (Rectangle) {  66, 142, -64, 96 }, 65 };

    p->forwardAnim.frameCount = 5;
    p->forwardAnim.currentFrame = 0;
    p->forwardAnim.frameTimeCounter = 0.0f;
    p->forwardAnim.stopAtLastFrame = false;
    createAnimationFrames( &p->forwardAnim, p->forwardAnim.frameCount );
    p->forwardAnim.frames[0] = (AnimationFrame) { (Rectangle) {   1, 514, -80, 96 }, 70 };
    p->forwardAnim.frames[1] = (AnimationFrame) { (Rectangle) {  82, 514, -80, 96 }, 70 };
    p->forwardAnim.frames[2] = (AnimationFrame) { (Rectangle) { 163, 514, -80, 96 }, 70 };
    p->forwardAnim.frames[3] = (AnimationFrame) { (Rectangle) { 244, 514, -80, 96 }, 70 };
    p->forwardAnim.frames[4] = (AnimationFrame) { (Rectangle) { 325, 514, -80, 96 }, 70 };

    p->backwardAnim.frameCount = 6;
    p->backwardAnim.currentFrame = 0;
    p->backwardAnim.frameTimeCounter = 0.0f;
    p->backwardAnim.stopAtLastFrame = false;
    createAnimationFrames( &p->backwardAnim, p->backwardAnim.frameCount );
    p->backwardAnim.frames[0] = (AnimationFrame) { (Rectangle) {   1, 611, -80, 96 }, 80 };
    p->backwardAnim.frames[1] = (AnimationFrame) { (Rectangle) {  82, 611, -80, 96 }, 80 };
    p->backwardAnim.frames[2] = (AnimationFrame) { (Rectangle) { 163, 611, -80, 96 }, 80 };
    p->backwardAnim.frames[3] = (AnimationFrame) { (Rectangle) { 244, 611, -80, 96 }, 80 };
    p->backwardAnim.frames[4] = (AnimationFrame) { (Rectangle) { 325, 611, -80, 96 }, 80 };
    p->backwardAnim.frames[5] = (AnimationFrame) { (Rectangle) { 406, 611, -80, 96 }, 80 };

    p->straightJumpAnim.frameCount = 6;
    p->straightJumpAnim.currentFrame = 0;
    p->straightJumpAnim.frameTimeCounter = 0.0f;
    p->straightJumpAnim.stopAtLastFrame = true;
    createAnimationFrames( &p->straightJumpAnim, p->straightJumpAnim.frameCount );
    p->straightJumpAnim.frames[0] = (AnimationFrame) { (Rectangle) {   1, 849, -64, 140 }, 70 };
    p->straightJumpAnim.frames[1] = (AnimationFrame) { (Rectangle) {  66, 849, -64, 140 }, 70 };
    p->straightJumpAnim.frames[2] = (AnimationFrame) { (Rectangle) { 131, 849, -64, 140 }, 70 };
    p->straightJumpAnim.frames[3] = (AnimationFrame) { (Rectangle) { 196, 849, -64, 140 }, 70 };
    p->straightJumpAnim.frames[4] = (AnimationFrame) { (Rectangle) { 261, 849, -64, 140 }, 70 };
    p->straightJumpAnim.frames[5] = (AnimationFrame) { (Rectangle) { 326, 849, -64, 140 }, 70 };

    p->forwardJumpAnim.frameCount = 7;
    p->forwardJumpAnim.currentFrame = 0;
    p->forwardJumpAnim.frameTimeCounter = 0.0f;
    p->forwardJumpAnim.stopAtLastFrame = true;
    createAnimationFrames( &p->forwardJumpAnim, p->forwardJumpAnim.frameCount );
    p->forwardJumpAnim.frames[0] = (AnimationFrame) { (Rectangle) {   1, 708, -128, 140 }, 50 };
    p->forwardJumpAnim.frames[1] = (AnimationFrame) { (Rectangle) { 130, 708, -128, 140 }, 50 };
    p->forwardJumpAnim.frames[2] = (AnimationFrame) { (Rectangle) { 259, 708, -128, 140 }, 50 };
    p->forwardJumpAnim.frames[3] = (AnimationFrame) { (Rectangle) { 388, 708, -128, 140 }, 50 };
    p->forwardJumpAnim.frames[4] = (AnimationFrame) { (Rectangle) { 517, 708, -128, 140 }, 50 };
    p->forwardJumpAnim.frames[5] = (AnimationFrame) { (Rectangle) { 646, 708, -128, 140 }, 50 };
    p->forwardJumpAnim.frames[6] = (AnimationFrame) { (Rectangle) {   1, 708, -128, 140 }, 50 };

    p->backwardJumpAnim.frameCount = 7;
    p->backwardJumpAnim.currentFrame = 0;
    p->backwardJumpAnim.frameTimeCounter = 0.0f;
    p->backwardJumpAnim.stopAtLastFrame = true;
    createAnimationFrames( &p->backwardJumpAnim, p->backwardJumpAnim.frameCount );
    p->backwardJumpAnim.frames[0] = (AnimationFrame) { (Rectangle) {   1, 708, -128, 140 }, 50 };
    p->backwardJumpAnim.frames[1] = (AnimationFrame) { (Rectangle) { 646, 708, -128, 140 }, 50 };
    p->backwardJumpAnim.frames[2] = (AnimationFrame) { (Rectangle) { 517, 708, -128, 140 }, 50 };
    p->backwardJumpAnim.frames[3] = (AnimationFrame) { (Rectangle) { 388, 708, -128, 140 }, 50 };
    p->backwardJumpAnim.frames[4] = (AnimationFrame) { (Rectangle) { 259, 708, -128, 140 }, 50 };
    p->backwardJumpAnim.frames[5] = (AnimationFrame) { (Rectangle) { 130, 708, -128, 140 }, 50 };
    p->backwardJumpAnim.frames[6] = (AnimationFrame) { (Rectangle) {   1, 708, -128, 140 }, 50 };

    p->animations[0] = &p->idleAnim;
    p->animations[1] = &p->forwardAnim;
    p->animations[2] = &p->backwardAnim;
    p->animations[3] = &p->straightJumpAnim;
    p->animations[4] = &p->forwardJumpAnim;
    p->animations[5] = &p->backwardJumpAnim;
    p->animationCount = 6;

}

void initializePlayerKen( float x, float y, Player *p ) {

    p->pos.x = x;
    p->pos.y = y;
    p->dim.x = 80;
    p->dim.y = 100;
    p->texture = &rm.kenTexture;
    p->vel = (Vector2) { 0 };
    p->forwardSpeed = 150;
    p->backwardSpeed = 120;
    p->jumpSpeed = 350;
    p->state = PLAYER_STATE_IDLE;
    p->lastState = PLAYER_STATE_IDLE;
    p->jumping = false;
    p->lookingRight = true;

    p->idleAnim.frameCount = 6;
    p->idleAnim.currentFrame = 0;
    p->idleAnim.frameTimeCounter = 0.0f;
    p->idleAnim.stopAtLastFrame = false;
    createAnimationFrames( &p->idleAnim, p->idleAnim.frameCount );
    p->idleAnim.frames[0] = (AnimationFrame) { (Rectangle) {   1, 142, -64, 96 }, 65 };
    p->idleAnim.frames[1] = (AnimationFrame) { (Rectangle) {  66, 142, -64, 96 }, 65 };
    p->idleAnim.frames[2] = (AnimationFrame) { (Rectangle) { 131, 142, -64, 96 }, 65 };
    p->idleAnim.frames[3] = (AnimationFrame) { (Rectangle) { 196, 142, -64, 96 }, 65 };
    p->idleAnim.frames[4] = (AnimationFrame) { (Rectangle) { 131, 142, -64, 96 }, 65 };
    p->idleAnim.frames[5] = (AnimationFrame) { (Rectangle) {  66, 142, -64, 96 }, 65 };

    p->forwardAnim.frameCount = 5;
    p->forwardAnim.currentFrame = 0;
    p->forwardAnim.frameTimeCounter = 0.0f;
    p->forwardAnim.stopAtLastFrame = false;
    createAnimationFrames( &p->forwardAnim, p->forwardAnim.frameCount );
    p->forwardAnim.frames[0] = (AnimationFrame) { (Rectangle) {   1, 514, -80, 96 }, 70 };
    p->forwardAnim.frames[1] = (AnimationFrame) { (Rectangle) {  82, 514, -80, 96 }, 70 };
    p->forwardAnim.frames[2] = (AnimationFrame) { (Rectangle) { 163, 514, -80, 96 }, 70 };
    p->forwardAnim.frames[3] = (AnimationFrame) { (Rectangle) { 244, 514, -80, 96 }, 70 };
    p->forwardAnim.frames[4] = (AnimationFrame) { (Rectangle) { 325, 514, -80, 96 }, 70 };

    p->backwardAnim.frameCount = 6;
    p->backwardAnim.currentFrame = 0;
    p->backwardAnim.frameTimeCounter = 0.0f;
    p->backwardAnim.stopAtLastFrame = false;
    createAnimationFrames( &p->backwardAnim, p->backwardAnim.frameCount );
    p->backwardAnim.frames[0] = (AnimationFrame) { (Rectangle) {   1, 611, -80, 96 }, 80 };
    p->backwardAnim.frames[1] = (AnimationFrame) { (Rectangle) {  82, 611, -80, 96 }, 80 };
    p->backwardAnim.frames[2] = (AnimationFrame) { (Rectangle) { 163, 611, -80, 96 }, 80 };
    p->backwardAnim.frames[3] = (AnimationFrame) { (Rectangle) { 244, 611, -80, 96 }, 80 };
    p->backwardAnim.frames[4] = (AnimationFrame) { (Rectangle) { 325, 611, -80, 96 }, 80 };
    p->backwardAnim.frames[5] = (AnimationFrame) { (Rectangle) { 406, 611, -80, 96 }, 80 };

    p->straightJumpAnim.frameCount = 6;
    p->straightJumpAnim.currentFrame = 0;
    p->straightJumpAnim.frameTimeCounter = 0.0f;
    p->straightJumpAnim.stopAtLastFrame = true;
    createAnimationFrames( &p->straightJumpAnim, p->straightJumpAnim.frameCount );
    p->straightJumpAnim.frames[0] = (AnimationFrame) { (Rectangle) {   1, 849, -64, 140 }, 70 };
    p->straightJumpAnim.frames[1] = (AnimationFrame) { (Rectangle) {  66, 849, -64, 140 }, 70 };
    p->straightJumpAnim.frames[2] = (AnimationFrame) { (Rectangle) { 131, 849, -64, 140 }, 70 };
    p->straightJumpAnim.frames[3] = (AnimationFrame) { (Rectangle) { 196, 849, -64, 140 }, 70 };
    p->straightJumpAnim.frames[4] = (AnimationFrame) { (Rectangle) { 261, 849, -64, 140 }, 70 };
    p->straightJumpAnim.frames[5] = (AnimationFrame) { (Rectangle) { 326, 849, -64, 140 }, 70 };

    p->forwardJumpAnim.frameCount = 7;
    p->forwardJumpAnim.currentFrame = 0;
    p->forwardJumpAnim.frameTimeCounter = 0.0f;
    p->forwardJumpAnim.stopAtLastFrame = true;
    createAnimationFrames( &p->forwardJumpAnim, p->forwardJumpAnim.frameCount );
    p->forwardJumpAnim.frames[0] = (AnimationFrame) { (Rectangle) {   1, 708, -128, 140 }, 50 };
    p->forwardJumpAnim.frames[1] = (AnimationFrame) { (Rectangle) { 130, 708, -128, 140 }, 50 };
    p->forwardJumpAnim.frames[2] = (AnimationFrame) { (Rectangle) { 259, 708, -128, 140 }, 50 };
    p->forwardJumpAnim.frames[3] = (AnimationFrame) { (Rectangle) { 388, 708, -128, 140 }, 50 };
    p->forwardJumpAnim.frames[4] = (AnimationFrame) { (Rectangle) { 517, 708, -128, 140 }, 50 };
    p->forwardJumpAnim.frames[5] = (AnimationFrame) { (Rectangle) { 646, 708, -128, 140 }, 50 };
    p->forwardJumpAnim.frames[6] = (AnimationFrame) { (Rectangle) {   1, 708, -128, 140 }, 50 };

    p->backwardJumpAnim.frameCount = 7;
    p->backwardJumpAnim.currentFrame = 0;
    p->backwardJumpAnim.frameTimeCounter = 0.0f;
    p->backwardJumpAnim.stopAtLastFrame = true;
    createAnimationFrames( &p->backwardJumpAnim, p->backwardJumpAnim.frameCount );
    p->backwardJumpAnim.frames[0] = (AnimationFrame) { (Rectangle) {   1, 708, -128, 140 }, 50 };
    p->backwardJumpAnim.frames[1] = (AnimationFrame) { (Rectangle) { 646, 708, -128, 140 }, 50 };
    p->backwardJumpAnim.frames[2] = (AnimationFrame) { (Rectangle) { 517, 708, -128, 140 }, 50 };
    p->backwardJumpAnim.frames[3] = (AnimationFrame) { (Rectangle) { 388, 708, -128, 140 }, 50 };
    p->backwardJumpAnim.frames[4] = (AnimationFrame) { (Rectangle) { 259, 708, -128, 140 }, 50 };
    p->backwardJumpAnim.frames[5] = (AnimationFrame) { (Rectangle) { 130, 708, -128, 140 }, 50 };
    p->backwardJumpAnim.frames[6] = (AnimationFrame) { (Rectangle) {   1, 708, -128, 140 }, 50 };

    p->animations[0] = &p->idleAnim;
    p->animations[1] = &p->forwardAnim;
    p->animations[2] = &p->backwardAnim;
    p->animations[3] = &p->straightJumpAnim;
    p->animations[4] = &p->forwardJumpAnim;
    p->animations[5] = &p->backwardJumpAnim;
    p->animationCount = 6;

}

void destroyPlayer( Player *player ) {
    for ( int i = 0; i < player->animationCount; i++ ) {
        destroyAnimationFrames( player->animations[i] );
    }
    free( player );
}

void drawPlayer( Player *player ) {

    AnimationFrame *frame = getPlayerCurrentAnimationFrame( player );

    if ( frame != NULL ) {
        DrawTexturePro( 
            *player->texture,
            (Rectangle) {
                frame->source.x,
                frame->source.y,
                player->lookingRight ? frame->source.width : -frame->source.width,
                frame->source.height
            },
            (Rectangle) { 
                player->pos.x - fabs( frame->source.width ) / 2,
                player->pos.y + player->dim.y - frame->source.height,
                frame->source.width,
                frame->source.height
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
        if ( IsKeyDown( player->kb.right ) ) {
            player->vel.x = player->forwardSpeed;
            player->state = PLAYER_STATE_WALKING_FORWARD;
        } else if ( IsKeyDown( player->kb.left ) ) {
            player->vel.x = -player->backwardSpeed;
            player->state = PLAYER_STATE_WALKING_BACKWARD;
        } else {
            player->vel.x = 0.0f;
            player->state = PLAYER_STATE_IDLE;
        }
    }

    if ( IsKeyDown( player->kb.up ) && !player->jumping ) {
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
            updateAnimation( &player->straightJumpAnim, delta );
        } else if ( player->vel.x > 0.0f ) {
            if ( player->lookingRight ) {
                updateAnimation( &player->forwardJumpAnim, delta );
            } else {
                updateAnimation( &player->backwardJumpAnim, delta );
            }
        } else {
            if ( player->lookingRight ) {
                updateAnimation( &player->backwardJumpAnim, delta );
            } else {
                updateAnimation( &player->forwardJumpAnim, delta );
            }
        }
    } else {
        if ( player->state == PLAYER_STATE_IDLE ) {
            updateAnimation( &player->idleAnim, delta );
        } else if ( player->state == PLAYER_STATE_WALKING_FORWARD ) {
            if ( player->lookingRight ) {
                updateAnimation( &player->forwardAnim, delta );
            } else {
                updateAnimation( &player->backwardAnim, delta );
            }
        } else if ( player->state == PLAYER_STATE_WALKING_BACKWARD ) {
            if ( player->lookingRight ) {
                updateAnimation( &player->backwardAnim, delta );
            } else {
                updateAnimation( &player->forwardAnim, delta );
            }
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
    player->lookingRight = !player->lookingRight;
}

AnimationFrame *getPlayerCurrentAnimationFrame( Player *player ) {

    if ( player->jumping ) {
        if ( player->vel.x == 0.0f ) {
            return getAnimationCurrentFrame( &player->straightJumpAnim );
        } else if ( player->vel.x > 0.0f ) {
            if ( player->lookingRight ) {
                return getAnimationCurrentFrame( &player->forwardJumpAnim );
            }
            return getAnimationCurrentFrame( &player->backwardJumpAnim );
        } else {
            if ( player->lookingRight ) {
                return getAnimationCurrentFrame( &player->backwardJumpAnim );
            }
            return getAnimationCurrentFrame( &player->forwardJumpAnim );
        }
    }

    switch ( player->state ) {
        case PLAYER_STATE_IDLE:
            return getAnimationCurrentFrame( &player->idleAnim );
        case PLAYER_STATE_WALKING_FORWARD:
            if ( player->lookingRight ) {
                return getAnimationCurrentFrame( &player->forwardAnim );
            }
            return getAnimationCurrentFrame( &player->backwardAnim );
        case PLAYER_STATE_WALKING_BACKWARD:
            if ( player->lookingRight ) {
                return getAnimationCurrentFrame( &player->backwardAnim );
            }
            return getAnimationCurrentFrame( &player->forwardAnim );
    }
    return NULL;
}

void resetPlayerAnimations( Player *player ) {
    for ( int i = 0; i < player->animationCount; i++ ) {
        resetAnimation( player->animations[i] );
    }
}
