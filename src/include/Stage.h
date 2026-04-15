/**
 * @file Stage.h
 * @author Prof. Dr. David Buzatto
 * @brief Stage rendering and update API.
 *
 * @copyright Copyright (c) 2026
 */
#pragma once

#include "Types.h"

#define SHOW_MODEL_STAGE_TEXTURE false

void initStage( GameWorld *gw );
void updateStage( float delta );
void drawStageBackground( GameWorld *gw );
void drawStageForeground( GameWorld *gw );
