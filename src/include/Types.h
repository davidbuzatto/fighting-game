#pragma once

#include "raylib/raylib.h"

typedef enum PlayerState {
    PLAYER_STATE_IDLE,
    PLAYER_STATE_WALKING_FORWARD,
    PLAYER_STATE_WALKING_BACKWARD
} PlayerState;

typedef struct Player {

    Texture2D *texture;
    Vector2 pos;
    float scale;

    Vector2 vel;

    Rectangle idleSources[5];
    int idleFrameCount;
    int idleCurrentFrame;
    float idleFrameTime;
    float idleFrameTimeCounter;

    Rectangle forwardSources[6];
    int forwardFrameCount;
    int forwardCurrentFrame;
    float forwardFrameTime;
    float forwardFrameTimeCounter;
    
    Rectangle backwardSources[6];
    int backwardFrameCount;
    int backwardCurrentFrame;
    float backwardFrameTime;
    float backwardFrameTimeCounter;

    PlayerState state;
    bool jumping;

} Player;

typedef struct GameWorld {
    Rectangle floor;
    Player player;
    float gravity;
} GameWorld;