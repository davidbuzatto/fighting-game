#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

#include "raylib/raylib.h"
#include "raylib/rlgl.h"

#include "Animation.h"
#include "Macros.h"
#include "Player.h"
#include "Projectile.h"
#include "ResourceManager.h"
#include "Types.h"
#include "Utils.h"

static const bool TRACE_SPECIAL_MOVE = false;

static void drawPlayerAnimationFrameBoxes( Player *player, AnimationFrame *af, Vector2 offset );
static void processInputAndFeedInputBuffer( Player *p, int currentFrame );
static void addInputToPlayerInputBuffer( Player *p, InputType input, int currentFrame );
static InputType peekAttackButton( Player *p, int currentFrame );
static CommandInput *checkCommandInputs( Player *p, int currentFrame, InputType *outButton );

static const int startLineToChangeColors = 0;
static const int endLineToChangeColors = 10000;

Player *createPlayer() {
    Player *p = (Player*) malloc( sizeof( Player ) );
    return p;
}

static void initializePlayerCommon( float x, float y, Player *p, PlayerStartSide startSide, int gamepadId, DurationMode animationDurationMode, bool showBoxes, bool showDebugInfo ) {

    p->pos.x = x;
    p->pos.y = y;
    p->dim.x = 50;
    p->dim.y = 90;

    p->vel = (Vector2) { 0 };
    p->forwardSpeed = 150;
    p->backwardSpeed = 120;
    p->jumpSpeed = 450;
    p->animationDurationMode = animationDurationMode;
    p->state = PLAYER_STATE_IDLE;
    p->lastState = PLAYER_STATE_IDLE;
    p->health = 100;
    p->lookingRight = true;
    p->startSide = startSide;
    p->showBoxes = showBoxes;
    p->showDebugInfo = showDebugInfo;
    p->gamepadId = gamepadId;

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
    initAnimationFrames( p->idleAnim.frames, p->idleAnim.frameCount, 1, 142, -64, 96, 0, 0, false );

    p->forwardAnim.frameCount = 5;
    p->forwardAnim.currentFrame = 0;
    p->forwardAnim.frameTimeCounter = 0.0f;
    p->forwardAnim.stopAtLastFrame = false;
    p->forwardAnim.runOnce = false;
    p->forwardAnim.finished = false;
    createAnimationFrames( &p->forwardAnim, p->forwardAnim.frameCount );
    initAnimationFrames( p->forwardAnim.frames, p->forwardAnim.frameCount, 1, 514, -80, 96, 0, 0, false );

    p->backwardAnim.frameCount = 6;
    p->backwardAnim.currentFrame = 0;
    p->backwardAnim.frameTimeCounter = 0.0f;
    p->backwardAnim.stopAtLastFrame = false;
    p->backwardAnim.runOnce = false;
    p->backwardAnim.finished = false;
    createAnimationFrames( &p->backwardAnim, p->backwardAnim.frameCount );
    initAnimationFrames( p->backwardAnim.frames, p->backwardAnim.frameCount, 1, 611, -80, 96, 0, 0, false );

    p->straightJumpAnim.frameCount = 6;
    p->straightJumpAnim.currentFrame = 0;
    p->straightJumpAnim.frameTimeCounter = 0.0f;
    p->straightJumpAnim.stopAtLastFrame = true;
    p->straightJumpAnim.runOnce = false;
    p->straightJumpAnim.finished = false;
    createAnimationFrames( &p->straightJumpAnim, p->straightJumpAnim.frameCount );
    initAnimationFrames( p->straightJumpAnim.frames, p->straightJumpAnim.frameCount, 1, 821, -64, 112, 0, 0, false );

    p->forwardJumpAnim.frameCount = 7;
    p->forwardJumpAnim.currentFrame = 0;
    p->forwardJumpAnim.frameTimeCounter = 0.0f;
    p->forwardJumpAnim.stopAtLastFrame = true;
    p->forwardJumpAnim.runOnce = false;
    p->forwardJumpAnim.finished = false;
    createAnimationFrames( &p->forwardJumpAnim, p->forwardJumpAnim.frameCount );
    initAnimationFrames( p->forwardJumpAnim.frames, p->forwardJumpAnim.frameCount, 1, 708, -128, 112, 0, 0, false );

    p->backwardJumpAnim.frameCount = 7;
    p->backwardJumpAnim.currentFrame = 0;
    p->backwardJumpAnim.frameTimeCounter = 0.0f;
    p->backwardJumpAnim.stopAtLastFrame = true;
    p->backwardJumpAnim.runOnce = false;
    p->backwardJumpAnim.finished = false;
    createAnimationFrames( &p->backwardJumpAnim, p->backwardJumpAnim.frameCount );
    initAnimationFrames( p->backwardJumpAnim.frames, p->backwardJumpAnim.frameCount, 1, 708, -128, 112, 0, 0, true );

    p->jumpCooldownAnim.frameCount = 1;
    p->jumpCooldownAnim.currentFrame = 0;
    p->jumpCooldownAnim.frameTimeCounter = 0.0f;
    p->jumpCooldownAnim.stopAtLastFrame = false;
    p->jumpCooldownAnim.runOnce = true;
    p->jumpCooldownAnim.finished = false;
    createAnimationFrames( &p->jumpCooldownAnim, p->jumpCooldownAnim.frameCount );
    initAnimationFrames( p->jumpCooldownAnim.frames, p->jumpCooldownAnim.frameCount, 1, 934, -64, 96, 0, 0, false );

    p->crouchingAnim.frameCount = 3;
    p->crouchingAnim.currentFrame = 0;
    p->crouchingAnim.frameTimeCounter = 0.0f;
    p->crouchingAnim.stopAtLastFrame = true;
    p->crouchingAnim.runOnce = false;
    p->crouchingAnim.finished = false;
    createAnimationFrames( &p->crouchingAnim, p->crouchingAnim.frameCount );
    initAnimationFrames( p->crouchingAnim.frames, p->crouchingAnim.frameCount, 1, 352, -80, 96, 0, 0, false );

    p->lpAnim.frameCount = 3;
    p->lpAnim.currentFrame = 0;
    p->lpAnim.frameTimeCounter = 0.0f;
    p->lpAnim.stopAtLastFrame = false;
    p->lpAnim.runOnce = true;
    p->lpAnim.finished = false;
    createAnimationFrames( &p->lpAnim, p->lpAnim.frameCount );
    initAnimationFrames( p->lpAnim.frames, p->lpAnim.frameCount, 1, 1031, -112, 96, 22, 0, false ); // verificar offset
    
    p->mpAnim.frameCount = 5;
    p->mpAnim.currentFrame = 0;
    p->mpAnim.frameTimeCounter = 0.0f;
    p->mpAnim.stopAtLastFrame = false;
    p->mpAnim.runOnce = true;
    p->mpAnim.finished = false;
    createAnimationFrames( &p->mpAnim, p->mpAnim.frameCount );
    initAnimationFrames( p->mpAnim.frames, p->mpAnim.frameCount, 1, 1128, -128, 96, 30, 0, false ); // verificar offset

    p->hpAnim.frameCount = 5;
    p->hpAnim.currentFrame = 0;
    p->hpAnim.frameTimeCounter = 0.0f;
    p->hpAnim.stopAtLastFrame = false;
    p->hpAnim.runOnce = true;
    p->hpAnim.finished = false;
    createAnimationFrames( &p->hpAnim, p->hpAnim.frameCount );
    initAnimationFrames( p->hpAnim.frames, p->hpAnim.frameCount, 1, 1128, -128, 96, 30, 0, false ); // verificar offset (igual acima)
    
    p->lkAnim.frameCount = 3;
    p->lkAnim.currentFrame = 0;
    p->lkAnim.frameTimeCounter = 0.0f;
    p->lkAnim.stopAtLastFrame = false;
    p->lkAnim.runOnce = true;
    p->lkAnim.finished = false;
    createAnimationFrames( &p->lkAnim, p->lkAnim.frameCount );
    initAnimationFrames( p->lkAnim.frames, p->lkAnim.frameCount, 1, 2001, -128, 96, 0, -5, false ); // verificar offset
    
    p->mkAnim.frameCount = 3;
    p->mkAnim.currentFrame = 0;
    p->mkAnim.frameTimeCounter = 0.0f;
    p->mkAnim.stopAtLastFrame = false;
    p->mkAnim.runOnce = true;
    p->mkAnim.finished = false;
    createAnimationFrames( &p->mkAnim, p->mkAnim.frameCount );
    initAnimationFrames( p->mkAnim.frames, p->mkAnim.frameCount, 1, 2001, -128, 96, 0, -5, false ); // verificar offset

    p->hkAnim.frameCount = 5;
    p->hkAnim.currentFrame = 0;
    p->hkAnim.frameTimeCounter = 0.0f;
    p->hkAnim.stopAtLastFrame = false;
    p->hkAnim.runOnce = true;
    p->hkAnim.finished = false;
    createAnimationFrames( &p->hkAnim, p->hkAnim.frameCount );
    initAnimationFrames( p->hkAnim.frames, p->hkAnim.frameCount, 1, 2098, -128, 96, 20, 0, false ); // verificar offset

    p->lpCloseAnim.frameCount = 1;
    p->lpCloseAnim.currentFrame = 0;
    p->lpCloseAnim.frameTimeCounter = 0.0f;
    p->lpCloseAnim.stopAtLastFrame = false;
    p->lpCloseAnim.runOnce = true;
    p->lpCloseAnim.finished = false;
    createAnimationFrames( &p->lpCloseAnim, p->lpCloseAnim.frameCount );
    initAnimationFrames( p->lpCloseAnim.frames, p->lpCloseAnim.frameCount, 1, 1225, -80, 112, 5, 0, false ); // verificar offset
    
    p->mpCloseAnim.frameCount = 5;
    p->mpCloseAnim.currentFrame = 0;
    p->mpCloseAnim.frameTimeCounter = 0.0f;
    p->mpCloseAnim.stopAtLastFrame = false;
    p->mpCloseAnim.runOnce = true;
    p->mpCloseAnim.finished = false;
    createAnimationFrames( &p->mpCloseAnim, p->mpCloseAnim.frameCount );
    initAnimationFrames( p->mpCloseAnim.frames, p->mpCloseAnim.frameCount, 1, 1338, -96, 96, 16, -1, false ); // verificar offset

    p->hpCloseAnim.frameCount = 5;
    p->hpCloseAnim.currentFrame = 0;
    p->hpCloseAnim.frameTimeCounter = 0.0f;
    p->hpCloseAnim.stopAtLastFrame = false;
    p->hpCloseAnim.runOnce = true;
    p->hpCloseAnim.finished = false;
    createAnimationFrames( &p->hpCloseAnim, p->hpCloseAnim.frameCount );
    initAnimationFrames( p->hpCloseAnim.frames, p->hpCloseAnim.frameCount, 1, 1435, -112, 128, 20, 0, false ); // verificar offset
    
    p->lkCloseAnim.frameCount = 3;
    p->lkCloseAnim.currentFrame = 0;
    p->lkCloseAnim.frameTimeCounter = 0.0f;
    p->lkCloseAnim.stopAtLastFrame = false;
    p->lkCloseAnim.runOnce = true;
    p->lkCloseAnim.finished = false;
    createAnimationFrames( &p->lkCloseAnim, p->lkCloseAnim.frameCount );
    initAnimationFrames( p->lkCloseAnim.frames, p->lkCloseAnim.frameCount, 1, 2195, -96, 96, 42, 0, false ); // verificar offset
    
    p->mkCloseAnim.frameCount = 3;
    p->mkCloseAnim.currentFrame = 0;
    p->mkCloseAnim.frameTimeCounter = 0.0f;
    p->mkCloseAnim.stopAtLastFrame = false;
    p->mkCloseAnim.runOnce = true;
    p->mkCloseAnim.finished = false;
    createAnimationFrames( &p->mkCloseAnim, p->mkCloseAnim.frameCount );
    initAnimationFrames( p->mkCloseAnim.frames, p->mkCloseAnim.frameCount, 1, 2292, -80, 112, 30, 0, false ); // verificar offset

    p->hkCloseAnim.frameCount = 4;
    p->hkCloseAnim.currentFrame = 0;
    p->hkCloseAnim.frameTimeCounter = 0.0f;
    p->hkCloseAnim.stopAtLastFrame = false;
    p->hkCloseAnim.runOnce = true;
    p->hkCloseAnim.finished = false;
    createAnimationFrames( &p->hkCloseAnim, p->hkCloseAnim.frameCount );
    initAnimationFrames( p->hkCloseAnim.frames, p->hkCloseAnim.frameCount, 1, 2405, -112, 128, 47, 0, false ); // verificar offset

    p->lpCrouchAnim.frameCount = 2;
    p->lpCrouchAnim.currentFrame = 0;
    p->lpCrouchAnim.frameTimeCounter = 0.0f;
    p->lpCrouchAnim.stopAtLastFrame = false;
    p->lpCrouchAnim.runOnce = true;
    p->lpCrouchAnim.finished = false;
    createAnimationFrames( &p->lpCrouchAnim, p->lpCrouchAnim.frameCount );
    initAnimationFrames( p->lpCrouchAnim.frames, p->lpCrouchAnim.frameCount, 1, 1564, -112, 64, 16, 0, false ); // verificar offset

    p->mpCrouchAnim.frameCount = 3;
    p->mpCrouchAnim.currentFrame = 0;
    p->mpCrouchAnim.frameTimeCounter = 0.0f;
    p->mpCrouchAnim.stopAtLastFrame = false;
    p->mpCrouchAnim.runOnce = true;
    p->mpCrouchAnim.finished = false;
    createAnimationFrames( &p->mpCrouchAnim, p->mpCrouchAnim.frameCount );
    initAnimationFrames( p->mpCrouchAnim.frames, p->mpCrouchAnim.frameCount, 1, 1629, -112, 64, 16, 0, false ); // verificar offset

    p->hpCrouchAnim.frameCount = 4;
    p->hpCrouchAnim.currentFrame = 0;
    p->hpCrouchAnim.frameTimeCounter = 0.0f;
    p->hpCrouchAnim.stopAtLastFrame = false;
    p->hpCrouchAnim.runOnce = true;
    p->hpCrouchAnim.finished = false;
    createAnimationFrames( &p->hpCrouchAnim, p->hpCrouchAnim.frameCount );
    initAnimationFrames( p->hpCrouchAnim.frames, p->hpCrouchAnim.frameCount, 1, 1694, -96, 128, 8, 0, false ); // verificar offset

    p->lkCrouchAnim.frameCount = 1;
    p->lkCrouchAnim.currentFrame = 0;
    p->lkCrouchAnim.frameTimeCounter = 0.0f;
    p->lkCrouchAnim.stopAtLastFrame = false;
    p->lkCrouchAnim.runOnce = true;
    p->lkCrouchAnim.finished = false;
    createAnimationFrames( &p->lkCrouchAnim, p->lkCrouchAnim.frameCount );
    initAnimationFrames( p->lkCrouchAnim.frames, p->lkCrouchAnim.frameCount, 1, 2534, -160, 64, 40, 0, false ); // verificar offset (atualizar sprite)

    p->mkCrouchAnim.frameCount = 2;
    p->mkCrouchAnim.currentFrame = 0;
    p->mkCrouchAnim.frameTimeCounter = 0.0f;
    p->mkCrouchAnim.stopAtLastFrame = false;
    p->mkCrouchAnim.runOnce = true;
    p->mkCrouchAnim.finished = false;
    createAnimationFrames( &p->mkCrouchAnim, p->mkCrouchAnim.frameCount );
    initAnimationFrames( p->mkCrouchAnim.frames, p->mkCrouchAnim.frameCount, 1, 2534, -160, 64, 40, 0, false ); // verificar offset

    p->hkCrouchAnim.frameCount = 4;
    p->hkCrouchAnim.currentFrame = 0;
    p->hkCrouchAnim.frameTimeCounter = 0.0f;
    p->hkCrouchAnim.stopAtLastFrame = false;
    p->hkCrouchAnim.runOnce = true;
    p->hkCrouchAnim.finished = false;
    createAnimationFrames( &p->hkCrouchAnim, p->hkCrouchAnim.frameCount );
    initAnimationFrames( p->hkCrouchAnim.frames, p->hkCrouchAnim.frameCount, 1, 2599, -144, 64, 32, 0, false ); // verificar offset

    p->lpJumpStraightAnim.frameCount = 2;
    p->lpJumpStraightAnim.currentFrame = 0;
    p->lpJumpStraightAnim.frameTimeCounter = 0.0f;
    p->lpJumpStraightAnim.stopAtLastFrame = false;
    p->lpJumpStraightAnim.runOnce = true;
    p->lpJumpStraightAnim.finished = false;
    createAnimationFrames( &p->lpJumpStraightAnim, p->lpJumpStraightAnim.frameCount );
    initAnimationFrames( p->lpJumpStraightAnim.frames, p->lpJumpStraightAnim.frameCount, 1, 1823, -96, 80, 8, 0, false ); // verificar offset

    p->mpJumpStraightAnim.frameCount = 2;
    p->mpJumpStraightAnim.currentFrame = 0;
    p->mpJumpStraightAnim.frameTimeCounter = 0.0f;
    p->mpJumpStraightAnim.stopAtLastFrame = false;
    p->mpJumpStraightAnim.runOnce = true;
    p->mpJumpStraightAnim.finished = false;
    createAnimationFrames( &p->mpJumpStraightAnim, p->mpJumpStraightAnim.frameCount );
    initAnimationFrames( p->mpJumpStraightAnim.frames, p->mpJumpStraightAnim.frameCount, 1, 1904, -96, 80, 8, 0, false ); // verificar offset

    p->hpJumpStraightAnim.frameCount = 2;
    p->hpJumpStraightAnim.currentFrame = 0;
    p->hpJumpStraightAnim.frameTimeCounter = 0.0f;
    p->hpJumpStraightAnim.stopAtLastFrame = false;
    p->hpJumpStraightAnim.runOnce = true;
    p->hpJumpStraightAnim.finished = false;
    createAnimationFrames( &p->hpJumpStraightAnim, p->hpJumpStraightAnim.frameCount );
    initAnimationFrames( p->hpJumpStraightAnim.frames, p->hpJumpStraightAnim.frameCount, 1, 1904, -96, 80, 8, 0, false ); // verificar offset

    p->lkJumpStraightAnim.frameCount = 1;
    p->lkJumpStraightAnim.currentFrame = 0;
    p->lkJumpStraightAnim.frameTimeCounter = 0.0f;
    p->lkJumpStraightAnim.stopAtLastFrame = false;
    p->lkJumpStraightAnim.runOnce = true;
    p->lkJumpStraightAnim.finished = false;
    createAnimationFrames( &p->lkJumpStraightAnim, p->lkJumpStraightAnim.frameCount );
    initAnimationFrames( p->lkJumpStraightAnim.frames, p->lkJumpStraightAnim.frameCount, 1, 2664, -80, 96, 0, 0, false );

    p->mkJumpStraightAnim.frameCount = 1;
    p->mkJumpStraightAnim.currentFrame = 0;
    p->mkJumpStraightAnim.frameTimeCounter = 0.0f;
    p->mkJumpStraightAnim.stopAtLastFrame = false;
    p->mkJumpStraightAnim.runOnce = true;
    p->mkJumpStraightAnim.finished = false;
    createAnimationFrames( &p->mkJumpStraightAnim, p->mkJumpStraightAnim.frameCount );
    initAnimationFrames( p->mkJumpStraightAnim.frames, p->mkJumpStraightAnim.frameCount, 1, 2664, -80, 96, 0, 0, false );

    p->hkJumpStraightAnim.frameCount = 4;
    p->hkJumpStraightAnim.currentFrame = 0;
    p->hkJumpStraightAnim.frameTimeCounter = 0.0f;
    p->hkJumpStraightAnim.stopAtLastFrame = false;
    p->hkJumpStraightAnim.runOnce = true;
    p->hkJumpStraightAnim.finished = false;
    createAnimationFrames( &p->hkJumpStraightAnim, p->hkJumpStraightAnim.frameCount );
    initAnimationFrames( p->hkJumpStraightAnim.frames, p->hkJumpStraightAnim.frameCount, 1, 2761, -96, 112, 8, 0, false ); // verificar offset

    p->lpJumpForwardAnim.frameCount = 2;
    p->lpJumpForwardAnim.currentFrame = 0;
    p->lpJumpForwardAnim.frameTimeCounter = 0.0f;
    p->lpJumpForwardAnim.stopAtLastFrame = false;
    p->lpJumpForwardAnim.runOnce = true;
    p->lpJumpForwardAnim.finished = false;
    createAnimationFrames( &p->lpJumpForwardAnim, p->lpJumpForwardAnim.frameCount );
    initAnimationFrames( p->lpJumpForwardAnim.frames, p->lpJumpForwardAnim.frameCount, 1, 1823, -96, 80, 8, 0, false ); // verificar offset

    p->mpJumpForwardAnim.frameCount = 2;
    p->mpJumpForwardAnim.currentFrame = 0;
    p->mpJumpForwardAnim.frameTimeCounter = 0.0f;
    p->mpJumpForwardAnim.stopAtLastFrame = false;
    p->mpJumpForwardAnim.runOnce = true;
    p->mpJumpForwardAnim.finished = false;
    createAnimationFrames( &p->mpJumpForwardAnim, p->mpJumpForwardAnim.frameCount );
    initAnimationFrames( p->mpJumpForwardAnim.frames, p->mpJumpForwardAnim.frameCount, 1, 1904, -96, 80, 8, 0, false ); // verificar offset

    p->hpJumpForwardAnim.frameCount = 2;
    p->hpJumpForwardAnim.currentFrame = 0;
    p->hpJumpForwardAnim.frameTimeCounter = 0.0f;
    p->hpJumpForwardAnim.stopAtLastFrame = false;
    p->hpJumpForwardAnim.runOnce = true;
    p->hpJumpForwardAnim.finished = false;
    createAnimationFrames( &p->hpJumpForwardAnim, p->hpJumpForwardAnim.frameCount );
    initAnimationFrames( p->hpJumpForwardAnim.frames, p->hpJumpForwardAnim.frameCount, 1, 1904, -96, 80, 8, 0, false ); // verificar offset

    p->lkJumpForwardAnim.frameCount = 3;
    p->lkJumpForwardAnim.currentFrame = 0;
    p->lkJumpForwardAnim.frameTimeCounter = 0.0f;
    p->lkJumpForwardAnim.stopAtLastFrame = false;
    p->lkJumpForwardAnim.runOnce = true;
    p->lkJumpForwardAnim.finished = false;
    createAnimationFrames( &p->lkJumpForwardAnim, p->lkJumpForwardAnim.frameCount );
    initAnimationFrames( p->lkJumpForwardAnim.frames, p->lkJumpForwardAnim.frameCount, 1, 2874, -80, 80, 0, 0, false );

    p->mkJumpForwardAnim.frameCount = 2;
    p->mkJumpForwardAnim.currentFrame = 0;
    p->mkJumpForwardAnim.frameTimeCounter = 0.0f;
    p->mkJumpForwardAnim.stopAtLastFrame = false;
    p->mkJumpForwardAnim.runOnce = true;
    p->mkJumpForwardAnim.finished = false;
    createAnimationFrames( &p->mkJumpForwardAnim, p->mkJumpForwardAnim.frameCount );
    initAnimationFrames( p->mkJumpForwardAnim.frames, p->mkJumpForwardAnim.frameCount, 1, 2955, -128, 80, 24, 0, false ); // verificar offset

    p->hkJumpForwardAnim.frameCount = 2;
    p->hkJumpForwardAnim.currentFrame = 0;
    p->hkJumpForwardAnim.frameTimeCounter = 0.0f;
    p->hkJumpForwardAnim.stopAtLastFrame = false;
    p->hkJumpForwardAnim.runOnce = true;
    p->hkJumpForwardAnim.finished = false;
    createAnimationFrames( &p->hkJumpForwardAnim, p->hkJumpForwardAnim.frameCount );
    initAnimationFrames( p->hkJumpForwardAnim.frames, p->hkJumpForwardAnim.frameCount, 1, 2955, -128, 80, 24, 0, false ); // verificar offset

    p->lpJumpBackwardAnim.frameCount = 2;
    p->lpJumpBackwardAnim.currentFrame = 0;
    p->lpJumpBackwardAnim.frameTimeCounter = 0.0f;
    p->lpJumpBackwardAnim.stopAtLastFrame = false;
    p->lpJumpBackwardAnim.runOnce = true;
    p->lpJumpBackwardAnim.finished = false;
    createAnimationFrames( &p->lpJumpBackwardAnim, p->lpJumpBackwardAnim.frameCount );
    initAnimationFrames( p->lpJumpBackwardAnim.frames, p->lpJumpBackwardAnim.frameCount, 1, 1823, -96, 80, 8, 0, false ); // verificar offset

    p->mpJumpBackwardAnim.frameCount = 2;
    p->mpJumpBackwardAnim.currentFrame = 0;
    p->mpJumpBackwardAnim.frameTimeCounter = 0.0f;
    p->mpJumpBackwardAnim.stopAtLastFrame = false;
    p->mpJumpBackwardAnim.runOnce = true;
    p->mpJumpBackwardAnim.finished = false;
    createAnimationFrames( &p->mpJumpBackwardAnim, p->mpJumpBackwardAnim.frameCount );
    initAnimationFrames( p->mpJumpBackwardAnim.frames, p->mpJumpBackwardAnim.frameCount, 1, 1904, -96, 80, 8, 0, false ); // verificar offset

    p->hpJumpBackwardAnim.frameCount = 2;
    p->hpJumpBackwardAnim.currentFrame = 0;
    p->hpJumpBackwardAnim.frameTimeCounter = 0.0f;
    p->hpJumpBackwardAnim.stopAtLastFrame = false;
    p->hpJumpBackwardAnim.runOnce = true;
    p->hpJumpBackwardAnim.finished = false;
    createAnimationFrames( &p->hpJumpBackwardAnim, p->hpJumpBackwardAnim.frameCount );
    initAnimationFrames( p->hpJumpBackwardAnim.frames, p->hpJumpBackwardAnim.frameCount, 1, 1904, -96, 80, 8, 0, false ); // verificar offset

    p->lkJumpBackwardAnim.frameCount = 3;
    p->lkJumpBackwardAnim.currentFrame = 0;
    p->lkJumpBackwardAnim.frameTimeCounter = 0.0f;
    p->lkJumpBackwardAnim.stopAtLastFrame = false;
    p->lkJumpBackwardAnim.runOnce = true;
    p->lkJumpBackwardAnim.finished = false;
    createAnimationFrames( &p->lkJumpBackwardAnim, p->lkJumpBackwardAnim.frameCount );
    initAnimationFrames( p->lkJumpBackwardAnim.frames, p->lkJumpBackwardAnim.frameCount, 1, 2874, -80, 80, 0, 0, false );

    p->mkJumpBackwardAnim.frameCount = 2;
    p->mkJumpBackwardAnim.currentFrame = 0;
    p->mkJumpBackwardAnim.frameTimeCounter = 0.0f;
    p->mkJumpBackwardAnim.stopAtLastFrame = false;
    p->mkJumpBackwardAnim.runOnce = true;
    p->mkJumpBackwardAnim.finished = false;
    createAnimationFrames( &p->mkJumpBackwardAnim, p->mkJumpBackwardAnim.frameCount );
    initAnimationFrames( p->mkJumpBackwardAnim.frames, p->mkJumpBackwardAnim.frameCount, 1, 2955, -128, 80, 24, 0, false ); // verificar offset

    p->hkJumpBackwardAnim.frameCount = 2;
    p->hkJumpBackwardAnim.currentFrame = 0;
    p->hkJumpBackwardAnim.frameTimeCounter = 0.0f;
    p->hkJumpBackwardAnim.stopAtLastFrame = false;
    p->hkJumpBackwardAnim.runOnce = true;
    p->hkJumpBackwardAnim.finished = false;
    createAnimationFrames( &p->hkJumpBackwardAnim, p->hkJumpBackwardAnim.frameCount );
    initAnimationFrames( p->hkJumpBackwardAnim.frames, p->hkJumpBackwardAnim.frameCount, 1, 2955, -128, 80, 24, 0, false ); // verificar offset

    p->hitUpStandingAnim.frameCount = 2;
    p->hitUpStandingAnim.currentFrame = 0;
    p->hitUpStandingAnim.frameTimeCounter = 0.0f;
    p->hitUpStandingAnim.stopAtLastFrame = false;
    p->hitUpStandingAnim.runOnce = true;
    p->hitUpStandingAnim.finished = false;
    createAnimationFrames( &p->hitUpStandingAnim, p->hitUpStandingAnim.frameCount );
    initAnimationFrames( p->hitUpStandingAnim.frames, p->hitUpStandingAnim.frameCount, 1, 3829, -96, 96, -16, 0, false ); // verificar offset

    p->hitMidStandingAnim.frameCount = 2;
    p->hitMidStandingAnim.currentFrame = 0;
    p->hitMidStandingAnim.frameTimeCounter = 0.0f;
    p->hitMidStandingAnim.stopAtLastFrame = false;
    p->hitMidStandingAnim.runOnce = true;
    p->hitMidStandingAnim.finished = false;
    createAnimationFrames( &p->hitMidStandingAnim, p->hitMidStandingAnim.frameCount );
    initAnimationFrames( p->hitMidStandingAnim.frames, p->hitMidStandingAnim.frameCount, 1, 3732, -96, 96, -16, 0, false ); // verificar offset

    p->hitCrouchAnim.frameCount = 2;
    p->hitCrouchAnim.currentFrame = 0;
    p->hitCrouchAnim.frameTimeCounter = 0.0f;
    p->hitCrouchAnim.stopAtLastFrame = false;
    p->hitCrouchAnim.runOnce = true;
    p->hitCrouchAnim.finished = false;
    createAnimationFrames( &p->hitCrouchAnim, p->hitCrouchAnim.frameCount );
    initAnimationFrames( p->hitCrouchAnim.frames, p->hitCrouchAnim.frameCount, 1, 3926, -96, 80, 0, 0, false );
    
    p->defenceStandingAnim.frameCount = 2;
    p->defenceStandingAnim.currentFrame = 0;
    p->defenceStandingAnim.frameTimeCounter = 0.0f;
    p->defenceStandingAnim.stopAtLastFrame = true;
    p->defenceStandingAnim.runOnce = false;
    p->defenceStandingAnim.finished = false;
    createAnimationFrames( &p->defenceStandingAnim, p->defenceStandingAnim.frameCount );
    initAnimationFrames( p->defenceStandingAnim.frames, p->defenceStandingAnim.frameCount, 1, 3569, -80, 96, 0, 0, false );

    p->defenceCrouchAnim.frameCount = 2;
    p->defenceCrouchAnim.currentFrame = 0;
    p->defenceCrouchAnim.frameTimeCounter = 0.0f;
    p->defenceCrouchAnim.stopAtLastFrame = true;
    p->defenceCrouchAnim.runOnce = false;
    p->defenceCrouchAnim.finished = false;
    createAnimationFrames( &p->defenceCrouchAnim, p->defenceCrouchAnim.frameCount );
    initAnimationFrames( p->defenceCrouchAnim.frames, p->defenceCrouchAnim.frameCount, 1, 3666, -80, 65, 0, 0, false );

    p->lpHadoukenAnim.frameCount = 9;
    p->lpHadoukenAnim.currentFrame = 0;
    p->lpHadoukenAnim.frameTimeCounter = 0.0f;
    p->lpHadoukenAnim.stopAtLastFrame = false;
    p->lpHadoukenAnim.runOnce = true;
    p->lpHadoukenAnim.finished = false;
    createAnimationFrames( &p->lpHadoukenAnim, p->lpHadoukenAnim.frameCount );
    initAnimationFrames( p->lpHadoukenAnim.frames, p->lpHadoukenAnim.frameCount, 1, 3230, -128, 96, 32, 0, false ); // verificar offset
    for ( int i = 4; i < p->lpHadoukenAnim.frameCount; i++ ) {
        p->lpHadoukenAnim.frames[i] = p->lpHadoukenAnim.frames[3];
    }

    p->mpHadoukenAnim.frameCount = 9;
    p->mpHadoukenAnim.currentFrame = 0;
    p->mpHadoukenAnim.frameTimeCounter = 0.0f;
    p->mpHadoukenAnim.stopAtLastFrame = false;
    p->mpHadoukenAnim.runOnce = true;
    p->mpHadoukenAnim.finished = false;
    createAnimationFrames( &p->mpHadoukenAnim, p->mpHadoukenAnim.frameCount );
    initAnimationFrames( p->mpHadoukenAnim.frames, p->mpHadoukenAnim.frameCount, 1, 3230, -128, 96, 32, 0, false ); // verificar offset
    for ( int i = 4; i < p->mpHadoukenAnim.frameCount; i++ ) {
        p->mpHadoukenAnim.frames[i] = p->mpHadoukenAnim.frames[3];
    }

    p->hpHadoukenAnim.frameCount = 9;
    p->hpHadoukenAnim.currentFrame = 0;
    p->hpHadoukenAnim.frameTimeCounter = 0.0f;
    p->hpHadoukenAnim.stopAtLastFrame = false;
    p->hpHadoukenAnim.runOnce = true;
    p->hpHadoukenAnim.finished = false;
    createAnimationFrames( &p->hpHadoukenAnim, p->hpHadoukenAnim.frameCount );
    initAnimationFrames( p->hpHadoukenAnim.frames, p->hpHadoukenAnim.frameCount, 1, 3230, -128, 96, 32, 0, false ); // verificar offset
    for ( int i = 4; i < p->hpHadoukenAnim.frameCount; i++ ) {
        p->hpHadoukenAnim.frames[i] = p->hpHadoukenAnim.frames[3];
    }

    p->lpShoryukenAnim.frameCount = 6;
    p->lpShoryukenAnim.currentFrame = 0;
    p->lpShoryukenAnim.frameTimeCounter = 0.0f;
    p->lpShoryukenAnim.stopAtLastFrame = false;
    p->lpShoryukenAnim.runOnce = true;
    p->lpShoryukenAnim.finished = false;
    createAnimationFrames( &p->lpShoryukenAnim, p->lpShoryukenAnim.frameCount );
    initAnimationFrames( p->lpShoryukenAnim.frames, p->lpShoryukenAnim.frameCount, 1, 3327, -96, 128, 16, 0, false ); // verificar offset

    p->mpShoryukenAnim.frameCount = 6;
    p->mpShoryukenAnim.currentFrame = 0;
    p->mpShoryukenAnim.frameTimeCounter = 0.0f;
    p->mpShoryukenAnim.stopAtLastFrame = false;
    p->mpShoryukenAnim.runOnce = true;
    p->mpShoryukenAnim.finished = false;
    createAnimationFrames( &p->mpShoryukenAnim, p->mpShoryukenAnim.frameCount );
    initAnimationFrames( p->mpShoryukenAnim.frames, p->mpShoryukenAnim.frameCount, 1, 3327, -96, 128, 16, 0, false ); // verificar offset

    p->hpShoryukenAnim.frameCount = 6;
    p->hpShoryukenAnim.currentFrame = 0;
    p->hpShoryukenAnim.frameTimeCounter = 0.0f;
    p->hpShoryukenAnim.stopAtLastFrame = false;
    p->hpShoryukenAnim.runOnce = true;
    p->hpShoryukenAnim.finished = false;
    createAnimationFrames( &p->hpShoryukenAnim, p->hpShoryukenAnim.frameCount );
    initAnimationFrames( p->hpShoryukenAnim.frames, p->hpShoryukenAnim.frameCount, 1, 3327, -96, 128, 16, 0, false ); // verificar offset

    p->lkTatsumakiAnim.frameCount = 10;
    p->lkTatsumakiAnim.currentFrame = 0;
    p->lkTatsumakiAnim.frameTimeCounter = 0.0f;
    p->lkTatsumakiAnim.stopAtLastFrame = false;
    p->lkTatsumakiAnim.runOnce = true;
    p->lkTatsumakiAnim.finished = false;
    createAnimationFrames( &p->lkTatsumakiAnim, p->lkTatsumakiAnim.frameCount );
    initAnimationFrames( p->lkTatsumakiAnim.frames, p->lkTatsumakiAnim.frameCount, 1, 3456, -128, 112, 0, 0, false ); // verificar offset

    p->mkTatsumakiAnim.frameCount = 22;
    p->mkTatsumakiAnim.currentFrame = 0;
    p->mkTatsumakiAnim.frameTimeCounter = 0.0f;
    p->mkTatsumakiAnim.stopAtLastFrame = false;
    p->mkTatsumakiAnim.runOnce = true;
    p->mkTatsumakiAnim.finished = false;
    createAnimationFrames( &p->mkTatsumakiAnim, p->mkTatsumakiAnim.frameCount );
    initAnimationFrames( p->mkTatsumakiAnim.frames, p->mkTatsumakiAnim.frameCount, 1, 3456, -128, 112, 0, 0, false ); // verificar offset
    int start = 9;
    for ( int i = 21; i >= 19; i-- ) {
        p->mkTatsumakiAnim.frames[i] = p->mkTatsumakiAnim.frames[start--];
    }
    for ( int i = 0; i < 3; i++ ) {
        for ( int j = 3; j <= 6; j++ ) {
            p->mkTatsumakiAnim.frames[j+4*(i+1)] = p->mkTatsumakiAnim.frames[j];
        }
    }

    p->hkTatsumakiAnim.frameCount = 26;
    p->hkTatsumakiAnim.currentFrame = 0;
    p->hkTatsumakiAnim.frameTimeCounter = 0.0f;
    p->hkTatsumakiAnim.stopAtLastFrame = false;
    p->hkTatsumakiAnim.runOnce = true;
    p->hkTatsumakiAnim.finished = false;
    createAnimationFrames( &p->hkTatsumakiAnim, p->hkTatsumakiAnim.frameCount );
    initAnimationFrames( p->hkTatsumakiAnim.frames, p->hkTatsumakiAnim.frameCount, 1, 3456, -128, 112, 0, 0, false ); // verificar offset
    start = 9;
    for ( int i = 25; i >= 23; i-- ) {
        p->hkTatsumakiAnim.frames[i] = p->hkTatsumakiAnim.frames[start--];
    }
    for ( int i = 0; i < 4; i++ ) {
        for ( int j = 3; j <= 6; j++ ) {
            p->hkTatsumakiAnim.frames[j+4*(i+1)] = p->hkTatsumakiAnim.frames[j];
        }
    }

    p->fallingAnim.frameCount = 4;
    p->fallingAnim.currentFrame = 0;
    p->fallingAnim.frameTimeCounter = 0.0f;
    p->fallingAnim.stopAtLastFrame = true;
    p->fallingAnim.runOnce = false;
    p->fallingAnim.finished = false;
    createAnimationFrames( &p->fallingAnim, p->fallingAnim.frameCount );
    initAnimationFrames( p->fallingAnim.frames, p->fallingAnim.frameCount, 1, 4217, -128, 96, 0, 0, false );

    p->gettingUpAnim.frameCount = 4;
    p->gettingUpAnim.currentFrame = 0;
    p->gettingUpAnim.frameTimeCounter = 0.0f;
    p->gettingUpAnim.stopAtLastFrame = true;
    p->gettingUpAnim.runOnce = false;
    p->gettingUpAnim.finished = false;
    createAnimationFrames( &p->gettingUpAnim, p->gettingUpAnim.frameCount );
    initAnimationFrames( p->gettingUpAnim.frames, p->gettingUpAnim.frameCount, 1, 4395, -112, 128, 0, 0, false );

    p->victory1Anim.frameCount = 3;
    p->victory1Anim.currentFrame = 0;
    p->victory1Anim.frameTimeCounter = 0.0f;
    p->victory1Anim.stopAtLastFrame = true;
    p->victory1Anim.runOnce = false;
    p->victory1Anim.finished = false;
    createAnimationFrames( &p->victory1Anim, p->victory1Anim.frameCount );
    initAnimationFrames( p->victory1Anim.frames, p->victory1Anim.frameCount, 1, 4524, -64, 128, 0, 0, false );

    p->victory2Anim.frameCount = 2;
    p->victory2Anim.currentFrame = 0;
    p->victory2Anim.frameTimeCounter = 0.0f;
    p->victory2Anim.stopAtLastFrame = true;
    p->victory2Anim.runOnce = false;
    p->victory2Anim.finished = false;
    createAnimationFrames( &p->victory2Anim, p->victory2Anim.frameCount );
    initAnimationFrames( p->victory2Anim.frames, p->victory2Anim.frameCount, 1, 4653, -64, 112, 0, 0, false );

    p->fallingLoseAnim.frameCount = 5;
    p->fallingLoseAnim.currentFrame = 0;
    p->fallingLoseAnim.frameTimeCounter = 0.0f;
    p->fallingLoseAnim.stopAtLastFrame = true;
    p->fallingLoseAnim.runOnce = false;
    p->fallingLoseAnim.finished = false;
    createAnimationFrames( &p->fallingLoseAnim, p->fallingLoseAnim.frameCount );
    initAnimationFrames( p->fallingLoseAnim.frames, p->fallingLoseAnim.frameCount, 1, 4314, -128, 80, 32, 0, false );

    p->timeOverAnim.frameCount = 1;
    p->timeOverAnim.currentFrame = 0;
    p->timeOverAnim.frameTimeCounter = 0.0f;
    p->timeOverAnim.stopAtLastFrame = true;
    p->timeOverAnim.runOnce = false;
    p->timeOverAnim.finished = false;
    createAnimationFrames( &p->timeOverAnim, p->timeOverAnim.frameCount );
    initAnimationFrames( p->timeOverAnim.frames, p->timeOverAnim.frameCount, 1, 239, -64, 112, 0, 0, false );

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
    p->animations[PLAYER_STATE_SPECIAL_LP_HADOUKEN] = &p->lpHadoukenAnim;   animationCount++;
    p->animations[PLAYER_STATE_SPECIAL_MP_HADOUKEN] = &p->mpHadoukenAnim;   animationCount++;
    p->animations[PLAYER_STATE_SPECIAL_HP_HADOUKEN] = &p->hpHadoukenAnim;   animationCount++;
    p->animations[PLAYER_STATE_SPECIAL_LP_SHORYUKEN] = &p->lpShoryukenAnim; animationCount++;
    p->animations[PLAYER_STATE_SPECIAL_MP_SHORYUKEN] = &p->mpShoryukenAnim; animationCount++;
    p->animations[PLAYER_STATE_SPECIAL_HP_SHORYUKEN] = &p->hpShoryukenAnim; animationCount++;
    p->animations[PLAYER_STATE_SPECIAL_LK_TATSUMAKI] = &p->lkTatsumakiAnim; animationCount++;
    p->animations[PLAYER_STATE_SPECIAL_MK_TATSUMAKI] = &p->mkTatsumakiAnim; animationCount++;
    p->animations[PLAYER_STATE_SPECIAL_HK_TATSUMAKI] = &p->hkTatsumakiAnim; animationCount++;
    p->animations[PLAYER_STATE_FALLING] = &p->fallingAnim;                  animationCount++;
    p->animations[PLAYER_STATE_GETTING_UP] = &p->gettingUpAnim;             animationCount++;
    p->animations[PLAYER_STATE_VICTORY_1] = &p->victory1Anim;               animationCount++;
    p->animations[PLAYER_STATE_VICTORY_2] = &p->victory2Anim;               animationCount++;
    p->animations[PLAYER_STATE_FALLING_LOSE] = &p->fallingLoseAnim;         animationCount++;
    p->animations[PLAYER_STATE_TIMEOVER] = &p->timeOverAnim;                animationCount++;
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

    p->onBlockAnimation.frameCount = 5;
    p->onBlockAnimation.currentFrame = 0;
    p->onBlockAnimation.frameTimeCounter = 0.0f;
    p->onBlockAnimation.stopAtLastFrame = false;
    p->onBlockAnimation.runOnce = true;
    p->onBlockAnimation.finished = false;
    createAnimationFrames( &p->onBlockAnimation, p->onBlockAnimation.frameCount );
    p->onBlockAnimation.frames[0] = (AnimationFrame) { {  12, 106, 32, 32 }, 30, { 0, 0 }, .boxes = { 0 }, true, true, 0 };
    p->onBlockAnimation.frames[1] = (AnimationFrame) { {  45, 106, 32, 32 }, 30, { 0, 0 }, .boxes = { 0 }, true, true, 0 };
    p->onBlockAnimation.frames[2] = (AnimationFrame) { {  78, 106, 32, 32 }, 30, { 0, 0 }, .boxes = { 0 }, true, true, 0 };
    p->onBlockAnimation.frames[3] = (AnimationFrame) { { 111, 106, 32, 32 }, 30, { 0, 0 }, .boxes = { 0 }, true, true, 0 };
    p->onBlockAnimation.frames[4] = (AnimationFrame) { { 144, 106, 32, 32 }, 30, { 0, 0 }, .boxes = { 0 }, true, true, 0 };

    int supportAnimationCount = 0;
    p->supportAnimations[supportAnimationCount++] = &p->onHitAnimation;
    p->supportAnimations[supportAnimationCount++] = &p->onBlockAnimation;
    p->supportAnimationCount = supportAnimationCount;

    p->onHitPos = (Vector2) { 0 };
    p->onHitPosActive = false;

    p->onBlockPos = (Vector2) { 0 };
    p->onBlockPosActive = false;

}

