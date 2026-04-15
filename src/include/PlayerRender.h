/**
 * @file PlayerRender.h
 * @author Prof. Dr. David Buzatto
 * @brief Player rendering API.
 *
 * @copyright Copyright (c) 2026
 */
#pragma once

#include "Types.h"

void drawPlayer( Player *player );
void drawPlayerShadow( Player *player, float floorY, float shear, float scaleY );
void drawPlayerInputBuffer( Player *player );
void drawPlayerOnionLayers( Player *player, int xOffset );
void drawPlayerAnimationFrame( Player *player, AnimationFrame *af, Vector2 offset, Color tint );
void drawPlayerAnimationFrameForShadow( Player *player, AnimationFrame *af, Vector2 offset, Color tint, float floorY, float shear, float scaleY );
void drawOnHitPlayerAnimation( Player *p );
void drawOnBlockPlayerAnimation( Player *p );
void drawPlayerProjectile( Player *p );
