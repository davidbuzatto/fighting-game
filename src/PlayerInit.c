/**
 * @file PlayerInit.c
 * @author Prof. Dr. David Buzatto
 * @brief Player creation, initialization and destruction implementation.
 *
 * @copyright Copyright (c) 2026
 */
#include <stdlib.h>
#include <string.h>

#include "raylib/raylib.h"

#include "Animation.h"
#include "PlayerInit.h"
#include "Projectile.h"
#include "ResourceManager.h"
#include "Types.h"
#include "Utils.h"

static const int startLineToChangeColors = 0;
static const int endLineToChangeColors = 10000;

static void setupPlayerCommands( Player *p ) {

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

}

static void setupPlayerAttackDistances( Player *p ) {

    p->lpCloseTriggerDist = 50;
    p->mpCloseTriggerDist = 60;
    p->hpCloseTriggerDist = 60;
    p->lkCloseTriggerDist = 60;
    p->mkCloseTriggerDist = 50;
    p->hkCloseTriggerDist = 60;

}

static void setupPlayerAnimations( Player *p ) {

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

}

static void setupPlayerAnimationMap( Player *p ) {

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

static void initializePlayerCommon( float x, float y, Player *p, PlayerStartSide startSide, int gamepadId, DurationMode animationDurationMode, bool showBoxes, bool showDebugInfo ) {

    p->pos.x = x;
    p->pos.y = y;
    p->dim.x = 50;
    p->dim.y = 90;

    // previous
    //p->dim.x = 80;
    //p->dim.y = 100;

    p->vel = (Vector2) { 0 };
    p->forwardSpeed = 150;
    p->backwardSpeed = 120;
    p->jumpSpeed = 450;
    p->roundsWon = 0;
    p->animationDurationMode = animationDurationMode;
    p->state = PLAYER_STATE_IDLE;
    p->lastState = PLAYER_STATE_IDLE;
    p->health = 100;
    if ( startSide == PLAYER_START_SIDE_LEFT ) {
        p->lookingRight = true;
    } else {
        p->lookingRight = false;
    }
    p->startSide = startSide;
    p->showBoxes = showBoxes;
    p->showDebugInfo = showDebugInfo;
    p->gamepadId = gamepadId;

    p->inputBufferHead = -1;
    p->inputBufferTail = -1;
    p->inputBufferSize = 0;
    p->lastDirectionalState = INPUT_TYPE_NEUTRAL;

    setupPlayerCommands( p );
    setupPlayerAttackDistances( p );
    setupPlayerAnimations( p );
    setupPlayerAnimationMap( p );

}

Player *createPlayer() {
    Player *p = (Player*) malloc( sizeof( Player ) );
    return p;
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
