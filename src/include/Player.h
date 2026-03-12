#pragma once

#include "Types.h"

Player *createPlayer( float x, float y, float scale );
void destroyPlayer( Player *player );
void drawPlayer( Player *player );
void updatePlayer( Player *player, float delta );

void createPlayerAnimationSources( PlayerAnimation *pa, int frameCount );
void destroyPlayerAnimationSources( PlayerAnimation *pa );
void updatePlayerAnimation( PlayerAnimation *pa, float delta );

int getPlayerAnimationFrame( PlayerAnimation *pa );
int getCurrentPlayerAnimationFrame( Player *player );

Rectangle *getPlayerAnimationSource( PlayerAnimation *pa );
Rectangle *getCurrentPlayerAnimationSource( Player *player );
void resetPlayerAnimations( Player *player );