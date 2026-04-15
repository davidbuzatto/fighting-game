/**
 * @file Hud.c
 * @author Prof. Dr. David Buzatto
 * @brief HUD rendering implementation.
 *
 * @copyright Copyright (c) 2026
 */
#include "raylib/raylib.h"

#include "Hud.h"
#include "ResourceManager.h"
#include "Types.h"
#include "Utils.h"

static const Color PORTRAIT_BG_COLOR = { 112, 136, 198, 255 };

extern Match match;

void drawHud( GameWorld *gw ) {

    int barWidth = 335;
    int healthWidthP1 = (int) ( barWidth * ( gw->player1->health / 100.0f ) );
    int healthWidthP2 = (int) ( barWidth * ( gw->player2->health / 100.0f ) );

    // player 1
    DrawRectangleRec(
        (Rectangle) { 75, 65, barWidth, 25 },
        Fade( RED, 0.7f )
    );

    DrawRectangleRec(
        (Rectangle) { 75 + barWidth - healthWidthP1, 65, healthWidthP1, 25 },
        YELLOW
    );

    DrawRectangleRoundedLinesEx(
        (Rectangle) { 77, 67, barWidth, 25 },
        0.5f,
        10,
        4,
        BLACK
    );

    DrawRectangleRoundedLinesEx(
        (Rectangle) { 75, 65, barWidth, 25 },
        0.5f,
        10,
        4,
        WHITE
    );

    drawTextUsingFont( TextFormat( "%s", gw->player1->name ), 75, 98, 3, -8 );

    // player 2
    DrawRectangleRec(
        (Rectangle) { 485, 65, barWidth, 25 },
        Fade( RED, 0.7f )
    );

    DrawRectangleRec(
        (Rectangle) { 485, 65, healthWidthP2, 25 },
        YELLOW
    );

    DrawRectangleRoundedLinesEx(
        (Rectangle) { 487, 67, barWidth, 25 },
        0.5f,
        10,
        4,
        BLACK
    );

    DrawRectangleRoundedLinesEx(
        (Rectangle) { 485, 65, barWidth, 25 },
        0.5f,
        10,
        4,
        WHITE
    );

    const char *p2Name = TextFormat( "%s", gw->player2->name );
    Vector2 measureP2Name = measureTextUsingFont( p2Name, 3, -8 );
    drawTextUsingFont( p2Name, 830 - measureP2Name.x, 98, 3, -8 );

    // ko
    DrawRectangleRounded(
        (Rectangle) { 417, 60, 61, 35 },
        0.5f,
        10,
        BLACK
    );

    DrawRectangleRoundedLinesEx(
        (Rectangle) { 419, 62, 61, 35 },
        0.5f,
        10,
        4,
        BLACK
    );

    DrawRectangleRoundedLinesEx(
        (Rectangle) { 417, 60, 61, 35 },
        0.5f,
        10,
        4,
        WHITE
    );

    DrawText( "K.O", 421, 61, 36, RED );

    const char *remainingTimeStr = TextFormat( "%02d", (int) match.remainingTime );
    Vector2 measureRemainingTime = measureTextUsingFont( remainingTimeStr, 4, -5 );
    drawTextUsingFont( remainingTimeStr, GetScreenWidth() / 2 - measureRemainingTime.x / 2 + 5, 103, 4, -5 );

    // portraits
    DrawRectangle( 0, 95, 70, 100, LIGHTGRAY );
    DrawRectangle( 2, 97, 66, 96, PORTRAIT_BG_COLOR );

    DrawRectangle( GetScreenWidth() - 70, 95, 70, 100, LIGHTGRAY );
    DrawRectangle( GetScreenWidth() - 68, 97, 66, 96, PORTRAIT_BG_COLOR );

    DrawTexturePro(
        gw->player1->currentSpriteMap,
        (Rectangle) { 131, 15, -96, 112 },
        (Rectangle) { 2, 97, 66, 96 },
        (Vector2) { 0 },
        0.0f,
        WHITE
    );

    DrawTexturePro(
        gw->player2->currentSpriteMap,
        (Rectangle) { 131, 15, 96, 112 },
        (Rectangle) { GetScreenWidth() - 68, 97, 66, 96 },
        (Vector2) { 0 },
        0.0f,
        WHITE
    );

    // rounds
    for ( int i = 0; i < gw->player1->roundsWon; i++ ) {
        DrawTexturePro(
            rm.fontsTexture,
            (Rectangle) { 346, 19, 16, 20 },
            (Rectangle) { 32 * i, 55, 32, 40 },
            (Vector2) { 0 },
            0.0f,
            WHITE
        );
    }

    for ( int i = 0; i < gw->player2->roundsWon; i++ ) {
        DrawTexturePro(
            rm.fontsTexture,
            (Rectangle) { 346, 19, 16, 20 },
            (Rectangle) { GetScreenWidth() - 32 * ( i + 1 ), 55, 32, 40 },
            (Vector2) { 0 },
            0.0f,
            WHITE
        );
    }

}
