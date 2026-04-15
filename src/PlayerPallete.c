/**
 * @file PlayerPallete.c
 * @author Prof. Dr. David Buzatto
 * @brief Player pallete swap implementation.
 *
 * @copyright Copyright (c) 2026
 */
#include "raylib/raylib.h"

#include "PlayerPallete.h"
#include "Types.h"
#include "Utils.h"

static const int startLineToChangeColors = 0;
static const int endLineToChangeColors = 10000;

void changePlayerPallete( Player *p, int palleteNumber, int palleteColorLimit ) {

    int spacing = 16;
    int startX = 2;
    int startY = 2;

    if ( startY + spacing * palleteNumber > p->pallete->height ) {
        return;
    }

    Color sourceColors[20];
    Color targetColors[20];
    int colorCount = 0;

    if ( palleteColorLimit > 20 ) {
        palleteColorLimit = 20;
    }

    int imageColorLimit = p->pallete->width / spacing;

    if ( palleteColorLimit > imageColorLimit ) {
        palleteColorLimit = imageColorLimit;
    }

    for ( int i = 0; i < palleteColorLimit; i++ ) {
        sourceColors[i] = GetImageColor( *(p->pallete), startX + spacing * i, startY );
        targetColors[i] = GetImageColor( *(p->pallete), startX + spacing * i, startY + spacing * palleteNumber );
        colorCount++;
    }

    Texture2D newSpriteMap = createTextureFromTextureReplacingColor(
        *(p->baseSpriteMap),
        sourceColors,
        targetColors,
        colorCount,
        startLineToChangeColors,
        endLineToChangeColors
    );

    UnloadTexture( p->currentSpriteMap );
    p->currentSpriteMap = newSpriteMap;

}
