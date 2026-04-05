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
//#include "raylib/raymath.h"
//#define RAYGUI_IMPLEMENTATION    // to use raygui, comment these three lines.
//#include "raylib/raygui.h"       // other compilation units must only include
//#undef RAYGUI_IMPLEMENTATION     // raygui.h

#include "Animation.h"
#include "GameWorld.h"
#include "Macros.h"
#include "ResourceManager.h"
#include "Types.h"
#include "Player.h"
#include "Utils.h"

#define SHOW_PLAYER_INPUT_BUFFER true
#define SHOW_BOXES false
#define SHOW_DEBUG_INFO false
#define SHOW_MODEL_STAGE_TEXTURE false
#define SHOW_PLAYER_ONION_MODE_EDITING true
#define DURATION_MODE DURATION_MODE_MILLISECONDS
#define INITIAL_GAME_MODE GAME_MODE_PLAYING

#define PLAYER_1_ANIMATIONS_FILE "resources/animations/ryu.json"
#define PLAYER_2_ANIMATIONS_FILE "resources/animations/ken.json"

static void drawGameWorldPlaying( GameWorld *gw );
static void drawGameWorldEditing( GameWorld *gw );
static void updateGameWorldPlaying( GameWorld *gw, float delta );
static void updateGameWorldEditing( GameWorld *gw, float delta );
static void editAnimationFrameBox( Rectangle *box );
static void showAnimationFrameBoxDetail( Player *p, Rectangle *box, Camera2D camera, Color color );
static void drawInfoPanel( GameWorld *gw );
static void drawEditorHelp( void );
static void drawHud( GameWorld *gw );
static void copyAnimationFrameBoxesPrevious( Player *p );
static void copyAnimationFrameBoxesNext( Player *p );
static void copyAllFrameBoxesToPreviousAnimation( Player *p );
static void copyAllFrameBoxesToNextAnimation( Player *p );

static void updateCameraPlaying( GameWorld *gw );
static void updateCameraEditing( GameWorld *gw );

static void resolveCollisionPlayerStage( Player *player, GameWorld *gw );
static void resolvePlayerPlayerCollision( Player *p1, Player *p2 );
static void flipPlayers( GameWorld *gw );

// editor
static EditorMode editorMode = EDITOR_MODE_COLLISION_BOX;
static bool showPlayerOnionEditing = SHOW_PLAYER_ONION_MODE_EDITING;
static bool showPlayerInputBuffer = SHOW_PLAYER_INPUT_BUFFER;
static bool runPlayerCurrentAnimation = false;
static bool runPlayerCurrentAnimationOnce = false;
static int onionOffset = 40;
static bool showHelp = false;
static int saveTimer = 0;
static PlayerState lastEditState = PLAYER_STATE_LAST;
static int lastEditFrame = 0;

// for camera target on playing
static float playerDist = 0.0f;

// flip players logic
static bool player1RightPlayer2 = false;
static bool needsToFlipPlayers = false;

// match data
static int remainingTime = 99;
static float remainingTimeCounter = 0;

/**
 * @brief Creates a dinamically allocated GameWorld struct instance.
 */
GameWorld* createGameWorld( void ) {

    GameWorld *gw = (GameWorld*) malloc( sizeof( GameWorld ) );

    float floorHeight = 30;

    gw->floor = (Rectangle) {
        0, GetScreenHeight() - floorHeight, GetScreenWidth(), floorHeight
    };
    gw->stageTexture = &rm.kenStageTexture;

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

    Player *player1 = createPlayer();
    Player *player2 = createPlayer();

    initializePlayerRyu( gw->stageTexture->width / 2 - 78, 542, player1, PLAYER_START_SIDE_LEFT, DURATION_MODE, SHOW_BOXES, SHOW_DEBUG_INFO );
    initializePlayerKen( gw->stageTexture->width / 2 + 50, 542, player2, PLAYER_START_SIDE_RIGHT, DURATION_MODE, SHOW_BOXES, SHOW_DEBUG_INFO );
    flipPlayerSide( player2 );

    player1->kb = (PlayerKeyBindings) {
        .left  = { KEY_LEFT,  INPUT_TYPE_LEFT },
        .right = { KEY_RIGHT, INPUT_TYPE_RIGHT },
        .up    = { KEY_UP,    INPUT_TYPE_UP },
        .down  = { KEY_DOWN,  INPUT_TYPE_DOWN },
        .lp    = { KEY_KP_4,  INPUT_TYPE_LP },
        .mp    = { KEY_KP_5,  INPUT_TYPE_MP },
        .hp    = { KEY_KP_6,  INPUT_TYPE_HP },
        .lk    = { KEY_KP_1,  INPUT_TYPE_LK },
        .mk    = { KEY_KP_2,  INPUT_TYPE_MK },
        .hk    = { KEY_KP_3,  INPUT_TYPE_HK },
    };

    player2->kb = (PlayerKeyBindings) {
        .left  = { KEY_A, INPUT_TYPE_LEFT },
        .right = { KEY_D, INPUT_TYPE_RIGHT },
        .up    = { KEY_W, INPUT_TYPE_UP },
        .down  = { KEY_S, INPUT_TYPE_DOWN },
        .lp    = { KEY_T, INPUT_TYPE_LP },
        .mp    = { KEY_Y, INPUT_TYPE_MP },
        .hp    = { KEY_U, INPUT_TYPE_HP },
        .lk    = { KEY_G, INPUT_TYPE_LK },
        .mk    = { KEY_H, INPUT_TYPE_MK },
        .hk    = { KEY_J, INPUT_TYPE_HK },
    };

    gw->player1 = player1;
    gw->player2 = player2;

    gw->mode = INITIAL_GAME_MODE;

    loadPlayerAnimationFrameBoxes( gw->player1, PLAYER_1_ANIMATIONS_FILE );
    loadPlayerAnimationFrameBoxes( gw->player2, PLAYER_2_ANIMATIONS_FILE );

    return gw;

}

