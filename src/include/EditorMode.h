/**
 * @file EditorMode.h
 * @author Prof. Dr. David Buzatto
 * @brief Animation editor draw/update API.
 *
 * @copyright Copyright (c) 2026
 */
#pragma once

#include "Types.h"

#define SHOW_PLAYER_ONION_MODE_EDITING true
#define SHOW_PLAYER_INPUT_BUFFER false
#define RYU_ANIMATIONS_FILE "resources/animations/ryu.json"
#define KEN_ANIMATIONS_FILE "resources/animations/ken.json"

// Shared state: accessed from GameWorld.c
extern bool showPlayerOnionEditing;
extern bool showPlayerInputBuffer;
extern PlayerState lastEditState;
extern int lastEditFrame;

void drawGameWorldEditing( GameWorld *gw );
void updateGameWorldEditing( GameWorld *gw, float delta );
