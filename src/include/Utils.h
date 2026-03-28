#pragma once

#include "Types.h"

const char *utilsPlayerStateToText( PlayerState state );
const char *utilsEditorModeToText( EditorMode mode );
void storePlayerAnimations( Player *p, bool prettyPrint, bool printOut, const char *filename );
void loadPlayerAnimationFrameBoxes( Player *p, const char *filename );