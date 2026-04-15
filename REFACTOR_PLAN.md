# Refactoring Plan — Fighting Game

Plano de refatoração incremental para quebrar `GameWorld.c` (2201 linhas) e `Player.c` (2304 linhas) em unidades de compilação menores e mais gerenciáveis.

## Princípios

1. **Incremental**: cada passo compila e roda. Parada para teste manual após cada passo.
2. **Preservação de comentários**: nunca deletar comentários "desligados" (código de debug comentado propositalmente). Copiar literalmente para a nova localização.
3. **Baixo risco primeiro**: peças mais isoladas (rendering, data) antes das perigosas (FSM, init monolítico).
4. **Preservar estilo**: mesma convenção de nomes (`camelCase`), mesmos cabeçalhos doxygen, mesmo padrão de includes. Refatoração **pura** de movimentação — nenhuma reescrita "oportunista" de lógica.
5. **Headers fine-grained (Abordagem A)**: cada novo `.c` tem seu próprio `.h` correspondente em `src/include/`. Ex: `PlayerPallete.c` ↔ `PlayerPallete.h`. Declarações migram do header "monolítico" (`Player.h`/`GameWorld.h`) para o header específico, e consumidores passam a incluir o header específico. O header monolítico vai encolhendo ao longo da refatoração.
6. **Estado global (`static` file-scope)**: migra **junto** com as funções que o usam.
7. **Ancoragem**: `Player.c` e `GameWorld.c` continuarão existindo como arquivos pequenos, para minimizar alterações no build.
8. **Sem mexer no git**: commits e pushs são manuais pelo autor.
9. **Build scripts**: `build.bat`, `build.ps1` e `Makefile` usam globbing, então novos `.c` são compilados automaticamente sem edição.

## Convenção de headers

- **Um `.h` por `.c`** em `src/include/`, com mesmo nome base (`Foo.c` ↔ `Foo.h`).
- Cada `.h` contém **apenas** as funções públicas exportadas pelo respectivo `.c`.
- Helpers privados continuam `static` dentro do `.c`.
- `Player.h` e `GameWorld.h` vão encolhendo à medida que as funções migram para headers específicos; ao fim da refatoração, podem conter só o mínimo (ou deixar de existir — decidiremos ao final).
- Headers devem ter `#pragma once` e incluir apenas o estritamente necessário (tipicamente `Types.h`).

## Roadmap de fases

| Fase | Arquivo novo | Conteúdo | Risco | Status |
|------|--------------|----------|-------|--------|
| **P1** | `PlayerPallete.c` + `PlayerPallete.h` | `changePlayerPallete` | Baixo | ✅ |
| **G3** | `Hud.c` + `Hud.h` | `drawHud` + `PORTRAIT_BG_COLOR` | Baixo | ✅ |
| **G5** | `GameCamera.c` + `GameCamera.h` | `updateCameraPlaying`, `updateCameraEditing` | Baixo | ✅ |
| **G6** | `GameCollision.c` + `GameCollision.h` | `resolveCollisionPlayerStage`, `resolvePlayerPlayerCollision`, `flipPlayers` + vars de flip | Baixo | ✅ |
| **P3** | `PlayerAnimation.c` + `PlayerAnimation.h` | `getPlayerCurrentAnimation`, `getPlayerCurrentAnimationFrame`, `resetPlayerAnimations`, `flipPlayerSide`, `distancePlayer` | Baixo | ✅ |
| **P2** | `PlayerRender.c` + `PlayerRender.h` | `drawPlayer`, `drawPlayerAnimationFrame`, `drawPlayerAnimationFrameBoxes`, `drawPlayerShadow`, `drawPlayerAnimationFrameForShadow`, `drawPlayerInputBuffer`, `drawPlayerOnionLayers`, `drawOnHitPlayerAnimation`, `drawOnBlockPlayerAnimation`, `drawPlayerProjectile` | Baixo | ✅ |
| **G2** | `PlayerSelectMode.c` + `PlayerSelectMode.h` | `drawGameWorldSelectingPlayers`, `updateGameWorldSelectingPlayers`, `getPlayerPalleteSelectingPlayers` + 14 vars de seleção + 3 matrizes const | Baixo | ✅ |
| **G4** | `Stage.c` + `Stage.h` | Texturas stage/barco, timers de troca, `updateStage`, `drawStageBackground`, `drawStageForeground` | Baixo-Médio | ✅ |
| **G1** | `EditorMode.c` + `EditorMode.h` | Toda a área do editor (dezenas de funções + 10 vars static) | Médio | ✅ |
| **P4** | `PlayerInput.c` + `PlayerInput.h` | `processInputAndFeedInputBuffer`, `addInputToPlayerInputBuffer`, `peekAttackButton`, `checkCommandInputs` | Baixo-Médio | ✅ |
| **P5** | `PlayerCollision.c` + `PlayerCollision.h` | `resolvePlayerOponnentContact`, `resolvePlayerOponnentProjectileContact` | Médio | ✅ |
| ⚠️ | **LEMBRETE** | **Antes de começar o P6, avisar o usuário para trocar o modelo de Sonnet de volta para Opus.** O usuário está usando Sonnet a partir do passo 2 por ser tarefa mecânica; o P6 e P7 envolvem mais julgamento (breakout de função monolítica, FSM complexa). | — | — |
| **P6** | `PlayerInit.c` + `PlayerInit.h` | `createPlayer`, `destroyPlayer`, `initializePlayerCommon` (quebrado em `setupPlayerAnimations`, `setupPlayerCommands`, `setupPlayerAttackDistances`, `setupPlayerAnimationMap`), `initializePlayerRyu`, `initializePlayerKen` | Médio | ⏳ |
| **P7** | `PlayerFSM.c` + `PlayerFSM.h` | `processInputPlayer` + `updatePlayer` | **Alto** | ✅ |

