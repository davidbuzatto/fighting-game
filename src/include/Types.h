#pragma once

#include "raylib/raylib.h"

typedef enum PlayerState {
    PLAYER_STATE_IDLE,
    PLAYER_STATE_WALKING_FORWARD,
    PLAYER_STATE_WALKING_BACKWARD,
    PLAYER_STATE_CROUCHING
} PlayerState;

typedef struct AnimationFrame {
    Rectangle source;
    int duration;      // milliseconds
} AnimationFrame;

typedef struct Animation {
    AnimationFrame *frames;
    int frameCount;
    int currentFrame;
    int frameTimeCounter;
    bool stopAtLastFrame;
} Animation;

typedef struct PlayerKeyBindings {
    int left;
    int right;
    int up;
    int down;
} PlayerKeyBindings;

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
    Animation crouchingAnim;
    Animation *animations[50];
    int animationCount;

    PlayerState state;
    PlayerState lastState;
    bool jumping;
    //bool crouching;
    bool lookingRight;

    PlayerKeyBindings kb;

} Player;

typedef struct GameWorld {

    Rectangle floor;
    Texture2D *stageTexture;

    float gravity;

    Camera2D camera;

    Player *player1;
    Player *player2;

} GameWorld;