/**
 * @file EditorMode.c
 * @author Prof. Dr. David Buzatto
 * @brief Animation editor draw/update implementation.
 *
 * @copyright Copyright (c) 2026
 */
#include <stdlib.h>

#include "raylib/raylib.h"

#include "Animation.h"
#include "EditorMode.h"
#include "GameCamera.h"
#include "Player.h"
#include "PlayerAnimation.h"
#include "PlayerRender.h"
#include "Types.h"
#include "Utils.h"

// ---------------------------------------------------------------------------
// Forward declarations — internal helpers
// ---------------------------------------------------------------------------

static void editAnimationFrameBox( Rectangle *box );
static void showAnimationFrameBoxDetail( Player *p, Rectangle *box, Camera2D camera, Color color );
static void drawInfoPanel( GameWorld *gw );
static void drawEditorHelp( void );
static void copyCurrentAnimationFrameBoxPrevious( Player *p );
static void copyCurrentAnimationFrameBoxNext( Player *p );
static void copyAllAnimationFrameBoxesPrevious( Player *p );
static void copyAllAnimationFrameBoxesNext( Player *p );
static void copyAllAnimationFrameBoxesToPreviousAnimation( Player *p );
static void copyAllAnimationFrameBoxesToNextAnimation( Player *p );
static void adjustAllAnimationFrameBoxes( Player *p, int offsetX, int offsetY );

// ---------------------------------------------------------------------------
// Shared state (extern'd in EditorMode.h so GameWorld.c can read/write them)
// ---------------------------------------------------------------------------

bool showPlayerOnionEditing = SHOW_PLAYER_ONION_MODE_EDITING;
bool showPlayerInputBuffer = SHOW_PLAYER_INPUT_BUFFER;
PlayerState lastEditState = PLAYER_STATE_LAST;
int lastEditFrame = 0;

// ---------------------------------------------------------------------------
// Internal state — not used outside this file
// ---------------------------------------------------------------------------

static EditorMode editorMode = EDITOR_MODE_COLLISION_BOX;
static bool runPlayerCurrentAnimation = false;
static bool runPlayerCurrentAnimationOnce = false;
static int onionOffset = 40;
static bool showHelp = false;
static int saveTimer = 0;

// ---------------------------------------------------------------------------
// Implementation
// ---------------------------------------------------------------------------

