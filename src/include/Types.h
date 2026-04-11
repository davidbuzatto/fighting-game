#pragma once

#include "raylib/raylib.h"

#define PLAYER_INPUT_BUFFER_SIZE 14
#define COMMAND_INPUT_WINDOW 30          // frames to complete a command input (~500ms at 60fps)
#define MAX_COMMAND_SEQUENCE 6
#define MAX_COMMANDS 10
#define PUSHBACK_ON_HIT 120.0f          // fixed pushback speed applied to defender on hit
#define PUSHBACK_ON_BLOCK 80.0f         // fixed pushback speed applied to defender on block
#define PUSHBACK_DECAY 0.90f            // per-frame velocity decay during hit/block stun

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

typedef enum InputType {
    INPUT_TYPE_RIGHT,
    INPUT_TYPE_DOWN,
    INPUT_TYPE_LEFT,
    INPUT_TYPE_UP,
    INPUT_TYPE_RIGHT_DOWN,
    INPUT_TYPE_LEFT_DOWN,
    INPUT_TYPE_LEFT_UP,
    INPUT_TYPE_RIGHT_UP,
    INPUT_TYPE_LP,
    INPUT_TYPE_MP,
    INPUT_TYPE_HP,
    INPUT_TYPE_LK,
    INPUT_TYPE_MK,
    INPUT_TYPE_HK,
    INPUT_TYPE_NEUTRAL
} InputType;

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
    PLAYER_STATE_SPECIAL_LP_HADOUKEN,
    PLAYER_STATE_SPECIAL_MP_HADOUKEN,
    PLAYER_STATE_SPECIAL_HP_HADOUKEN,
    PLAYER_STATE_SPECIAL_LP_SHORYUKEN,
    PLAYER_STATE_SPECIAL_MP_SHORYUKEN,
    PLAYER_STATE_SPECIAL_HP_SHORYUKEN,
    PLAYER_STATE_SPECIAL_LK_TATSUMAKI,
    PLAYER_STATE_SPECIAL_MK_TATSUMAKI,
    PLAYER_STATE_SPECIAL_HK_TATSUMAKI,
    PLAYER_STATE_FALLING,
    PLAYER_STATE_GETTING_UP,
    PLAYER_STATE_VICTORY_1,
    PLAYER_STATE_VICTORY_2,
    PLAYER_STATE_FALLING_LOSE,
    PLAYER_STATE_TIMEOVER,
    PLAYER_STATE_LAST,              // just to mark the last (circular behaviour)
} PlayerState;

typedef enum CommandType {
    COMMAND_TYPE_HADOUKEN,
    COMMAND_TYPE_SHORYUKEN,
    COMMAND_TYPE_TATSUMAKI,
    // add more as needed
} CommandType;

typedef enum PlayerStartSide {
    PLAYER_START_SIDE_LEFT,
    PLAYER_START_SIDE_RIGHT,
} PlayerStartSide;

typedef enum ProjectileType {
    PROJECTILE_TYPE_LOW,
    PROJECTILE_TYPE_MID,
    PROJECTILE_TYPE_HIGH,
} ProjectileType;

typedef struct CommandInput {
    CommandType type;
    InputType sequence[MAX_COMMAND_SEQUENCE];  // directional sequence (right-facing)
    int sequenceLength;
    bool requiresPunch;     // true = any punch completes the command
    bool requiresKick;      // true = any kick completes the command
    int frameWindow;        // max frames to complete the sequence
} CommandInput;

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

typedef struct InputBufferEntry {
    InputType type;
    int frame;          // frame number when this input was recorded
} InputBufferEntry;

typedef struct InputEntry {
    int key;
    InputType type;
} InputEntry;

typedef struct PlayerKeyBindings {
    InputEntry left;
    InputEntry right;
    InputEntry up;
    InputEntry down;
    InputEntry lp;
    InputEntry mp;
    InputEntry hp;
    InputEntry lk;
    InputEntry mk;
    InputEntry hk;
} PlayerKeyBindings;

typedef struct Projectile {
    ProjectileType type;
    bool active;
    Vector2 pos;
    Vector2 vel;
    Rectangle hurtbox;
    int damageOnHurt;
    bool runImpactAnim;
    Animation startupAnim;
    Animation continuousAnimLP;
    Animation continuousAnimDetailLP;
    Animation continuousAnimMP;
    Animation continuousAnimDetailMP;
    Animation continuousAnimHP;
    Animation continuousAnimDetailHP;
    Animation impactAnim;
} Projectile;

typedef struct PlayerSounds {
    Sound attackLowSound;
    Sound attackMidSound;
    Sound attackHighSound;
    Sound hitSound;
    Sound hadoukenSound;
    Sound shoryukenSound;
    Sound tatsumakiSound;
} PlayerSounds;

