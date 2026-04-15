/**
 * @file PlayerSelectMode.c
 * @author Prof. Dr. David Buzatto
 * @brief Player selection screen implementation.
 *
 * @copyright Copyright (c) 2026
 */
#include "raylib/raylib.h"
#include "raylib/raymath.h"

#include "PlayerSelectMode.h"
#include "ResourceManager.h"
#include "Types.h"
#include "Utils.h"

// defined in GameWorld.c — needed here to play/stop music
extern bool playMusic;

// defined in GameWorld.c — called when both players confirm selection
void startMatch( GameWorld *gw, PlayerType playerType1, int player1Pallete, PlayerType playerType2, int player2Pallete );

// defined in GameWorld.c — key bindings
extern const PlayerKeyBindings p1KeyBindings;
extern const PlayerKeyBindings p2KeyBindings;

// internal helper — not exported
static int getPlayerPalleteSelectingPlayers( int gamepadId, const PlayerKeyBindings *kb );

// ---------------------------------------------------------------------------
// Shared state (extern'd in PlayerSelectMode.h so GameWorld.c can reset them)
// ---------------------------------------------------------------------------

int p1SelectedLine = 0;
int p1SelectedColumn = 0;
int p2SelectedLine = 1;
int p2SelectedColumn = 0;

float pSelectBlinkCounter = 0.0f;
bool pSelectBlink = false;

float pSelectTransitionCounter = 0.0f;
float startTransitionToPlay = false;

bool p1Selected = false;
bool p2Selected = false;
int p1SelectedPallete = -1;
int p2SelectedPallete = -1;

// ---------------------------------------------------------------------------
// Internal constants — not used outside this file
// ---------------------------------------------------------------------------

static float pSelectBlinkTime = 0.02f;
static float pSelectTransitionTime = 1.0f;

// ---------------------------------------------------------------------------
// Const lookup tables — internal only
// ---------------------------------------------------------------------------

static Vector2 portraitsMap[2][8] = {
    { { 0, 0 }, { 0, 1 }, { 0, 2 }, { 0, 3 }, { 2, 0 }, { 2, 1 }, { 2, 2 }, { 2, 3 } },
    { { 1, 0 }, { 1, 1 }, { 1, 2 }, { 1, 3 }, { 3, 0 }, { 3, 1 }, { 3, 2 }, { 3, 3 } },
};

static bool playerSelectSelectable[2][8] = {
    { true, false, false, false, false, false, false, false },
    { true, false, false, false, false, false, false, false },
};

static PlayerType playerSelectPlayerType[2][8] = {
    { PLAYER_TYPE_RYU, PLAYER_TYPE_OTHER, PLAYER_TYPE_OTHER, PLAYER_TYPE_OTHER, PLAYER_TYPE_OTHER, PLAYER_TYPE_OTHER, PLAYER_TYPE_OTHER, PLAYER_TYPE_OTHER },
    { PLAYER_TYPE_KEN, PLAYER_TYPE_OTHER, PLAYER_TYPE_OTHER, PLAYER_TYPE_OTHER, PLAYER_TYPE_OTHER, PLAYER_TYPE_OTHER, PLAYER_TYPE_OTHER, PLAYER_TYPE_OTHER },
};

// ---------------------------------------------------------------------------
// Implementation
// ---------------------------------------------------------------------------

