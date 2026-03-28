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

#define TRACK_STATE_SIZE 14
static PlayerState states[TRACK_STATE_SIZE] = { 0 };
static int head = -1;
static int tail = -1;
static int count = 0;

static void drawPlayerAnimationFrameBoxes( Player *player );

static void addState( int state ) {
    if ( count == 0 ) {
        states[0] = state;
        head = 0;
        tail = 0;
        count++;
    } else {
        tail++;
        count++;
        states[tail%TRACK_STATE_SIZE] = state;
        if ( count > TRACK_STATE_SIZE ) {
            head++;
        }
    }
}

/*static const char *playerStateToText( PlayerState state ) {

    switch ( state ) {
        case PLAYER_STATE_IDLE: return "I";
        case PLAYER_STATE_WALKING_FORWARD: return "F";
        case PLAYER_STATE_WALKING_BACKWARD: return "B";
        case PLAYER_STATE_CROUCHING: return "C";
        case PLAYER_STATE_JUMPING_STRAIGHT: return "J";
        case PLAYER_STATE_JUMPING_FORWARD: return "J";
        case PLAYER_STATE_JUMPING_BACKWARD: return "J";
        case PLAYER_STATE_LP: return "P";
        case PLAYER_STATE_MP: return "P";
        case PLAYER_STATE_HP: return "P";
        case PLAYER_STATE_LK: return "K";
        case PLAYER_STATE_MK: return "K";
        case PLAYER_STATE_HK: return "K";
        case PLAYER_STATE_LP_CLOSE: return "P";
        case PLAYER_STATE_MP_CLOSE: return "P";
        case PLAYER_STATE_HP_CLOSE: return "P";
        case PLAYER_STATE_LK_CLOSE: return "K";
        case PLAYER_STATE_MK_CLOSE: return "K";
        case PLAYER_STATE_HK_CLOSE: return "K";
        case PLAYER_STATE_LP_CROUCH: return "P";
        case PLAYER_STATE_MP_CROUCH: return "P";
        case PLAYER_STATE_HP_CROUCH: return "P";
        case PLAYER_STATE_LK_CROUCH: return "K";
        case PLAYER_STATE_MK_CROUCH: return "K";
        case PLAYER_STATE_HK_CROUCH: return "K";
        case PLAYER_STATE_JUMP_COOLDOWN: return "L";
        case PLAYER_STATE_LP_JUMP_STRAIGHT: return "P";
        case PLAYER_STATE_MP_JUMP_STRAIGHT: return "P";
        case PLAYER_STATE_HP_JUMP_STRAIGHT: return "P";
        case PLAYER_STATE_LK_JUMP_STRAIGHT: return "K";
        case PLAYER_STATE_MK_JUMP_STRAIGHT: return "K";
        case PLAYER_STATE_HK_JUMP_STRAIGHT: return "K";
        case PLAYER_STATE_LP_JUMP_FORWARD: return "P";
        case PLAYER_STATE_MP_JUMP_FORWARD: return "P";
        case PLAYER_STATE_HP_JUMP_FORWARD: return "P";
        case PLAYER_STATE_LK_JUMP_FORWARD: return "K";
        case PLAYER_STATE_MK_JUMP_FORWARD: return "K";
        case PLAYER_STATE_HK_JUMP_FORWARD: return "K";
        case PLAYER_STATE_LP_JUMP_BACKWARD: return "P";
        case PLAYER_STATE_MP_JUMP_BACKWARD: return "P";
        case PLAYER_STATE_HP_JUMP_BACKWARD: return "P";
        case PLAYER_STATE_LK_JUMP_BACKWARD: return "K";
        case PLAYER_STATE_MK_JUMP_BACKWARD: return "K";
        case PLAYER_STATE_HK_JUMP_BACKWARD: return "K";
    }

    return "";

}

static Color playerStateToColor( PlayerState state ) {

    switch ( state ) {
        case PLAYER_STATE_IDLE: return LIGHTGRAY;
        case PLAYER_STATE_WALKING_FORWARD: return LIGHTGRAY;
        case PLAYER_STATE_WALKING_BACKWARD: return LIGHTGRAY;
        case PLAYER_STATE_CROUCHING: return LIGHTGRAY;
        case PLAYER_STATE_JUMPING_STRAIGHT: return LIGHTGRAY;
        case PLAYER_STATE_JUMPING_FORWARD: return LIGHTGRAY;
        case PLAYER_STATE_JUMPING_BACKWARD: return LIGHTGRAY;
        case PLAYER_STATE_LP: return SKYBLUE;
        case PLAYER_STATE_MP: return YELLOW;
        case PLAYER_STATE_HP: return RED;
        case PLAYER_STATE_LK: return SKYBLUE;
        case PLAYER_STATE_MK: return YELLOW;
        case PLAYER_STATE_HK: return RED;
        case PLAYER_STATE_LP_CLOSE: return SKYBLUE;
        case PLAYER_STATE_MP_CLOSE: return YELLOW;
        case PLAYER_STATE_HP_CLOSE: return RED;
        case PLAYER_STATE_LK_CLOSE: return SKYBLUE;
        case PLAYER_STATE_MK_CLOSE: return YELLOW;
        case PLAYER_STATE_HK_CLOSE: return RED;
        case PLAYER_STATE_LP_CROUCH: return SKYBLUE;
        case PLAYER_STATE_MP_CROUCH: return YELLOW;
        case PLAYER_STATE_HP_CROUCH: return RED;
        case PLAYER_STATE_LK_CROUCH: return SKYBLUE;
        case PLAYER_STATE_MK_CROUCH: return YELLOW;
        case PLAYER_STATE_HK_CROUCH: return RED;
        case PLAYER_STATE_JUMP_COOLDOWN: return ORANGE;
        case PLAYER_STATE_LP_JUMP_STRAIGHT: return SKYBLUE;
        case PLAYER_STATE_MP_JUMP_STRAIGHT: return YELLOW;
        case PLAYER_STATE_HP_JUMP_STRAIGHT: return RED;
        case PLAYER_STATE_LK_JUMP_STRAIGHT: return SKYBLUE;
        case PLAYER_STATE_MK_JUMP_STRAIGHT: return YELLOW;
        case PLAYER_STATE_HK_JUMP_STRAIGHT: return RED;
        case PLAYER_STATE_LP_JUMP_FORWARD: return SKYBLUE;
        case PLAYER_STATE_MP_JUMP_FORWARD: return YELLOW;
        case PLAYER_STATE_HP_JUMP_FORWARD: return RED;
        case PLAYER_STATE_LK_JUMP_FORWARD: return SKYBLUE;
        case PLAYER_STATE_MK_JUMP_FORWARD: return YELLOW;
        case PLAYER_STATE_HK_JUMP_FORWARD: return RED;
        case PLAYER_STATE_LP_JUMP_BACKWARD: return SKYBLUE;
        case PLAYER_STATE_MP_JUMP_BACKWARD: return YELLOW;
        case PLAYER_STATE_HP_JUMP_BACKWARD: return RED;
        case PLAYER_STATE_LK_JUMP_BACKWARD: return SKYBLUE;
        case PLAYER_STATE_MK_JUMP_BACKWARD: return YELLOW;
        case PLAYER_STATE_HK_JUMP_BACKWARD: return RED;
    }

    return PINK;

}*/

Player *createPlayer() {
    Player *p = (Player*) malloc( sizeof( Player ) );
    return p;
}

