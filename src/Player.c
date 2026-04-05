#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

#include "raylib/raylib.h"

#include "Animation.h"
#include "Macros.h"
#include "Player.h"
#include "ResourceManager.h"
#include "Types.h"
#include "Utils.h"

static void drawPlayerAnimationFrameBoxes( Player *player, AnimationFrame *af, Vector2 offset );
static void processInputAndFeedInputBuffer( Player *p, int currentFrame );
static void addInputToPlayerInputBuffer( Player *p, InputType input, int currentFrame );
static InputType peekAttackButton( Player *p, int currentFrame );
static CommandInput *checkCommandInputs( Player *p, int currentFrame, InputType *outButton );

Player *createPlayer() {
    Player *p = (Player*) malloc( sizeof( Player ) );
    return p;
}

void initializePlayerRyu( float x, float y, Player *p, PlayerStartSide startSide, DurationMode animationDurationMode, bool showBoxes, bool showDebugInfo ) {

    p->pos.x = x;
    p->pos.y = y;
    p->dim.x = 80;
    p->dim.y = 100;
    p->texture = &rm.ryuTexture;
    p->vel = (Vector2) { 0 };
    p->forwardSpeed = 150;
    p->backwardSpeed = 120;
    p->jumpSpeed = 450;
    p->animationDurationMode = animationDurationMode;
    p->state = PLAYER_STATE_IDLE;
    p->lastState = PLAYER_STATE_IDLE;
    p->health = 100;
    strcpy( p->name, "Ryu" );
    p->lookingRight = true;
    p->startSide = startSide;
    p->showBoxes = showBoxes;
    p->showDebugInfo = showDebugInfo;

    p->inputBufferHead = -1;
    p->inputBufferTail = -1;
    p->inputBufferSize = 0;
    p->lastDirectionalState = INPUT_TYPE_NEUTRAL;

    // command inputs (sequences defined for right-facing; auto-mirrored for left)
    p->commandCount = 0;

    // Hadouken: ↓ ↘ → + Punch
    p->commands[p->commandCount++] = (CommandInput) {
        .type = COMMAND_TYPE_HADOUKEN,
        .sequence = { INPUT_TYPE_DOWN, INPUT_TYPE_RIGHT_DOWN, INPUT_TYPE_RIGHT },
        .sequenceLength = 3,
        .requiresPunch = true,
        .requiresKick = false,
        .frameWindow = COMMAND_INPUT_WINDOW,
    };

    // Shoryuken: → ↓ ↘ + Punch
    p->commands[p->commandCount++] = (CommandInput) {
        .type = COMMAND_TYPE_SHORYUKEN,
        .sequence = { INPUT_TYPE_RIGHT, INPUT_TYPE_DOWN, INPUT_TYPE_RIGHT_DOWN },
        .sequenceLength = 3,
        .requiresPunch = true,
        .requiresKick = false,
        .frameWindow = COMMAND_INPUT_WINDOW,
    };

    // Tatsumaki: ↓ ↙ ← + Kick
    p->commands[p->commandCount++] = (CommandInput) {
        .type = COMMAND_TYPE_TATSUMAKI,
        .sequence = { INPUT_TYPE_DOWN, INPUT_TYPE_LEFT_DOWN, INPUT_TYPE_LEFT },
        .sequenceLength = 3,
        .requiresPunch = false,
        .requiresKick = true,
        .frameWindow = COMMAND_INPUT_WINDOW,
    };

    p->lpCloseTriggerDist = 50;
    p->mpCloseTriggerDist = 60;
    p->hpCloseTriggerDist = 60;
    p->lkCloseTriggerDist = 60;
    p->mkCloseTriggerDist = 50;
    p->hkCloseTriggerDist = 60;

    p->idleAnim.frameCount = 6;
    p->idleAnim.currentFrame = 0;
    p->idleAnim.frameTimeCounter = 0.0f;
    p->idleAnim.stopAtLastFrame = false;
    p->idleAnim.runOnce = false;
    p->idleAnim.finished = false;
    createAnimationFrames( &p->idleAnim, p->idleAnim.frameCount );
    p->idleAnim.frames[0] = (AnimationFrame) { {   1, 142, -64, 96 }, 0, { 0, 0 }, .boxes = { 0 }, true, true, 0 };
    p->idleAnim.frames[1] = (AnimationFrame) { {  66, 142, -64, 96 }, 0, { 0, 0 }, .boxes = { 0 }, true, true, 0 };
    p->idleAnim.frames[2] = (AnimationFrame) { { 131, 142, -64, 96 }, 0, { 0, 0 }, .boxes = { 0 }, true, true, 0 };
    p->idleAnim.frames[3] = (AnimationFrame) { { 196, 142, -64, 96 }, 0, { 0, 0 }, .boxes = { 0 }, true, true, 0 };
    p->idleAnim.frames[4] = (AnimationFrame) { { 131, 142, -64, 96 }, 0, { 0, 0 }, .boxes = { 0 }, true, true, 0 };
    p->idleAnim.frames[5] = (AnimationFrame) { {  66, 142, -64, 96 }, 0, { 0, 0 }, .boxes = { 0 }, true, true, 0 };

    p->forwardAnim.frameCount = 5;
    p->forwardAnim.currentFrame = 0;
    p->forwardAnim.frameTimeCounter = 0.0f;
    p->forwardAnim.stopAtLastFrame = false;
    p->forwardAnim.runOnce = false;
    p->forwardAnim.finished = false;
    createAnimationFrames( &p->forwardAnim, p->forwardAnim.frameCount );
    p->forwardAnim.frames[0] = (AnimationFrame) { {   1, 514, -80, 96 }, 0, { 0, 0 }, .boxes = { 0 }, true, true, 0 };
    p->forwardAnim.frames[1] = (AnimationFrame) { {  82, 514, -80, 96 }, 0, { 0, 0 }, .boxes = { 0 }, true, true, 0 };
    p->forwardAnim.frames[2] = (AnimationFrame) { { 163, 514, -80, 96 }, 0, { 0, 0 }, .boxes = { 0 }, true, true, 0 };
    p->forwardAnim.frames[3] = (AnimationFrame) { { 244, 514, -80, 96 }, 0, { 0, 0 }, .boxes = { 0 }, true, true, 0 };
    p->forwardAnim.frames[4] = (AnimationFrame) { { 325, 514, -80, 96 }, 0, { 0, 0 }, .boxes = { 0 }, true, true, 0 };

    p->backwardAnim.frameCount = 6;
    p->backwardAnim.currentFrame = 0;
    p->backwardAnim.frameTimeCounter = 0.0f;
    p->backwardAnim.stopAtLastFrame = false;
    p->backwardAnim.runOnce = false;
    p->backwardAnim.finished = false;
    createAnimationFrames( &p->backwardAnim, p->backwardAnim.frameCount );
    p->backwardAnim.frames[0] = (AnimationFrame) { {   1, 611, -80, 96 }, 0, { 0, 0 }, .boxes = { 0 }, true, true, 0 };
    p->backwardAnim.frames[1] = (AnimationFrame) { {  82, 611, -80, 96 }, 0, { 0, 0 }, .boxes = { 0 }, true, true, 0 };
    p->backwardAnim.frames[2] = (AnimationFrame) { { 163, 611, -80, 96 }, 0, { 0, 0 }, .boxes = { 0 }, true, true, 0 };
    p->backwardAnim.frames[3] = (AnimationFrame) { { 244, 611, -80, 96 }, 0, { 0, 0 }, .boxes = { 0 }, true, true, 0 };
    p->backwardAnim.frames[4] = (AnimationFrame) { { 325, 611, -80, 96 }, 0, { 0, 0 }, .boxes = { 0 }, true, true, 0 };
    p->backwardAnim.frames[5] = (AnimationFrame) { { 406, 611, -80, 96 }, 0, { 0, 0 }, .boxes = { 0 }, true, true, 0 };

    p->straightJumpAnim.frameCount = 6;
    p->straightJumpAnim.currentFrame = 0;
    p->straightJumpAnim.frameTimeCounter = 0.0f;
    p->straightJumpAnim.stopAtLastFrame = true;
    p->straightJumpAnim.runOnce = false;
    p->straightJumpAnim.finished = false;
    createAnimationFrames( &p->straightJumpAnim, p->straightJumpAnim.frameCount );
    p->straightJumpAnim.frames[0] = (AnimationFrame) { {   1, 821, -64, 112 }, 0, { 0, 0 }, .boxes = { 0 }, true, true, 0 };
    p->straightJumpAnim.frames[1] = (AnimationFrame) { {  66, 821, -64, 112 }, 0, { 0, 0 }, .boxes = { 0 }, true, true, 0 };
    p->straightJumpAnim.frames[2] = (AnimationFrame) { { 131, 821, -64, 112 }, 0, { 0, 0 }, .boxes = { 0 }, true, true, 0 };
    p->straightJumpAnim.frames[3] = (AnimationFrame) { { 196, 821, -64, 112 }, 0, { 0, 0 }, .boxes = { 0 }, true, true, 0 };
    p->straightJumpAnim.frames[4] = (AnimationFrame) { { 261, 821, -64, 112 }, 0, { 0, 0 }, .boxes = { 0 }, true, true, 0 };
    p->straightJumpAnim.frames[5] = (AnimationFrame) { { 326, 821, -64, 112 }, 0, { 0, 0 }, .boxes = { 0 }, true, true, 0 };

    p->forwardJumpAnim.frameCount = 7;
    p->forwardJumpAnim.currentFrame = 0;
    p->forwardJumpAnim.frameTimeCounter = 0.0f;
    p->forwardJumpAnim.stopAtLastFrame = true;
    p->forwardJumpAnim.runOnce = false;
    p->forwardJumpAnim.finished = false;
    createAnimationFrames( &p->forwardJumpAnim, p->forwardJumpAnim.frameCount );
    p->forwardJumpAnim.frames[0] = (AnimationFrame) { {   1, 708, -128, 112 }, 0, { 0, 0 }, .boxes = { 0 }, true, true, 0 };
    p->forwardJumpAnim.frames[1] = (AnimationFrame) { { 130, 708, -128, 112 }, 0, { 0, 0 }, .boxes = { 0 }, true, true, 0 };
    p->forwardJumpAnim.frames[2] = (AnimationFrame) { { 259, 708, -128, 112 }, 0, { 0, 0 }, .boxes = { 0 }, true, true, 0 };
    p->forwardJumpAnim.frames[3] = (AnimationFrame) { { 388, 708, -128, 112 }, 0, { 0, 0 }, .boxes = { 0 }, true, true, 0 };
    p->forwardJumpAnim.frames[4] = (AnimationFrame) { { 517, 708, -128, 112 }, 0, { 0, 0 }, .boxes = { 0 }, true, true, 0 };
    p->forwardJumpAnim.frames[5] = (AnimationFrame) { { 646, 708, -128, 112 }, 0, { 0, 0 }, .boxes = { 0 }, true, true, 0 };
    p->forwardJumpAnim.frames[6] = (AnimationFrame) { {   1, 708, -128, 112 }, 0, { 0, 0 }, .boxes = { 0 }, true, true, 0 };

    p->backwardJumpAnim.frameCount = 7;
    p->backwardJumpAnim.currentFrame = 0;
    p->backwardJumpAnim.frameTimeCounter = 0.0f;
    p->backwardJumpAnim.stopAtLastFrame = true;
    p->backwardJumpAnim.runOnce = false;
    p->backwardJumpAnim.finished = false;
    createAnimationFrames( &p->backwardJumpAnim, p->backwardJumpAnim.frameCount );
    p->backwardJumpAnim.frames[0] = (AnimationFrame) { {   1, 708, -128, 112 }, 0, { 0, 0 }, .boxes = { 0 }, true, true, 0 };
    p->backwardJumpAnim.frames[1] = (AnimationFrame) { { 646, 708, -128, 112 }, 0, { 0, 0 }, .boxes = { 0 }, true, true, 0 };
    p->backwardJumpAnim.frames[2] = (AnimationFrame) { { 517, 708, -128, 112 }, 0, { 0, 0 }, .boxes = { 0 }, true, true, 0 };
    p->backwardJumpAnim.frames[3] = (AnimationFrame) { { 388, 708, -128, 112 }, 0, { 0, 0 }, .boxes = { 0 }, true, true, 0 };
    p->backwardJumpAnim.frames[4] = (AnimationFrame) { { 259, 708, -128, 112 }, 0, { 0, 0 }, .boxes = { 0 }, true, true, 0 };
    p->backwardJumpAnim.frames[5] = (AnimationFrame) { { 130, 708, -128, 112 }, 0, { 0, 0 }, .boxes = { 0 }, true, true, 0 };
    p->backwardJumpAnim.frames[6] = (AnimationFrame) { {   1, 708, -128, 112 }, 0, { 0, 0 }, .boxes = { 0 }, true, true, 0 };

    p->jumpCooldownAnim.frameCount = 1;
    p->jumpCooldownAnim.currentFrame = 0;
    p->jumpCooldownAnim.frameTimeCounter = 0.0f;
    p->jumpCooldownAnim.stopAtLastFrame = false;
    p->jumpCooldownAnim.runOnce = true;
    p->jumpCooldownAnim.finished = false;
    createAnimationFrames( &p->jumpCooldownAnim, p->jumpCooldownAnim.frameCount );
    p->jumpCooldownAnim.frames[0] = (AnimationFrame) { { 1, 934, -64, 96 }, 0, { 0, 0 }, .boxes = { 0 }, true, true, 0 };

    p->crouchingAnim.frameCount = 3;
    p->crouchingAnim.currentFrame = 0;
    p->crouchingAnim.frameTimeCounter = 0.0f;
    p->crouchingAnim.stopAtLastFrame = true;
    p->crouchingAnim.runOnce = false;
    p->crouchingAnim.finished = false;
    createAnimationFrames( &p->crouchingAnim, p->crouchingAnim.frameCount );
    p->crouchingAnim.frames[0] = (AnimationFrame) { {   1, 352, -80, 96 }, 0, { 0, 0 }, .boxes = { 0 }, true, true, 0 };
    p->crouchingAnim.frames[1] = (AnimationFrame) { {  82, 352, -80, 96 }, 0, { 0, 0 }, .boxes = { 0 }, true, true, 0 };
    p->crouchingAnim.frames[2] = (AnimationFrame) { { 163, 352, -80, 96 }, 0, { 0, 0 }, .boxes = { 0 }, true, true, 0 };

    p->lpAnim.frameCount = 3;
    p->lpAnim.currentFrame = 0;
    p->lpAnim.frameTimeCounter = 0.0f;
    p->lpAnim.stopAtLastFrame = false;
    p->lpAnim.runOnce = true;
    p->lpAnim.finished = false;
    createAnimationFrames( &p->lpAnim, p->lpAnim.frameCount );
    p->lpAnim.frames[0] = (AnimationFrame) { {   1, 1031, -112, 96 }, 0, { 22, 0 }, .boxes = { 0 }, true, true, 0 };
    p->lpAnim.frames[1] = (AnimationFrame) { { 114, 1031, -112, 96 }, 0, { 22, 0 }, .boxes = { 0 }, true, true, 0 };
    p->lpAnim.frames[2] = (AnimationFrame) { {   1, 1031, -112, 96 }, 0, { 22, 0 }, .boxes = { 0 }, true, true, 0 };
    
    p->mpAnim.frameCount = 5;
    p->mpAnim.currentFrame = 0;
    p->mpAnim.frameTimeCounter = 0.0f;
    p->mpAnim.stopAtLastFrame = false;
    p->mpAnim.runOnce = true;
    p->mpAnim.finished = false;
    createAnimationFrames( &p->mpAnim, p->mpAnim.frameCount );
    p->mpAnim.frames[0] = (AnimationFrame) { {   1, 1128, -128, 96 }, 0, { 30, 0 }, .boxes = { 0 }, true, true, 0 };
    p->mpAnim.frames[1] = (AnimationFrame) { { 130, 1128, -128, 96 }, 0, { 30, 0 }, .boxes = { 0 }, true, true, 0 };
    p->mpAnim.frames[2] = (AnimationFrame) { { 259, 1128, -128, 96 }, 0, { 30, 0 }, .boxes = { 0 }, true, true, 0 };
    p->mpAnim.frames[3] = (AnimationFrame) { { 130, 1128, -128, 96 }, 0, { 30, 0 }, .boxes = { 0 }, true, true, 0 };
    p->mpAnim.frames[4] = (AnimationFrame) { {   1, 1128, -128, 96 }, 0, { 30, 0 }, .boxes = { 0 }, true, true, 0 };

    p->hpAnim.frameCount = 5;
    p->hpAnim.currentFrame = 0;
    p->hpAnim.frameTimeCounter = 0.0f;
    p->hpAnim.stopAtLastFrame = false;
    p->hpAnim.runOnce = true;
    p->hpAnim.finished = false;
    createAnimationFrames( &p->hpAnim, p->hpAnim.frameCount );
    p->hpAnim.frames[0] = (AnimationFrame) { {   1, 1128, -128, 96 }, 0, { 30, 0 }, .boxes = { 0 }, true, true, 0 };
    p->hpAnim.frames[1] = (AnimationFrame) { { 130, 1128, -128, 96 }, 0, { 30, 0 }, .boxes = { 0 }, true, true, 0 };
    p->hpAnim.frames[2] = (AnimationFrame) { { 259, 1128, -128, 96 }, 0, { 30, 0 }, .boxes = { 0 }, true, true, 0 };
    p->hpAnim.frames[3] = (AnimationFrame) { { 130, 1128, -128, 96 }, 0, { 30, 0 }, .boxes = { 0 }, true, true, 0 };
    p->hpAnim.frames[4] = (AnimationFrame) { {   1, 1128, -128, 96 }, 0, { 30, 0 }, .boxes = { 0 }, true, true, 0 };
    
    p->lkAnim.frameCount = 3;
    p->lkAnim.currentFrame = 0;
    p->lkAnim.frameTimeCounter = 0.0f;
    p->lkAnim.stopAtLastFrame = false;
    p->lkAnim.runOnce = true;
    p->lkAnim.finished = false;
    createAnimationFrames( &p->lkAnim, p->lkAnim.frameCount );
    p->lkAnim.frames[0] = (AnimationFrame) { {   1, 2001, -128, 96 }, 0, { 0, -5 }, .boxes = { 0 }, true, true, 0 };
    p->lkAnim.frames[1] = (AnimationFrame) { { 130, 2001, -128, 96 }, 0, { 0, -5 }, .boxes = { 0 }, true, true, 0 };
    p->lkAnim.frames[2] = (AnimationFrame) { {   1, 2001, -128, 96 }, 0, { 0, -5 }, .boxes = { 0 }, true, true, 0 };
    
    p->mkAnim.frameCount = 3;
    p->mkAnim.currentFrame = 0;
    p->mkAnim.frameTimeCounter = 0.0f;
    p->mkAnim.stopAtLastFrame = false;
    p->mkAnim.runOnce = true;
    p->mkAnim.finished = false;
    createAnimationFrames( &p->mkAnim, p->mkAnim.frameCount );
    p->mkAnim.frames[0] = (AnimationFrame) { {   1, 2001, -128, 96 }, 0, { 0, -5 }, .boxes = { 0 }, true, true, 0 };
    p->mkAnim.frames[1] = (AnimationFrame) { { 130, 2001, -128, 96 }, 0, { 0, -5 }, .boxes = { 0 }, true, true, 0 };
    p->mkAnim.frames[2] = (AnimationFrame) { {   1, 2001, -128, 96 }, 0, { 0, -5 }, .boxes = { 0 }, true, true, 0 };

    p->hkAnim.frameCount = 5;
    p->hkAnim.currentFrame = 0;
    p->hkAnim.frameTimeCounter = 0.0f;
    p->hkAnim.stopAtLastFrame = false;
    p->hkAnim.runOnce = true;
    p->hkAnim.finished = false;
    createAnimationFrames( &p->hkAnim, p->hkAnim.frameCount );
    p->hkAnim.frames[0] = (AnimationFrame) { {   1, 2098, -128, 96 }, 0, { 20, 0 }, .boxes = { 0 }, true, true, 0 };
    p->hkAnim.frames[1] = (AnimationFrame) { { 130, 2098, -128, 96 }, 0, { 20, 0 }, .boxes = { 0 }, true, true, 0 };
    p->hkAnim.frames[2] = (AnimationFrame) { { 259, 2098, -128, 96 }, 0, { 20, 0 }, .boxes = { 0 }, true, true, 0 };
    p->hkAnim.frames[3] = (AnimationFrame) { { 388, 2098, -128, 96 }, 0, { 20, 0 }, .boxes = { 0 }, true, true, 0 };
    p->hkAnim.frames[4] = (AnimationFrame) { { 517, 2098, -128, 96 }, 0, { 20, 0 }, .boxes = { 0 }, true, true, 0 };

    p->lpCloseAnim.frameCount = 1;
    p->lpCloseAnim.currentFrame = 0;
    p->lpCloseAnim.frameTimeCounter = 0.0f;
    p->lpCloseAnim.stopAtLastFrame = false;
    p->lpCloseAnim.runOnce = true;
    p->lpCloseAnim.finished = false;
    createAnimationFrames( &p->lpCloseAnim, p->lpCloseAnim.frameCount );
    p->lpCloseAnim.frames[0] = (AnimationFrame) { { 1, 1225, -80, 112 }, 0, { 5, 0 }, .boxes = { 0 }, true, true, 0 };
    
    p->mpCloseAnim.frameCount = 5;
    p->mpCloseAnim.currentFrame = 0;
    p->mpCloseAnim.frameTimeCounter = 0.0f;
    p->mpCloseAnim.stopAtLastFrame = false;
    p->mpCloseAnim.runOnce = true;
    p->mpCloseAnim.finished = false;
    createAnimationFrames( &p->mpCloseAnim, p->mpCloseAnim.frameCount );
    p->mpCloseAnim.frames[0] = (AnimationFrame) { {   1, 1338, -96, 96 }, 0, { 16, -1 }, .boxes = { 0 }, true, true, 0 };
    p->mpCloseAnim.frames[1] = (AnimationFrame) { {  98, 1338, -96, 96 }, 0, { 10, -1 }, .boxes = { 0 }, true, true, 0 };
    p->mpCloseAnim.frames[2] = (AnimationFrame) { { 195, 1338, -96, 96 }, 0, { 10, -1 }, .boxes = { 0 }, true, true, 0 };
    p->mpCloseAnim.frames[3] = (AnimationFrame) { {  98, 1338, -96, 96 }, 0, { 10, -1 }, .boxes = { 0 }, true, true, 0 };
    p->mpCloseAnim.frames[4] = (AnimationFrame) { {   1, 1338, -96, 96 }, 0, { 10, -1 }, .boxes = { 0 }, true, true, 0 };

    p->hpCloseAnim.frameCount = 5;
    p->hpCloseAnim.currentFrame = 0;
    p->hpCloseAnim.frameTimeCounter = 0.0f;
    p->hpCloseAnim.stopAtLastFrame = false;
    p->hpCloseAnim.runOnce = true;
    p->hpCloseAnim.finished = false;
    createAnimationFrames( &p->hpCloseAnim, p->hpCloseAnim.frameCount );
    p->hpCloseAnim.frames[0] = (AnimationFrame) { {   1, 1435, -112, 128 }, 0, { 20, 0 }, .boxes = { 0 }, true, true, 0 };
    p->hpCloseAnim.frames[1] = (AnimationFrame) { { 114, 1435, -112, 128 }, 0, { 20, 0 }, .boxes = { 0 }, true, true, 0 };
    p->hpCloseAnim.frames[2] = (AnimationFrame) { { 227, 1435, -112, 128 }, 0, { 20, 0 }, .boxes = { 0 }, true, true, 0 };
    p->hpCloseAnim.frames[3] = (AnimationFrame) { { 114, 1435, -112, 128 }, 0, { 20, 0 }, .boxes = { 0 }, true, true, 0 };
    p->hpCloseAnim.frames[4] = (AnimationFrame) { {   1, 1435, -112, 128 }, 0, { 20, 0 }, .boxes = { 0 }, true, true, 0 };
    
    p->lkCloseAnim.frameCount = 3;
    p->lkCloseAnim.currentFrame = 0;
    p->lkCloseAnim.frameTimeCounter = 0.0f;
    p->lkCloseAnim.stopAtLastFrame = false;
    p->lkCloseAnim.runOnce = true;
    p->lkCloseAnim.finished = false;
    createAnimationFrames( &p->lkCloseAnim, p->lkCloseAnim.frameCount );
    p->lkCloseAnim.frames[0] = (AnimationFrame) { {  1, 2195, -96, 96 }, 0, { 42, 0 }, .boxes = { 0 }, true, true, 0 };
    p->lkCloseAnim.frames[1] = (AnimationFrame) { { 98, 2195, -96, 96 }, 0, { 42, 0 }, .boxes = { 0 }, true, true, 0 };
    p->lkCloseAnim.frames[2] = (AnimationFrame) { {  1, 2195, -96, 96 }, 0, { 42, 0 }, .boxes = { 0 }, true, true, 0 };
    
    p->mkCloseAnim.frameCount = 3;
    p->mkCloseAnim.currentFrame = 0;
    p->mkCloseAnim.frameTimeCounter = 0.0f;
    p->mkCloseAnim.stopAtLastFrame = false;
    p->mkCloseAnim.runOnce = true;
    p->mkCloseAnim.finished = false;
    createAnimationFrames( &p->mkCloseAnim, p->mkCloseAnim.frameCount );
    p->mkCloseAnim.frames[0] = (AnimationFrame) { {  1, 2292, -80, 112 }, 0, { 30, 0 }, .boxes = { 0 }, true, true, 0 };
    p->mkCloseAnim.frames[1] = (AnimationFrame) { { 82, 2292, -80, 112 }, 0, { 30, 0 }, .boxes = { 0 }, true, true, 0 };
    p->mkCloseAnim.frames[2] = (AnimationFrame) { {  1, 2292, -80, 112 }, 0, { 30, 0 }, .boxes = { 0 }, true, true, 0 };

    p->hkCloseAnim.frameCount = 4;
    p->hkCloseAnim.currentFrame = 0;
    p->hkCloseAnim.frameTimeCounter = 0.0f;
    p->hkCloseAnim.stopAtLastFrame = false;
    p->hkCloseAnim.runOnce = true;
    p->hkCloseAnim.finished = false;
    createAnimationFrames( &p->hkCloseAnim, p->hkCloseAnim.frameCount );
    p->hkCloseAnim.frames[0] = (AnimationFrame) { {   1, 2405, -112, 128 }, 0, { 47, 0 }, .boxes = { 0 }, true, true, 0 };
    p->hkCloseAnim.frames[1] = (AnimationFrame) { { 114, 2405, -112, 128 }, 0, { 47, 0 }, .boxes = { 0 }, true, true, 0 };
    p->hkCloseAnim.frames[2] = (AnimationFrame) { { 227, 2405, -112, 128 }, 0, { 47, 0 }, .boxes = { 0 }, true, true, 0 };
    p->hkCloseAnim.frames[3] = (AnimationFrame) { {   1, 2405, -112, 128 }, 0, { 47, 0 }, .boxes = { 0 }, true, true, 0 };

    p->lpCrouchAnim.frameCount = 2;
    p->lpCrouchAnim.currentFrame = 0;
    p->lpCrouchAnim.frameTimeCounter = 0.0f;
    p->lpCrouchAnim.stopAtLastFrame = false;
    p->lpCrouchAnim.runOnce = true;
    p->lpCrouchAnim.finished = false;
    createAnimationFrames( &p->lpCrouchAnim, p->lpCrouchAnim.frameCount );
    p->lpCrouchAnim.frames[0] = (AnimationFrame) { {   1, 1564, -112, 64 }, 0, { 16, 0 }, .boxes = { 0 }, true, true, 0 };
    p->lpCrouchAnim.frames[1] = (AnimationFrame) { { 114, 1564, -112, 64 }, 0, { 16, 0 }, .boxes = { 0 }, true, true, 0 };

    p->mpCrouchAnim.frameCount = 3;
    p->mpCrouchAnim.currentFrame = 0;
    p->mpCrouchAnim.frameTimeCounter = 0.0f;
    p->mpCrouchAnim.stopAtLastFrame = false;
    p->mpCrouchAnim.runOnce = true;
    p->mpCrouchAnim.finished = false;
    createAnimationFrames( &p->mpCrouchAnim, p->mpCrouchAnim.frameCount );
    p->mpCrouchAnim.frames[0] = (AnimationFrame) { {   1, 1629, -112, 64 }, 0, { 16, 0 }, .boxes = { 0 }, true, true, 0 };
    p->mpCrouchAnim.frames[1] = (AnimationFrame) { { 114, 1629, -112, 64 }, 0, { 16, 0 }, .boxes = { 0 }, true, true, 0 };
    p->mpCrouchAnim.frames[2] = (AnimationFrame) { { 227, 1629, -112, 64 }, 0, { 16, 0 }, .boxes = { 0 }, true, true, 0 };

    p->hpCrouchAnim.frameCount = 4;
    p->hpCrouchAnim.currentFrame = 0;
    p->hpCrouchAnim.frameTimeCounter = 0.0f;
    p->hpCrouchAnim.stopAtLastFrame = false;
    p->hpCrouchAnim.runOnce = true;
    p->hpCrouchAnim.finished = false;
    createAnimationFrames( &p->hpCrouchAnim, p->hpCrouchAnim.frameCount );
    p->hpCrouchAnim.frames[0] = (AnimationFrame) { {   1, 1694, -96, 128 }, 0, { 8, 0 }, .boxes = { 0 }, true, true, 0 };
    p->hpCrouchAnim.frames[1] = (AnimationFrame) { {  98, 1694, -96, 128 }, 0, { 8, 0 }, .boxes = { 0 }, true, true, 0 };
    p->hpCrouchAnim.frames[2] = (AnimationFrame) { { 195, 1694, -96, 128 }, 0, { 8, 0 }, .boxes = { 0 }, true, true, 0 };
    p->hpCrouchAnim.frames[3] = (AnimationFrame) { {  98, 1694, -96, 128 }, 0, { 8, 0 }, .boxes = { 0 }, true, true, 0 };

    p->lkCrouchAnim.frameCount = 1;
    p->lkCrouchAnim.currentFrame = 0;
    p->lkCrouchAnim.frameTimeCounter = 0.0f;
    p->lkCrouchAnim.stopAtLastFrame = false;
    p->lkCrouchAnim.runOnce = true;
    p->lkCrouchAnim.finished = false;
    createAnimationFrames( &p->lkCrouchAnim, p->lkCrouchAnim.frameCount );
    p->lkCrouchAnim.frames[0] = (AnimationFrame) { { 1, 2534, -160, 64 }, 0, { 40, 0 }, .boxes = { 0 }, true, true, 0 };

    p->mkCrouchAnim.frameCount = 2;
    p->mkCrouchAnim.currentFrame = 0;
    p->mkCrouchAnim.frameTimeCounter = 0.0f;
    p->mkCrouchAnim.stopAtLastFrame = false;
    p->mkCrouchAnim.runOnce = true;
    p->mkCrouchAnim.finished = false;
    createAnimationFrames( &p->mkCrouchAnim, p->mkCrouchAnim.frameCount );
    p->mkCrouchAnim.frames[0] = (AnimationFrame) { {   1, 2534, -160, 64 }, 0, { 40, 0 }, .boxes = { 0 }, true, true, 0 };
    p->mkCrouchAnim.frames[1] = (AnimationFrame) { { 162, 2534, -160, 64 }, 0, { 40, 0 }, .boxes = { 0 }, true, true, 0 };

    p->hkCrouchAnim.frameCount = 4;
    p->hkCrouchAnim.currentFrame = 0;
    p->hkCrouchAnim.frameTimeCounter = 0.0f;
    p->hkCrouchAnim.stopAtLastFrame = false;
    p->hkCrouchAnim.runOnce = true;
    p->hkCrouchAnim.finished = false;
    createAnimationFrames( &p->hkCrouchAnim, p->hkCrouchAnim.frameCount );
    p->hkCrouchAnim.frames[0] = (AnimationFrame) { {   1, 2599, -144, 64 }, 0, { 32, 0 }, .boxes = { 0 }, true, true, 0 };
    p->hkCrouchAnim.frames[1] = (AnimationFrame) { { 146, 2599, -144, 64 }, 0, { 32, 0 }, .boxes = { 0 }, true, true, 0 };
    p->hkCrouchAnim.frames[2] = (AnimationFrame) { { 291, 2599, -144, 64 }, 0, { 32, 0 }, .boxes = { 0 }, true, true, 0 };
    p->hkCrouchAnim.frames[3] = (AnimationFrame) { { 436, 2599, -144, 64 }, 0, { 32, 0 }, .boxes = { 0 }, true, true, 0 };

    p->lpJumpStraightAnim.frameCount = 2;
    p->lpJumpStraightAnim.currentFrame = 0;
    p->lpJumpStraightAnim.frameTimeCounter = 0.0f;
    p->lpJumpStraightAnim.stopAtLastFrame = false;
    p->lpJumpStraightAnim.runOnce = true;
    p->lpJumpStraightAnim.finished = false;
    createAnimationFrames( &p->lpJumpStraightAnim, p->lpJumpStraightAnim.frameCount );
    p->lpJumpStraightAnim.frames[0] = (AnimationFrame) { {  1, 1823, -96, 80 }, 0, { 8, 0 }, .boxes = { 0 }, true, true, 0 };
    p->lpJumpStraightAnim.frames[1] = (AnimationFrame) { { 98, 1823, -96, 80 }, 0, { 8, 0 }, .boxes = { 0 }, true, true, 0 };

    p->mpJumpStraightAnim.frameCount = 2;
    p->mpJumpStraightAnim.currentFrame = 0;
    p->mpJumpStraightAnim.frameTimeCounter = 0.0f;
    p->mpJumpStraightAnim.stopAtLastFrame = false;
    p->mpJumpStraightAnim.runOnce = true;
    p->mpJumpStraightAnim.finished = false;
    createAnimationFrames( &p->mpJumpStraightAnim, p->mpJumpStraightAnim.frameCount );
    p->mpJumpStraightAnim.frames[0] = (AnimationFrame) { {  1, 1904, -96, 80 }, 0, { 8, 0 }, .boxes = { 0 }, true, true, 0 };
    p->mpJumpStraightAnim.frames[1] = (AnimationFrame) { { 98, 1904, -96, 80 }, 0, { 8, 0 }, .boxes = { 0 }, true, true, 0 };

    p->hpJumpStraightAnim.frameCount = 2;
    p->hpJumpStraightAnim.currentFrame = 0;
    p->hpJumpStraightAnim.frameTimeCounter = 0.0f;
    p->hpJumpStraightAnim.stopAtLastFrame = false;
    p->hpJumpStraightAnim.runOnce = true;
    p->hpJumpStraightAnim.finished = false;
    createAnimationFrames( &p->hpJumpStraightAnim, p->hpJumpStraightAnim.frameCount );
    p->hpJumpStraightAnim.frames[0] = (AnimationFrame) { {  1, 1904, -96, 80 }, 0, { 8, 0 }, .boxes = { 0 }, true, true, 0 };
    p->hpJumpStraightAnim.frames[1] = (AnimationFrame) { { 98, 1904, -96, 80 }, 0, { 8, 0 }, .boxes = { 0 }, true, true, 0 };

    p->lkJumpStraightAnim.frameCount = 1;
    p->lkJumpStraightAnim.currentFrame = 0;
    p->lkJumpStraightAnim.frameTimeCounter = 0.0f;
    p->lkJumpStraightAnim.stopAtLastFrame = false;
    p->lkJumpStraightAnim.runOnce = true;
    p->lkJumpStraightAnim.finished = false;
    createAnimationFrames( &p->lkJumpStraightAnim, p->lkJumpStraightAnim.frameCount );
    p->lkJumpStraightAnim.frames[0] = (AnimationFrame) { { 1, 2664, -80, 96 }, 0, { 0, 0 }, .boxes = { 0 }, true, true, 0 };

    p->mkJumpStraightAnim.frameCount = 1;
    p->mkJumpStraightAnim.currentFrame = 0;
    p->mkJumpStraightAnim.frameTimeCounter = 0.0f;
    p->mkJumpStraightAnim.stopAtLastFrame = false;
    p->mkJumpStraightAnim.runOnce = true;
    p->mkJumpStraightAnim.finished = false;
    createAnimationFrames( &p->mkJumpStraightAnim, p->mkJumpStraightAnim.frameCount );
    p->mkJumpStraightAnim.frames[0] = (AnimationFrame) { { 1, 2664, -80, 96 }, 0, { 0, 0 }, .boxes = { 0 }, true, true, 0 };

    p->hkJumpStraightAnim.frameCount = 4;
    p->hkJumpStraightAnim.currentFrame = 0;
    p->hkJumpStraightAnim.frameTimeCounter = 0.0f;
    p->hkJumpStraightAnim.stopAtLastFrame = false;
    p->hkJumpStraightAnim.runOnce = true;
    p->hkJumpStraightAnim.finished = false;
    createAnimationFrames( &p->hkJumpStraightAnim, p->hkJumpStraightAnim.frameCount );
    p->hkJumpStraightAnim.frames[0] = (AnimationFrame) { {   1, 2761, -96, 112 }, 0, { 8, 0 }, .boxes = { 0 }, true, true, 0 };
    p->hkJumpStraightAnim.frames[1] = (AnimationFrame) { {  98, 2761, -96, 112 }, 0, { 8, 0 }, .boxes = { 0 }, true, true, 0 };
    p->hkJumpStraightAnim.frames[2] = (AnimationFrame) { { 195, 2761, -96, 112 }, 0, { 8, 0 }, .boxes = { 0 }, true, true, 0 };
    p->hkJumpStraightAnim.frames[3] = (AnimationFrame) { { 292, 2761, -96, 112 }, 0, { 8, 0 }, .boxes = { 0 }, true, true, 0 };

    p->lpJumpForwardAnim.frameCount = 2;
    p->lpJumpForwardAnim.currentFrame = 0;
    p->lpJumpForwardAnim.frameTimeCounter = 0.0f;
    p->lpJumpForwardAnim.stopAtLastFrame = false;
    p->lpJumpForwardAnim.runOnce = true;
    p->lpJumpForwardAnim.finished = false;
    createAnimationFrames( &p->lpJumpForwardAnim, p->lpJumpForwardAnim.frameCount );
    p->lpJumpForwardAnim.frames[0] = (AnimationFrame) { {  1, 1823, -96, 80 }, 0, { 8, 0 }, .boxes = { 0 }, true, true, 0 };
    p->lpJumpForwardAnim.frames[1] = (AnimationFrame) { { 98, 1823, -96, 80 }, 0, { 8, 0 }, .boxes = { 0 }, true, true, 0 };

    p->mpJumpForwardAnim.frameCount = 2;
    p->mpJumpForwardAnim.currentFrame = 0;
    p->mpJumpForwardAnim.frameTimeCounter = 0.0f;
    p->mpJumpForwardAnim.stopAtLastFrame = false;
    p->mpJumpForwardAnim.runOnce = true;
    p->mpJumpForwardAnim.finished = false;
    createAnimationFrames( &p->mpJumpForwardAnim, p->mpJumpForwardAnim.frameCount );
    p->mpJumpForwardAnim.frames[0] = (AnimationFrame) { {  1, 1904, -96, 80 }, 0, { 8, 0 }, .boxes = { 0 }, true, true, 0 };
    p->mpJumpForwardAnim.frames[1] = (AnimationFrame) { { 98, 1904, -96, 80 }, 0, { 8, 0 }, .boxes = { 0 }, true, true, 0 };

    p->hpJumpForwardAnim.frameCount = 2;
    p->hpJumpForwardAnim.currentFrame = 0;
    p->hpJumpForwardAnim.frameTimeCounter = 0.0f;
    p->hpJumpForwardAnim.stopAtLastFrame = false;
    p->hpJumpForwardAnim.runOnce = true;
    p->hpJumpForwardAnim.finished = false;
    createAnimationFrames( &p->hpJumpForwardAnim, p->hpJumpForwardAnim.frameCount );
    p->hpJumpForwardAnim.frames[0] = (AnimationFrame) { {  1, 1904, -96, 80 }, 0, { 8, 0 }, .boxes = { 0 }, true, true, 0 };
    p->hpJumpForwardAnim.frames[1] = (AnimationFrame) { { 98, 1904, -96, 80 }, 0, { 8, 0 }, .boxes = { 0 }, true, true, 0 };

    p->lkJumpForwardAnim.frameCount = 3;
    p->lkJumpForwardAnim.currentFrame = 0;
    p->lkJumpForwardAnim.frameTimeCounter = 0.0f;
    p->lkJumpForwardAnim.stopAtLastFrame = false;
    p->lkJumpForwardAnim.runOnce = true;
    p->lkJumpForwardAnim.finished = false;
    createAnimationFrames( &p->lkJumpForwardAnim, p->lkJumpForwardAnim.frameCount );
    p->lkJumpForwardAnim.frames[0] = (AnimationFrame) { {   1, 2874, -80, 80 }, 0, { 0, 0 }, .boxes = { 0 }, true, true, 0 };
    p->lkJumpForwardAnim.frames[1] = (AnimationFrame) { {  82, 2874, -80, 80 }, 0, { 0, 0 }, .boxes = { 0 }, true, true, 0 };
    p->lkJumpForwardAnim.frames[2] = (AnimationFrame) { { 163, 2874, -80, 80 }, 0, { 0, 0 }, .boxes = { 0 }, true, true, 0 };

    p->mkJumpForwardAnim.frameCount = 2;
    p->mkJumpForwardAnim.currentFrame = 0;
    p->mkJumpForwardAnim.frameTimeCounter = 0.0f;
    p->mkJumpForwardAnim.stopAtLastFrame = false;
    p->mkJumpForwardAnim.runOnce = true;
    p->mkJumpForwardAnim.finished = false;
    createAnimationFrames( &p->mkJumpForwardAnim, p->mkJumpForwardAnim.frameCount );
    p->mkJumpForwardAnim.frames[0] = (AnimationFrame) { {   1, 2955, -128, 80 }, 0, { 24, 0 }, .boxes = { 0 }, true, true, 0 };
    p->mkJumpForwardAnim.frames[1] = (AnimationFrame) { { 130, 2955, -128, 80 }, 0, { 24, 0 }, .boxes = { 0 }, true, true, 0 };

    p->hkJumpForwardAnim.frameCount = 2;
    p->hkJumpForwardAnim.currentFrame = 0;
    p->hkJumpForwardAnim.frameTimeCounter = 0.0f;
    p->hkJumpForwardAnim.stopAtLastFrame = false;
    p->hkJumpForwardAnim.runOnce = true;
    p->hkJumpForwardAnim.finished = false;
    createAnimationFrames( &p->hkJumpForwardAnim, p->hkJumpForwardAnim.frameCount );
    p->hkJumpForwardAnim.frames[0] = (AnimationFrame) { {   1, 2955, -128, 80 }, 0, { 24, 0 }, .boxes = { 0 }, true, true, 0 };
    p->hkJumpForwardAnim.frames[1] = (AnimationFrame) { { 130, 2955, -128, 80 }, 0, { 24, 0 }, .boxes = { 0 }, true, true, 0 };

    p->lpJumpBackwardAnim.frameCount = 2;
    p->lpJumpBackwardAnim.currentFrame = 0;
    p->lpJumpBackwardAnim.frameTimeCounter = 0.0f;
    p->lpJumpBackwardAnim.stopAtLastFrame = false;
    p->lpJumpBackwardAnim.runOnce = true;
    p->lpJumpBackwardAnim.finished = false;
    createAnimationFrames( &p->lpJumpBackwardAnim, p->lpJumpBackwardAnim.frameCount );
    p->lpJumpBackwardAnim.frames[0] = (AnimationFrame) { {  1, 1823, -96, 80 }, 0, { 8, 0 }, .boxes = { 0 }, true, true, 0 };
    p->lpJumpBackwardAnim.frames[1] = (AnimationFrame) { { 98, 1823, -96, 80 }, 0, { 8, 0 }, .boxes = { 0 }, true, true, 0 };

    p->mpJumpBackwardAnim.frameCount = 2;
    p->mpJumpBackwardAnim.currentFrame = 0;
    p->mpJumpBackwardAnim.frameTimeCounter = 0.0f;
    p->mpJumpBackwardAnim.stopAtLastFrame = false;
    p->mpJumpBackwardAnim.runOnce = true;
    p->mpJumpBackwardAnim.finished = false;
    createAnimationFrames( &p->mpJumpBackwardAnim, p->mpJumpBackwardAnim.frameCount );
    p->mpJumpBackwardAnim.frames[0] = (AnimationFrame) { {  1, 1904, -96, 80 }, 0, { 8, 0 }, .boxes = { 0 }, true, true, 0 };
    p->mpJumpBackwardAnim.frames[1] = (AnimationFrame) { { 98, 1904, -96, 80 }, 0, { 8, 0 }, .boxes = { 0 }, true, true, 0 };

    p->hpJumpBackwardAnim.frameCount = 2;
    p->hpJumpBackwardAnim.currentFrame = 0;
    p->hpJumpBackwardAnim.frameTimeCounter = 0.0f;
    p->hpJumpBackwardAnim.stopAtLastFrame = false;
    p->hpJumpBackwardAnim.runOnce = true;
    p->hpJumpBackwardAnim.finished = false;
    createAnimationFrames( &p->hpJumpBackwardAnim, p->hpJumpBackwardAnim.frameCount );
    p->hpJumpBackwardAnim.frames[0] = (AnimationFrame) { {  1, 1904, -96, 80 }, 0, { 8, 0 }, .boxes = { 0 }, true, true, 0 };
    p->hpJumpBackwardAnim.frames[1] = (AnimationFrame) { { 98, 1904, -96, 80 }, 0, { 8, 0 }, .boxes = { 0 }, true, true, 0 };

    p->lkJumpBackwardAnim.frameCount = 3;
    p->lkJumpBackwardAnim.currentFrame = 0;
    p->lkJumpBackwardAnim.frameTimeCounter = 0.0f;
    p->lkJumpBackwardAnim.stopAtLastFrame = false;
    p->lkJumpBackwardAnim.runOnce = true;
    p->lkJumpBackwardAnim.finished = false;
    createAnimationFrames( &p->lkJumpBackwardAnim, p->lkJumpBackwardAnim.frameCount );
    p->lkJumpBackwardAnim.frames[0] = (AnimationFrame) { {   1, 2874, -80, 80 }, 0, { 0, 0 }, .boxes = { 0 }, true, true, 0 };
    p->lkJumpBackwardAnim.frames[1] = (AnimationFrame) { {  82, 2874, -80, 80 }, 0, { 0, 0 }, .boxes = { 0 }, true, true, 0 };
    p->lkJumpBackwardAnim.frames[2] = (AnimationFrame) { { 163, 2874, -80, 80 }, 0, { 0, 0 }, .boxes = { 0 }, true, true, 0 };

    p->mkJumpBackwardAnim.frameCount = 2;
    p->mkJumpBackwardAnim.currentFrame = 0;
    p->mkJumpBackwardAnim.frameTimeCounter = 0.0f;
    p->mkJumpBackwardAnim.stopAtLastFrame = false;
    p->mkJumpBackwardAnim.runOnce = true;
    p->mkJumpBackwardAnim.finished = false;
    createAnimationFrames( &p->mkJumpBackwardAnim, p->mkJumpBackwardAnim.frameCount );
    p->mkJumpBackwardAnim.frames[0] = (AnimationFrame) { {   1, 2955, -128, 80 }, 0, { 24, 0 }, .boxes = { 0 }, true, true, 0 };
    p->mkJumpBackwardAnim.frames[1] = (AnimationFrame) { { 130, 2955, -128, 80 }, 0, { 24, 0 }, .boxes = { 0 }, true, true, 0 };

    p->hkJumpBackwardAnim.frameCount = 2;
    p->hkJumpBackwardAnim.currentFrame = 0;
    p->hkJumpBackwardAnim.frameTimeCounter = 0.0f;
    p->hkJumpBackwardAnim.stopAtLastFrame = false;
    p->hkJumpBackwardAnim.runOnce = true;
    p->hkJumpBackwardAnim.finished = false;
    createAnimationFrames( &p->hkJumpBackwardAnim, p->hkJumpBackwardAnim.frameCount );
    p->hkJumpBackwardAnim.frames[0] = (AnimationFrame) { {   1, 2955, -128, 80 }, 0, { 24, 0 }, .boxes = { 0 }, true, true, 0 };
    p->hkJumpBackwardAnim.frames[1] = (AnimationFrame) { { 130, 2955, -128, 80 }, 0, { 24, 0 }, .boxes = { 0 }, true, true, 0 };

    p->hitUpStandingAnim.frameCount = 2;
    p->hitUpStandingAnim.currentFrame = 0;
    p->hitUpStandingAnim.frameTimeCounter = 0.0f;
    p->hitUpStandingAnim.stopAtLastFrame = false;
    p->hitUpStandingAnim.runOnce = true;
    p->hitUpStandingAnim.finished = false;
    createAnimationFrames( &p->hitUpStandingAnim, p->hitUpStandingAnim.frameCount );
    p->hitUpStandingAnim.frames[0] = (AnimationFrame) { {  1, 3829, -96, 96 }, 0, { -16, 0 }, .boxes = { 0 }, true, true, 0 };
    p->hitUpStandingAnim.frames[1] = (AnimationFrame) { { 98, 3829, -96, 96 }, 0, { -16, 0 }, .boxes = { 0 }, true, true, 0 };

    p->hitMidStandingAnim.frameCount = 2;
    p->hitMidStandingAnim.currentFrame = 0;
    p->hitMidStandingAnim.frameTimeCounter = 0.0f;
    p->hitMidStandingAnim.stopAtLastFrame = false;
    p->hitMidStandingAnim.runOnce = true;
    p->hitMidStandingAnim.finished = false;
    createAnimationFrames( &p->hitMidStandingAnim, p->hitMidStandingAnim.frameCount );
    p->hitMidStandingAnim.frames[0] = (AnimationFrame) { {  1, 3732, -96, 96 }, 0, { -16, 0 }, .boxes = { 0 }, true, true, 0 };
    p->hitMidStandingAnim.frames[1] = (AnimationFrame) { { 98, 3732, -96, 96 }, 0, { -16, 0 }, .boxes = { 0 }, true, true, 0 };

    p->hitCrouchAnim.frameCount = 2;
    p->hitCrouchAnim.currentFrame = 0;
    p->hitCrouchAnim.frameTimeCounter = 0.0f;
    p->hitCrouchAnim.stopAtLastFrame = false;
    p->hitCrouchAnim.runOnce = true;
    p->hitCrouchAnim.finished = false;
    createAnimationFrames( &p->hitCrouchAnim, p->hitCrouchAnim.frameCount );
    p->hitCrouchAnim.frames[0] = (AnimationFrame) { {  1, 3926, -96, 80 }, 0, { 0, 0 }, .boxes = { 0 }, true, true, 0 };
    p->hitCrouchAnim.frames[1] = (AnimationFrame) { { 98, 3926, -96, 80 }, 0, { 0, 0 }, .boxes = { 0 }, true, true, 0 };
    
    p->defenceStandingAnim.frameCount = 2;
    p->defenceStandingAnim.currentFrame = 0;
    p->defenceStandingAnim.frameTimeCounter = 0.0f;
    p->defenceStandingAnim.stopAtLastFrame = true;
    p->defenceStandingAnim.runOnce = false;
    p->defenceStandingAnim.finished = false;
    createAnimationFrames( &p->defenceStandingAnim, p->defenceStandingAnim.frameCount );
    p->defenceStandingAnim.frames[0] = (AnimationFrame) { {  1, 3569, -80, 96 }, 0, { 0, 0 }, .boxes = { 0 }, true, true, 0 };
    p->defenceStandingAnim.frames[1] = (AnimationFrame) { { 82, 3569, -80, 96 }, 0, { 0, 0 }, .boxes = { 0 }, true, true, 0 };

    p->defenceCrouchAnim.frameCount = 2;
    p->defenceCrouchAnim.currentFrame = 0;
    p->defenceCrouchAnim.frameTimeCounter = 0.0f;
    p->defenceCrouchAnim.stopAtLastFrame = true;
    p->defenceCrouchAnim.runOnce = false;
    p->defenceCrouchAnim.finished = false;
    createAnimationFrames( &p->defenceCrouchAnim, p->defenceCrouchAnim.frameCount );
    p->defenceCrouchAnim.frames[0] = (AnimationFrame) { {  1, 3666, -80, 65 }, 0, { 0, 0 }, .boxes = { 0 }, true, true, 0 };
    p->defenceCrouchAnim.frames[1] = (AnimationFrame) { { 82, 3666, -80, 65 }, 0, { 0, 0 }, .boxes = { 0 }, true, true, 0 };

    p->lastAnim.frameCount = 0;
    p->lastAnim.currentFrame = 0;
    p->lastAnim.frameTimeCounter = 0.0f;
    p->lastAnim.stopAtLastFrame = false;
    p->lastAnim.frames = NULL;
    p->lastAnim.runOnce = false;
    p->lastAnim.finished = false;

    int animationCount = 0;
    p->animations[PLAYER_STATE_IDLE] = &p->idleAnim;                        animationCount++;
    p->animations[PLAYER_STATE_WALKING_FORWARD] = &p->forwardAnim;          animationCount++;
    p->animations[PLAYER_STATE_WALKING_BACKWARD] = &p->backwardAnim;        animationCount++;
    p->animations[PLAYER_STATE_JUMPING_STRAIGHT] = &p->straightJumpAnim;    animationCount++;
    p->animations[PLAYER_STATE_JUMPING_FORWARD] = &p->forwardJumpAnim;      animationCount++;
    p->animations[PLAYER_STATE_JUMPING_BACKWARD] = &p->backwardJumpAnim;    animationCount++;
    p->animations[PLAYER_STATE_JUMP_COOLDOWN] = &p->jumpCooldownAnim;       animationCount++;
    p->animations[PLAYER_STATE_CROUCHING] = &p->crouchingAnim;              animationCount++;
    p->animations[PLAYER_STATE_LP] = &p->lpAnim;                            animationCount++;
    p->animations[PLAYER_STATE_MP] = &p->mpAnim;                            animationCount++;
    p->animations[PLAYER_STATE_HP] = &p->hpAnim;                            animationCount++;
    p->animations[PLAYER_STATE_LK] = &p->lkAnim;                            animationCount++;
    p->animations[PLAYER_STATE_MK] = &p->mkAnim;                            animationCount++;
    p->animations[PLAYER_STATE_HK] = &p->hkAnim;                            animationCount++;
    p->animations[PLAYER_STATE_LP_CLOSE] = &p->lpCloseAnim;                 animationCount++;
    p->animations[PLAYER_STATE_MP_CLOSE] = &p->mpCloseAnim;                 animationCount++;
    p->animations[PLAYER_STATE_HP_CLOSE] = &p->hpCloseAnim;                 animationCount++;
    p->animations[PLAYER_STATE_LK_CLOSE] = &p->lkCloseAnim;                 animationCount++;
    p->animations[PLAYER_STATE_MK_CLOSE] = &p->mkCloseAnim;                 animationCount++;
    p->animations[PLAYER_STATE_HK_CLOSE] = &p->hkCloseAnim;                 animationCount++;
    p->animations[PLAYER_STATE_LP_CROUCH] = &p->lpCrouchAnim;               animationCount++;
    p->animations[PLAYER_STATE_MP_CROUCH] = &p->mpCrouchAnim;               animationCount++;
    p->animations[PLAYER_STATE_HP_CROUCH] = &p->hpCrouchAnim;               animationCount++;
    p->animations[PLAYER_STATE_LK_CROUCH] = &p->lkCrouchAnim;               animationCount++;
    p->animations[PLAYER_STATE_MK_CROUCH] = &p->mkCrouchAnim;               animationCount++;
    p->animations[PLAYER_STATE_HK_CROUCH] = &p->hkCrouchAnim;               animationCount++;
    p->animations[PLAYER_STATE_LP_JUMP_STRAIGHT] = &p->lpJumpStraightAnim;  animationCount++;
    p->animations[PLAYER_STATE_MP_JUMP_STRAIGHT] = &p->mpJumpStraightAnim;  animationCount++;
    p->animations[PLAYER_STATE_HP_JUMP_STRAIGHT] = &p->hpJumpStraightAnim;  animationCount++;
    p->animations[PLAYER_STATE_LK_JUMP_STRAIGHT] = &p->lkJumpStraightAnim;  animationCount++;
    p->animations[PLAYER_STATE_MK_JUMP_STRAIGHT] = &p->mkJumpStraightAnim;  animationCount++;
    p->animations[PLAYER_STATE_HK_JUMP_STRAIGHT] = &p->hkJumpStraightAnim;  animationCount++;
    p->animations[PLAYER_STATE_LP_JUMP_FORWARD] = &p->lpJumpForwardAnim;    animationCount++;
    p->animations[PLAYER_STATE_MP_JUMP_FORWARD] = &p->mpJumpForwardAnim;    animationCount++;
    p->animations[PLAYER_STATE_HP_JUMP_FORWARD] = &p->hpJumpForwardAnim;    animationCount++;
    p->animations[PLAYER_STATE_LK_JUMP_FORWARD] = &p->lkJumpForwardAnim;    animationCount++;
    p->animations[PLAYER_STATE_MK_JUMP_FORWARD] = &p->mkJumpForwardAnim;    animationCount++;
    p->animations[PLAYER_STATE_HK_JUMP_FORWARD] = &p->hkJumpForwardAnim;    animationCount++;
    p->animations[PLAYER_STATE_LP_JUMP_BACKWARD] = &p->lpJumpBackwardAnim;  animationCount++;
    p->animations[PLAYER_STATE_MP_JUMP_BACKWARD] = &p->mpJumpBackwardAnim;  animationCount++;
    p->animations[PLAYER_STATE_HP_JUMP_BACKWARD] = &p->hpJumpBackwardAnim;  animationCount++;
    p->animations[PLAYER_STATE_LK_JUMP_BACKWARD] = &p->lkJumpBackwardAnim;  animationCount++;
    p->animations[PLAYER_STATE_MK_JUMP_BACKWARD] = &p->mkJumpBackwardAnim;  animationCount++;
    p->animations[PLAYER_STATE_HK_JUMP_BACKWARD] = &p->hkJumpBackwardAnim;  animationCount++;
    p->animations[PLAYER_STATE_HIT_UP_STANDING] = &p->hitUpStandingAnim;    animationCount++;
    p->animations[PLAYER_STATE_HIT_MID_STANDING] = &p->hitMidStandingAnim;  animationCount++;
    p->animations[PLAYER_STATE_HIT_CROUCH] = &p->hitCrouchAnim;             animationCount++;
    p->animations[PLAYER_STATE_DEFENCE_STANDING] = &p->defenceStandingAnim; animationCount++;
    p->animations[PLAYER_STATE_DEFENCE_CROUCH] = &p->defenceCrouchAnim;     animationCount++;
    p->animations[PLAYER_STATE_LAST] = &p->lastAnim;                        animationCount++;
    p->animationCount = animationCount;

    // support animations
    p->onHitAnimation.frameCount = 3;
    p->onHitAnimation.currentFrame = 0;
    p->onHitAnimation.frameTimeCounter = 0.0f;
    p->onHitAnimation.stopAtLastFrame = false;
    p->onHitAnimation.runOnce = true;
    p->onHitAnimation.finished = false;
    createAnimationFrames( &p->onHitAnimation, p->onHitAnimation.frameCount );
    p->onHitAnimation.frames[0] = (AnimationFrame) { {  8, 10, 20, 19 }, 30, { 0, 0 }, .boxes = { 0 }, true, true, 0 };
    p->onHitAnimation.frames[1] = (AnimationFrame) { { 29, 10, 20, 19 }, 30, { 0, 0 }, .boxes = { 0 }, true, true, 0 };
    p->onHitAnimation.frames[2] = (AnimationFrame) { { 71, 10, 20, 19 }, 30, { 0, 0 }, .boxes = { 0 }, true, true, 0 };

    int supportAnimationCount = 0;
    p->supportAnimations[supportAnimationCount++] = &p->onHitAnimation;
    p->supportAnimationCount = supportAnimationCount;

    p->onHitPos = (Vector2) { 0 };
    p->onHitPosActive = false;

}

