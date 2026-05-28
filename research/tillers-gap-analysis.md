# Tillers System — Gap Analysis vs Retail (Patch 5.4.8)

Last updated: 2026-05-27

## Summary

The Tillers Farm system is functional with core farming mechanics, friendship reputation, daily quests, and vote progression implemented. However, many retail features from patches 5.1, 5.2, and the base 5.0 system are missing. This document catalogs every gap.

## Legend

| Priority | Meaning |
|----------|---------|
| **P0** | Core retail feature — players expect this; breaks progression |
| **P1** | Major retail feature — significant gameplay impact |
| **P2** | Notable retail feature — moderate gameplay/immersion impact |
| **P3** | Polish/immersion — cosmetic or convenience |

---

## FULLY MISSING FEATURES

### P0 — Core Progression

| # | Feature | Retail Description | Notes |
|---|---------|-------------------|-------|
| 1 | **Inherit the Earth questline** (Quest 32682) | After reaching Exalted + all 16 plots, player inherits Sunsong Ranch from Farmer Yoon. He joins Tillers Council. Sets Hearth at farm via Farmer's Journal. Unlocks Work Orders. | No C++ code, no quest entries in SQL migrations. Listed as Phase 4C. |
| 2 | **Work Orders bulletin board** | Post-Inherit, a bulletin board appears on the farm. Players plant specific crops and turn them in for rep with other Pandaria factions: Green Cabbage → Shado-Pan, Juicycrunch Carrot → Kirin Tor Offensive, Mogu Pumpkin → Klaxxi, Red Blossom Leek → Dominance Offensive, Scallion → Sunreaver Onslaught, Striped Melon → August Celestials, White Turnip → Golden Lotus, Witchberry → Operation: Shieldwall. | Requires Inherit the Earth first. No C++ code. |

### P1 — Friendship Rewards

