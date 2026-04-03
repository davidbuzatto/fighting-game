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

void updateAnimation( Animation *anim, DurationMode durationMode, float delta ) {

    if ( anim->finished ) {
        return;
    }

    anim->frameTimeCounter += (int) ( delta * 1000 );

    AnimationFrame *frame = getAnimationCurrentFrame( anim );

    int actualDuration = 0;

    if ( durationMode == DURATION_MODE_MILLISECONDS ) {
        actualDuration = frame->duration;
    } else { // DURATION_MODE_FRAMES
        actualDuration = (int) ( frame->duration * 16.67f );
    }

    if ( anim->frameTimeCounter >= actualDuration ) {

        anim->frameTimeCounter = 0;
        anim->currentFrame++;

        if ( anim->stopAtLastFrame && anim->currentFrame >= anim->frameCount ) {
            anim->currentFrame = anim->frameCount - 1;
            anim->finished = true;
            return;
        }

        if ( anim->runOnce && anim->currentFrame >= anim->frameCount ) {
            //anim->currentFrame = anim->frameCount - 1;
            anim->currentFrame = 0;
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
    if ( anim != NULL ) {
        return &anim->frames[getAnimationCurrentFrameNumber(anim)];
    }
    return NULL;
}

void resetAnimation( Animation *anim ) {
    anim->currentFrame = 0;
    anim->frameTimeCounter = 0;
    anim->finished = false;
}