void initializePlayerKen( float x, float y, Player *p, PlayerStartSide startSide, DurationMode animationDurationMode, bool showBoxes, bool showDebugInfo ) {

    initializePlayerRyu( x, y, p, startSide, animationDurationMode, showBoxes, showDebugInfo );
    p->texture = &rm.kenTexture;
    strcpy( p->name, "Ken" );

}

void destroyPlayer( Player *player ) {
    for ( int i = 0; i < player->animationCount; i++ ) {
        destroyAnimationFrames( player->animations[i] );
    }
    for ( int i = 0; i < player->supportAnimationCount; i++ ) {
        destroyAnimationFrames( player->supportAnimations[i] );
    }
    free( player );
}

void drawPlayer( Player *player ) {

    AnimationFrame *af = getPlayerCurrentAnimationFrame( player );
    drawPlayerAnimationFrame( player, af, (Vector2) { 0 }, WHITE );

    if ( player->showDebugInfo ) {

        DrawCircle( player->pos.x, player->pos.y, 1, ORANGE );
        DrawRectangleLines( player->pos.x - player->dim.x / 2, player->pos.y, player->dim.x, player->dim.y, ORANGE );
        DrawText( TextFormat( "x: %.2f", player->pos.x ), player->pos.x + 5, player->pos.y - 20, 10, BLACK );
        DrawText( TextFormat( "y: %.2f", player->pos.y ), player->pos.x + 5, player->pos.y - 10, 10, BLACK );
        
    }

}

