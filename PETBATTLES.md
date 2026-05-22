# Pet Battle System — Documentation

## File Locations

| File | Purpose |
|---|---|
| `src/server/game/BattlePet/PetBattle.cpp` | Core battle logic |
| `src/server/game/BattlePet/PetBattle.h` | Types, enums, `PetBattleTeam` struct |
| `src/server/game/BattlePet/BattlePetSpawnMgr.cpp` | Wild pet spawning, GUID management |
| `src/server/game/BattlePet/BattlePetTrainerMgr.cpp` | Trainer roster loading |
| `src/server/game/Handlers/BattlePetHandler.cpp` | Client packet handlers |
| `src/server/game/BattlePet/BattlePetAbilityEffect.cpp` | Ability effect handlers (damage, heal, aura, weather, etc.) |
| `src/server/game/BattlePet/BattlePetAura.cpp` | Aura lifecycle: `OnApply()`, `Process()`, `Expire()` |

## Battle Flow

### 1. Battle Start

1. Player right-clicks trainer → `CMSG_PET_BATTLE_REQUEST_UPDATE` → `PetBattle` created (state: `Created`)
2. `SendInitialUpdate()` → client shows pet selection UI
3. State → `WaitingForFrontPets`
4. Player sends `CMSG_PET_BATTLE_SET_FRONT_PET` (pet 1) → player team `m_ready = true`
5. Trainer team already ready (set via `SetActivePet(GetPet(0))` in `AddPlayer()`)
6. Both ready → `StartBattle()` → `SendFirstRound()` → state → `InProgress`

### 2. Round Loop

1. Player sends `CMSG_PET_BATTLE_INPUT` (ability or swap) → `m_ready = true`
2. `Update()` tick: both ready → `HandleRound()`
3. Pending moves processed: trainer casts ability, player casts ability or swaps pet
4. `GetFirstAttackingTeam()` — faster pet attacks first (based on speed stat)
5. Round start procs → abilities execute → round end procs
6. Auto-swap trainer dead pets (if any) before sending round result
7. Auras processed, stat updates sent
8. Cooldowns decremented, then round result sent to player
9. `TurnFinished()` on both teams → trainer auto-queues ability, player `m_ready = false`
10. Death check → repeat

### 3. Pet Swapping

Players can swap pets at any time (alive or dead). Two separate packet paths:

**Swap alive pet** — `CMSG_PET_BATTLE_INPUT` with `PET_BATTLE_MOVE_TYPE_SWAP_OR_PASS`:
1. Player sends `CMSG_PET_BATTLE_INPUT` with move type 2 + pet index → `SetPendingMove(SWAP_OR_PASS)` → `m_ready = true`
2. `HandleRound()` processes pending move → `SwapActivePet()` → new pet active for this round
3. `CanSwap()` validates: no multi-turn ability running, no swap lock from abilities (e.g., Sticky Web, Banished)

**Swap dead pet** — `CMSG_PET_BATTLE_SET_FRONT_PET`:
1. Player sends `CMSG_PET_BATTLE_SET_FRONT_PET` with pet index
2. Server validates: active pet must be dead (line 852-855 in `BattlePetHandler.cpp`)
3. `SetPendingMove(SWAP_DEAD_PET)` → `m_ready = true`
4. Next `HandleRound()` processes swap → new pet active

**Trainer dead pet** — auto-swap in `HandleRound()`:
1. Pet dies in round → `HandleRound()` auto-swaps trainer dead pet to next available pet before `SendRoundResult()`
2. Client receives pet death + swap effect in same round result packet, allowing it to proceed correctly
3. `TurnFinished()` also queues `SWAP_DEAD_PET` pending move for trainer (used for AI ability selection on the new pet)

### 4. Pet Death

1. Pet dies in round → `Kill()` sets round result
2. For trainer battles: `HandleRound()` auto-swaps dead trainer pet before sending round result
3. For wild pet battles: `CATCH_OR_KILL` round result is sent, triggering catch screen
4. `TurnFinished()` handles trainer AI: sets `SWAP_DEAD_PET` pending move and picks new ability
5. Player must send `CMSG_PET_BATTLE_SET_FRONT_PET` → `SetPendingMove(SWAP_DEAD_PET)` for their own dead pet
6. Next round: both ready → swap processed → new pet active

### 5. Battle End

1. One team has no alive pets → `EndBattle(lostTeam)`
2. XP calculation, achievement credit, quest credit for defeated trainer
3. State → `Finished`

## Key Design Principles

