#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "raylib/raylib.h"

#include "Macros.h"
#include "Player.h"
#include "ResourceManager.h"

Player *createPlayer( float x, float y, float scaleX, float scaleY ) {

    Player *p = (Player*) malloc( sizeof( Player ) );

    p->pos.x = x;
    p->pos.y = y;
    p->dim.x = 300;
    p->dim.y = 300;
    p->texture = &rm.playerTexture;
    p->scale = (Vector2) { scaleX, scaleY };
    p->vel = (Vector2) { 0 };
    p->walkingSpeed = 450;
    p->jumpSpeed = 980;
    p->state = PLAYER_STATE_IDLE;
    p->lastState = PLAYER_STATE_IDLE;
    p->jumping = false;
    p->lookingRight = true;

    //p->idleAnim.frameCount = 5;
    p->idleAnim.frameCount = 6;
    p->idleAnim.currentFrame = 0;
    p->idleAnim.frameTime = 0.042f;
    p->idleAnim.frameTimeCounter = 0.0f;
    p->idleAnim.stopAtLastFrame = false;
    createPlayerAnimationSources( &p->idleAnim, p->idleAnim.frameCount );
    p->idleAnim.sources[0] = (Rectangle) { 1, 904, -64, 96 };
    p->idleAnim.sources[1] = (Rectangle) { 66, 904, -64, 96 };
    p->idleAnim.sources[2] = (Rectangle) { 131, 904, -64, 96 };
    p->idleAnim.sources[3] = (Rectangle) { 196, 904, -64, 96 };
    p->idleAnim.sources[4] = (Rectangle) { 131, 904, -64, 96 };
    p->idleAnim.sources[5] = (Rectangle) { 66, 904, -64, 96 };

    //p->forwardAnim.frameCount = 6;
    p->forwardAnim.frameCount = 5;
    p->forwardAnim.currentFrame = 0;
    p->forwardAnim.frameTime = 0.06f;
    p->forwardAnim.frameTimeCounter = 0.0f;
    p->forwardAnim.stopAtLastFrame = false;
    createPlayerAnimationSources( &p->forwardAnim, p->forwardAnim.frameCount );
    p->forwardAnim.sources[0] = (Rectangle) { 1, 1276, -80, 96 };
    p->forwardAnim.sources[1] = (Rectangle) { 82, 1276, -80, 96 };
    p->forwardAnim.sources[2] = (Rectangle) { 163, 1276, -80, 96 };
    p->forwardAnim.sources[3] = (Rectangle) { 244, 1276, -80, 96 };
    p->forwardAnim.sources[4] = (Rectangle) { 325, 1276, -80, 96 };

    p->backwardAnim.frameCount = 6;
    p->backwardAnim.currentFrame = 0;
    p->backwardAnim.frameTime = 0.07f;
    p->backwardAnim.frameTimeCounter = 0.0f;
    p->backwardAnim.stopAtLastFrame = false;
    createPlayerAnimationSources( &p->backwardAnim, p->backwardAnim.frameCount );
    p->backwardAnim.sources[0] = (Rectangle) { 1, 1373, -80, 96 };
    p->backwardAnim.sources[1] = (Rectangle) { 82, 1373, -80, 96 };
    p->backwardAnim.sources[2] = (Rectangle) { 163, 1373, -80, 96 };
    p->backwardAnim.sources[3] = (Rectangle) { 244, 1373, -80, 96 };
    p->backwardAnim.sources[4] = (Rectangle) { 325, 1373, -80, 96 };
    p->backwardAnim.sources[5] = (Rectangle) { 406, 1373, -80, 96 };

    p->straightJumpAnim.frameCount = 6;
    p->straightJumpAnim.currentFrame = 0;
    p->straightJumpAnim.frameTime = 0.07f;
    p->straightJumpAnim.frameTimeCounter = 0.0f;
    p->straightJumpAnim.stopAtLastFrame = true;
    createPlayerAnimationSources( &p->straightJumpAnim, p->straightJumpAnim.frameCount );
    p->straightJumpAnim.sources[0] = (Rectangle) { 1, 1611, -64, 140 };
    p->straightJumpAnim.sources[1] = (Rectangle) { 66, 1611, -64, 140 };
    p->straightJumpAnim.sources[2] = (Rectangle) { 131, 1611, -64, 140 };
    p->straightJumpAnim.sources[3] = (Rectangle) { 196, 1611, -64, 140 };
    p->straightJumpAnim.sources[4] = (Rectangle) { 261, 1611, -64, 140 };
    p->straightJumpAnim.sources[5] = (Rectangle) { 326, 1611, -64, 140 };

    p->forwardJumpAnim.frameCount = 7;
    p->forwardJumpAnim.currentFrame = 0;
    p->forwardJumpAnim.frameTime = 0.05f;
    p->forwardJumpAnim.frameTimeCounter = 0.0f;
    p->forwardJumpAnim.stopAtLastFrame = true;
    createPlayerAnimationSources( &p->forwardJumpAnim, p->forwardJumpAnim.frameCount );
    p->forwardJumpAnim.sources[0] = (Rectangle) { 1, 1470, -128, 140 };
    p->forwardJumpAnim.sources[1] = (Rectangle) { 130, 1470, -128, 140 };
    p->forwardJumpAnim.sources[2] = (Rectangle) { 259, 1470, -128, 140 };
    p->forwardJumpAnim.sources[3] = (Rectangle) { 388, 1470, -128, 140 };
    p->forwardJumpAnim.sources[4] = (Rectangle) { 517, 1470, -128, 140 };
    p->forwardJumpAnim.sources[5] = (Rectangle) { 646, 1470, -128, 140 };
    p->forwardJumpAnim.sources[6] = (Rectangle) { 1, 1470, -128, 140 };

    p->backwardJumpAnim.frameCount = 7;
    p->backwardJumpAnim.currentFrame = 0;
    p->backwardJumpAnim.frameTime = 0.05f;
    p->backwardJumpAnim.frameTimeCounter = 0.0f;
    p->backwardJumpAnim.stopAtLastFrame = true;
    createPlayerAnimationSources( &p->backwardJumpAnim, p->backwardJumpAnim.frameCount );
    p->backwardJumpAnim.sources[0] = (Rectangle) { 1, 1470, -128, 140 };
    p->backwardJumpAnim.sources[1] = (Rectangle) { 646, 1470, -128, 140 };
    p->backwardJumpAnim.sources[2] = (Rectangle) { 517, 1470, -128, 140 };
    p->backwardJumpAnim.sources[3] = (Rectangle) { 388, 1470, -128, 140 };
    p->backwardJumpAnim.sources[4] = (Rectangle) { 259, 1470, -128, 140 };
    p->backwardJumpAnim.sources[5] = (Rectangle) { 130, 1470, -128, 140 };
    p->backwardJumpAnim.sources[6] = (Rectangle) { 1, 1470, -128, 140 };

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
                player->pos.x - ( sourceRectangle->width * player->scale.x ) / 2,
                player->pos.y + player->dim.y - ( sourceRectangle->height * player->scale.y ),
                sourceRectangle->width * player->scale.x,
                sourceRectangle->height * player->scale.y
            },
            (Vector2) { 0 },
            0.0f,
            WHITE
        );
    }

    DrawCircle( player->pos.x - ( sourceRectangle->width * player->scale.x ) / 2, player->pos.y, 10, BLUE );
    DrawRectangleLines( player->pos.x, player->pos.y, player->dim.x, player->dim.y, BLUE );

}

