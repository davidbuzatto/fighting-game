# Design: Refatoração do sistema de animação e estado do personagem

**Data:** 2026-03-23
**Projeto:** Fighting Game (Super Street Fighter 2 clone em C + raylib)
**Escopo:** Corrigir bugs no sistema de animação one-shot e unificar o controle de estado do personagem para suportar golpes básicos (LP como referência).

---

## Contexto e motivação

O protótipo já possui movimentação funcional (andar, agachar, pular) e um esboço de golpe fraco (LP). Dois problemas impedem que os golpes funcionem corretamente:

1. **Sistema de animação inconsistente:** A animação LP usa `stopAtLastFrame=true` + `runOnce=true` juntos, que são semânticas contraditórias. A lógica de detecção de fim de animação (`runnedAtLastOneFrame`) exige que a animação complete um ciclo inteiro e comece outro antes de ser marcada como terminada. O último frame tem `duration=0`, que era um workaround para "hold indefinido" via `stopAtLastFrame`.

2. **Estado do personagem dividido em três campos:** `PlayerState` (movimento), `PlayerMoveState` (ataque) e o flag `jumping` convivem na struct `Player`. O `processInputPlayer()` não bloqueia input de movimento durante um ataque, e as animações de movimento são atualizadas em paralelo com as de ataque.

---

## Design

### 1. Sistema de animação (`Animation.c` + `Types.h`)

**Remover** o field `runnedAtLastOneFrame` da struct `Animation`.

**Novo fluxo de `updateAnimation()`:**

```
1. Se anim->finished → retorna imediatamente (idempotente)
2. Acumula delta (em ms) no frameTimeCounter
3. Se frameTimeCounter >= duração do frame atual:
   a. Zera frameTimeCounter
   b. Avança currentFrame++
   c. Se stopAtLastFrame E currentFrame >= frameCount-1:
        → clamp currentFrame em frameCount-1
        → finished = true, retorna
   d. Se runOnce E currentFrame >= frameCount:
        → clamp currentFrame em frameCount-1
        → finished = true, retorna
   e. Aplica currentFrame %= frameCount (wrapping para loop)
```

**Nota sobre a assimetria dos checks 3c e 3d:**
- `stopAtLastFrame` usa `>= frameCount-1`: congela assim que o frame avança para o índice do último frame (o último frame é exibido a partir desse tick, e a duração é respeitada porque o avanço só ocorre após `frameTimeCounter >= duration`).
- `runOnce` usa `>= frameCount`: congela quando `currentFrame` avançaria além do array. Isso garante que o último frame (índice `frameCount-1`) seja exibido por sua duração completa antes de `finished` ser marcado.

**Semânticas resultantes:**
- `stopAtLastFrame=true, runOnce=false` → congela no último frame após sua duração (usado por pulos e agachamento)
- `runOnce=true, stopAtLastFrame=false` → toca uma vez, respeita duração de todos os frames, congela no último e marca finished (usado por ataques)
- `stopAtLastFrame=false, runOnce=false` → loop contínuo (usado por idle/walk)

**Impacto em `resetAnimation()`:** remover a linha que zera `runnedAtLastOneFrame`.

---

### 2. Estado unificado do personagem (`Types.h`)

**Remover:** enum `PlayerMoveState`, fields `moveState`, `lastMoveState`, `jumping` da struct `Player`.

**Novo enum `PlayerState`:**

```c
typedef enum PlayerState {
    PLAYER_STATE_IDLE,
    PLAYER_STATE_WALKING_FORWARD,
    PLAYER_STATE_WALKING_BACKWARD,
    PLAYER_STATE_CROUCHING,
    PLAYER_STATE_JUMPING_STRAIGHT,
    PLAYER_STATE_JUMPING_FORWARD,
    PLAYER_STATE_JUMPING_BACKWARD,
    PLAYER_STATE_LP,
    PLAYER_STATE_MP,
    PLAYER_STATE_HP,
    PLAYER_STATE_LK,
    PLAYER_STATE_MK,
    PLAYER_STATE_HK,
} PlayerState;
```

O tipo do pulo é determinado no momento do salto e codificado diretamente no estado, eliminando a necessidade de checar `vel.x` para resolver a animação.

**Struct `Player`:** remove `moveState`, `lastMoveState`, `jumping`. O `lastState` permanece (cobre todos os casos).

