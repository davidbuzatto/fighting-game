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

static const char *playerStateToText( PlayerState state ) {

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
    }

    return "";

}

static const Color playerStateToColor( PlayerState state ) {

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
    }

    return PINK;

}

Player *createPlayer() {
    Player *p = (Player*) malloc( sizeof( Player ) );
    return p;
}

void initializePlayerRyu( float x, float y, Player *p, bool showDebugInfo ) {

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
    p->idleAnim.frames[0] = (AnimationFrame) { (Rectangle) {   1, 142, -64, 96 }, 65, (Vector2) { 0 } };
    p->idleAnim.frames[1] = (AnimationFrame) { (Rectangle) {  66, 142, -64, 96 }, 65, (Vector2) { 0 } };
    p->idleAnim.frames[2] = (AnimationFrame) { (Rectangle) { 131, 142, -64, 96 }, 65, (Vector2) { 0 } };
    p->idleAnim.frames[3] = (AnimationFrame) { (Rectangle) { 196, 142, -64, 96 }, 65, (Vector2) { 0 } };
    p->idleAnim.frames[4] = (AnimationFrame) { (Rectangle) { 131, 142, -64, 96 }, 65, (Vector2) { 0 } };
    p->idleAnim.frames[5] = (AnimationFrame) { (Rectangle) {  66, 142, -64, 96 }, 65, (Vector2) { 0 } };

    p->forwardAnim.frameCount = 5;
    p->forwardAnim.currentFrame = 0;
    p->forwardAnim.frameTimeCounter = 0.0f;
    p->forwardAnim.stopAtLastFrame = false;
    p->forwardAnim.runOnce = false;
    p->forwardAnim.finished = false;
    createAnimationFrames( &p->forwardAnim, p->forwardAnim.frameCount );
    p->forwardAnim.frames[0] = (AnimationFrame) { (Rectangle) {   1, 514, -80, 96 }, 70, (Vector2) { 0 } };
    p->forwardAnim.frames[1] = (AnimationFrame) { (Rectangle) {  82, 514, -80, 96 }, 70, (Vector2) { 0 } };
    p->forwardAnim.frames[2] = (AnimationFrame) { (Rectangle) { 163, 514, -80, 96 }, 70, (Vector2) { 0 } };
    p->forwardAnim.frames[3] = (AnimationFrame) { (Rectangle) { 244, 514, -80, 96 }, 70, (Vector2) { 0 } };
    p->forwardAnim.frames[4] = (AnimationFrame) { (Rectangle) { 325, 514, -80, 96 }, 70, (Vector2) { 0 } };

    p->backwardAnim.frameCount = 6;
    p->backwardAnim.currentFrame = 0;
    p->backwardAnim.frameTimeCounter = 0.0f;
    p->backwardAnim.stopAtLastFrame = false;
    p->backwardAnim.runOnce = false;
    p->backwardAnim.finished = false;
    createAnimationFrames( &p->backwardAnim, p->backwardAnim.frameCount );
    p->backwardAnim.frames[0] = (AnimationFrame) { (Rectangle) {   1, 611, -80, 96 }, 80, (Vector2) { 0 } };
    p->backwardAnim.frames[1] = (AnimationFrame) { (Rectangle) {  82, 611, -80, 96 }, 80, (Vector2) { 0 } };
    p->backwardAnim.frames[2] = (AnimationFrame) { (Rectangle) { 163, 611, -80, 96 }, 80, (Vector2) { 0 } };
    p->backwardAnim.frames[3] = (AnimationFrame) { (Rectangle) { 244, 611, -80, 96 }, 80, (Vector2) { 0 } };
    p->backwardAnim.frames[4] = (AnimationFrame) { (Rectangle) { 325, 611, -80, 96 }, 80, (Vector2) { 0 } };
    p->backwardAnim.frames[5] = (AnimationFrame) { (Rectangle) { 406, 611, -80, 96 }, 80, (Vector2) { 0 } };

    p->straightJumpAnim.frameCount = 6;
    p->straightJumpAnim.currentFrame = 0;
    p->straightJumpAnim.frameTimeCounter = 0.0f;
    p->straightJumpAnim.stopAtLastFrame = true;
    p->straightJumpAnim.runOnce = false;
    p->straightJumpAnim.finished = false;
    createAnimationFrames( &p->straightJumpAnim, p->straightJumpAnim.frameCount );
    p->straightJumpAnim.frames[0] = (AnimationFrame) { (Rectangle) {   1, 821, -64, 112 }, 250, (Vector2) { 0 } };
    p->straightJumpAnim.frames[1] = (AnimationFrame) { (Rectangle) {  66, 821, -64, 112 }, 50, (Vector2) { 0 } };
    p->straightJumpAnim.frames[2] = (AnimationFrame) { (Rectangle) { 131, 821, -64, 112 }, 50, (Vector2) { 0 } };
    p->straightJumpAnim.frames[3] = (AnimationFrame) { (Rectangle) { 196, 821, -64, 112 }, 50, (Vector2) { 0 } };
    p->straightJumpAnim.frames[4] = (AnimationFrame) { (Rectangle) { 261, 821, -64, 112 }, 50, (Vector2) { 0 } };
    p->straightJumpAnim.frames[5] = (AnimationFrame) { (Rectangle) { 326, 821, -64, 112 }, 50, (Vector2) { 0 } };

    p->forwardJumpAnim.frameCount = 7;
    p->forwardJumpAnim.currentFrame = 0;
    p->forwardJumpAnim.frameTimeCounter = 0.0f;
    p->forwardJumpAnim.stopAtLastFrame = true;
    p->forwardJumpAnim.runOnce = false;
    p->forwardJumpAnim.finished = false;
    createAnimationFrames( &p->forwardJumpAnim, p->forwardJumpAnim.frameCount );
    p->forwardJumpAnim.frames[0] = (AnimationFrame) { (Rectangle) {   1, 708, -128, 112 }, 250, (Vector2) { 0 } };
    p->forwardJumpAnim.frames[1] = (AnimationFrame) { (Rectangle) { 130, 708, -128, 112 }, 50, (Vector2) { 0 } };
    p->forwardJumpAnim.frames[2] = (AnimationFrame) { (Rectangle) { 259, 708, -128, 112 }, 50, (Vector2) { 0 } };
    p->forwardJumpAnim.frames[3] = (AnimationFrame) { (Rectangle) { 388, 708, -128, 112 }, 50, (Vector2) { 0 } };
    p->forwardJumpAnim.frames[4] = (AnimationFrame) { (Rectangle) { 517, 708, -128, 112 }, 50, (Vector2) { 0 } };
    p->forwardJumpAnim.frames[5] = (AnimationFrame) { (Rectangle) { 646, 708, -128, 112 }, 50, (Vector2) { 0 } };
    p->forwardJumpAnim.frames[6] = (AnimationFrame) { (Rectangle) {   1, 708, -128, 112 }, 50, (Vector2) { 0 } };

    p->backwardJumpAnim.frameCount = 7;
    p->backwardJumpAnim.currentFrame = 0;
    p->backwardJumpAnim.frameTimeCounter = 0.0f;
    p->backwardJumpAnim.stopAtLastFrame = true;
    p->backwardJumpAnim.runOnce = false;
    p->backwardJumpAnim.finished = false;
    createAnimationFrames( &p->backwardJumpAnim, p->backwardJumpAnim.frameCount );
    p->backwardJumpAnim.frames[0] = (AnimationFrame) { (Rectangle) {   1, 708, -128, 112 }, 250, (Vector2) { 0 } };
    p->backwardJumpAnim.frames[1] = (AnimationFrame) { (Rectangle) { 646, 708, -128, 112 }, 50, (Vector2) { 0 } };
    p->backwardJumpAnim.frames[2] = (AnimationFrame) { (Rectangle) { 517, 708, -128, 112 }, 50, (Vector2) { 0 } };
    p->backwardJumpAnim.frames[3] = (AnimationFrame) { (Rectangle) { 388, 708, -128, 112 }, 50, (Vector2) { 0 } };
    p->backwardJumpAnim.frames[4] = (AnimationFrame) { (Rectangle) { 259, 708, -128, 112 }, 50, (Vector2) { 0 } };
    p->backwardJumpAnim.frames[5] = (AnimationFrame) { (Rectangle) { 130, 708, -128, 112 }, 50, (Vector2) { 0 } };
    p->backwardJumpAnim.frames[6] = (AnimationFrame) { (Rectangle) {   1, 708, -128, 112 }, 50, (Vector2) { 0 } };

    p->crouchingAnim.frameCount = 3;
    p->crouchingAnim.currentFrame = 0;
    p->crouchingAnim.frameTimeCounter = 0.0f;
    p->crouchingAnim.stopAtLastFrame = true;
    p->crouchingAnim.runOnce = false;
    p->crouchingAnim.finished = false;
    createAnimationFrames( &p->crouchingAnim, p->crouchingAnim.frameCount );
    p->crouchingAnim.frames[0] = (AnimationFrame) { (Rectangle) {   1, 352, -80, 96 }, 30, (Vector2) { 0 } };
    p->crouchingAnim.frames[1] = (AnimationFrame) { (Rectangle) {  82, 352, -80, 96 }, 30, (Vector2) { 0 } };
    p->crouchingAnim.frames[2] = (AnimationFrame) { (Rectangle) { 163, 352, -80, 96 }, 30, (Vector2) { 0 } };

    p->lpAnim.frameCount = 3;
    p->lpAnim.currentFrame = 0;
    p->lpAnim.frameTimeCounter = 0.0f;
    p->lpAnim.stopAtLastFrame = false;
    p->lpAnim.runOnce = true;
    p->lpAnim.finished = false;
    createAnimationFrames( &p->lpAnim, p->lpAnim.frameCount );
    p->lpAnim.frames[0] = (AnimationFrame) { (Rectangle) {   1, 1031, -112, 96 }, 50, (Vector2) { 22, 0 } };
    p->lpAnim.frames[1] = (AnimationFrame) { (Rectangle) { 114, 1031, -112, 96 }, 50, (Vector2) { 22, 0 } };
    p->lpAnim.frames[2] = (AnimationFrame) { (Rectangle) {   1, 1031, -112, 96 }, 50, (Vector2) { 22, 0 } };
    
    p->mpAnim.frameCount = 5;
    p->mpAnim.currentFrame = 0;
    p->mpAnim.frameTimeCounter = 0.0f;
    p->mpAnim.stopAtLastFrame = false;
    p->mpAnim.runOnce = true;
    p->mpAnim.finished = false;
    createAnimationFrames( &p->mpAnim, p->mpAnim.frameCount );
    p->mpAnim.frames[0] = (AnimationFrame) { (Rectangle) {   1, 1128, -128, 96 }, 50, (Vector2) { 30, 0 } };
    p->mpAnim.frames[1] = (AnimationFrame) { (Rectangle) { 130, 1128, -128, 96 }, 50, (Vector2) { 30, 0 } };
    p->mpAnim.frames[2] = (AnimationFrame) { (Rectangle) { 259, 1128, -128, 96 }, 50, (Vector2) { 30, 0 } };
    p->mpAnim.frames[3] = (AnimationFrame) { (Rectangle) { 130, 1128, -128, 96 }, 50, (Vector2) { 30, 0 } };
    p->mpAnim.frames[4] = (AnimationFrame) { (Rectangle) {   1, 1128, -128, 96 }, 50, (Vector2) { 30, 0 } };

    p->hpAnim.frameCount = 5;
    p->hpAnim.currentFrame = 0;
    p->hpAnim.frameTimeCounter = 0.0f;
    p->hpAnim.stopAtLastFrame = false;
    p->hpAnim.runOnce = true;
    p->hpAnim.finished = false;
    createAnimationFrames( &p->hpAnim, p->hpAnim.frameCount );
    p->hpAnim.frames[0] = (AnimationFrame) { (Rectangle) {   1, 1128, -128, 96 }, 90, (Vector2) { 30, 0 } };
    p->hpAnim.frames[1] = (AnimationFrame) { (Rectangle) { 130, 1128, -128, 96 }, 90, (Vector2) { 30, 0 } };
    p->hpAnim.frames[2] = (AnimationFrame) { (Rectangle) { 259, 1128, -128, 96 }, 90, (Vector2) { 30, 0 } };
    p->hpAnim.frames[3] = (AnimationFrame) { (Rectangle) { 130, 1128, -128, 96 }, 90, (Vector2) { 30, 0 } };
    p->hpAnim.frames[4] = (AnimationFrame) { (Rectangle) {   1, 1128, -128, 96 }, 90, (Vector2) { 30, 0 } };
    
    p->lkAnim.frameCount = 3;
    p->lkAnim.currentFrame = 0;
    p->lkAnim.frameTimeCounter = 0.0f;
    p->lkAnim.stopAtLastFrame = false;
    p->lkAnim.runOnce = true;
    p->lkAnim.finished = false;
    createAnimationFrames( &p->lkAnim, p->lkAnim.frameCount );
    p->lkAnim.frames[0] = (AnimationFrame) { (Rectangle) {   1, 2001, -128, 96 }, 60, (Vector2) { 0, -5 } };
    p->lkAnim.frames[1] = (AnimationFrame) { (Rectangle) { 130, 2001, -128, 96 }, 60, (Vector2) { 0, -5 } };
    p->lkAnim.frames[2] = (AnimationFrame) { (Rectangle) {   1, 2001, -128, 96 }, 60, (Vector2) { 0, -5 } };
    
    p->mkAnim.frameCount = 3;
    p->mkAnim.currentFrame = 0;
    p->mkAnim.frameTimeCounter = 0.0f;
    p->mkAnim.stopAtLastFrame = false;
    p->mkAnim.runOnce = true;
    p->mkAnim.finished = false;
    createAnimationFrames( &p->mkAnim, p->mkAnim.frameCount );
    p->mkAnim.frames[0] = (AnimationFrame) { (Rectangle) {   1, 2001, -128, 96 }, 90, (Vector2) { 0, -5 } };
    p->mkAnim.frames[1] = (AnimationFrame) { (Rectangle) { 130, 2001, -128, 96 }, 90, (Vector2) { 0, -5 } };
    p->mkAnim.frames[2] = (AnimationFrame) { (Rectangle) {   1, 2001, -128, 96 }, 90, (Vector2) { 0, -5 } };

    p->hkAnim.frameCount = 5;
    p->hkAnim.currentFrame = 0;
    p->hkAnim.frameTimeCounter = 0.0f;
    p->hkAnim.stopAtLastFrame = false;
    p->hkAnim.runOnce = true;
    p->hkAnim.finished = false;
    createAnimationFrames( &p->hkAnim, p->hkAnim.frameCount );
    p->hkAnim.frames[0] = (AnimationFrame) { (Rectangle) {   1, 2098, -128, 96 }, 90, (Vector2) { 20, 0 } };
    p->hkAnim.frames[1] = (AnimationFrame) { (Rectangle) { 130, 2098, -128, 96 }, 90, (Vector2) { 20, 0 } };
    p->hkAnim.frames[2] = (AnimationFrame) { (Rectangle) { 259, 2098, -128, 96 }, 90, (Vector2) { 20, 0 } };
    p->hkAnim.frames[3] = (AnimationFrame) { (Rectangle) { 388, 2098, -128, 96 }, 90, (Vector2) { 20, 0 } };
    p->hkAnim.frames[4] = (AnimationFrame) { (Rectangle) { 517, 2098, -128, 96 }, 90, (Vector2) { 20, 0 } };

    p->lpCloseAnim.frameCount = 1;
    p->lpCloseAnim.currentFrame = 0;
    p->lpCloseAnim.frameTimeCounter = 0.0f;
    p->lpCloseAnim.stopAtLastFrame = false;
    p->lpCloseAnim.runOnce = true;
    p->lpCloseAnim.finished = false;
    createAnimationFrames( &p->lpCloseAnim, p->lpCloseAnim.frameCount );
    p->lpCloseAnim.frames[0] = (AnimationFrame) { (Rectangle) { 1, 1225, -80, 112 }, 150, (Vector2) { 5, 0 } };
    
    p->mpCloseAnim.frameCount = 5;
    p->mpCloseAnim.currentFrame = 0;
    p->mpCloseAnim.frameTimeCounter = 0.0f;
    p->mpCloseAnim.stopAtLastFrame = false;
    p->mpCloseAnim.runOnce = true;
    p->mpCloseAnim.finished = false;
    createAnimationFrames( &p->mpCloseAnim, p->mpCloseAnim.frameCount );
    p->mpCloseAnim.frames[0] = (AnimationFrame) { (Rectangle) {   1, 1338, -96, 96 }, 65, (Vector2) { 7, -1 } };
    p->mpCloseAnim.frames[1] = (AnimationFrame) { (Rectangle) {  98, 1338, -96, 96 }, 65, (Vector2) { 7, -1 } };
    p->mpCloseAnim.frames[2] = (AnimationFrame) { (Rectangle) { 195, 1338, -96, 96 }, 65, (Vector2) { 7, -1 } };
    p->mpCloseAnim.frames[3] = (AnimationFrame) { (Rectangle) {  98, 1338, -96, 96 }, 65, (Vector2) { 7, -1 } };
    p->mpCloseAnim.frames[4] = (AnimationFrame) { (Rectangle) {   1, 1338, -96, 96 }, 65, (Vector2) { 7, -1 } };

    p->hpCloseAnim.frameCount = 5;
    p->hpCloseAnim.currentFrame = 0;
    p->hpCloseAnim.frameTimeCounter = 0.0f;
    p->hpCloseAnim.stopAtLastFrame = false;
    p->hpCloseAnim.runOnce = true;
    p->hpCloseAnim.finished = false;
    createAnimationFrames( &p->hpCloseAnim, p->hpCloseAnim.frameCount );
    p->hpCloseAnim.frames[0] = (AnimationFrame) { (Rectangle) {   1, 1435, -112, 128 }, 65, (Vector2) { 20, 0 } };
    p->hpCloseAnim.frames[1] = (AnimationFrame) { (Rectangle) { 114, 1435, -112, 128 }, 65, (Vector2) { 20, 0 } };
    p->hpCloseAnim.frames[2] = (AnimationFrame) { (Rectangle) { 227, 1435, -112, 128 }, 65, (Vector2) { 20, 0 } };
    p->hpCloseAnim.frames[3] = (AnimationFrame) { (Rectangle) { 114, 1435, -112, 128 }, 65, (Vector2) { 20, 0 } };
    p->hpCloseAnim.frames[4] = (AnimationFrame) { (Rectangle) {   1, 1435, -112, 128 }, 65, (Vector2) { 20, 0 } };
    
    p->lkCloseAnim.frameCount = 3;
    p->lkCloseAnim.currentFrame = 0;
    p->lkCloseAnim.frameTimeCounter = 0.0f;
    p->lkCloseAnim.stopAtLastFrame = false;
    p->lkCloseAnim.runOnce = true;
    p->lkCloseAnim.finished = false;
    createAnimationFrames( &p->lkCloseAnim, p->lkCloseAnim.frameCount );
    p->lkCloseAnim.frames[0] = (AnimationFrame) { (Rectangle) {  1, 2195, -96, 96 },  60, (Vector2) { 42, 0 } };
    p->lkCloseAnim.frames[1] = (AnimationFrame) { (Rectangle) { 98, 2195, -96, 96 }, 120, (Vector2) { 42, 0 } };
    p->lkCloseAnim.frames[2] = (AnimationFrame) { (Rectangle) {  1, 2195, -96, 96 },  60, (Vector2) { 42, 0 } };
    
    p->mkCloseAnim.frameCount = 3;
    p->mkCloseAnim.currentFrame = 0;
    p->mkCloseAnim.frameTimeCounter = 0.0f;
    p->mkCloseAnim.stopAtLastFrame = false;
    p->mkCloseAnim.runOnce = true;
    p->mkCloseAnim.finished = false;
    createAnimationFrames( &p->mkCloseAnim, p->mkCloseAnim.frameCount );
    p->mkCloseAnim.frames[0] = (AnimationFrame) { (Rectangle) {  1, 2292, -80, 112 },  60, (Vector2) { 30, 0 } };
    p->mkCloseAnim.frames[1] = (AnimationFrame) { (Rectangle) { 82, 2292, -80, 112 }, 120, (Vector2) { 30, 0 } };
    p->mkCloseAnim.frames[2] = (AnimationFrame) { (Rectangle) {  1, 2292, -80, 112 },  60, (Vector2) { 30, 0 } };

    p->hkCloseAnim.frameCount = 4;
    p->hkCloseAnim.currentFrame = 0;
    p->hkCloseAnim.frameTimeCounter = 0.0f;
    p->hkCloseAnim.stopAtLastFrame = false;
    p->hkCloseAnim.runOnce = true;
    p->hkCloseAnim.finished = false;
    createAnimationFrames( &p->hkCloseAnim, p->hkCloseAnim.frameCount );
    p->hkCloseAnim.frames[0] = (AnimationFrame) { (Rectangle) {   1, 2405, -112, 128 },  60, (Vector2) { 47, 0 } };
    p->hkCloseAnim.frames[1] = (AnimationFrame) { (Rectangle) { 114, 2405, -112, 128 }, 120, (Vector2) { 47, 0 } };
    p->hkCloseAnim.frames[2] = (AnimationFrame) { (Rectangle) { 227, 2405, -112, 128 }, 120, (Vector2) { 47, 0 } };
    p->hkCloseAnim.frames[3] = (AnimationFrame) { (Rectangle) {   1, 2405, -112, 128 },  60, (Vector2) { 47, 0 } };

    p->lpCrouchAnim.frameCount = 2;
    p->lpCrouchAnim.currentFrame = 0;
    p->lpCrouchAnim.frameTimeCounter = 0.0f;
    p->lpCrouchAnim.stopAtLastFrame = false;
    p->lpCrouchAnim.runOnce = true;
    p->lpCrouchAnim.finished = false;
    createAnimationFrames( &p->lpCrouchAnim, p->lpCrouchAnim.frameCount );
    p->lpCrouchAnim.frames[0] = (AnimationFrame) { (Rectangle) {   1, 1564, -112, 64 }, 60, (Vector2) { 16, 0 } };
    p->lpCrouchAnim.frames[1] = (AnimationFrame) { (Rectangle) { 114, 1564, -112, 64 }, 60, (Vector2) { 16, 0 } };

    p->mpCrouchAnim.frameCount = 3;
    p->mpCrouchAnim.currentFrame = 0;
    p->mpCrouchAnim.frameTimeCounter = 0.0f;
    p->mpCrouchAnim.stopAtLastFrame = false;
    p->mpCrouchAnim.runOnce = true;
    p->mpCrouchAnim.finished = false;
    createAnimationFrames( &p->mpCrouchAnim, p->mpCrouchAnim.frameCount );
    p->mpCrouchAnim.frames[0] = (AnimationFrame) { (Rectangle) {   1, 1629, -112, 64 }, 60, (Vector2) { 16, 0 } };
    p->mpCrouchAnim.frames[1] = (AnimationFrame) { (Rectangle) { 114, 1629, -112, 64 }, 60, (Vector2) { 16, 0 } };
    p->mpCrouchAnim.frames[2] = (AnimationFrame) { (Rectangle) { 227, 1629, -112, 64 }, 60, (Vector2) { 16, 0 } };

    p->hpCrouchAnim.frameCount = 3;
    p->hpCrouchAnim.currentFrame = 0;
    p->hpCrouchAnim.frameTimeCounter = 0.0f;
    p->hpCrouchAnim.stopAtLastFrame = false;
    p->hpCrouchAnim.runOnce = true;
    p->hpCrouchAnim.finished = false;
    createAnimationFrames( &p->hpCrouchAnim, p->hpCrouchAnim.frameCount );
    p->hpCrouchAnim.frames[0] = (AnimationFrame) { (Rectangle) {   1, 1694, -96, 128 }, 60, (Vector2) { 8, 0 } };
    p->hpCrouchAnim.frames[1] = (AnimationFrame) { (Rectangle) {  98, 1694, -96, 128 }, 60, (Vector2) { 8, 0 } };
    p->hpCrouchAnim.frames[2] = (AnimationFrame) { (Rectangle) { 195, 1694, -96, 128 }, 60, (Vector2) { 8, 0 } };

    p->lkCrouchAnim.frameCount = 1;
    p->lkCrouchAnim.currentFrame = 0;
    p->lkCrouchAnim.frameTimeCounter = 0.0f;
    p->lkCrouchAnim.stopAtLastFrame = false;
    p->lkCrouchAnim.runOnce = true;
    p->lkCrouchAnim.finished = false;
    createAnimationFrames( &p->lkCrouchAnim, p->lkCrouchAnim.frameCount );
    p->lkCrouchAnim.frames[0] = (AnimationFrame) { (Rectangle) { 1, 2534, -160, 64 }, 60, (Vector2) { 40, 0 } };

    p->mkCrouchAnim.frameCount = 2;
    p->mkCrouchAnim.currentFrame = 0;
    p->mkCrouchAnim.frameTimeCounter = 0.0f;
    p->mkCrouchAnim.stopAtLastFrame = false;
    p->mkCrouchAnim.runOnce = true;
    p->mkCrouchAnim.finished = false;
    createAnimationFrames( &p->mkCrouchAnim, p->mkCrouchAnim.frameCount );
    p->mkCrouchAnim.frames[0] = (AnimationFrame) { (Rectangle) {   1, 2534, -160, 64 }, 60, (Vector2) { 40, 0 } };
    p->mkCrouchAnim.frames[1] = (AnimationFrame) { (Rectangle) { 162, 2534, -160, 64 }, 60, (Vector2) { 40, 0 } };

    p->hkCrouchAnim.frameCount = 4;
    p->hkCrouchAnim.currentFrame = 0;
    p->hkCrouchAnim.frameTimeCounter = 0.0f;
    p->hkCrouchAnim.stopAtLastFrame = false;
    p->hkCrouchAnim.runOnce = true;
    p->hkCrouchAnim.finished = false;
    createAnimationFrames( &p->hkCrouchAnim, p->hkCrouchAnim.frameCount );
    p->hkCrouchAnim.frames[0] = (AnimationFrame) { (Rectangle) {   1, 2599, -144, 64 }, 60, (Vector2) { 32, 0 } };
    p->hkCrouchAnim.frames[1] = (AnimationFrame) { (Rectangle) { 146, 2599, -144, 64 }, 60, (Vector2) { 32, 0 } };
    p->hkCrouchAnim.frames[2] = (AnimationFrame) { (Rectangle) { 291, 2599, -144, 64 }, 60, (Vector2) { 32, 0 } };
    p->hkCrouchAnim.frames[2] = (AnimationFrame) { (Rectangle) { 436, 2599, -144, 64 }, 60, (Vector2) { 32, 0 } };

    int animationCount = 0;
    p->animations[animationCount++] = &p->idleAnim;
    p->animations[animationCount++] = &p->forwardAnim;
    p->animations[animationCount++] = &p->backwardAnim;
    p->animations[animationCount++] = &p->straightJumpAnim;
    p->animations[animationCount++] = &p->forwardJumpAnim;
    p->animations[animationCount++] = &p->backwardJumpAnim;
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
    p->animationCount = animationCount;

}

