# Animation & State Refactor Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Unificar o estado do personagem em um único enum e corrigir a lógica de animações one-shot para que golpes rodem do início ao fim e retornem ao idle corretamente.

**Architecture:** O `PlayerState` absorve `PlayerMoveState` e o flag `jumping`. O `updateAnimation()` é reescrito para detectar fim de ciclo de forma direta (sem `runnedAtLastOneFrame`). O `processInputPlayer()` usa blocos de prioridade com retorno antecipado para garantir que ataques bloqueiem todo input de movimento.

**Tech Stack:** C99, raylib. Build: `make compile` (na raiz do projeto, requer bash/Git Bash no Windows). Sem framework de testes unitários — verificação via compilação + execução manual do jogo.

---

## Mapa de arquivos

| Arquivo | O que muda |
|---|---|
| `src/include/Types.h` | Remove `PlayerMoveState`; expande `PlayerState`; remove `runnedAtLastOneFrame` de `Animation`; remove `moveState`, `lastMoveState`, `jumping` de `Player` |
| `src/Animation.c` | Reescreve `updateAnimation()`; simplifica `resetAnimation()` |
| `src/Player.c` | Atualiza `initializePlayerRyu()`, reescreve `processInputPlayer()` e `getPlayerCurrentAnimationFrame()` |
| `src/GameWorld.c` | Atualiza `resolveCollisionPlayerStage()`; remove argumento `gravity` dos call sites de `processInputPlayer` |
| `src/include/Player.h` | Remove parâmetro `gravity` da declaração de `processInputPlayer` |

---

## Task 1: Atualizar `Types.h`

**Files:**
- Modify: `src/include/Types.h`

- [ ] **Step 1: Substituir o enum `PlayerState` e remover `PlayerMoveState`**

Substituir o bloco atual (linhas 5-20) por:

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

O enum `PlayerMoveState` (linhas 12-20) é **removido inteiramente**.

- [ ] **Step 2: Remover `runnedAtLastOneFrame` da struct `Animation`**

A struct `Animation` (linhas 28-37) passa a ser:

```c
typedef struct Animation {
    AnimationFrame *frames;
    int frameCount;
    int currentFrame;
    int frameTimeCounter;
    bool stopAtLastFrame;
    bool runOnce;
    bool finished;
} Animation;
```

`runnedAtLastOneFrame` é removido.

- [ ] **Step 3: Atualizar a struct `Player`**

Remover os fields `moveState`, `lastMoveState` e `jumping`. A struct passa a ser:

```c
typedef struct Player {

    Texture2D *texture;
    Vector2 pos;
    Vector2 dim;

    Vector2 vel;
    float forwardSpeed;
    float backwardSpeed;
    float jumpSpeed;

    Animation idleAnim;
    Animation forwardAnim;
    Animation backwardAnim;
    Animation straightJumpAnim;
    Animation forwardJumpAnim;
    Animation backwardJumpAnim;
    Animation crouchingAnim;

    Animation lpAnim;
    Animation mpAnim;
    Animation hpAnim;
    Animation lkAnim;
    Animation mkAnim;
    Animation hkAnim;

    Animation *animations[50];
    int animationCount;

    PlayerState state;
    PlayerState lastState;

    bool lookingRight;

    PlayerKeyBindings kb;

} Player;
```

- [ ] **Step 4: Compilar para ver os erros de compilação**

```bash
make compile 2>&1 | head -60
```

Esperado: erros de compilação em `Animation.c` e `Player.c` referenciando `runnedAtLastOneFrame`, `jumping`, `moveState`, `lastMoveState`, `PLAYER_MOVE_STATE_*`. Isso é esperado — as tarefas seguintes corrigem cada arquivo.

---

## Task 2: Reescrever `Animation.c`

**Files:**
- Modify: `src/Animation.c`

- [ ] **Step 1: Reescrever `updateAnimation()`**

Substituir a função inteira (linhas 15-37) por:

```c
void updateAnimation( Animation *anim, float delta ) {

    if ( anim->finished ) {
        return;
    }

    anim->frameTimeCounter += (int) ( delta * 1000 );

    AnimationFrame *frame = getAnimationCurrentFrame( anim );

    if ( anim->frameTimeCounter >= frame->duration ) {
        anim->frameTimeCounter = 0;
        anim->currentFrame++;

        if ( anim->stopAtLastFrame && anim->currentFrame >= anim->frameCount ) {
            anim->currentFrame = anim->frameCount - 1;
            anim->finished = true;
            return;
        }

        if ( anim->runOnce && anim->currentFrame >= anim->frameCount ) {
            anim->currentFrame = anim->frameCount - 1;
            anim->finished = true;
            return;
        }

        anim->currentFrame %= anim->frameCount;
    }

}
```

