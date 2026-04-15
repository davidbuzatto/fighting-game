/**
 * @file PlayerInit.h
 * @author Prof. Dr. David Buzatto
 * @brief Player creation, initialization and destruction.
 *
 * @copyright Copyright (c) 2026
 */
#pragma once

#include <stdbool.h>
#include "Types.h"

Player *createPlayer();
void initializePlayerRyu( float x, float y, Player *p, PlayerStartSide startSide, int gamepadId, DurationMode animationDurationMode, bool showBoxes, bool showDebugInfo );
void initializePlayerKen( float x, float y, Player *p, PlayerStartSide startSide, int gamepadId, DurationMode animationDurationMode, bool showBoxes, bool showDebugInfo );
void destroyPlayer( Player *player );