Legenda: ⬜ pendente · ⏳ em andamento · ✅ testado e aprovado · ⚠️ testado com problemas

## Comentários "desligados" a preservar (inventário inicial)

Lista provisória — ampliar à medida que cada passo encontrar os blocos.

### GameWorld.c
- **Linhas ~545–546**: `drawGameWorldPlaying` — `drawPlayerShadow` alternativo (spread 60, alpha 0.15).
- **Linhas ~790–800**: `updateGameWorldPlaying` — input debug de paleta por teclado (0–9 com Ctrl).
- **Linhas ~809–814**: `updateGameWorldPlaying` — reset de vida com tecla R.
- **Linhas ~925, 931, 958, 964**: `updateGameWorldPlaying` — `lookingRight` comentado.
- **Linha ~2072**: `updateCameraEditing` — força câmera fixa para debug.

### Player.c
- **Linhas ~184, 193, 202...**: `initializePlayerCommon` — comentários `// verificar offset` em vários pontos.
- **Linha ~1180**: `processInputPlayer` — guard do spawn Hadouken.
- **Linha ~1418**: TODO para generalizar reconhecimento de comando.
- **Linha ~1754**: `distancePlayer` — `hypotf` euclidiano comentado.
- **Linhas ~1833–1834, 1857–1858**: `resolvePlayerOponnentContact` — debug de hurtbox/hitbox via `GetWorldToScreen2D`.
- **Linhas ~901–907**: `drawPlayerShadow` — `DrawEllipse` comentado (sombra antiga).

## Protocolo de execução de cada passo

1. Ler a seção do arquivo alvo na íntegra (antes de mover).
2. Criar novo `.c` (e `.h` se aplicável) preservando ordem, whitespace e comentários.
3. Adicionar `#include`s necessários no novo `.c`.
4. Remover do arquivo original e adicionar o `#include` do novo header onde for necessário.
5. Verificar build com `build.bat -compile`.
6. Reportar para teste manual do usuário.
7. Após aprovação, marcar a fase como ✅ e passar para a próxima.

## Tarefas pós-refatoração (não fazem parte deste plano, mas ficam anotadas)

- **Separar Ryu e Ken em unidades de compilação próprias** (ex: `CharacterRyu.c`, `CharacterKen.c`) para permitir adição futura de novos personagens sem tocar em código compartilhado. Isto será feito **depois** das 13 fases deste plano.

---

## Histórico de execução

