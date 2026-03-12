#pragma once

#include "raylib/raylib.h"

typedef enum PlayerState {
    PLAYER_STATE_IDLE,
    PLAYER_STATE_WALKING_FORWARD,
    PLAYER_STATE_WALKING_BACKWARD
} PlayerState;

typedef struct PlayerAnimation {
    Rectangle *sources;
    int frameCount;
    int currentFrame;
    float frameTime;
    float frameTimeCounter;
} PlayerAnimation;

typedef struct Player {

    Texture2D *texture;
    Vector2 pos;
    float scale;

    Vector2 vel;

    PlayerAnimation idleAnim;
    PlayerAnimation forwardAnim;
    PlayerAnimation backwardAnim;

    PlayerState state;
    PlayerState lastState;
    bool jumping;

} Player;

typedef struct GameWorld {
    Rectangle floor;
    Player *player;
    float gravity;
} GameWorld;