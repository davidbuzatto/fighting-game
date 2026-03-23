#include <stdlib.h>

#include "Animation.h"
#include "Macros.h"
#include "Types.h"

void createAnimationFrames( Animation *anim, int frameCount ) {
    anim->frames = (AnimationFrame*) malloc( sizeof( AnimationFrame ) * frameCount );
}

void destroyAnimationFrames( Animation *anim ) {
    free( anim->frames );
}

void updateAnimation( Animation *anim, float delta ) {

    if ( anim->finished ) {
        return;
    }

    anim->frameTimeCounter += (int) ( delta * 1000 );

    AnimationFrame *frame = getAnimationCurrentFrame( anim );

    if ( anim->frameTimeCounter >= frame->duration ) {
        anim->frameTimeCounter = 0;
        anim->currentFrame++;

        if ( anim->stopAtLastFrame && anim->currentFrame >= anim->frameCount ) {
            anim->currentFrame = anim->frameCount - 1;
            anim->finished = true;
            return;
        }

        if ( anim->runOnce && anim->currentFrame >= anim->frameCount ) {
            anim->currentFrame = anim->frameCount - 1;
            anim->finished = true;
            return;
        }

        anim->currentFrame %= anim->frameCount;
    }

}

int getAnimationCurrentFrameNumber( Animation *anim ) {
    return anim->currentFrame % anim->frameCount;
}

AnimationFrame *getAnimationCurrentFrame( Animation *anim ) {
    return &anim->frames[getAnimationCurrentFrameNumber(anim)];
}

void resetAnimation( Animation *anim ) {
    anim->currentFrame = 0;
    anim->frameTimeCounter = 0;
    anim->finished = false;
}