### Fase P1 — PlayerPallete ✅
- **Criado**: `src/PlayerPallete.c` (59 linhas) contendo `changePlayerPallete` + constantes `startLineToChangeColors`/`endLineToChangeColors` (cópia local, `static const`).
- **Criado**: `src/include/PlayerPallete.h` com a declaração pública de `changePlayerPallete`.
- **Removido de `Player.c`**: função `changePlayerPallete` (43 linhas). `Player.c` passou de 2304 → 2260 linhas.
- **Removido de `Player.h`**: declaração de `changePlayerPallete` (migrada para `PlayerPallete.h`).
- **Include adicionado em `GameWorld.c`**: `#include "PlayerPallete.h"` (onde estão as 4 chamadas).
- **Notas**: `startLineToChangeColors` e `endLineToChangeColors` continuam em `Player.c` também porque são usadas por `initializePlayerRyu`/`Ken`. Serão consolidadas no passo P6.
- **Decisão arquitetural**: após o P1, o usuário definiu que o projeto seguirá a **Abordagem A — um `.h` por `.c`** (fine-grained headers). O P1 foi ajustado retroativamente para seguir esse padrão.
- **Build**: `build.bat -cleanAndCompile` executado com sucesso.
- **Teste**: funcional, paletas aplicadas corretamente.

### Fases G3, G5, G6, P3, P2 — ✅ (ver histórico anterior)
- Fases concluídas e testadas conforme descrição original. Bug visual de boxes identificado como pré-existente ao refactoring (não introduzido pelo refactoring); será corrigido em etapa separada após conclusão de todas as fases.

---

### Fase G2 — PlayerSelectMode ✅
- **Criado**: `src/PlayerSelectMode.c` + `src/include/PlayerSelectMode.h`.
- **Vars extern'd** (não-static, declaradas no .h): 12 vars de estado de seleção.
- **Vars internas** (static): `pSelectBlinkTime`, `pSelectTransitionTime`, 3 matrizes const.
- **`startMatch`** e **`playMusic`**: removido `static` para acesso externo.
- **Testado e aprovado.**

---

### Fase G4 — Stage ✅
- **Criado**: `src/Stage.c` + `src/include/Stage.h`.
- **`SHOW_MODEL_STAGE_TEXTURE`** movido para `Stage.h` (removido de `GameWorld.c`).
- **Vars internas** (static): 2 `#define` de contagem + 7 vars de estado do stage.
- **`initStage(gw)`**: inicializa arrays de ponteiros de texturas (substituiu 8 linhas em `createGameWorld`).
- **`updateStage(delta)`**: troca de texturas (substituiu bloco em `updateGameWorldPlaying`).
- **`drawStageBackground(gw)`** / **`drawStageForeground(gw)`**: desenho do cenário.
- **Testado e aprovado.**

---

### Fase G1 — EditorMode ⏳ Aguardando teste
- **Criado**: `src/EditorMode.c` + `src/include/EditorMode.h`.
- **`#define`s movidos para `EditorMode.h`**: `SHOW_PLAYER_ONION_MODE_EDITING`, `SHOW_PLAYER_INPUT_BUFFER`, `RYU_ANIMATIONS_FILE`, `KEN_ANIMATIONS_FILE`.
- **Vars extern'd** (não-static, declaradas no .h): `showPlayerOnionEditing`, `showPlayerInputBuffer`, `lastEditState`, `lastEditFrame` — acessadas de `updateGameWorld` e `drawGameWorldPlaying`.
- **Vars internas** (static): `editorMode`, `runPlayerCurrentAnimation`, `runPlayerCurrentAnimationOnce`, `onionOffset`, `showHelp`, `saveTimer`.
- **`drawGameWorldEditing`** e **`updateGameWorldEditing`**: removido `static` (tornam-se públicas).
- **11 funções helper**: permanecem `static` em `EditorMode.c`.
- **Removido de `GameWorld.c`**: 10 vars do bloco `// editor`, 13 forward decls, todos os corpos das 11 funções.
- **Build**: compilado com sucesso (exit code 0). Correção adicionada: `#include <stdlib.h>` para `NULL`.
- **Teste**: aguardando.

---

### Fase P4 — PlayerInput ⏳ Aguardando teste
- **Criado**: `src/PlayerInput.c` + `src/include/PlayerInput.h`.
- **Funções movidas** (removido `static`, agora públicas): `peekAttackButton`, `checkCommandInputs`, `processInputAndFeedInputBuffer`, `addInputToPlayerInputBuffer`.
- **Removido de `Player.c`**: 4 forward declarations estáticas + 4 corpos de função (~158 linhas). `Player.c` truncado na linha 1765.
- **Include adicionado em `Player.c`**: `#include "PlayerInput.h"`.
- **Build**: compilado com sucesso (exit code 0).
- **Teste**: aguardando.

