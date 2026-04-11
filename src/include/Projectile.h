#pragma once

#include "Types.h"

Projectile *createProjectile( Texture2D *spriteMap );
void destroyProjectile( Projectile *p );
void setupProjectile( Projectile *p, ProjectileType type, int damageOnHurt, float x, float y, float velX, float velY );
void drawProjectile( Projectile *p );
void updateProjectile( Projectile *p, Camera2D camera, float delta );