**Por que os dois checks usam `>= frameCount`:**
Ambos aguardam o `currentFrame` avançar ALÉM do array (i.e., após a duração do último frame ter expirado). Isso garante que o último frame seja exibido por sua duração completa antes de `finished=true` ser marcado. A diferença entre os dois flags é semântica, não de boundary: `stopAtLastFrame` e `runOnce` expressam intenções distintas para o chamador, mas a lógica de detecção de fim é a mesma.

- [ ] **Step 2: Simplificar `resetAnimation()`**

Substituir a função inteira (linhas 47-53) por:

```c
void resetAnimation( Animation *anim ) {
    anim->currentFrame = 0;
    anim->frameTimeCounter = 0;
    anim->finished = false;
}
```

- [ ] **Step 3: Compilar**

```bash
make compile 2>&1 | grep "Animation.c"
```

Esperado: sem erros em `Animation.c`. Erros em `Player.c` e `GameWorld.c` ainda existem — são resolvidos nas próximas tarefas.

---

## Task 3: Atualizar `initializePlayerRyu()` em `Player.c`

**Files:**
- Modify: `src/Player.c` (função `initializePlayerRyu`, linhas 19-223)

- [ ] **Step 1: Remover inicialização dos campos removidos**

Remover as linhas que inicializam `jumping`, `moveState` e `lastMoveState` (linhas 32-34 aproximadamente):

```c
// REMOVER estas três linhas:
p->moveState = PLAYER_MOVE_STATE_NONE;
p->lastMoveState = PLAYER_MOVE_STATE_NONE;
p->jumping = false;
```

- [ ] **Step 2: Corrigir dados da animação LP**

Localizar o bloco de `lpAnim` (linhas 140-150) e aplicar as mudanças:

```c
p->lpAnim.frameCount = 3;
p->lpAnim.currentFrame = 0;
p->lpAnim.frameTimeCounter = 0.0f;
p->lpAnim.stopAtLastFrame = false;   // era true
p->lpAnim.runOnce = true;
p->lpAnim.runnedAtLastOneFrame = false;  // será removido no próximo passo
p->lpAnim.finished = false;
createAnimationFrames( &p->lpAnim, p->lpAnim.frameCount );
p->lpAnim.frames[0] = (AnimationFrame) { (Rectangle) {   1, 1087, -112, 96 }, 50, (Vector2) { 20, 0 } };
p->lpAnim.frames[1] = (AnimationFrame) { (Rectangle) { 114, 1087, -112, 96 }, 80, (Vector2) { 20, 0 } };
p->lpAnim.frames[2] = (AnimationFrame) { (Rectangle) { 114, 1087, -112, 96 }, 50, (Vector2) { 20, 0 } };
//                                                                              ^^
//                                       era 0 — agora tem duração real de 50ms
```

- [ ] **Step 3: Corrigir `stopAtLastFrame` dos outros golpes**

Para `mpAnim`, `hpAnim`, `lkAnim`, `mkAnim` e `hkAnim`, alterar `stopAtLastFrame = true` para `stopAtLastFrame = false` em cada bloco. O `runOnce = true` já está correto em todos.

- [ ] **Step 4: Remover todas as referências a `runnedAtLastOneFrame` na inicialização**

Em toda a função `initializePlayerRyu()`, remover as linhas do tipo:
```c
p->xAnim.runnedAtLastOneFrame = false;
```
Há uma dessas linhas em cada bloco de inicialização de animação (idle, forward, backward, todos os pulos, crouching, e todos os golpes). Remover todas.

- [ ] **Step 5: Compilar**

```bash
make compile 2>&1 | grep "Player.c"
```

Esperado: erros restantes apenas em `processInputPlayer()` e `getPlayerCurrentAnimationFrame()` — serão corrigidos nas próximas tarefas. Não deve haver erros na função de inicialização.

---

## Task 4: Reescrever `processInputPlayer()` em `Player.c`

**Files:**
- Modify: `src/Player.c` (função `processInputPlayer`, linhas 272-396)

- [ ] **Step 1: Substituir a função inteira pelo novo código**