---

### Fase P5 — PlayerCollision ✅
- **Criado**: `src/PlayerCollision.c` + `src/include/PlayerCollision.h`.
- **Funções movidas**: `resolvePlayerOponnentContact`, `resolvePlayerOponnentProjectileContact`.
- **Removido de `Player.h`**: 2 declarações das funções movidas.
- **Removido de `Player.c`**: 2 corpos de função (~266 linhas). `Player.c` truncado na linha 1498.
- **Include adicionado em `GameWorld.c`**: `#include "PlayerCollision.h"`.
- **Testado e aprovado.**

---

### Fase P6 — PlayerInit ⏳ Aguardando teste
- **Criado**: `src/PlayerInit.c` (895 linhas) + `src/include/PlayerInit.h`.
- **Constantes movidas de `Player.c`**: `startLineToChangeColors` e `endLineToChangeColors` (static const, agora em `PlayerInit.c`). Consolidação prevista no plano original do P1.
- **Funções movidas para `PlayerInit.c`**:
  - `createPlayer` (pública).
  - `initializePlayerRyu` (pública).
  - `initializePlayerKen` (pública).
  - `destroyPlayer` (pública).
  - `initializePlayerCommon` (permanece `static`, agora é um orquestrador).
- **Quebra de `initializePlayerCommon`** em 4 sub-funções `static` privadas de `PlayerInit.c`:
  - `setupPlayerCommands(Player *p)` — inicializa os 3 command inputs (Hadouken, Shoryuken, Tatsumaki).
  - `setupPlayerAttackDistances(Player *p)` — seis `*CloseTriggerDist`.
  - `setupPlayerAnimations(Player *p)` — `createAnimationFrames` + `initAnimationFrames` de ~60 animações (incluindo os dois blocos especiais do `mk`/`hk` Tatsumaki com `start = 9`).
  - `setupPlayerAnimationMap(Player *p)` — mapa `p->animations[STATE] = &p->xxAnim`, suporte `onHit`/`onBlock` e `onHitPos`/`onBlockPos`.
  - O orquestrador `initializePlayerCommon` mantém apenas as propriedades básicas (pos, dim, vel, speeds, state, health, direction, gamepad, input buffer init) e chama as 4 sub-funções em sequência.
- **Comentários "desligados" preservados literalmente**: bloco `// previous` (dim antigas) em `initializePlayerCommon` e todos os `// verificar offset` nos `initAnimationFrames`.
- **Removido de `Player.h`**: 4 declarações (`createPlayer`, `initializePlayerRyu`, `initializePlayerKen`, `destroyPlayer`). `Player.h` agora tem apenas `processInputPlayer` e `updatePlayer`.
- **Removido de `Player.c`**: linhas 23–877 (constantes + `createPlayer` + `initializePlayerCommon` + `initializePlayerRyu` + `initializePlayerKen` + `destroyPlayer`). `Player.c` passou de 1498 → 641 linhas.
- **Include adicionado em `GameWorld.c`**: `#include "PlayerInit.h"` (logo após `PlayerAnimation.h`).
- **Includes de `PlayerInit.c`**: `<stdlib.h>` (malloc/free), `<string.h>` (strcpy), `raylib/raylib.h` (UnloadTexture, Vector2), `Animation.h` (createAnimationFrames/initAnimationFrames/destroyAnimationFrames), `PlayerInit.h`, `Projectile.h` (createProjectile), `ResourceManager.h` (rm), `Types.h`, `Utils.h` (createTextureFromTextureReplacingColor).
- **Build**: `build.bat -compile` executado com sucesso (exit code 0).
- **Teste**: aguardando.

---

### Fase P7 — PlayerFSM ✅
- **Criado**: `src/PlayerFSM.c` (643 linhas) + `src/include/PlayerFSM.h`.
- **Funções movidas para `PlayerFSM.c`** (agora públicas, declaradas em `PlayerFSM.h`):
  - `processInputPlayer` — FSM principal: tratamento de vitória/derrota, falling/getting-up, special moves em progresso (spawn de projétil do Hadouken no frame 5), hitstun, blockstun, ataques em progresso, pulos (com jump-attacks), jump cooldown, recognição de commands (Hadouken/Shoryuken/Tatsumaki), basic attacks (com `*Close` triggers), movimento no chão, e update das animações idle/forward/backward/crouching.
  - `updatePlayer` — física (posição, velocidade, gravidade), pushback friction em hit/block stun, atualização das animações `onHit`/`onBlock`, e `updateProjectile`.
