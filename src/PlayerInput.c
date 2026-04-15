/**
 * @file PlayerInput.c
 * @author Prof. Dr. David Buzatto
 * @brief Player input processing and input buffer implementation.
 *
 * @copyright Copyright (c) 2026
 */
#include <stdlib.h>

#include "raylib/raylib.h"

#include "PlayerInput.h"
#include "Types.h"
#include "Utils.h"

InputType peekAttackButton( Player *p, int currentFrame ) {
    if ( p->inputBufferSize == 0 ) {
        return INPUT_TYPE_NEUTRAL;
    }
    InputBufferEntry *last = &p->inputBuffer[p->inputBufferTail % PLAYER_INPUT_BUFFER_SIZE];
    if ( last->frame == currentFrame && isAttackInput( last->type ) ) {
        return last->type;
    }
    return INPUT_TYPE_NEUTRAL;
}

/**
 * Searches the input buffer for a completed command input.
 * Returns a pointer to the matched CommandInput, or NULL if no match.
 * outButton receives the attack button that completed the command (LP, MP, HP, LK, MK, HK).
 *
 * Matching logic:
 * 1. The most recent buffer entry (this frame) must be an attack button of the required type
 * 2. Searching backwards from the button, the directional sequence must appear in order
 * 3. Extra inputs between sequence steps are tolerated (leniency)
 * 4. All entries must be within the command's frame window
 * 5. Directionals are mirrored if the player is facing left
 *
 * Commands with longer sequences are checked first to avoid
 * a shorter command (hadouken) eating a longer one (shoryuken).
 */
CommandInput *checkCommandInputs( Player *p, int currentFrame, InputType *outButton ) {

    if ( p->inputBufferSize < 2 ) {
        return NULL;
    }

    // the most recent entry must be an attack button added this frame
    InputBufferEntry *buttonEntry = &p->inputBuffer[p->inputBufferTail % PLAYER_INPUT_BUFFER_SIZE];
    if ( buttonEntry->frame != currentFrame || !isAttackInput( buttonEntry->type ) ) {
        return NULL;
    }

    InputType button = buttonEntry->type;

    // try each command (longer sequences first - sort by sequenceLength desc would be ideal,
    // but for now the order in the commands array determines priority)
    for ( int c = 0; c < p->commandCount; c++ ) {

        CommandInput *cmd = &p->commands[c];

        // check button type requirement
        if ( cmd->requiresPunch && !isPunchInput( button ) ) continue;
        if ( cmd->requiresKick && !isKickInput( button ) ) continue;

        // search backwards through buffer for the directional sequence
        int seqIdx = cmd->sequenceLength - 1;  // start matching from end of sequence
        bool matched = true;

        for ( int i = p->inputBufferTail - 1; i >= p->inputBufferHead && seqIdx >= 0; i-- ) {

            InputBufferEntry *entry = &p->inputBuffer[i % PLAYER_INPUT_BUFFER_SIZE];

            // check time window: entry must be within frameWindow of the button press
            if ( buttonEntry->frame - entry->frame > cmd->frameWindow ) {
                matched = false;
                break;
            }

            // get the expected directional (mirror if facing left)
            InputType expected = cmd->sequence[seqIdx];
            if ( !p->lookingRight ) {
                expected = mirrorDirectional( expected );
            }

            if ( entry->type == expected ) {
                seqIdx--;
            }

            // else: tolerate extra inputs (leniency) — just keep searching

        }

        if ( matched && seqIdx < 0 ) {
            *outButton = button;
            return cmd;
        }

    }

    return NULL;

}