/**
 * @brief Destroys a GameWindow object and its dependecies.
 */
void destroyGameWorld( GameWorld *gw ) {
    destroyPlayer( gw->player1 );
    destroyPlayer( gw->player2 );
    free( gw );
}

/**
 * @brief Reads user input and updates the state of the game.
 */
void updateGameWorld( GameWorld *gw, float delta ) {

    if ( IsKeyPressed( KEY_F1 ) ) {

        if ( gw->mode == GAME_MODE_PLAYING ) {
            gw->mode = GAME_MODE_EDITING;
        } else {
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

    if ( gw->mode == GAME_MODE_PLAYING ) {
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

    if ( gw->mode == GAME_MODE_PLAYING ) {
        drawGameWorldPlaying( gw );
    } else {
        drawGameWorldEditing( gw );
    }

    EndDrawing();

}

static void drawGameWorldPlaying( GameWorld *gw ) {

    ClearBackground( WHITE );
    
    if ( SHOW_MODEL_STAGE_TEXTURE ) {
        DrawTexture( rm.modelStageTexture, 0, 0, WHITE );
    }

    BeginMode2D( gw->camera );

    if ( !SHOW_MODEL_STAGE_TEXTURE ) {
        DrawTexture( *gw->stageTexture, 0, GetScreenHeight() - gw->stageTexture->height, WHITE );
    }
    drawPlayer( gw->player2 );
    drawPlayer( gw->player1 );

    drawOnHitPlayerAnimation( gw->player1 );
    drawOnHitPlayerAnimation( gw->player2 );

    EndMode2D();

    if ( showPlayerInputBuffer ) {
        drawPlayerInputBuffer( gw->player1 );
        drawPlayerInputBuffer( gw->player2 );
    }

    drawHud( gw );

}

static void updateGameWorldPlaying( GameWorld *gw, float delta ) {

    if ( remainingTime > 0 ) {
        remainingTimeCounter += delta;
        if ( remainingTimeCounter >= 1.0f ) {
            remainingTime--;
            remainingTimeCounter = 0.0f;
        }
    }

    if ( IsKeyPressed( KEY_R ) ) {
        gw->player1->health = 100;
        gw->player2->health = 100;
    }

    if ( IsKeyPressed( KEY_ONE ) ) {
        gw->stageTexture = &rm.kenStageTexture;
    }
    if ( IsKeyPressed( KEY_TWO ) ) {
        gw->stageTexture = &rm.blankaStageTexture;
    }
    if ( IsKeyPressed( KEY_THREE ) ) {
        gw->stageTexture = &rm.chunliStageTexture;
    }
    if ( IsKeyPressed( KEY_FOUR ) ) {
        gw->stageTexture = &rm.eHondaStageTexture;
    }
    if ( IsKeyPressed( KEY_FIVE ) ) {
        gw->stageTexture = &rm.guileStageTexture;
    }

    processInputPlayer( gw->player1, gw->player2, delta );
    processInputPlayer( gw->player2, gw->player1, delta );

    // camera
    playerDist = fabs( gw->player1->pos.x - gw->player2->pos.x ) * gw->camera.zoom;
    float width = ( gw->player1->dim.x + gw->player2->dim.x ) / 2 * gw->camera.zoom;
    bool adjust = false;
    if ( playerDist > GetScreenWidth() - width ) {
        gw->player1->vel.x = 0.0f;
        gw->player2->vel.x = 0.0f;
        adjust = true;
    }

    updatePlayer( gw->player1, gw->player2, gw->gravity, delta );
    updatePlayer( gw->player2, gw->player1, gw->gravity, delta );

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

    flipPlayers( gw );

    updateCameraPlaying( gw );

}

static void drawGameWorldEditing( GameWorld *gw ) {

    ClearBackground( RAYWHITE );

    BeginMode2D( gw->camera );

    Vector2 basePos = GetScreenToWorld2D( (Vector2) { 0, 0 }, gw->camera );

    // grid
    int gridSpacing = (int) ( 2.5f * gw->camera.zoom );
    int gridX = 0;
    int gridY = 0;
    int gridC = 0;

    while ( true ) {

        gridX = (int) ( basePos.x + gridC * gridSpacing );
        gridY = (int) ( basePos.y + gridC * gridSpacing );
        gridC++;

        Color color = gridC % 5 == 0 ? GRAY : LIGHTGRAY;

        DrawLine( gridX, basePos.y, gridX, basePos.y + GetScreenHeight(), color );
        DrawLine( basePos.x, gridY, basePos.x + GetScreenWidth(), gridY, color );

        if ( gridX > GetScreenWidth() ) {
            break;
        }

    }

    if ( showPlayerOnionEditing ) {
        drawPlayerOnionLayers( gw->player1, onionOffset );
    } else {
        drawPlayer( gw->player1 );
    }

    EndMode2D();

    drawInfoPanel( gw );

}

static void updateGameWorldEditing( GameWorld *gw, float delta ) {

    if ( saveTimer > 0 ) saveTimer--;

    if ( IsKeyPressed( KEY_H ) ) {
        showHelp = !showHelp;
    }

    if ( IsKeyDown( KEY_LEFT_CONTROL ) && IsKeyPressed( KEY_S ) ) {
        resetPlayerAnimations( gw->player1 );
        storePlayerAnimations( gw->player1, true, false, PLAYER_1_ANIMATIONS_FILE );
        saveTimer = 90;
        return;
    }

    if ( IsKeyDown( KEY_LEFT_CONTROL ) && IsKeyPressed( KEY_X ) ) {
        if ( IsKeyDown( KEY_LEFT_ALT ) ) {
            copyAllFrameBoxesToPreviousAnimation( gw->player1 );
        } else {
            copyAnimationFrameBoxesPrevious( gw->player1 );
        }
        return;
    }

    if ( IsKeyDown( KEY_LEFT_CONTROL ) && IsKeyPressed( KEY_C ) ) {
        if ( IsKeyDown( KEY_LEFT_ALT ) ) {
            copyAllFrameBoxesToNextAnimation( gw->player1 );
        } else {
            copyAnimationFrameBoxesNext( gw->player1 );
        }
        return;
    }

    if ( IsKeyDown( KEY_O ) ) {
        if ( IsKeyDown( KEY_RIGHT_CONTROL ) ) {
            if ( IsKeyPressed( KEY_LEFT ) ) {
                onionOffset--;
            } else if ( IsKeyPressed( KEY_RIGHT ) ) {
                onionOffset++;
            }
        } else {
            if ( IsKeyDown( KEY_LEFT ) ) {
                onionOffset--;
            } else if ( IsKeyDown( KEY_RIGHT ) ) {
                onionOffset++;
            }
        }
        return;
    }

    if ( IsKeyUp( KEY_M ) && IsKeyUp( KEY_SPACE ) ) {
        if ( IsKeyPressed( KEY_UP ) ) {
            int state = (int) gw->player1->state;
            state--;
            if ( state == -1 ) {
                gw->player1->state = PLAYER_STATE_LAST - 1;
            } else {
                gw->player1->state = state;
            }
            lastEditState = gw->player1->state;
        } else if ( IsKeyPressed( KEY_DOWN ) ) {
            gw->player1->state++;
            if ( gw->player1->state == PLAYER_STATE_LAST ) {
                gw->player1->state = PLAYER_STATE_IDLE;
            }
            lastEditState = gw->player1->state;
        }
    }

    Animation *anim = getPlayerCurrentAnimation( gw->player1 );

    if ( anim != NULL ) {
        if ( IsKeyUp( KEY_M ) && IsKeyUp( KEY_SPACE ) ) {
            if ( IsKeyPressed( KEY_LEFT ) ) {
                anim->currentFrame--;
                if ( anim->currentFrame < 0 ) {
                    anim->currentFrame = anim->frameCount - 1;
                }
                lastEditFrame = anim->currentFrame;
            } else if ( IsKeyPressed( KEY_RIGHT ) ) {
                anim->currentFrame++;
                if ( anim->currentFrame >= anim->frameCount ) {
                    anim->currentFrame = 0;
                }
                lastEditFrame = anim->currentFrame;
            }
        }
    }

    if ( IsKeyDown( KEY_LEFT_CONTROL ) ) {
        if ( IsKeyPressed( KEY_A ) ) {
            gw->player1->pos.x--;
        }
        if ( IsKeyPressed( KEY_D ) ) {
            gw->player1->pos.x++;
        }
        if ( IsKeyPressed( KEY_W ) ) {
            gw->player1->pos.y--;
        }
        if ( IsKeyPressed( KEY_S ) ) {
            gw->player1->pos.y++;
        }
    } else {
        if ( IsKeyDown( KEY_A ) ) {
            gw->player1->pos.x--;
        }
        if ( IsKeyDown( KEY_D ) ) {
            gw->player1->pos.x++;
        }
        if ( IsKeyDown( KEY_W ) ) {
            gw->player1->pos.y--;
        }
        if ( IsKeyDown( KEY_S ) ) {
            gw->player1->pos.y++;
        }
    }

    AnimationFrame *af = getPlayerCurrentAnimationFrame( gw->player1 );

    if ( IsKeyPressed( KEY_ONE ) ) {
        editorMode = EDITOR_MODE_COLLISION_BOX;
    } else if ( IsKeyPressed( KEY_TWO ) && af->boxes.hitboxCount >= 1 ) {
        editorMode = EDITOR_MODE_HIT_BOX_0;
    } else if ( IsKeyPressed( KEY_THREE ) && af->boxes.hitboxCount >= 2 ) {
        editorMode = EDITOR_MODE_HIT_BOX_1;
    } else if ( IsKeyPressed( KEY_FOUR ) && af->boxes.hitboxCount >= 3 ) {
        editorMode = EDITOR_MODE_HIT_BOX_2;
    } else if ( IsKeyPressed( KEY_FIVE ) && af->boxes.hurtboxCount >= 1 ) {
        editorMode = EDITOR_MODE_HURT_BOX_0;
    } else if ( IsKeyPressed( KEY_SIX ) && af->boxes.hurtboxCount >= 2 ) {
        editorMode = EDITOR_MODE_HURT_BOX_1;
    } else if ( IsKeyPressed( KEY_SEVEN ) && af->boxes.hurtboxCount >= 3 ) {
        editorMode = EDITOR_MODE_HURT_BOX_2;
    }

    switch ( editorMode ) {
        case EDITOR_MODE_COLLISION_BOX: editAnimationFrameBox( &af->boxes.collisionBox ); break;
        case EDITOR_MODE_HIT_BOX_0:     editAnimationFrameBox( &af->boxes.hitboxes[0] );  break;
        case EDITOR_MODE_HIT_BOX_1:     editAnimationFrameBox( &af->boxes.hitboxes[1] );  break;
        case EDITOR_MODE_HIT_BOX_2:     editAnimationFrameBox( &af->boxes.hitboxes[2] );  break;
        case EDITOR_MODE_HURT_BOX_0:    editAnimationFrameBox( &af->boxes.hurtboxes[0] ); break;
        case EDITOR_MODE_HURT_BOX_1:    editAnimationFrameBox( &af->boxes.hurtboxes[1] ); break;
        case EDITOR_MODE_HURT_BOX_2:    editAnimationFrameBox( &af->boxes.hurtboxes[2] ); break;
        default: break;
    }

    if ( IsKeyDown( KEY_RIGHT_CONTROL ) ) {
        if ( IsKeyPressed( KEY_KP_ADD ) ) {
            af->duration++;
        } else if ( IsKeyPressed( KEY_KP_SUBTRACT ) ) {
            af->duration--;
            if ( af->duration < 0 ) {
                af->duration = 0;
            }
        }
    } else {
        if ( IsKeyDown( KEY_KP_ADD ) ) {
            af->duration++;
        } else if ( IsKeyDown( KEY_KP_SUBTRACT ) ) {
            af->duration--;
            if ( af->duration < 0 ) {
                af->duration = 0;
            }
        }
    }

    if ( IsKeyDown( KEY_RIGHT_CONTROL ) ) {
        if ( IsKeyPressed( KEY_PAGE_UP ) ) {
            af->damageOnHurt++;
        } else if ( IsKeyPressed( KEY_PAGE_DOWN ) ) {
            af->damageOnHurt--;
            if ( af->damageOnHurt < 0 ) {
                af->damageOnHurt = 0;
            }
        }
    } else {
        if ( IsKeyDown( KEY_PAGE_UP ) ) {
            af->damageOnHurt++;
        } else if ( IsKeyDown( KEY_PAGE_DOWN ) ) {
            af->damageOnHurt--;
            if ( af->damageOnHurt < 0 ) {
                af->damageOnHurt = 0;
            }
        }
    }

    if ( !runPlayerCurrentAnimationOnce ) {
        if ( IsKeyPressed( KEY_ENTER ) ) {
            runPlayerCurrentAnimation = !runPlayerCurrentAnimation;
        }
    }

    if ( runPlayerCurrentAnimation ) {
        updateAnimation( anim, DURATION_MODE, delta );
        if ( ( anim->runOnce || anim->stopAtLastFrame ) && anim->finished ) {
            resetAnimation( anim );
        }
    }

    if ( !runPlayerCurrentAnimation ) {
        if ( IsKeyPressed( KEY_KP_ENTER ) ) {
            runPlayerCurrentAnimationOnce = true;
            resetAnimation( anim );
        }
    }

    if ( runPlayerCurrentAnimationOnce ) {
        updateAnimation( anim, DURATION_MODE, delta );
        if ( anim->finished ) {
            runPlayerCurrentAnimationOnce = false;
        }
    }

    updateCameraEditing( gw );

}

static void editAnimationFrameBox( Rectangle *box ) {

    if ( IsKeyDown( KEY_M ) ) {
        if ( IsKeyDown( KEY_RIGHT_CONTROL ) ) {
            if ( IsKeyPressed( KEY_LEFT ) ) {
                box->x--;
            }
            if ( IsKeyPressed( KEY_RIGHT ) ) {
                box->x++;
            }
            if ( IsKeyPressed( KEY_UP ) ) {
                box->y--;
            }
            if ( IsKeyPressed( KEY_DOWN ) ) {
                box->y++;
            }
        } else {
            if ( IsKeyDown( KEY_LEFT ) ) {
                box->x--;
            }
            if ( IsKeyDown( KEY_RIGHT ) ) {
                box->x++;
            }
            if ( IsKeyDown( KEY_UP ) ) {
                box->y--;
            }
            if ( IsKeyDown( KEY_DOWN ) ) {
                box->y++;
            }
        }
    } else if ( IsKeyDown( KEY_SPACE ) ) {
        if ( IsKeyDown( KEY_RIGHT_CONTROL ) ) {
            if ( IsKeyPressed( KEY_LEFT ) ) {
                box->width--;
                if ( box->width < 0 ) {
                    box->width = 0;
                }
            }
            if ( IsKeyPressed( KEY_RIGHT ) ) {
                box->width++;
            }
            if ( IsKeyPressed( KEY_UP ) ) {
                box->height--;
                if ( box->height < 0 ) {
                    box->height = 0;
                }
            }
            if ( IsKeyPressed( KEY_DOWN ) ) {
                box->height++;
            }
        } else {
            if ( IsKeyDown( KEY_LEFT ) ) {
                box->width--;
                if ( box->width < 0 ) {
                    box->width = 0;
                }
            }
            if ( IsKeyDown( KEY_RIGHT ) ) {
                box->width++;
            }
            if ( IsKeyDown( KEY_UP ) ) {
                box->height--;
                if ( box->height < 0 ) {
                    box->height = 0;
                }
            }
            if ( IsKeyDown( KEY_DOWN ) ) {
                box->height++;
            }
        }
    }

    if ( IsKeyPressed( KEY_R ) ) {
        box->x = 0;
        box->y = 0;
        box->width = 0;
        box->height = 0;
    }

}

static void showAnimationFrameBoxDetail( Player *p, Rectangle *box, Camera2D camera, Color color ) {

    color = ColorBrightness( color, -0.5f );

    int x = (int) box->x;
    int y = (int) box->y;
    int w = (int) box->width;
    int h = (int) box->height;

    Vector2 pos = GetWorldToScreen2D( p->pos, camera );
    Rectangle box2 = { 
        box->x * camera.zoom, box->y * camera.zoom, 
        box->width * camera.zoom, box->height * camera.zoom
    };

    if ( !( w == 0 && h == 0 ) ) {
        DrawText( "offsets", pos.x + box2.x, pos.y + box2.y - 100, 20, color );
        DrawText( TextFormat( "x: %d", x ), pos.x + box2.x, pos.y + box2.y - 80, 20, color );
        DrawText( TextFormat( "y: %d", y ), pos.x + box2.x, pos.y + box2.y - 60, 20, color );
        DrawText( TextFormat( "w: %d", w ), pos.x + box2.x, pos.y + box2.y - 40, 20, color );
        DrawText( TextFormat( "h: %d", h ), pos.x + box2.x, pos.y + box2.y - 20, 20, color );
    } else {
        DrawText( "disabled", pos.x + box2.x, pos.y + box2.y - 20, 20, Fade( color, 0.5f ) );
    }

}

static void drawInfoPanel( GameWorld *gw ) {

    DrawRectangle( 0, 0, GetScreenWidth(), 175, Fade( BLACK, 0.25f ) );

    DrawText( "Animation Editor", 5, 5, 20, BLACK );

    const char *playStatus;
    Color playColor;
    if ( runPlayerCurrentAnimation ) {
        playStatus = "PLAYING";
        playColor = DARKGREEN;
    } else if ( runPlayerCurrentAnimationOnce ) {
        playStatus = "PLAYING ONCE";
        playColor = DARKGREEN;
    } else {
        playStatus = "STOPPED";
        playColor = DARKGRAY;
    }
    int playW = MeasureText( playStatus, 20 );
    DrawText( playStatus, GetScreenWidth() - playW - 8, 5, 20, playColor );

    DrawText( TextFormat( "State: %s", utilsPlayerStateToText( gw->player1->state ) ), 5, 30, 20, BLACK );

    Animation *anim = getPlayerCurrentAnimation( gw->player1 );
    AnimationFrame *af = getPlayerCurrentAnimationFrame( gw->player1 );

    if ( anim != NULL ) {
        DrawText( TextFormat( "Frame: %d / %d", anim->currentFrame, anim->frameCount - 1 ), 5, 55, 20, BLACK );
    }
    if ( af != NULL ) {
        DrawText( TextFormat( "Duration: %d", af->duration ), 210, 55, 20, BLACK );
        DrawText( TextFormat( "Damage on Hurt: %d", af->damageOnHurt ), 380, 55, 20, BLACK );
    }
    if ( showPlayerOnionEditing ) {
        DrawText( TextFormat( "Onion: %d", onionOffset ), 620, 55, 20, DARKBLUE );
    }

    int slotW = 80;
    int slotH = 30;
    int slotY = 82;
    int slotPad = 4;

    for ( int i = 0; i < 7; i++ ) {

        const char *label;
        EditorMode mode;
        Color color;
        bool available;

        switch ( i ) {
            case 0: label = "1 COL";  mode = EDITOR_MODE_COLLISION_BOX; color = GREEN; available = true; break;
            case 1: label = "2 HIT0"; mode = EDITOR_MODE_HIT_BOX_0;     color = BLUE;  available = af != NULL && af->boxes.hitboxCount >= 1;  break;
            case 2: label = "3 HIT1"; mode = EDITOR_MODE_HIT_BOX_1;     color = BLUE;  available = af != NULL && af->boxes.hitboxCount >= 2;  break;
            case 3: label = "4 HIT2"; mode = EDITOR_MODE_HIT_BOX_2;     color = BLUE;  available = af != NULL && af->boxes.hitboxCount >= 3;  break;
            case 4: label = "5 HUR0"; mode = EDITOR_MODE_HURT_BOX_0;    color = RED;   available = af != NULL && af->boxes.hurtboxCount >= 1; break;
            case 5: label = "6 HUR1"; mode = EDITOR_MODE_HURT_BOX_1;    color = RED;   available = af != NULL && af->boxes.hurtboxCount >= 2; break;
            case 6: label = "7 HUR2"; mode = EDITOR_MODE_HURT_BOX_2;    color = RED;   available = af != NULL && af->boxes.hurtboxCount >= 3; break;
            default: continue;
        }

        bool selected = editorMode == mode;
        Color bg      = available ? Fade( color, selected ? 0.70f : 0.30f ) : Fade( GRAY, 0.20f );
        Color border  = available ? ( selected ? ColorBrightness( color, -0.5f ) : Fade( color, 0.5f ) ) : Fade( DARKGRAY, 0.5f );
        Color textCol = selected ? ColorBrightness( color, -0.7f ) : DARKGRAY;

        int sx = 5 + i * ( slotW + slotPad );
        DrawRectangle( sx, slotY, slotW, slotH, bg );
        DrawRectangleLines( sx, slotY, slotW, slotH, border );
        int tw = MeasureText( label, 20 );
        DrawText( label, sx + ( slotW - tw ) / 2, slotY + 5, 20, textCol );

    }

    if ( af != NULL ) {

        Rectangle *box = NULL;
        Color boxColor = WHITE;
        switch ( editorMode ) {
            case EDITOR_MODE_COLLISION_BOX: box = &af->boxes.collisionBox; boxColor = GREEN; break;
            case EDITOR_MODE_HIT_BOX_0:     box = &af->boxes.hitboxes[0];  boxColor = BLUE;  break;
            case EDITOR_MODE_HIT_BOX_1:     box = &af->boxes.hitboxes[1];  boxColor = BLUE;  break;
            case EDITOR_MODE_HIT_BOX_2:     box = &af->boxes.hitboxes[2];  boxColor = BLUE;  break;
            case EDITOR_MODE_HURT_BOX_0:    box = &af->boxes.hurtboxes[0]; boxColor = RED;   break;
            case EDITOR_MODE_HURT_BOX_1:    box = &af->boxes.hurtboxes[1]; boxColor = RED;   break;
            case EDITOR_MODE_HURT_BOX_2:    box = &af->boxes.hurtboxes[2]; boxColor = RED;   break;
            default: break;
        }

        Color dc = ColorBrightness( boxColor, -0.5f );
        if ( box != NULL ) {
            if ( box->width == 0 && box->height == 0 ) {
                DrawText( "disabled", 5, 118, 20, Fade( dc, 0.5f ) );
            } else {
                DrawText( TextFormat( "x: %-5d  y: %-5d  w: %-5d  h: %-5d",
                    (int) box->x, (int) box->y, (int) box->width, (int) box->height ),
                    5, 118, 20, dc );
            }
            showAnimationFrameBoxDetail( gw->player1, box, gw->camera, boxColor );
        }

    }

    const char *hint;
    Color hintColor;
    if ( IsKeyDown( KEY_M ) ) {
        hint = "M + arrows: move box (x/y)  |  + RIGHT CTRL: 1px step";
        hintColor = BLACK;
    } else if ( IsKeyDown( KEY_SPACE ) ) {
        hint = "SPACE + arrows: resize box (w/h)  |  + RIGHT CTRL: 1px step";
        hintColor = BLACK;
    } else if ( IsKeyDown( KEY_O ) ) {
        hint = "O + arrows: adjust onion offset  |  + RIGHT CTRL: step 1";
        hintColor = BLACK;
    } else if ( IsKeyDown( KEY_LEFT_CONTROL ) ) {
        hint = "CTRL+S: save  |  CTRL+X/C: copy boxes  |  CTRL+ALT+X/C: copy animation";
        hintColor = BLACK;
    } else {
        hint = "arrows: navigate  |  1-7: select box  |  M/SPACE+arrows: move/resize  |  H: help";
        hintColor = BLACK;
    }
    DrawText( hint, 5, 148, 20, hintColor );

    if ( saveTimer > 0 ) {
        const char *savedText = "SAVED!";
        int sw = MeasureText( savedText, 20 );
        float alpha = saveTimer > 30 ? 1.0f : (float) saveTimer / 30.0f;
        DrawText( savedText, GetScreenWidth() - sw - 10, 30, 20, Fade( DARKGREEN, alpha ) );
    }

    if ( showHelp ) {
        drawEditorHelp();
    }

}

static void drawEditorHelp( void ) {

    DrawRectangle( 0, 0, GetScreenWidth(), GetScreenHeight(), Fade( BLACK, 0.88f ) );

    int fs = 10;
    int lh = 14;
    int col1 = 30;
    int col2 = GetScreenWidth() / 2 - 100;
    int desc1 = col1 + 150;   // description column for left side
    int desc2 = col2 + 150;   // description column for right side
    int yStart = 20;

    DrawText( "HELP  -  ANIMATION EDITOR        ( H to close )", col1, yStart, 20, YELLOW );

    int yL = yStart + 35;
    int yR = yStart + 35;

    // left column
    DrawText( "NAVIGATION",   col1, yL, fs, LIGHTGRAY ); yL += lh;
    DrawText( "UP / DOWN",    col1, yL, fs, WHITE ); DrawText( "change state", desc1, yL, fs, WHITE ); yL += lh;
    DrawText( "LEFT / RIGHT", col1, yL, fs, WHITE ); DrawText( "change frame", desc1, yL, fs, WHITE ); yL += lh;
    yL += lh / 2;

    DrawText( "BOX SELECTION", col1, yL, fs, LIGHTGRAY ); yL += lh;
    DrawText( "1",             col1, yL, fs, GREEN ); DrawText( "collision box",     desc1, yL, fs, GREEN ); yL += lh;
    DrawText( "2 / 3 / 4",     col1, yL, fs, BLUE  ); DrawText( "hitbox 0 / 1 / 2",  desc1, yL, fs, BLUE  ); yL += lh;
    DrawText( "5 / 6 / 7",     col1, yL, fs, RED   ); DrawText( "hurtbox 0 / 1 / 2", desc1, yL, fs, RED   ); yL += lh;
    yL += lh / 2;

    DrawText( "HURTBOXES DAMAGE (PER FRAME)", col1, yL, fs, LIGHTGRAY ); yL += lh;
    DrawText( "PAGE UP / PAGE DOWN",      col1, yL, fs, WHITE ); DrawText( "adjust damage", desc1, yL, fs, WHITE ); yL += lh;
    DrawText( "+ RIGHT CTRL",   col1, yL, fs, WHITE ); DrawText( "step 1",          desc1, yL, fs, WHITE ); yL += lh;
    yL += lh / 2;

    DrawText( "BOX EDITING",    col1, yL, fs, LIGHTGRAY ); yL += lh;
    DrawText( "M + arrows",     col1, yL, fs, WHITE ); DrawText( "move box (x/y)", desc1, yL, fs, WHITE ); yL += lh;
    DrawText( "SPACE + arrows", col1, yL, fs, WHITE ); DrawText( "resize (w/h)",   desc1, yL, fs, WHITE ); yL += lh;
    DrawText( "+ RIGHT CTRL",   col1, yL, fs, WHITE ); DrawText( "1 pixel step",   desc1, yL, fs, WHITE ); yL += lh;
    DrawText( "R",              col1, yL, fs, WHITE ); DrawText( "reset box",      desc1, yL, fs, WHITE ); yL += lh;
    yL += lh / 2;

    DrawText( "FRAME DURATION", col1, yL, fs, LIGHTGRAY ); yL += lh;
    DrawText( "KP+ / KP-",      col1, yL, fs, WHITE ); DrawText( "adjust duration", desc1, yL, fs, WHITE ); yL += lh;
    DrawText( "+ RIGHT CTRL",   col1, yL, fs, WHITE ); DrawText( "step 1",          desc1, yL, fs, WHITE ); yL += lh;
    yL += lh / 2;

    DrawText( "CHARACTER POSITION", col1, yL, fs, LIGHTGRAY ); yL += lh;
    DrawText( "A / D / W / S",      col1, yL, fs, WHITE ); DrawText( "move (continuous)", desc1, yL, fs, WHITE ); yL += lh;
    DrawText( "CTRL + A/D/W/S",     col1, yL, fs, WHITE ); DrawText( "move 1 pixel",      desc1, yL, fs, WHITE ); yL += lh;

    // right column
    DrawText( "ANIMATION", col2, yR, fs, LIGHTGRAY ); yR += lh;
    DrawText( "ENTER",     col2, yR, fs, WHITE ); DrawText( "start/pause loop", desc2, yR, fs, WHITE ); yR += lh;
    DrawText( "KP ENTER",  col2, yR, fs, WHITE ); DrawText( "play once",        desc2, yR, fs, WHITE ); yR += lh;
    yR += lh / 2;

    DrawText( "ONION SKIN",         col2, yR, fs, LIGHTGRAY ); yR += lh;
    DrawText( "F4",                 col2, yR, fs, WHITE ); DrawText( "toggle on/off", desc2, yR, fs, WHITE ); yR += lh;
    DrawText( "O + arrows",         col2, yR, fs, WHITE ); DrawText( "adjust offset", desc2, yR, fs, WHITE ); yR += lh;
    DrawText( "O + RCTRL + arrows", col2, yR, fs, WHITE ); DrawText( "step 1",      desc2, yR, fs, WHITE ); yR += lh;
    yR += lh / 2;

    DrawText( "COPY BOXES",   col2, yR, fs, LIGHTGRAY ); yR += lh;
    DrawText( "CTRL + X",     col2, yR, fs, WHITE ); DrawText( "copy -> previous frame", desc2, yR, fs, WHITE ); yR += lh;
    DrawText( "CTRL + C",     col2, yR, fs, WHITE ); DrawText( "copy -> next frame",     desc2, yR, fs, WHITE ); yR += lh;
    DrawText( "CTRL+ALT + X", col2, yR, fs, WHITE ); DrawText( "copy all -> prev anim.", desc2, yR, fs, WHITE ); yR += lh;
    DrawText( "CTRL+ALT + C", col2, yR, fs, WHITE ); DrawText( "copy all -> next anim.", desc2, yR, fs, WHITE ); yR += lh;
    yR += lh / 2;

    DrawText( "FILE / GENERAL", col2, yR, fs, LIGHTGRAY ); yR += lh;
    DrawText( "CTRL + S",       col2, yR, fs, WHITE ); DrawText( "save",                  desc2, yR, fs, WHITE ); yR += lh;
    DrawText( "F1",             col2, yR, fs, WHITE ); DrawText( "toggle play/edit mode", desc2, yR, fs, WHITE ); yR += lh;
    DrawText( "F2",             col2, yR, fs, WHITE ); DrawText( "show/hide boxes",       desc2, yR, fs, WHITE ); yR += lh;
    DrawText( "F3",             col2, yR, fs, WHITE ); DrawText( "show/hide debug",       desc2, yR, fs, WHITE ); yR += lh;
    DrawText( "H",              col2, yR, fs, WHITE ); DrawText( "show/hide help",        desc2, yR, fs, WHITE ); yR += lh;

}

static void drawHud( GameWorld *gw ) {

    int barWidth = 335;
    int healthWidthP1 = (int) ( barWidth * ( gw->player1->health / 100.0f ) );
    int healthWidthP2 = (int) ( barWidth * ( gw->player2->health / 100.0f ) );

    // player 1
    DrawRectangleRec(
        (Rectangle) { 75, 65, barWidth, 25 },
        Fade( RED, 0.7f )
    );

    DrawRectangleRec(
        (Rectangle) { 75 + barWidth - healthWidthP1, 65, healthWidthP1, 25 },
        YELLOW
    );

    DrawRectangleRoundedLinesEx(
        (Rectangle) { 77, 67, barWidth, 25 },
        0.5f,
        10,
        4,
        BLACK
    );

    DrawRectangleRoundedLinesEx(
        (Rectangle) { 75, 65, barWidth, 25 },
        0.5f,
        10,
        4,
        WHITE
    );

    drawTextUsingFont( TextFormat( "%s", gw->player1->name ), 75, 98, 3 );

    // player 2
    DrawRectangleRec(
        (Rectangle) { 485, 65, barWidth, 25 },
        Fade( RED, 0.7f )
    );

    DrawRectangleRec(
        (Rectangle) { 485, 65, healthWidthP2, 25 },
        YELLOW
    );

    DrawRectangleRoundedLinesEx(
        (Rectangle) { 487, 67, barWidth, 25 },
        0.5f,
        10,
        4,
        BLACK
    );

    DrawRectangleRoundedLinesEx(
        (Rectangle) { 485, 65, barWidth, 25 },
        0.5f,
        10,
        4,
        WHITE
    );

    const char *p2Name = TextFormat( "%s", gw->player2->name );
    Vector2 measureP2Name = measureTextUsingFont( p2Name, 3 );
    drawTextUsingFont( p2Name, 823 - measureP2Name.x, 98, 3 );

    // ko
    DrawRectangleRounded(
        (Rectangle) { 417, 60, 61, 35 },
        0.5f,
        10,
        BLACK
    );

    DrawRectangleRoundedLinesEx(
        (Rectangle) { 419, 62, 61, 35 },
        0.5f,
        10,
        4,
        BLACK
    );

    DrawRectangleRoundedLinesEx(
        (Rectangle) { 417, 60, 61, 35 },
        0.5f,
        10,
        4,
        WHITE
    );

    DrawText( "K.O", 421, 61, 36, RED );
    
    const char *remainingTimeStr = TextFormat( "%02d", remainingTime );
    Vector2 measureRemainingTime = measureTextUsingFont( remainingTimeStr, 4 );
    drawTextUsingFont( remainingTimeStr, GetScreenWidth() / 2 - measureRemainingTime.x / 2 + 5, 103, 4 );

}

static void copyAnimationFrameBoxesPrevious( Player *p ) {
        
    Animation *a = getPlayerCurrentAnimation( p );
    AnimationFrame *sourceAf = getPlayerCurrentAnimationFrame( p );

    for ( int i = 0; i < a->frameCount; i++ ) {
        if ( &a->frames[i] == sourceAf ) {
            int prev = i - 1;
            if ( prev == -1 ) {
                prev = a->frameCount - 1;
            }
            AnimationFrame *destAf = &a->frames[prev];
            destAf->boxes = sourceAf->boxes;
            break;
        }
    }

}

static void copyAnimationFrameBoxesNext( Player *p ) {
        
    Animation *a = getPlayerCurrentAnimation( p );
    AnimationFrame *sourceAf = getPlayerCurrentAnimationFrame( p );

    for ( int i = 0; i < a->frameCount; i++ ) {
        if ( &a->frames[i] == sourceAf ) {
            int next = ( i + 1 ) % a->frameCount;
            AnimationFrame *destAf = &a->frames[next];
            destAf->boxes = sourceAf->boxes;
            break;
        }
    }

}

static void copyAllFrameBoxesToPreviousAnimation( Player *p ) {

    Animation *sourceA = getPlayerCurrentAnimation( p );
    Animation *destA = NULL;

    for ( int i = 0; i < p->animationCount; i++ ) {
        if ( p->animations[i] == sourceA ) {
            int prev = i - 1;
            if ( prev == -1 ) {
                prev = p->animationCount - 1;
            }
            destA = p->animations[prev];
            break;
        }
    }

    if ( destA != NULL ) {

        int quant = 0;

        if ( sourceA->frameCount <= destA->frameCount ) {
            quant = sourceA->frameCount;
        } else {
            quant = destA->frameCount;
        }

        for ( int i = 0; i < quant; i++ ) {
            AnimationFrame *sourceAf = &sourceA->frames[i];
            AnimationFrame *destAf = &destA->frames[i];
            destAf->boxes = sourceAf->boxes;
        }

    }

}

static void copyAllFrameBoxesToNextAnimation( Player *p ) {
        
    Animation *sourceA = getPlayerCurrentAnimation( p );
    Animation *destA = NULL;

    for ( int i = 0; i < p->animationCount; i++ ) {
        if ( p->animations[i] == sourceA ) {
            int next = ( i + 1 ) % p->animationCount;
            destA = p->animations[next];
            break;
        }
    }

    if ( destA != NULL ) {

        int quant = 0;

        if ( sourceA->frameCount <= destA->frameCount ) {
            quant = sourceA->frameCount;
        } else {
            quant = destA->frameCount;
        }

        for ( int i = 0; i < quant; i++ ) {
            AnimationFrame *sourceAf = &sourceA->frames[i];
            AnimationFrame *destAf = &destA->frames[i];
            destAf->boxes = sourceAf->boxes;
        }

    }

}

static void updateCameraPlaying( GameWorld *gw ) {

    gw->camera.target.x = fabs( ( gw->player1->pos.x + gw->player2->pos.x ) / 2 );

    float worldWidth = gw->stageTexture->width;
    float zoom = gw->camera.zoom;
    float offsetX = gw->camera.offset.x;
    float screenWidth = GetScreenWidth();

    float minTargetX = offsetX / zoom;
    float maxTargetX = worldWidth - ( screenWidth - offsetX ) / zoom;

    if ( gw->camera.target.x < minTargetX ) {
        gw->camera.target.x = minTargetX;
    } else if ( gw->camera.target.x > maxTargetX ) {
        gw->camera.target.x = maxTargetX;
    }

}

static void updateCameraEditing( GameWorld *gw ) {

    gw->camera.target.x = fabs( ( gw->player1->pos.x + gw->player2->pos.x ) / 2 );
    //gw->camera.target.x = GetScreenWidth() / 2 - 100;

    float worldWidth = gw->stageTexture->width;
    float zoom = gw->camera.zoom;
    float offsetX = gw->camera.offset.x;
    float screenWidth = GetScreenWidth();

    float minTargetX = offsetX / zoom;
    float maxTargetX = worldWidth - ( screenWidth - offsetX ) / zoom;

    if ( gw->camera.target.x < minTargetX ) {
        gw->camera.target.x = minTargetX;
    } else if ( gw->camera.target.x > maxTargetX ) {
        gw->camera.target.x = maxTargetX;
    }

}

static void resolveCollisionPlayerStage( Player *player, GameWorld *gw ) {

    if ( player->pos.x - player->dim.x / 2 < 0 ) {
        player->pos.x = player->dim.x / 2;
    } else if ( player->pos.x + player->dim.x / 2 > gw->stageTexture->width ) {
        player->pos.x = gw->stageTexture->width - player->dim.x / 2;
    }

    if ( player->pos.y + player->dim.y > gw->floor.y ) {
        player->pos.y = gw->floor.y - player->dim.y;
        player->vel.y = 0.0f;
        if ( isAirborneState( player->state ) ) {
            resetPlayerAnimations( player );
            player->vel.x = 0.0f;
            player->state = PLAYER_STATE_JUMP_COOLDOWN;
        }
    }

}

static void resolvePlayerPlayerCollision( Player *p1, Player *p2 ) {

    AnimationFrame *af1 = getPlayerCurrentAnimationFrame( p1 );
    AnimationFrame *af2 = getPlayerCurrentAnimationFrame( p2 );

    if ( af1 == NULL || af2 == NULL ) {
        return;
    }

    Rectangle cb1 = af1->boxes.collisionBox;
    Rectangle cb2 = af2->boxes.collisionBox;

    // skip if either collision box is empty (not yet defined)
    if ( ( cb1.width == 0 && cb1.height == 0 ) || ( cb2.width == 0 && cb2.height == 0 ) ) {
        return;
    }

    // convert to world coordinates (same facing-aware logic as hitbox/hurtbox)
    Rectangle wcb1 = {
        p1->lookingRight ? p1->pos.x + cb1.x : p1->pos.x - cb1.x - cb1.width,
        p1->pos.y + cb1.y,
        cb1.width,
        cb1.height
    };

    Rectangle wcb2 = {
        p2->lookingRight ? p2->pos.x + cb2.x : p2->pos.x - cb2.x - cb2.width,
        p2->pos.y + cb2.y,
        cb2.width,
        cb2.height
    };

    if ( CheckCollisionRecs( wcb1, wcb2 ) ) {
        Rectangle overlap = GetCollisionRec( wcb1, wcb2 );
        float pushDist = overlap.width / 2;

        if ( p1->pos.x < p2->pos.x ) {
            p1->pos.x -= pushDist;
            p2->pos.x += pushDist;
        } else {
            p1->pos.x += pushDist;
            p2->pos.x -= pushDist;
        }
    }

}

static void flipPlayers( GameWorld *gw ) {

    if ( gw->player1->pos.x > gw->player2->pos.x ) {
        if ( !player1RightPlayer2 ) {
            needsToFlipPlayers = true;
        }
        player1RightPlayer2 = true;
    } else {
        if ( player1RightPlayer2 ) {
            needsToFlipPlayers = true;
        }
        player1RightPlayer2 = false;
    }

    if ( needsToFlipPlayers ) {
        flipPlayerSide( gw->player1 );
        flipPlayerSide( gw->player2 );
        needsToFlipPlayers = false;
    }

}
