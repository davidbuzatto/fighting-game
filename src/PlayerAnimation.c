/**
 * @file PlayerAnimation.c
 * @author Prof. Dr. David Buzatto
 * @brief Player animation helpers and side-flip implementation.
 *
 * @copyright Copyright (c) 2026
 */
#include <math.h>

#include "Animation.h"
#include "PlayerAnimation.h"
#include "Types.h"

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
