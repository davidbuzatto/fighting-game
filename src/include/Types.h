#pragma once

#include "raylib/raylib.h"

typedef enum PlayerState {
    PLAYER_STATE_IDLE,
    PLAYER_STATE_WALKING_FORWARD,
    PLAYER_STATE_WALKING_BACKWARD,
    PLAYER_STATE_CROUCHING
} PlayerState;

typedef enum PlayerMoveState {
    PLAYER_MOVE_STATE_NONE,
    PLAYER_MOVE_STATE_LP,
    PLAYER_MOVE_STATE_MP,
    PLAYER_MOVE_STATE_HP,
    PLAYER_MOVE_STATE_LK,
    PLAYER_MOVE_STATE_MK,
    PLAYER_MOVE_STATE_HK,
} PlayerMoveState;

typedef struct AnimationFrame {
    Rectangle source;
    int duration;      // milliseconds
    Vector2 offset;
} AnimationFrame;

typedef struct Animation {
    AnimationFrame *frames;
    int frameCount;
    int currentFrame;
    int frameTimeCounter;
    bool stopAtLastFrame;
    bool runOnce;
    bool runnedAtLastOneFrame;
    bool finished;
} Animation;

typedef struct PlayerKeyBindings {
    int left;
    int right;
    int up;
    int down;
    int lp;
    int mp;
    int hp;
    int lk;
    int mk;
    int hk;
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

    Animation lpAnim;
    Animation mpAnim;
    Animation hpAnim;
    Animation lkAnim;
    Animation mkAnim;
    Animation hkAnim;

    Animation *animations[50];
    int animationCount;

    PlayerState state;
    PlayerState lastState;
    PlayerMoveState moveState;
    PlayerMoveState lastMoveState;

    bool jumping;
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