```c
void processInputPlayer( Player *player, float delta ) {

    // Bloco 1 — Ataque em progresso: bloqueia todo input
    Animation *activeAnim = NULL;
    switch ( player->state ) {
        case PLAYER_STATE_LP: activeAnim = &player->lpAnim; break;
        case PLAYER_STATE_MP: activeAnim = &player->mpAnim; break;
        case PLAYER_STATE_HP: activeAnim = &player->hpAnim; break;
        case PLAYER_STATE_LK: activeAnim = &player->lkAnim; break;
        case PLAYER_STATE_MK: activeAnim = &player->mkAnim; break;
        case PLAYER_STATE_HK: activeAnim = &player->hkAnim; break;
        default: break;
    }

    if ( activeAnim != NULL ) {
        player->lastState = player->state;   // registra o estado de ataque ANTES de transicionar
        updateAnimation( activeAnim, delta );
        if ( activeAnim->finished ) {
            player->state = PLAYER_STATE_IDLE;
            resetAnimation( activeAnim );
        }
        return;
    }

    // Bloco 2 — Pulo em progresso: atualiza animação, bloqueia input de ataque
    if ( player->state == PLAYER_STATE_JUMPING_STRAIGHT ||
         player->state == PLAYER_STATE_JUMPING_FORWARD  ||
         player->state == PLAYER_STATE_JUMPING_BACKWARD ) {
        switch ( player->state ) {
            case PLAYER_STATE_JUMPING_STRAIGHT:
                updateAnimation( &player->straightJumpAnim, delta );
                break;
            case PLAYER_STATE_JUMPING_FORWARD:
                if ( player->lookingRight ) {
                    updateAnimation( &player->forwardJumpAnim, delta );
                } else {
                    updateAnimation( &player->backwardJumpAnim, delta );
                }
                break;
            case PLAYER_STATE_JUMPING_BACKWARD:
                if ( player->lookingRight ) {
                    updateAnimation( &player->backwardJumpAnim, delta );
                } else {
                    updateAnimation( &player->forwardJumpAnim, delta );
                }
                break;
            default:
                break;
        }
        // Ataque no ar descartado intencionalmente nesta iteração
        player->lastState = player->state;
        return;
    }

    // Bloco 3 — Chão: ataques têm prioridade sobre movimento

    // 3a. Detecção de ataque
    PlayerState attackState = PLAYER_STATE_IDLE;
    Animation *attackAnim = NULL;

    if ( IsKeyPressed( player->kb.lp ) ) {
        attackState = PLAYER_STATE_LP;  attackAnim = &player->lpAnim;
    } else if ( IsKeyPressed( player->kb.mp ) ) {
        attackState = PLAYER_STATE_MP;  attackAnim = &player->mpAnim;
    } else if ( IsKeyPressed( player->kb.hp ) ) {
        attackState = PLAYER_STATE_HP;  attackAnim = &player->hpAnim;
    } else if ( IsKeyPressed( player->kb.lk ) ) {
        attackState = PLAYER_STATE_LK;  attackAnim = &player->lkAnim;
    } else if ( IsKeyPressed( player->kb.mk ) ) {
        attackState = PLAYER_STATE_MK;  attackAnim = &player->mkAnim;
    } else if ( IsKeyPressed( player->kb.hk ) ) {
        attackState = PLAYER_STATE_HK;  attackAnim = &player->hkAnim;
    }

    if ( attackAnim != NULL ) {
        if ( player->state == PLAYER_STATE_CROUCHING ) {
            resetAnimation( &player->crouchingAnim );
        }
        resetAnimation( attackAnim );
        player->vel.x = 0.0f;
        player->state = attackState;
        player->lastState = player->state;
        return;
    }

    // 3b. Pulo (IsKeyPressed: tap, não hold — evita pulo duplo ao aterrissar)
    if ( IsKeyPressed( player->kb.up ) && player->state != PLAYER_STATE_CROUCHING ) {
        if ( player->vel.x == 0.0f ) {
            player->vel.y = -player->jumpSpeed;
            resetAnimation( &player->straightJumpAnim );
            player->state = PLAYER_STATE_JUMPING_STRAIGHT;
        } else if ( player->vel.x > 0.0f ) {
            player->vel.y = -player->jumpSpeed;
            player->vel.x = player->forwardSpeed * 1.6f;
            resetAnimation( &player->forwardJumpAnim );
            player->state = PLAYER_STATE_JUMPING_FORWARD;
        } else {
            player->vel.y = -player->jumpSpeed;
            player->vel.x = -player->backwardSpeed * 2.0f;
            resetAnimation( &player->backwardJumpAnim );
            player->state = PLAYER_STATE_JUMPING_BACKWARD;
        }
        player->lastState = player->state;
        return;
    }

    // 3c. Movimento de chão
    if ( IsKeyDown( player->kb.down ) ) {
        if ( player->state != PLAYER_STATE_CROUCHING ) {
            resetAnimation( &player->crouchingAnim );
        }
        player->vel.x = 0.0f;
        player->state = PLAYER_STATE_CROUCHING;
    } else if ( IsKeyDown( player->kb.right ) ) {
        player->vel.x = player->forwardSpeed;
        player->state = PLAYER_STATE_WALKING_FORWARD;
    } else if ( IsKeyDown( player->kb.left ) ) {
        player->vel.x = -player->backwardSpeed;
        player->state = PLAYER_STATE_WALKING_BACKWARD;
    } else {
        player->vel.x = 0.0f;
        player->state = PLAYER_STATE_IDLE;
    }

    // 3d. Atualiza animação de chão
    switch ( player->state ) {
        case PLAYER_STATE_IDLE:
            updateAnimation( &player->idleAnim, delta );
            break;
        case PLAYER_STATE_WALKING_FORWARD:
            if ( player->lookingRight ) {
                updateAnimation( &player->forwardAnim, delta );
            } else {
                updateAnimation( &player->backwardAnim, delta );
            }
            break;
        case PLAYER_STATE_WALKING_BACKWARD:
            if ( player->lookingRight ) {
                updateAnimation( &player->backwardAnim, delta );
            } else {
                updateAnimation( &player->forwardAnim, delta );
            }
            break;
        case PLAYER_STATE_CROUCHING:
            updateAnimation( &player->crouchingAnim, delta );
            break;
        default:
            break;
    }

    player->lastState = player->state;

}
```

