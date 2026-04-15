/**
 * @file PlayerSelectMode.h
 * @author Prof. Dr. David Buzatto
 * @brief Player selection screen API.
 *
 * @copyright Copyright (c) 2026
 */
#pragma once

#include "Types.h"

// Shared state: accessed from GameWorld.c (reset on F11)
extern int p1SelectedLine;
extern int p1SelectedColumn;
extern int p2SelectedLine;
extern int p2SelectedColumn;
extern float pSelectBlinkCounter;
extern bool pSelectBlink;
extern float pSelectTransitionCounter;
extern float startTransitionToPlay;
extern bool p1Selected;
extern bool p2Selected;
extern int p1SelectedPallete;
extern int p2SelectedPallete;

void drawGameWorldSelectingPlayers( GameWorld *gw );
void updateGameWorldSelectingPlayers( GameWorld *gw, float delta );
