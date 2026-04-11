#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "Animation.h"
#include "Macros.h"
#include "Projectile.h"
#include "ResourceManager.h"
#include "Types.h"

static void setupDuration( Animation *a, int duration ) {
    for ( int i = 0; i < a->frameCount; i++ ) {
        a->frames[i].duration = duration;
    }
}

Projectile *createProjectile( Texture2D *spriteMap ) {

    Projectile *p = (Projectile*) malloc( sizeof( Projectile ) );

    p->spriteMap = spriteMap;
    p->active = false;
    p->pos = (Vector2) { 0 };
    p->vel = (Vector2) { 0 };
    p->hurtbox = (Rectangle) { 0, 0, 35, 25 };
    p->damageOnHurt = 0;
    p->runImpactAnim = false;

    p->startupAnim.frameCount = 3;
    p->startupAnim.currentFrame = 0;
    p->startupAnim.frameTimeCounter = 0.0f;
    p->startupAnim.stopAtLastFrame = false;
    p->startupAnim.runOnce = true;
    p->startupAnim.finished = false;
    createAnimationFrames( &p->startupAnim, p->startupAnim.frameCount );
    initAnimationFrames( p->startupAnim.frames, p->startupAnim.frameCount, 1, 1, -64, 64, 0, 0, false );
    setupDuration( &p->startupAnim, 50 );

    p->continuousAnimLP.frameCount = 6;
    p->continuousAnimLP.currentFrame = 0;
    p->continuousAnimLP.frameTimeCounter = 0.0f;
    p->continuousAnimLP.stopAtLastFrame = false;
    p->continuousAnimLP.runOnce = false;
    p->continuousAnimLP.finished = false;
    createAnimationFrames( &p->continuousAnimLP, p->continuousAnimLP.frameCount );
    initAnimationFrames( p->continuousAnimLP.frames, p->continuousAnimLP.frameCount, 1, 66, -64, 32, 14, 0, false );
    setupDuration( &p->continuousAnimLP, 50 );

    p->continuousAnimDetailLP.frameCount = 6;
    p->continuousAnimDetailLP.currentFrame = 0;
    p->continuousAnimDetailLP.frameTimeCounter = 0.0f;
    p->continuousAnimDetailLP.stopAtLastFrame = false;
    p->continuousAnimDetailLP.runOnce = false;
    p->continuousAnimDetailLP.finished = false;
    createAnimationFrames( &p->continuousAnimDetailLP, p->continuousAnimDetailLP.frameCount );
    initAnimationFrames( p->continuousAnimDetailLP.frames, p->continuousAnimDetailLP.frameCount, 1, 99, -64, 32, 14, 0, false );
    setupDuration( &p->continuousAnimDetailLP, 50 );

    p->continuousAnimMP.frameCount = 6;
    p->continuousAnimMP.currentFrame = 0;
    p->continuousAnimMP.frameTimeCounter = 0.0f;
    p->continuousAnimMP.stopAtLastFrame = false;
    p->continuousAnimMP.runOnce = false;
    p->continuousAnimMP.finished = false;
    createAnimationFrames( &p->continuousAnimMP, p->continuousAnimMP.frameCount );
    initAnimationFrames( p->continuousAnimMP.frames, p->continuousAnimMP.frameCount, 1, 132, -80, 32, 24, 0, false );
    setupDuration( &p->continuousAnimMP, 50 );

    p->continuousAnimDetailMP.frameCount = 6;
    p->continuousAnimDetailMP.currentFrame = 0;
    p->continuousAnimDetailMP.frameTimeCounter = 0.0f;
    p->continuousAnimDetailMP.stopAtLastFrame = false;
    p->continuousAnimDetailMP.runOnce = false;
    p->continuousAnimDetailMP.finished = false;
    createAnimationFrames( &p->continuousAnimDetailMP, p->continuousAnimDetailMP.frameCount );
    initAnimationFrames( p->continuousAnimDetailMP.frames, p->continuousAnimDetailMP.frameCount, 1, 165, -80, 32, 24, 0, false );
    setupDuration( &p->continuousAnimDetailMP, 50 );

    p->continuousAnimHP.frameCount = 6;
    p->continuousAnimHP.currentFrame = 0;
    p->continuousAnimHP.frameTimeCounter = 0.0f;
    p->continuousAnimHP.stopAtLastFrame = false;
    p->continuousAnimHP.runOnce = false;
    p->continuousAnimHP.finished = false;
    createAnimationFrames( &p->continuousAnimHP, p->continuousAnimHP.frameCount );
    initAnimationFrames( p->continuousAnimHP.frames, p->continuousAnimHP.frameCount, 1, 198, -80, 32, 24, 0, false );
    setupDuration( &p->continuousAnimHP, 50 );

    p->continuousAnimDetailHP.frameCount = 6;
    p->continuousAnimDetailHP.currentFrame = 0;
    p->continuousAnimDetailHP.frameTimeCounter = 0.0f;
    p->continuousAnimDetailHP.stopAtLastFrame = false;
    p->continuousAnimDetailHP.runOnce = false;
    p->continuousAnimDetailHP.finished = false;
    createAnimationFrames( &p->continuousAnimDetailHP, p->continuousAnimDetailHP.frameCount );
    initAnimationFrames( p->continuousAnimDetailHP.frames, p->continuousAnimDetailHP.frameCount, 1, 231, -80, 32, 24, 0, false );
    setupDuration( &p->continuousAnimDetailHP, 50 );

    p->impactAnim.frameCount = 5;
    p->impactAnim.currentFrame = 0;
    p->impactAnim.frameTimeCounter = 0.0f;
    p->impactAnim.stopAtLastFrame = false;
    p->impactAnim.runOnce = true;
    p->impactAnim.finished = false;
    createAnimationFrames( &p->impactAnim, p->impactAnim.frameCount );
    initAnimationFrames( p->impactAnim.frames, p->impactAnim.frameCount, 1, 264, -80, 64, 0, 0, false );
    setupDuration( &p->impactAnim, 30 );

    return p;

}

