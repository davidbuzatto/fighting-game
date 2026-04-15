/**
 * @file PlayerInput.h
 * @author Prof. Dr. David Buzatto
 * @brief Player input processing and input buffer API.
 *
 * @copyright Copyright (c) 2026
 */
#pragma once

#include "Types.h"

void processInputAndFeedInputBuffer( Player *p, int currentFrame );
void addInputToPlayerInputBuffer( Player *p, InputType input, int currentFrame );
InputType peekAttackButton( Player *p, int currentFrame );
CommandInput *checkCommandInputs( Player *p, int currentFrame, InputType *outButton );
