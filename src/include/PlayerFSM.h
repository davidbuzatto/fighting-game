/**
 * @file PlayerFSM.h
 * @author Prof. Dr. David Buzatto
 * @brief Player finite state machine: input processing and per-frame update.
 *
 * @copyright Copyright (c) 2026
 */
#pragma once

#include <stdbool.h>
#include "raylib/raylib.h"
#include "Types.h"

void processInputPlayer( Player *player, Player *opponent, float delta, int currentFrame, bool discardInput );
void updatePlayer( Player *player, Player *opponent, Camera2D camera, float gravity, float delta );
