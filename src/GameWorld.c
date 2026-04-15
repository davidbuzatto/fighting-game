/**
 * @file GameWorld.h
 * @author Prof. Dr. David Buzatto
 * @brief GameWorld implementation.
 * 
 * @copyright Copyright (c) 2026
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "raylib/raylib.h"
#include "raylib/raymath.h"
//#define RAYGUI_IMPLEMENTATION    // to use raygui, comment these three lines.
//#include "raylib/raygui.h"       // other compilation units must only include
//#undef RAYGUI_IMPLEMENTATION     // raygui.h

#include "Animation.h"
#include "EditorMode.h"
#include "GameCamera.h"
#include "GameCollision.h"
#include "GameWorld.h"
#include "Hud.h"
#include "Macros.h"
#include "ResourceManager.h"
#include "Types.h"
#include "Player.h"
#include "PlayerAnimation.h"
#include "PlayerPallete.h"
#include "PlayerRender.h"
#include "PlayerSelectMode.h"
#include "Projectile.h"
#include "Stage.h"
#include "Utils.h"

#define SHOW_BOXES false
#define SHOW_DEBUG_INFO false
#define ANIMATION_DURATION_MODE DURATION_MODE_MILLISECONDS
#define INITIAL_GAME_MODE GAME_MODE_SELECT_PLAYERS
#define PLAY_MUSIC true
#define PALLETE_COLOR_LIMIT 10
#define REMAINING_TIME 99
#define START_HEALTH 100

void startMatch( GameWorld *gw, PlayerType playerType1, int player1Pallete, PlayerType playerType2, int player2Pallete );
static void drawGameWorldPlaying( GameWorld *gw );
static void updateGameWorldPlaying( GameWorld *gw, float delta );



// gameplay
const PlayerKeyBindings p1KeyBindings = {
    .left   = { KEY_A,         GAMEPAD_BUTTON_LEFT_FACE_LEFT,   INPUT_TYPE_LEFT  },
    .right  = { KEY_D,         GAMEPAD_BUTTON_LEFT_FACE_RIGHT,  INPUT_TYPE_RIGHT },
    .up     = { KEY_W,         GAMEPAD_BUTTON_LEFT_FACE_UP,     INPUT_TYPE_UP    },
    .down   = { KEY_S,         GAMEPAD_BUTTON_LEFT_FACE_DOWN,   INPUT_TYPE_DOWN  },
    .lp     = { KEY_G,         GAMEPAD_BUTTON_RIGHT_FACE_LEFT,  INPUT_TYPE_LP    },
    .mp     = { KEY_H,         GAMEPAD_BUTTON_RIGHT_FACE_UP,    INPUT_TYPE_MP    },
    .hp     = { KEY_J,         GAMEPAD_BUTTON_LEFT_TRIGGER_1,   INPUT_TYPE_HP    },
    .lmhp   = { KEY_T,         GAMEPAD_BUTTON_LEFT_TRIGGER_2,   INPUT_TYPE_LMHP  },
    .lk     = { KEY_B,         GAMEPAD_BUTTON_RIGHT_FACE_DOWN,  INPUT_TYPE_LK    },
    .mk     = { KEY_N,         GAMEPAD_BUTTON_RIGHT_FACE_RIGHT, INPUT_TYPE_MK    },
    .hk     = { KEY_M,         GAMEPAD_BUTTON_RIGHT_TRIGGER_1,  INPUT_TYPE_HK    },
    .lmhk   = { KEY_U,         GAMEPAD_BUTTON_RIGHT_TRIGGER_2,  INPUT_TYPE_LMHK  },
    .select = { KEY_BACKSPACE, GAMEPAD_BUTTON_MIDDLE_LEFT,      INPUT_TYPE_HK    },
    .start  = { KEY_ENTER,     GAMEPAD_BUTTON_MIDDLE_RIGHT,     INPUT_TYPE_HK    },
};

const PlayerKeyBindings p2KeyBindings = {
    .left   = { KEY_LEFT,      GAMEPAD_BUTTON_LEFT_FACE_LEFT,   INPUT_TYPE_LEFT  },
    .right  = { KEY_RIGHT,     GAMEPAD_BUTTON_LEFT_FACE_RIGHT,  INPUT_TYPE_RIGHT },
    .up     = { KEY_UP,        GAMEPAD_BUTTON_LEFT_FACE_UP,     INPUT_TYPE_UP    },
    .down   = { KEY_DOWN,      GAMEPAD_BUTTON_LEFT_FACE_DOWN,   INPUT_TYPE_DOWN  },
    .lp     = { KEY_KP_4,      GAMEPAD_BUTTON_RIGHT_FACE_LEFT,  INPUT_TYPE_LP    },
    .mp     = { KEY_KP_5,      GAMEPAD_BUTTON_RIGHT_FACE_UP,    INPUT_TYPE_MP    },
    .hp     = { KEY_KP_6,      GAMEPAD_BUTTON_LEFT_TRIGGER_1,   INPUT_TYPE_HP    },
    .lmhp   = { KEY_KP_7,      GAMEPAD_BUTTON_LEFT_TRIGGER_2,   INPUT_TYPE_LMHP  },
    .lk     = { KEY_KP_1,      GAMEPAD_BUTTON_RIGHT_FACE_DOWN,  INPUT_TYPE_LK    },
    .mk     = { KEY_KP_2,      GAMEPAD_BUTTON_RIGHT_FACE_RIGHT, INPUT_TYPE_MK    },
    .hk     = { KEY_KP_3,      GAMEPAD_BUTTON_RIGHT_TRIGGER_1,  INPUT_TYPE_HK    },
    .lmhk   = { KEY_KP_9,      GAMEPAD_BUTTON_RIGHT_TRIGGER_2,  INPUT_TYPE_LMHK  },
    .select = { KEY_KP_ADD,    GAMEPAD_BUTTON_MIDDLE_LEFT,      INPUT_TYPE_HK    },
    .start  = { KEY_KP_ENTER,  GAMEPAD_BUTTON_MIDDLE_RIGHT,     INPUT_TYPE_HK    },
};

// for camera target on playing
static float playerDist = 0.0f;

// flip players logic

// match data
Match match;
static bool clearLastMatch = false;

bool playMusic = PLAY_MUSIC;



/**
 * @brief Creates a dinamically allocated GameWorld struct instance.
 */