void drawPlayerInputBuffer( Player *player ) {

    int backWidth = 100;

    if ( player->startSide == PLAYER_START_SIDE_LEFT ) {
        DrawRectangleGradientH( 0, 0, backWidth, GetScreenHeight(), Fade( BLACK, 0.7f ), BLANK );
    } else {
        DrawRectangleGradientH( GetScreenWidth() - backWidth, 0, backWidth, GetScreenHeight(), BLANK, Fade( BLACK, 0.7f ) );
    }

    if ( player->inputBufferSize == 0 ) {
        return;
    }

    int margin = 35;
    int step = 35;
    int width = 23;
    int height = width;

    int startX = margin;
    int startY = 120 + step * ( player->inputBufferSize - 1 );
    int lineStart = 0;

    int sliceX = 1;
    int sliceY = 1;
    int sliceW = 23;
    int sliceH = 23;
    int sliceDiv = 1;

    if ( player->startSide == PLAYER_START_SIDE_RIGHT ) {
        startX = GetScreenWidth() - margin;
        lineStart = startX - width;
    }

    int c = 0;

    for ( int i = player->inputBufferHead; i <= player->inputBufferTail; i++ ) {
        int p = i % PLAYER_INPUT_BUFFER_SIZE;
        InputType type = player->inputBuffer[p].type;
        int x = startX - width / 2;
        int y = startY - height / 2 - step * c;
        DrawTexturePro(
            rm.inputIconsTexture,
            (Rectangle) { sliceX + ( sliceW + sliceDiv ) * type, sliceY, sliceW, sliceH },
            (Rectangle) { x, y, width, height },
            (Vector2) { 0 },
            0.0f,
            WHITE
        );
        y = y + height + ( step - height ) / 2;
        DrawLineEx( (Vector2) { lineStart, y }, (Vector2) { lineStart + x + width + 10, y }, 2.0f, Fade( WHITE, 0.5f ) );
        c++;
    }

}

