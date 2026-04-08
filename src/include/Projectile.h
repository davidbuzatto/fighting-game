#pragma once

#include "Types.h"

Projectile *createProjectile( void );
void destroyProjectile( Projectile *p );
void setupProjectile( Projectile *p, ProjectileType type, float x, float y, float velX, float velY );
void drawProjectile( Projectile *p );
void updateProjectile( Projectile *p, float delta );

