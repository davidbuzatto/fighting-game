#pragma once

#include "Types.h"

const char *utilsPlayerStateToText( PlayerState state );
const char *utilsEditorModeToText( EditorMode mode );
void storePlayerAnimations( Player *p, bool prettyPrint, bool printOut, const char *filename );
void loadPlayerAnimationFrameBoxes( Player *p, const char *filename );

void drawTextUsingFont( const char *text, int x, int y, float scale, int spacing );
Vector2 measureTextUsingFont( const char *text, float scale, int spacing );

Rectangle getRectangleIntersection( Rectangle r1, Rectangle r2 );

bool isGamepadButtonPressed( int gamepadId, int gamepadButton );
bool isGamepadButtonDown( int gamepadId, int gamepadButton );

Texture2D loadTextureReplacingColor( const char *path, Color *sourceColors, Color *targetColors, int colorCount );
Texture2D createTextureFromTextureReplacingColor( Texture2D texture, Color *sourceColors, Color *targetColors, int colorCount, int startLine, int endLine );
void customImageColorReplace( Image *image, Color color, Color replace, int startLine, int endLine );