void initializePlayerRyu( float x, float y, Player *p, bool showBoxes, bool showDebugInfo ) {

    p->pos.x = x;
    p->pos.y = y;
    p->dim.x = 80;
    p->dim.y = 100;
    p->texture = &rm.ryuTexture;
    p->vel = (Vector2) { 0 };
    p->forwardSpeed = 150;
    p->backwardSpeed = 120;
    p->jumpSpeed = 420;
    p->state = PLAYER_STATE_IDLE;
    p->lastState = PLAYER_STATE_IDLE;
    p->lookingRight = true;
    p->showBoxes = showBoxes;
    p->showDebugInfo = showDebugInfo;

    p->lpCloseTriggerDist = 70;
    p->mpCloseTriggerDist = 70;
    p->hpCloseTriggerDist = 70;
    p->lkCloseTriggerDist = 50;
    p->mkCloseTriggerDist = 50;
    p->hkCloseTriggerDist = 90;

    p->idleAnim.frameCount = 6;
    p->idleAnim.currentFrame = 0;
    p->idleAnim.frameTimeCounter = 0.0f;
    p->idleAnim.stopAtLastFrame = false;
    p->idleAnim.runOnce = false;
    p->idleAnim.finished = false;
    createAnimationFrames( &p->idleAnim, p->idleAnim.frameCount );
    p->idleAnim.frames[0] = (AnimationFrame) { {   1, 142, -64, 96 }, 65, { 0, 0 }, .boxes = { 0 } };
    p->idleAnim.frames[1] = (AnimationFrame) { {  66, 142, -64, 96 }, 65, { 0, 0 }, .boxes = { 0 } };
    p->idleAnim.frames[2] = (AnimationFrame) { { 131, 142, -64, 96 }, 65, { 0, 0 }, .boxes = { 0 } };
    p->idleAnim.frames[3] = (AnimationFrame) { { 196, 142, -64, 96 }, 65, { 0, 0 }, .boxes = { 0 } };
    p->idleAnim.frames[4] = (AnimationFrame) { { 131, 142, -64, 96 }, 65, { 0, 0 }, .boxes = { 0 } };
    p->idleAnim.frames[5] = (AnimationFrame) { {  66, 142, -64, 96 }, 65, { 0, 0 }, .boxes = { 0 } };

    p->forwardAnim.frameCount = 5;
    p->forwardAnim.currentFrame = 0;
    p->forwardAnim.frameTimeCounter = 0.0f;
    p->forwardAnim.stopAtLastFrame = false;
    p->forwardAnim.runOnce = false;
    p->forwardAnim.finished = false;
    createAnimationFrames( &p->forwardAnim, p->forwardAnim.frameCount );
    p->forwardAnim.frames[0] = (AnimationFrame) { {   1, 514, -80, 96 }, 70, { 0, 0 }, .boxes = { 0 } };
    p->forwardAnim.frames[1] = (AnimationFrame) { {  82, 514, -80, 96 }, 70, { 0, 0 }, .boxes = { 0 } };
    p->forwardAnim.frames[2] = (AnimationFrame) { { 163, 514, -80, 96 }, 70, { 0, 0 }, .boxes = { 0 } };
    p->forwardAnim.frames[3] = (AnimationFrame) { { 244, 514, -80, 96 }, 70, { 0, 0 }, .boxes = { 0 } };
    p->forwardAnim.frames[4] = (AnimationFrame) { { 325, 514, -80, 96 }, 70, { 0, 0 }, .boxes = { 0 } };

    p->backwardAnim.frameCount = 6;
    p->backwardAnim.currentFrame = 0;
    p->backwardAnim.frameTimeCounter = 0.0f;
    p->backwardAnim.stopAtLastFrame = false;
    p->backwardAnim.runOnce = false;
    p->backwardAnim.finished = false;
    createAnimationFrames( &p->backwardAnim, p->backwardAnim.frameCount );
    p->backwardAnim.frames[0] = (AnimationFrame) { {   1, 611, -80, 96 }, 80, { 0, 0 }, .boxes = { 0 } };
    p->backwardAnim.frames[1] = (AnimationFrame) { {  82, 611, -80, 96 }, 80, { 0, 0 }, .boxes = { 0 } };
    p->backwardAnim.frames[2] = (AnimationFrame) { { 163, 611, -80, 96 }, 80, { 0, 0 }, .boxes = { 0 } };
    p->backwardAnim.frames[3] = (AnimationFrame) { { 244, 611, -80, 96 }, 80, { 0, 0 }, .boxes = { 0 } };
    p->backwardAnim.frames[4] = (AnimationFrame) { { 325, 611, -80, 96 }, 80, { 0, 0 }, .boxes = { 0 } };
    p->backwardAnim.frames[5] = (AnimationFrame) { { 406, 611, -80, 96 }, 80, { 0, 0 }, .boxes = { 0 } };

    p->straightJumpAnim.frameCount = 6;
    p->straightJumpAnim.currentFrame = 0;
    p->straightJumpAnim.frameTimeCounter = 0.0f;
    p->straightJumpAnim.stopAtLastFrame = true;
    p->straightJumpAnim.runOnce = false;
    p->straightJumpAnim.finished = false;
    createAnimationFrames( &p->straightJumpAnim, p->straightJumpAnim.frameCount );
    p->straightJumpAnim.frames[0] = (AnimationFrame) { {   1, 821, -64, 112 }, 250, { 0, 0 }, .boxes = { 0 } };
    p->straightJumpAnim.frames[1] = (AnimationFrame) { {  66, 821, -64, 112 },  50, { 0, 0 }, .boxes = { 0 } };
    p->straightJumpAnim.frames[2] = (AnimationFrame) { { 131, 821, -64, 112 },  50, { 0, 0 }, .boxes = { 0 } };
    p->straightJumpAnim.frames[3] = (AnimationFrame) { { 196, 821, -64, 112 },  50, { 0, 0 }, .boxes = { 0 } };
    p->straightJumpAnim.frames[4] = (AnimationFrame) { { 261, 821, -64, 112 },  50, { 0, 0 }, .boxes = { 0 } };
    p->straightJumpAnim.frames[5] = (AnimationFrame) { { 326, 821, -64, 112 },  50, { 0, 0 }, .boxes = { 0 } };

    p->forwardJumpAnim.frameCount = 7;
    p->forwardJumpAnim.currentFrame = 0;
    p->forwardJumpAnim.frameTimeCounter = 0.0f;
    p->forwardJumpAnim.stopAtLastFrame = true;
    p->forwardJumpAnim.runOnce = false;
    p->forwardJumpAnim.finished = false;
    createAnimationFrames( &p->forwardJumpAnim, p->forwardJumpAnim.frameCount );
    p->forwardJumpAnim.frames[0] = (AnimationFrame) { {   1, 708, -128, 112 }, 250, { 0, 0 }, .boxes = { 0 } };
    p->forwardJumpAnim.frames[1] = (AnimationFrame) { { 130, 708, -128, 112 },  50, { 0, 0 }, .boxes = { 0 } };
    p->forwardJumpAnim.frames[2] = (AnimationFrame) { { 259, 708, -128, 112 },  50, { 0, 0 }, .boxes = { 0 } };
    p->forwardJumpAnim.frames[3] = (AnimationFrame) { { 388, 708, -128, 112 },  50, { 0, 0 }, .boxes = { 0 } };
    p->forwardJumpAnim.frames[4] = (AnimationFrame) { { 517, 708, -128, 112 },  50, { 0, 0 }, .boxes = { 0 } };
    p->forwardJumpAnim.frames[5] = (AnimationFrame) { { 646, 708, -128, 112 },  50, { 0, 0 }, .boxes = { 0 } };
    p->forwardJumpAnim.frames[6] = (AnimationFrame) { {   1, 708, -128, 112 },  50, { 0, 0 }, .boxes = { 0 } };

    p->backwardJumpAnim.frameCount = 7;
    p->backwardJumpAnim.currentFrame = 0;
    p->backwardJumpAnim.frameTimeCounter = 0.0f;
    p->backwardJumpAnim.stopAtLastFrame = true;
    p->backwardJumpAnim.runOnce = false;
    p->backwardJumpAnim.finished = false;
    createAnimationFrames( &p->backwardJumpAnim, p->backwardJumpAnim.frameCount );
    p->backwardJumpAnim.frames[0] = (AnimationFrame) { {   1, 708, -128, 112 }, 250, { 0, 0 }, .boxes = { 0 } };
    p->backwardJumpAnim.frames[1] = (AnimationFrame) { { 646, 708, -128, 112 }, 50, { 0, 0 }, .boxes = { 0 } };
    p->backwardJumpAnim.frames[2] = (AnimationFrame) { { 517, 708, -128, 112 }, 50, { 0, 0 }, .boxes = { 0 } };
    p->backwardJumpAnim.frames[3] = (AnimationFrame) { { 388, 708, -128, 112 }, 50, { 0, 0 }, .boxes = { 0 } };
    p->backwardJumpAnim.frames[4] = (AnimationFrame) { { 259, 708, -128, 112 }, 50, { 0, 0 }, .boxes = { 0 } };
    p->backwardJumpAnim.frames[5] = (AnimationFrame) { { 130, 708, -128, 112 }, 50, { 0, 0 }, .boxes = { 0 } };
    p->backwardJumpAnim.frames[6] = (AnimationFrame) { {   1, 708, -128, 112 }, 50, { 0, 0 }, .boxes = { 0 } };

    p->jumpCooldownAnim.frameCount = 1;
    p->jumpCooldownAnim.currentFrame = 0;
    p->jumpCooldownAnim.frameTimeCounter = 0.0f;
    p->jumpCooldownAnim.stopAtLastFrame = false;
    p->jumpCooldownAnim.runOnce = true;
    p->jumpCooldownAnim.finished = false;
    createAnimationFrames( &p->jumpCooldownAnim, p->jumpCooldownAnim.frameCount );
    p->jumpCooldownAnim.frames[0] = (AnimationFrame) { {   1, 934, -64, 96 }, 60, { 0, 0 }, .boxes = { 0 } };

    p->crouchingAnim.frameCount = 3;
    p->crouchingAnim.currentFrame = 0;
    p->crouchingAnim.frameTimeCounter = 0.0f;
    p->crouchingAnim.stopAtLastFrame = true;
    p->crouchingAnim.runOnce = false;
    p->crouchingAnim.finished = false;
    createAnimationFrames( &p->crouchingAnim, p->crouchingAnim.frameCount );
    p->crouchingAnim.frames[0] = (AnimationFrame) { {   1, 352, -80, 96 }, 30, { 0, 0 }, .boxes = { 0 } };
    p->crouchingAnim.frames[1] = (AnimationFrame) { {  82, 352, -80, 96 }, 30, { 0, 0 }, .boxes = { 0 } };
    p->crouchingAnim.frames[2] = (AnimationFrame) { { 163, 352, -80, 96 }, 30, { 0, 0 }, .boxes = { 0 } };

    p->lpAnim.frameCount = 3;
    p->lpAnim.currentFrame = 0;
    p->lpAnim.frameTimeCounter = 0.0f;
    p->lpAnim.stopAtLastFrame = false;
    p->lpAnim.runOnce = true;
    p->lpAnim.finished = false;
    createAnimationFrames( &p->lpAnim, p->lpAnim.frameCount );
    p->lpAnim.frames[0] = (AnimationFrame) { {   1, 1031, -112, 96 }, 50, { 22, 0 }, .boxes = { 0 } };
    p->lpAnim.frames[1] = (AnimationFrame) { { 114, 1031, -112, 96 }, 50, { 22, 0 }, .boxes = { 0 } };
    p->lpAnim.frames[2] = (AnimationFrame) { {   1, 1031, -112, 96 }, 50, { 22, 0 }, .boxes = { 0 } };
    
    p->mpAnim.frameCount = 5;
    p->mpAnim.currentFrame = 0;
    p->mpAnim.frameTimeCounter = 0.0f;
    p->mpAnim.stopAtLastFrame = false;
    p->mpAnim.runOnce = true;
    p->mpAnim.finished = false;
    createAnimationFrames( &p->mpAnim, p->mpAnim.frameCount );
    p->mpAnim.frames[0] = (AnimationFrame) { {   1, 1128, -128, 96 }, 50, { 30, 0 }, .boxes = { 0 } };
    p->mpAnim.frames[1] = (AnimationFrame) { { 130, 1128, -128, 96 }, 50, { 30, 0 }, .boxes = { 0 } };
    p->mpAnim.frames[2] = (AnimationFrame) { { 259, 1128, -128, 96 }, 50, { 30, 0 }, .boxes = { 0 } };
    p->mpAnim.frames[3] = (AnimationFrame) { { 130, 1128, -128, 96 }, 50, { 30, 0 }, .boxes = { 0 } };
    p->mpAnim.frames[4] = (AnimationFrame) { {   1, 1128, -128, 96 }, 50, { 30, 0 }, .boxes = { 0 } };

    p->hpAnim.frameCount = 5;
    p->hpAnim.currentFrame = 0;
    p->hpAnim.frameTimeCounter = 0.0f;
    p->hpAnim.stopAtLastFrame = false;
    p->hpAnim.runOnce = true;
    p->hpAnim.finished = false;
    createAnimationFrames( &p->hpAnim, p->hpAnim.frameCount );
    p->hpAnim.frames[0] = (AnimationFrame) { {   1, 1128, -128, 96 }, 90, { 30, 0 }, .boxes = { 0 } };
    p->hpAnim.frames[1] = (AnimationFrame) { { 130, 1128, -128, 96 }, 90, { 30, 0 }, .boxes = { 0 } };
    p->hpAnim.frames[2] = (AnimationFrame) { { 259, 1128, -128, 96 }, 90, { 30, 0 }, .boxes = { 0 } };
    p->hpAnim.frames[3] = (AnimationFrame) { { 130, 1128, -128, 96 }, 90, { 30, 0 }, .boxes = { 0 } };
    p->hpAnim.frames[4] = (AnimationFrame) { {   1, 1128, -128, 96 }, 90, { 30, 0 }, .boxes = { 0 } };
    
    p->lkAnim.frameCount = 3;
    p->lkAnim.currentFrame = 0;
    p->lkAnim.frameTimeCounter = 0.0f;
    p->lkAnim.stopAtLastFrame = false;
    p->lkAnim.runOnce = true;
    p->lkAnim.finished = false;
    createAnimationFrames( &p->lkAnim, p->lkAnim.frameCount );
    p->lkAnim.frames[0] = (AnimationFrame) { {   1, 2001, -128, 96 }, 60, { 0, -5 }, .boxes = { 0 } };
    p->lkAnim.frames[1] = (AnimationFrame) { { 130, 2001, -128, 96 }, 60, { 0, -5 }, .boxes = { 0 } };
    p->lkAnim.frames[2] = (AnimationFrame) { {   1, 2001, -128, 96 }, 60, { 0, -5 }, .boxes = { 0 } };
    
    p->mkAnim.frameCount = 3;
    p->mkAnim.currentFrame = 0;
    p->mkAnim.frameTimeCounter = 0.0f;
    p->mkAnim.stopAtLastFrame = false;
    p->mkAnim.runOnce = true;
    p->mkAnim.finished = false;
    createAnimationFrames( &p->mkAnim, p->mkAnim.frameCount );
    p->mkAnim.frames[0] = (AnimationFrame) { {   1, 2001, -128, 96 }, 90, { 0, -5 }, .boxes = { 0 } };
    p->mkAnim.frames[1] = (AnimationFrame) { { 130, 2001, -128, 96 }, 90, { 0, -5 }, .boxes = { 0 } };
    p->mkAnim.frames[2] = (AnimationFrame) { {   1, 2001, -128, 96 }, 90, { 0, -5 }, .boxes = { 0 } };

    p->hkAnim.frameCount = 5;
    p->hkAnim.currentFrame = 0;
    p->hkAnim.frameTimeCounter = 0.0f;
    p->hkAnim.stopAtLastFrame = false;
    p->hkAnim.runOnce = true;
    p->hkAnim.finished = false;
    createAnimationFrames( &p->hkAnim, p->hkAnim.frameCount );
    p->hkAnim.frames[0] = (AnimationFrame) { {   1, 2098, -128, 96 }, 90, { 20, 0 }, .boxes = { 0 } };
    p->hkAnim.frames[1] = (AnimationFrame) { { 130, 2098, -128, 96 }, 90, { 20, 0 }, .boxes = { 0 } };
    p->hkAnim.frames[2] = (AnimationFrame) { { 259, 2098, -128, 96 }, 90, { 20, 0 }, .boxes = { 0 } };
    p->hkAnim.frames[3] = (AnimationFrame) { { 388, 2098, -128, 96 }, 90, { 20, 0 }, .boxes = { 0 } };
    p->hkAnim.frames[4] = (AnimationFrame) { { 517, 2098, -128, 96 }, 90, { 20, 0 }, .boxes = { 0 } };

    p->lpCloseAnim.frameCount = 1;
    p->lpCloseAnim.currentFrame = 0;
    p->lpCloseAnim.frameTimeCounter = 0.0f;
    p->lpCloseAnim.stopAtLastFrame = false;
    p->lpCloseAnim.runOnce = true;
    p->lpCloseAnim.finished = false;
    createAnimationFrames( &p->lpCloseAnim, p->lpCloseAnim.frameCount );
    p->lpCloseAnim.frames[0] = (AnimationFrame) { { 1, 1225, -80, 112 }, 150, { 5, 0 }, .boxes = { 0 } };
    
    p->mpCloseAnim.frameCount = 5;
    p->mpCloseAnim.currentFrame = 0;
    p->mpCloseAnim.frameTimeCounter = 0.0f;
    p->mpCloseAnim.stopAtLastFrame = false;
    p->mpCloseAnim.runOnce = true;
    p->mpCloseAnim.finished = false;
    createAnimationFrames( &p->mpCloseAnim, p->mpCloseAnim.frameCount );
    p->mpCloseAnim.frames[0] = (AnimationFrame) { {   1, 1338, -96, 96 }, 65, { 7, -1 }, .boxes = { 0 } };
    p->mpCloseAnim.frames[1] = (AnimationFrame) { {  98, 1338, -96, 96 }, 65, { 7, -1 }, .boxes = { 0 } };
    p->mpCloseAnim.frames[2] = (AnimationFrame) { { 195, 1338, -96, 96 }, 65, { 7, -1 }, .boxes = { 0 } };
    p->mpCloseAnim.frames[3] = (AnimationFrame) { {  98, 1338, -96, 96 }, 65, { 7, -1 }, .boxes = { 0 } };
    p->mpCloseAnim.frames[4] = (AnimationFrame) { {   1, 1338, -96, 96 }, 65, { 7, -1 }, .boxes = { 0 } };

    p->hpCloseAnim.frameCount = 5;
    p->hpCloseAnim.currentFrame = 0;
    p->hpCloseAnim.frameTimeCounter = 0.0f;
    p->hpCloseAnim.stopAtLastFrame = false;
    p->hpCloseAnim.runOnce = true;
    p->hpCloseAnim.finished = false;
    createAnimationFrames( &p->hpCloseAnim, p->hpCloseAnim.frameCount );
    p->hpCloseAnim.frames[0] = (AnimationFrame) { {   1, 1435, -112, 128 }, 65, { 20, 0 }, .boxes = { 0 } };
    p->hpCloseAnim.frames[1] = (AnimationFrame) { { 114, 1435, -112, 128 }, 65, { 20, 0 }, .boxes = { 0 } };
    p->hpCloseAnim.frames[2] = (AnimationFrame) { { 227, 1435, -112, 128 }, 65, { 20, 0 }, .boxes = { 0 } };
    p->hpCloseAnim.frames[3] = (AnimationFrame) { { 114, 1435, -112, 128 }, 65, { 20, 0 }, .boxes = { 0 } };
    p->hpCloseAnim.frames[4] = (AnimationFrame) { {   1, 1435, -112, 128 }, 65, { 20, 0 }, .boxes = { 0 } };
    
    p->lkCloseAnim.frameCount = 3;
    p->lkCloseAnim.currentFrame = 0;
    p->lkCloseAnim.frameTimeCounter = 0.0f;
    p->lkCloseAnim.stopAtLastFrame = false;
    p->lkCloseAnim.runOnce = true;
    p->lkCloseAnim.finished = false;
    createAnimationFrames( &p->lkCloseAnim, p->lkCloseAnim.frameCount );
    p->lkCloseAnim.frames[0] = (AnimationFrame) { {  1, 2195, -96, 96 },  60, { 42, 0 }, .boxes = { 0 } };
    p->lkCloseAnim.frames[1] = (AnimationFrame) { { 98, 2195, -96, 96 }, 120, { 42, 0 }, .boxes = { 0 } };
    p->lkCloseAnim.frames[2] = (AnimationFrame) { {  1, 2195, -96, 96 },  60, { 42, 0 }, .boxes = { 0 } };
    
    p->mkCloseAnim.frameCount = 3;
    p->mkCloseAnim.currentFrame = 0;
    p->mkCloseAnim.frameTimeCounter = 0.0f;
    p->mkCloseAnim.stopAtLastFrame = false;
    p->mkCloseAnim.runOnce = true;
    p->mkCloseAnim.finished = false;
    createAnimationFrames( &p->mkCloseAnim, p->mkCloseAnim.frameCount );
    p->mkCloseAnim.frames[0] = (AnimationFrame) { {  1, 2292, -80, 112 },  60, { 30, 0 }, .boxes = { 0 } };
    p->mkCloseAnim.frames[1] = (AnimationFrame) { { 82, 2292, -80, 112 }, 120, { 30, 0 }, .boxes = { 0 } };
    p->mkCloseAnim.frames[2] = (AnimationFrame) { {  1, 2292, -80, 112 },  60, { 30, 0 }, .boxes = { 0 } };

    p->hkCloseAnim.frameCount = 4;
    p->hkCloseAnim.currentFrame = 0;
    p->hkCloseAnim.frameTimeCounter = 0.0f;
    p->hkCloseAnim.stopAtLastFrame = false;
    p->hkCloseAnim.runOnce = true;
    p->hkCloseAnim.finished = false;
    createAnimationFrames( &p->hkCloseAnim, p->hkCloseAnim.frameCount );
    p->hkCloseAnim.frames[0] = (AnimationFrame) { {   1, 2405, -112, 128 },  60, { 47, 0 }, .boxes = { 0 } };
    p->hkCloseAnim.frames[1] = (AnimationFrame) { { 114, 2405, -112, 128 }, 120, { 47, 0 }, .boxes = { 0 } };
    p->hkCloseAnim.frames[2] = (AnimationFrame) { { 227, 2405, -112, 128 }, 120, { 47, 0 }, .boxes = { 0 } };
    p->hkCloseAnim.frames[3] = (AnimationFrame) { {   1, 2405, -112, 128 },  60, { 47, 0 }, .boxes = { 0 } };

    p->lpCrouchAnim.frameCount = 2;
    p->lpCrouchAnim.currentFrame = 0;
    p->lpCrouchAnim.frameTimeCounter = 0.0f;
    p->lpCrouchAnim.stopAtLastFrame = false;
    p->lpCrouchAnim.runOnce = true;
    p->lpCrouchAnim.finished = false;
    createAnimationFrames( &p->lpCrouchAnim, p->lpCrouchAnim.frameCount );
    p->lpCrouchAnim.frames[0] = (AnimationFrame) { {   1, 1564, -112, 64 }, 60, { 16, 0 }, .boxes = { 0 } };
    p->lpCrouchAnim.frames[1] = (AnimationFrame) { { 114, 1564, -112, 64 }, 60, { 16, 0 }, .boxes = { 0 } };

    p->mpCrouchAnim.frameCount = 3;
    p->mpCrouchAnim.currentFrame = 0;
    p->mpCrouchAnim.frameTimeCounter = 0.0f;
    p->mpCrouchAnim.stopAtLastFrame = false;
    p->mpCrouchAnim.runOnce = true;
    p->mpCrouchAnim.finished = false;
    createAnimationFrames( &p->mpCrouchAnim, p->mpCrouchAnim.frameCount );
    p->mpCrouchAnim.frames[0] = (AnimationFrame) { {   1, 1629, -112, 64 }, 60, { 16, 0 }, .boxes = { 0 } };
    p->mpCrouchAnim.frames[1] = (AnimationFrame) { { 114, 1629, -112, 64 }, 60, { 16, 0 }, .boxes = { 0 } };
    p->mpCrouchAnim.frames[2] = (AnimationFrame) { { 227, 1629, -112, 64 }, 60, { 16, 0 }, .boxes = { 0 } };

    p->hpCrouchAnim.frameCount = 3;
    p->hpCrouchAnim.currentFrame = 0;
    p->hpCrouchAnim.frameTimeCounter = 0.0f;
    p->hpCrouchAnim.stopAtLastFrame = false;
    p->hpCrouchAnim.runOnce = true;
    p->hpCrouchAnim.finished = false;
    createAnimationFrames( &p->hpCrouchAnim, p->hpCrouchAnim.frameCount );
    p->hpCrouchAnim.frames[0] = (AnimationFrame) { {   1, 1694, -96, 128 }, 60, { 8, 0 }, .boxes = { 0 } };
    p->hpCrouchAnim.frames[1] = (AnimationFrame) { {  98, 1694, -96, 128 }, 60, { 8, 0 }, .boxes = { 0 } };
    p->hpCrouchAnim.frames[2] = (AnimationFrame) { { 195, 1694, -96, 128 }, 60, { 8, 0 }, .boxes = { 0 } };

    p->lkCrouchAnim.frameCount = 1;
    p->lkCrouchAnim.currentFrame = 0;
    p->lkCrouchAnim.frameTimeCounter = 0.0f;
    p->lkCrouchAnim.stopAtLastFrame = false;
    p->lkCrouchAnim.runOnce = true;
    p->lkCrouchAnim.finished = false;
    createAnimationFrames( &p->lkCrouchAnim, p->lkCrouchAnim.frameCount );
    p->lkCrouchAnim.frames[0] = (AnimationFrame) { { 1, 2534, -160, 64 }, 60, { 40, 0 }, .boxes = { 0 } };

    p->mkCrouchAnim.frameCount = 2;
    p->mkCrouchAnim.currentFrame = 0;
    p->mkCrouchAnim.frameTimeCounter = 0.0f;
    p->mkCrouchAnim.stopAtLastFrame = false;
    p->mkCrouchAnim.runOnce = true;
    p->mkCrouchAnim.finished = false;
    createAnimationFrames( &p->mkCrouchAnim, p->mkCrouchAnim.frameCount );
    p->mkCrouchAnim.frames[0] = (AnimationFrame) { {   1, 2534, -160, 64 }, 60, { 40, 0 }, .boxes = { 0 } };
    p->mkCrouchAnim.frames[1] = (AnimationFrame) { { 162, 2534, -160, 64 }, 60, { 40, 0 }, .boxes = { 0 } };

    p->hkCrouchAnim.frameCount = 4;
    p->hkCrouchAnim.currentFrame = 0;
    p->hkCrouchAnim.frameTimeCounter = 0.0f;
    p->hkCrouchAnim.stopAtLastFrame = false;
    p->hkCrouchAnim.runOnce = true;
    p->hkCrouchAnim.finished = false;
    createAnimationFrames( &p->hkCrouchAnim, p->hkCrouchAnim.frameCount );
    p->hkCrouchAnim.frames[0] = (AnimationFrame) { {   1, 2599, -144, 64 }, 60, { 32, 0 }, .boxes = { 0 } };
    p->hkCrouchAnim.frames[1] = (AnimationFrame) { { 146, 2599, -144, 64 }, 60, { 32, 0 }, .boxes = { 0 } };
    p->hkCrouchAnim.frames[2] = (AnimationFrame) { { 291, 2599, -144, 64 }, 60, { 32, 0 }, .boxes = { 0 } };
    p->hkCrouchAnim.frames[3] = (AnimationFrame) { { 436, 2599, -144, 64 }, 60, { 32, 0 }, .boxes = { 0 } };

    p->lpJumpStraightAnim.frameCount = 2;
    p->lpJumpStraightAnim.currentFrame = 0;
    p->lpJumpStraightAnim.frameTimeCounter = 0.0f;
    p->lpJumpStraightAnim.stopAtLastFrame = false;
    p->lpJumpStraightAnim.runOnce = true;
    p->lpJumpStraightAnim.finished = false;
    createAnimationFrames( &p->lpJumpStraightAnim, p->lpJumpStraightAnim.frameCount );
    p->lpJumpStraightAnim.frames[0] = (AnimationFrame) { {  1, 1823, -96, 80 },  60, { 8, 0 }, .boxes = { 0 } };
    p->lpJumpStraightAnim.frames[1] = (AnimationFrame) { { 98, 1823, -96, 80 }, 100, { 8, 0 }, .boxes = { 0 } };

    p->mpJumpStraightAnim.frameCount = 2;
    p->mpJumpStraightAnim.currentFrame = 0;
    p->mpJumpStraightAnim.frameTimeCounter = 0.0f;
    p->mpJumpStraightAnim.stopAtLastFrame = false;
    p->mpJumpStraightAnim.runOnce = true;
    p->mpJumpStraightAnim.finished = false;
    createAnimationFrames( &p->mpJumpStraightAnim, p->mpJumpStraightAnim.frameCount );
    p->mpJumpStraightAnim.frames[0] = (AnimationFrame) { {  1, 1904, -96, 80 },  60, { 8, 0 }, .boxes = { 0 } };
    p->mpJumpStraightAnim.frames[1] = (AnimationFrame) { { 98, 1904, -96, 80 }, 100, { 8, 0 }, .boxes = { 0 } };

    p->hpJumpStraightAnim.frameCount = 2;
    p->hpJumpStraightAnim.currentFrame = 0;
    p->hpJumpStraightAnim.frameTimeCounter = 0.0f;
    p->hpJumpStraightAnim.stopAtLastFrame = false;
    p->hpJumpStraightAnim.runOnce = true;
    p->hpJumpStraightAnim.finished = false;
    createAnimationFrames( &p->hpJumpStraightAnim, p->hpJumpStraightAnim.frameCount );
    p->hpJumpStraightAnim.frames[0] = (AnimationFrame) { {  1, 1904, -96, 80 },  60, { 8, 0 }, .boxes = { 0 } };
    p->hpJumpStraightAnim.frames[1] = (AnimationFrame) { { 98, 1904, -96, 80 }, 100, { 8, 0 }, .boxes = { 0 } };

    p->lkJumpStraightAnim.frameCount = 1;
    p->lkJumpStraightAnim.currentFrame = 0;
    p->lkJumpStraightAnim.frameTimeCounter = 0.0f;
    p->lkJumpStraightAnim.stopAtLastFrame = false;
    p->lkJumpStraightAnim.runOnce = true;
    p->lkJumpStraightAnim.finished = false;
    createAnimationFrames( &p->lkJumpStraightAnim, p->lkJumpStraightAnim.frameCount );
    p->lkJumpStraightAnim.frames[0] = (AnimationFrame) { { 1, 2664, -80, 96 }, 100, { 0, 0 }, .boxes = { 0 } };

    p->mkJumpStraightAnim.frameCount = 1;
    p->mkJumpStraightAnim.currentFrame = 0;
    p->mkJumpStraightAnim.frameTimeCounter = 0.0f;
    p->mkJumpStraightAnim.stopAtLastFrame = false;
    p->mkJumpStraightAnim.runOnce = true;
    p->mkJumpStraightAnim.finished = false;
    createAnimationFrames( &p->mkJumpStraightAnim, p->mkJumpStraightAnim.frameCount );
    p->mkJumpStraightAnim.frames[0] = (AnimationFrame) { { 1, 2664, -80, 96 }, 100, { 0, 0 }, .boxes = { 0 } };

    p->hkJumpStraightAnim.frameCount = 4;
    p->hkJumpStraightAnim.currentFrame = 0;
    p->hkJumpStraightAnim.frameTimeCounter = 0.0f;
    p->hkJumpStraightAnim.stopAtLastFrame = false;
    p->hkJumpStraightAnim.runOnce = true;
    p->hkJumpStraightAnim.finished = false;
    createAnimationFrames( &p->hkJumpStraightAnim, p->hkJumpStraightAnim.frameCount );
    p->hkJumpStraightAnim.frames[0] = (AnimationFrame) { {   1, 2761, -96, 112 }, 60, { 8, 0 }, .boxes = { 0 } };
    p->hkJumpStraightAnim.frames[1] = (AnimationFrame) { {  98, 2761, -96, 112 }, 60, { 8, 0 }, .boxes = { 0 } };
    p->hkJumpStraightAnim.frames[2] = (AnimationFrame) { { 195, 2761, -96, 112 }, 60, { 8, 0 }, .boxes = { 0 } };
    p->hkJumpStraightAnim.frames[3] = (AnimationFrame) { { 292, 2761, -96, 112 }, 60, { 8, 0 }, .boxes = { 0 } };

    p->lpJumpForwardAnim.frameCount = 2;
    p->lpJumpForwardAnim.currentFrame = 0;
    p->lpJumpForwardAnim.frameTimeCounter = 0.0f;
    p->lpJumpForwardAnim.stopAtLastFrame = false;
    p->lpJumpForwardAnim.runOnce = true;
    p->lpJumpForwardAnim.finished = false;
    createAnimationFrames( &p->lpJumpForwardAnim, p->lpJumpForwardAnim.frameCount );
    p->lpJumpForwardAnim.frames[0] = (AnimationFrame) { {  1, 1823, -96, 80 },  60, { 8, 0 }, .boxes = { 0 } };
    p->lpJumpForwardAnim.frames[1] = (AnimationFrame) { { 98, 1823, -96, 80 }, 100, { 8, 0 }, .boxes = { 0 } };

    p->mpJumpForwardAnim.frameCount = 2;
    p->mpJumpForwardAnim.currentFrame = 0;
    p->mpJumpForwardAnim.frameTimeCounter = 0.0f;
    p->mpJumpForwardAnim.stopAtLastFrame = false;
    p->mpJumpForwardAnim.runOnce = true;
    p->mpJumpForwardAnim.finished = false;
    createAnimationFrames( &p->mpJumpForwardAnim, p->mpJumpForwardAnim.frameCount );
    p->mpJumpForwardAnim.frames[0] = (AnimationFrame) { {  1, 1904, -96, 80 },  60, { 8, 0 }, .boxes = { 0 } };
    p->mpJumpForwardAnim.frames[1] = (AnimationFrame) { { 98, 1904, -96, 80 }, 100, { 8, 0 }, .boxes = { 0 } };

    p->hpJumpForwardAnim.frameCount = 2;
    p->hpJumpForwardAnim.currentFrame = 0;
    p->hpJumpForwardAnim.frameTimeCounter = 0.0f;
    p->hpJumpForwardAnim.stopAtLastFrame = false;
    p->hpJumpForwardAnim.runOnce = true;
    p->hpJumpForwardAnim.finished = false;
    createAnimationFrames( &p->hpJumpForwardAnim, p->hpJumpForwardAnim.frameCount );
    p->hpJumpForwardAnim.frames[0] = (AnimationFrame) { {  1, 1904, -96, 80 },  60, { 8, 0 }, .boxes = { 0 } };
    p->hpJumpForwardAnim.frames[1] = (AnimationFrame) { { 98, 1904, -96, 80 }, 100, { 8, 0 }, .boxes = { 0 } };

    p->lkJumpForwardAnim.frameCount = 3;
    p->lkJumpForwardAnim.currentFrame = 0;
    p->lkJumpForwardAnim.frameTimeCounter = 0.0f;
    p->lkJumpForwardAnim.stopAtLastFrame = false;
    p->lkJumpForwardAnim.runOnce = true;
    p->lkJumpForwardAnim.finished = false;
    createAnimationFrames( &p->lkJumpForwardAnim, p->lkJumpForwardAnim.frameCount );
    p->lkJumpForwardAnim.frames[0] = (AnimationFrame) { {   1, 2874, -80, 80 }, 60, { 0, 0 }, .boxes = { 0 } };
    p->lkJumpForwardAnim.frames[1] = (AnimationFrame) { {  82, 2874, -80, 80 }, 60, { 0, 0 }, .boxes = { 0 } };
    p->lkJumpForwardAnim.frames[2] = (AnimationFrame) { { 163, 2874, -80, 80 }, 60, { 0, 0 }, .boxes = { 0 } };

    p->mkJumpForwardAnim.frameCount = 2;
    p->mkJumpForwardAnim.currentFrame = 0;
    p->mkJumpForwardAnim.frameTimeCounter = 0.0f;
    p->mkJumpForwardAnim.stopAtLastFrame = false;
    p->mkJumpForwardAnim.runOnce = true;
    p->mkJumpForwardAnim.finished = false;
    createAnimationFrames( &p->mkJumpForwardAnim, p->mkJumpForwardAnim.frameCount );
    p->mkJumpForwardAnim.frames[0] = (AnimationFrame) { {   1, 2955, -128, 80 },  60, { 24, 0 }, .boxes = { 0 } };
    p->mkJumpForwardAnim.frames[1] = (AnimationFrame) { { 130, 2955, -128, 80 }, 120, { 24, 0 }, .boxes = { 0 } };

    p->hkJumpForwardAnim.frameCount = 2;
    p->hkJumpForwardAnim.currentFrame = 0;
    p->hkJumpForwardAnim.frameTimeCounter = 0.0f;
    p->hkJumpForwardAnim.stopAtLastFrame = false;
    p->hkJumpForwardAnim.runOnce = true;
    p->hkJumpForwardAnim.finished = false;
    createAnimationFrames( &p->hkJumpForwardAnim, p->hkJumpForwardAnim.frameCount );
    p->hkJumpForwardAnim.frames[0] = (AnimationFrame) { {   1, 2955, -128, 80 },  60, { 24, 0 }, .boxes = { 0 } };
    p->hkJumpForwardAnim.frames[1] = (AnimationFrame) { { 130, 2955, -128, 80 }, 120, { 24, 0 }, .boxes = { 0 } };

    p->lpJumpBackwardAnim.frameCount = 2;
    p->lpJumpBackwardAnim.currentFrame = 0;
    p->lpJumpBackwardAnim.frameTimeCounter = 0.0f;
    p->lpJumpBackwardAnim.stopAtLastFrame = false;
    p->lpJumpBackwardAnim.runOnce = true;
    p->lpJumpBackwardAnim.finished = false;
    createAnimationFrames( &p->lpJumpBackwardAnim, p->lpJumpBackwardAnim.frameCount );
    p->lpJumpBackwardAnim.frames[0] = (AnimationFrame) { {  1, 1823, -96, 80 },  60, { 8, 0 }, .boxes = { 0 } };
    p->lpJumpBackwardAnim.frames[1] = (AnimationFrame) { { 98, 1823, -96, 80 }, 100, { 8, 0 }, .boxes = { 0 } };

    p->mpJumpBackwardAnim.frameCount = 2;
    p->mpJumpBackwardAnim.currentFrame = 0;
    p->mpJumpBackwardAnim.frameTimeCounter = 0.0f;
    p->mpJumpBackwardAnim.stopAtLastFrame = false;
    p->mpJumpBackwardAnim.runOnce = true;
    p->mpJumpBackwardAnim.finished = false;
    createAnimationFrames( &p->mpJumpBackwardAnim, p->mpJumpBackwardAnim.frameCount );
    p->mpJumpBackwardAnim.frames[0] = (AnimationFrame) { {  1, 1904, -96, 80 },  60, { 8, 0 }, .boxes = { 0 } };
    p->mpJumpBackwardAnim.frames[1] = (AnimationFrame) { { 98, 1904, -96, 80 }, 100, { 8, 0 }, .boxes = { 0 } };

    p->hpJumpBackwardAnim.frameCount = 2;
    p->hpJumpBackwardAnim.currentFrame = 0;
    p->hpJumpBackwardAnim.frameTimeCounter = 0.0f;
    p->hpJumpBackwardAnim.stopAtLastFrame = false;
    p->hpJumpBackwardAnim.runOnce = true;
    p->hpJumpBackwardAnim.finished = false;
    createAnimationFrames( &p->hpJumpBackwardAnim, p->hpJumpBackwardAnim.frameCount );
    p->hpJumpBackwardAnim.frames[0] = (AnimationFrame) { {  1, 1904, -96, 80 },  60, { 8, 0 }, .boxes = { 0 } };
    p->hpJumpBackwardAnim.frames[1] = (AnimationFrame) { { 98, 1904, -96, 80 }, 100, { 8, 0 }, .boxes = { 0 } };

    p->lkJumpBackwardAnim.frameCount = 3;
    p->lkJumpBackwardAnim.currentFrame = 0;
    p->lkJumpBackwardAnim.frameTimeCounter = 0.0f;
    p->lkJumpBackwardAnim.stopAtLastFrame = false;
    p->lkJumpBackwardAnim.runOnce = true;
    p->lkJumpBackwardAnim.finished = false;
    createAnimationFrames( &p->lkJumpBackwardAnim, p->lkJumpBackwardAnim.frameCount );
    p->lkJumpBackwardAnim.frames[0] = (AnimationFrame) { {   1, 2874, -80, 80 }, 60, { 0, 0 }, .boxes = { 0 } };
    p->lkJumpBackwardAnim.frames[1] = (AnimationFrame) { {  82, 2874, -80, 80 }, 60, { 0, 0 }, .boxes = { 0 } };
    p->lkJumpBackwardAnim.frames[2] = (AnimationFrame) { { 163, 2874, -80, 80 }, 60, { 0, 0 }, .boxes = { 0 } };

    p->mkJumpBackwardAnim.frameCount = 2;
    p->mkJumpBackwardAnim.currentFrame = 0;
    p->mkJumpBackwardAnim.frameTimeCounter = 0.0f;
    p->mkJumpBackwardAnim.stopAtLastFrame = false;
    p->mkJumpBackwardAnim.runOnce = true;
    p->mkJumpBackwardAnim.finished = false;
    createAnimationFrames( &p->mkJumpBackwardAnim, p->mkJumpBackwardAnim.frameCount );
    p->mkJumpBackwardAnim.frames[0] = (AnimationFrame) { {   1, 2955, -128, 80 },  60, { 24, 0 }, .boxes = { 0 } };
    p->mkJumpBackwardAnim.frames[1] = (AnimationFrame) { { 130, 2955, -128, 80 }, 120, { 24, 0 }, .boxes = { 0 } };

    p->hkJumpBackwardAnim.frameCount = 2;
    p->hkJumpBackwardAnim.currentFrame = 0;
    p->hkJumpBackwardAnim.frameTimeCounter = 0.0f;
    p->hkJumpBackwardAnim.stopAtLastFrame = false;
    p->hkJumpBackwardAnim.runOnce = true;
    p->hkJumpBackwardAnim.finished = false;
    createAnimationFrames( &p->hkJumpBackwardAnim, p->hkJumpBackwardAnim.frameCount );
    p->hkJumpBackwardAnim.frames[0] = (AnimationFrame) { {   1, 2955, -128, 80 },  60, { 24, 0 }, .boxes = { 0 } };
    p->hkJumpBackwardAnim.frames[1] = (AnimationFrame) { { 130, 2955, -128, 80 }, 120, { 24, 0 }, .boxes = { 0 } };

    int animationCount = 0;
    p->animations[animationCount++] = &p->idleAnim;
    p->animations[animationCount++] = &p->forwardAnim;
    p->animations[animationCount++] = &p->backwardAnim;
    p->animations[animationCount++] = &p->straightJumpAnim;
    p->animations[animationCount++] = &p->forwardJumpAnim;
    p->animations[animationCount++] = &p->backwardJumpAnim;
    p->animations[animationCount++] = &p->jumpCooldownAnim;
    p->animations[animationCount++] = &p->crouchingAnim;
    p->animations[animationCount++] = &p->lpAnim;
    p->animations[animationCount++] = &p->mpAnim;
    p->animations[animationCount++] = &p->hpAnim;
    p->animations[animationCount++] = &p->lkAnim;
    p->animations[animationCount++] = &p->mkAnim;
    p->animations[animationCount++] = &p->hkAnim;
    p->animations[animationCount++] = &p->lpCloseAnim;
    p->animations[animationCount++] = &p->mpCloseAnim;
    p->animations[animationCount++] = &p->hpCloseAnim;
    p->animations[animationCount++] = &p->lkCloseAnim;
    p->animations[animationCount++] = &p->mkCloseAnim;
    p->animations[animationCount++] = &p->hkCloseAnim;
    p->animations[animationCount++] = &p->lpCrouchAnim;
    p->animations[animationCount++] = &p->mpCrouchAnim;
    p->animations[animationCount++] = &p->hpCrouchAnim;
    p->animations[animationCount++] = &p->lkCrouchAnim;
    p->animations[animationCount++] = &p->mkCrouchAnim;
    p->animations[animationCount++] = &p->hkCrouchAnim;
    p->animations[animationCount++] = &p->lpJumpStraightAnim;
    p->animations[animationCount++] = &p->mpJumpStraightAnim;
    p->animations[animationCount++] = &p->hpJumpStraightAnim;
    p->animations[animationCount++] = &p->lkJumpStraightAnim;
    p->animations[animationCount++] = &p->mkJumpStraightAnim;
    p->animations[animationCount++] = &p->hkJumpStraightAnim;
    p->animations[animationCount++] = &p->lpJumpForwardAnim;
    p->animations[animationCount++] = &p->mpJumpForwardAnim;
    p->animations[animationCount++] = &p->hpJumpForwardAnim;
    p->animations[animationCount++] = &p->lkJumpForwardAnim;
    p->animations[animationCount++] = &p->mkJumpForwardAnim;
    p->animations[animationCount++] = &p->hkJumpForwardAnim;
    p->animations[animationCount++] = &p->lpJumpBackwardAnim;
    p->animations[animationCount++] = &p->mpJumpBackwardAnim;
    p->animations[animationCount++] = &p->hpJumpBackwardAnim;
    p->animations[animationCount++] = &p->lkJumpBackwardAnim;
    p->animations[animationCount++] = &p->mkJumpBackwardAnim;
    p->animations[animationCount++] = &p->hkJumpBackwardAnim;
    p->animationCount = animationCount;

}

