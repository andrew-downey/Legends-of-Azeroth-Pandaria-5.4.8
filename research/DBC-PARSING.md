# DBC/DB2 Parsing Reference

Practical findings from parsing WoW 5.4.8 DBC/DB2 files for battle pet research.

## File Format Detection

Check the 4-byte magic at offset 0:

| Magic | Format | Used In |
|-------|--------|---------|
| `WDB2` | DBC (flat) | `BattlePetSpecies.db2`, older formats |
| `WDC1` | WDC v1 | Cata/MoP transition format |
| `WDB5` | WDB v5 | Some MoP DB2s |
| `WDC2` | WDC v2 | Later expansions (not in 5.4.8) |
| `WCH6` | WCH v6 | Not used in 5.4.8 |
| `WDBC` | Classic DBC | Legacy files |

## WDB2 Header Layout (20 bytes)

```
Offset  Size  Field            Example (BattlePetSpecies.db2)
------  ----  ---------------- --------------------------------
0       4     magic            "WDB2"
4       4     records_count    845                     (0x34D)
8       4     fields_count     9
12      4     record_size      36  (must be fields × 4)
16      4     string_table_size 101209                 (0x18B59)
```

After header: `records_count × record_size` bytes of data, then `string_table_size` bytes of string data.

## BattlePetSpecies.db2 — Field Layout

**Fields per record (9 uint32, 36 bytes):**

| Index | Field | Example (species 634 = Crystal Spider) |
|-------|-------|----------------------------------------|
| 0 | **Hash/ID** | `463569` — appears to be a hash or alternate ID |
| 1 | **Flags bitmask** | `0` — 0=common, 4=?, 6=?, 7=?, 8=? |
| 2 | **???** | `4` — possibly family/type |
| 3 | **???** | `4` — possibly sub-type |
| 4 | **MinLevel** | `26` — minimum pet battle level (this is the DBC level, not the zone level) |
| 5 | **Description offset** | offset into string table → "earance like burnt cinders." |
| 6 | **Location/Zone string offset** | offset into string table → "aks, Wetlands, Winterspring" |
| 7 | **Species ID** | `634` — primary key, matches `battle_pet_wild_pool.species` |
| 8 | **Creature ID** | `62435` — FK to `creature_template.entry`, used for type-14 NPC spawns |

**Confirmed examples:**

| Species | Creature | Name | Field[6] (zone ref) |
|---------|----------|------|---------------------|
| 554 | 62922 | Crimson Shale Hatchling | `FD200Pet Battle: \|rDeepholm` |
| 555 | 62924 | Deepholm Cockroach | `of a rat was a welcome one.` |
| 556 | 62925 | Crystal Beetle | `ation of their fire spells.` |
| 559 | 62927 | Crimson Geode | `\|n\|cFFFFD200Weather: \|rSnow` |
| 634 | 62435 | Crystal Spider | `aks, Wetlands, Winterspring` |
| 837 | 62915 | Emerald Shale Hatchling | `200Zone:\|r Dread Wastes\|n\|n` |
| 838 | 62182 | Amethyst Shale Hatchling | `ons, and even a family pet.` |
| 470 | 62117 | Twilight Spider | `hara, Deepholm, Mount Hyjal` |
| 480 | 62181 | Topaz Shale Hatchling | `ce, Mount Hyjal, Teldrassil` |
| 756 | 62916 | Fungal Moth | `Pet Battle:\|r Mount Hyjal\|n` |

## Reading the String Table

String table starts at offset `20 + records_count × record_size`. Strings are null-terminated and referenced by their byte offset within the string table.

```python
def read_string(offset, string_table_start, data):
    end = data[string_table_start + offset:].index(b'\x00')
    return data[string_table_start + offset:
                string_table_start + offset + end].decode('utf-8', errors='replace')
```

Note: Strings often have truncated prefixes because earlier strings overlap. Example: field[6] for species 634 returns `aks, Wetlands, Winterspring` — the full string is likely `"Spawns in Wetlands, Winterspring"` but the string table starts mid-word due to how the preceding string ends.

## Python Parsing Snippet

```python
import struct

with open('/path/to/BattlePetSpecies.db2', 'rb') as f:
    data = f.read()

records_count = struct.unpack_from('<I', data, 4)[0]
fields_count  = struct.unpack_from('<I', data, 8)[0]
record_size   = struct.unpack_from('<I', data, 12)[0]
string_offset = 20 + records_count * record_size

for rec_idx in range(records_count):
    offset = 20 + rec_idx * record_size
    vals = struct.unpack_from(f'<{record_size // 4}I', data, offset)
    species_id = vals[7]    # field[7] = species ID
    creature_id = vals[8]   # field[8] = creature entry (type-14 NPC)
    
    # Read location string from field[6]
    loc_off = vals[6]
    end = data[string_offset + loc_off:].index(b'\x00')
    loc_desc = data[string_offset + loc_off:
                     string_offset + loc_off + end].decode('utf-8', errors='replace')
    
    print(f"species={species_id} creature={creature_id} desc={loc_desc}")
```

## Cross-Referencing with SQL

### BattlePetSpecies.db2 ↔ battle_pet_wild_pool

The `battle_pet_wild_pool` table joins with the DB2 via:
- `species` column → `[7]` in DB2 record
- `entry` column → type-8 critter NPC (for type-8 replacement) OR type-14 NPC (for type-14 direct spawn)

### creature_template.type interpretations

| type | Meaning | Used For |
|------|---------|----------|
| 8 | Critter | Type-8 replacement base (existing world spawns) |
| 14 | Battle Pet | Wild pet NPC (MoP-style direct spawns) |
| 1 | Beast | Normal creatures |
| 7 | Humanoid | NPCs |

### Flow: Type-8 Replacement

1. `creature` table has type-8 critter spawns (e.g., 49771 Crystal Beetle, map 646)
2. `battle_pet_wild_pool` maps `entry=49771` to `species=556`
3. Runtime: `BattlePetSpawnMgr` detects critter in zone, replaces it with battle pet version

### Flow: Type-14 Direct Spawn

1. No matching type-8 critter spawns in zone
2. `creature` table gets new spawns with `id=62925` (the type-14 NPC)
3. `battle_pet_wild_pool` maps `entry=62925` to `species=556`
4. Runtime: `BattlePetSpawnMgr` spawns the type-14 NPC with `WILDPET_CAPTURABLE` flag

## Other DBC/DB2 Files of Interest

| File | Path | Purpose |
|------|------|---------|
| `BattlePetSpecies.db2` | `Data/dbc/` | Species → NPC mapping (this doc) |
| `BattlePetSpeciesState.db2` | `Data/dbc/` | Per-species default states |
| `BattlePetSpeciesXAbility.db2` | `Data/dbc/` | Species → ability mappings |
| `BattlePetAbilityEffect.db2` | `Data/dbc/` | Ability effect parameters |
| `Spell.dbc` | `Data/dbc/` | Spell definitions (203K entries) |