void initializePlayerRyu( float x, float y, Player *p, PlayerStartSide startSide, int gamepadId, DurationMode animationDurationMode, bool showBoxes, bool showDebugInfo ) {

    initializePlayerCommon( x, y, p, startSide, gamepadId, animationDurationMode, showBoxes, showDebugInfo );
    p->type = PLAYER_TYPE_RYU;
    p->baseSpriteMap = &rm.ryuSpriteMapTexture;
    p->specialMovesSpriteMap = &rm.ryuSpecialMovesSpriteMapTexture;
    p->pallete = &rm.ryuPalleteImage;
    p->currentSpriteMap = createTextureFromTextureReplacingColor( *(p->baseSpriteMap), NULL, NULL, 0, startLineToChangeColors, endLineToChangeColors );
    strcpy( p->name, "Ryu" );

    p->sounds.attackLowSound = rm.ryuAttackLowSound;
    p->sounds.attackMidSound = rm.ryuAttackMidSound;
    p->sounds.attackHighSound = rm.ryuAttackHighSound;
    p->sounds.hitSound = rm.ryuHitSound;
    p->sounds.hadoukenSound = rm.ryuHadoukenSound;
    p->sounds.shoryukenSound = rm.ryuShoryukenSound;
    p->sounds.tatsumakiSound = rm.ryuTatsumakiSound;

    p->projectile = createProjectile( p->specialMovesSpriteMap, animationDurationMode );

}