void initializePlayerKen( float x, float y, Player *p, bool showBoxes, bool showDebugInfo ) {

    initializePlayerRyu( x, y, p, showBoxes, showDebugInfo );
    p->texture = &rm.kenTexture;

}

void destroyPlayer( Player *player ) {
    for ( int i = 0; i < player->animationCount; i++ ) {
        destroyAnimationFrames( player->animations[i] );
    }
    free( player );
}

void drawPlayer( Player *player, Camera2D *camera ) {

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
                player->pos.x - fabs( frame->source.width ) / 2 + ( player->lookingRight ? frame->offset.x : -frame->offset.x ),
                player->pos.y + player->dim.y - frame->source.height + frame->offset.y,
                frame->source.width,
                frame->source.height
            },
            (Vector2) { 0 },
            0.0f,
            WHITE
        );
    }

    if ( player->showDebugInfo ) {

        DrawCircle( player->pos.x, player->pos.y, 1, ORANGE );
        DrawRectangleLines( player->pos.x - player->dim.x / 2, player->pos.y, player->dim.x, player->dim.y, ORANGE );
        DrawText( TextFormat( "x: %.2f", player->pos.x ), player->pos.x + 5, player->pos.y - 20, 10, BLACK );
        DrawText( TextFormat( "y: %.2f", player->pos.y ), player->pos.x + 5, player->pos.y - 10, 10, BLACK );

        // states - will be removed!
        /*int pos = 0;
        Vector2 base = GetScreenToWorld2D( (Vector2){ 10, 20 }, *camera );
        float radius = 5;
        int fontSize = 4;

        DrawRectangle( base.x - 10, base.y - 10, 35, 400, Fade( BLACK, 0.5 ) );

        for ( int i = head; i <= tail; i++ ) {

            Color color = playerStateToColor( states[i%TRACK_STATE_SIZE] );

            Vector2 center = {
                base.x + 10, 
                base.y + 215 + (radius * 2 + 6) * -pos
            };

            DrawCircle( 
                center.x, 
                center.y, 
                radius + 1,
                ColorBrightness( color, -0.5 )
            );

            DrawCircle( 
                center.x, 
                center.y, 
                radius,
                color
            );

            const char *t = playerStateToText( states[i%TRACK_STATE_SIZE] );
            int w = MeasureText( t, fontSize );

            DrawText( 
                t, 
                center.x - w / 2, 
                center.y - fontSize, 
                fontSize, 
                ColorBrightness( color, -0.5 )
            );

            DrawLine( 
                center.x - 15, 
                center.y + radius + 3,
                center.x + 15,
                center.y + radius + 3,
                LIGHTGRAY
            );
            
            pos++;

        }*/
        
    }

    if ( player->showBoxes ) {
        drawPlayerAnimationFrameBoxes( player );
    }

}