typedef struct Player {

    Texture2D *spriteMap;
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

    Animation lpHadoukenAnim;
    Animation mpHadoukenAnim;
    Animation hpHadoukenAnim;
    Animation lpShoryukenAnim;
    Animation mpShoryukenAnim;
    Animation hpShoryukenAnim;
    Animation lkTatsumakiAnim;
    Animation mkTatsumakiAnim;
    Animation hkTatsumakiAnim;

    Animation fallingAnim;
    Animation gettingUpAnim;
    
    Animation victory1Anim;
    Animation victory2Anim;
    Animation fallingLoseAnim;
    Animation timeOverAnim;

    Animation lastAnim;

    int lpCloseTriggerDist;
    int mpCloseTriggerDist;
    int hpCloseTriggerDist;
    int lkCloseTriggerDist;
    int mkCloseTriggerDist;
    int hkCloseTriggerDist;

    Animation *animations[80];
    int animationCount;

    Animation onHitAnimation;
    Vector2 onHitPos;
    bool onHitPosActive;

    Animation onBlockAnimation;
    Vector2 onBlockPos;
    bool onBlockPosActive;

    Animation *supportAnimations[80];
    int supportAnimationCount;

    Projectile *projectile;

    PlayerState state;
    PlayerState lastState;

    int health;
    char name[100];

    bool lookingRight;
    PlayerStartSide startSide;

    PlayerKeyBindings kb;

    bool showBoxes;
    bool showDebugInfo;

    InputBufferEntry inputBuffer[PLAYER_INPUT_BUFFER_SIZE];
    int inputBufferHead;
    int inputBufferTail;
    int inputBufferSize;
    InputType lastDirectionalState;   // directional state from previous frame

    CommandInput commands[MAX_COMMANDS];
    int commandCount;

    PlayerSounds sounds;

} Player;

typedef struct GameWorld {

    Rectangle floor;

    Texture2D *anchorTexture;
    Texture2D *floorTexture;
    Texture2D *back01Texture;
    Texture2D *back02Texture;
    Texture2D *back03Texture;
    Texture2D *boat01Texture;
    Texture2D *boat02Texture;
    Texture2D *boat03Texture;
    Texture2D *boat04Texture;
    Texture2D *boat05Texture;

    float gravity;

    Camera2D camera;

    Player *player1;
    Player *player2;

    GameMode mode;

    int frameCounter;   // incremented every frame, used for input buffer timing

} GameWorld;

static inline bool isAttackState( PlayerState s ) {
    return s >= PLAYER_STATE_LP && s <= PLAYER_STATE_HK_JUMP_BACKWARD;
}

static inline bool isCrouchAttackState( PlayerState s ) {
    return s >= PLAYER_STATE_LP_CROUCH && s <= PLAYER_STATE_HK_CROUCH;
}

static inline bool isJumpAttackState( PlayerState s ) {
    return s >= PLAYER_STATE_LP_JUMP_STRAIGHT && s <= PLAYER_STATE_HK_JUMP_BACKWARD;
}

static inline bool isJumpState( PlayerState s ) {
    return s == PLAYER_STATE_JUMPING_STRAIGHT ||
           s == PLAYER_STATE_JUMPING_FORWARD  ||
           s == PLAYER_STATE_JUMPING_BACKWARD;
}

static inline bool isHitState( PlayerState s ) {
    return s >= PLAYER_STATE_HIT_UP_STANDING && s <= PLAYER_STATE_HIT_CROUCH;
}

static inline bool isDefenceState( PlayerState s ) {
    return s == PLAYER_STATE_DEFENCE_STANDING || s == PLAYER_STATE_DEFENCE_CROUCH;
}

static inline bool isAirborneState( PlayerState s ) {
    return isJumpState( s ) || isJumpAttackState( s );
}

static inline bool isSpecialMoveState( PlayerState s ) {
    return s >= PLAYER_STATE_SPECIAL_LP_HADOUKEN && s <= PLAYER_STATE_SPECIAL_HK_TATSUMAKI;
}

static inline bool isBringDownMoveState( PlayerState s ) {
    return s == PLAYER_STATE_HK || s == PLAYER_STATE_HK_CROUCH;
}

static inline bool isAttackInput( InputType t ) {
    return t >= INPUT_TYPE_LP && t <= INPUT_TYPE_HK;
}

static inline bool isPunchInput( InputType t ) {
    return t == INPUT_TYPE_LP || t == INPUT_TYPE_MP || t == INPUT_TYPE_HP;
}

static inline bool isKickInput( InputType t ) {
    return t == INPUT_TYPE_LK || t == INPUT_TYPE_MK || t == INPUT_TYPE_HK;
}

// mirrors a directional input for left-facing players
// (commands are defined assuming right-facing)
static inline InputType mirrorDirectional( InputType t ) {
    switch ( t ) {
        case INPUT_TYPE_RIGHT:      return INPUT_TYPE_LEFT;
        case INPUT_TYPE_LEFT:       return INPUT_TYPE_RIGHT;
        case INPUT_TYPE_RIGHT_DOWN: return INPUT_TYPE_LEFT_DOWN;
        case INPUT_TYPE_LEFT_DOWN:  return INPUT_TYPE_RIGHT_DOWN;
        case INPUT_TYPE_RIGHT_UP:   return INPUT_TYPE_LEFT_UP;
        case INPUT_TYPE_LEFT_UP:    return INPUT_TYPE_RIGHT_UP;
        default:                    return t;
    }
}