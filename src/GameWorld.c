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

#include "GameWorld.h"
#include "Macros.h"
#include "ResourceManager.h"
#include "Types.h"
#include "Player.h"
#include "Utils.h"

#define SHOW_BOXES true
#define SHOW_DEBUG_INFO false
#define INITIAL_GAME_MODE GAME_MODE_EDITING

#define PLAYER_1_FILE "resources/animations/ryu.json"
#define PLAYER_2_FILE "resources/animations/ken.json"

static void drawGameWorldPlaying( GameWorld *gw );
static void drawGameWorldEditing( GameWorld *gw );
static void updateGameWorldPlaying( GameWorld *gw, float delta );
static void updateGameWorldEditing( GameWorld *gw, float delta );
static void editAnimationFrameBox( Rectangle *box );
static void showAnimationFrameBoxDetail( Player *p, Rectangle *box, Color color );

static void updateCameraPlaying( GameWorld *gw );
static void updateCameraEditing( GameWorld *gw );

static void resolveCollisionPlayerStage( Player *player, GameWorld *gw );
static void flipPlayers( GameWorld *gw );

// editor
static EditorMode editorMode = EDITOR_MODE_COLLISION_BOX;

// for camera target on playing
static float playerDist = 0.0f;

// flip players logic
static bool player1RightPlayer2 = false;
static bool needsToFlipPlayers = false;

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

    initializePlayerRyu( gw->stageTexture->width / 2 - 78, 542, player1, SHOW_BOXES, SHOW_DEBUG_INFO );
    initializePlayerKen( gw->stageTexture->width / 2 + 50, 542, player2, SHOW_BOXES, SHOW_DEBUG_INFO );
    flipPlayerSide( player2 );

    player1->kb = (PlayerKeyBindings) {
        .left = KEY_LEFT,
        .right = KEY_RIGHT,
        .up = KEY_UP,
        .down = KEY_DOWN,
        .lp = KEY_KP_4,
        .mp = KEY_KP_5,
        .hp = KEY_KP_6,
        .lk = KEY_KP_1,
        .mk = KEY_KP_2,
        .hk = KEY_KP_3,
    };

    player2->kb = (PlayerKeyBindings) {
        .left = KEY_A,
        .right = KEY_D,
        .up = KEY_W,
        .down = KEY_S,
        .lp = KEY_T,
        .mp = KEY_Y,
        .hp = KEY_U,
        .lk = KEY_G,
        .mk = KEY_H,
        .hk = KEY_J,
    };

    gw->player1 = player1;
    gw->player2 = player2;

    gw->mode = INITIAL_GAME_MODE;

    //storePlayerAnimations( gw->player1, true, false, PLAYER_1_FILE );
    //storePlayerAnimations( gw->player2, true, false, PLAYER_2_FILE );

    loadPlayerAnimationFrameBoxes( gw->player1, PLAYER_1_FILE );
    loadPlayerAnimationFrameBoxes( gw->player2, PLAYER_2_FILE );

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
        }
        gw->player1->state = PLAYER_STATE_IDLE;
        resetPlayerAnimations( gw->player1 );
    }

    if ( IsKeyPressed( KEY_F2 ) ) {
        gw->player1->showBoxes = !gw->player1->showBoxes;
        gw->player2->showBoxes = !gw->player2->showBoxes;
    }

    if ( IsKeyPressed( KEY_F3 ) ) {
        gw->player1->showDebugInfo = !gw->player1->showDebugInfo;
        gw->player2->showDebugInfo = !gw->player2->showDebugInfo;
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
    
    BeginMode2D( gw->camera );

    DrawTexture( *gw->stageTexture, 0, GetScreenHeight() - gw->stageTexture->height, WHITE );
    drawPlayer( gw->player2, &gw->camera );
    drawPlayer( gw->player1, &gw->camera );

    EndMode2D();

}