void drawPlayerOnionLayers( Player *player, int xOffset ) {

    Animation *a = getPlayerCurrentAnimation( player );
    AnimationFrame *af = getPlayerCurrentAnimationFrame( player );

    drawPlayer( player );

    // search
    for ( int i = 0; i < a->frameCount; i++ ) {
        if ( af == &a->frames[i] ) {
            for ( int j = 1; j < a->frameCount; j++ ) {
                int next = ( i + j ) % a->frameCount;
                drawPlayerAnimationFrame( player, &(getPlayerCurrentAnimation( player )->frames[next]), (Vector2) { xOffset * j, 0 }, Fade( WHITE, 0.5f ) );
            }
        }
    }
    
}

void drawPlayerAnimationFrame( Player *player, AnimationFrame *af, Vector2 offset, Color tint ) {

    if ( af != NULL ) {
        DrawTexturePro( 
            *player->texture,
            (Rectangle) {
                af->source.x,
                af->source.y,
                player->lookingRight ? af->source.width : -af->source.width,
                af->source.height
            },
            (Rectangle) { 
                offset.x + player->pos.x - fabs( af->source.width ) / 2 + ( player->lookingRight ? af->offset.x : -af->offset.x ),
                offset.y + player->pos.y + player->dim.y - af->source.height + af->offset.y,
                af->source.width,
                af->source.height
            },
            (Vector2) { 0 },
            0.0f,
            tint
        );
    }

    if ( player->showBoxes ) {
        drawPlayerAnimationFrameBoxes( player, af, offset );
    }

}

