# Pet Battle System — Documentation

## File Locations

| File | Purpose |
|---|---|
| `src/server/game/BattlePet/PetBattle.cpp` | Core battle logic |
| `src/server/game/BattlePet/PetBattle.h` | Types, enums, `PetBattleTeam` struct |
| `src/server/game/BattlePet/BattlePetSpawnMgr.cpp` | Wild pet spawning, GUID management |
| `src/server/game/BattlePet/BattlePetTrainerMgr.cpp` | Trainer roster loading |
| `src/server/game/Handlers/BattlePetHandler.cpp` | Client packet handlers |

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
6. Auras processed, stat updates sent
7. Round result sent to player
8. `TurnFinished()` on both teams → trainer auto-queues ability, player `m_ready = false`
9. Death check → repeat

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

**Trainer dead pet** — `TurnFinished()` auto-swap:
1. Pet dies in round → `TurnFinished()` detects dead pet (line 350)
2. `SetPendingMove(SWAP_DEAD_PET, 0, nextPet)` → `m_ready = true`
3. Next `HandleRound()` processes swap → new pet active

### 4. Pet Death

1. Pet dies in round → no auto-swap in `HandleRound()`
2. `TurnFinished()` handles pet death swap for `!m_owner` teams (trainer) via `SetPendingMove(SWAP_DEAD_PET)`
3. Player must send `CMSG_PET_BATTLE_SET_FRONT_PET` → `SetPendingMove(SWAP_DEAD_PET)`
4. Next round: both ready → swap processed → new pet active

### 5. Battle End

1. One team has no alive pets → `EndBattle(lostTeam)`
2. XP calculation, quest credits, trainer creature cleanup
3. State → `Finished`

## Key Design Principles

- **Trainer and player death flows are symmetric.** Both use `SetPendingMove()` → `m_ready = true` → next `HandleRound()`. The only difference is the trainer auto-submits via `TurnFinished()` while the player sends explicit client input.
- **`TurnFinished()` is the single entry point for PvE team decision-making** — both trainer and wild opponents use the same `!m_owner` path.
- **`HandleRound()` MUST auto-swap trainer dead pets before `SendRoundResult()`.** The client needs the swap effect in the same round as the pet death to proceed correctly. Without it, the client receives `CATCH_OR_KILL` with the trainer's pet still dead and doesn't know how to proceed.
- **`CATCH_OR_KILL` round result is ONLY for wild pet battles (PET_BATTLE_TYPE_PVE).** Trainer pet deaths send `NORMAL` round result. The client expects `CATCH_OR_KILL` to be followed by `SMSG_PET_BATTLE_FINAL_ROUND`. In trainer battles, `CATCH_OR_KILL` was sent but the battle continued, putting the client in an invalid state waiting for the final round packet that never comes.
- **Players can swap pets at any time** (alive or dead). Two packet paths:
  - `CMSG_PET_BATTLE_INPUT` with `PET_BATTLE_MOVE_TYPE_SWAP_OR_PASS` — swap alive pet mid-round
  - `CMSG_PET_BATTLE_SET_FRONT_PET` — initial pet selection AND dead pet replacement only
- **`CMSG_PET_BATTLE_INPUT`** is used for both ability selection AND alive pet swaps.
- **`CanSwap()`** validates swap eligibility: no multi-turn ability running, no swap locks from abilities (e.g., Sticky Web, Banished), target pet must be alive.
- **The `m_ready` flag is the synchronization mechanism** — both teams must be ready before `HandleRound()` runs.

## Fixes Applied

1. `BattlePetSpawnMgr.cpp:413` — `erase(guid)` instead of `erase(replacementGUID)` — fixes duplicate GUID crash when trainer swaps pets
2. `BattlePetSpawnMgr.cpp:89` — `break;` instead of `break;;` — syntax bug
3. `PetBattle.cpp:375-379` — removed `else if (m_owner && !m_ready)` auto-pass block — fixes player never getting a turn (server was auto-submitting for player)
4. `PetBattle.h:182` — removed orphaned `SetReady()` declaration
5. `PetBattle.cpp:692-704` — removed `HandleRound()` auto-swap block — fixes trainer/player death flow asymmetry
6. `PetBattle.cpp:723` — added `true` parameter to `SwapActivePet(availablePets[0], true)` in `HandleRound()` auto-swap — fixes trainer second pet never appearing (was missing `ignoreAlive` flag so `CanSwap()` rejected the swap because the outgoing pet was dead)
7. `PetBattle.cpp:989` — only set `CATCH_OR_KILL` round result for wild pet battles (`PET_BATTLE_TYPE_PVE`). Trainer pet deaths now send `NORMAL` round result. The client expects `CATCH_OR_KILL` to be followed by `SMSG_PET_BATTLE_FINAL_ROUND`. In trainer battles, `CATCH_OR_KILL` was sent but the battle continued, putting the client in an invalid state waiting for the final round packet that never comes.
8. `PetBattle.cpp:226-239` — removed duplicate `PET_BATTLE_EFFECT_ACTIVE_PET` from `SetActivePet()`. `SwapActivePet()` already adds the effect, so `SetActivePet()` adding it caused two identical swap effects in the same round result packet.
9. `PetBattle.cpp:725-730` — moved cooldown decrement from `TurnFinished()` to before `SendRoundResult()`. Cooldowns were being sent to the client before they were decremented, causing abilities to appear on cooldown for one extra turn.
10. `PetBattle.cpp:697-711` — restored `HandleRound()` auto-swap for trainer dead pets. The client needs the swap effect in the same round as the pet death to proceed correctly.

## Current State

- ✅ Wild pet battles: working
- ✅ Trainer pet battles: multi-pet rosters, pet death swaps, round flow all working
- ✅ Pet death handling: symmetric between player and trainer
- ✅ Trainer creature cleanup: unroots, un-pacifies, teleports to original position
- ✅ XP/achievement credit on player victory
- ⚠️ Pet battle weather effects: not applied to incoming swapped pets (e.g., "Darkness" effect doesn't transfer to the trainer's second pet)
- ⚠️ Quest credit: not awarded for defeating trainer (e.g., "defeat Zunta" quest credit not given)
- ⏳ Multi-pet wild battles: not implemented (TODO at `PetBattle.cpp:434` — `AddWildBattlePet()` only adds one pet)
- ⏳ Trainer AI: random ability selection (acceptable for 3 abilities)