static void drawPlayerAnimationFrameBoxes( Player *player ) {

    AnimationFrame *af = getPlayerCurrentAnimationFrame( player );
    int x = (int) ( player->pos.x + af->boxes.collisionBox.x );
    int y = (int) ( player->pos.y + af->boxes.collisionBox.y );
    int w = (int) af->boxes.collisionBox.width;
    int h = (int) af->boxes.collisionBox.height;

    if ( !player->lookingRight ) {
        x = (int) ( player->pos.x - af->boxes.collisionBox.x - af->boxes.collisionBox.width );
    }

    if ( !( w == 0 && h == 0 ) ) {
        DrawRectangle( x, y, w, h, Fade( GREEN, 0.4 ) );
        DrawRectangleLines( x, y, w, h, GREEN );
    }

    for ( int i = 0; i < af->boxes.hitboxCount; i++ ) {

        Rectangle *r = &af->boxes.hitboxes[i];
        int x = (int) ( player->pos.x + r->x );
        int y = (int) ( player->pos.y + r->y );
        int w = (int) r->width;
        int h = (int) r->height;

        if ( !player->lookingRight ) {
            x = (int) player->pos.x - r->x - r->width;
        }

        if ( !( w == 0 && h == 0 ) ) {
            DrawText( TextFormat( "%d", i ), x + 1, y, 5, ColorBrightness( BLUE, -0.7 ) );
            DrawRectangle( x, y, w, h, Fade( BLUE, 0.4 ) );
            DrawRectangleLines( x, y, w, h, BLUE );
        }

    }

    for ( int i = 0; i < af->boxes.hurtboxCount; i++ ) {

        Rectangle *r = &af->boxes.hurtboxes[i];
        int x = (int) ( player->pos.x + r->x );
        int y = (int) ( player->pos.y + r->y );
        int w = (int) r->width;
        int h = (int) r->height;

        if ( !player->lookingRight ) {
            x = (int) player->pos.x - r->x - r->width;
        }

        if ( !( w == 0 && h == 0 ) ) {
            DrawText( TextFormat( "%d", i ), x + 1, y, 5, ColorBrightness( RED, -0.7 ) );
            DrawRectangle( x, y, w, h, Fade( RED, 0.4 ) );
            DrawRectangleLines( x, y, w, h, RED );
        }

    }

}

