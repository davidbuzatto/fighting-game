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
    bool stopAtLastFrame;
} PlayerAnimation;

typedef struct Player {

    Texture2D *texture;
    Vector2 pos;
    Vector2 dim;

    Vector2 vel;
    float forwardSpeed;
    float backwardSpeed;
    float jumpSpeed;

    PlayerAnimation idleAnim;
    PlayerAnimation forwardAnim;
    PlayerAnimation backwardAnim;
    PlayerAnimation straightJumpAnim;
    PlayerAnimation forwardJumpAnim;
    PlayerAnimation backwardJumpAnim;
    PlayerAnimation *animations[10];
    int animationCount;

    PlayerState state;
    PlayerState lastState;
    bool jumping;
    bool lookingRight;

} Player;

typedef struct GameWorld {
    Camera2D camera;
    Player *player;
    Rectangle floor;
    float gravity;
} GameWorld;