void drawGameWorldSelectingPlayers( GameWorld *gw ) {

    ClearBackground( (Color) { 68, 51, 136, 255 } );
    int scale = 2;

    int pSelectBackW = 384;
    int pSelectBackH = 224;
    int pSelectW = 32;
    int pSelectH = 39;
    int pSelectWi = 32;
    int pSelectHi = 32;

    int startX = GetScreenWidth() / 2 - ( pSelectBackW * scale ) / 2;
    int startY = GetScreenHeight() / 2 - ( pSelectBackH * scale ) / 2;

    int pSelectStartX = startX + 64 * scale;
    int pSelectStartY = startY + 156 * scale;

    DrawTexturePro(
        rm.playerSelectTexture,
        (Rectangle) { 30, 823, pSelectBackW, pSelectBackH },
        (Rectangle) { startX, startY, pSelectBackW * scale, pSelectBackH * scale },
        (Vector2) { 0 },
        0.0f,
        WHITE
    );

    DrawTexturePro(
        rm.playerSelectTexture,
        (Rectangle) { 158, 5, 128, 13 },
        (Rectangle) { pSelectStartX + 130, pSelectStartY - 25, 128 * scale, 13 * scale },
        (Vector2) { 0 },
        0.0f,
        WHITE
    );

    Vector2 vP1 = portraitsMap[p1SelectedLine][p1SelectedColumn];
    Vector2 vP2 = portraitsMap[p2SelectedLine][p2SelectedColumn];

    bool p1Selectable = playerSelectSelectable[p1SelectedLine][p1SelectedColumn];
    bool p2Selectable = playerSelectSelectable[p2SelectedLine][p2SelectedColumn];

    // p1 portrait
    DrawRectangle( startX, startY + 30, 96 * scale, 112 * scale, Fade( BLACK, 0.5f ) );
    DrawTexturePro(
        rm.playerSelectTexture,
        (Rectangle) { 15 + 106 * vP1.y, 149 + 138 * vP1.x, 96, 112 },
        (Rectangle) {
            startX,
            startY + 30,
            96 * scale,
            112 * scale
        },
        (Vector2) { 0 },
        0.0f,
        p1Selectable ? WHITE : Fade( DARKGRAY, 0.7f )
    );
    DrawTexturePro(
        rm.playerSelectTexture,
        (Rectangle) { 15 + 106 * vP1.y, 266 + 138 * vP1.x, 96, 16 },
        (Rectangle) {
            startX,
            startY + 30 + 112 * scale + 4 * scale,
            96 * scale,
            16 * scale
        },
        (Vector2) { 0 },
        0.0f,
        p1Selectable ? WHITE : Fade( DARKGRAY, 0.7f )
    );

    DrawRectangle( startX + pSelectBackW * scale - 96 * scale, startY + 30, 96 * scale, 112 * scale, Fade( BLACK, 0.5f ) );
    DrawTexturePro(
        rm.playerSelectTexture,
        (Rectangle) { 15 + 106 * vP2.y, 149 + 138 * vP2.x, -96, 112 },
        (Rectangle) {
            startX + pSelectBackW * scale - 96 * scale, startY + 30, 96 * scale, 112 * scale
        },
        (Vector2) { 0 },
        0.0f,
        p2Selectable ? WHITE : Fade( DARKGRAY, 0.7f )
    );
    DrawTexturePro(
        rm.playerSelectTexture,
        (Rectangle) { 15 + 106 * vP2.y, 266 + + 138 * vP2.x, 96, 16 },
        (Rectangle) {
            startX + pSelectBackW * scale - 96 * scale,
            startY + 30 + 112 * scale + 4 * scale,
            96 * scale,
            16 * scale
        },
        (Vector2) { 0 },
        0.0f,
        p2Selectable ? WHITE : Fade( DARKGRAY, 0.7f )
    );

    DrawRectangleLines( startX, startY, pSelectBackW * scale, pSelectBackH * scale, BLACK );

    // p1 select
    DrawTexturePro(
        rm.playerSelectTexture,
        (Rectangle) { p1Selected ? 149 : pSelectBlink ? 223 : 149, 100, pSelectW, pSelectH },
        (Rectangle) {
            pSelectStartX + p1SelectedColumn * pSelectWi * scale,
            pSelectStartY + p1SelectedLine * pSelectHi * scale,
            pSelectW * scale,
            pSelectH * scale
        },
        (Vector2) { 0 },
        0.0f,
        WHITE
    );

    // p2 select
    DrawTexturePro(
        rm.playerSelectTexture,
        (Rectangle) { p2Selected ? 186 : pSelectBlink ? 260 : 186, 100, pSelectW, pSelectH },
        (Rectangle) {
            pSelectStartX + p2SelectedColumn * pSelectWi * scale,
            pSelectStartY + p2SelectedLine * pSelectHi * scale,
            pSelectW * scale,
            pSelectH * scale
        },
        (Vector2) { 0 },
        0.0f,
        WHITE
    );

    if ( startTransitionToPlay ) {
        DrawRectangle( 0, 0, GetScreenWidth(), GetScreenHeight(), Fade( BLACK, pSelectTransitionCounter / pSelectTransitionTime ) );
    }


}

static int getPlayerPalleteSelectingPlayers( int gamepadId, const PlayerKeyBindings *kb ) {

    if ( IsKeyPressed( kb->lp.key )     || isGamepadButtonPressed( gamepadId, kb->lp.gamepadButton ) )     return 0;
    if ( IsKeyPressed( kb->mp.key )     || isGamepadButtonPressed( gamepadId, kb->mp.gamepadButton ) )     return 1;
    if ( IsKeyPressed( kb->hp.key )     || isGamepadButtonPressed( gamepadId, kb->hp.gamepadButton ) )     return 2;
    if ( IsKeyPressed( kb->lmhp.key )   || isGamepadButtonPressed( gamepadId, kb->lmhp.gamepadButton ) )   return 3;
    if ( IsKeyPressed( kb->lk.key )     || isGamepadButtonPressed( gamepadId, kb->lk.gamepadButton ) )     return 4;
    if ( IsKeyPressed( kb->mk.key )     || isGamepadButtonPressed( gamepadId, kb->mk.gamepadButton ) )     return 5;
    if ( IsKeyPressed( kb->hk.key )     || isGamepadButtonPressed( gamepadId, kb->hk.gamepadButton ) )     return 6;
    if ( IsKeyPressed( kb->lmhk.key )   || isGamepadButtonPressed( gamepadId, kb->lmhk.gamepadButton ) )   return 7;
    if ( IsKeyPressed( kb->select.key ) || isGamepadButtonPressed( gamepadId, kb->select.gamepadButton ) ) return 8;
    if ( IsKeyPressed( kb->start.key )  || isGamepadButtonPressed( gamepadId, kb->start.gamepadButton ) )  return 9;

    return -1;

}