void processInputPlayer( Player *player, Player *opponent, float delta ) {

    // attack in progress: blocks all input
    Animation *activeAnim = NULL;
    switch ( player->state ) {
        case PLAYER_STATE_LP: activeAnim = &player->lpAnim; break;
        case PLAYER_STATE_MP: activeAnim = &player->mpAnim; break;
        case PLAYER_STATE_HP: activeAnim = &player->hpAnim; break;
        case PLAYER_STATE_LK: activeAnim = &player->lkAnim; break;
        case PLAYER_STATE_MK: activeAnim = &player->mkAnim; break;
        case PLAYER_STATE_HK: activeAnim = &player->hkAnim; break;
        case PLAYER_STATE_LP_CLOSE: activeAnim = &player->lpCloseAnim; break;
        case PLAYER_STATE_MP_CLOSE: activeAnim = &player->mpCloseAnim; break;
        case PLAYER_STATE_HP_CLOSE: activeAnim = &player->hpCloseAnim; break;
        case PLAYER_STATE_LK_CLOSE: activeAnim = &player->lkCloseAnim; break;
        case PLAYER_STATE_MK_CLOSE: activeAnim = &player->mkCloseAnim; break;
        case PLAYER_STATE_HK_CLOSE: activeAnim = &player->hkCloseAnim; break;
        case PLAYER_STATE_LP_CROUCH: activeAnim = &player->lpCrouchAnim; break;
        case PLAYER_STATE_MP_CROUCH: activeAnim = &player->mpCrouchAnim; break;
        case PLAYER_STATE_HP_CROUCH: activeAnim = &player->hpCrouchAnim; break;
        case PLAYER_STATE_LK_CROUCH: activeAnim = &player->lkCrouchAnim; break;
        case PLAYER_STATE_MK_CROUCH: activeAnim = &player->mkCrouchAnim; break;
        case PLAYER_STATE_HK_CROUCH: activeAnim = &player->hkCrouchAnim; break;
        case PLAYER_STATE_LP_JUMP_STRAIGHT: activeAnim = &player->lpJumpStraightAnim; break;
        case PLAYER_STATE_MP_JUMP_STRAIGHT: activeAnim = &player->mpJumpStraightAnim; break;
        case PLAYER_STATE_HP_JUMP_STRAIGHT: activeAnim = &player->hpJumpStraightAnim; break;
        case PLAYER_STATE_LK_JUMP_STRAIGHT: activeAnim = &player->lkJumpStraightAnim; break;
        case PLAYER_STATE_MK_JUMP_STRAIGHT: activeAnim = &player->mkJumpStraightAnim; break;
        case PLAYER_STATE_HK_JUMP_STRAIGHT: activeAnim = &player->hkJumpStraightAnim; break;
        case PLAYER_STATE_LP_JUMP_FORWARD: activeAnim = &player->lpJumpForwardAnim; break;
        case PLAYER_STATE_MP_JUMP_FORWARD: activeAnim = &player->mpJumpForwardAnim; break;
        case PLAYER_STATE_HP_JUMP_FORWARD: activeAnim = &player->hpJumpForwardAnim; break;
        case PLAYER_STATE_LK_JUMP_FORWARD: activeAnim = &player->lkJumpForwardAnim; break;
        case PLAYER_STATE_MK_JUMP_FORWARD: activeAnim = &player->mkJumpForwardAnim; break;
        case PLAYER_STATE_HK_JUMP_FORWARD: activeAnim = &player->hkJumpForwardAnim; break;
        case PLAYER_STATE_LP_JUMP_BACKWARD: activeAnim = &player->lpJumpBackwardAnim; break;
        case PLAYER_STATE_MP_JUMP_BACKWARD: activeAnim = &player->mpJumpBackwardAnim; break;
        case PLAYER_STATE_HP_JUMP_BACKWARD: activeAnim = &player->hpJumpBackwardAnim; break;
        case PLAYER_STATE_LK_JUMP_BACKWARD: activeAnim = &player->lkJumpBackwardAnim; break;
        case PLAYER_STATE_MK_JUMP_BACKWARD: activeAnim = &player->mkJumpBackwardAnim; break;
        case PLAYER_STATE_HK_JUMP_BACKWARD: activeAnim = &player->hkJumpBackwardAnim; break;
        default: break;
    }

    if ( activeAnim != NULL ) {
        updateAnimation( activeAnim, delta );
        if ( activeAnim->finished ) {
            bool isCrouchAttack = (
                player->state == PLAYER_STATE_LP_CROUCH ||
                player->state == PLAYER_STATE_MP_CROUCH ||
                player->state == PLAYER_STATE_HP_CROUCH ||
                player->state == PLAYER_STATE_LK_CROUCH ||
                player->state == PLAYER_STATE_MK_CROUCH ||
                player->state == PLAYER_STATE_HK_CROUCH
            );
            bool isJumpAttack = (
                player->state == PLAYER_STATE_LP_JUMP_STRAIGHT ||
                player->state == PLAYER_STATE_MP_JUMP_STRAIGHT ||
                player->state == PLAYER_STATE_HP_JUMP_STRAIGHT ||
                player->state == PLAYER_STATE_LK_JUMP_STRAIGHT ||
                player->state == PLAYER_STATE_MK_JUMP_STRAIGHT ||
                player->state == PLAYER_STATE_HK_JUMP_STRAIGHT ||
                player->state == PLAYER_STATE_LP_JUMP_FORWARD  ||
                player->state == PLAYER_STATE_MP_JUMP_FORWARD  ||
                player->state == PLAYER_STATE_HP_JUMP_FORWARD  ||
                player->state == PLAYER_STATE_LK_JUMP_FORWARD  ||
                player->state == PLAYER_STATE_MK_JUMP_FORWARD  ||
                player->state == PLAYER_STATE_HK_JUMP_FORWARD  ||
                player->state == PLAYER_STATE_LP_JUMP_BACKWARD ||
                player->state == PLAYER_STATE_MP_JUMP_BACKWARD ||
                player->state == PLAYER_STATE_HP_JUMP_BACKWARD ||
                player->state == PLAYER_STATE_LK_JUMP_BACKWARD ||
                player->state == PLAYER_STATE_MK_JUMP_BACKWARD ||
                player->state == PLAYER_STATE_HK_JUMP_BACKWARD
            );
            if ( isCrouchAttack ) {
                player->state = PLAYER_STATE_CROUCHING;
            } else if ( isJumpAttack ) {
                player->state = player->lastState;
            } else {
                player->state = PLAYER_STATE_IDLE;
            }
            resetAnimation( activeAnim );
        }
        return;
    }

    // jump in progress: updates animation and blocks input
    if ( player->state == PLAYER_STATE_JUMPING_STRAIGHT ||
         player->state == PLAYER_STATE_JUMPING_FORWARD  ||
         player->state == PLAYER_STATE_JUMPING_BACKWARD ) {
        switch ( player->state ) {
            case PLAYER_STATE_JUMPING_STRAIGHT:
                updateAnimation( &player->straightJumpAnim, delta );
                break;
            case PLAYER_STATE_JUMPING_FORWARD:
                if ( player->lookingRight ) {
                    updateAnimation( &player->forwardJumpAnim, delta );
                } else {
                    updateAnimation( &player->backwardJumpAnim, delta );
                }
                break;
            case PLAYER_STATE_JUMPING_BACKWARD:
                if ( player->lookingRight ) {
                    updateAnimation( &player->backwardJumpAnim, delta );
                } else {
                    updateAnimation( &player->forwardJumpAnim, delta );
                }
                break;
            default:
                break;
        }
        // jump attack input
        PlayerState jumpAttackState = PLAYER_STATE_IDLE;
        Animation *jumpAttackAnim = NULL;

        if ( IsKeyPressed( player->kb.lp ) ) {
            if ( player->state == PLAYER_STATE_JUMPING_STRAIGHT ) {
                jumpAttackState = PLAYER_STATE_LP_JUMP_STRAIGHT;
                jumpAttackAnim = &player->lpJumpStraightAnim;
            } else if ( player->state == PLAYER_STATE_JUMPING_FORWARD ) {
                jumpAttackState = PLAYER_STATE_LP_JUMP_FORWARD;
                jumpAttackAnim = &player->lpJumpForwardAnim;
            } else {
                jumpAttackState = PLAYER_STATE_LP_JUMP_BACKWARD;
                jumpAttackAnim = &player->lpJumpBackwardAnim;
            }
        } else if ( IsKeyPressed( player->kb.mp ) ) {
            if ( player->state == PLAYER_STATE_JUMPING_STRAIGHT ) {
                jumpAttackState = PLAYER_STATE_MP_JUMP_STRAIGHT;
                jumpAttackAnim = &player->mpJumpStraightAnim;
            } else if ( player->state == PLAYER_STATE_JUMPING_FORWARD ) {
                jumpAttackState = PLAYER_STATE_MP_JUMP_FORWARD;
                jumpAttackAnim = &player->mpJumpForwardAnim;
            } else {
                jumpAttackState = PLAYER_STATE_MP_JUMP_BACKWARD;
                jumpAttackAnim = &player->mpJumpBackwardAnim;
            }
        } else if ( IsKeyPressed( player->kb.hp ) ) {
            if ( player->state == PLAYER_STATE_JUMPING_STRAIGHT ) {
                jumpAttackState = PLAYER_STATE_HP_JUMP_STRAIGHT;
                jumpAttackAnim = &player->hpJumpStraightAnim;
            } else if ( player->state == PLAYER_STATE_JUMPING_FORWARD ) {
                jumpAttackState = PLAYER_STATE_HP_JUMP_FORWARD;
                jumpAttackAnim = &player->hpJumpForwardAnim;
            } else {
                jumpAttackState = PLAYER_STATE_HP_JUMP_BACKWARD;
                jumpAttackAnim = &player->hpJumpBackwardAnim;
            }
        } else if ( IsKeyPressed( player->kb.lk ) ) {
            if ( player->state == PLAYER_STATE_JUMPING_STRAIGHT ) {
                jumpAttackState = PLAYER_STATE_LK_JUMP_STRAIGHT;
                jumpAttackAnim = &player->lkJumpStraightAnim;
            } else if ( player->state == PLAYER_STATE_JUMPING_FORWARD ) {
                jumpAttackState = PLAYER_STATE_LK_JUMP_FORWARD;
                jumpAttackAnim = &player->lkJumpForwardAnim;
            } else {
                jumpAttackState = PLAYER_STATE_LK_JUMP_BACKWARD;
                jumpAttackAnim = &player->lkJumpBackwardAnim;
            }
        } else if ( IsKeyPressed( player->kb.mk ) ) {
            if ( player->state == PLAYER_STATE_JUMPING_STRAIGHT ) {
                jumpAttackState = PLAYER_STATE_MK_JUMP_STRAIGHT;
                jumpAttackAnim = &player->mkJumpStraightAnim;
            } else if ( player->state == PLAYER_STATE_JUMPING_FORWARD ) {
                jumpAttackState = PLAYER_STATE_MK_JUMP_FORWARD;
                jumpAttackAnim = &player->mkJumpForwardAnim;
            } else {
                jumpAttackState = PLAYER_STATE_MK_JUMP_BACKWARD;
                jumpAttackAnim = &player->mkJumpBackwardAnim;
            }
        } else if ( IsKeyPressed( player->kb.hk ) ) {
            if ( player->state == PLAYER_STATE_JUMPING_STRAIGHT ) {
                jumpAttackState = PLAYER_STATE_HK_JUMP_STRAIGHT;
                jumpAttackAnim = &player->hkJumpStraightAnim;
            } else if ( player->state == PLAYER_STATE_JUMPING_FORWARD ) {
                jumpAttackState = PLAYER_STATE_HK_JUMP_FORWARD;
                jumpAttackAnim = &player->hkJumpForwardAnim;
            } else {
                jumpAttackState = PLAYER_STATE_HK_JUMP_BACKWARD;
                jumpAttackAnim = &player->hkJumpBackwardAnim;
            }
        }

        if ( jumpAttackAnim != NULL ) {
            resetAnimation( jumpAttackAnim );
            player->lastState = player->state;   // preserve jump state for recovery after attack
            player->state = jumpAttackState;
        }

        return;
    }

    // jump cooldown in progress: blocks input until animation finishes
    if ( player->state == PLAYER_STATE_JUMP_COOLDOWN ) {
        updateAnimation( &player->jumpCooldownAnim, delta );
        if ( player->jumpCooldownAnim.finished ) {
            player->state = PLAYER_STATE_IDLE;
            resetAnimation( &player->jumpCooldownAnim );
        }
        player->lastState = player->state;
        return;
    }

    // attack
    PlayerState attackState = PLAYER_STATE_IDLE;
    Animation *attackAnim = NULL;
    float dist = distancePlayer( player, opponent );

    if ( IsKeyPressed( player->kb.lp ) ) {
        if ( player->state == PLAYER_STATE_CROUCHING ) {
            attackState = PLAYER_STATE_LP_CROUCH;
            attackAnim = &player->lpCrouchAnim;
        } else if ( dist > player->lpCloseTriggerDist ) {
            attackState = PLAYER_STATE_LP;
            attackAnim = &player->lpAnim;
        } else {
            attackState = PLAYER_STATE_LP_CLOSE;
            attackAnim = &player->lpCloseAnim;
        }
    } else if ( IsKeyPressed( player->kb.mp ) ) {
        if ( player->state == PLAYER_STATE_CROUCHING ) {
            attackState = PLAYER_STATE_MP_CROUCH;
            attackAnim = &player->mpCrouchAnim;
        } else if ( dist > player->mpCloseTriggerDist ) {
            attackState = PLAYER_STATE_MP;
            attackAnim = &player->mpAnim;
        } else {
            attackState = PLAYER_STATE_MP_CLOSE;
            attackAnim = &player->mpCloseAnim;
        }
    } else if ( IsKeyPressed( player->kb.hp ) ) {
        if ( player->state == PLAYER_STATE_CROUCHING ) {
            attackState = PLAYER_STATE_HP_CROUCH;
            attackAnim = &player->hpCrouchAnim;
        } else if ( dist > player->hpCloseTriggerDist ) {
            attackState = PLAYER_STATE_HP;
            attackAnim = &player->hpAnim;
        } else {
            attackState = PLAYER_STATE_HP_CLOSE;
            attackAnim = &player->hpCloseAnim;
        }
    } else if ( IsKeyPressed( player->kb.lk ) ) {
        if ( player->state == PLAYER_STATE_CROUCHING ) {
            attackState = PLAYER_STATE_LK_CROUCH;
            attackAnim = &player->lkCrouchAnim;
        } else if ( dist > player->lkCloseTriggerDist ) {
            attackState = PLAYER_STATE_LK;
            attackAnim = &player->lkAnim;
        } else {
            attackState = PLAYER_STATE_LK_CLOSE;
            attackAnim = &player->lkCloseAnim;
        }
    } else if ( IsKeyPressed( player->kb.mk ) ) {
        if ( player->state == PLAYER_STATE_CROUCHING ) {
            attackState = PLAYER_STATE_MK_CROUCH;
            attackAnim = &player->mkCrouchAnim;
        } else if ( dist > player->mkCloseTriggerDist ) {
            attackState = PLAYER_STATE_MK;
            attackAnim = &player->mkAnim;
        } else {
            attackState = PLAYER_STATE_MK_CLOSE;
            attackAnim = &player->mkCloseAnim;
        }
    } else if ( IsKeyPressed( player->kb.hk ) ) {
        if ( player->state == PLAYER_STATE_CROUCHING ) {
            attackState = PLAYER_STATE_HK_CROUCH;
            attackAnim = &player->hkCrouchAnim;
        } else if ( dist > player->hkCloseTriggerDist ) {
            attackState = PLAYER_STATE_HK;
            attackAnim = &player->hkAnim;
        } else {
            attackState = PLAYER_STATE_HK_CLOSE;
            attackAnim = &player->hkCloseAnim;
        }
    }

    if ( attackAnim != NULL ) {
        resetAnimation( attackAnim );
        player->vel.x = 0.0f;
        player->state = attackState;
        player->lastState = player->state;
        return;
    }

    // jump
    if ( IsKeyDown( player->kb.up ) && player->state != PLAYER_STATE_CROUCHING ) {
        if ( IsKeyDown( player->kb.right ) ) {
            player->vel.y = -player->jumpSpeed;
            player->vel.x = player->forwardSpeed * 1.6f;
            resetAnimation( &player->forwardJumpAnim );
            player->state = PLAYER_STATE_JUMPING_FORWARD;
        } else if ( IsKeyDown( player->kb.left ) ) {
            player->vel.y = -player->jumpSpeed;
            player->vel.x = -player->backwardSpeed * 2.0f;
            resetAnimation( &player->backwardJumpAnim );
            player->state = PLAYER_STATE_JUMPING_BACKWARD;
        } else {
            player->vel.y = -player->jumpSpeed;
            player->vel.x = 0.0f;
            resetAnimation( &player->straightJumpAnim );
            player->state = PLAYER_STATE_JUMPING_STRAIGHT;
        }
        player->lastState = player->state;
        return;
    }

    // floor movement
    if ( IsKeyDown( player->kb.down ) ) {
        if ( player->state != PLAYER_STATE_CROUCHING ) {
            resetAnimation( &player->crouchingAnim );
        }
        player->vel.x = 0.0f;
        player->state = PLAYER_STATE_CROUCHING;
    } else if ( IsKeyDown( player->kb.right ) ) {
        player->vel.x = player->forwardSpeed;
        player->state = PLAYER_STATE_WALKING_FORWARD;
    } else if ( IsKeyDown( player->kb.left ) ) {
        player->vel.x = -player->backwardSpeed;
        player->state = PLAYER_STATE_WALKING_BACKWARD;
    } else {
        player->vel.x = 0.0f;
        player->state = PLAYER_STATE_IDLE;
    }

    // updates floor animation
    switch ( player->state ) {
        case PLAYER_STATE_IDLE:
            updateAnimation( &player->idleAnim, delta );
            break;
        case PLAYER_STATE_WALKING_FORWARD:
            if ( player->lookingRight ) {
                updateAnimation( &player->forwardAnim, delta );
            } else {
                updateAnimation( &player->backwardAnim, delta );
            }
            break;
        case PLAYER_STATE_WALKING_BACKWARD:
            if ( player->lookingRight ) {
                updateAnimation( &player->backwardAnim, delta );
            } else {
                updateAnimation( &player->forwardAnim, delta );
            }
            break;
        case PLAYER_STATE_CROUCHING:
            updateAnimation( &player->crouchingAnim, delta );
            break;
        default:
            break;
    }

    if ( player->lastState != player->state ) {
        addState( player->lastState );
    }

    player->lastState = player->state;

}