GameWorld* createGameWorld( void ) {

    GameWorld *gw = (GameWorld*) malloc( sizeof( GameWorld ) );

    float floorHeight = 30;

    gw->floor = (Rectangle) {
        0, GetScreenHeight() - floorHeight, GetScreenWidth(), floorHeight
    };

    gw->anchorTexture = &rm.kenStageAnchorTexture;
    gw->floorTexture = &rm.kenStageFloorTexture;
    gw->back01Texture = &rm.kenStageBack01Texture;
    gw->back02Texture = &rm.kenStageBack02Texture;
    gw->back03Texture = &rm.kenStageBack03Texture;
    gw->boat01Texture = &rm.kenStageBoat01Texture;
    gw->boat02Texture = &rm.kenStageBoat02Texture;
    gw->boat03Texture = &rm.kenStageBoat03Texture;
    gw->boat04Texture = &rm.kenStageBoat04Texture;
    gw->boat05Texture = &rm.kenStageBoat05Texture;

    initStage( gw );

    gw->gravity = 1200;

    gw->camera = (Camera2D) {
        .offset = { GetScreenWidth() / 2, 0 },
        .rotation = 0.0f,
        .target = {
            0,
            425
        },
        .zoom = 2.75f
    };

    gw->mode = INITIAL_GAME_MODE;
    gw->player1 = NULL;
    gw->player2 = NULL;

    if ( gw->mode == GAME_MODE_PLAYING || gw->mode == GAME_MODE_EDITING ) {
        startMatch( gw, PLAYER_TYPE_RYU, 9, PLAYER_TYPE_KEN, 0 );
        updateCameraPlaying( gw );
    }

    return gw;

}