void destroyProjectile( Projectile *p ) {
    free( p );
}

void setupProjectile( Projectile *p, ProjectileType type, int damageOnHurt, float x, float y, float velX, float velY ) {

    p->type = type;
    p->active = true;
    p->damageOnHurt = damageOnHurt;
    p->runImpactAnim = false;

    p->pos.x = x;
    p->pos.y = y;
    p->vel.x = velX;
    p->vel.y = velY;

    resetAnimation( &p->startupAnim );
    resetAnimation( &p->continuousAnimLP );
    resetAnimation( &p->continuousAnimDetailLP );
    resetAnimation( &p->continuousAnimMP );
    resetAnimation( &p->continuousAnimDetailMP );
    resetAnimation( &p->continuousAnimHP );
    resetAnimation( &p->continuousAnimDetailHP );
    resetAnimation( &p->impactAnim );

}

void drawProjectile( Projectile *p ) {

    AnimationFrame *af = NULL;
    AnimationFrame *afD = NULL;
    
    if ( p->active ) {

        if ( !p->startupAnim.finished ) {
            af = getAnimationCurrentFrame( &p->startupAnim );
        } else {
            switch ( p->type ) {
                case PROJECTILE_TYPE_LOW:
                    af = getAnimationCurrentFrame( &p->continuousAnimLP );
                    afD = getAnimationCurrentFrame( &p->continuousAnimDetailLP );
                    break;
                case PROJECTILE_TYPE_MID:
                    af = getAnimationCurrentFrame( &p->continuousAnimMP );
                    afD = getAnimationCurrentFrame( &p->continuousAnimDetailMP );
                    break;
                case PROJECTILE_TYPE_HIGH:
                    af = getAnimationCurrentFrame( &p->continuousAnimHP );
                    afD = getAnimationCurrentFrame( &p->continuousAnimDetailHP );
                    break;
            }
        }

        bool goingRight = p->vel.x >= 0;

        if ( af != NULL ) {
            DrawTexturePro(
                *(p->spriteMap),
                (Rectangle) {
                    af->source.x,
                    af->source.y,
                    goingRight ? af->source.width : -af->source.width,
                    af->source.height
                },
                (Rectangle) {
                    p->pos.x + af->source.width / 2 + ( goingRight ? -af->offset.x : af->offset.x ),
                    p->pos.y - af->source.height / 2,
                    af->source.width,
                    af->source.height },
                (Vector2) { 0 },
                0.0f,
                WHITE
            );
        }

        if ( afD != NULL ) {
            DrawTexturePro(
                *(p->spriteMap),
                (Rectangle) {
                    afD->source.x,
                    afD->source.y,
                    goingRight ? afD->source.width : -afD->source.width,
                    afD->source.height
                },
                (Rectangle) {
                    p->pos.x + afD->source.width / 2 + ( goingRight ? -afD->offset.x : afD->offset.x ),
                    p->pos.y - afD->source.height / 2,
                    afD->source.width,
                    afD->source.height },
                (Vector2) { 0 },
                0.0f,
                WHITE
            );
        }

        /*DrawRectangle( p->pos.x + p->hurtbox.x - p->hurtbox.width / 2, p->pos.y + p->hurtbox.y - p->hurtbox.height / 2, p->hurtbox.width, p->hurtbox.height, Fade( RED, 0.4 ) );
        DrawRectangleLines( p->pos.x + p->hurtbox.x - p->hurtbox.width / 2, p->pos.y + p->hurtbox.y - p->hurtbox.height / 2, p->hurtbox.width, p->hurtbox.height, RED );*/

    }

    if ( p->runImpactAnim ) {

        if ( p->runImpactAnim ) {
            af = getAnimationCurrentFrame( &p->impactAnim );
        }

        bool goingRight = p->vel.x >= 0;

        if ( af != NULL ) {
            DrawTexturePro(
                *(p->spriteMap),
                (Rectangle) {
                    af->source.x,
                    af->source.y,
                    goingRight ? af->source.width : -af->source.width,
                    af->source.height
                },
                (Rectangle) {
                    p->pos.x + af->source.width / 2 + ( goingRight ? -af->offset.x : af->offset.x ),
                    p->pos.y - af->source.height / 2,
                    af->source.width,
                    af->source.height },
                (Vector2) { 0 },
                0.0f,
                WHITE
            );
        }

    }

}

