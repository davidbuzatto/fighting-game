#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

#include "raylib/raylib.h"
#include "parson/parson.h"

#include "Macros.h"
#include "ResourceManager.h"
#include "Types.h"

const char *utilsPlayerStateToText( PlayerState state ) {

    switch ( state ) {
        case PLAYER_STATE_IDLE: return "PLAYER_STATE_IDLE";
        case PLAYER_STATE_WALKING_FORWARD: return "PLAYER_STATE_WALKING_FORWARD";
        case PLAYER_STATE_WALKING_BACKWARD: return "PLAYER_STATE_WALKING_BACKWARD";
        case PLAYER_STATE_JUMPING_STRAIGHT: return "PLAYER_STATE_JUMPING_STRAIGHT";
        case PLAYER_STATE_JUMPING_FORWARD: return "PLAYER_STATE_JUMPING_FORWARD";
        case PLAYER_STATE_JUMPING_BACKWARD: return "PLAYER_STATE_JUMPING_BACKWARD";
        case PLAYER_STATE_CROUCHING: return "PLAYER_STATE_CROUCHING";
        case PLAYER_STATE_LP: return "PLAYER_STATE_LP";
        case PLAYER_STATE_MP: return "PLAYER_STATE_MP";
        case PLAYER_STATE_HP: return "PLAYER_STATE_HP";
        case PLAYER_STATE_LK: return "PLAYER_STATE_LK";
        case PLAYER_STATE_MK: return "PLAYER_STATE_MK";
        case PLAYER_STATE_HK: return "PLAYER_STATE_HK";
        case PLAYER_STATE_LP_CLOSE: return "PLAYER_STATE_LP_CLOSE";
        case PLAYER_STATE_MP_CLOSE: return "PLAYER_STATE_MP_CLOSE";
        case PLAYER_STATE_HP_CLOSE: return "PLAYER_STATE_HP_CLOSE";
        case PLAYER_STATE_LK_CLOSE: return "PLAYER_STATE_LK_CLOSE";
        case PLAYER_STATE_MK_CLOSE: return "PLAYER_STATE_MK_CLOSE";
        case PLAYER_STATE_HK_CLOSE: return "PLAYER_STATE_HK_CLOSE";
        case PLAYER_STATE_LP_CROUCH: return "PLAYER_STATE_LP_CROUCH";
        case PLAYER_STATE_MP_CROUCH: return "PLAYER_STATE_MP_CROUCH";
        case PLAYER_STATE_HP_CROUCH: return "PLAYER_STATE_HP_CROUCH";
        case PLAYER_STATE_LK_CROUCH: return "PLAYER_STATE_LK_CROUCH";
        case PLAYER_STATE_MK_CROUCH: return "PLAYER_STATE_MK_CROUCH";
        case PLAYER_STATE_HK_CROUCH: return "PLAYER_STATE_HK_CROUCH";
        case PLAYER_STATE_JUMP_COOLDOWN: return "PLAYER_STATE_JUMP_COOLDOWN";
        case PLAYER_STATE_LP_JUMP_STRAIGHT: return "PLAYER_STATE_LP_STRAIGHT";
        case PLAYER_STATE_MP_JUMP_STRAIGHT: return "PLAYER_STATE_MP_STRAIGHT";
        case PLAYER_STATE_HP_JUMP_STRAIGHT: return "PLAYER_STATE_HP_STRAIGHT";
        case PLAYER_STATE_LK_JUMP_STRAIGHT: return "PLAYER_STATE_LK_STRAIGHT";
        case PLAYER_STATE_MK_JUMP_STRAIGHT: return "PLAYER_STATE_MK_STRAIGHT";
        case PLAYER_STATE_HK_JUMP_STRAIGHT: return "PLAYER_STATE_HK_STRAIGHT";
        case PLAYER_STATE_LP_JUMP_FORWARD: return "PLAYER_STATE_LP_FORWARD";
        case PLAYER_STATE_MP_JUMP_FORWARD: return "PLAYER_STATE_MP_FORWARD";
        case PLAYER_STATE_HP_JUMP_FORWARD: return "PLAYER_STATE_HP_FORWARD";
        case PLAYER_STATE_LK_JUMP_FORWARD: return "PLAYER_STATE_LK_FORWARD";
        case PLAYER_STATE_MK_JUMP_FORWARD: return "PLAYER_STATE_MK_FORWARD";
        case PLAYER_STATE_HK_JUMP_FORWARD: return "PLAYER_STATE_HK_FORWARD";
        case PLAYER_STATE_LP_JUMP_BACKWARD: return "PLAYER_STATE_LP_BACKWARD";
        case PLAYER_STATE_MP_JUMP_BACKWARD: return "PLAYER_STATE_MP_BACKWARD";
        case PLAYER_STATE_HP_JUMP_BACKWARD: return "PLAYER_STATE_HP_BACKWARD";
        case PLAYER_STATE_LK_JUMP_BACKWARD: return "PLAYER_STATE_LK_BACKWARD";
        case PLAYER_STATE_MK_JUMP_BACKWARD: return "PLAYER_STATE_MK_BACKWARD";
        case PLAYER_STATE_HK_JUMP_BACKWARD: return "PLAYER_STATE_HK_BACKWARD";
        case PLAYER_STATE_HIT_UP_STANDING: return "PLAYER_STATE_HIT_UP_STANDING";
        case PLAYER_STATE_HIT_MID_STANDING: return "PLAYER_STATE_HIT_MID_STANDING";
        case PLAYER_STATE_HIT_CROUCH: return "PLAYER_STATE_HIT_CROUCH";
        case PLAYER_STATE_DEFENCE_STANDING: return "PLAYER_STATE_DEFENCE_STANDING";
        case PLAYER_STATE_DEFENCE_CROUCH: return "PLAYER_STATE_DEFENCE_CROUCH";
        case PLAYER_STATE_SPECIAL_LP_HADOUKEN: return "PLAYER_STATE_SPECIAL_LP_HADOUKEN";
        case PLAYER_STATE_SPECIAL_MP_HADOUKEN: return "PLAYER_STATE_SPECIAL_MP_HADOUKEN";
        case PLAYER_STATE_SPECIAL_HP_HADOUKEN: return "PLAYER_STATE_SPECIAL_HP_HADOUKEN";
        case PLAYER_STATE_SPECIAL_LP_SHORYUKEN: return "PLAYER_STATE_SPECIAL_LP_SHORYUKEN";
        case PLAYER_STATE_SPECIAL_MP_SHORYUKEN: return "PLAYER_STATE_SPECIAL_MP_SHORYUKEN";
        case PLAYER_STATE_SPECIAL_HP_SHORYUKEN: return "PLAYER_STATE_SPECIAL_HP_SHORYUKEN";
        case PLAYER_STATE_SPECIAL_LK_TATSUMAKI: return "PLAYER_STATE_SPECIAL_LK_TATSUMAKI";
        case PLAYER_STATE_SPECIAL_MK_TATSUMAKI: return "PLAYER_STATE_SPECIAL_MK_TATSUMAKI";
        case PLAYER_STATE_SPECIAL_HK_TATSUMAKI: return "PLAYER_STATE_SPECIAL_HK_TATSUMAKI";
        case PLAYER_STATE_FALLING: return "PLAYER_STATE_FALLING";
        case PLAYER_STATE_GETTING_UP: return "PLAYER_STATE_GETTING_UP";
        case PLAYER_STATE_VICTORY_1: return "PLAYER_STATE_VICTORY_1";
        case PLAYER_STATE_VICTORY_2: return "PLAYER_STATE_VICTORY_2";
        case PLAYER_STATE_FALLING_LOSE: return "PLAYER_STATE_FALLING_LOSE";
        case PLAYER_STATE_TIMEOVER: return "PLAYER_STATE_TIMEOVER";
        case PLAYER_STATE_LAST: return "PLAYER_STATE_LAST";
        default: return "CANT FIND";
    }

}

