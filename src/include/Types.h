#pragma once

#include "raylib/raylib.h"

typedef enum GameMode {
    GAME_MODE_PLAYING,
    GAME_MODE_EDITING,
} GameMode;

typedef enum EditorMode {
    EDITOR_MODE_COLLISION_BOX,
    EDITOR_MODE_HIT_BOX_0,
    EDITOR_MODE_HIT_BOX_1,
    EDITOR_MODE_HIT_BOX_2,
    EDITOR_MODE_HURT_BOX_0,
    EDITOR_MODE_HURT_BOX_1,
    EDITOR_MODE_HURT_BOX_2,
} EditorMode;

typedef enum DurationMode {
    DURATION_MODE_MILLISECONDS,
    DURATION_MODE_FRAMES,
} DurationMode;

typedef enum PlayerState {
    PLAYER_STATE_IDLE,
    PLAYER_STATE_WALKING_FORWARD,
    PLAYER_STATE_WALKING_BACKWARD,
    PLAYER_STATE_JUMPING_STRAIGHT,
    PLAYER_STATE_JUMPING_FORWARD,
    PLAYER_STATE_JUMPING_BACKWARD,
    PLAYER_STATE_JUMP_COOLDOWN,
    PLAYER_STATE_CROUCHING,
    PLAYER_STATE_LP,
    PLAYER_STATE_MP,
    PLAYER_STATE_HP,
    PLAYER_STATE_LK,
    PLAYER_STATE_MK,
    PLAYER_STATE_HK,
    PLAYER_STATE_LP_CLOSE,
    PLAYER_STATE_MP_CLOSE,
    PLAYER_STATE_HP_CLOSE,
    PLAYER_STATE_LK_CLOSE,
    PLAYER_STATE_MK_CLOSE,
    PLAYER_STATE_HK_CLOSE,
    PLAYER_STATE_LP_CROUCH,
    PLAYER_STATE_MP_CROUCH,
    PLAYER_STATE_HP_CROUCH,
    PLAYER_STATE_LK_CROUCH,
    PLAYER_STATE_MK_CROUCH,
    PLAYER_STATE_HK_CROUCH,
    PLAYER_STATE_LP_JUMP_STRAIGHT,
    PLAYER_STATE_MP_JUMP_STRAIGHT,
    PLAYER_STATE_HP_JUMP_STRAIGHT,
    PLAYER_STATE_LK_JUMP_STRAIGHT,
    PLAYER_STATE_MK_JUMP_STRAIGHT,
    PLAYER_STATE_HK_JUMP_STRAIGHT,
    PLAYER_STATE_LP_JUMP_FORWARD,
    PLAYER_STATE_MP_JUMP_FORWARD,
    PLAYER_STATE_HP_JUMP_FORWARD,
    PLAYER_STATE_LK_JUMP_FORWARD,
    PLAYER_STATE_MK_JUMP_FORWARD,
    PLAYER_STATE_HK_JUMP_FORWARD,
    PLAYER_STATE_LP_JUMP_BACKWARD,
    PLAYER_STATE_MP_JUMP_BACKWARD,
    PLAYER_STATE_HP_JUMP_BACKWARD,
    PLAYER_STATE_LK_JUMP_BACKWARD,
    PLAYER_STATE_MK_JUMP_BACKWARD,
    PLAYER_STATE_HK_JUMP_BACKWARD,
    PLAYER_STATE_HIT_UP_STANDING,
    PLAYER_STATE_HIT_MID_STANDING,
    PLAYER_STATE_HIT_CROUCH,
    PLAYER_STATE_DEFENCE_STANDING,
    PLAYER_STATE_DEFENCE_CROUCH,
    PLAYER_STATE_LAST,              // just to mark the last (circular behaviour)
} PlayerState;

typedef struct AnimationFrameBoxes {
    Rectangle collisionBox;
    int hitboxCount;
    Rectangle hitboxes[3];
    int hurtboxCount;
    Rectangle hurtboxes[3];
} AnimationFrameBoxes;

typedef struct AnimationFrame {
    Rectangle source;
    int duration;      // milliseconds OR frames (see updateAnimation)
    Vector2 offset;
    AnimationFrameBoxes boxes;
    bool hitboxesActive;
    bool hurtboxesActive;
    int damageOnHurt;
} AnimationFrame;

typedef struct Animation {
    AnimationFrame *frames;
    int frameCount;
    int currentFrame;
    int frameTimeCounter;
    bool stopAtLastFrame;
    bool runOnce;
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

    DurationMode animationDurationMode;

    Animation idleAnim;
    Animation forwardAnim;
    Animation backwardAnim;
    Animation straightJumpAnim;
    Animation forwardJumpAnim;
    Animation backwardJumpAnim;
    Animation jumpCooldownAnim;
    Animation crouchingAnim;

    Animation lpAnim;
    Animation mpAnim;
    Animation hpAnim;
    Animation lkAnim;
    Animation mkAnim;
    Animation hkAnim;
    
    Animation lpCloseAnim;
    Animation mpCloseAnim;
    Animation hpCloseAnim;
    Animation lkCloseAnim;
    Animation mkCloseAnim;
    Animation hkCloseAnim;

    Animation lpCrouchAnim;
    Animation mpCrouchAnim;
    Animation hpCrouchAnim;
    Animation lkCrouchAnim;
    Animation mkCrouchAnim;
    Animation hkCrouchAnim;

    Animation lpJumpStraightAnim;
    Animation mpJumpStraightAnim;
    Animation hpJumpStraightAnim;
    Animation lkJumpStraightAnim;
    Animation mkJumpStraightAnim;
    Animation hkJumpStraightAnim;

    Animation lpJumpForwardAnim;
    Animation mpJumpForwardAnim;
    Animation hpJumpForwardAnim;
    Animation lkJumpForwardAnim;
    Animation mkJumpForwardAnim;
    Animation hkJumpForwardAnim;

    Animation lpJumpBackwardAnim;
    Animation mpJumpBackwardAnim;
    Animation hpJumpBackwardAnim;
    Animation lkJumpBackwardAnim;
    Animation mkJumpBackwardAnim;
    Animation hkJumpBackwardAnim;

    Animation hitUpStandingAnim;
    Animation hitMidStandingAnim;
    Animation hitCrouchAnim;
    Animation defenceStandingAnim;
    Animation defenceCrouchAnim;

    Animation lastAnim;

    int lpCloseTriggerDist;
    int mpCloseTriggerDist;
    int hpCloseTriggerDist;
    int lkCloseTriggerDist;
    int mkCloseTriggerDist;
    int hkCloseTriggerDist;

    Animation *animations[80];
    int animationCount;

    PlayerState state;
    PlayerState lastState;

    int health;
    char name[100];

    bool lookingRight;

    PlayerKeyBindings kb;

    bool showBoxes;
    bool showDebugInfo;

} Player;

typedef struct GameWorld {

    Rectangle floor;
    Texture2D *stageTexture;

    float gravity;

    Camera2D camera;

    Player *player1;
    Player *player2;

    GameMode mode;

} GameWorld;