/**
 * @brief Destroys a GameWindow object and its dependecies.
 */
void destroyGameWorld( GameWorld *gw ) {
    if ( gw->player1 != NULL ) {
        destroyPlayer( gw->player1 );
    }
    if ( gw->player2 != NULL ) {
        destroyPlayer( gw->player2 );
    }
    free( gw );
}

/**
 * @brief Reads user input and updates the state of the game.
 */
void updateGameWorld( GameWorld *gw, float delta ) {

    if ( IsKeyPressed( KEY_F1 ) ) {

        if ( gw->mode == GAME_MODE_PLAYING ) {
            gw->mode = GAME_MODE_EDITING;
        } else if ( gw->mode == GAME_MODE_EDITING ) {
            gw->mode = GAME_MODE_PLAYING;
            resetPlayerAnimations( gw->player1 );
        }

        if ( gw->mode == GAME_MODE_EDITING ) {
            if ( lastEditState == PLAYER_STATE_LAST ) {
                gw->player1->state = PLAYER_STATE_IDLE;
            } else {
                gw->player1->state = lastEditState;
            }
            Animation *a = getPlayerCurrentAnimation( gw->player1 );
            if ( a != NULL )  {
                a->currentFrame = lastEditFrame;
            }
        }

    }

    if ( gw->mode == GAME_MODE_PLAYING || gw->mode == GAME_MODE_PLAYING ) {

        if ( IsKeyPressed( KEY_F2 ) ) {
            gw->player1->showBoxes = !gw->player1->showBoxes;
            gw->player2->showBoxes = !gw->player2->showBoxes;
        }

        if ( IsKeyPressed( KEY_F3 ) ) {
            gw->player1->showDebugInfo = !gw->player1->showDebugInfo;
            gw->player2->showDebugInfo = !gw->player2->showDebugInfo;
        }

        if ( IsKeyPressed( KEY_F4 ) ) {
            showPlayerOnionEditing = !showPlayerOnionEditing;
        }

        if ( IsKeyPressed( KEY_F5 ) ) {
            showPlayerInputBuffer = !showPlayerInputBuffer;
        }

    }

    if ( gw->mode == GAME_MODE_SELECT_PLAYERS ) {
        updateGameWorldSelectingPlayers( gw, delta );
    } else if ( gw->mode == GAME_MODE_PLAYING ) {
        updateGameWorldPlaying( gw, delta );
    } else {
        updateGameWorldEditing( gw, delta );
    }

}

/**
 * @brief Draws the state of the game.
 */
void drawGameWorld( GameWorld *gw ) {

    BeginDrawing();

    if ( gw->mode == GAME_MODE_SELECT_PLAYERS ) {
        drawGameWorldSelectingPlayers( gw );
    } else if ( gw->mode == GAME_MODE_PLAYING ) {
        drawGameWorldPlaying( gw );
    } else {
        drawGameWorldEditing( gw );
    }

    EndDrawing();

}