**Nota sobre `IsKeyPressed` para pulo:** o código original usava `IsKeyDown` com o guard `!player->jumping`. Com o estado unificado, o guard implícito é estar em Bloco 3, mas `IsKeyDown` ainda causaria pulo imediato ao aterrissar se UP fosse mantido. `IsKeyPressed` é o comportamento correto para Street Fighter.

**Nota sobre o parâmetro `gravity`:** foi removido da assinatura (não é usado na nova implementação). O passo seguinte atualiza a declaração em `Player.h` e os call sites em `GameWorld.c`.

- [ ] **Step 2: Atualizar declaração em `Player.h`**

Localizar a declaração de `processInputPlayer` em `src/include/Player.h` e remover o parâmetro `gravity`:

```c
// antes:
void processInputPlayer( Player *player, float gravity, float delta );
// depois:
void processInputPlayer( Player *player, float delta );
```

- [ ] **Step 3: Compilar**

```bash
make compile 2>&1 | grep "Player.c"
```

Esperado: sem erros em `Player.c`. Haverá erros em `GameWorld.c` sobre número incorreto de argumentos — resolvidos na Task 6.

---

## Task 5: Reescrever `getPlayerCurrentAnimationFrame()` em `Player.c`

**Files:**
- Modify: `src/Player.c` (função `getPlayerCurrentAnimationFrame`, linhas 416-470)

- [ ] **Step 1: Substituir a função inteira pelo switch unificado**

```c
AnimationFrame *getPlayerCurrentAnimationFrame( Player *player ) {

    switch ( player->state ) {
        case PLAYER_STATE_IDLE:
            return getAnimationCurrentFrame( &player->idleAnim );
        case PLAYER_STATE_WALKING_FORWARD:
            if ( player->lookingRight ) {
                return getAnimationCurrentFrame( &player->forwardAnim );
            }
            return getAnimationCurrentFrame( &player->backwardAnim );
        case PLAYER_STATE_WALKING_BACKWARD:
            if ( player->lookingRight ) {
                return getAnimationCurrentFrame( &player->backwardAnim );
            }
            return getAnimationCurrentFrame( &player->forwardAnim );
        case PLAYER_STATE_CROUCHING:
            return getAnimationCurrentFrame( &player->crouchingAnim );
        case PLAYER_STATE_JUMPING_STRAIGHT:
            return getAnimationCurrentFrame( &player->straightJumpAnim );
        case PLAYER_STATE_JUMPING_FORWARD:
            if ( player->lookingRight ) {
                return getAnimationCurrentFrame( &player->forwardJumpAnim );
            }
            return getAnimationCurrentFrame( &player->backwardJumpAnim );
        case PLAYER_STATE_JUMPING_BACKWARD:
            if ( player->lookingRight ) {
                return getAnimationCurrentFrame( &player->backwardJumpAnim );
            }
            return getAnimationCurrentFrame( &player->forwardJumpAnim );
        case PLAYER_STATE_LP:
            return getAnimationCurrentFrame( &player->lpAnim );
        case PLAYER_STATE_MP:
            return getAnimationCurrentFrame( &player->mpAnim );
        case PLAYER_STATE_HP:
            return getAnimationCurrentFrame( &player->hpAnim );
        case PLAYER_STATE_LK:
            return getAnimationCurrentFrame( &player->lkAnim );
        case PLAYER_STATE_MK:
            return getAnimationCurrentFrame( &player->mkAnim );
        case PLAYER_STATE_HK:
            return getAnimationCurrentFrame( &player->hkAnim );
    }

    return NULL;

}
```