void updateProjectile( Projectile *p, Camera2D camera, float delta ) {

    if ( p->active ) {

        p->pos.x += p->vel.x * delta;
        p->pos.y += p->vel.y * delta;

        if ( !p->startupAnim.finished ) {
            updateAnimation( &p->startupAnim, DURATION_MODE_MILLISECONDS, delta );
        } else {
            switch ( p->type ) {
                case PROJECTILE_TYPE_LOW:
                    updateAnimation( &p->continuousAnimLP, DURATION_MODE_MILLISECONDS, delta );
                    updateAnimation( &p->continuousAnimDetailLP, DURATION_MODE_MILLISECONDS, delta );
                    break;
                case PROJECTILE_TYPE_MID:
                    updateAnimation( &p->continuousAnimMP, DURATION_MODE_MILLISECONDS, delta );
                    updateAnimation( &p->continuousAnimDetailMP, DURATION_MODE_MILLISECONDS, delta );
                    break;
                case PROJECTILE_TYPE_HIGH:
                    updateAnimation( &p->continuousAnimHP, DURATION_MODE_MILLISECONDS, delta );
                    updateAnimation( &p->continuousAnimDetailHP, DURATION_MODE_MILLISECONDS, delta );
                    break;
            }
        }

        // out of bounds (right and left)
        float offBoundsLimit = ( ( GetScreenWidth() / 2 ) / camera.zoom ) + ( p->hurtbox.width / camera.zoom );
        if ( p->pos.x > camera.target.x + offBoundsLimit || p->pos.x < camera.target.x - offBoundsLimit ) {
            p->active = false;
            p->runImpactAnim = false;
        }

    }

    if ( p->runImpactAnim ) {
        updateAnimation( &p->impactAnim, DURATION_MODE_MILLISECONDS, delta );
        if ( p->impactAnim.finished ) {
            p->runImpactAnim = false;
        }
    }

}