const char *utilsEditorModeToText( EditorMode mode ) {

    switch ( mode ) {
        case EDITOR_MODE_COLLISION_BOX: return "EDITOR_MODE_COLLISION_BOX";
        case EDITOR_MODE_HIT_BOX_0: return "EDITOR_MODE_HIT_BOX_0";
        case EDITOR_MODE_HIT_BOX_1: return "EDITOR_MODE_HIT_BOX_1";
        case EDITOR_MODE_HIT_BOX_2: return "EDITOR_MODE_HIT_BOX_2";
        case EDITOR_MODE_HURT_BOX_0: return "EDITOR_MODE_HURT_BOX_0";
        case EDITOR_MODE_HURT_BOX_1: return "EDITOR_MODE_HURT_BOX_1";
        case EDITOR_MODE_HURT_BOX_2: return "EDITOR_MODE_HURT_BOX_2";
        default: return "";
    }

}

void storePlayerAnimations( Player *p, bool prettyPrint, bool printOut, const char *filename ) {

    JSON_Value *rootValue = json_value_init_object();
    JSON_Object *rootObj = json_value_get_object( rootValue );

    JSON_Value *animationsArrayValue = json_value_init_array();
    JSON_Array *animationsArray = json_value_get_array( animationsArrayValue );

    for ( int s = 0; s < p->animationCount; s++ ) {

        Animation *a = p->animations[s];
        if ( a == NULL ) continue;

        JSON_Value *animationValue = json_value_init_object();
        JSON_Object *animationObj = json_value_get_object( animationValue );

        json_object_set_number( animationObj, "frameCount", a->frameCount );
        json_object_set_number( animationObj, "currentFrame", a->currentFrame );
        json_object_set_number( animationObj, "frameTimeCounter", a->frameTimeCounter );
        json_object_set_boolean( animationObj, "stopAtLastFrame", a->stopAtLastFrame );
        json_object_set_boolean( animationObj, "runOnce", a->runOnce );
        json_object_set_boolean( animationObj, "finished", a->finished );

        JSON_Value *framesArrayValue = json_value_init_array();
        JSON_Array *framesArray = json_value_get_array( framesArrayValue );

        for ( int f = 0; f < a->frameCount; f++ ) {

            AnimationFrame *af = &a->frames[f];
            AnimationFrameBoxes *afb = &af->boxes;

            JSON_Value *frameValue = json_value_init_object();
            JSON_Object *frameObj = json_value_get_object( frameValue );

            JSON_Value *sourceValue = json_value_init_object();
            JSON_Object *sourceObj = json_value_get_object( sourceValue );
            json_object_set_number( sourceObj, "x", af->source.x );
            json_object_set_number( sourceObj, "y", af->source.y );
            json_object_set_number( sourceObj, "width", af->source.width );
            json_object_set_number( sourceObj, "height", af->source.height );
            json_object_set_value( frameObj, "source", sourceValue );

            json_object_set_number( frameObj, "duration", af->duration );

            JSON_Value *offsetValue = json_value_init_object();
            JSON_Object *offsetObj = json_value_get_object( offsetValue );
            json_object_set_number( offsetObj, "x", af->offset.x );
            json_object_set_number( offsetObj, "y", af->offset.y );
            json_object_set_value( frameObj, "offset", offsetValue );

            JSON_Value *boxesValue = json_value_init_object();
            JSON_Object *boxesObj = json_value_get_object( boxesValue );

            JSON_Value *collisionBoxValue = json_value_init_object();
            JSON_Object *collisionBoxObj = json_value_get_object( collisionBoxValue );
            json_object_set_number( collisionBoxObj, "x", afb->collisionBox.x );
            json_object_set_number( collisionBoxObj, "y", afb->collisionBox.y );
            json_object_set_number( collisionBoxObj, "width", afb->collisionBox.width );
            json_object_set_number( collisionBoxObj, "height", afb->collisionBox.height );
            json_object_set_value( boxesObj, "collisionBox", collisionBoxValue );

            json_object_set_number( boxesObj, "hitboxCount", afb->hitboxCount );
            JSON_Value *hitboxArrayValue = json_value_init_array();
            JSON_Array *hitboxArray = json_value_get_array( hitboxArrayValue );
            for ( int i = 0; i < afb->hitboxCount; i++ ) {
                Rectangle *r = &afb->hitboxes[i];
                JSON_Value *rectValue = json_value_init_object();
                JSON_Object *rectObj = json_value_get_object( rectValue );
                json_object_set_number( rectObj, "x", r->x );
                json_object_set_number( rectObj, "y", r->y );
                json_object_set_number( rectObj, "width", r->width );
                json_object_set_number( rectObj, "height", r->height );
                json_array_append_value( hitboxArray, rectValue );
            }
            json_object_set_value( boxesObj, "hitboxes", hitboxArrayValue );

            json_object_set_number( boxesObj, "hurtboxCount", afb->hurtboxCount );
            JSON_Value *hurtboxArrayValue = json_value_init_array();
            JSON_Array *hurtboxArray = json_value_get_array( hurtboxArrayValue );
            for ( int i = 0; i < afb->hurtboxCount; i++ ) {
                Rectangle *r = &afb->hurtboxes[i];
                JSON_Value *rectValue = json_value_init_object();
                JSON_Object *rectObj = json_value_get_object( rectValue );
                json_object_set_number( rectObj, "x", r->x );
                json_object_set_number( rectObj, "y", r->y );
                json_object_set_number( rectObj, "width", r->width );
                json_object_set_number( rectObj, "height", r->height );
                json_array_append_value( hurtboxArray, rectValue );
            }
            json_object_set_value( boxesObj, "hurtboxes", hurtboxArrayValue );

            json_object_set_value( frameObj, "boxes", boxesValue );

            json_object_set_number( frameObj, "damageOnHurt", af->damageOnHurt );

            json_array_append_value( framesArray, frameValue );

        }

        json_object_set_value( animationObj, "frames", framesArrayValue );
        json_array_append_value( animationsArray, animationValue );

    }

    json_object_set_value( rootObj, "animations", animationsArrayValue );

    if ( filename != NULL ) {
        if ( prettyPrint ) {
            json_serialize_to_file_pretty( rootValue, filename );
        } else {
            json_serialize_to_file( rootValue, filename );
        }
    }

    if ( printOut ) {
        char *serialized_string = NULL;
        if ( prettyPrint ) {
            serialized_string = json_serialize_to_string_pretty( rootValue );
        } else {
            serialized_string = json_serialize_to_string( rootValue );
        }
        puts( serialized_string );
        json_free_serialized_string( serialized_string );    
    }
    
    json_value_free( rootValue );

}

