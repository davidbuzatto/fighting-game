/**
 * @file Player.c
 * @author Prof. Dr. David Buzatto
 * @brief Anchor translation unit kept after the refactoring.
 *
 * All Player functionality has been split into smaller units:
 *   - PlayerPallete.c     : paleta de cores
 *   - PlayerAnimation.c   : utilitários de animação (getter, reset, flip, distance)
 *   - PlayerRender.c      : desenho (player, boxes, shadow, onion, projétil)
 *   - PlayerInput.c       : input buffer e reconhecimento de comandos
 *   - PlayerCollision.c   : resolução de contato player vs. oponente
 *   - PlayerInit.c        : create/destroy e inicialização de Ryu/Ken
 *   - PlayerFSM.c         : processInputPlayer + updatePlayer (FSM)
 *
 * Este arquivo existe apenas como âncora para minimizar alterações no
 * sistema de build (globbing de src).
 *
 * @copyright Copyright (c) 2026
 */

// marker to avoid ISO C "empty translation unit" warning
typedef int player_c_anchor_unit_t;
