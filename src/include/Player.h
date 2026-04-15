#pragma once

#include <stdbool.h>
#include "Types.h"

void processInputPlayer( Player *player, Player *opponent, float delta, int currentFrame, bool discardInput );
void updatePlayer( Player *player, Player *opponent, Camera2D camera, float gravity, float delta );
