/**
 * @file PlayerRender.c
 * @author Prof. Dr. David Buzatto
 * @brief Player rendering implementation.
 *
 * @copyright Copyright (c) 2026
 */
#include <math.h>
#include <stddef.h>

#include "raylib/raylib.h"
#include "raylib/rlgl.h"

#include "Animation.h"
#include "Macros.h"
#include "PlayerAnimation.h"
#include "PlayerRender.h"
#include "Projectile.h"
#include "ResourceManager.h"
#include "Types.h"

// forward declaration of internal helper
static void drawPlayerAnimationFrameBoxes( Player *player, AnimationFrame *af, Vector2 offset );

void drawPlayer( Player *player ) {

    AnimationFrame *af = getPlayerCurrentAnimationFrame( player );
    drawPlayerAnimationFrame( player, af, (Vector2) { 0 }, WHITE );

    if ( player->showDebugInfo ) {

        DrawCircle( player->pos.x, player->pos.y, 1, ORANGE );
        DrawRectangleLines( player->pos.x - player->dim.x / 2, player->pos.y, player->dim.x, player->dim.y, ORANGE );
        DrawText( TextFormat( "x: %.2f", player->pos.x ), player->pos.x + 5, player->pos.y - 20, 10, BLACK );
        DrawText( TextFormat( "y: %.2f", player->pos.y ), player->pos.x + 5, player->pos.y - 10, 10, BLACK );

    }

}

void drawPlayerShadow( Player *player, float floorY, float shear, float scaleY ) {

    float dy = floorY - ( player->pos.y + player->dim.y );

    AnimationFrame *af = getPlayerCurrentAnimationFrame( player );
    drawPlayerAnimationFrameForShadow( player, af, (Vector2) { 0 }, Fade( BLACK, 0.5f - dy / 400 ), floorY - dy / 10, shear, scaleY );

    /*DrawEllipse(
        player->pos.x,
        floorY - 5,
        player->dim.x - 15 + dy / 10,
        10 + dy / 20,
        Fade( BLACK, 0.5f - dy / 200 )
    );*/

}

void drawPlayerInputBuffer( Player *player ) {

    int backWidth = 100;

    if ( player->startSide == PLAYER_START_SIDE_LEFT ) {
        DrawRectangleGradientH( 0, 0, backWidth, GetScreenHeight(), Fade( BLACK, 0.7f ), BLANK );
    } else {
        DrawRectangleGradientH( GetScreenWidth() - backWidth, 0, backWidth, GetScreenHeight(), BLANK, Fade( BLACK, 0.7f ) );
    }

    if ( player->inputBufferSize == 0 ) {
        return;
    }

    int margin = 35;
    int step = 35;
    int width = 23;
    int height = width;

    int startX = margin;
    int startY = 120 + step * ( player->inputBufferSize - 1 );
    int lineStart = 0;

    int sliceX = 1;
    int sliceY = 1;
    int sliceW = 23;
    int sliceH = 23;
    int sliceDiv = 1;

    if ( player->startSide == PLAYER_START_SIDE_RIGHT ) {
        startX = GetScreenWidth() - margin;
        lineStart = startX - width;
    }

    int c = 0;

    for ( int i = player->inputBufferHead; i <= player->inputBufferTail; i++ ) {
        int p = i % PLAYER_INPUT_BUFFER_SIZE;
        InputType type = player->inputBuffer[p].type;
        int x = startX - width / 2;
        int y = startY - height / 2 - step * c;
        DrawTexturePro(
            rm.inputIconsTexture,
            (Rectangle) { sliceX + ( sliceW + sliceDiv ) * type, sliceY, sliceW, sliceH },
            (Rectangle) { x, y, width, height },
            (Vector2) { 0 },
            0.0f,
            WHITE
        );
        y = y + height + ( step - height ) / 2;
        DrawLineEx( (Vector2) { lineStart, y }, (Vector2) { lineStart + x + width + 10, y }, 2.0f, Fade( WHITE, 0.5f ) );
        c++;
    }

}

void drawPlayerOnionLayers( Player *player, int xOffset ) {

    Animation *a = getPlayerCurrentAnimation( player );
    AnimationFrame *af = getPlayerCurrentAnimationFrame( player );

    drawPlayer( player );

    // search
    for ( int i = 0; i < a->frameCount; i++ ) {
        if ( af == &a->frames[i] ) {
            for ( int j = 1; j < a->frameCount; j++ ) {
                int next = ( i + j ) % a->frameCount;
                drawPlayerAnimationFrame( player, &(getPlayerCurrentAnimation( player )->frames[next]), (Vector2) { xOffset * j, 0 }, Fade( WHITE, 0.5f ) );
            }
        }
    }

}

void drawPlayerAnimationFrame( Player *player, AnimationFrame *af, Vector2 offset, Color tint ) {

    if ( af != NULL ) {
        DrawTexturePro(
            player->currentSpriteMap,
            (Rectangle) {
                af->source.x,
                af->source.y,
                player->lookingRight ? af->source.width : -af->source.width,
                af->source.height
            },
            (Rectangle) {
                offset.x + player->pos.x - fabs( af->source.width ) / 2 + ( player->lookingRight ? af->offset.x : -af->offset.x ),
                offset.y + player->pos.y + player->dim.y - af->source.height + af->offset.y,
                af->source.width,
                af->source.height
            },
            (Vector2) { 0 },
            0.0f,
            tint
        );
    }

    if ( player->showBoxes ) {
        drawPlayerAnimationFrameBoxes( player, af, offset );
    }

}