void initializePlayerKen( float x, float y, Player *p, bool showDebugInfo ) {

    initializePlayerRyu( x, y, p, showDebugInfo );
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

        //DrawCircle( player->pos.x, player->pos.y, 2, BLUE );
        //DrawRectangleLines( player->pos.x - player->dim.x / 2, player->pos.y, player->dim.x, player->dim.y, BLUE );
        //DrawText( TextFormat( "y: %.2f", player->pos.y ), player->pos.x + 10, player->pos.y, 10, BLACK );

        // states
        int pos = 0;
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
        default: break;
    }

    if ( activeAnim != NULL ) {
        player->lastState = player->state;   // register stack state before transitioning
        updateAnimation( activeAnim, delta );
        if ( activeAnim->finished ) {
            player->state = PLAYER_STATE_IDLE;
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
        // air atack discarted for now
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
        if ( player->state == PLAYER_STATE_CROUCHING ) {
            resetAnimation( &player->crouchingAnim );
        }
        resetAnimation( attackAnim );
        player->vel.x = 0.0f;
        player->state = attackState;
        player->lastState = player->state;
        return;
    }

    // jump
    if ( IsKeyPressed( player->kb.up ) && player->state != PLAYER_STATE_CROUCHING ) {
        if ( player->vel.x == 0.0f ) {
            player->vel.y = -player->jumpSpeed;
            resetAnimation( &player->straightJumpAnim );
            player->state = PLAYER_STATE_JUMPING_STRAIGHT;
        } else if ( player->vel.x > 0.0f ) {
            player->vel.y = -player->jumpSpeed;
            player->vel.x = player->forwardSpeed * 1.6f;
            resetAnimation( &player->forwardJumpAnim );
            player->state = PLAYER_STATE_JUMPING_FORWARD;
        } else {
            player->vel.y = -player->jumpSpeed;
            player->vel.x = -player->backwardSpeed * 2.0f;
            resetAnimation( &player->backwardJumpAnim );
            player->state = PLAYER_STATE_JUMPING_BACKWARD;
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
    }

    return NULL;

}

void resetPlayerAnimations( Player *player ) {
    for ( int i = 0; i < player->animationCount; i++ ) {
        resetAnimation( player->animations[i] );
    }
}