void loadPlayerAnimationFrameBoxes( Player *p, const char *filename ) {

    JSON_Value *rootValue = json_parse_file( filename );
    if ( rootValue == NULL ) {
        printf( "loadPlayerAnimationFrameBoxes: falha ao carregar \"%s\"\n", filename );
        return;
    }

    JSON_Object *rootObj = json_value_get_object( rootValue );
    JSON_Array *animationsArray = json_object_get_array( rootObj, "animations" );
    if ( animationsArray == NULL ) {
        printf( "loadPlayerAnimationFrameBoxes: array \"animations\" nao encontrado em \"%s\"\n", filename );
        json_value_free( rootValue );
        return;
    }

    int animCount = (int) json_array_get_count( animationsArray );
    if ( animCount > p->animationCount ) {
        animCount = p->animationCount;
    }

    for ( int s = 0; s < animCount; s++ ) {

        Animation *a = p->animations[s];
        if ( a == NULL ) continue;

        JSON_Object *animationObj = json_array_get_object( animationsArray, s );
        if ( animationObj == NULL ) continue;

        JSON_Array *framesArray = json_object_get_array( animationObj, "frames" );
        if ( framesArray == NULL ) continue;

        int frameCount = (int) json_array_get_count( framesArray );
        if ( frameCount > a->frameCount ) {
            frameCount = a->frameCount;
        }

        for ( int f = 0; f < frameCount; f++ ) {

            AnimationFrame *af = &a->frames[f];

            JSON_Object *frameObj = json_array_get_object( framesArray, f );
            if ( frameObj == NULL ) continue;

            int duration = (int) json_object_get_number( frameObj, "duration" );
            af->duration = duration;

            int damageOnHurt = (int) json_object_get_number( frameObj, "damageOnHurt" );
            af->damageOnHurt = damageOnHurt;

            JSON_Object *boxesObj = json_object_get_object( frameObj, "boxes" );
            if ( boxesObj == NULL ) continue;

            AnimationFrameBoxes *afb = &a->frames[f].boxes;

            JSON_Object *collisionBoxObj = json_object_get_object( boxesObj, "collisionBox" );
            if ( collisionBoxObj != NULL ) {
                afb->collisionBox.x      = (float) json_object_get_number( collisionBoxObj, "x" );
                afb->collisionBox.y      = (float) json_object_get_number( collisionBoxObj, "y" );
                afb->collisionBox.width  = (float) json_object_get_number( collisionBoxObj, "width" );
                afb->collisionBox.height = (float) json_object_get_number( collisionBoxObj, "height" );
            }

            afb->hitboxCount = (int) json_object_get_number( boxesObj, "hitboxCount" );
            if ( afb->hitboxCount > 3 ) afb->hitboxCount = 3;
            JSON_Array *hitboxArray = json_object_get_array( boxesObj, "hitboxes" );
            if ( hitboxArray != NULL ) {
                for ( int i = 0; i < afb->hitboxCount; i++ ) {
                    JSON_Object *rectObj = json_array_get_object( hitboxArray, i );
                    if ( rectObj == NULL ) continue;
                    afb->hitboxes[i].x      = (float) json_object_get_number( rectObj, "x" );
                    afb->hitboxes[i].y      = (float) json_object_get_number( rectObj, "y" );
                    afb->hitboxes[i].width  = (float) json_object_get_number( rectObj, "width" );
                    afb->hitboxes[i].height = (float) json_object_get_number( rectObj, "height" );
                }
            }

            afb->hurtboxCount = (int) json_object_get_number( boxesObj, "hurtboxCount" );
            if ( afb->hurtboxCount > 3 ) afb->hurtboxCount = 3;
            JSON_Array *hurtboxArray = json_object_get_array( boxesObj, "hurtboxes" );
            if ( hurtboxArray != NULL ) {
                for ( int i = 0; i < afb->hurtboxCount; i++ ) {
                    JSON_Object *rectObj = json_array_get_object( hurtboxArray, i );
                    if ( rectObj == NULL ) continue;
                    afb->hurtboxes[i].x      = (float) json_object_get_number( rectObj, "x" );
                    afb->hurtboxes[i].y      = (float) json_object_get_number( rectObj, "y" );
                    afb->hurtboxes[i].width  = (float) json_object_get_number( rectObj, "width" );
                    afb->hurtboxes[i].height = (float) json_object_get_number( rectObj, "height" );
                }
            }

        }

    }

    json_value_free( rootValue );

}