void updatePlayer( Player *player, Player *opponent, float gravity, float delta ) {

    // positioning and physics
    player->pos.x += player->vel.x * delta;
    player->pos.y += player->vel.y * delta;

    player->vel.y += gravity * delta;
    
    if ( player->vel.y > gravity ) {
        player->vel.y = gravity;
    }
    
}

void flipPlayerSide( Player *player ) {
    player->lookingRight = !player->lookingRight;
}

float distancePlayer( Player *player1, Player *player2 ) {
    return hypotf( player1->pos.x - player2->pos.x, player1->pos.y - player2->pos.y );
}

AnimationFrame *getPlayerCurrentAnimationFrame( Player *player ) {

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
        case PLAYER_STATE_CROUCHING:
            return getAnimationCurrentFrame( &player->crouchingAnim );
        case PLAYER_STATE_JUMPING_STRAIGHT:
            return getAnimationCurrentFrame( &player->straightJumpAnim );
        case PLAYER_STATE_JUMPING_FORWARD:
            if ( player->lookingRight ) {
                return getAnimationCurrentFrame( &player->forwardJumpAnim );
            }
            return getAnimationCurrentFrame( &player->backwardJumpAnim );
        case PLAYER_STATE_JUMPING_BACKWARD:
            if ( player->lookingRight ) {
                return getAnimationCurrentFrame( &player->backwardJumpAnim );
            }
            return getAnimationCurrentFrame( &player->forwardJumpAnim );
        case PLAYER_STATE_LP:
            return getAnimationCurrentFrame( &player->lpAnim );
        case PLAYER_STATE_MP:
            return getAnimationCurrentFrame( &player->mpAnim );
        case PLAYER_STATE_HP:
            return getAnimationCurrentFrame( &player->hpAnim );
        case PLAYER_STATE_LK:
            return getAnimationCurrentFrame( &player->lkAnim );
        case PLAYER_STATE_MK:
            return getAnimationCurrentFrame( &player->mkAnim );
        case PLAYER_STATE_HK:
            return getAnimationCurrentFrame( &player->hkAnim );
        case PLAYER_STATE_LP_CLOSE:
            return getAnimationCurrentFrame( &player->lpCloseAnim );
        case PLAYER_STATE_MP_CLOSE:
            return getAnimationCurrentFrame( &player->mpCloseAnim );
        case PLAYER_STATE_HP_CLOSE:
            return getAnimationCurrentFrame( &player->hpCloseAnim );
        case PLAYER_STATE_LK_CLOSE:
            return getAnimationCurrentFrame( &player->lkCloseAnim );
        case PLAYER_STATE_MK_CLOSE:
            return getAnimationCurrentFrame( &player->mkCloseAnim );
        case PLAYER_STATE_HK_CLOSE:
            return getAnimationCurrentFrame( &player->hkCloseAnim );
        case PLAYER_STATE_LP_CROUCH:
            return getAnimationCurrentFrame( &player->lpCrouchAnim );
        case PLAYER_STATE_MP_CROUCH:
            return getAnimationCurrentFrame( &player->mpCrouchAnim );
        case PLAYER_STATE_HP_CROUCH:
            return getAnimationCurrentFrame( &player->hpCrouchAnim );
        case PLAYER_STATE_LK_CROUCH:
            return getAnimationCurrentFrame( &player->lkCrouchAnim );
        case PLAYER_STATE_MK_CROUCH:
            return getAnimationCurrentFrame( &player->mkCrouchAnim );
        case PLAYER_STATE_HK_CROUCH:
            return getAnimationCurrentFrame( &player->hkCrouchAnim );
        case PLAYER_STATE_JUMP_COOLDOWN:
            return getAnimationCurrentFrame( &player->jumpCooldownAnim );
        case PLAYER_STATE_LP_JUMP_STRAIGHT:
            return getAnimationCurrentFrame( &player->lpJumpStraightAnim );
        case PLAYER_STATE_MP_JUMP_STRAIGHT:
            return getAnimationCurrentFrame( &player->mpJumpStraightAnim );
        case PLAYER_STATE_HP_JUMP_STRAIGHT:
            return getAnimationCurrentFrame( &player->hpJumpStraightAnim );
        case PLAYER_STATE_LK_JUMP_STRAIGHT:
            return getAnimationCurrentFrame( &player->lkJumpStraightAnim );
        case PLAYER_STATE_MK_JUMP_STRAIGHT:
            return getAnimationCurrentFrame( &player->mkJumpStraightAnim );
        case PLAYER_STATE_HK_JUMP_STRAIGHT:
            return getAnimationCurrentFrame( &player->hkJumpStraightAnim );
        case PLAYER_STATE_LP_JUMP_FORWARD:
            return getAnimationCurrentFrame( &player->lpJumpForwardAnim );
        case PLAYER_STATE_MP_JUMP_FORWARD:
            return getAnimationCurrentFrame( &player->mpJumpForwardAnim );
        case PLAYER_STATE_HP_JUMP_FORWARD:
            return getAnimationCurrentFrame( &player->hpJumpForwardAnim );
        case PLAYER_STATE_LK_JUMP_FORWARD:
            return getAnimationCurrentFrame( &player->lkJumpForwardAnim );
        case PLAYER_STATE_MK_JUMP_FORWARD:
            return getAnimationCurrentFrame( &player->mkJumpForwardAnim );
        case PLAYER_STATE_HK_JUMP_FORWARD:
            return getAnimationCurrentFrame( &player->hkJumpForwardAnim );
        case PLAYER_STATE_LP_JUMP_BACKWARD:
            return getAnimationCurrentFrame( &player->lpJumpBackwardAnim );
        case PLAYER_STATE_MP_JUMP_BACKWARD:
            return getAnimationCurrentFrame( &player->mpJumpBackwardAnim );
        case PLAYER_STATE_HP_JUMP_BACKWARD:
            return getAnimationCurrentFrame( &player->hpJumpBackwardAnim );
        case PLAYER_STATE_LK_JUMP_BACKWARD:
            return getAnimationCurrentFrame( &player->lkJumpBackwardAnim );
        case PLAYER_STATE_MK_JUMP_BACKWARD:
            return getAnimationCurrentFrame( &player->mkJumpBackwardAnim );
        case PLAYER_STATE_HK_JUMP_BACKWARD:
            return getAnimationCurrentFrame( &player->hkJumpBackwardAnim );
        default:
            return NULL;
    }

}