void updateGameWorldSelectingPlayers( GameWorld *gw, float delta ) {

    if ( playMusic ) {
        if ( !IsMusicStreamPlaying( rm.playerSelectTheme ) )  {
            PlayMusicStream( rm.playerSelectTheme );
        }
        UpdateMusicStream( rm.playerSelectTheme );
    }

    if ( startTransitionToPlay ) {

        pSelectTransitionCounter += delta;

        if ( pSelectTransitionCounter >= pSelectTransitionTime ) {

            pSelectTransitionCounter = 0.0f;

            PlayerType pType1Selected = playerSelectPlayerType[p1SelectedLine][p1SelectedColumn];
            PlayerType pType2Selected = playerSelectPlayerType[p2SelectedLine][p2SelectedColumn];
            startMatch( gw, pType1Selected, p1SelectedPallete, pType2Selected, p2SelectedPallete );

            gw->mode = GAME_MODE_PLAYING;

            if ( playMusic ) {
                if ( IsMusicStreamPlaying( rm.playerSelectTheme ) )  {
                    StopMusicStream( rm.playerSelectTheme );
                }
            }

        }

        return;

    }

    pSelectBlinkCounter += delta;
    if ( pSelectBlinkCounter >= pSelectBlinkTime ) {
        pSelectBlinkCounter = 0.0f;
        pSelectBlink = !pSelectBlink;
    }

    int p1LastSelectedLine = p1SelectedLine;
    int p1LastSelectedColumn = p1SelectedColumn;
    int p2LastSelectedLine = p2SelectedLine;
    int p2LastSelectedColumn = p2SelectedColumn;

    if ( !p1Selected ) {
        if ( IsKeyPressed( p1KeyBindings.left.key ) || isGamepadButtonPressed( 0, p1KeyBindings.left.gamepadButton ) ) {
            p1SelectedColumn--;
        } else if ( IsKeyPressed( p1KeyBindings.right.key ) || isGamepadButtonPressed( 0, p1KeyBindings.right.gamepadButton ) ) {
            p1SelectedColumn++;
        } else if ( IsKeyPressed( p1KeyBindings.up.key ) || isGamepadButtonPressed( 0, p1KeyBindings.up.gamepadButton ) ) {
            p1SelectedLine--;
        } else if ( IsKeyPressed( p1KeyBindings.down.key ) || isGamepadButtonPressed( 0, p1KeyBindings.down.gamepadButton ) ) {
            p1SelectedLine++;
        }
    }

    if ( !p2Selected ) {
        if ( IsKeyPressed( p2KeyBindings.left.key ) || isGamepadButtonPressed( 1, p2KeyBindings.left.gamepadButton ) ) {
            p2SelectedColumn--;
        } else if ( IsKeyPressed( p2KeyBindings.right.key ) || isGamepadButtonPressed( 1, p2KeyBindings.left.gamepadButton ) ) {
            p2SelectedColumn++;
        } else if ( IsKeyPressed( p2KeyBindings.up.key ) || isGamepadButtonPressed( 1, p2KeyBindings.left.gamepadButton ) ) {
            p2SelectedLine--;
        } else if ( IsKeyPressed( p2KeyBindings.down.key ) || isGamepadButtonPressed( 1, p2KeyBindings.left.gamepadButton ) ) {
            p2SelectedLine++;
        }
    }

    p1SelectedLine = (int) Clamp( p1SelectedLine, 0, 1 );
    p1SelectedColumn = (int) Clamp( p1SelectedColumn, 0, 7 );
    p2SelectedLine = (int) Clamp( p2SelectedLine, 0, 1 );
    p2SelectedColumn = (int) Clamp( p2SelectedColumn, 0, 7 );

    if ( p1SelectedLine != p1LastSelectedLine ) {
        PlaySound( rm.p1ChangeSelectedSound );
    } else if ( p1SelectedColumn != p1LastSelectedColumn ) {
        PlaySound( rm.p1ChangeSelectedSound );
    }

    if ( p2SelectedLine != p2LastSelectedLine ) {
        PlaySound( rm.p2ChangeSelectedSound );
    } else if ( p2SelectedColumn != p2LastSelectedColumn ) {
        PlaySound( rm.p2ChangeSelectedSound );
    }

    bool p1Selectable = playerSelectSelectable[p1SelectedLine][p1SelectedColumn];
    bool p2Selectable = playerSelectSelectable[p2SelectedLine][p2SelectedColumn];

    int p1Pallete = getPlayerPalleteSelectingPlayers( 0, &p1KeyBindings );
    int p2Pallete = getPlayerPalleteSelectingPlayers( 1, &p2KeyBindings );

    if ( p1Selectable && p1Pallete != -1 && !p1Selected ) {
        p1Selected = true;
        p1SelectedPallete = p1Pallete;
        PlaySound( rm.p1SelectionSound );
    }

    if ( p2Selectable && p2Pallete != -1 && !p2Selected ) {
        p2Selected = true;
        p2SelectedPallete = p2Pallete;
        PlaySound( rm.p2SelectionSound );
    }

    if ( p1Selected && p2Selected ) {
        startTransitionToPlay = true;
    }

}
