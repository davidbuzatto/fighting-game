#pragma once

#include <stdbool.h>
#include "Types.h"

Player *createPlayer();
void initializePlayerRyu( float x, float y, Player *p, PlayerStartSide startSide, int gamepadId, DurationMode animationDurationMode, bool showBoxes, bool showDebugInfo );
void initializePlayerKen( float x, float y, Player *p, PlayerStartSide startSide, int gamepadId, DurationMode animationDurationMode, bool showBoxes, bool showDebugInfo );

void destroyPlayer( Player *player );
void processInputPlayer( Player *player, Player *opponent, float delta, int currentFrame, bool discardInput );
void updatePlayer( Player *player, Player *opponent, Camera2D camera, float gravity, float delta );

void resolvePlayerOponnentContact( Player *p, Player *o );
void resolvePlayerOponnentProjectileContact( Player *p, Player *o );