Animation *getPlayerCurrentAnimation( Player *player ) {

    switch ( player->state ) {
        case PLAYER_STATE_IDLE:
            return &player->idleAnim;
        case PLAYER_STATE_WALKING_FORWARD:
            if ( player->lookingRight ) {
                return &player->forwardAnim;
            }
            return &player->backwardAnim;
        case PLAYER_STATE_WALKING_BACKWARD:
            if ( player->lookingRight ) {
                return &player->backwardAnim;
            }
            return &player->forwardAnim;
        case PLAYER_STATE_CROUCHING:
            return &player->crouchingAnim;
        case PLAYER_STATE_JUMPING_STRAIGHT:
            return &player->straightJumpAnim;
        case PLAYER_STATE_JUMPING_FORWARD:
            if ( player->lookingRight ) {
                return &player->forwardJumpAnim;
            }
            return &player->backwardJumpAnim;
        case PLAYER_STATE_JUMPING_BACKWARD:
            if ( player->lookingRight ) {
                return &player->backwardJumpAnim;
            }
            return &player->forwardJumpAnim;
        case PLAYER_STATE_LP:
            return &player->lpAnim;
        case PLAYER_STATE_MP:
            return &player->mpAnim;
        case PLAYER_STATE_HP:
            return &player->hpAnim;
        case PLAYER_STATE_LK:
            return &player->lkAnim;
        case PLAYER_STATE_MK:
            return &player->mkAnim;
        case PLAYER_STATE_HK:
            return &player->hkAnim;
        case PLAYER_STATE_LP_CLOSE:
            return &player->lpCloseAnim;
        case PLAYER_STATE_MP_CLOSE:
            return &player->mpCloseAnim;
        case PLAYER_STATE_HP_CLOSE:
            return &player->hpCloseAnim;
        case PLAYER_STATE_LK_CLOSE:
            return &player->lkCloseAnim;
        case PLAYER_STATE_MK_CLOSE:
            return &player->mkCloseAnim;
        case PLAYER_STATE_HK_CLOSE:
            return &player->hkCloseAnim;
        case PLAYER_STATE_LP_CROUCH:
            return &player->lpCrouchAnim;
        case PLAYER_STATE_MP_CROUCH:
            return &player->mpCrouchAnim;
        case PLAYER_STATE_HP_CROUCH:
            return &player->hpCrouchAnim;
        case PLAYER_STATE_LK_CROUCH:
            return &player->lkCrouchAnim;
        case PLAYER_STATE_MK_CROUCH:
            return &player->mkCrouchAnim;
        case PLAYER_STATE_HK_CROUCH:
            return &player->hkCrouchAnim;
        case PLAYER_STATE_JUMP_COOLDOWN:
            return &player->jumpCooldownAnim;
        case PLAYER_STATE_LP_JUMP_STRAIGHT:
            return &player->lpJumpStraightAnim;
        case PLAYER_STATE_MP_JUMP_STRAIGHT:
            return &player->mpJumpStraightAnim;
        case PLAYER_STATE_HP_JUMP_STRAIGHT:
            return &player->hpJumpStraightAnim;
        case PLAYER_STATE_LK_JUMP_STRAIGHT:
            return &player->lkJumpStraightAnim;
        case PLAYER_STATE_MK_JUMP_STRAIGHT:
            return &player->mkJumpStraightAnim;
        case PLAYER_STATE_HK_JUMP_STRAIGHT:
            return &player->hkJumpStraightAnim;
        case PLAYER_STATE_LP_JUMP_FORWARD:
            return &player->lpJumpForwardAnim;
        case PLAYER_STATE_MP_JUMP_FORWARD:
            return &player->mpJumpForwardAnim;
        case PLAYER_STATE_HP_JUMP_FORWARD:
            return &player->hpJumpForwardAnim;
        case PLAYER_STATE_LK_JUMP_FORWARD:
            return &player->lkJumpForwardAnim;
        case PLAYER_STATE_MK_JUMP_FORWARD:
            return &player->mkJumpForwardAnim;
        case PLAYER_STATE_HK_JUMP_FORWARD:
            return &player->hkJumpForwardAnim;
        case PLAYER_STATE_LP_JUMP_BACKWARD:
            return &player->lpJumpBackwardAnim;
        case PLAYER_STATE_MP_JUMP_BACKWARD:
            return &player->mpJumpBackwardAnim;
        case PLAYER_STATE_HP_JUMP_BACKWARD:
            return &player->hpJumpBackwardAnim;
        case PLAYER_STATE_LK_JUMP_BACKWARD:
            return &player->lkJumpBackwardAnim;
        case PLAYER_STATE_MK_JUMP_BACKWARD:
            return &player->mkJumpBackwardAnim;
        case PLAYER_STATE_HK_JUMP_BACKWARD:
            return &player->hkJumpBackwardAnim;
        default:
            return NULL;
    }

}

void resetPlayerAnimations( Player *player ) {
    for ( int i = 0; i < player->animationCount; i++ ) {
        resetAnimation( player->animations[i] );
    }
}