void processInputAndFeedInputBuffer( Player *p, int currentFrame ) {

    // compute current directional state from held keys
    bool rightDown = IsKeyDown( p->kb.right.key ) || isGamepadButtonDown( p->gamepadId, p->kb.right.gamepadButton );
    bool leftDown  = IsKeyDown( p->kb.left.key )  || isGamepadButtonDown( p->gamepadId, p->kb.left.gamepadButton );
    bool downDown  = IsKeyDown( p->kb.down.key )  || isGamepadButtonDown( p->gamepadId, p->kb.down.gamepadButton );
    bool upDown    = IsKeyDown( p->kb.up.key )    || isGamepadButtonDown( p->gamepadId, p->kb.up.gamepadButton );

    InputType currentDir = INPUT_TYPE_NEUTRAL;

    if ( rightDown && downDown ) {
        currentDir = INPUT_TYPE_RIGHT_DOWN;
    } else if ( leftDown && downDown ) {
        currentDir = INPUT_TYPE_LEFT_DOWN;
    } else if ( rightDown && upDown ) {
        currentDir = INPUT_TYPE_RIGHT_UP;
    } else if ( leftDown && upDown ) {
        currentDir = INPUT_TYPE_LEFT_UP;
    } else if ( rightDown ) {
        currentDir = INPUT_TYPE_RIGHT;
    } else if ( downDown ) {
        currentDir = INPUT_TYPE_DOWN;
    } else if ( leftDown ) {
        currentDir = INPUT_TYPE_LEFT;
    } else if ( upDown ) {
        currentDir = INPUT_TYPE_UP;
    }

    // register only on state transitions (including neutral)
    if ( currentDir != p->lastDirectionalState ) {
        addInputToPlayerInputBuffer( p, currentDir, currentFrame );
        p->lastDirectionalState = currentDir;
    }

    // buttons: independent from directionals (both can register in the same frame)
    if ( IsKeyPressed( p->kb.lp.key ) || isGamepadButtonPressed( p->gamepadId, p->kb.lp.gamepadButton ) ) {
        addInputToPlayerInputBuffer( p, p->kb.lp.type, currentFrame );
    } else if ( IsKeyPressed( p->kb.mp.key ) || isGamepadButtonPressed( p->gamepadId, p->kb.mp.gamepadButton ) ) {
        addInputToPlayerInputBuffer( p, p->kb.mp.type, currentFrame );
    } else if ( IsKeyPressed( p->kb.hp.key ) || isGamepadButtonPressed( p->gamepadId, p->kb.hp.gamepadButton ) ) {
        addInputToPlayerInputBuffer( p, p->kb.hp.type, currentFrame );
    } else if ( IsKeyPressed( p->kb.lk.key ) || isGamepadButtonPressed( p->gamepadId, p->kb.lk.gamepadButton ) ) {
        addInputToPlayerInputBuffer( p, p->kb.lk.type, currentFrame );
    } else if ( IsKeyPressed( p->kb.mk.key ) || isGamepadButtonPressed( p->gamepadId, p->kb.mk.gamepadButton ) ) {
        addInputToPlayerInputBuffer( p, p->kb.mk.type, currentFrame );
    } else if ( IsKeyPressed( p->kb.hk.key ) || isGamepadButtonPressed( p->gamepadId, p->kb.hk.gamepadButton ) ) {
        addInputToPlayerInputBuffer( p, p->kb.hk.type, currentFrame );
    }

}

void addInputToPlayerInputBuffer( Player *p, InputType input, int currentFrame ) {

    if ( p->inputBufferSize == 0 ) {
        p->inputBufferHead = 0;
        p->inputBufferTail = 0;
        p->inputBufferSize++;
        p->inputBuffer[p->inputBufferTail] = (InputBufferEntry) { input, currentFrame };
    } else {
        if ( p->inputBufferSize >= PLAYER_INPUT_BUFFER_SIZE ) {
            p->inputBufferHead++;
        } else {
            p->inputBufferSize++;
        }
        p->inputBufferTail++;
        p->inputBuffer[p->inputBufferTail % PLAYER_INPUT_BUFFER_SIZE] = (InputBufferEntry) { input, currentFrame };
    }
}