void startMatch( GameWorld *gw, PlayerType playerType1, int player1Pallete, PlayerType playerType2, int player2Pallete ) {

    if ( clearLastMatch ) {
        destroyPlayer( gw->player1 );
        destroyPlayer( gw->player2 );
    }
    clearLastMatch = true;

    Player *player1 = createPlayer();
    Player *player2 = createPlayer();

    gw->player1 = player1;
    gw->player2 = player2;

    if ( playerType1 == PLAYER_TYPE_RYU ) {
        initializePlayerRyu( gw->back01Texture->width / 2 - 78, 552, player1, PLAYER_START_SIDE_LEFT, 0, ANIMATION_DURATION_MODE, SHOW_BOXES, SHOW_DEBUG_INFO );
        loadPlayerAnimationFrameBoxes( gw->player1, RYU_ANIMATIONS_FILE );
    } else {
        initializePlayerKen( gw->back01Texture->width / 2 - 78, 552, player1, PLAYER_START_SIDE_LEFT, 0, ANIMATION_DURATION_MODE, SHOW_BOXES, SHOW_DEBUG_INFO );
        loadPlayerAnimationFrameBoxes( gw->player1, KEN_ANIMATIONS_FILE );
    }

    if ( playerType2 == PLAYER_TYPE_RYU ) {
        initializePlayerRyu( gw->back01Texture->width / 2 + 50, 552, player2, PLAYER_START_SIDE_RIGHT, 1, ANIMATION_DURATION_MODE, SHOW_BOXES, SHOW_DEBUG_INFO );
        loadPlayerAnimationFrameBoxes( gw->player2, RYU_ANIMATIONS_FILE );
    } else {
        initializePlayerKen( gw->back01Texture->width / 2 + 50, 552, player2, PLAYER_START_SIDE_RIGHT, 1, ANIMATION_DURATION_MODE, SHOW_BOXES, SHOW_DEBUG_INFO );
        loadPlayerAnimationFrameBoxes( gw->player2, KEN_ANIMATIONS_FILE );
    }

    player1->kb = p1KeyBindings;
    player2->kb = p2KeyBindings;

    player1RightPlayer2 = false;
    needsToFlipPlayers = false;

    changePlayerPallete( player1, player1Pallete, PALLETE_COLOR_LIMIT );
    changePlayerPallete( player2, player2Pallete, PALLETE_COLOR_LIMIT );

    updateCameraPlaying( gw );

    match = (Match) {
        .state = MATCH_STATE_ROUND_1_STARTING,
        .timeToStart = 2.0f,
        .timeToKO = 4.0f,
        .timeToFinish = 6.0f,
        .remainingTime = REMAINING_TIME,
        .timeCounter = 0.0f,
        .roundsFinished = 0,
        .timeToShowMatchText = 2.0f,
        .timeToShowMatchTextCounter = 0.0f,
        .showMatchText = true,
        .playRoundFight = true,
    };

    gw->player1->health = START_HEALTH;
    gw->player2->health = START_HEALTH;

}

