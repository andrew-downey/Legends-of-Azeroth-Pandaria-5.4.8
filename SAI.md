# SmartAI (SAI) Reference

SmartAI is the server's primary scripting system for creatures and gameobjects. It uses a table-driven approach: entries in `smart_scripts` define event→action mappings, loaded into memory at startup.

## Table: `smart_scripts`

| Column | Type | Description |
|--------|------|-------------|
| `entryorguid` | INT | Creature/gameobject entry ID (or unique GUID for custom spawns) |
| `source_type` | INT | **0** = creature/gameobject entry, **90** = map, **1** = spell |
| `id` | INT | Script ID (unique per entryorguid). Used for chaining via `link` |
| `link` | INT | Links to another script: when this action finishes, triggers `id=link` |
| `event_type` | INT | What triggers this script (see table below) |
| `event_phase_mask` | INT | Bitmask of event phases (usually 0) |
| `event_chance` | INT | 0-100, probability this event fires |
| `event_flags` | INT | Event-specific flags |
| `event_param1`-`4` | INT | Event-specific parameters (varies by event_type) |
| `event_param5` | INT | Event-specific parameter |
| `action_type` | INT | What to do (see table below) |
| `action_param1`-`3` | INT | Action-specific parameters |
| `action_param4` | INT | Action-specific parameter |
| `action_param5` | INT | Action-specific parameter |
| `action_param6` | INT | Action-specific parameter |
| `target_type` | INT | Who to target (see table below) |
| `target_param1`-`3` | INT | Target-specific parameters |
| `target_x`-`o` | FLOAT | Target position (if applicable) |
| `comment` | VARCHAR | Human-readable description |

## Event Types (event_type)

| Type | Name | Event Param(s) | Description |
|------|------|----------------|-------------|
| 19 | SMART_EVENT_QUEST_ACCEPTED | quest_id | Player accepts a quest from this creature |
| 20 | SMART_EVENT_QUEST_TAKEN | quest_id | Player picks up a quest |
| 21 | SMART_EVENT_QUEST_REWARDED | quest_id | Player completes a quest |
| 25 | SMART_EVENT_SPELLHIT_TARGET | spell_id | Creature is hit by a spell |
| 27 | SMART_EVENT_EVENT_ENTRY | event_id | Custom event fired via `DoZoneInCombat` or manual trigger |
| 61 | SMART_EVENT_TIMED_ACTION | timer (ms) | Periodic timer (fires every `event_param1` ms) |
| 64 | SMART_EVENT_SAY | — | Creature says text (gossip) |
| 73 | SMART_EVENT_ON_SPELLCLICK | — | Player clicks creature with spellclick |

## Action Types (action_type)

| Type | Name | Param1 | Param2 | Param3 | Param4 | Param5 | Target Type | Description |
|------|------|--------|--------|--------|--------|--------|-------------|-------------|
| 1 | SMART_ACTION_TALK | text_id | — | — | — | — | 7 (Caster) | Creature says/emotes text |
| 8 | SMART_ACTION_KILL_CREDIT | — | — | — | — | — | 7 (Caster) | Kill credit for player |
| 12 | SMART_ACTION_SUMMON_CREATURE | entry | amount | despawn_time | — | — | 8 (Nearby) | Summon creature at position |
| 14 | SMART_ACTION_REMOVE_AURA | spell_id | — | — | — | — | 1 (Self) | Remove aura from target |
| 18 | SMART_ACTION_SUMMON_CREATURE_AT_GUID | entry | amount | despawn_time | — | — | 8 (Nearby) | Summon at guid position |
| 28 | SMART_ACTION_REMOVE_AURA | spell_id | — | — | — | — | 1 (Self) | Remove aura from target |
| 33 | SMART_ACTION_CALL_KILLEDMONSTER | kill_credit_id | — | — | — | — | 7 (Caster) | Give kill credit |
| 41 | SMART_ACTION_FORCE_DESPAWN | despawn_timer (ms) | — | — | — | — | 1 (Self) | Despawn after delay |
| 72 | SMART_ACTION_SET_DATA | data | — | — | — | — | 0 (Self) | Set AI data flag |
| 73 | SMART_ACTION_SET_UNIT_FLAG | flag | — | — | — | — | 1 (Self) | Set unit flags |
| 74 | SMART_ACTION_REMOVE_UNIT_FLAG | flag | — | — | — | — | 1 (Self) | Clear unit flags |