static Vector2 fontStartDigits = { 17, 101 };
static Vector2 fontStartAUpper = { 29, 113 };
static Vector2 fontStartPUpper = { 17, 125 };
static Vector2 fontStartALower = { 29, 137 };
static Vector2 fontStartPLower = { 17, 149 };
static Vector2 fontDim = { 12, 10 };

void drawTextUsingFont( const char *text, int x, int y, float scale, int spacing ) {

    int size = strlen( text );
    int offset = 0;
    Vector2 start = { 0 };

    for ( int i = 0; i < size; i++ ) {

        char c = text[i];
        
        if ( c >= '0' && c <= '9' ) {
            offset = (int) ( c - '0' );
            start = fontStartDigits;
        } else if ( c >= 'A' && c <= 'O' ) {
            offset = (int) ( c - 'A' );
            start = fontStartAUpper;
        } else if ( c >= 'P' && c <= 'Z'  ) {
            offset = (int) ( c - 'P' );
            start = fontStartPUpper;
        } else if ( c >= 'a' && c <= 'o' ) {
            offset = (int) ( c - 'a' );
            start = fontStartALower;
        } else if ( c >= 'p' && c <= 'z'  ) {
            offset = (int) ( c - 'p' );
            start = fontStartPLower;
        } else {
            continue;
        }

        DrawTexturePro( 
            rm.fontsTexture,
            (Rectangle) { (int) ( start.x + (int) ( fontDim.x * offset ) ), start.y, fontDim.x, fontDim.y },
            (Rectangle) { x + ( fontDim.x * scale + spacing ) * i, y, fontDim.x * scale, fontDim.y * scale },
            (Vector2) { 0 },
            0.0f, 
            WHITE
        );

    }

}

Vector2 measureTextUsingFont( const char *text, float scale, int spacing ) {
    int len = strlen( text );
    return (Vector2) {
        len * fontDim.x * scale + ( len - 1 ) * spacing,
        fontDim.y * scale
    };
}

Rectangle getRectangleIntersection( Rectangle r1, Rectangle r2 ) {

    float left = fmaxf( r1.x, r2.x );
    float top = fmaxf( r1.y, r2.y );
    float right = fminf( r1.x + r1.width,  r2.x + r2.width  );
    float bottom = fminf( r1.y + r1.height, r2.y + r2.height );

    if ( right <= left || bottom <= top ) {
        return (Rectangle){ 0 };   // no intersection
    }

    return (Rectangle){
        .x = left,
        .y = top,
        .width  = right  - left,
        .height = bottom - top
    };

}