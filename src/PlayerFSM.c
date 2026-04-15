/**
 * @file PlayerFSM.c
 * @author Prof. Dr. David Buzatto
 * @brief Player finite state machine: input processing and per-frame update.
 *
 * @copyright Copyright (c) 2026
 */
#include <stdbool.h>
#include <stddef.h>
#include <math.h>

#include "raylib/raylib.h"

#include "Animation.h"
#include "Macros.h"
#include "PlayerAnimation.h"
#include "PlayerFSM.h"
#include "PlayerInput.h"
#include "Projectile.h"
#include "Types.h"
#include "Utils.h"

static const bool TRACE_SPECIAL_MOVE = false;

void processInputPlayer( Player *player, Player *opponent, float delta, int currentFrame, bool discardInput ) {

    if ( !discardInput ) {
        processInputAndFeedInputBuffer( player, currentFrame );
    }

    Animation *activeAnim = NULL;

    // victory and lose
    if ( player->state >= PLAYER_STATE_VICTORY_1 && player->state <= PLAYER_STATE_TIMEOVER ) {
        activeAnim = getPlayerCurrentAnimation( player );
        if ( activeAnim != NULL ) {
            updateAnimation( activeAnim, player->animationDurationMode, delta );
        }
        // animations will be reseted in GameWindow
        return;
    }

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

    if ( !discardInput ) {

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
