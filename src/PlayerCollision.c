/**
 * @file PlayerCollision.c
 * @author Prof. Dr. David Buzatto
 * @brief Player-opponent contact resolution implementation.
 *
 * @copyright Copyright (c) 2026
 */
#include "raylib/raylib.h"

#include "PlayerAnimation.h"
#include "PlayerCollision.h"
#include "Types.h"
#include "Utils.h"

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
