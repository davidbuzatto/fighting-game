/**
 * @file GameCollision.c
 * @author Prof. Dr. David Buzatto
 * @brief Game-level collision resolution implementation.
 *
 * @copyright Copyright (c) 2026
 */
#include <math.h>
#include <stddef.h>

#include "raylib/raylib.h"

#include "GameCollision.h"
#include "Player.h"
#include "PlayerAnimation.h"
#include "Types.h"
#include "Utils.h"

bool player1RightPlayer2 = false;
bool needsToFlipPlayers = false;

void resolveCollisionPlayerStage( Player *player, GameWorld *gw ) {

    float half = player->dim.x / 2.0f;
    float zoom = gw->camera.zoom;
    float offsetX = gw->camera.offset.x;
    float screenW = GetScreenWidth();
    float targetX = gw->camera.target.x;

    // stage hard limits
    float stageMinX = half;
    float stageMaxX = gw->back01Texture->width - half;

    // camera visible limits (world coordinates)
    float camMinX = targetX - offsetX / zoom + half;
    float camMaxX = targetX + ( screenW - offsetX ) / zoom - half;

    // effective limit: tightest of stage boundary and camera edge
    float minX = fmaxf( stageMinX, camMinX );
    float maxX = fminf( stageMaxX, camMaxX );

    if ( player->pos.x < minX ) {
        player->pos.x = minX;
        if ( player->vel.x < 0 ) {
            player->vel.x = 0.0f;
        }
    } else if ( player->pos.x > maxX ) {
        player->pos.x = maxX;
        if ( player->vel.x > 0 ) {
            player->vel.x = 0.0f;
        }
    }

    if ( player->pos.y + player->dim.y > gw->floor.y ) {
        player->pos.y = gw->floor.y - player->dim.y;
        player->vel.y = 0.0f;
        if ( isAirborneState( player->state ) ) {
            resetPlayerAnimations( player );
            player->vel.x = 0.0f;
            player->state = PLAYER_STATE_JUMP_COOLDOWN;
        }
    }

}

void resolvePlayerPlayerCollision( Player *p1, Player *p2 ) {

    AnimationFrame *af1 = getPlayerCurrentAnimationFrame( p1 );
    AnimationFrame *af2 = getPlayerCurrentAnimationFrame( p2 );

    if ( af1 == NULL || af2 == NULL ) {
        return;
    }

    Rectangle cb1 = af1->boxes.collisionBox;
    Rectangle cb2 = af2->boxes.collisionBox;

    // skip if either collision box is empty (not yet defined)
    if ( ( cb1.width == 0 && cb1.height == 0 ) || ( cb2.width == 0 && cb2.height == 0 ) ) {
        return;
    }

    // convert to world coordinates (same facing-aware logic as hitbox/hurtbox)
    Rectangle wcb1 = {
        p1->lookingRight ? p1->pos.x + cb1.x : p1->pos.x - cb1.x - cb1.width,
        p1->pos.y + cb1.y,
        cb1.width,
        cb1.height
    };

    Rectangle wcb2 = {
        p2->lookingRight ? p2->pos.x + cb2.x : p2->pos.x - cb2.x - cb2.width,
        p2->pos.y + cb2.y,
        cb2.width,
        cb2.height
    };

    if ( CheckCollisionRecs( wcb1, wcb2 ) ) {
        Rectangle overlap = GetCollisionRec( wcb1, wcb2 );
        float pushDist = overlap.width / 2;

        if ( p1->pos.x < p2->pos.x ) {
            p1->pos.x -= pushDist;
            p2->pos.x += pushDist;
        } else {
            p1->pos.x += pushDist;
            p2->pos.x -= pushDist;
        }
    }

}

void flipPlayers( GameWorld *gw ) {

    if ( gw->player1->pos.x > gw->player2->pos.x ) {
        if ( !player1RightPlayer2 ) {
            needsToFlipPlayers = true;
        }
        player1RightPlayer2 = true;
    } else {
        if ( player1RightPlayer2 ) {
            needsToFlipPlayers = true;
        }
        player1RightPlayer2 = false;
    }

    if ( needsToFlipPlayers ) {
        flipPlayerSide( gw->player1 );
        flipPlayerSide( gw->player2 );
        needsToFlipPlayers = false;
    }

}