- **Trainer and player death flows are symmetric.** Both use `SetPendingMove()` → `m_ready = true` → next `HandleRound()`. The only difference is the trainer auto-submits via `TurnFinished()` while the player sends explicit client input.
- **`TurnFinished()` is the single entry point for PvE team decision-making** — both trainer and wild opponents use the same `!m_owner` path.
- **`HandleRound()` MUST auto-swap trainer dead pets before `SendRoundResult()`.** The client needs the swap effect in the same round as the pet death to proceed correctly.
- **`CATCH_OR_KILL` round result is ONLY for wild pet battles (`PET_BATTLE_TYPE_PVE`).** Trainer pet deaths send `NORMAL` round result.
- **Players can swap pets at any time** (alive or dead). Two packet paths:
  - `CMSG_PET_BATTLE_INPUT` with `PET_BATTLE_MOVE_TYPE_SWAP_OR_PASS` — swap alive pet mid-round
  - `CMSG_PET_BATTLE_SET_FRONT_PET` — initial pet selection AND dead pet replacement only
- **`CMSG_PET_BATTLE_INPUT`** is used for both ability selection AND alive pet swaps.
- **`CanSwap()`** validates swap eligibility: no multi-turn ability running, no swap locks from abilities (e.g., Sticky Web, Banished), target pet must be alive.
- **The `m_ready` flag is the synchronization mechanism** — both teams must be ready before `HandleRound()` runs.
- **Weather effects (Effect 80) target all pets on both teams** via `PET_BATTLE_ABILITY_TARGET_ALL`. The aura is applied to each pet individually with per-pet state modifiers (e.g., reduced healing taken). All weather abilities have `MaxAllowed=0` (no stack cap).
- **Quest credit for trainer battles uses `QUEST_OBJECTIVE_WINPETBATTLEAGAINSTNPC` (type 11),** not `QUEST_OBJECTIVE_MONSTER` (type 0). `KilledMonsterCredit` handles type 0; `PetBattleCompleteQuest` handles type 11.

## Fixes Applied

1. `BattlePetSpawnMgr.cpp:413` — `erase(guid)` instead of `erase(replacementGUID)` — fixes duplicate GUID crash when trainer swaps pets
2. `BattlePetSpawnMgr.cpp:89` — `break;` instead of `break;;` — syntax bug
3. `PetBattle.cpp:375-379` — removed `else if (m_owner && !m_ready)` auto-pass block — fixes player never getting a turn (server was auto-submitting for player)
4. `PetBattle.h:182` — removed orphaned `SetReady()` declaration
5. `PetBattle.cpp:697-711` — `HandleRound()` auto-swap for trainer dead pets with `ignoreAlive=true` — client needs swap effect in same round as pet death
6. `PetBattle.cpp:989` — only set `CATCH_OR_KILL` round result for wild pet battles (`PET_BATTLE_TYPE_PVE`). Trainer pet deaths send `NORMAL` round result.
7. `PetBattle.cpp:226-239` — removed duplicate `PET_BATTLE_EFFECT_ACTIVE_PET` from `SetActivePet()`. `SwapActivePet()` already adds the effect.
8. `PetBattle.cpp:725-730` — moved cooldown decrement from `TurnFinished()` to before `SendRoundResult()`. Cooldowns were sent one turn too high.
9. `BattlePetAbilityEffect.cpp:111` — changed weather effect target from `PET_BATTLE_ABILITY_TARGET_HEAD` to `PET_BATTLE_ABILITY_TARGET_ALL`. Weather auras now apply to all 6 pets in the battle, so swapped-in pets already have the weather effect.
10. `Player.cpp:17975-18016`, `Player.h:1772`, `PetBattle.cpp:570` — added `PetBattleCompleteQuest()` to handle `QUEST_OBJECTIVE_WINPETBATTLEAGAINSTNPC` (type 11) quest objectives. Trainer battle victory now correctly completes quests like "Zunta" (31818).

## Current State

- ✅ Wild pet battles: working
- ✅ Trainer pet battles: multi-pet rosters, pet death swaps, round flow all working
- ✅ Pet death handling: symmetric between player and trainer
- ✅ Trainer creature cleanup: unroots, un-pacifies, teleports to original position
- ✅ XP/achievement credit on player victory
- ✅ Weather effects: applied to all pets on both teams, persist through swaps
- ✅ Quest credit: `QUEST_OBJECTIVE_WINPETBATTLEAGAINSTNPC` objectives complete on trainer victory
- ⏳ Multi-pet wild battles: not implemented (TODO at `PetBattle.cpp:434` — `AddWildBattlePet()` only adds one pet)
- ⏳ Trainer AI: random ability selection (acceptable for 3 abilities)