static void updateGameWorldPlaying( GameWorld *gw, float delta ) {

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

    // TODO: improve
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

    // TODO: improve
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

    drawPlayer( gw->player1, &gw->camera );

    DrawText( "Animation Frame Box Editor", basePos.x + 5, basePos.y + 5, 10, BLACK );
    DrawText( TextFormat( "Mode: %s", utilsEditorModeToText( editorMode ) ), basePos.x + 5, basePos.y + 20, 10, BLACK );
    DrawText( TextFormat( "State: %s", utilsPlayerStateToText( gw->player1->state ) ), basePos.x + 5, basePos.y + 30, 10, BLACK );
    DrawText( TextFormat( "Frame: %d", getPlayerCurrentAnimation( gw->player1 )->currentFrame ), basePos.x + 5, basePos.y + 40, 10, BLACK );

    AnimationFrame *af = getPlayerCurrentAnimationFrame( gw->player1 );

    switch ( editorMode ) {
        case EDITOR_MODE_COLLISION_BOX: showAnimationFrameBoxDetail( gw->player1, &af->boxes.collisionBox, GREEN ); break;
        case EDITOR_MODE_HIT_BOX_0: showAnimationFrameBoxDetail( gw->player1, &af->boxes.hitboxes[0], BLUE ); break;
        case EDITOR_MODE_HIT_BOX_1: showAnimationFrameBoxDetail( gw->player1, &af->boxes.hitboxes[1], BLUE ); break;
        case EDITOR_MODE_HIT_BOX_2: showAnimationFrameBoxDetail( gw->player1, &af->boxes.hitboxes[2], BLUE ); break;
        case EDITOR_MODE_HURT_BOX_0: showAnimationFrameBoxDetail( gw->player1, &af->boxes.hurtboxes[0], RED ); break;
        case EDITOR_MODE_HURT_BOX_1: showAnimationFrameBoxDetail( gw->player1, &af->boxes.hurtboxes[1], RED ); break;
        case EDITOR_MODE_HURT_BOX_2: showAnimationFrameBoxDetail( gw->player1, &af->boxes.hurtboxes[2], RED ); break;
        default: break;
    }

    EndMode2D();

}