void initializePlayerKen( float x, float y, Player *p, PlayerStartSide startSide, int gamepadId, DurationMode animationDurationMode, bool showBoxes, bool showDebugInfo ) {

    initializePlayerCommon( x, y, p, startSide, gamepadId, animationDurationMode, showBoxes, showDebugInfo );
    p->type = PLAYER_TYPE_KEN;
    p->baseSpriteMap = &rm.kenSpriteMapTexture;
    p->specialMovesSpriteMap = &rm.kenSpecialMovesSpriteMapTexture;
    p->pallete = &rm.kenPalleteImage;
    p->currentSpriteMap = createTextureFromTextureReplacingColor( *(p->baseSpriteMap), NULL, NULL, 0, startLineToChangeColors, endLineToChangeColors );
    strcpy( p->name, "Ken" );

    p->sounds.attackLowSound = rm.kenAttackLowSound;
    p->sounds.attackMidSound = rm.kenAttackMidSound;
    p->sounds.attackHighSound = rm.kenAttackHighSound;
    p->sounds.hitSound = rm.kenHitSound;
    p->sounds.hadoukenSound = rm.kenHadoukenSound;
    p->sounds.shoryukenSound = rm.kenShoryukenSound;
    p->sounds.tatsumakiSound = rm.kenTatsumakiSound;

    p->projectile = createProjectile( p->specialMovesSpriteMap, animationDurationMode );

}