| # | Feature | Retail Description | Notes |
|---|---------|-------------------|-------|
| 3 | **Best Friend mail rewards** | When reaching Best Friend with each NPC, the NPC sends a mail with unique items and adds cosmetic creatures/objects to the farm: | No SendMail code exists. No item reward IDs in code. |
|   | Chee Chee | Sends [Chee Chee's Goodie Bag] (cloth+leather). Adds sheep to farm. | |
|   | Ella | Sends [Tree Seed Pack] (blossom seeds). Adds "Luna" cat to farm. | |
|   | Farmer Fung | Sends [Enigma Seed]. Adds "Shaggy the Prize Yak" to farm. | |
|   | Fish Fellreed | Sends [Special Seed Pack] (profession seeds). Adds 3 pink pigs to farm. | |
|   | Gina Mudclaw | Sends [Celebration Gift] (fireworks). Adds a mailbox at farm entrance. | |
|   | Haohan Mudclaw | Sends 3x [Songbell Seed]. Adds "Miss Fifi" mushan pet to farm. | |
|   | Jogu the Drunk | Sends [Secret Stash] (alcohol). Crop predictions become free. | |
|   | Old Hillpaw | Sends [Straw Hat] (cloth/leather/mail/plate). Adds chickens to farm. | |
|   | Sho | Sends [Chirping Package] containing [Red Cricket] pet. Adds orange tree to farm. | |
|   | Tina Mudclaw | Sends [Nicely Packed Lunch] (food). Adds furniture (bed, stove) to farm house. | |
| 4 | **Best Friend bonus seeds** | When Best Friends with NPCs, planting vegetable crops yields bonus profession seeds (Snakeroot, Enigma, etc.) instead of just the same vegetable seeds. Dark Soil also stops dropping. | No Best Friend bonus seed code exists. |
| 5 | **Friendship-level-gated quests** | Each NPC has quests that unlock at specific friendship thresholds: | No friendship-rank-gating on quests. |
|   | Chee Chee | Lost Sheepie (Buddy, +3750), Lost Sheepie... Again (Friend, +3750), Oh Sheepie... (Good Friend, +3750), A Wolf In Sheep's Clothing (Good Friend, +3750) | |
|   | Ella | The Beginner's Brew (Buddy, +5000), Ella's Taste Test (Friend, +5000), A Worthy Brew (Good Friend, +5000) | |
|   | Jogu | Buy A Fish A Drink? (Acquaintance, +1000), Buy A Fish A Round? (Buddy, +3000), Buy A Fish A Keg? (Friend, +5000), Buy A Fish A Brewery? (Good Friend, +6000) | |
|   | Sho | Mission: Aerial Threat, Mission: Culling The Vermin (Buddy, +3750), Mission: Predator of the Cliffs, Mission: The Hozen Dozen | |
|   | Tina | Nice Necklace (86433), A Very Nice Necklace, Tasteful Tiara (86434), Exquisite Earring (86435), Beautiful Brooch (86436) — each gives +3750 friendship | Nowhere in code. |
| 6 | **Farm visuals from Best Friends** | Cosmetic NPCs/animals appear on the player's farm when reaching Best Friend with each NPC. Each NPC adds a unique visual element (chickens, cat, pigs, sheep, yak, mushan, orange tree, furniture, mailbox). | No creature-summon or visual-change code for farm. |

### P1 — Jogu Prediction System

| # | Feature | Retail Description | Notes |
|---|---------|-------------------|-------|
| 7 | **Jogu's crop prediction** | Jogu the Drunk has a gossip option: "Tell me about today's crops." He predicts which crop will get +2 bonus yield tomorrow. Cost ranges from 25g (Stranger) → 10g → 1g → 10s → free (Best Friend). | No prediction gossip. BonusCropSchedule data array exists and is correctly used for the yield bonus on harvest, but players have no way to query it. |

### P1 — Convenience Items

| # | Feature | Retail Description | Notes |
|---|---------|-------------------|-------|
| 8 | **Seed bags** | Items like "Planting Pouch: Green Cabbage" added in 5.2. Plant all 16 plots at once with 4 uses (4 plots at a time). Purchased from Merchant Greenfield for ~27-30g. | No seed bag items or multi-plant logic exists. |

### P1 — Andi Gift Daily (BROKEN)

| # | Feature | Retail Description | Notes |
|---|---------|-------------------|-------|
| 9 | **Andi gift delivery daily** | Andi gives quest 32010 to deliver a gift (item 79247) to another Tiller NPC. Rewards +150 Tillers rep + 1400 friendship. | Constants exist, quest entries exist in SQL, BUT: (1) SQL migration binds ScriptName `npc_andi_gift` to NPC 58708 which is **Sho**, not Andi (Andi is 64464). (2) No C++ file named `npc_andi_gift` exists anywhere. The script would fail to load at runtime. |

### P2 — Reputation Rewards

| # | Feature | Retail Description | Notes |
|---|---------|-------------------|-------|
| 10 | **Pandaren Scarecrow** (item 89869) | Purchased from Merchant Greenfield at Revered for 250g. Prevents Runty crops on the farm when placed. Has a "Pandaren Scarecrow" visual effect. | No scarecrow script. Item exists in DB (locale data only), no C++ code. |
| 11 | **Grand Commendation of the Tillers** (item 93226) | Purchased at Revered for 50g. Bind-to-Account. Use to gain 100% bonus rep with Tillers on all characters. | No commendation script. Item exists in DB (locale data only). |
| 12 | **Reins of goats** (brown 89362, black 89391, white 89390) | Mount rewards at Exalted. Should work automatically via DB items. | Verify item_template entries exist. |
| 13 | **Gin-Ji Knife Set** (90175), **Tillers Tabard** (89784) | Exalted rewards. Tabard shows faction crest. | Verify item_template entries. |

### P2 — Achievements

| # | Feature | Retail Description | Notes |
|---|---------|-------------------|-------|
| 14 | **Till the Break of Dawn** | Harvest 100 crops. | No achievement tracking code. |
| 15 | **Green Acres** | Harvest your first crop. | |
| 16 | **Friend on the Farm** | Reach Best Friend with 1 Tiller NPC. | |
| 17 | **Friends on the Farm** | Reach Best Friend with all 10 Tiller NPCs. | |
| 18 | **Listen to the Drunk Fish** | Harvest each vegetable crop on a high-forecast (bonus) day. Requires Jogu prediction system. | |
| 19 | **A Taste of Things to Come** | Harvest your first special crop (Plump/Bursting). | |
| 20 | **Ain't Lost No More** | Return Old Man Thistle's lost treasure. | |
| 21 | **The Tillers** | Reach Exalted with The Tillers. | |

### P3 — Minor Retail Features

| # | Feature | Retail Description | Notes |
|---|---------|-------------------|-------|
| 22 | **Master Plow vs Virmen** | Running Master Plow across underground Virmen pops them out at 30% health + stunned. Till + vermin combat. | Master Plow item 89815 is given as tool, but has no special use script. |
| 23 | **Unstable Portal Shard** (item 91806, patch 5.1) | Grow a portal shard on farm that turns into one of four capital city portals (random). | No portal shard code. |
| 24 | **Portable Refrigerator** (item 92748, patch 5.1) | Purchased for 10 Ironpaw Tokens. Provides mail-like interaction when placed on the farm. | No refrigerator code. |
| 25 | **"Lost and Lonely" quest** | Dog quest north of Halfhill. Gives a dog on the farm. | Not in code. |
| 26 | **Yoon's Mailbox** → **Sunsong Ranch Mailbox** | Renamed mailbox after Inherit the Earth. | Not relevant until Inherit is done. |
| 27 | **Old Man Thistle's treasure** | "Ain't Lost No More" achievement questline. | Not in code. |

---

## PARTIALLY IMPLEMENTED FEATURES

### P1 — Vote System

| What's Done | What's Missing |
|-------------|----------------|
| Vote bitmask, VoteRequirements data array with NPC/item/rep thresholds | In retail, each vote has a full quest chain (3-5 quests each: Mung-Mung's Vote I/II/III, Farmer Fung's Vote I/II/III, etc.). Current implementation uses custom gossip-based turn-in (no quests). Quest IDs 32025-32029 were defined but file was deleted & consolidated into friendship NPC. |
| 5 voting NPCs (Gina, Mung-Mung, Farmer Fung, Nana, Haohan) with gossip in npc_tillers_friendship.cpp | Vote quests should give reputation rewards as quest completion (standard system). Custom gossip bypasses standard quest rewards. |
| Rep thresholds corrected to retail values (4200/14600/25200/29400/37800) | The vote quest chain order in retail is: Gina (Neutral/Friendly), Mung-Mung (~5.6k Honored), Farmer Fung (~4.2k Revered), Nana (~8.4k Revered), Haohan (~16.8k Revered leading to Exalted). Verify code order matches. |

### P1 — Andi Gift Daily

| What's Done | What's Missing |
|-------------|----------------|
| QUEST_ANDI_GIFT = 32010 constant | No C++ script (`npc_andi_gift`) — SQL ScriptName will fail at runtime |
| ITEM_ANDI_GIFT = 79247 constant | SQL binds to NPC 58708 (Sho) instead of 64464 (Andi) |
| NPC_ANDI = 64464 constant | The delivery-to-target-NPC mechanic is not scripted |
| SQL quest starter/ender bindings exist | The gift delivery interaction (gossip on target NPC to turn in gift) is not implemented |
| Farmer Yoon rotation includes this quest | |

### P2 — BonusCropSchedule

| What's Done | What's Missing |
|-------------|----------------|
| 10-element BonusCropSchedule array (correct crop rotation) | Jogu's prediction gossip to allow players to query which crop is bonus today |
| Used in harvest logic for +2 bonus yield on matching days | No gold fee system based on friendship rank |
| `GetTodaySeed()` = `time(nullptr)/86400` | No "Listen to the Drunk Fish" achievement |

### P2 — Learn and Grow Chain

| What's Done | What's Missing |
|-------------|----------------|
| Quest 30257 (Learn and Grow V) used as farm-creation gate | Learn and Grow I (30252), II (30254), III (30255), IV (30256) are not referenced in code |
| Auto-creation on login if quest is completed | Quest VI: Gina's Vote (31945) — not in code |

### P2 — Growing the Farm (Expansion) Chain

| What's Done | What's Missing |
|-------------|----------------|
| 15-minute real-time expansion timer (expansion_timer_end column) | Specific quest IDs 30260, 30516, 30523, 30524, 30529 not used |
| Farmer Yoon gossip bypasses quests with reputation check | No quest starter/ender bindings for these quests (verify SQL) |
| Three expansion tiers with correct plot counts | In retail, each expansion is gated by a specific quest chain that awards reputation |

---

## KNOWN BUGS (Discovered during analysis)

| Bug | Details | Severity |
|-----|---------|----------|
| **Andi gift script bound to wrong NPC** | SQL migration `2026_05_25_05_tillers_andi_gift_script.sql` sets ScriptName = 'npc_andi_gift' on NPC entry 58708 (which is **Sho**, not Andi). Andi is entry 64464. | **Critical** — script will load on wrong NPC, fail on correct NPC |
| **NPC_ANDI gift comment** | tillers_farm.h:338 says "Andi gift item (given by NPC 64464)" — comment is correct but check if SQL migration was fixed | Verify |
| **No npc_andi_gift.cpp exists** | Despite ScriptName in SQL, no C++ file implements this script. The Andi gift daily is non-functional. | **Critical** — feature is dead code |
| **Vote quests are gossip-based, not quest-based** | Retail uses standard quests (creature_queststarter/ender). Current implementation uses custom gossip in friendship NPC. This means standard quest rewards/tracking don't apply. | **Medium** — functional but non-standard |
| **Farm expansion bypasses Growing the Farm quests** | Players expand plots via Yoon gossip with rep check, without completing the quest chain. This means they skip the quest rewards and narrative. | **Low** — functional but not retail-accurate |
| **BonusCropSchedule has no player-facing interaction** | The schedule works automatically for yield bonus, but players have no way to know which crop is bonus today without Jogu. | **Low** — game mechanic works, UX feature missing |

---

## Implementation Priority Matrix

### Phase 4C — Inherit the Earth + Work Orders (P0)
- Inherit the Earth questline (32682)
- Work Orders bulletin board + faction crop map
- Sunsong Ranch ownership changes
- Hearth setting at farm

### Phase 4E — Friendship Rewards (P1)
- Best Friend mail rewards for all 10 NPCs (SendMail with item attachments)
- Farm visual elements (summon creatures/objects on Best Friend)
- Best Friend bonus seed mechanic in harvest logic
- Friendship-level-gated quests

### Phase 4F — Jogu Prediction System (P1)
- Jogu gossip option for crop prediction
- Gold fee scaling by friendship rank
- "Listen to the Drunk Fish" achievement

### Phase 4G — Convenience & Fixes (P1)
- Andi gift daily: fix ScriptName binding, create npc_andi_gift.cpp script
- Seed bags: multi-plot planting items
- Master Plow vs Virmen special behavior

### Phase 4H — Reputation Rewards (P2)
- Pandaren Scarecrow script (89869)
- Grand Commendation of the Tillers (93226)
- Verify goat mounts, tabard, knife set exist in DB

### Phase 4I — Achievements (P2)
- All 7 Tillers-related achievement trackers

### Phase 4J — Minor Retail Features (P3)
- Unstable Portal Shard (91806)
- Portable Refrigerator (92748)
- "Lost and Lonely" dog quest
- Old Man Thistle's treasure
- Sunsong Ranch Mailbox rename

---

## Verification Queries Needed

To fully assess remaining work, run these SQL queries against the world database:

1. `SELECT entry, ScriptName FROM creature_template WHERE entry IN (58708, 64464);` — Check Andi/Sho ScriptName bindings
2. `SELECT * FROM creature_queststarter WHERE quest = 32010;` — Check Andi gift quest starter
3. `SELECT * FROM creature_questender WHERE quest = 32010;` — Check Andi gift quest ender
4. `SELECT entry, name, ScriptName FROM creature_template WHERE ScriptName LIKE '%andi%';` — Find all Andi-related scripts
5. `SELECT entry FROM item_template WHERE entry IN (89869, 93226, 91806, 92748, 89362, 89391, 89390, 90175, 89784);` — Verify reward items exist
6. `SELECT * FROM creature_queststarter WHERE quest IN (30260, 30516, 30523, 30524, 30529);` — Check Growing the Farm bindings
7. `SELECT * FROM creature_queststarter WHERE quest IN (30252, 30254, 30255, 30256, 31945);` — Check Learn and Grow bindings