static void drawGameWorldPlaying( GameWorld *gw ) {

    ClearBackground( WHITE );
    
    if ( SHOW_MODEL_STAGE_TEXTURE ) {
        DrawTexture( rm.modelStageTexture, 0, 0, WHITE );
    }

    BeginMode2D( gw->camera );

    drawStageBackground( gw );

    drawPlayerShadow( gw->player2, gw->floor.y - 2, 40.0f, 0.25f );
    drawPlayerShadow( gw->player1, gw->floor.y - 2, 40.0f, 0.25f );
    //drawPlayerShadow( gw->player2, gw->floor.y - 2, 60.0f, 0.15f );
    //drawPlayerShadow( gw->player1, gw->floor.y - 2, 60.0f, 0.15f );

    drawPlayer( gw->player2 );
    drawPlayer( gw->player1 );

    drawOnHitPlayerAnimation( gw->player1 );
    drawOnHitPlayerAnimation( gw->player2 );

    drawOnBlockPlayerAnimation( gw->player1 );
    drawOnBlockPlayerAnimation( gw->player2 );

    drawPlayerProjectile( gw->player1 );
    drawPlayerProjectile( gw->player2 );

    drawStageForeground( gw );

    EndMode2D();

    drawHud( gw );

    if ( showPlayerInputBuffer ) {
        drawPlayerInputBuffer( gw->player1 );
        drawPlayerInputBuffer( gw->player2 );
    }

    switch ( match.state ) {
        case MATCH_STATE_ROUND_1_STARTING: {
                const char *t = "ROUND 1";
                Vector2 d = measureTextUsingFont( t, 4.0f, -5 );
                drawTextUsingFont( t, GetScreenWidth()  / 2 - d.x / 2, GetScreenHeight() / 2 - 80, 4.0, -5 );
            }
            break;
        case MATCH_STATE_ROUND_2_STARTING: {
                const char *t = "ROUND 2";
                Vector2 d = measureTextUsingFont( t, 4.0f, -5 );
                drawTextUsingFont( t, GetScreenWidth()  / 2 - d.x / 2, GetScreenHeight() / 2 - 80, 4.0, -5 );
            }
            break;
        case MATCH_STATE_ROUND_FINAL_STARTING: {
                const char *t = "FINAL ROUND";
                Vector2 d = measureTextUsingFont( t, 4.0f, -5 );
                drawTextUsingFont( t, GetScreenWidth()  / 2 - d.x / 2, GetScreenHeight() / 2 - 80, 4.0, -5 );
            }
            break;

        case MATCH_STATE_ROUND_1_FIGHTING:
        case MATCH_STATE_ROUND_2_FIGHTING:
        case MATCH_STATE_ROUND_FINAL_FIGHTING: {
                if ( match.showMatchText ) {
                    const char *t = "FIGHT";
                    Vector2 d = measureTextUsingFont( t, 4.0f, -5 );
                    drawTextUsingFont( t, GetScreenWidth()  / 2 - d.x / 2, GetScreenHeight() / 2 - 80, 4.0, -5 );
                }
            }
            break;

        case MATCH_STATE_KO: {
                const char *t = "KO";
                Vector2 d = measureTextUsingFont( t, 5.0f, -2 );
                drawTextUsingFont( t, GetScreenWidth()  / 2 - d.x / 2, GetScreenHeight() / 2 - 80, 5.0, -2 );
            }
            break;

        case MATCH_STATE_DOUBLE_KO: {
                const char *t = "DOUBLE KO";
                Vector2 d = measureTextUsingFont( t, 5.0f, -2 );
                drawTextUsingFont( t, GetScreenWidth()  / 2 - d.x / 2, GetScreenHeight() / 2 - 80, 5.0, -2 );
            }
            break;

        case MATCH_STATE_FINISHING: {
                const char *t = TextFormat( "%s WINS", gw->player1->roundsWon == 2 ? gw->player1->name : gw->player2->name );
                Vector2 d = measureTextUsingFont( t, 5.0f, -2 );
                drawTextUsingFont( t, GetScreenWidth()  / 2 - d.x / 2, GetScreenHeight() / 2 - 80, 5.0, -2 );
            }
            break;
    }

}