---

### 3. Lógica de transição de estado (`Player.c` — `processInputPlayer`)

Reestruturada com prioridade explícita via retorno antecipado:

```
Bloco 1 — Estado de ataque (LP/MP/HP/LK/MK/HK):
  - Atualiza animação do ataque
  - Se finished → state = IDLE, reset da animação
  - Retorna (bloqueia movimento e outros inputs)

Bloco 2 — Estado de pulo (JUMPING_STRAIGHT/FORWARD/BACKWARD):
  - Atualiza animação de pulo correspondente
  - Input de ataque no ar é descartado intencionalmente nesta iteração
  - Retorna (física continua em updatePlayer)

Bloco 3 — Chão (IDLE/WALKING/CROUCHING):
  - Se LP/MP/HP/LK/MK/HK pressionado:
      → Se state == CROUCHING: resetAnimation(&crouchingAnim)   ← verifica ANTES de sobrescrever state
      → resetAnimation() na animação do golpe correspondente
      → state = PLAYER_STATE_LP (etc), vel.x = 0
      → retorna
  - Se UP pressionado:
      → Se state == CROUCHING: resetAnimation(&crouchingAnim)   ← verifica ANTES de sobrescrever state
      → state = JUMPING_STRAIGHT/FORWARD/BACKWARD conforme vel.x atual
      → resetAnimation() na animação de pulo correspondente
      → vel.y = -jumpSpeed, vel.x conforme direção
      → retorna
  - Se DOWN → CROUCHING, vel.x = 0
      → Se state anterior != CROUCHING: resetAnimation(&crouchingAnim)
  - Se RIGHT → WALKING_FORWARD, vel.x = forwardSpeed
  - Se LEFT → WALKING_BACKWARD, vel.x = -backwardSpeed
  - Senão → IDLE, vel.x = 0
  - Atualiza animação de chão
```

**`getPlayerCurrentAnimationFrame()`** vira um único `switch(player->state)` sem checagem de `vel.x` ou `jumping`.

---

### 4. Dados da animação LP (`initializePlayerRyu` em `Player.c`)

```
lpAnim:
  stopAtLastFrame = false
  runOnce = true
  Frame 0: (wind-up)     duration = 50ms
  Frame 1: (impacto)     duration = 80ms
  Frame 2: (recuperação) duration = 50ms   ← duração real, não mais 0
```

Os outros golpes (MP, HP, LK, MK, HK) mantêm seus sprites placeholder mas recebem `stopAtLastFrame=false` e `runOnce=true` para ficarem consistentes com a nova lógica.

---

### 5. Resolução de aterrissagem (`GameWorld.c` — `resolveCollisionPlayerStage`)

O código atual chama `resetPlayerAnimations()` e zera o flag `jumping` quando o personagem toca o chão. Com o estado unificado, essa lógica passa a:
1. Verificar se `player->state` é `JUMPING_*`
2. Chamar `resetPlayerAnimations(player)` (mantém o reset completo para garantir que a animação de pulo esteja limpa na próxima vez)
3. Setar `player->state = PLAYER_STATE_IDLE`

O reset completo de animações na aterrissagem é mantido propositalmente para evitar que animações de pulo tenham `finished=true` na segunda vez que o jogador pular.

---

## Arquivos afetados

| Arquivo | Mudança |
|---|---|
| `src/include/Types.h` | Remove `PlayerMoveState`, remove fields da struct, novo `PlayerState` |
| `src/Animation.c` | Reescreve `updateAnimation()`, remove `runnedAtLastOneFrame` de `resetAnimation()` |
| `src/Player.c` | Reestrutura `processInputPlayer()`, `getPlayerCurrentAnimationFrame()`, dados LP, inicialização dos outros golpes |
| `src/GameWorld.c` | Atualiza `resolveCollisionPlayerStage()` para novo estado de pulo |
| `src/Player.c` (`updatePlayer`) | **Sem mudança** — a física (gravidade, velocidade) é aplicada incondicionalmente; `jumping` não é verificado lá |

---

## O que não muda

- Sprites e coordenadas das animações existentes (idle, walk, crouch, jump)
- Sistema de câmera e física
- `flipPlayers()` e orientação dos personagens
- Dados de sprite dos golpes MP/HP/LK/MK/HK (serão implementados posteriormente)
