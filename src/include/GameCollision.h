/**
 * @file GameCollision.h
 * @author Prof. Dr. David Buzatto
 * @brief Game-level collision resolution API.
 *
 * @copyright Copyright (c) 2026
 */
#pragma once

#include <stdbool.h>

#include "Types.h"

extern bool player1RightPlayer2;
extern bool needsToFlipPlayers;

void resolveCollisionPlayerStage( Player *player, GameWorld *gw );
void resolvePlayerPlayerCollision( Player *p1, Player *p2 );
void flipPlayers( GameWorld *gw );