- [ ] **Step 2: Compilar**

```bash
make compile 2>&1 | grep "Player.c"
```

Esperado: sem erros em `Player.c`.

---

## Task 6: Atualizar `resolveCollisionPlayerStage()` em `GameWorld.c`

**Files:**
- Modify: `src/GameWorld.c` (função `resolveCollisionPlayerStage`, linhas 186-203)

- [ ] **Step 1: Substituir o bloco de detecção de aterrissagem**

O bloco atual (linhas 194-201):
```c
if ( player->pos.y + player->dim.y > gw->floor.y ) {
    player->pos.y = gw->floor.y - player->dim.y;
    player->vel.y = 0.0f;
    if ( player->jumping ) {
        player->jumping = false;
        resetPlayerAnimations( player );
    }
}
```

Substituir por:
```c
if ( player->pos.y + player->dim.y > gw->floor.y ) {
    player->pos.y = gw->floor.y - player->dim.y;
    player->vel.y = 0.0f;
    if ( player->state == PLAYER_STATE_JUMPING_STRAIGHT ||
         player->state == PLAYER_STATE_JUMPING_FORWARD  ||
         player->state == PLAYER_STATE_JUMPING_BACKWARD ) {
        resetPlayerAnimations( player );
        player->state = PLAYER_STATE_IDLE;
    }
}
```

`resetPlayerAnimations()` é mantido para garantir que animações de pulo tenham `finished=false` e `currentFrame=0` na próxima vez que o jogador pular.

- [ ] **Step 2: Atualizar call sites de `processInputPlayer` em `GameWorld.c`**

Localizar as duas chamadas em `updateGameWorld()` (aproximadamente linhas 130-131) e remover o argumento `gw->gravity`:

```c
// antes:
processInputPlayer( gw->player1, gw->gravity, delta );
processInputPlayer( gw->player2, gw->gravity, delta );

// depois:
processInputPlayer( gw->player1, delta );
processInputPlayer( gw->player2, delta );
```

- [ ] **Step 3: Compilar projeto completo**

```bash
make compile 2>&1
```

Esperado: **zero erros e zero warnings**.

---

## Task 7: Verificação manual do comportamento

- [ ] **Step 1: Executar o jogo**

```bash
make run
```

- [ ] **Step 2: Verificar soco fraco (LP) — Player 1**

Controles Player 1: movimento com setas, LP com Numpad 4.

Comportamento esperado:
1. Pressionar LP: animação de soco toca (~180ms total: 50+80+50ms) e retorna ao idle
2. Durante o soco: movimento com setas **não funciona** (personagem fica parado)
3. Após o soco: personagem responde normalmente ao movimento
4. Pressionar LP enquanto segura direita: soca parado, depois retoma caminhada
5. Pressionar LP repetidamente: cada pressão dispara uma animação completa (não fica preso)

- [ ] **Step 3: Verificar agachamento**

1. Segurar DOWN: animação de agachamento toca e para no último frame (comportamento correto para `stopAtLastFrame`)
2. Soltar DOWN: personagem volta ao idle normalmente
3. Segurar DOWN → soltar → segurar DOWN novamente: animação de agachar reinicia (não pula para o último frame)

- [ ] **Step 4: Verificar pulos**

1. Pulo vertical: pressionar UP sem movimento — sobe e desce com animação correta
2. Pulo forward: pressionar UP com RIGHT — arco para frente
3. Segundo pulo imediato: aterrissar e pressionar UP imediatamente — **não** deve pular de novo se UP estava pressionado antes de aterrissar (comportamento de `IsKeyPressed`)
4. Segundo pulo após aterrissar: pressionar UP depois de aterrissar — deve funcionar normalmente

- [ ] **Step 5: Verificar Player 2**

Mesmos testes com Player 2 (Ken): controles WASD + T para LP.

---

## Referência rápida: semânticas de animação após o refactor

| `stopAtLastFrame` | `runOnce` | Comportamento | Usado por |
|---|---|---|---|
| `false` | `false` | Loop contínuo | idle, walk |
| `true` | `false` | Congela no último frame | jump, crouch |
| `false` | `true` | Toca uma vez, para no último frame | LP, MP, HP, LK, MK, HK |
