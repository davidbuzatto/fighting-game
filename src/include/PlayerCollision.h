/**
 * @file PlayerCollision.h
 * @author Prof. Dr. David Buzatto
 * @brief Player-opponent contact resolution API.
 *
 * @copyright Copyright (c) 2026
 */
#pragma once

#include "Types.h"

void resolvePlayerOponnentContact( Player *p, Player *o );
void resolvePlayerOponnentProjectileContact( Player *p, Player *o );
