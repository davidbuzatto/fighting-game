#pragma once

#include "Types.h"

void createAnimationFrames( Animation *anim, int frameCount );
void destroyAnimationFrames( Animation *anim );
void updateAnimation( Animation *anim, float delta );

int getAnimationCurrentFrameNumber( Animation *anim );
AnimationFrame *getAnimationCurrentFrame( Animation *anim );
void resetAnimation( Animation *anim );