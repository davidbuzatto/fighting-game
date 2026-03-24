#pragma once

#include "Types.h"

Player *createPlayer();
void initializePlayerRyu( float x, float y, Player *p );
void initializePlayerKen( float x, float y, Player *p );

void destroyPlayer( Player *player );
void drawPlayer( Player *player );
void processInputPlayer( Player *player, Player *opponent, float delta );
void updatePlayer( Player *player, Player *opponent, float gravity, float delta );
void flipPlayerSide( Player *player );

AnimationFrame *getPlayerCurrentAnimationFrame( Player *player );
void resetPlayerAnimations( Player *player );