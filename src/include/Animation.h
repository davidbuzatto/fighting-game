#pragma once

#include "Types.h"

void createAnimationFrames( Animation *anim, int frameCount );
void initAnimationFrames( AnimationFrame *frames, int frameCount, int startX, int startY, int sourceW, int sourceH, int offsetX, int offsetY, bool backwards );
void destroyAnimationFrames( Animation *anim );
void updateAnimation( Animation *anim, DurationMode durationMode, float delta );

int getAnimationCurrentFrameNumber( Animation *anim );
AnimationFrame *getAnimationCurrentFrame( Animation *anim );
void resetAnimation( Animation *anim );