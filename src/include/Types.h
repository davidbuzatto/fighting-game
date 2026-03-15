#pragma once

#include "raylib/raylib.h"

typedef enum PlayerState {
    PLAYER_STATE_IDLE,
    PLAYER_STATE_WALKING_FORWARD,
    PLAYER_STATE_WALKING_BACKWARD
} PlayerState;

typedef struct AnimationFrame {
    Rectangle source;
    int duration;
} AnimationFrame;

typedef struct Animation {
    Rectangle *sources;
    int frameCount;
    int currentFrame;
    float frameTime;
    float frameTimeCounter;
    bool stopAtLastFrame;
} Animation;

typedef struct Player {

    Texture2D *texture;
    Vector2 pos;
    Vector2 dim;

    Vector2 vel;
    float forwardSpeed;
    float backwardSpeed;
    float jumpSpeed;

    Animation idleAnim;
    Animation forwardAnim;
    Animation backwardAnim;
    Animation straightJumpAnim;
    Animation forwardJumpAnim;
    Animation backwardJumpAnim;
    Animation *animations[10];
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