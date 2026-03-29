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
#define DRAW_PLAYER_ONION true
#define DURATION_MODE DURATION_MODE_MILLISECONDS
#define INITIAL_GAME_MODE GAME_MODE_EDITING

#define PLAYER_1_FILE "resources/animations/ryu.json"
#define PLAYER_2_FILE "resources/animations/ken.json"

static void drawGameWorldPlaying( GameWorld *gw );
static void drawGameWorldEditing( GameWorld *gw );
static void updateGameWorldPlaying( GameWorld *gw, float delta );
static void updateGameWorldEditing( GameWorld *gw, float delta );
static void editAnimationFrameBox( Rectangle *box );
static void showAnimationFrameBoxDetail( Player *p, Rectangle *box, Camera2D camera, Color color );
static void copyAnimationFrameBoxesPrevious( Player *p );
static void copyAnimationFrameBoxesNext( Player *p );

static void updateCameraPlaying( GameWorld *gw );
static void updateCameraEditing( GameWorld *gw );

static void resolveCollisionPlayerStage( Player *player, GameWorld *gw );
static void flipPlayers( GameWorld *gw );

// editor
static EditorMode editorMode = EDITOR_MODE_COLLISION_BOX;
static bool drawPlayerOnion = DRAW_PLAYER_ONION;
static int onionOffset = 40;

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

    initializePlayerRyu( gw->stageTexture->width / 2 - 78, 542, player1, DURATION_MODE, SHOW_BOXES, SHOW_DEBUG_INFO );
    initializePlayerKen( gw->stageTexture->width / 2 + 50, 542, player2, DURATION_MODE, SHOW_BOXES, SHOW_DEBUG_INFO );
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

    if ( IsKeyPressed( KEY_F4 ) ) {
        drawPlayerOnion = !drawPlayerOnion;
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
    drawPlayer( gw->player2 );
    drawPlayer( gw->player1 );

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

    if ( drawPlayerOnion ) {
        drawPlayerOnionLayers( gw->player1, onionOffset );
    } else {
        drawPlayer( gw->player1 );
    }

    EndMode2D();

    DrawRectangle( 0, 0, GetScreenWidth(), 100, Fade( BLACK, 0.2f ) );
    DrawText( "Animation Frame Box Editor", 5, 5, 20, BLACK );
    DrawText( TextFormat( "Mode: %s", utilsEditorModeToText( editorMode ) ), 5, 35, 20, BLACK );
    DrawText( TextFormat( "State: %s", utilsPlayerStateToText( gw->player1->state ) ), 5, 55, 20, BLACK );
    DrawText( TextFormat( "Frame: %d", getPlayerCurrentAnimation( gw->player1 )->currentFrame ), 5, 75, 20, BLACK );

    AnimationFrame *af = getPlayerCurrentAnimationFrame( gw->player1 );

    switch ( editorMode ) {
        case EDITOR_MODE_COLLISION_BOX: showAnimationFrameBoxDetail( gw->player1, &af->boxes.collisionBox, gw->camera, GREEN ); break;
        case EDITOR_MODE_HIT_BOX_0: showAnimationFrameBoxDetail( gw->player1, &af->boxes.hitboxes[0], gw->camera, BLUE ); break;
        case EDITOR_MODE_HIT_BOX_1: showAnimationFrameBoxDetail( gw->player1, &af->boxes.hitboxes[1], gw->camera, BLUE ); break;
        case EDITOR_MODE_HIT_BOX_2: showAnimationFrameBoxDetail( gw->player1, &af->boxes.hitboxes[2], gw->camera, BLUE ); break;
        case EDITOR_MODE_HURT_BOX_0: showAnimationFrameBoxDetail( gw->player1, &af->boxes.hurtboxes[0], gw->camera, RED ); break;
        case EDITOR_MODE_HURT_BOX_1: showAnimationFrameBoxDetail( gw->player1, &af->boxes.hurtboxes[1], gw->camera, RED ); break;
        case EDITOR_MODE_HURT_BOX_2: showAnimationFrameBoxDetail( gw->player1, &af->boxes.hurtboxes[2], gw->camera, RED ); break;
        default: break;
    }

}

static void updateGameWorldEditing( GameWorld *gw, float delta ) {

    if ( IsKeyDown( KEY_LEFT_CONTROL ) && IsKeyPressed( KEY_S ) ) {
        storePlayerAnimations( gw->player1, true, false, PLAYER_1_FILE );
        return;
    }

    if ( IsKeyDown( KEY_LEFT_CONTROL ) && IsKeyPressed( KEY_X ) ) {
        copyAnimationFrameBoxesPrevious( gw->player1 );
        return;
    }

    if ( IsKeyDown( KEY_LEFT_CONTROL ) && IsKeyPressed( KEY_C ) ) {
        copyAnimationFrameBoxesNext( gw->player1 );
        return;
    }

    if ( IsKeyDown( KEY_O ) ) {
        if ( IsKeyPressed( KEY_LEFT ) ) {
            onionOffset--;
        } else if ( IsKeyPressed( KEY_RIGHT ) ) {
            onionOffset++;
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

static void copyAnimationFrameBoxesPrevious( Player *p ) {
        
    Animation *a = getPlayerCurrentAnimation( p );
    AnimationFrame *sourceAf = getPlayerCurrentAnimationFrame( p );

    for ( int i = 0; i < a->frameCount; i++ ) {
        if ( &a->frames[i] == sourceAf ) {
            int prev = i - 1;
            if ( i == -1 ) {
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