## Target Types (target_type)

| Type | Name | Description |
|------|------|-------------|
| 0 | SMART_TARGET_SELF | The creature/gameobject itself |
| 1 | SMART_TARGET_SELF | Self (for creatures) |
| 7 | SMART_TARGET_CASTER | The player/unit who triggered the event |
| 8 | SMART_TARGET_nearby | Nearby creatures (uses target_x/y/z/o) |
| 17 | SMART_TARGET_NEARBY_CREATURE | Nearby creature with position |

## Spellclick Pattern (event_type 73)

The spellclick flow:

1. **`npc_spellclick_spells`** — defines which spell the client sends when clicking the NPC
2. **`conditions`** — optional conditions (SourceTypeOrReferenceId=18) to gate the spellclick
3. **`smart_scripts`** — event_type=73 handles the click and executes actions
4. **`creature_template.npcflag`** — must be 16777216 (SPELLCLICK)

### Example: Spellclick → Kill Credit → Despawn

```sql
-- Step 1: Set NPC flag for spellclick
UPDATE `creature_template` SET `npcflag` = 16777216 WHERE `entry` = <creature_id>;

-- Step 2: Define which spell triggers the click
DELETE FROM `npc_spellclick_spells` WHERE `npc_entry` = <creature_id>;
INSERT INTO `npc_spellclick_spells` (`npc_entry`, `spell_id`, `cast_flags`, `user_type`)
VALUES (<creature_id>, <spell_id>, 1, 0);

-- Step 3: Set up SmartAI
SET @ENTRY := <creature_id>;
UPDATE `creature_template` SET `AIName` = "SmartAI" WHERE `entry` = @ENTRY;
DELETE FROM `smart_scripts` WHERE `entryorguid` = @ENTRY AND `source_type` = 0;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
  (@ENTRY, 0, 0, 0, 73, 0, 100, 1, 0, 0, 0, 0, 28, <aura_id>, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Remove Aura"),
  (@ENTRY, 0, 1, 0, 73, 0, 100, 1, 0, 0, 0, 0, 33, <kill_credit_id>, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, "Kill Credit"),
  (@ENTRY, 0, 2, 0, 73, 0, 100, 0, 0, 0, 0, 0, 41, 10000, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Despawn 10s");
```

### Event Param Details

**event_type 73 (SPELLCLICK)** — event_param1/2/3/4 are unused (0). The spell_id is defined in `npc_spellclick_spells`, not in SAI.

**event_type 19 (QUEST_ACCEPTED)** — event_param1 = quest_id. Fires when player accepts quest from this creature.

**event_type 20 (QUEST_TAKEN)** — event_param1 = quest_id. Fires when player picks up quest.

**event_type 21 (QUEST_REWARDED)** — event_param1 = quest_id. Fires when player completes quest.

**event_type 25 (SPELLHIT_TARGET)** — event_param1 = spell_id that hit this creature.

**event_type 61 (TIMED_ACTION)** — event_param1 = timer in milliseconds. Fires periodically.

**event_type 64 (SAY)** — event_param1-4 unused. Fires when creature speaks (gossip).

### Action Param Details

**action_type 28 (REMOVE_AURA)** — action_param1 = spell aura ID. action_param2 unused. target_type = 1 (Self).

**action_type 33 (CALL_KILLEDMONSTER)** — action_param1 = kill credit creature entry ID. Gives kill credit to the triggering player.

**action_type 41 (FORCE_DESPAWN)** — action_param1 = despawn delay in milliseconds. target_type = 1 (Self).

**action_type 12 (SUMMON_CREATURE)** — action_param1 = creature entry to summon. action_param2 = amount. action_param3 = despawn time in ms. target_type = 8 (Nearby) with position in target_x/y/z.

## Conditions for Spellclicks

Conditions for spellclicks use `SourceTypeOrReferenceId = 18` (CONDITION_SOURCE_TYPE_SPELL_CLICK_EVENT).

| Column | Meaning for Spellclicks |
|--------|------------------------|
| `SourceGroup` | NPC entry ID (creature that is clicked) |
| `SourceEntry` | Spell ID (the spellclick spell) |
| `ConditionTypeOrReference` | Condition type (e.g., 9 = QUESTTAKEN, 2 = HAS_ITEM) |
| `ConditionValue1` | Quest ID (for type 9) or Item ID (for type 2) |