static void updateGameWorldPlaying( GameWorld *gw, float delta ) {

    if ( playMusic ) {
        if ( !IsMusicStreamPlaying( rm.kenTheme ) )  {
            PlayMusicStream( rm.kenTheme );
        }
        UpdateMusicStream( rm.kenTheme );
    }

    if ( IsKeyPressed( KEY_F11 ) ) {

        if ( playMusic ) {
            if ( IsMusicStreamPlaying( rm.kenTheme ) )  {
                StopMusicStream( rm.kenTheme );
            }   
        }

        p1SelectedLine = 0;
        p1SelectedColumn = 0;
        p2SelectedLine = 1;
        p2SelectedColumn = 0;
        pSelectBlinkCounter = 0.0f;
        pSelectBlink = false;
        pSelectTransitionCounter = 0.0f;
        startTransitionToPlay = false;
        p1Selected = false;
        p2Selected = false;
        p1SelectedPallete = -1;
        p2SelectedPallete = -1;
        gw->mode = GAME_MODE_SELECT_PLAYERS;
        return;

    }

    /*
    int keyPressed = GetKeyPressed();
    if ( keyPressed >= KEY_ZERO && keyPressed <= KEY_NINE ) {
        int pallete = keyPressed - KEY_ZERO;
        if ( IsKeyDown( KEY_LEFT_CONTROL ) ) {
            changePlayerPallete( gw->player2, pallete, PALLETE_COLOR_LIMIT );
        } else {
            changePlayerPallete( gw->player1, pallete, PALLETE_COLOR_LIMIT );
        }
    }
    */

    updateStage( delta );

    /*
    if ( IsKeyPressed( KEY_R ) ) {
        gw->player1->health = 100;
        gw->player2->health = 100;
    }
    */

    gw->frameCounter++;
    bool discardInput = !( match.state >= MATCH_STATE_ROUND_1_FIGHTING && match.state <= MATCH_STATE_ROUND_FINAL_FIGHTING );
    processInputPlayer( gw->player1, gw->player2, delta, gw->frameCounter, discardInput );
    processInputPlayer( gw->player2, gw->player1, delta, gw->frameCounter, discardInput );

    // camera
    playerDist = fabs( gw->player1->pos.x - gw->player2->pos.x ) * gw->camera.zoom;
    float width = ( gw->player1->dim.x + gw->player2->dim.x ) / 2 * gw->camera.zoom;
    bool adjust = false;
    if ( playerDist > GetScreenWidth() - width ) {
        gw->player1->vel.x = 0.0f;
        gw->player2->vel.x = 0.0f;
        adjust = true;
    }

    updatePlayer( gw->player1, gw->player2, gw->camera, gw->gravity, delta );
    updatePlayer( gw->player2, gw->player1, gw->camera, gw->gravity, delta );

    // camera
    if ( adjust ) {
        if ( !player1RightPlayer2 ) {
            gw->player1->pos.x += 0.1f;
            gw->player2->pos.x -= 0.1f;
        } else {
            gw->player1->pos.x -= 0.1f;
            gw->player2->pos.x += 0.1f;
        }
    }

    resolveCollisionPlayerStage( gw->player1, gw );
    resolveCollisionPlayerStage( gw->player2, gw );

    resolvePlayerPlayerCollision( gw->player1, gw->player2 );

    resolvePlayerOponnentContact( gw->player1, gw->player2 );
    resolvePlayerOponnentContact( gw->player2, gw->player1 );

    resolvePlayerOponnentProjectileContact( gw->player1, gw->player2 );
    resolvePlayerOponnentProjectileContact( gw->player2, gw->player1 );

    flipPlayers( gw );

    updateCameraPlaying( gw );

    switch ( match.state ) {
        case MATCH_STATE_ROUND_1_STARTING:
            if ( match.playRoundFight ) {
                PlaySound( rm.round1FightSound );
                match.playRoundFight = false;
            }
            match.timeCounter += delta;
            if ( match.timeCounter >= match.timeToStart ) {
                match.timeCounter = 0.0f;
                match.state = MATCH_STATE_ROUND_1_FIGHTING;
                match.showMatchText = true;
            }
            break;
        case MATCH_STATE_ROUND_2_STARTING:
            if ( match.playRoundFight ) {
                PlaySound( rm.round2FightSound );
                match.playRoundFight = false;
            }
            match.timeCounter += delta;
            if ( match.timeCounter >= match.timeToStart ) {
                match.timeCounter = 0.0f;
                match.state = MATCH_STATE_ROUND_2_FIGHTING;
                match.showMatchText = true;
            }
            break;
        case MATCH_STATE_ROUND_FINAL_STARTING:
            if ( match.playRoundFight ) {
                PlaySound( rm.finalRoundFightSound );
                match.playRoundFight = false;
            }
            match.timeCounter += delta;
            if ( match.timeCounter >= match.timeToStart ) {
                match.timeCounter = 0.0f;
                match.state = MATCH_STATE_ROUND_FINAL_FIGHTING;
                match.showMatchText = true;
            }
            break;

        case MATCH_STATE_ROUND_1_FIGHTING:
        case MATCH_STATE_ROUND_2_FIGHTING:
        case MATCH_STATE_ROUND_FINAL_FIGHTING:
            match.timeToShowMatchTextCounter += delta;
            if ( match.timeToShowMatchTextCounter >= match.timeToShowMatchText ) {
                match.timeToShowMatchTextCounter = 0.0f;
                match.showMatchText = false;
            }
            break;

        case MATCH_STATE_KO:
            match.timeCounter += delta;
            if ( match.timeCounter >= match.timeToStart ) {
                match.timeCounter = 0.0f;
                match.remainingTime = REMAINING_TIME;
                match.playRoundFight = true;
                if ( match.roundsFinished == 0 ) {
                    match.state = MATCH_STATE_ROUND_1_STARTING;
                } else if ( match.roundsFinished == 1 ) {
                    match.state = MATCH_STATE_ROUND_2_STARTING;
                } else if ( match.roundsFinished == 2 ) {
                    match.state = MATCH_STATE_ROUND_FINAL_STARTING;
                }
                gw->player1->state = PLAYER_STATE_IDLE;
                gw->player1->pos = (Vector2) { gw->back01Texture->width / 2 - 78, 552 };
                gw->player1->health = START_HEALTH;
                resetPlayerAnimations( gw->player1 );
                //gw->player1->lookingRight = true;

                gw->player2->state = PLAYER_STATE_IDLE;
                gw->player2->pos = (Vector2) { gw->back01Texture->width / 2 + 50, 552 };
                gw->player2->health = START_HEALTH;
                resetPlayerAnimations( gw->player2 );
                //gw->player2->lookingRight = false;

                updateCameraPlaying( gw );

            }
            break;

        case MATCH_STATE_DOUBLE_KO:
            match.timeCounter += delta;
            if ( match.timeCounter >= match.timeToStart ) {

                match.timeCounter = 0.0f;
                match.remainingTime = REMAINING_TIME;
                match.playRoundFight = true;

                if ( match.roundsFinished == 0 ) {
                    match.state = MATCH_STATE_ROUND_1_STARTING;
                } else if ( match.roundsFinished == 1 ) {
                    match.state = MATCH_STATE_ROUND_2_STARTING;
                } else if ( match.roundsFinished >= 2 ) {
                    match.state = MATCH_STATE_ROUND_FINAL_STARTING;
                }

                gw->player1->state = PLAYER_STATE_IDLE;
                gw->player1->pos = (Vector2) { gw->back01Texture->width / 2 - 78, 552 };
                gw->player1->health = START_HEALTH;
                resetPlayerAnimations( gw->player1 );
                //gw->player1->lookingRight = true;

                gw->player2->state = PLAYER_STATE_IDLE;
                gw->player2->pos = (Vector2) { gw->back01Texture->width / 2 + 50, 552 };
                gw->player2->health = START_HEALTH;
                resetPlayerAnimations( gw->player2 );
                //gw->player2->lookingRight = false;

                updateCameraPlaying( gw );

            }
            break;

        case MATCH_STATE_FINISHING:
            match.timeCounter += delta;
            if ( match.timeCounter >= match.timeToStart ) {
                
                if ( playMusic ) {
                    if ( IsMusicStreamPlaying( rm.kenTheme ) )  {
                        StopMusicStream( rm.kenTheme );
                    }   
                }

                p1SelectedLine = 0;
                p1SelectedColumn = 0;
                p2SelectedLine = 1;
                p2SelectedColumn = 0;
                pSelectBlinkCounter = 0.0f;
                pSelectBlink = false;
                pSelectTransitionCounter = 0.0f;
                startTransitionToPlay = false;
                p1Selected = false;
                p2Selected = false;
                p1SelectedPallete = -1;
                p2SelectedPallete = -1;
                gw->mode = GAME_MODE_SELECT_PLAYERS;
                
            }
            break;

    }

    if ( match.state >= MATCH_STATE_ROUND_1_FIGHTING && match.state <= MATCH_STATE_ROUND_FINAL_FIGHTING ) {

        if ( match.remainingTime > 0 ) {
            match.remainingTime -= delta;
            if ( match.remainingTime < 0.0f ) {
                match.remainingTime = 0.0f;
            }
        }

        if ( gw->player1->health <= 0 && gw->player2->health <= 0 ) {

            match.state = MATCH_STATE_DOUBLE_KO;
            //match.roundsFinished++;

            gw->player1->state = PLAYER_STATE_TIMEOVER;
            gw->player1->vel.x = 0.0f;
            gw->player1->vel.y = 0.0f;

            gw->player2->state = PLAYER_STATE_TIMEOVER;
            gw->player2->vel.x = 0.0f;
            gw->player2->vel.y = 0.0f;

        } else if ( gw->player1->health <= 0 ) {

            match.state = MATCH_STATE_KO;
            match.roundsFinished++;

            gw->player2->roundsWon++;
            gw->player2->state = GetRandomValue( 0, 1 ) == 0 ? PLAYER_STATE_VICTORY_1 : PLAYER_STATE_VICTORY_2;
            gw->player2->vel.x = 0.0f;
            gw->player2->vel.y = 0.0f;

            gw->player1->state = PLAYER_STATE_FALLING_LOSE;
            gw->player1->vel.x = 0.0f;
            gw->player1->vel.y = 0.0f;

            if ( gw->player2->roundsWon == 2 ) {
                match.state = MATCH_STATE_FINISHING;
            }

        } else if ( gw->player2->health <= 0 ) {

            match.state = MATCH_STATE_KO;
            match.roundsFinished++;

            gw->player1->roundsWon++;
            gw->player1->state = GetRandomValue( 0, 1 ) == 0 ? PLAYER_STATE_VICTORY_1 : PLAYER_STATE_VICTORY_2;
            gw->player1->vel.x = 0.0f;
            gw->player1->vel.y = 0.0f;

            gw->player2->state = PLAYER_STATE_FALLING_LOSE;
            gw->player2->vel.x = 0.0f;
            gw->player2->vel.y = 0.0f;

            if ( gw->player1->roundsWon == 2 ) {
                match.state = MATCH_STATE_FINISHING;
            }

        }

        if ( match.remainingTime == 0.0f ) {

            if ( gw->player1->health < gw->player2->health ) {

                match.state = MATCH_STATE_KO;
                match.roundsFinished++;

                gw->player2->roundsWon++;
                gw->player2->state = GetRandomValue( 0, 1 ) == 0 ? PLAYER_STATE_VICTORY_1 : PLAYER_STATE_VICTORY_2;
                gw->player2->vel.x = 0.0f;
                gw->player2->vel.y = 0.0f;

                gw->player1->state = PLAYER_STATE_FALLING_LOSE;
                gw->player1->vel.x = 0.0f;
                gw->player1->vel.y = 0.0f;

                if ( gw->player2->roundsWon == 2 ) {
                    match.state = MATCH_STATE_FINISHING;
                }

            } else if ( gw->player1->health > gw->player2->health ) {

                match.state = MATCH_STATE_KO;
                match.roundsFinished++;

                gw->player1->roundsWon++;
                gw->player1->state = GetRandomValue( 0, 1 ) == 0 ? PLAYER_STATE_VICTORY_1 : PLAYER_STATE_VICTORY_2;
                gw->player1->vel.x = 0.0f;
                gw->player1->vel.y = 0.0f;

                gw->player2->state = PLAYER_STATE_FALLING_LOSE;
                gw->player2->vel.x = 0.0f;
                gw->player2->vel.y = 0.0f;

                if ( gw->player1->roundsWon == 2 ) {
                    match.state = MATCH_STATE_FINISHING;
                }

            } else {

                match.state = MATCH_STATE_DOUBLE_KO;

                gw->player1->state = PLAYER_STATE_TIMEOVER;
                gw->player1->vel.x = 0.0f;
                gw->player1->vel.y = 0.0f;

                gw->player2->state = PLAYER_STATE_TIMEOVER;
                gw->player2->vel.x = 0.0f;
                gw->player2->vel.y = 0.0f;

            }

        }

    }

}

