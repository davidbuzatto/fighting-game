/**
 * @file Player.c
 * @author Prof. Dr. David Buzatto
 * @brief Anchor translation unit kept after the refactoring.
 *
 * All Player functionality has been split into smaller units:
 *   - PlayerPallete.c     : color palette swap
 *   - PlayerAnimation.c   : animation helpers (getter, reset, flip, distance)
 *   - PlayerRender.c      : rendering (player, boxes, shadow, onion, projectile)
 *   - PlayerInput.c       : input buffer and command recognition
 *   - PlayerCollision.c   : player vs. opponent contact resolution
 *   - PlayerInit.c        : create/destroy and Ryu/Ken initialization
 *   - PlayerFSM.c         : processInputPlayer + updatePlayer (FSM)
 *
 * This file exists only as an anchor to minimize changes in the build
 * system (src globbing).
 *
 * @copyright Copyright (c) 2026
 */

// marker to avoid ISO C "empty translation unit" warning
typedef int player_c_anchor_unit_t;
