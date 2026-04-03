#pragma once

#include "Types.h"

const char *utilsPlayerStateToText( PlayerState state );
const char *utilsEditorModeToText( EditorMode mode );
void storePlayerAnimations( Player *p, bool prettyPrint, bool printOut, const char *filename );
void loadPlayerAnimationFrameBoxes( Player *p, const char *filename );

void drawTextUsingFont( const char *text, int x, int y, float scale );
Vector2 measureTextUsingFont( const char *text, float scale );

Rectangle getRectangleIntersection( Rectangle r1, Rectangle r2 );