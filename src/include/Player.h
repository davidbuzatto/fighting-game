#pragma once

#include "Types.h"

Player *createPlayer( float x, float y );
void destroyPlayer( Player *player );
void drawPlayer( Player *player );
void updatePlayer( Player *player, float gravity, float delta );
void flipPlayerSide( Player *player );

void createPlayerAnimationSources( Animation *pa, int frameCount );
void destroyPlayerAnimationSources( Animation *pa );
void updatePlayerAnimation( Animation *pa, float delta );

int getPlayerAnimationFrame( Animation *pa );
int getCurrentPlayerAnimationFrame( Player *player );

Rectangle *getPlayerAnimationSource( Animation *pa );
Rectangle *getCurrentPlayerAnimationSource( Player *player );
void resetPlayerAnimations( Player *player );