### CONDITION_QUESTTAKEN (type 9)

Fires only while the specified quest is **active** (taken but not rewarded).

```sql
DELETE FROM conditions WHERE SourceTypeOrReferenceId IN (18) AND SourceGroup IN (<npc_entry>) AND `SourceEntry` IN (<spell_id>);
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorType`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES
(18, <npc_entry>, <spell_id>, 0, 0, 9, 0, <quest_id>, 0, 0, 0, 0, 0, '', 'Quest <quest_id> required for spellclick');
```

### CONDITION_HAS_ITEM (type 2)

Fires only while player has the specified item in inventory.

```sql
INSERT INTO `conditions` (...) VALUES
(18, <npc_entry>, <spell_id>, 0, 0, 2, 0, <item_id>, 1, 0, 0, 0, 0, '', 'Item <item_id> required for spellclick');
```

## Existing Farm Creature SAI

### Farmer Yoon (ground, entry 58646)

```sql
-- Spellhit 126987 → kill credit 64942
(58646, 0, 0, 1, 8, 0, 100, 0, 126987, 0, 0, 0, 0, 33, 64942, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, "On spellhit 126987 - kill credit 64942"),
-- Talk 0
(58646, 0, 1, 0, 61, 0, 100, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 0, "On spellhit 126987 - talk 0"),
-- Quest 30254 → summon rocks (59985 x2, 2min despawn)
(58646, 0, 2, 4, 19, 0, 100, 0, 30254, 0, 0, 0, 0, 33, 59985, 2, 2, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 0, "[Kill]OnSpeak"),
-- Quest 30254 → summon rabbits (59990 x2, 2min despawn)
(58646, 0, 3, 4, 19, 0, 100, 0, 30254, 0, 0, 0, 0, 33, 59990, 2, 2, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0),
-- Quest 30255 → summon parched cabbage at pos 1
(58646, 0, 4, 0, 19, 0, 100, 0, 30255, 0, 0, 0, 0, 12, 59987, 7, 0, 0, 0, 0, 8, 0, 0, 0, -161.232, 637.367, 165.409, 0, "On Quest Accept Summon Green Cabbage"),
-- Quest 30255 → summon parched cabbage at pos 2
(58646, 0, 5, 0, 19, 0, 100, 0, 30255, 0, 0, 0, 0, 12, 59987, 7, 0, 0, 0, 0, 8, 0, 0, 0, -161.734, 641.861, 165.409, 0, "On Quest Accept Summon Green Cabbage"),
```

**Key:** Uses `link` field for chaining (id=0 links to id=1). Quest accept events summon temporary creatures.

### Unbudging Rock (entry 58719)

```sql
-- Event 64 → action 72 (set data)
(58719, 0, 0, 1, 64, 0, 100, 0, 0, 0, 0, 0, 72, 0, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, "Unbudging Rock"),
-- Event 61 → action 33 (kill credit)
(58719, 0, 1, 2, 61, 0, 100, 0, 0, 0, 0, 0, 33, 58719, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, "Unbudging Rock"),
-- Event 61 → action 41 (despawn)
(58719, 0, 2, 0, 61, 0, 100, 0, 0, 0, 0, 0, 41, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Unbudging Rock"),
```

**Key:** Uses `link` for chaining. Event 64 (SAY) → links to id=1 → links to id=2 → no link (end).

### Parched Cabbage (entry 59987)

```sql
-- Spellclick → remove aura 115824
(59987, 0, 0, 0, 73, 0, 100, 1, 0, 0, 0, 0, 28, 115824, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "On SpellClick Remove Aura"),
-- Spellclick → kill credit 59987
(59987, 0, 1, 0, 73, 0, 100, 1, 0, 0, 0, 0, 33, 59987, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, "On SpellClick Kill Credit"),
-- Spellclick → despawn 10s
(59987, 0, 2, 0, 73, 0, 100, 0, 0, 0, 0, 0, 41, 10000, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "Despawn 10s"),
```

**Key:** Three independent spellclick actions (no chaining). event_flags=1 on first two = only fire if conditions pass.

## spell_scripts Table

The `spell_scripts` table hooks into spell effects. Scripts fire when a spell with the matching effect is cast.

### Required Spell Effect

`spell_scripts` only fires for two spell effects:

- **SPELL_EFFECT_DUMMY** (effect ID 3) — `SpellEffects.cpp:814`
- **SPELL_EFFECT_SCRIPT_EFFECT** (effect ID 77) — `SpellEffects.cpp:4589`

Both call:
```cpp
m_caster->GetMap()->ScriptsStart(sSpellScripts, uint32(m_spellInfo->Id | (effIndex << 24)), m_caster, unitTarget);
```

The lookup key is `spellId | (effIndex << 24)`, where `effIndex` is 0, 1, or 2 (spell effect slot).

### Table Columns

| Column | Type | Description |
|--------|------|-------------|
| `id` | INT | Spell ID (lower 24 bits) |
| `delay` | INT | Delay in ms before executing (0 = immediate) |
| `command` | INT | Script command number |
| `datalong` | INT | Command-specific data |
| `datalong2` | INT | Command-specific data |
| `dataint` | INT | Command-specific data |
| `x`, `y`, `z`, `o` | FLOAT | Command-specific position data |
| `effIndex` | TINYINT | 0, 1, or 2 — which spell effect slot triggers this |

### SCRIPT_COMMAND_CREATE_ITEM (command 17)

Creates an item for a player.

| Column | Value |
|--------|-------|
| `datalong` | Item entry ID |
| `datalong2` | Quantity |
| `dataint` | Unused (0) |

**Requirements:** `datalong` must exist in item template store. `datalong2` must be > 0.

**Existing entries:**
```sql
-- spell 21014 → create item 34127 x1
(21014, 0, 0, 17, 34127, 1, 0, 0, 0, 0, 0),
-- spell 42287 → create item 33041 x1
(42287, 0, 0, 17, 33041, 1, 0, 0, 0, 0, 0),
```

**Execution flow:**
1. Looks for Player in source or target
2. Checks `CanStoreNewItem` for inventory space
3. If space: `StoreNewItem` + `SendNewItem` (visual/sound effects)
4. If no space: `SendEquipError` to player

### SCRIPT_COMMAND_DESPAWN_SELF (command 18)

Despawns the spell's target (usually the caster or a summoned creature).

| Column | Value |
|--------|-------|
| `datalong` | Delay in ms (0 = immediate) |
| `datalong2` | Unused (0) |

**Existing entries:** Many, typically `datalong=0` (immediate) or `datalong=500` (0.5s).

### SCRIPT_COMMAND_CAST_SPELL (command 15)

Casts a spell on a target.

| Column | Value |
|--------|-------|
| `datalong` | Spell ID to cast |
| `datalong2` | Target type bitmask |

**Existing entries:**
```sql
-- spell 25652 → cast spell 30141 on target type 2
(25652, 0, 0, 15, 30141, 2, 0, 0, 0, 0, 0),
-- spell 25650 → cast spell 30140 on target type 2
(25650, 0, 0, 15, 30140, 2, 0, 0, 0, 0, 0),
```

### SCRIPT_COMMAND_TEMP_SUMMON_CREATURE (command 10)

Temporarily summons a creature at a position.

| Column | Value |
|--------|-------|
| `datalong` | Creature entry ID |
| `datalong2` | Despawn time in ms |
| `x`, `y`, `z`, `o` | Spawn position |

### SCRIPT_COMMAND_KILL_CREDIT (command 8)

Gives kill credit to source/target player.

| Column | Value |
|--------|-------|
| `datalong` | Kill credit creature entry ID |

### SCRIPT_COMMAND_RESPAWN_GAMEOBJECT (command 9)

Respawns a gameobject.

| Column | Value |
|--------|-------|
| `datalong` | GO entry ID |

### SCRIPT_COMMAND_QUEST_EXPLORED (command 7)

Marks a quest as explored for the player.

| Column | Value |
|--------|-------|
| `datalong` | Quest ID |

### SCRIPT_COMMAND_TALK (command 0)

Creature says/emotes text.

| Column | Value |
|--------|-------|
| `datalong` | Text ID (DB2 reference) |

### SCRIPT_COMMAND_PLAY_SOUND (command 16)

Plays a sound for nearby players.

| Column | Value |
|--------|-------|
| `datalong` | Sound ID |

### SCRIPT_COMMAND_EMOTE (command 1)

Sets creature emote state.

| Column | Value |
|--------|-------|
| `datalong` | Emote ID |

## Spellclick + spell_scripts Combined Pattern

For harvest mechanics where clicking a creature should create an item:

1. **Creature setup:**
   - `creature_template`: entry, npcflag=16777216 (SPELLCLICK), AIName=SmartAI
   - `npc_spellclick_spells`: npc_entry + spell_id mapping
   - `conditions` (optional): type=9 (QUESTTAKEN) to gate by quest

2. **SAI on creature (event_type=73):**
   - Action 28: Remove aura from clicker (if applicable)
   - Action 33: Kill credit (if applicable)
   - Action 41: Despawn creature after delay

3. **spell_scripts on the spellclick spell:**
   - `command=17` (CREATE_ITEM): datalong=item_entry, datalong2=amount
   - `command=18` (DESPAWN_SELF): datalong=delay_ms (if despawning a summoned creature)

**Example:**
```sql
-- SAI: spellclick → remove aura → kill credit → despawn 10s
INSERT INTO `smart_scripts` (...) VALUES
  (<entry>, 0, 0, 0, 73, 0, 100, 1, 0, 0, 0, 0, 28, <aura_id>, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),
  (<entry>, 0, 1, 0, 73, 0, 100, 1, 0, 0, 0, 0, 33, <kill_credit_id>, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, ""),
  (<entry>, 0, 2, 0, 73, 0, 100, 0, 0, 0, 0, 0, 41, 10000, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, "");

-- spell_scripts: spellclick spell → create item
INSERT INTO `spell_scripts` (`id`, `delay`, `command`, `datalong`, `datalong2`, `dataint`, `x`, `y`, `z`, `o`, `effIndex`)
VALUES (<spell_id>, 0, 17, <item_entry>, <amount>, 0, 0, 0, 0, 0, 0);

-- Optional: despawn self after delay
INSERT INTO `spell_scripts` (`id`, `delay`, `command`, `datalong`, `datalong2`, `dataint`, `x`, `y`, `z`, `o`, `effIndex`)
VALUES (<spell_id>, 1000, 18, 0, 0, 0, 0, 0, 0, 0, 0);
```

## Chaining Scripts

Scripts chain via the `link` field:

- `id=0, link=1 → id=1, link=2 → id=2, link=0` (end)
- When action at `id=N` completes, it triggers `id=link`
- `link=0` at the end means stop (no chaining)
- `link` can also point to a different `entryorguid` for cross-entity chaining

**Example (linear chain):**
```sql
-- id=0 links to id=1, id=1 links to id=2, id=2 ends
(58646, 0, 0, 1, 8, 0, 100, 0, 126987, 0, 0, 0, 0, 33, 64942, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, ""),
(58646, 0, 1, 0, 61, 0, 100, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 0, "On spellhit 126987 - talk 0"),
```

**Independent actions (no chaining):**
```sql
-- Each id fires independently, no link chain
(59987, 0, 0, 0, 73, 0, 100, 1, 0, 0, 0, 0, 28, 115824, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),
(59987, 0, 1, 0, 73, 0, 100, 1, 0, 0, 0, 0, 33, 59987, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, ""),
(59987, 0, 2, 0, 73, 0, 100, 0, 0, 0, 0, 0, 41, 10000, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, ""),
```

## Loading and Reloading

SAI entries are loaded from `smart_scripts` table at server startup. They can be reloaded at runtime:

```
reload smart_scripts
```

This is handled by `cs_reload.cpp` and calls `SmartScriptMgr::LoadSmartScripts()`.

## Key Implementation Files

| File | Role |
|------|------|
| `src/server/game/AI/SmartScripts/SmartAI.cpp` | SmartAI class, OnSpellClick handler |
| `src/server/game/AI/SmartScripts/SmartScript.cpp` | ProcessEventsFor, ProcessAction |
| `src/server/game/AI/SmartScripts/SmartScriptMgr.h` | Event/action type enums |
| `src/server/game/Conditions/ConditionMgr.cpp` | Spellclick condition checking |
| `src/server/game/Scripting/MapScripts.cpp` | SCRIPT_COMMAND execution |
| `src/server/game/Globals/ObjectMgr.cpp` | spell_scripts loading + validation |
| `src/server/game/Spells/SpellEffects.cpp` | Trigger points for spell_scripts |
| `src/server/scripts/Commands/cs_reload.cpp` | reload smart_scripts command |
