#pragma once

#include <stdbool.h>
#include "Types.h"

Player *createPlayer();
void initializePlayerRyu( float x, float y, Player *p, PlayerStartSide startSide, DurationMode animationDurationMode, bool showBoxes, bool showDebugInfo );
void initializePlayerKen( float x, float y, Player *p, PlayerStartSide startSide, DurationMode animationDurationMode, bool showBoxes, bool showDebugInfo );

void destroyPlayer( Player *player );
void drawPlayer( Player *player );
void drawPlayerInputBuffer( Player *player );
void drawPlayerOnionLayers( Player *player, int xOffset );
void drawPlayerAnimationFrame( Player *player, AnimationFrame *af, Vector2 offset, Color tint );
void processInputPlayer( Player *player, Player *opponent, float delta );
void updatePlayer( Player *player, Player *opponent, float gravity, float delta );
void flipPlayerSide( Player *player );
float distancePlayer( Player *player1, Player *player2 );

AnimationFrame *getPlayerCurrentAnimationFrame( Player *player );
Animation *getPlayerCurrentAnimation( Player *player );
void resetPlayerAnimations( Player *player );

void resolvePlayerOponnentContact( Player *p, Player *o );
void drawOnHitPlayerAnimation( Player *p );