static void updateGameWorldEditing( GameWorld *gw, float delta ) {

    if ( IsKeyUp( KEY_M ) && IsKeyUp( KEY_SPACE ) ) {
        if ( IsKeyPressed( KEY_UP ) ) {
            int state = (int) gw->player1->state;
            state--;
            if ( state == -1 ) {
                gw->player1->state = PLAYER_STATE_LAST - 1;
            } else {
                gw->player1->state = state;
            }
            editorMode = EDITOR_MODE_COLLISION_BOX;
        } else if ( IsKeyPressed( KEY_DOWN ) ) {
            gw->player1->state++;
            if ( gw->player1->state == PLAYER_STATE_LAST ) {
                gw->player1->state = PLAYER_STATE_IDLE;
            }
            editorMode = EDITOR_MODE_COLLISION_BOX;
        }
    }

    Animation *anim = getPlayerCurrentAnimation( gw->player1 );

    if ( IsKeyUp( KEY_M ) && IsKeyUp( KEY_SPACE ) ) {
        if ( IsKeyPressed( KEY_LEFT ) ) {
            anim->currentFrame--;
            if ( anim->currentFrame < 0 ) {
                anim->currentFrame = anim->frameCount - 1;
            }
        } else if ( IsKeyPressed( KEY_RIGHT ) ) {
            anim->currentFrame++;
            if ( anim->currentFrame >= anim->frameCount ) {
                anim->currentFrame = 0;
            }
        }
    }

    if ( IsKeyDown( KEY_LEFT_CONTROL ) ) {
        if ( IsKeyPressed( KEY_A ) ) {
            gw->player1->pos.x--;
        } else if ( IsKeyPressed( KEY_D ) ) {
            gw->player1->pos.x++;
        } else if ( IsKeyPressed( KEY_W ) ) {
            gw->player1->pos.y--;
        } else if ( IsKeyPressed( KEY_S ) ) {
            gw->player1->pos.y++;
        }
    } else {
        if ( IsKeyDown( KEY_A ) ) {
            gw->player1->pos.x--;
        } else if ( IsKeyDown( KEY_D ) ) {
            gw->player1->pos.x++;
        } else if ( IsKeyDown( KEY_W ) ) {
            gw->player1->pos.y--;
        } else if ( IsKeyDown( KEY_S ) ) {
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
    } else if ( IsKeyPressed( KEY_SEVEN && af->boxes.hurtboxCount >= 3 ) ) {
        editorMode = EDITOR_MODE_HURT_BOX_2;
    }

    switch ( editorMode ) {
        case EDITOR_MODE_COLLISION_BOX: editAnimationFrameBox( &af->boxes.collisionBox ); break;
        case EDITOR_MODE_HIT_BOX_0: editAnimationFrameBox( &af->boxes.hitboxes[0] ); break;
        case EDITOR_MODE_HIT_BOX_1: editAnimationFrameBox( &af->boxes.hitboxes[1] ); break;
        case EDITOR_MODE_HIT_BOX_2: editAnimationFrameBox( &af->boxes.hitboxes[2] ); break;
        case EDITOR_MODE_HURT_BOX_0: editAnimationFrameBox( &af->boxes.hurtboxes[0] ); break;
        case EDITOR_MODE_HURT_BOX_1: editAnimationFrameBox( &af->boxes.hurtboxes[1] ); break;
        case EDITOR_MODE_HURT_BOX_2: editAnimationFrameBox( &af->boxes.hurtboxes[2] ); break;
        default: break;
    }

    updateCameraEditing( gw );

}

static void editAnimationFrameBox( Rectangle *box ) {

    if ( IsKeyDown( KEY_M ) ) {
        if ( IsKeyDown( KEY_RIGHT_CONTROL ) ) {
            if ( IsKeyPressed( KEY_LEFT ) ) {
                box->x--;
            } else if ( IsKeyPressed( KEY_RIGHT ) ) {
                box->x++;
            } else if ( IsKeyPressed( KEY_UP ) ) {
                box->y--;
            } else if ( IsKeyPressed( KEY_DOWN ) ) {
                box->y++;
            }
        } else {
            if ( IsKeyDown( KEY_LEFT ) ) {
                box->x--;
            } else if ( IsKeyDown( KEY_RIGHT ) ) {
                box->x++;
            } else if ( IsKeyDown( KEY_UP ) ) {
                box->y--;
            } else if ( IsKeyDown( KEY_DOWN ) ) {
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
            } else if ( IsKeyPressed( KEY_RIGHT ) ) {
                box->width++;
            } else if ( IsKeyPressed( KEY_UP ) ) {
                box->height--;
                if ( box->height < 0 ) {
                    box->height = 0;
                }
            } else if ( IsKeyPressed( KEY_DOWN ) ) {
                box->height++;
            }
        } else {
            if ( IsKeyDown( KEY_LEFT ) ) {
                box->width--;
                if ( box->width < 0 ) {
                    box->width = 0;
                }
            } else if ( IsKeyDown( KEY_RIGHT ) ) {
                box->width++;
            } else if ( IsKeyDown( KEY_UP ) ) {
                box->height--;
                if ( box->height < 0 ) {
                    box->height = 0;
                }
            } else if ( IsKeyDown( KEY_DOWN ) ) {
                box->height++;
            }
        }
    }

}

static void showAnimationFrameBoxDetail( Player *p, Rectangle *box, Color color ) {

    color = ColorBrightness( color, -0.5f );

    int x = (int) box->x;
    int y = (int) box->y;
    int w = (int) box->width;
    int h = (int) box->height;

    if ( !( w == 0 && h == 0 ) ) {
        DrawText( "offsets", p->pos.x + box->x, p->pos.y + box->y - 50, 5, color );
        DrawText( TextFormat( "x: %d", x ), p->pos.x + box->x, p->pos.y + box->y - 40, 5, color );
        DrawText( TextFormat( "y: %d", y ), p->pos.x + box->x, p->pos.y + box->y - 30, 5, color );
        DrawText( TextFormat( "w: %d", w ), p->pos.x + box->x, p->pos.y + box->y - 20, 5, color );
        DrawText( TextFormat( "h: %d", h ), p->pos.x + box->x, p->pos.y + box->y - 10, 5, color );
    } else {
        DrawText( "disabled", p->pos.x + box->x, p->pos.y + box->y - 10, 5, Fade( color, 0.5f ) );
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
        if ( player->state == PLAYER_STATE_JUMPING_STRAIGHT ||
             player->state == PLAYER_STATE_JUMPING_FORWARD  ||
             player->state == PLAYER_STATE_JUMPING_BACKWARD ) {
            resetPlayerAnimations( player );
            player->vel.x = 0.0f;
            player->state = PLAYER_STATE_JUMP_COOLDOWN;
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