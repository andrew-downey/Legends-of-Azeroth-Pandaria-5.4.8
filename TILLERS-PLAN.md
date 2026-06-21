---
status: in-progress
phase: 2
updated: 2026-06-20
---

# Tillers Farm Implementation Plan

## Goal
Complete the Tillers farm system for Sunsong Ranch to retail 5.4.8 behavior.

## Context & Decisions
| Decision | Rationale | Source |
|----------|-----------|--------|
| Retail accuracy always | Project philosophy mandates retail behavior | `ref:tillers-code-review` |
| Per-NPC `IsBestFriend(factionId)` | Retail tracks 10 individual NPC friendships, not just Tillers faction | `ref:tillers-code-review` |
| Phase 1 first (low effort) | Close docs/code gaps before tackling high-effort mechanic implementations | `ref:tillers-code-review` |

## Phase 1: Complete Scene Builder [COMPLETED]
- [x] **1.1 Verify companion DB entries** — Verified creature/GO table for all 10 companion entries on map 870 via DB query + DBC verification of faction entries 1275–1283
- [x] 1.2 Wire `SpawnBestFriendUnlock()` into `SpawnPlayerFarm()` — Added `SpawnPlayerFarmCompanions()` call after state load at line 460; tracks both creature and GO companions in `_playerBestFriendUnlocks`
- [x] 1.3 Switch `IsBestFriend()` to per-NPC friendship faction checks (factions 1275–1283) — Replaces single-faction 1934 check with per-NPC friendship faction

## Phase 2: Core Farming [IN PROGRESS]
- [x] **2.1 Fix `HarvestCrop` rewards** — 5 vegetables (retail base yield), seed→vegetable item mapping (9 seed types), Tillers rep gain per harvest (125→15 scaling by rank, later revised to flat 50 at 90)
- [ ] **2.2 Complete planting→harvest retail cycle** ← CURRENT
  - [ ] 2.2a — Remove `lastGrowthTick` from struct/DB (replaced by WorldScript daily reset hook)
  - [ ] 2.2b — WorldScript: daily reset batch UPDATE advancing all SEEDED/GROWING crops
  - [ ] 2.2c — In-zone visual refresh for players in VFW after daily reset
  - [ ] 2.2d — Bursting crops (~1.1% instant harvest on planting)
  - [ ] 2.2e — Crop problem states (~88% chance: water/pest on planting)
  - [ ] 2.2f — Seed return on harvest (50% chance, 1–3 seeds)
  - [ ] 2.2g — Plump crop bonus (~5% chance of 8 veg)
  - [ ] 2.2h — Flat 50 Tillers rep at level 90 on harvest, 0 below 90
- [ ] 2.3 Seed system — map actual item entries to spells for all seeds
- [ ] 2.4 Crop tending (8 problems) — minigame implementations
- [ ] 2.5 Post-harvest states — Occupied, Stubborn
- [ ] 2.6 Seed bags — 4-plot planting

## Phase 3: Tillers Union Questline [PENDING]
- [ ] 3.1 Learn and Grow chain scripts
- [ ] 3.2 Obstacle clearing SAI
- [ ] 3.3 Vote quest chains (Mung-Mung, Farmer Fung, Nana, Haohan)
- [ ] 3.4 Farm machinery item scripts

## Phase 4: Friendship System [PENDING]
- [ ] 4.1 Friendship bar display
- [ ] 4.2 Food gift daily (5x favorite dish → +1800)
- [ ] 4.3 Dark Soil gifts (540/900 per gift)
- [ ] 4.4 Friendship rank quests
- [ ] 4.5 Best Friend mail rewards
- [ ] 4.6 Hire Best Friend

## Phase 5: Dailies, Work Orders & Achievements [PENDING]
- [ ] 5.1 Farmer Yoon planting daily
- [ ] 5.2 Andi gift daily
- [ ] 5.3 Farmer dailies
- [ ] 5.4 Killing dailies
- [ ] 5.5 Work Orders (post-ownership)
- [ ] 5.6 Achievements
- [ ] 5.7 Ranch customization
- [ ] 5.8 Quartermaster vendor

## Notes
- 2026-06-20: Code review completed. `SpawnBestFriendUnlock()` defined but never called. `IsBestFriend()` uses Tillers faction 1934 (simplified). `HarvestCrop` gives 1 item instead of retail 5. Crop minigames, friendship, dailies all stubs. Plan created from current state. `ref:tillers-code-review`
- 2026-06-20: Phase 1 completed — companion DB migration (2026_06_20_01_tillers_companions.sql), scene builder wired, per-NPC friendship checks. Phase 2.1 completed — seed→vegetable mapping with 9 types, 5-veg base yield, Tillers rep scaling. SQL migration for companion entries created and applied. `ref:tillers-build`
- 2026-06-21: Adversarial review found retail discrepancies in Phase 2.2 plan. `lastGrowthTick` replaced with WorldScript daily reset hook. Added bursting crops, problem states, seed return, plump bonus, flat 50 rep. Phase 2.2 revised to cover complete planting→harvest retail cycle. `ref:tillers-adversarial-review`