static void drawPlayerAnimationFrameBoxes( Player *player, AnimationFrame *af, Vector2 offset ) {

    int x = (int) ( player->pos.x + af->boxes.collisionBox.x + offset.x );
    int y = (int) ( player->pos.y + af->boxes.collisionBox.y + offset.y );
    int w = (int) af->boxes.collisionBox.width;
    int h = (int) af->boxes.collisionBox.height;

    if ( !player->lookingRight ) {
        x = (int) ( player->pos.x - af->boxes.collisionBox.x - af->boxes.collisionBox.width + offset.x );
    }

    if ( !( w == 0 && h == 0 ) ) {
        DrawRectangle( x, y, w, h, Fade( GREEN, 0.4 ) );
        DrawRectangleLines( x, y, w, h, GREEN );
    }

    for ( int i = 0; i < af->boxes.hitboxCount; i++ ) {

        Rectangle *r = &af->boxes.hitboxes[i];
        int x = (int) ( player->pos.x + r->x + offset.x );
        int y = (int) ( player->pos.y + r->y + offset.y );
        int w = (int) r->width;
        int h = (int) r->height;

        if ( !player->lookingRight ) {
            x = (int) ( player->pos.x - r->x - r->width + offset.x );
        }

        if ( !( w == 0 && h == 0 ) ) {
            DrawText( TextFormat( "%d", i ), x + 1, y, 5, ColorBrightness( BLUE, -0.7 ) );
            DrawRectangle( x, y, w, h, Fade( BLUE, 0.4 ) );
            DrawRectangleLines( x, y, w, h, BLUE );
        }

    }

    for ( int i = 0; i < af->boxes.hurtboxCount; i++ ) {

        Rectangle *r = &af->boxes.hurtboxes[i];
        int x = (int) ( player->pos.x + r->x + offset.x );
        int y = (int) ( player->pos.y + r->y + offset.y );
        int w = (int) r->width;
        int h = (int) r->height;

        if ( !player->lookingRight ) {
            x = (int) ( player->pos.x - r->x - r->width + offset.x );
        }

        if ( !( w == 0 && h == 0 ) ) {
            DrawText( TextFormat( "%d", i ), x + 1, y, 5, ColorBrightness( RED, -0.7 ) );
            DrawRectangle( x, y, w, h, Fade( RED, 0.4 ) );
            DrawRectangleLines( x, y, w, h, RED );
        }

    }

}