void drawGameWorldEditing( GameWorld *gw ) {

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

void updateGameWorldEditing( GameWorld *gw, float delta ) {

    if ( saveTimer > 0 ) saveTimer--;

    if ( IsKeyPressed( KEY_H ) ) {
        showHelp = !showHelp;
    }

    if ( IsKeyDown( KEY_LEFT_CONTROL ) && IsKeyPressed( KEY_S ) ) {
        resetPlayerAnimations( gw->player1 );
        if ( gw->player1->type == PLAYER_TYPE_RYU ) {
            storePlayerAnimations( gw->player1, true, false, RYU_ANIMATIONS_FILE );
        } else {
            storePlayerAnimations( gw->player1, true, false, KEN_ANIMATIONS_FILE );
        }
        saveTimer = 90;
        return;
    }

    if ( IsKeyPressed( KEY_BACKSPACE ) ) {
        adjustAllAnimationFrameBoxes( gw->player1, 32, 0 );
    }

    if ( IsKeyDown( KEY_LEFT_CONTROL ) && IsKeyPressed( KEY_X ) ) {
        if ( IsKeyDown( KEY_LEFT_ALT ) ) {
            copyAllAnimationFrameBoxesToPreviousAnimation( gw->player1 );
        } if ( IsKeyDown( KEY_LEFT_SHIFT ) ) {
            copyAllAnimationFrameBoxesPrevious( gw->player1 );
        } else {
            copyCurrentAnimationFrameBoxPrevious( gw->player1 );

        }
        return;
    }

    if ( IsKeyDown( KEY_LEFT_CONTROL ) && IsKeyPressed( KEY_C ) ) {
        if ( IsKeyDown( KEY_LEFT_ALT ) ) {
            copyAllAnimationFrameBoxesToNextAnimation( gw->player1 );
        } else if ( IsKeyDown( KEY_LEFT_SHIFT ) ) {
            copyAllAnimationFrameBoxesNext( gw->player1 );
        } else {
            copyCurrentAnimationFrameBoxNext( gw->player1 );
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
        updateAnimation( anim, gw->player1->animationDurationMode, delta );
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
        updateAnimation( anim, gw->player1->animationDurationMode, delta );
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
        hint = "CTRL+S: save | CTRL[+SHIFT]+X/C: copy boxes | CTRL+ALT+X/C: copy animation";
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
    DrawText( "O + RCTRL + arrows", col2, yR, fs, WHITE ); DrawText( "step 1",        desc2, yR, fs, WHITE ); yR += lh;
    yR += lh / 2;

    DrawText( "COPY BOXES",     col2, yR, fs, LIGHTGRAY ); yR += lh;
    DrawText( "CTRL + X",       col2, yR, fs, WHITE ); DrawText( "copy current -> previous frame", desc2, yR, fs, WHITE ); yR += lh;
    DrawText( "CTRL + C",       col2, yR, fs, WHITE ); DrawText( "copy current -> next frame",     desc2, yR, fs, WHITE ); yR += lh;
    DrawText( "CTRL+SHIFT + X", col2, yR, fs, WHITE ); DrawText( "copy all -> previous frame",     desc2, yR, fs, WHITE ); yR += lh;
    DrawText( "CTRL+SHIFT + C", col2, yR, fs, WHITE ); DrawText( "copy all -> next frame",         desc2, yR, fs, WHITE ); yR += lh;
    DrawText( "CTRL+ALT + X",   col2, yR, fs, WHITE ); DrawText( "copy all -> prev anim.",         desc2, yR, fs, WHITE ); yR += lh;
    DrawText( "CTRL+ALT + C",   col2, yR, fs, WHITE ); DrawText( "copy all -> next anim.",         desc2, yR, fs, WHITE ); yR += lh;
    yR += lh / 2;

    DrawText( "FILE / GENERAL", col2, yR, fs, LIGHTGRAY ); yR += lh;
    DrawText( "CTRL + S",       col2, yR, fs, WHITE ); DrawText( "save",                  desc2, yR, fs, WHITE ); yR += lh;
    DrawText( "F1",             col2, yR, fs, WHITE ); DrawText( "toggle play/edit mode", desc2, yR, fs, WHITE ); yR += lh;
    DrawText( "F2",             col2, yR, fs, WHITE ); DrawText( "show/hide boxes",       desc2, yR, fs, WHITE ); yR += lh;
    DrawText( "F3",             col2, yR, fs, WHITE ); DrawText( "show/hide debug",       desc2, yR, fs, WHITE ); yR += lh;
    DrawText( "H",              col2, yR, fs, WHITE ); DrawText( "show/hide help",        desc2, yR, fs, WHITE ); yR += lh;

}

static void copyCurrentAnimationFrameBoxPrevious( Player *p ) {

    Animation *a = getPlayerCurrentAnimation( p );
    AnimationFrame *sourceAf = getPlayerCurrentAnimationFrame( p );

    for ( int i = 0; i < a->frameCount; i++ ) {
        if ( &a->frames[i] == sourceAf ) {

            int prev = i - 1;
            if ( prev == -1 ) {
                prev = a->frameCount - 1;
            }
            AnimationFrame *destAf = &a->frames[prev];

            switch ( editorMode ) {
                case EDITOR_MODE_COLLISION_BOX: destAf->boxes.collisionBox = sourceAf->boxes.collisionBox; return;
                case EDITOR_MODE_HIT_BOX_0:     destAf->boxes.hitboxes[0]  = sourceAf->boxes.hitboxes[0];  return;
                case EDITOR_MODE_HIT_BOX_1:     destAf->boxes.hitboxes[1]  = sourceAf->boxes.hitboxes[1];  return;
                case EDITOR_MODE_HIT_BOX_2:     destAf->boxes.hitboxes[2]  = sourceAf->boxes.hitboxes[2];  return;
                case EDITOR_MODE_HURT_BOX_0:    destAf->boxes.hurtboxes[0] = sourceAf->boxes.hurtboxes[0]; return;
                case EDITOR_MODE_HURT_BOX_1:    destAf->boxes.hurtboxes[1] = sourceAf->boxes.hurtboxes[1]; return;
                case EDITOR_MODE_HURT_BOX_2:    destAf->boxes.hurtboxes[2] = sourceAf->boxes.hurtboxes[2]; return;
                default: return;
            }

        }

    }

}

static void copyCurrentAnimationFrameBoxNext( Player *p ) {

    Animation *a = getPlayerCurrentAnimation( p );
    AnimationFrame *sourceAf = getPlayerCurrentAnimationFrame( p );

    for ( int i = 0; i < a->frameCount; i++ ) {
        if ( &a->frames[i] == sourceAf ) {

            int next = ( i + 1 ) % a->frameCount;
            AnimationFrame *destAf = &a->frames[next];

            switch ( editorMode ) {
                case EDITOR_MODE_COLLISION_BOX: destAf->boxes.collisionBox = sourceAf->boxes.collisionBox; return;
                case EDITOR_MODE_HIT_BOX_0:     destAf->boxes.hitboxes[0]  = sourceAf->boxes.hitboxes[0];  return;
                case EDITOR_MODE_HIT_BOX_1:     destAf->boxes.hitboxes[1]  = sourceAf->boxes.hitboxes[1];  return;
                case EDITOR_MODE_HIT_BOX_2:     destAf->boxes.hitboxes[2]  = sourceAf->boxes.hitboxes[2];  return;
                case EDITOR_MODE_HURT_BOX_0:    destAf->boxes.hurtboxes[0] = sourceAf->boxes.hurtboxes[0]; return;
                case EDITOR_MODE_HURT_BOX_1:    destAf->boxes.hurtboxes[1] = sourceAf->boxes.hurtboxes[1]; return;
                case EDITOR_MODE_HURT_BOX_2:    destAf->boxes.hurtboxes[2] = sourceAf->boxes.hurtboxes[2]; return;
                default: return;
            }

        }

    }

}

static void copyAllAnimationFrameBoxesPrevious( Player *p ) {

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

static void copyAllAnimationFrameBoxesNext( Player *p ) {

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

static void copyAllAnimationFrameBoxesToPreviousAnimation( Player *p ) {

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

static void copyAllAnimationFrameBoxesToNextAnimation( Player *p ) {

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

static void adjustAllAnimationFrameBoxes( Player *p, int offsetX, int offsetY ) {

    Animation *a = getPlayerCurrentAnimation( p );

    for ( int i = 0; i < a->frameCount; i++ ) {
        AnimationFrameBoxes *afb = &a->frames[i].boxes;
        afb->collisionBox.x -= offsetX;
        afb->collisionBox.y -= offsetY;
        for ( int j = 0; j < afb->hitboxCount; j++ ) {
            Rectangle *r = &afb->hitboxes[j];
            if ( r->width != 0 && r->height != 0 ) {
                r->x -= offsetX;
                r->y -= offsetY;
            }
        }
        for ( int j = 0; j < afb->hurtboxCount; j++ ) {
            Rectangle *r = &afb->hurtboxes[j];
            if ( r->width != 0 && r->height != 0 ) {
                r->x -= offsetX;
                r->y -= offsetY;
            }
        }
    }

}
