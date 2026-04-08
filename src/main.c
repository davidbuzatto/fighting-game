/**
 * @file main.c
 * @author Prof. Dr. David Buzatto
 * @brief Main function and logic for the game. Fighting game prototype.
 * 
 * @copyright Copyright (c) 2026
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "GameWindow.h"

int main( void ) {

    GameWindow *gameWindow = createGameWindow(
        896,             // width
        672,             // height
        "Fighting Game", // title
        60,              // target FPS
        false,           // antialiasing
        false,           // resizable
        false,           // full screen
        false,           // undecorated
        false,           // always on top
        false,           // invisible background
        false,           // always run
        true,            // load resources
        true             // init audio
    );

    initGameWindow( gameWindow );

    return 0;

}