void drawPlayerAnimationFrameForShadow( Player *player, AnimationFrame *af, Vector2 offset, Color tint, float floorY, float shear, float scaleY ) {

    if ( af == NULL ) return;

    float srcWidth  = fabs( af->source.width );
    float srcHeight = af->source.height;
    float texW      = (float) player->currentSpriteMap.width;
    float texH      = (float) player->currentSpriteMap.height;

    // destination size: height flattened by scaleY
    float destW = srcWidth;
    float destH = srcHeight * scaleY;

    // X position: same logic as the normal draw
    float destX = offset.x + player->pos.x - srcWidth / 2.0f + ( player->lookingRight ? af->offset.x : -af->offset.x );

    // Y position: shadow base anchored to the floor (floorY)
    float destY = floorY - destH;

    // UV coordinates of the source rect in the spritesheet
    float u0 = af->source.x / texW;
    float u1 = ( af->source.x + srcWidth ) / texW;
    float v0 = af->source.y / texH;
    float v1 = ( af->source.y + srcHeight ) / texH;

    // flip UVs horizontally when the sprite should be mirrored
    // (same logic as drawPlayerAnimationFrame: lookingRight uses negative source.width)
    if ( ( player->lookingRight ? af->source.width : -af->source.width ) < 0 ) {
        float tmp = u0; u0 = u1; u1 = tmp;
    }

    rlSetTexture( player->currentSpriteMap.id );
    rlBegin( RL_QUADS );
        rlNormal3f( 0.0f, 0.0f, 1.0f );
        rlColor4ub( tint.r, tint.g, tint.b, tint.a );

        // top-left  (shifted right by shear)
        rlTexCoord2f( u0, v0 );
        rlVertex2f( destX + shear, destY );

        // bottom-left  (anchored to the floor)
        rlTexCoord2f( u0, v1 );
        rlVertex2f( destX, destY + destH );

        // bottom-right  (anchored to the floor)
        rlTexCoord2f( u1, v1 );
        rlVertex2f( destX + destW, destY + destH );

        // top-right  (shifted right by shear)
        rlTexCoord2f( u1, v0 );
        rlVertex2f( destX + destW + shear, destY );
    rlEnd();
    rlSetTexture( 0 );

}

static void drawPlayerAnimationFrameBoxes( Player *player, AnimationFrame *af, Vector2 offset ) {

    int x = (int) ( player->pos.x + af->boxes.collisionBox.x + offset.x );
    int y = (int) ( player->pos.y + af->boxes.collisionBox.y + offset.y );
    int w = (int) af->boxes.collisionBox.width;
    int h = (int) af->boxes.collisionBox.height;

    if ( !player->lookingRight ) {
        x = (int) ( player->pos.x - af->boxes.collisionBox.x - af->boxes.collisionBox.width + offset.x );
    }

    if ( !( w == 0 && h == 0 ) ) {
        DrawRectangle( x, y, w, h, Fade( GREEN, 0.4 ) );
        DrawRectangleLines( x, y, w, h, GREEN );
    }

    for ( int i = 0; i < af->boxes.hitboxCount; i++ ) {

        Rectangle *r = &af->boxes.hitboxes[i];
        int x = (int) ( player->pos.x + r->x + offset.x );
        int y = (int) ( player->pos.y + r->y + offset.y );
        int w = (int) r->width;
        int h = (int) r->height;

        if ( !player->lookingRight ) {
            x = (int) ( player->pos.x - r->x - r->width + offset.x );
        }

        if ( !( w == 0 && h == 0 ) ) {
            DrawText( TextFormat( "%d", i ), x + 1, y, 5, ColorBrightness( BLUE, -0.7 ) );
            DrawRectangle( x, y, w, h, Fade( BLUE, 0.4 ) );
            DrawRectangleLines( x, y, w, h, BLUE );
        }

    }

    for ( int i = 0; i < af->boxes.hurtboxCount; i++ ) {

        Rectangle *r = &af->boxes.hurtboxes[i];
        int x = (int) ( player->pos.x + r->x + offset.x );
        int y = (int) ( player->pos.y + r->y + offset.y );
        int w = (int) r->width;
        int h = (int) r->height;

        if ( !player->lookingRight ) {
            x = (int) ( player->pos.x - r->x - r->width + offset.x );
        }

        if ( !( w == 0 && h == 0 ) ) {
            DrawText( TextFormat( "%d", i ), x + 1, y, 5, ColorBrightness( RED, -0.7 ) );
            DrawRectangle( x, y, w, h, Fade( RED, 0.4 ) );
            DrawRectangleLines( x, y, w, h, RED );
        }

    }

}

void drawOnHitPlayerAnimation( Player *p ) {

    if ( !p->onHitPosActive ) {
        return;
    }

    AnimationFrame *af = getAnimationCurrentFrame( &p->onHitAnimation );

    DrawTexturePro(
        rm.effectsTexture,
        (Rectangle) { af->source.x, af->source.y, af->source.width, af->source.height },
        (Rectangle) { p->onHitPos.x - af->source.width / 2, p->onHitPos.y - af->source.height / 2, af->source.width, af->source.height },
        (Vector2) { 0 },
        0.0f,
        WHITE
    );

}

void drawOnBlockPlayerAnimation( Player *p ) {

    if ( !p->onBlockPosActive ) {
        return;
    }

    AnimationFrame *af = getAnimationCurrentFrame( &p->onBlockAnimation );

    DrawTexturePro(
        rm.effectsTexture,
        (Rectangle) { af->source.x, af->source.y, af->source.width, af->source.height },
        (Rectangle) { p->onBlockPos.x - af->source.width / 2, p->onBlockPos.y - af->source.height / 2, af->source.width, af->source.height },
        (Vector2) { 0 },
        0.0f,
        WHITE
    );

}

void drawPlayerProjectile( Player *p ) {
    drawProjectile( p->projectile );
}