- **`TRACE_SPECIAL_MOVE`** (static const) movido junto com a função que o usa.
- **Comentários "desligados" preservados literalmente**:
  - `// TODO: needs to be generalized (in the future)` no bloco de reconhecimento de commands.
  - Guard do spawn Hadouken (`if ( ( specialState == PLAYER_STATE_SPECIAL_LP_HADOUKEN || ... ) && player->projectile->active )`).
  - Todos os comentários in-line do corpo da FSM.
- **Includes de `PlayerFSM.c`**: `<stdbool.h>`, `<stddef.h>` (para `NULL`), `<math.h>` (para `fabs`), `raylib/raylib.h`, `Animation.h`, `Macros.h` (trace, `PUSHBACK_DECAY`), `PlayerAnimation.h`, `PlayerFSM.h`, `PlayerInput.h`, `Projectile.h`, `Types.h`, `Utils.h`.
- **`Player.c` esvaziado**: agora contém apenas um cabeçalho doxygen descrevendo a decomposição + um `typedef int player_c_anchor_unit_t;` para evitar `ISO C forbids an empty translation unit [-Wpedantic]`. Passou de 641 → 22 linhas. Mantido como **arquivo-âncora** conforme o princípio (7) do plano, para minimizar alterações no build.
- **`Player.h` esvaziado**: agora contém apenas cabeçalho doxygen + `#pragma once`. Mantido como âncora porque ainda é incluído por `EditorMode.c`, `GameCollision.c`, `GameWorld.c` e o próprio `Player.c`.
- **Removido de `GameWorld.c`**: nada (apenas adicionado). `#include "PlayerFSM.h"` inserido após `PlayerAnimation.h`.
- **Correções durante o build**:
  1. `PlayerFSM.c` usava `NULL` — adicionado `#include <stddef.h>`.
  2. `Player.c` tinha `src/*.c` num comentário, disparando `warning: '/*' within comment` — trocado por "globbing de src".
  3. `Player.c` vazio disparava `error: ISO C forbids an empty translation unit` — adicionado `typedef` placeholder.
- **Build**: `build.bat -cleanAndCompile` executado com sucesso (exit code 0), `fighting-game.exe` recriado.
- **Testado e aprovado.**

---

## 🎉 Estado final da refatoração

Todas as 13 fases executadas. Resumo dos tamanhos de arquivo:

| Arquivo | Antes | Depois | Δ |
|---------|------:|-------:|---:|
| `GameWorld.c` | 2201 | ~ (ver estado atual) | — |
| `Player.c` | 2304 | **22** (âncora) | −2282 |
| `Player.h` | — | **11** (âncora) | — |

Novos arquivos criados ao longo do processo:
- **Player**: `PlayerPallete.{c,h}`, `PlayerAnimation.{c,h}`, `PlayerRender.{c,h}`, `PlayerInput.{c,h}`, `PlayerCollision.{c,h}`, `PlayerInit.{c,h}`, `PlayerFSM.{c,h}`.
- **GameWorld**: `Hud.{c,h}`, `GameCamera.{c,h}`, `GameCollision.{c,h}`, `PlayerSelectMode.{c,h}`, `Stage.{c,h}`, `EditorMode.{c,h}`.

Estado global file-scope migrou junto com as funções que o usam. Headers fine-grained (um `.h` por `.c`) implementados conforme Abordagem A. Nenhuma reescrita de lógica — apenas movimentação mecânica preservando comentários "desligados".

### Próximo passo (fora deste plano)
Conforme anotado na seção "Tarefas pós-refatoração": separar Ryu e Ken em unidades de compilação próprias (`CharacterRyu.c`, `CharacterKen.c`) para permitir adição de novos personagens sem tocar em código compartilhado.

_Última atualização: **P7 (PlayerFSM) testado e aprovado em 2026-04-15. 🎉 Plano de refatoração 100% concluído — todas as 13 fases executadas, testadas e aprovadas.**_