void processInputPlayer( Player *player, Player *opponent, float delta, int currentFrame ) {

    processInputAndFeedInputBuffer( player, currentFrame );

    Animation *activeAnim = NULL;

    // damage in progress: blocks all input
    if ( isHitState( player->state ) ) {
        activeAnim = getPlayerCurrentAnimation( player );
        if ( activeAnim != NULL ) {
            updateAnimation( activeAnim, player->animationDurationMode, delta );
            if ( activeAnim->finished ) {
                player->state = PLAYER_STATE_IDLE;
                resetAnimation( activeAnim );
            }
        }
        return;
    }

    // block in progress: blocks all input until animation finishes
    if ( isDefenceState( player->state ) ) {
        activeAnim = getPlayerCurrentAnimation( player );
        if ( activeAnim != NULL ) {
            updateAnimation( activeAnim, player->animationDurationMode, delta );
            if ( activeAnim->finished ) {
                if ( player->state == PLAYER_STATE_DEFENCE_CROUCH ) {
                    player->state = PLAYER_STATE_CROUCHING;
                } else {
                    player->state = PLAYER_STATE_IDLE;
                }
                resetAnimation( activeAnim );
            }
        }
        return;
    }

    // attack in progress: blocks all input
    if ( isAttackState( player->state ) ) {
        activeAnim = getPlayerCurrentAnimation( player );
    }

    if ( activeAnim != NULL ) {
        updateAnimation( activeAnim, player->animationDurationMode, delta );
        if ( activeAnim->finished ) {
            // is crouch attack?
            if ( isCrouchAttackState( player->state ) ) {
                player->state = PLAYER_STATE_CROUCHING;
                // is jump attack?
            } else if ( isJumpAttackState( player->state ) ) {
                player->state = player->lastState;
            } else {
                player->state = PLAYER_STATE_IDLE;
            }
            resetAnimation( activeAnim );
        }
        return;
    }

    // jump in progress: updates animation and blocks input
    if ( isJumpState( player->state ) ) {
        switch ( player->state ) {
            case PLAYER_STATE_JUMPING_STRAIGHT:
                updateAnimation( &player->straightJumpAnim, player->animationDurationMode, delta );
                break;
            case PLAYER_STATE_JUMPING_FORWARD:
                if ( player->lookingRight ) {
                    updateAnimation( &player->forwardJumpAnim, player->animationDurationMode, delta );
                } else {
                    updateAnimation( &player->backwardJumpAnim, player->animationDurationMode, delta );
                }
                break;
            case PLAYER_STATE_JUMPING_BACKWARD:
                if ( player->lookingRight ) {
                    updateAnimation( &player->backwardJumpAnim, player->animationDurationMode, delta );
                } else {
                    updateAnimation( &player->forwardJumpAnim, player->animationDurationMode, delta );
                }
                break;
            default:
                break;
        }
        // jump attack input (from buffer)
        InputType jumpButton = peekAttackButton( player, currentFrame );

        if ( jumpButton != INPUT_TYPE_NEUTRAL ) {

            PlayerState jumpAttackState = PLAYER_STATE_IDLE;
            Animation *jumpAttackAnim = NULL;

            switch ( jumpButton ) {
                case INPUT_TYPE_LP:
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
                    break;
                case INPUT_TYPE_MP:
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
                    break;
                case INPUT_TYPE_HP:
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
                    break;
                case INPUT_TYPE_LK:
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
                    break;
                case INPUT_TYPE_MK:
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
                    break;
                case INPUT_TYPE_HK:
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
                    break;
                default:
                    break;
            }

            if ( jumpAttackAnim != NULL ) {
                resetAnimation( jumpAttackAnim );
                player->lastState = player->state;   // preserve jump state for recovery after attack
                player->state = jumpAttackState;
            }
        }

        return;
    }

    // jump cooldown in progress: blocks input until animation finishes
    if ( player->state == PLAYER_STATE_JUMP_COOLDOWN ) {
        updateAnimation( &player->jumpCooldownAnim, player->animationDurationMode, delta );
        if ( player->jumpCooldownAnim.finished ) {
            player->state = PLAYER_STATE_IDLE;
            resetAnimation( &player->jumpCooldownAnim );
        }
        player->lastState = player->state;
        return;
    }

    // attack (from buffer)
    // command inputs: checked BEFORE basic attacks (priority)
    InputType cmdButton = INPUT_TYPE_NEUTRAL;
    CommandInput *cmd = checkCommandInputs( player, currentFrame, &cmdButton );

    if ( cmd != NULL ) {

        // ---------------------------------------------------------------
        // COMMAND INPUT MATCHED
        // TODO: replace trace + return with actual state/animation change
        //   - Add new PlayerState entries for each special move
        //   - Add Animation fields to Player for each special
        //   - Set player->state, resetAnimation, player->vel.x = 0, etc.
        //   - For projectiles (hadouken), spawn a Projectile entity
        //
        //   Example when ready:
        //     PlayerState specialState = getSpecialState( cmd, cmdButton );
        //     Animation *specialAnim = getSpecialAnimation( player, specialState );
        //     resetAnimation( specialAnim );
        //     player->vel.x = 0.0f;
        //     player->state = specialState;
        //     player->lastState = player->state;
        //     return;
        // ---------------------------------------------------------------

        const char *cmdName = "UNKNOWN";
        switch ( cmd->type ) {
            case COMMAND_TYPE_HADOUKEN:   cmdName = "HADOUKEN";   break;
            case COMMAND_TYPE_SHORYUKEN:  cmdName = "SHORYUKEN";  break;
            case COMMAND_TYPE_TATSUMAKI:  cmdName = "TATSUMAKI";  break;
        }

        const char *btnName = "?";
        switch ( cmdButton ) {
            case INPUT_TYPE_LP: btnName = "LP"; break;
            case INPUT_TYPE_MP: btnName = "MP"; break;
            case INPUT_TYPE_HP: btnName = "HP"; break;
            case INPUT_TYPE_LK: btnName = "LK"; break;
            case INPUT_TYPE_MK: btnName = "MK"; break;
            case INPUT_TYPE_HK: btnName = "HK"; break;
            default: break;
        }

        trace( "[%s] %s + %s detected! (player: %s, frame: %d)",
               player->lookingRight ? "RIGHT" : "LEFT",
               cmdName, btnName, player->name, currentFrame );

        // for now, fall through to basic attack (remove this when special moves are implemented)
    }

    // basic attacks (from buffer)
    InputType attackButton = peekAttackButton( player, currentFrame );

    if ( attackButton != INPUT_TYPE_NEUTRAL ) {

        PlayerState attackState = PLAYER_STATE_IDLE;
        Animation *attackAnim = NULL;
        float dist = distancePlayer( player, opponent );

        switch ( attackButton ) {
            case INPUT_TYPE_LP:
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
                break;
            case INPUT_TYPE_MP:
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
                break;
            case INPUT_TYPE_HP:
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
                break;
            case INPUT_TYPE_LK:
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
                break;
            case INPUT_TYPE_MK:
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
                break;
            case INPUT_TYPE_HK:
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
                break;
            default:
                break;
        }

        if ( attackAnim != NULL ) {
            resetAnimation( attackAnim );
            player->vel.x = 0.0f;
            player->state = attackState;
            player->lastState = player->state;
            return;
        }
    }

    // jump
    if ( IsKeyDown( player->kb.up.key ) && player->state != PLAYER_STATE_CROUCHING ) {
        if ( IsKeyDown( player->kb.right.key ) ) {
            player->vel.y = -player->jumpSpeed;
            player->vel.x = player->forwardSpeed * 1.6f;
            resetAnimation( &player->forwardJumpAnim );
            player->state = PLAYER_STATE_JUMPING_FORWARD;
        } else if ( IsKeyDown( player->kb.left.key ) ) {
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
    if ( IsKeyDown( player->kb.down.key ) ) {
        if ( player->state != PLAYER_STATE_CROUCHING ) {
            resetAnimation( &player->crouchingAnim );
        }
        player->vel.x = 0.0f;
        player->state = PLAYER_STATE_CROUCHING;
    } else if ( IsKeyDown( player->kb.right.key ) ) {
        player->vel.x = player->forwardSpeed;
        player->state = PLAYER_STATE_WALKING_FORWARD;
    } else if ( IsKeyDown( player->kb.left.key ) ) {
        player->vel.x = -player->backwardSpeed;
        player->state = PLAYER_STATE_WALKING_BACKWARD;
    } else {
        player->vel.x = 0.0f;
        player->state = PLAYER_STATE_IDLE;
    }

    // updates floor animation
    switch ( player->state ) {
        case PLAYER_STATE_IDLE:
            updateAnimation( &player->idleAnim, player->animationDurationMode, delta );
            break;
        case PLAYER_STATE_WALKING_FORWARD:
            if ( player->lookingRight ) {
                updateAnimation( &player->forwardAnim, player->animationDurationMode, delta );
            } else {
                updateAnimation( &player->backwardAnim, player->animationDurationMode, delta );
            }
            break;
        case PLAYER_STATE_WALKING_BACKWARD:
            if ( player->lookingRight ) {
                updateAnimation( &player->backwardAnim, player->animationDurationMode, delta );
            } else {
                updateAnimation( &player->forwardAnim, player->animationDurationMode, delta );
            }
            break;
        case PLAYER_STATE_CROUCHING:
            updateAnimation( &player->crouchingAnim, player->animationDurationMode, delta );
            break;
        default:
            break;
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

    // pushback friction during hit/block stun
    if ( isHitState( player->state ) || isDefenceState( player->state ) ) {
        player->vel.x *= PUSHBACK_DECAY;
        if ( fabs( player->vel.x ) < 1.0f ) {
            player->vel.x = 0.0f;
        }
    }

    if ( player->onHitPosActive ) {
        updateAnimation( &player->onHitAnimation, DURATION_MODE_MILLISECONDS, delta );
        if ( player->onHitAnimation.finished ) {
            player->onHitPosActive = false;
            resetAnimation( &player->onHitAnimation );
        }
    }
    
}

void flipPlayerSide( Player *player ) {
    player->lookingRight = !player->lookingRight;
}

float distancePlayer( Player *player1, Player *player2 ) {
    //return hypotf( player1->pos.x - player2->pos.x, player1->pos.y - player2->pos.y );
    return fabs( player1->pos.x - player2->pos.x ); // only x axis
}

AnimationFrame *getPlayerCurrentAnimationFrame( Player *player ) {
    return getAnimationCurrentFrame( getPlayerCurrentAnimation( player ) );
}

Animation *getPlayerCurrentAnimation( Player *player ) {

    switch ( player->state ) {
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
        default:
            return player->animations[player->state];
    }

}

void resetPlayerAnimations( Player *player ) {
    for ( int i = 0; i < player->animationCount; i++ ) {
        resetAnimation( player->animations[i] );
    }
}

void resolvePlayerOponnentContact( Player *p, Player *o ) {

    AnimationFrame *paf = getPlayerCurrentAnimationFrame( p );
    AnimationFrame *oaf = getPlayerCurrentAnimationFrame( o );

    // skip if defender is already in hitstun or blockstun
    if ( isHitState( o->state ) || isDefenceState( o->state ) ) {
        return;
    }

    if ( !paf->hurtboxesActive ) {
        return;
    }

    for ( int i = 0; i < paf->boxes.hurtboxCount; i++ ) {

        Rectangle *pHurt = &paf->boxes.hurtboxes[i];

        if ( pHurt->width == 0 && pHurt->height == 0 ) {
            continue;
        }

        Rectangle hurtbox = {
            p->pos.x,
            p->pos.y + pHurt->y,
            pHurt->width,
            pHurt->height
        };

        if ( p->lookingRight ) {
            hurtbox.x += pHurt->x;
        } else {
            hurtbox.x -= pHurt->x + pHurt->width;
        }

        // for debug (use in draw)
        //Vector2 phurt = GetWorldToScreen2D( (Vector2) { hurtbox.x, hurtbox.y }, camera );
        //DrawRectangle( phurt.x, phurt.y, pHurt->width * camera.zoom, pHurt->height * camera.zoom, Fade( ORANGE, 0.5f ) );

        for ( int j = 0; j < oaf->boxes.hitboxCount; j++ ) {
            
            Rectangle *oHit = &oaf->boxes.hitboxes[j];
            if ( oHit->width == 0 && oHit->height == 0 ) {
                continue;
            }

            Rectangle hitbox = {
                o->pos.x,
                o->pos.y + oHit->y,
                oHit->width,
                oHit->height
            };

            if ( o->lookingRight ) {
                hitbox.x += oHit->x;
            } else {
                hitbox.x -= oHit->x + oHit->width;
            }

            // for debug (use in draw)
            //Vector2 phit = GetWorldToScreen2D( (Vector2) { hitbox.x, hitbox.y }, camera );
            //DrawRectangle( phit.x, phit.y, oHit->width * camera.zoom, oHit->height * camera.zoom, Fade( PURPLE, 0.5f ) );

            if ( CheckCollisionRecs( hurtbox, hitbox ) ) {

                paf->hurtboxesActive = false;

                // check if defender is blocking:
                //   - must be holding "back" (direction away from attacker)
                //   - must be in a blockable state (idle, walking backward, crouching)
                //   - crouch attacks can only be blocked while crouching
                //   - standing/jump attacks can only be blocked while standing
                bool holdingBack = false;
                if ( p->pos.x < o->pos.x ) {
                    // attacker is to the left: "back" for defender is RIGHT
                    holdingBack = IsKeyDown( o->kb.right.key );
                } else {
                    // attacker is to the right: "back" for defender is LEFT
                    holdingBack = IsKeyDown( o->kb.left.key );
                }

                bool canBlock = o->state == PLAYER_STATE_IDLE ||
                                o->state == PLAYER_STATE_WALKING_FORWARD ||
                                o->state == PLAYER_STATE_WALKING_BACKWARD ||
                                o->state == PLAYER_STATE_CROUCHING;

                bool isCrouchAttack = isCrouchAttackState( p->state );
                bool defenderCrouching = o->state == PLAYER_STATE_CROUCHING;

                // height check: crouch attacks must be blocked crouching, others must be blocked standing
                bool heightMatch = isCrouchAttack ? defenderCrouching : !defenderCrouching;

                bool blocked = holdingBack && canBlock && heightMatch;

                // pushback direction: push defender away from attacker
                float pushDir = ( o->pos.x > p->pos.x ) ? 1.0f : -1.0f;

                if ( blocked ) {
                    // block successful
                    o->state = defenderCrouching ? PLAYER_STATE_DEFENCE_CROUCH : PLAYER_STATE_DEFENCE_STANDING;
                    o->vel.x = pushDir * PUSHBACK_ON_BLOCK;
                    // no damage on normal block (chip damage for specials will be added later)
                } else {
                    // hit connects
                    o->health -= paf->damageOnHurt;
                    o->state = PLAYER_STATE_HIT_UP_STANDING;
                    o->vel.x = pushDir * PUSHBACK_ON_HIT;
                }

                Rectangle inter = getRectangleIntersection( hurtbox, hitbox );
                p->onHitPos = (Vector2) { inter.x + inter.width / 2, inter.y + inter.height / 2 };
                p->onHitPosActive = true;

                return;

            }

        }

    }

}

void drawOnHitPlayerAnimation( Player *p ) {

    if ( !p->onHitPosActive ) {
        return;
    }

    AnimationFrame *af = getAnimationCurrentFrame( &p->onHitAnimation );

    DrawTexturePro( 
        rm.effectsTexture,
        (Rectangle) { af->source.x, af->source.y, af->source.width, af->source.height },
        (Rectangle) { p->onHitPos.x - af->source.width / 2, p->onHitPos.y - af->source.height / 2, af->source.width, af->source.height },
        (Vector2) { 0 },
        0.0f, 
        WHITE
    );

}

static InputType peekAttackButton( Player *p, int currentFrame ) {
    if ( p->inputBufferSize == 0 ) {
        return INPUT_TYPE_NEUTRAL;
    }
    InputBufferEntry *last = &p->inputBuffer[p->inputBufferTail % PLAYER_INPUT_BUFFER_SIZE];
    if ( last->frame == currentFrame && isAttackInput( last->type ) ) {
        return last->type;
    }
    return INPUT_TYPE_NEUTRAL;
}

/**
 * Searches the input buffer for a completed command input.
 * Returns a pointer to the matched CommandInput, or NULL if no match.
 * outButton receives the attack button that completed the command (LP, MP, HP, LK, MK, HK).
 *
 * Matching logic:
 * 1. The most recent buffer entry (this frame) must be an attack button of the required type
 * 2. Searching backwards from the button, the directional sequence must appear in order
 * 3. Extra inputs between sequence steps are tolerated (leniency)
 * 4. All entries must be within the command's frame window
 * 5. Directionals are mirrored if the player is facing left
 *
 * Commands with longer sequences are checked first to avoid
 * a shorter command (hadouken) eating a longer one (shoryuken).
 */
static CommandInput *checkCommandInputs( Player *p, int currentFrame, InputType *outButton ) {

    if ( p->inputBufferSize < 2 ) {
        return NULL;
    }

    // the most recent entry must be an attack button added this frame
    InputBufferEntry *buttonEntry = &p->inputBuffer[p->inputBufferTail % PLAYER_INPUT_BUFFER_SIZE];
    if ( buttonEntry->frame != currentFrame || !isAttackInput( buttonEntry->type ) ) {
        return NULL;
    }

    InputType button = buttonEntry->type;

    // try each command (longer sequences first — sort by sequenceLength desc would be ideal,
    // but for now the order in the commands array determines priority)
    for ( int c = 0; c < p->commandCount; c++ ) {

        CommandInput *cmd = &p->commands[c];

        // check button type requirement
        if ( cmd->requiresPunch && !isPunchInput( button ) ) continue;
        if ( cmd->requiresKick && !isKickInput( button ) ) continue;

        // search backwards through buffer for the directional sequence
        int seqIdx = cmd->sequenceLength - 1;  // start matching from end of sequence
        bool matched = true;

        for ( int i = p->inputBufferTail - 1; i >= p->inputBufferHead && seqIdx >= 0; i-- ) {
            InputBufferEntry *entry = &p->inputBuffer[i % PLAYER_INPUT_BUFFER_SIZE];

            // check time window: entry must be within frameWindow of the button press
            if ( buttonEntry->frame - entry->frame > cmd->frameWindow ) {
                matched = false;
                break;
            }

            // get the expected directional (mirror if facing left)
            InputType expected = cmd->sequence[seqIdx];
            if ( !p->lookingRight ) {
                expected = mirrorDirectional( expected );
            }

            if ( entry->type == expected ) {
                seqIdx--;
            }
            // else: tolerate extra inputs (leniency) — just keep searching
        }

        if ( matched && seqIdx < 0 ) {
            *outButton = button;
            return cmd;
        }
    }

    return NULL;
}

static void processInputAndFeedInputBuffer( Player *p, int currentFrame ) {

    // compute current directional state from held keys
    bool rightDown = IsKeyDown( p->kb.right.key );
    bool leftDown  = IsKeyDown( p->kb.left.key );
    bool downDown  = IsKeyDown( p->kb.down.key );
    bool upDown    = IsKeyDown( p->kb.up.key );

    InputType currentDir = INPUT_TYPE_NEUTRAL;

    if ( rightDown && downDown ) {
        currentDir = INPUT_TYPE_RIGHT_DOWN;
    } else if ( leftDown && downDown ) {
        currentDir = INPUT_TYPE_LEFT_DOWN;
    } else if ( rightDown && upDown ) {
        currentDir = INPUT_TYPE_RIGHT_UP;
    } else if ( leftDown && upDown ) {
        currentDir = INPUT_TYPE_LEFT_UP;
    } else if ( rightDown ) {
        currentDir = INPUT_TYPE_RIGHT;
    } else if ( downDown ) {
        currentDir = INPUT_TYPE_DOWN;
    } else if ( leftDown ) {
        currentDir = INPUT_TYPE_LEFT;
    } else if ( upDown ) {
        currentDir = INPUT_TYPE_UP;
    }

    // register only on state transitions (including neutral)
    if ( currentDir != p->lastDirectionalState ) {
        addInputToPlayerInputBuffer( p, currentDir, currentFrame );
        p->lastDirectionalState = currentDir;
    }

    // buttons: independent from directionals (both can register in the same frame)
    if ( IsKeyPressed( p->kb.lp.key ) ) {
        addInputToPlayerInputBuffer( p, p->kb.lp.type, currentFrame );
    } else if ( IsKeyPressed( p->kb.mp.key ) ) {
        addInputToPlayerInputBuffer( p, p->kb.mp.type, currentFrame );
    } else if ( IsKeyPressed( p->kb.hp.key ) ) {
        addInputToPlayerInputBuffer( p, p->kb.hp.type, currentFrame );
    } else if ( IsKeyPressed( p->kb.lk.key ) ) {
        addInputToPlayerInputBuffer( p, p->kb.lk.type, currentFrame );
    } else if ( IsKeyPressed( p->kb.mk.key ) ) {
        addInputToPlayerInputBuffer( p, p->kb.mk.type, currentFrame );
    } else if ( IsKeyPressed( p->kb.hk.key ) ) {
        addInputToPlayerInputBuffer( p, p->kb.hk.type, currentFrame );
    }

}

static void addInputToPlayerInputBuffer( Player *p, InputType input, int currentFrame ) {

    if ( p->inputBufferSize == 0 ) {
        p->inputBufferHead = 0;
        p->inputBufferTail = 0;
        p->inputBufferSize++;
        p->inputBuffer[p->inputBufferTail] = (InputBufferEntry) { input, currentFrame };
    } else {
        if ( p->inputBufferSize >= PLAYER_INPUT_BUFFER_SIZE ) {
            p->inputBufferHead++;
        } else {
            p->inputBufferSize++;
        }
        p->inputBufferTail++;
        p->inputBuffer[p->inputBufferTail % PLAYER_INPUT_BUFFER_SIZE] = (InputBufferEntry) { input, currentFrame };
    }
}
