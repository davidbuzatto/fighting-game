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
| **P2** | `PlayerRender.c` + `PlayerRender.h` | `drawPlayer`, `drawPlayerAnimationFrame`, `drawPlayerAnimationFrameBoxes`, `drawPlayerShadow`, `drawPlayerAnimationFrameForShadow`, `drawPlayerInputBuffer`, `drawPlayerOnionLayers`, `drawOnHitPlayerAnimation`, `drawOnBlockPlayerAnimation`, `drawPlayerProjectile` | Baixo | ⏳ Aguardando teste |
| **G2** | `PlayerSelectMode.c` + `PlayerSelectMode.h` | `drawGameWorldSelectingPlayers`, `updateGameWorldSelectingPlayers`, `getPlayerPalleteSelectingPlayers` + 14 vars de seleção + 3 matrizes const | Baixo | ⬜ |
| **G4** | `Stage.c` + `Stage.h` | Texturas stage/barco, timers de troca, `updateStage`, `drawStageBackground`, `drawStageForeground` | Baixo-Médio | ⬜ |
| **G1** | `EditorMode.c` + `EditorMode.h` | Toda a área do editor (dezenas de funções + 10 vars static) | Médio | ⬜ |
| **P4** | `PlayerInput.c` + `PlayerInput.h` | `processInputAndFeedInputBuffer`, `addInputToPlayerInputBuffer`, `peekAttackButton`, `checkCommandInputs` | Baixo-Médio | ⬜ |
| **P5** | `PlayerCollision.c` + `PlayerCollision.h` | `resolvePlayerOponnentContact`, `resolvePlayerOponnentProjectileContact` | Médio | ⬜ |
| ⚠️ | **LEMBRETE** | **Antes de começar o P6, avisar o usuário para trocar o modelo de Sonnet de volta para Opus.** O usuário está usando Sonnet a partir do passo 2 por ser tarefa mecânica; o P6 e P7 envolvem mais julgamento (breakout de função monolítica, FSM complexa). | — | — |
| **P6** | `PlayerInit.c` + `PlayerInit.h` | `createPlayer`, `destroyPlayer`, `initializePlayerCommon` (quebrado em `setupPlayerAnimations`, `setupPlayerCommands`, `setupPlayerAttackDistances`, `setupPlayerAnimationMap`), `initializePlayerRyu`, `initializePlayerKen` | Médio | ⬜ |
| **P7** | `PlayerFSM.c` + `PlayerFSM.h` | `processInputPlayer` + `updatePlayer` | **Alto** | ⬜ |

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

---

_Última atualização: fim da Fase P1 (PlayerPallete), testado e aprovado. Próximo: Fase G3 (Hud)._