void destroyPlayer( Player *player ) {
    for ( int i = 0; i < player->animationCount; i++ ) {
        destroyAnimationFrames( player->animations[i] );
    }
    for ( int i = 0; i < player->supportAnimationCount; i++ ) {
        destroyAnimationFrames( player->supportAnimations[i] );
    }
    free( player->projectile );
    UnloadTexture( player->currentSpriteMap );
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

void drawPlayerShadow( Player *player, float floorY, float shear, float scaleY ) {

    float dy = floorY - ( player->pos.y + player->dim.y );

    AnimationFrame *af = getPlayerCurrentAnimationFrame( player );
    drawPlayerAnimationFrameForShadow( player, af, (Vector2) { 0 }, Fade( BLACK, 0.5f - dy / 400 ), floorY - dy / 10, shear, scaleY );

    /*DrawEllipse( 
        player->pos.x, 
        floorY - 5, 
        player->dim.x - 15 + dy / 10, 
        10 + dy / 20, 
        Fade( BLACK, 0.5f - dy / 200 )
    );*/

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
            player->currentSpriteMap,
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

void drawPlayerAnimationFrameForShadow( Player *player, AnimationFrame *af, Vector2 offset, Color tint, float floorY, float shear, float scaleY ) {

    if ( af == NULL ) return;

    float srcWidth  = fabs( af->source.width );
    float srcHeight = af->source.height;
    float texW      = (float) player->currentSpriteMap.width;
    float texH      = (float) player->currentSpriteMap.height;

    // destination size: height flattened by scaleY
    float destW = srcWidth;
    float destH = srcHeight * scaleY;

    // X position: same logic as the normal draw
    float destX = offset.x + player->pos.x - srcWidth / 2.0f + ( player->lookingRight ? af->offset.x : -af->offset.x );

    // Y position: shadow base anchored to the floor (floorY)
    float destY = floorY - destH;

    // UV coordinates of the source rect in the spritesheet
    float u0 = af->source.x / texW;
    float u1 = ( af->source.x + srcWidth ) / texW;
    float v0 = af->source.y / texH;
    float v1 = ( af->source.y + srcHeight ) / texH;

    // flip UVs horizontally when the sprite should be mirrored
    // (same logic as drawPlayerAnimationFrame: lookingRight uses negative source.width)
    if ( ( player->lookingRight ? af->source.width : -af->source.width ) < 0 ) {
        float tmp = u0; u0 = u1; u1 = tmp;
    }

    rlSetTexture( player->currentSpriteMap.id );
    rlBegin( RL_QUADS );
        rlNormal3f( 0.0f, 0.0f, 1.0f );
        rlColor4ub( tint.r, tint.g, tint.b, tint.a );

        // top-left  (shifted right by shear)
        rlTexCoord2f( u0, v0 );
        rlVertex2f( destX + shear, destY );

        // bottom-left  (anchored to the floor)
        rlTexCoord2f( u0, v1 );
        rlVertex2f( destX, destY + destH );

        // bottom-right  (anchored to the floor)
        rlTexCoord2f( u1, v1 );
        rlVertex2f( destX + destW, destY + destH );

        // top-right  (shifted right by shear)
        rlTexCoord2f( u1, v0 );
        rlVertex2f( destX + destW + shear, destY );
    rlEnd();
    rlSetTexture( 0 );

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

    // falling: blocks all input
    if ( player->state == PLAYER_STATE_FALLING || player->state == PLAYER_STATE_GETTING_UP ) {

        activeAnim = getPlayerCurrentAnimation( player );

        if ( activeAnim != NULL ) {
            updateAnimation( activeAnim, player->animationDurationMode, delta );
        }

        if ( activeAnim->finished ) {
            if ( player->state == PLAYER_STATE_FALLING ) {
                player->state = PLAYER_STATE_GETTING_UP;
                player->vel.x = 0.0f;
                player->vel.y = 0.0f;
            } else {
                player->state = PLAYER_STATE_IDLE;
            }
            resetAnimation( activeAnim );
        }

        return;

    }

    // special move in progress: blocks all input
    if ( isSpecialMoveState( player->state ) ) {

        activeAnim = getPlayerCurrentAnimation( player );

        if ( activeAnim != NULL ) {

            updateAnimation( activeAnim, player->animationDurationMode, delta );

            if ( !player->projectile->active && !player->projectile->runImpactAnim && activeAnim->currentFrame == 5 ) {
                if ( player->state == PLAYER_STATE_SPECIAL_LP_HADOUKEN  ) {
                    setupProjectile( 
                        player->projectile, 
                        PROJECTILE_TYPE_LOW, 
                        15,
                        player->pos.x + ( player->lookingRight ? 80 : -80 ), 
                        player->pos.y + 30, 
                        player->lookingRight ? 100 : -100,
                        0
                    );
                } else if ( player->state == PLAYER_STATE_SPECIAL_MP_HADOUKEN  ) {
                    setupProjectile( 
                        player->projectile, 
                        PROJECTILE_TYPE_MID, 
                        20,
                        player->pos.x + ( player->lookingRight ? 80 : -80 ), 
                        player->pos.y + 30, 
                        player->lookingRight ? 150 : -150,
                        0
                    );
                } else if ( player->state == PLAYER_STATE_SPECIAL_HP_HADOUKEN  ) {
                    setupProjectile( 
                        player->projectile, 
                        PROJECTILE_TYPE_HIGH, 
                        25,
                        player->pos.x + ( player->lookingRight ? 80 : -80 ), 
                        player->pos.y + 30, 
                        player->lookingRight ? 200 : - 200,
                        0
                    );
                }
            }

            if ( activeAnim->finished ) {
                player->state = PLAYER_STATE_IDLE;
                resetAnimation( activeAnim );
            }

        }

        return;
    }

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
                    PlaySound( player->sounds.attackLowSound );
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
                    PlaySound( player->sounds.attackMidSound );
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
                    PlaySound( player->sounds.attackHighSound );
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
                    PlaySound( player->sounds.attackLowSound );
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
                    PlaySound( player->sounds.attackMidSound );
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
                    PlaySound( player->sounds.attackHighSound );
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

        // TODO: needs to be generalized (in the future)

        PlayerState specialState = PLAYER_STATE_LAST;

        const char *cmdName = "UNKNOWN";

        bool isHadouken = false;
        bool isShoryuken = false;
        bool isTatsumaki = false;
        Sound soundToPlay;

        switch ( cmd->type ) {
            case COMMAND_TYPE_HADOUKEN:
                cmdName = "HADOUKEN";
                isHadouken = true;
                soundToPlay = player->sounds.hadoukenSound;
                break;
            case COMMAND_TYPE_SHORYUKEN:
                cmdName = "SHORYUKEN";
                specialState = PLAYER_STATE_SPECIAL_LP_SHORYUKEN;
                isShoryuken = true;
                soundToPlay = player->sounds.shoryukenSound;
                break;
            case COMMAND_TYPE_TATSUMAKI:
                cmdName = "TATSUMAKI";
                specialState = PLAYER_STATE_SPECIAL_LK_TATSUMAKI;
                isTatsumaki = true;
                soundToPlay = player->sounds.tatsumakiSound;
                break;
        }

        
        float velX = 0.0f;
        float velY = 0.0f;

        const char *btnName = "?";

        switch ( cmdButton ) {
            case INPUT_TYPE_LP:
                btnName = "LP";
                if ( isHadouken ) {
                    specialState = PLAYER_STATE_SPECIAL_LP_HADOUKEN;
                } else if ( isShoryuken ) {
                    specialState = PLAYER_STATE_SPECIAL_LP_SHORYUKEN;
                    velX = 10.0f;
                    velY = -300.0f;
                }
                break;
            case INPUT_TYPE_MP:
                btnName = "MP";
                if ( isHadouken ) {
                    specialState = PLAYER_STATE_SPECIAL_MP_HADOUKEN;
                } else if ( isShoryuken ) {
                    specialState = PLAYER_STATE_SPECIAL_MP_SHORYUKEN;
                    velX = 15.0f;
                    velY = -400.0f;
                }
                break;
            case INPUT_TYPE_HP:
                btnName = "HP";
                if ( isHadouken ) {
                    specialState = PLAYER_STATE_SPECIAL_HP_HADOUKEN;
                } else if ( isShoryuken ) {
                    specialState = PLAYER_STATE_SPECIAL_HP_SHORYUKEN;
                    velX = 20.0f;
                    velY = -500.0f;
                }
                break;
            case INPUT_TYPE_LK:
                btnName = "LK";
                if ( isTatsumaki ) {
                    specialState = PLAYER_STATE_SPECIAL_LK_TATSUMAKI;
                    velX = 80.0f;
                }
                break;
            case INPUT_TYPE_MK:
                btnName = "MK";
                if ( isTatsumaki ) {
                    specialState = PLAYER_STATE_SPECIAL_MK_TATSUMAKI;
                    velX = 160.0f;
                }
                break;
            case INPUT_TYPE_HK:
                btnName = "HK";
                if ( isTatsumaki ) {
                    specialState = PLAYER_STATE_SPECIAL_HK_TATSUMAKI;
                    velX = 200.0f;
                }
                break;
            default: break;
        }

        if ( TRACE_SPECIAL_MOVE ) {
            trace( "[%s] %s + %s detected! (player: %s, frame: %d)",
                player->lookingRight ? "RIGHT" : "LEFT",
                cmdName, btnName, player->name, currentFrame );
        }

        if ( specialState != PLAYER_STATE_LAST ) {
            if ( ( specialState == PLAYER_STATE_SPECIAL_LP_HADOUKEN ||
                   specialState == PLAYER_STATE_SPECIAL_MP_HADOUKEN || 
                   specialState == PLAYER_STATE_SPECIAL_HP_HADOUKEN ) && player->projectile->active ) {
                return;
            }
            PlaySound( soundToPlay );
            player->state = specialState;
            player->vel.x = player->lookingRight ? velX : -velX;
            player->vel.y = velY;
            resetAnimation( getPlayerCurrentAnimation( player ) );
            player->lastState = player->state;
        }

        return;

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
                PlaySound( player->sounds.attackLowSound );
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
                PlaySound( player->sounds.attackMidSound );
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
                PlaySound( player->sounds.attackHighSound );
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
                PlaySound( player->sounds.attackLowSound );
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
                PlaySound( player->sounds.attackMidSound );
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
                PlaySound( player->sounds.attackHighSound );
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
    if ( ( IsKeyDown( player->kb.up.key ) || isGamepadButtonDown( player->gamepadId, player->kb.up.gamepadButton ) ) && player->state != PLAYER_STATE_CROUCHING ) {
        if ( IsKeyDown( player->kb.right.key ) || isGamepadButtonDown( player->gamepadId, player->kb.right.gamepadButton ) ) {
            player->vel.y = -player->jumpSpeed;
            player->vel.x = player->forwardSpeed * 1.6f;
            resetAnimation( &player->forwardJumpAnim );
            player->state = PLAYER_STATE_JUMPING_FORWARD;
        } else if ( IsKeyDown( player->kb.left.key ) || isGamepadButtonDown( player->gamepadId, player->kb.left.gamepadButton ) ) {
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
    if ( IsKeyDown( player->kb.down.key ) || isGamepadButtonDown( player->gamepadId, player->kb.down.gamepadButton ) ) {
        if ( player->state != PLAYER_STATE_CROUCHING ) {
            resetAnimation( &player->crouchingAnim );
        }
        player->vel.x = 0.0f;
        player->state = PLAYER_STATE_CROUCHING;
    } else if ( IsKeyDown( player->kb.right.key ) || isGamepadButtonDown( player->gamepadId, player->kb.right.gamepadButton ) ) {
        player->vel.x = player->forwardSpeed;
        player->state = PLAYER_STATE_WALKING_FORWARD;
    } else if ( IsKeyDown( player->kb.left.key ) || isGamepadButtonDown( player->gamepadId, player->kb.left.gamepadButton ) ) {
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

void updatePlayer( Player *player, Player *opponent, Camera2D camera, float gravity, float delta ) {

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
        updateAnimation( &player->onHitAnimation, player->animationDurationMode, delta );
        if ( player->onHitAnimation.finished ) {
            player->onHitPosActive = false;
            resetAnimation( &player->onHitAnimation );
        }
    }
    
    if ( player->onBlockPosActive ) {
        updateAnimation( &player->onBlockAnimation, player->animationDurationMode, delta );
        if ( player->onBlockAnimation.finished ) {
            player->onBlockPosActive = false;
            resetAnimation( &player->onBlockAnimation );
        }
    }

    updateProjectile( player->projectile, camera, delta );
    
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
                    holdingBack = IsKeyDown( o->kb.right.key ) || isGamepadButtonDown( o->gamepadId, o->kb.right.gamepadButton );
                } else {
                    // attacker is to the right: "back" for defender is LEFT
                    holdingBack = IsKeyDown( o->kb.left.key ) || isGamepadButtonDown( o->gamepadId, o->kb.left.gamepadButton );
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

                    // chip damage for specials
                    if ( isSpecialMoveState( p->state ) ) {
                        o->health -= 1;
                    }

                    Rectangle inter = GetCollisionRec( hurtbox, hitbox );
                    p->onBlockPos = (Vector2) { inter.x + inter.width / 2, inter.y + inter.height / 2 };
                    p->onBlockPosActive = true;

                } else {

                    // hit connects
                    o->health -= paf->damageOnHurt;
                    o->state = PLAYER_STATE_HIT_UP_STANDING;
                    o->vel.x = pushDir * PUSHBACK_ON_HIT;

                    Rectangle inter = GetCollisionRec( hurtbox, hitbox );
                    p->onHitPos = (Vector2) { inter.x + inter.width / 2, inter.y + inter.height / 2 };
                    p->onHitPosActive = true;

                    if ( isBringDownMoveState( p->state ) ) {
                        o->state = PLAYER_STATE_FALLING;
                        o->vel.y = -100.0f;
                    }

                }

                PlaySound( p->sounds.hitSound );

                return;

            }

        }

    }

}

void resolvePlayerOponnentProjectileContact( Player *p, Player *o ) {

    if ( !p->projectile->active ) {
        return;
    }

    AnimationFrame *paf = getPlayerCurrentAnimationFrame( p );
    AnimationFrame *oaf = getPlayerCurrentAnimationFrame( o );

    // skip if defender is already in hitstun or blockstun
    if ( isHitState( o->state ) || isDefenceState( o->state ) ) {
        return;
    }

    Projectile *pProj = p->projectile;
    Projectile *oProj = o->projectile;

    Rectangle pProjHurt = {
        pProj->pos.x + pProj->hurtbox.x - pProj->hurtbox.width / 2,
        pProj->pos.y + pProj->hurtbox.y - pProj->hurtbox.height / 2,
        pProj->hurtbox.width,
        pProj->hurtbox.height
    };

    if ( oProj->active ) {

        Rectangle oProjHurt = {
            oProj->pos.x + oProj->hurtbox.x - oProj->hurtbox.width / 2,
            oProj->pos.y + oProj->hurtbox.y - oProj->hurtbox.height / 2,
            oProj->hurtbox.width,
            oProj->hurtbox.height
        };

        if ( CheckCollisionRecs( pProjHurt, oProjHurt ) ) {
            pProj->runImpactAnim = true;
            pProj->active = false;
            oProj->runImpactAnim = true;
            oProj->active = false;
            PlaySound( p->sounds.hitSound );
            return;
        }

    }

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

        if ( CheckCollisionRecs( pProjHurt, hitbox ) ) {

            paf->hurtboxesActive = false;

            bool holdingBack = false;
            if ( p->pos.x < o->pos.x ) {
                holdingBack = IsKeyDown( o->kb.right.key ) || isGamepadButtonDown( o->gamepadId, o->kb.right.gamepadButton );
            } else {
                holdingBack = IsKeyDown( o->kb.left.key ) || isGamepadButtonDown( o->gamepadId, o->kb.left.gamepadButton );
            }

            bool canBlock = o->state == PLAYER_STATE_IDLE ||
                            o->state == PLAYER_STATE_WALKING_FORWARD ||
                            o->state == PLAYER_STATE_WALKING_BACKWARD ||
                            o->state == PLAYER_STATE_CROUCHING;

            bool isCrouchAttack = isCrouchAttackState( p->state );
            bool defenderCrouching = o->state == PLAYER_STATE_CROUCHING;
            bool heightMatch = isCrouchAttack ? defenderCrouching : !defenderCrouching;
            bool blocked = holdingBack && canBlock && heightMatch;

            float pushDir = ( o->pos.x > p->pos.x ) ? 1.0f : -1.0f;

            if ( blocked ) {

                o->state = defenderCrouching ? PLAYER_STATE_DEFENCE_CROUCH : PLAYER_STATE_DEFENCE_STANDING;
                o->vel.x = pushDir * PUSHBACK_ON_BLOCK;

                // chip damage for specials
                o->health -= 1;

                Rectangle inter = GetCollisionRec( pProjHurt, hitbox );
                p->onBlockPos = (Vector2) { inter.x + inter.width / 2, inter.y + inter.height / 2 };
                p->onBlockPosActive = true;

            } else {

                o->health -= pProj->damageOnHurt;
                o->state = PLAYER_STATE_HIT_UP_STANDING;
                o->vel.x = pushDir * PUSHBACK_ON_HIT;

                Rectangle inter = GetCollisionRec( pProjHurt, hitbox );
                p->onHitPos = (Vector2) { inter.x + inter.width / 2, inter.y + inter.height / 2 };
                p->onHitPosActive = true;

            }

            pProj->runImpactAnim = true;
            pProj->active = false;

            PlaySound( p->sounds.hitSound );

            return;

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

void drawOnBlockPlayerAnimation( Player *p ) {

    if ( !p->onBlockPosActive ) {
        return;
    }

    AnimationFrame *af = getAnimationCurrentFrame( &p->onBlockAnimation );

    DrawTexturePro( 
        rm.effectsTexture,
        (Rectangle) { af->source.x, af->source.y, af->source.width, af->source.height },
        (Rectangle) { p->onBlockPos.x - af->source.width / 2, p->onBlockPos.y - af->source.height / 2, af->source.width, af->source.height },
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

    // try each command (longer sequences first - sort by sequenceLength desc would be ideal,
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
    bool rightDown = IsKeyDown( p->kb.right.key ) || isGamepadButtonDown( p->gamepadId, p->kb.right.gamepadButton );
    bool leftDown  = IsKeyDown( p->kb.left.key )  || isGamepadButtonDown( p->gamepadId, p->kb.left.gamepadButton );
    bool downDown  = IsKeyDown( p->kb.down.key )  || isGamepadButtonDown( p->gamepadId, p->kb.down.gamepadButton );
    bool upDown    = IsKeyDown( p->kb.up.key )    || isGamepadButtonDown( p->gamepadId, p->kb.up.gamepadButton );

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
    if ( IsKeyPressed( p->kb.lp.key ) || isGamepadButtonPressed( p->gamepadId, p->kb.lp.gamepadButton ) ) {
        addInputToPlayerInputBuffer( p, p->kb.lp.type, currentFrame );
    } else if ( IsKeyPressed( p->kb.mp.key ) || isGamepadButtonPressed( p->gamepadId, p->kb.mp.gamepadButton ) ) {
        addInputToPlayerInputBuffer( p, p->kb.mp.type, currentFrame );
    } else if ( IsKeyPressed( p->kb.hp.key ) || isGamepadButtonPressed( p->gamepadId, p->kb.hp.gamepadButton ) ) {
        addInputToPlayerInputBuffer( p, p->kb.hp.type, currentFrame );
    } else if ( IsKeyPressed( p->kb.lk.key ) || isGamepadButtonPressed( p->gamepadId, p->kb.lk.gamepadButton ) ) {
        addInputToPlayerInputBuffer( p, p->kb.lk.type, currentFrame );
    } else if ( IsKeyPressed( p->kb.mk.key ) || isGamepadButtonPressed( p->gamepadId, p->kb.mk.gamepadButton ) ) {
        addInputToPlayerInputBuffer( p, p->kb.mk.type, currentFrame );
    } else if ( IsKeyPressed( p->kb.hk.key ) || isGamepadButtonPressed( p->gamepadId, p->kb.hk.gamepadButton ) ) {
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

void drawPlayerProjectile( Player *p ) {
    drawProjectile( p->projectile );
}

void changePlayerPallete( Player *p, int palleteNumber, int palleteColorLimit ) {

    int spacing = 16;
    int startX = 2;
    int startY = 2;

    if ( startY + spacing * palleteNumber > p->pallete->height ) {
        return;
    }

    Color sourceColors[20];
    Color targetColors[20];
    int colorCount = 0;

    if ( palleteColorLimit > 20 ) {
        palleteColorLimit = 20;
    }

    int imageColorLimit = p->pallete->width / spacing;

    if ( palleteColorLimit > imageColorLimit ) {
        palleteColorLimit = imageColorLimit;
    }

    for ( int i = 0; i < palleteColorLimit; i++ ) {
        sourceColors[i] = GetImageColor( *(p->pallete), startX + spacing * i, startY );
        targetColors[i] = GetImageColor( *(p->pallete), startX + spacing * i, startY + spacing * palleteNumber );
        colorCount++;
    }

    Texture2D newSpriteMap = createTextureFromTextureReplacingColor( 
        *(p->baseSpriteMap),
        sourceColors,
        targetColors,
        colorCount,
        startLineToChangeColors,
        endLineToChangeColors
    );

    UnloadTexture( p->currentSpriteMap );
    p->currentSpriteMap = newSpriteMap;

}