void updatePlayer( Player *player, float delta ) {

    // state changes
    if ( !player->jumping ) {
        if ( IsKeyDown( KEY_LEFT ) ) {
            player->vel.x = -player->walkingSpeed;
            player->state = PLAYER_STATE_WALKING_BACKWARD;
        } else if ( IsKeyDown( KEY_RIGHT ) ) {
            player->vel.x = player->walkingSpeed;
            player->state = PLAYER_STATE_WALKING_FORWARD;
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
            player->jumping = true;
        } else {
            //trace( "jumping left" );
            player->vel.y = -player->jumpSpeed;
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

    player->vel.y += GRAVITY * delta;
    
    if ( player->vel.y > 900 ) {
        player->vel.y = 900;
    }

    player->lastState = player->state;
    
}

void flipPlayerSide( Player *player ) {
    for ( int i = 0; i < player->animationCount; i++ ) {
        PlayerAnimation *a = player->animations[i];
        for ( int j = 0; j < a->frameCount; j++ ) {
            a->sources[j].width = -a->sources[j].width;
        }
    }
    player->lookingRight = !player->lookingRight;
}

void createPlayerAnimationSources( PlayerAnimation *pa, int frameCount ) {
    pa->sources = (Rectangle*) malloc( sizeof( Rectangle ) * frameCount );
}

void destroyPlayerAnimationSources( PlayerAnimation *pa ) {
    free( pa->sources );
}

void updatePlayerAnimation( PlayerAnimation *pa, float delta ) {

    if ( pa->stopAtLastFrame && pa->currentFrame % pa->frameCount == pa->frameCount - 1 ) {
        return;
    }

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

Rectangle *getPlayerAnimationSource( PlayerAnimation *pa ) {
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
        PlayerAnimation *a = player->animations[i];
        a->currentFrame = 0;
        a->frameTimeCounter = 0.0f;
        a->currentFrame = 0;
    }
}
