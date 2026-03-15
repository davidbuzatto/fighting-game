#pragma once

#include "Types.h"

Player *createPlayer();
void initializePlayerRyu( float x, float y, Player *p );
void initializePlayerKen( float x, float y, Player *p );

void destroyPlayer( Player *player );
void drawPlayer( Player *player );
void processInputPlayer( Player *player, float gravity, float delta );
void updatePlayer( Player *player, float gravity, float delta );
void flipPlayerSide( Player *player );

AnimationFrame *getPlayerCurrentAnimationFrame( Player *player );
void resetPlayerAnimations( Player *player );