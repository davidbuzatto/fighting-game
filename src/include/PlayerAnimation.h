/**
 * @file PlayerAnimation.h
 * @author Prof. Dr. David Buzatto
 * @brief Player animation helpers and side-flip API.
 *
 * @copyright Copyright (c) 2026
 */
#pragma once

#include "Types.h"

void flipPlayerSide( Player *player );
float distancePlayer( Player *player1, Player *player2 );
AnimationFrame *getPlayerCurrentAnimationFrame( Player *player );
Animation *getPlayerCurrentAnimation( Player *player );
void resetPlayerAnimations( Player *player );
