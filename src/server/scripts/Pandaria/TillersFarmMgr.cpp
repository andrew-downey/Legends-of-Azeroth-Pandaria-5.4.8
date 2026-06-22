/*
 * This file is part of the Pandaria 5.4.8 Project. See THANKS file for Copyright information
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "TillersFarmMgr.h"
#include "Creature.h"
#include "GameObject.h"
#include "Log.h"
#include "ObjectAccessor.h"
#include "Player.h"
#include "ReputationMgr.h"
#include "ScriptMgr.h"
#include "TemporarySummon.h"
#include "World.h"

// Ground-level Farmer Yoon (58646) dynamically spawned in each player's farm phase.
// Tools Yoon (58721) and farmhouse Yoon (58646) remain at phaseMask=1 as static world
// spawns for pre-quest visibility — farm players won't see them due to phase mismatch.

// ============================================================================
// TillersFarmMgr Implementation
// ============================================================================

time_t TillersFarmMgr::GetMaturityTime(uint32 /*seedEntry*/, uint8 /*plotId*/)
{
    // Mature at next daily quest reset time (~03:00 server time).
    // This matches retail: crops planted today are harvestable after the next daily reset.
    time_t nextReset = sWorld->GetNextDailyQuestsResetTime();
    TC_LOG_DEBUG("scripts", "TillersFarmMgr: Seed planted - matures at next daily reset (%lu)",
                 static_cast<unsigned long>(nextReset));
    return nextReset;
}

bool TillersFarmMgr::LoadPlayerState(Player* player)
{
    uint32 guidLow = player->GetGUID().GetCounter();

    QueryResult result = CharacterDatabase.PQuery(
        "SELECT farm_phase, plots_unlocked, best_friend_unlocks FROM player_farm_state WHERE guid = %u",
        guidLow);

    if (result)
    {
        Field* fields = result->Fetch();
        uint8 phase   = fields[0].GetUInt8();
        uint8 plots   = fields[1].GetUInt8();
        uint16 bestFriendUnlocks = fields[2].GetUInt16();

        // Validate loaded state
        if (phase != FARM_STATE_FULL && phase != FARM_STATE_WEEDS_CLEARED && phase != FARM_STATE_WAGON_CLEARED && phase != FARM_STATE_ALL_CLEARED)
        {
            TC_LOG_ERROR("scripts", "TillersFarmMgr: Player %u has invalid farm_phase %u, resetting to defaults", guidLow, phase);
            return false;
        }

        // plots_unlocked must be 4, 8, 12, or 16
        if ((plots != 4 && plots != 8 && plots != 12 && plots != 16) || plots > TILLERS_MAX_PLOTS)
        {
            TC_LOG_ERROR("scripts", "TillersFarmMgr: Player %u has invalid plots_unlocked %u, resetting to defaults", guidLow, plots);
            return false;
        }

        PlayerFarmState state;
        state.farmState     = phase;
        state.plotsUnlocked = plots;
        state.bestFriendUnlocks = bestFriendUnlocks;

        _playerStates[guidLow] = state;

        // Load plot data
        QueryResult plotResult = CharacterDatabase.PQuery(
            "SELECT plot_id, state, seed_entry, needs_watering, has_pests, maturity_timestamp FROM player_farm_plots WHERE guid = %u ORDER BY plot_id",
            guidLow);

        if (plotResult)
        {
            PlotMap& plots = _playerPlots[guidLow];
            do
            {
                Field* pf = plotResult->Fetch();
                uint8  pid   = pf[0].GetUInt8();

                if (!IsValidPlotId(pid))
                    continue;

                FarmPlotData plot;
                plot.plotId      = pid;
                plot.state       = static_cast<FarmPlotState>(pf[1].GetUInt8());
                plot.seedEntry   = pf[2].GetUInt32();
                plot.needsWatering  = pf[3].GetBool();
                plot.hasPests      = pf[4].GetBool();
                plot.maturityTimestamp = static_cast<time_t>(pf[5].GetUInt64());

                plots[pid] = plot;
            } while (plotResult->NextRow());

            TC_LOG_INFO("scripts", "TillersFarmMgr: Player %u loaded farm state phase=%u, plots=%zu", guidLow, phase, plots.size());
        }

        return true;
    }

    // No existing state - create defaults
    PlayerFarmState defaultState;
    defaultState.farmState     = FARM_STATE_FULL;
    defaultState.plotsUnlocked = 4;

    _playerStates[guidLow] = defaultState;

    PlotMap& plots = _playerPlots[guidLow];
    for (uint8 i = 0; i < TILLERS_MAX_PLOTS; ++i)
    {
        FarmPlotData plot;
        plot.plotId      = i;
        plot.state       = PLOT_EMPTY;
        plot.seedEntry   = 0;
        plots[i] = plot;
    }

    TC_LOG_INFO("scripts", "TillersFarmMgr: Player %u created new default farm state", guidLow);
    return true;
}

void TillersFarmMgr::CreateSoilGos(Player* player, uint8 plotsCount, uint32 phaseMask)
{
    if (!player || !player->IsInWorld())
        return;

    Map* map = player->GetMap();
    if (!map)
        return;

    uint32 guidLow = player->GetGUID().GetCounter();

    for (uint8 i = 0; i < plotsCount && IsValidPlotId(i); ++i)
    {
        PlotPosition pos;
        if (!GetPlotPosition(i, pos))
        {
            TC_LOG_DEBUG("scripts", "TillersFarmMgr: No plot position for plotId %u, skipping soil GO creation", i);
            continue;
        }

        GameObject* soilGo = new GameObject;
        if (!soilGo->Create(map->GenerateLowGuid<HighGuid::GameObject>(), SOIL_GO_ENTRY, map,
            phaseMask, pos.posX, pos.posY, pos.posZ, pos.orientation, { }, 100, GO_STATE_READY))
        {
            delete soilGo;
            continue;
        }

        soilGo->SetOwnerGUID(player->GetGUID());
        soilGo->SetPrivateObjectOwner(player->GetGUID());
        soilGo->SetSpellId(static_cast<uint32>(i));  // plotId stored as SpellId

        map->AddToMap(soilGo);
        _playerSoilGOs[guidLow].push_back(soilGo->GetGUID());
    }

    TC_LOG_DEBUG("scripts", "TillersFarmMgr: Created %u soil GOs for player %u (plotsCount=%u)",
                 static_cast<uint32>(_playerSoilGOs[guidLow].size()), guidLow, plotsCount);
}

void TillersFarmMgr::RemoveSoilGos(Player* player)
{
    if (!player || !player->IsInWorld())
        return;

    Map* map = player->GetMap();
    if (!map)
        return;

    uint32 guidLow = player->GetGUID().GetCounter();
    auto it = _playerSoilGOs.find(guidLow);
    if (it == _playerSoilGOs.end())
        return;

    for (ObjectGuid const& guid : it->second)
    {
        GameObject* go = map->GetGameObject(guid);
        if (go)
            go->ForcedDespawn();
    }

    TC_LOG_DEBUG("scripts", "TillersFarmMgr: Removed %zu soil GOs for player %u", it->second.size(), guidLow);
    _playerSoilGOs.erase(it);
}

void TillersFarmMgr::SpawnYoon(Player* player, uint32 phaseMask)
{
    if (!_yoonSpawnData.loaded)
    {
        TC_LOG_ERROR("scripts", "TillersFarmMgr: Yoon spawn data not loaded, cannot spawn");
        return;
    }

    if (Creature* yoon = player->SummonCreature(FARMER_YOON_ENTRY,
        _yoonSpawnData.posX, _yoonSpawnData.posY, _yoonSpawnData.posZ,
        _yoonSpawnData.orientation, TEMPSUMMON_MANUAL_DESPAWN))
    {
        yoon->SetPhaseMask(phaseMask, true);
        yoon->SetPrivateObjectOwner(player->GetGUID());
        _playerSpawnedCreatures[player->GetGUID().GetCounter()].push_back(yoon->GetGUID());
    }
}

void TillersFarmMgr::RemoveSpawnedCreatures(Player* player)
{
    if (!player || !player->IsInWorld())
        return;

    uint32 guidLow = player->GetGUID().GetCounter();
    auto it = _playerSpawnedCreatures.find(guidLow);
    if (it == _playerSpawnedCreatures.end())
        return;

    Map* map = player->GetMap();
    for (ObjectGuid const& guid : it->second)
    {
        Creature* c = map->GetCreature(guid);
        if (c)
            c->DespawnOrUnsummon();
    }

    _playerSpawnedCreatures.erase(it);
}

void TillersFarmMgr::SpawnObstacles(Player* player, uint8 farmState, uint32 phaseMask)
{
    if (!player || !player->IsInWorld())
        return;

    Map* map = player->GetMap();
    if (!map)
        return;

    uint32 guidLow = player->GetGUID().GetCounter();

    // Determine which obstacle types are visible at this farmState
    // farmState bits: bit 1 = weeds, bit 2 = wagon, bit 3 = boulder
    bool spawnWeeds   = (farmState & 2) != 0;
    bool spawnWagon   = (farmState & 4) != 0;
    bool spawnBoulder = (farmState & 8) != 0;

    for (ObstacleSpawnData const& obs : _obstaclePositions)
    {
        bool shouldSpawn = false;
        if (obs.IsWeed())
            shouldSpawn = spawnWeeds;
        else if (obs.IsWagon())
            shouldSpawn = spawnWagon;
        else if (obs.IsBoulder())
            shouldSpawn = spawnBoulder;

        if (!shouldSpawn)
            continue;

        GameObject* go = new GameObject;
        if (!go->Create(map->GenerateLowGuid<HighGuid::GameObject>(), obs.entry, map,
            phaseMask, obs.posX, obs.posY, obs.posZ, obs.orientation,
            { }, 100, GO_STATE_READY))
        {
            delete go;
            continue;
        }

        go->SetOwnerGUID(player->GetGUID());
        go->SetPrivateObjectOwner(player->GetGUID());
        map->AddToMap(go);
        _playerObstacleGOs[guidLow].push_back(go->GetGUID());
    }

    TC_LOG_DEBUG("scripts", "TillersFarmMgr: Created %zu obstacle GOs for player %u (farmState=%u)",
                 _playerObstacleGOs[guidLow].size(), guidLow, farmState);
}

void TillersFarmMgr::RemoveObstacles(Player* player)
{
    if (!player || !player->IsInWorld())
        return;

    Map* map = player->GetMap();
    if (!map)
        return;

    uint32 guidLow = player->GetGUID().GetCounter();
    auto it = _playerObstacleGOs.find(guidLow);
    if (it == _playerObstacleGOs.end())
        return;

    for (ObjectGuid const& guid : it->second)
    {
        GameObject* go = map->GetGameObject(guid);
        if (go)
            go->ForcedDespawn();
    }

    TC_LOG_DEBUG("scripts", "TillersFarmMgr: Removed %zu obstacle GOs for player %u", it->second.size(), guidLow);
    _playerObstacleGOs.erase(it);
}

void TillersFarmMgr::SpawnPlayerFarm(Player* player)
{
    if (!player || !player->IsInWorld())
        return;

    uint32 guidLow = player->GetGUID().GetCounter();

    // Only spawn farm for players who have completed the entry quest
    // Pre-quest players interact with static ground Yoon at PUBLIC_FARM_MASK (128)
    if (!player->IsQuestRewarded(30252))
        return;

    // Check if farm is already spawned (avoid double-spawn)
    {
        uint8 bucket = static_cast<uint8>(guidLow % TILLERS_FARM_MGR_MUTEX_BUCKETS);
        std::lock_guard<std::mutex> lock(_mutexes[bucket]);
        auto it = _playerSpawnedCreatures.find(guidLow);
        if (it != _playerSpawnedCreatures.end())
        {
            // Upgrade path: if Learn and Grow IV (30256) was just completed, create soil GOs
            if (player->IsQuestRewarded(30256))
            {
                auto soilIt = _playerSoilGOs.find(guidLow);
                if (soilIt == _playerSoilGOs.end() || soilIt->second.empty())
                {
                    uint32 phaseMask = static_cast<uint32>((guidLow << 8) | 1);
                    TC_LOG_DEBUG("scripts", "TillersFarmMgr: Upgrading farm — creating soil GOs for player %u", guidLow);
                    CreateSoilGos(player, GetPlotsUnlockedForFarmState(_playerStates[guidLow].farmState), phaseMask);
                    return;
                }
            }
            TC_LOG_DEBUG("scripts", "TillersFarmMgr: Farm already spawned for player %u, skipping", guidLow);
            return;
        }
    }

    // Load data from DB on first call (singleton init)
    static bool sInitDataLoaded = false;
    if (!sInitDataLoaded)
    {
        LoadPlotPositions();
        LoadYoonPosition();
        LoadObstaclePositions();
        LoadBestFriendUnlockPositions();
        sInitDataLoaded = true;
    }

    // Load state from DB
    if (!LoadPlayerState(player))
    {
        TC_LOG_ERROR("scripts", "TillersFarmMgr: Failed to load farm state for player %u, using defaults", guidLow);
        PlayerFarmState defaultState;
        defaultState.farmState     = FARM_STATE_FULL;
        defaultState.plotsUnlocked = 4;

        _playerStates[guidLow] = defaultState;

        PlotMap& plots = _playerPlots[guidLow];
        for (uint8 i = 0; i < TILLERS_MAX_PLOTS; ++i)
        {
            FarmPlotData plot;
            plot.plotId      = i;
            plot.state       = PLOT_EMPTY;
            plot.seedEntry   = 0;
            plots[i] = plot;
        }
    }

    PlayerFarmState& state = _playerStates[guidLow];

    // Detect and save any newly unlocked best friends (rep gained outside zone)
    UpdateBestFriendUnlockState(player);

    uint8 plotsCount = GetPlotsUnlockedForFarmState(state.farmState);

    // Initialize any missing plots for the current phase
    PlotMap& plots = _playerPlots[guidLow];
    if (plots.empty())
    {
        for (uint8 i = 0; i < TILLERS_MAX_PLOTS; ++i)
        {
            FarmPlotData plot;
            plot.plotId      = i;
            plot.state       = PLOT_EMPTY;
            plot.seedEntry   = 0;
            plots[i] = plot;
        }
    }

    // Compute per-player phase mask: (guid << 8) | PHASEMASK_NORMAL
    // Unique per player via guid bits 8+, bit 0 keeps normal world visible.
    // Static farm objects at PUBLIC_FARM_MASK (128) are invisible via custom phase
    // bypassing zone phase definitions. Private farm objects at this phase are
    // gated per-player via _privateObjectOwner.
    uint32 phaseMask = static_cast<uint32>((guidLow << 8) | 1);

    // Set custom phase first (before building, so spawned objects use correct phase)
    player->GetPhaseMgr().SetCustomPhase(phaseMask);

    // Clean up any previously spawned objects (from prior rebuild or stale state)
    RemoveSoilGos(player);
    RemoveSpawnedCreatures(player);
    RemoveObstacles(player);
    RemoveBestFriendUnlocks(player);

    // Build farm from state: personal Yoon, obstacles matching current farmState, soil plots
    SpawnYoon(player, phaseMask);
    SpawnObstacles(player, state.farmState, phaseMask);

    // Soil GOs only after Learn and Grow IV (30256) — tutorial chain uses its own soil objects
    if (player->IsQuestRewarded(30256))
    {
        // Ensure unlocked plots have SOIL_PREPARED state if they're still empty
        for (uint8 i = 0; i < plotsCount && IsValidPlotId(i); ++i)
        {
            auto pit = plots.find(i);
            if (pit != plots.end())
            {
                uint8 initialUnlock = GetPlotsUnlockedForFarmState(FARM_STATE_FULL);
                if (pit->second.state == PLOT_EMPTY && i < initialUnlock)
                    pit->second.state = PLOT_SOIL_PREPARED;

                if ((pit->second.state == PLOT_SEEDED || pit->second.state == PLOT_GROWING) &&
                    pit->second.maturityTimestamp > 0 &&
                    static_cast<time_t>(time(nullptr)) >= pit->second.maturityTimestamp)
                {
                    pit->second.state = PLOT_READY_TO_HARVEST;
                }
            }
        }

        CreateSoilGos(player, plotsCount, phaseMask);
    }

    // Spawn unlocked best friend companions
    SpawnPlayerFarmCompanions(player, phaseMask);

    TC_LOG_INFO("scripts", "TillersFarmMgr: Spawned farm for player %u (farmState=%u, plots=%u, phaseMask=%u)",
                guidLow, state.farmState, plotsCount, phaseMask);
}

void TillersFarmMgr::DespawnPlayerFarm(Player* player)
{
    if (!player || !player->IsInWorld())
        return;

    uint32 guidLow = player->GetGUID().GetCounter();

    // Guard: nothing to despawn if no plots exist for this player
    auto pit = _playerPlots.find(guidLow);
    if (pit == _playerPlots.end())
        return;

    // Save all plot data before despawning
    SavePlayerFarm(player);

    // Clear stale in-memory state so IsPlayerFarmSpawned() returns false on re-entry
    _playerPlots.erase(guidLow);
    _playerStates.erase(guidLow);

    // Remove all dynamically spawned farm objects
    RemoveSoilGos(player);
    RemoveSpawnedCreatures(player);
    RemoveObstacles(player);
    RemoveBestFriendUnlocks(player);

    // Clear custom phase — player returns to zone phase definitions
    player->GetPhaseMgr().SetCustomPhase(0);

    TC_LOG_DEBUG("scripts", "TillersFarmMgr: Despawned farm for player %u", guidLow);
}

void TillersFarmMgr::SavePlayerFarm(Player* player)
{
    if (!player)
        return;

    uint32 guidLow = player->GetGUID().GetCounter();

    // Guard: nothing to save if no plots exist for this player
    auto pit = _playerPlots.find(guidLow);
    if (pit == _playerPlots.end() || pit->second.empty())
        return;

    PlotMap& plots = _playerPlots[guidLow];
    PlayerFarmState& state = _playerStates[guidLow];

    // Build transaction with all plot updates
    CharacterDatabaseTransaction trans = CharacterDatabase.BeginTransaction();

    // First, remove any old plot entries for this player (clean slate approach)
    {
        CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_PLAYER_FARM_PLOTS);
        stmt->setUInt32(0, guidLow);
        trans->Append(stmt);
    }

    for (auto const& [plotId, plotData] : plots)
    {
        if (!IsValidPlotId(plotId))
            continue;

        CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_INS_PLAYER_FARM_PLOT);
        stmt->setUInt32(0, guidLow);
        stmt->setUInt8(1, plotData.plotId);
        stmt->setUInt8(2, static_cast<uint8>(plotData.state));
        stmt->setUInt32(3, plotData.seedEntry);
        stmt->setBool(4, plotData.needsWatering);
        stmt->setBool(5, plotData.hasPests);
        stmt->setUInt64(6, static_cast<uint64>(plotData.maturityTimestamp));
        trans->Append(stmt);
    }

    // Save player farm state (phase info) — REPLACE handles both insert and update
    {
        CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_PLAYER_FARM_STATE);
        stmt->setUInt8(0, state.farmState);
        stmt->setUInt8(1, state.plotsUnlocked);
        stmt->setUInt16(2, state.bestFriendUnlocks);
        stmt->setUInt32(3, guidLow);
        trans->Append(stmt);
    }

    CharacterDatabase.CommitTransaction(trans);

    TC_LOG_DEBUG("scripts", "TillersFarmMgr: Saved farm state for player %u (%zu plots)", guidLow, plots.size());
}

bool TillersFarmMgr::PlantSeed(Player* player, uint8 plotId, uint32 seedEntry)
{
    if (!player || !IsValidPlotId(plotId))
        return false;

    uint32 guidLow = player->GetGUID().GetCounter();

    // Validate: player must have the seed in inventory
    if (!HasItemInInventory(player, seedEntry))
    {
        TC_LOG_DEBUG("scripts", "TillersFarmMgr: Player %u does not have seed %u in inventory", guidLow, seedEntry);
        return false;
    }

    // Validate plot exists and is in a plantable state
    PlotMap& plots = _playerPlots[guidLow];
    auto pit = plots.find(plotId);
    if (pit == plots.end())
        return false;

    FarmPlotData& plot = pit->second;

    // Only allow planting on EMPTY or SOIL_PREPARED plots
    if (plot.state != PLOT_EMPTY && plot.state != PLOT_SOIL_PREPARED)
    {
        TC_LOG_DEBUG("scripts", "TillersFarmMgr: Plot %u for player %u is in state %u, cannot plant",
                     plotId, guidLow, static_cast<uint8>(plot.state));
        return false;
    }

   // Validate plot is unlocked (within farm state bounds)
    uint8 plotsUnlocked = GetPlotsUnlockedForFarmState(_playerStates[guidLow].farmState);
    if (plotId >= plotsUnlocked)
    {
        TC_LOG_DEBUG("scripts", "TillersFarmMgr: Plot %u not yet unlocked for player %u (max=%u)",
                     plotId, guidLow, plotsUnlocked);
        return false;
    }

    // Begin transaction for atomic seed consumption + state update
    CharacterDatabaseTransaction trans = CharacterDatabase.BeginTransaction();

    // Determine crop state on planting (retail mechanics)
    // 1.1% chance of bursting (instant harvest), ~28% chance of water/pest problem
    uint32 roll = urand(1, 1000);
    bool bursting = roll <= 11;

    FarmPlotState newState;
    bool needsWater = false;
    bool hasPests = false;
    time_t maturity;

    if (bursting)
    {
        // Bursting crop: instantly ready to harvest
        newState = PLOT_READY_TO_HARVEST;
        maturity = 0;
        TC_LOG_INFO("scripts", "TillersFarmMgr: Player %u got a BURSTING crop on plot %u!", guidLow, plotId);
    }
    else
    {
        maturity = GetMaturityTime(seedEntry, plotId);

        // Roll for water/pest problems (~28% combined)
        uint32 problemRoll = urand(1, 1000);
        if (problemRoll <= 140)
        {
            newState = PLOT_NEEDS_WATER;
            needsWater = true;
            TC_LOG_DEBUG("scripts", "TillersFarmMgr: Player %u crop on plot %u needs water", guidLow, plotId);
        }
        else if (problemRoll <= 280)
        {
            newState = PLOT_NEEDS_PEST_CONTROL;
            hasPests = true;
            TC_LOG_DEBUG("scripts", "TillersFarmMgr: Player %u crop on plot %u has pests", guidLow, plotId);
        }
        else
        {
            newState = PLOT_SEEDED;
            TC_LOG_DEBUG("scripts", "TillersFarmMgr: Player %u crop on plot %u planted normally", guidLow, plotId);
        }
    }

    CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_PLAYER_FARM_PLOT_PLANT);
    stmt->setUInt8(0, static_cast<uint8>(newState));
    stmt->setUInt32(1, seedEntry);
    stmt->setBool(2, needsWater);
    stmt->setBool(3, hasPests);
    stmt->setUInt64(4, static_cast<uint64>(maturity));
    stmt->setUInt32(5, guidLow);
    stmt->setUInt8(6, plotId);
    trans->Append(stmt);

    CharacterDatabase.CommitTransaction(trans);

    // Consume seed from inventory after DB commit succeeds
    player->DestroyItemCount(seedEntry, 1, true);

    // Update local state immediately
    plot.state          = newState;
    plot.seedEntry      = seedEntry;
    plot.needsWatering  = needsWater;
    plot.hasPests       = hasPests;
    plot.maturityTimestamp = maturity;

    TC_LOG_INFO("scripts", "TillersFarmMgr: Player %u planted seed %u on plot %u (state=%u, matures at %lu)",
                guidLow, seedEntry, plotId, static_cast<uint8>(newState), static_cast<unsigned long>(maturity));

    return true;
}

bool TillersFarmMgr::HarvestCrop(Player* player, uint8 plotId)
{
    if (!player || !IsValidPlotId(plotId))
        return false;

    uint32 guidLow = player->GetGUID().GetCounter();

    PlotMap& plots = _playerPlots[guidLow];
    auto pit = plots.find(plotId);
    if (pit == plots.end())
        return false;

    FarmPlotData& plot = pit->second;

    // Only harvest READY_TO_HARVEST plots
    if (plot.state != PLOT_READY_TO_HARVEST)
    {
        TC_LOG_DEBUG("scripts", "TillersFarmMgr: Plot %u for player %u is in state %u, cannot harvest",
                     plotId, guidLow, static_cast<uint8>(plot.state));
        return false;
    }

    // Determine rewards based on crop type
    uint32 rewardItem = GetVegetableForSeed(plot.seedEntry);
    uint8   rewardCount = TILLERS_HARVEST_YIELD;

    if (rewardItem == 0 || rewardItem == plot.seedEntry)  // fallback: give seed back (legacy)
    {
        rewardItem = plot.seedEntry;
        rewardCount = 1;
    }

    // Plump crop bonus: ~5% chance of 8 vegetables instead of 5
    if (urand(1, 100) <= 5)
    {
        rewardCount += 3;
        TC_LOG_DEBUG("scripts", "TillersFarmMgr: Player %u got a PLUMP harvest on plot %u!", guidLow, plotId);
    }

    // Give rewards to player
    player->AddItem(rewardItem, rewardCount);

    // Seed return: 50% chance of 1-3 seeds
    if (urand(0, 1))
    {
        uint32 seedsReturned = urand(1, 3);
        player->AddItem(plot.seedEntry, seedsReturned);
        TC_LOG_DEBUG("scripts", "TillersFarmMgr: Player %u got %u seeds returned on plot %u", guidLow, seedsReturned, plotId);
    }

    // Award Tillers reputation (faction 1934)
    // Retail: flat 50 reputation at level 90, 0 below 90 (guild perk adds 10)
    if (player->GetLevel() >= 90)
    {
        uint32 tillersFaction = 1934;
        if (FactionEntry const* factionEntry = sFactionStore.LookupEntry(tillersFaction))
            player->GetReputationMgr().ModifyReputation(factionEntry, 50);
    }

    // Reset plot to SOIL_PREPARED state
    CharacterDatabaseTransaction trans = CharacterDatabase.BeginTransaction();

    CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_PLAYER_FARM_PLOT_RESET);
    stmt->setUInt8(0, static_cast<uint8>(PLOT_SOIL_PREPARED));
    stmt->setUInt32(1, 0);       // seed_entry cleared
    stmt->setBool(2, false);     // needs_watering = false
    stmt->setBool(3, false);     // has_pests = false
    stmt->setUInt64(4, 0);       // maturity cleared
    stmt->setUInt32(5, guidLow);
    stmt->setUInt8(6, plotId);
    trans->Append(stmt);

    CharacterDatabase.CommitTransaction(trans);

    // Update local state
    plot.state       = PLOT_SOIL_PREPARED;
    plot.seedEntry   = 0;
    plot.needsWatering  = false;
    plot.hasPests      = false;
    plot.maturityTimestamp = 0;

    TC_LOG_INFO("scripts", "TillersFarmMgr: Player %u harvested plot %u, received %ux item %u",
                guidLow, plotId, rewardCount, rewardItem);

    return true;
}

void TillersFarmMgr::WaterPlot(Player* player, uint8 plotId)
{
    if (!player || !IsValidPlotId(plotId))
        return;

    uint32 guidLow = player->GetGUID().GetCounter();

    // Validate: player must have watering can in inventory
    if (!HasItemInInventory(player, WATERING_CAN_ITEM))
    {
        TC_LOG_DEBUG("scripts", "TillersFarmMgr: Player %u does not have watering can (item %u)", guidLow, WATERING_CAN_ITEM);
        return;
    }

    PlotMap& plots = _playerPlots[guidLow];
    auto pit = plots.find(plotId);
    if (pit == plots.end())
        return;

    FarmPlotData& plot = pit->second;

    // Only water plots that need it
    if (plot.state != PLOT_NEEDS_WATER && plot.state != PLOT_SEEDED && plot.state != PLOT_GROWING)
        return;

    CharacterDatabaseTransaction trans = CharacterDatabase.BeginTransaction();

    CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_PLAYER_FARM_PLOT_WATER);
    stmt->setBool(0, false);       // needs_watering = false
    stmt->setUInt32(1, guidLow);
    stmt->setUInt8(2, plotId);
    trans->Append(stmt);

    // If plot was in NEEDS_WATER state, transition to growing
    if (plot.state == PLOT_NEEDS_WATER)
    {
        plot.state = PLOT_GROWING;
        CharacterDatabasePreparedStatement* stateStmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_PLAYER_FARM_PLOT_STATE);
        stateStmt->setUInt8(0, static_cast<uint8>(PLOT_GROWING));
        stateStmt->setUInt32(1, guidLow);
        stateStmt->setUInt8(2, plotId);
        trans->Append(stateStmt);
    }

    CharacterDatabase.CommitTransaction(trans);

    // Update local state
    plot.needsWatering = false;

    TC_LOG_DEBUG("scripts", "TillersFarmMgr: Player %u watered plot %u", guidLow, plotId);
}

void TillersFarmMgr::RemovePests(Player* player, uint8 plotId)
{
    if (!player || !IsValidPlotId(plotId))
        return;

    uint32 guidLow = player->GetGUID().GetCounter();

    // Validate: player must have bug sprayer in inventory
    if (!HasItemInInventory(player, BUG_SPRAYER_ITEM))
    {
        TC_LOG_DEBUG("scripts", "TillersFarmMgr: Player %u does not have bug sprayer (item %u)", guidLow, BUG_SPRAYER_ITEM);
        return;
    }

    PlotMap& plots = _playerPlots[guidLow];
    auto pit = plots.find(plotId);
    if (pit == plots.end())
        return;

    FarmPlotData& plot = pit->second;

    // Only remove pests from plots that have them
    if (!plot.hasPests)
        return;

    CharacterDatabaseTransaction trans = CharacterDatabase.BeginTransaction();

    CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_PLAYER_FARM_PLOT_PESTS);
    stmt->setBool(0, false);       // has_pests = false
    stmt->setUInt32(1, guidLow);
    stmt->setUInt8(2, plotId);
    trans->Append(stmt);

    // If plot was in NEEDS_PEST_CONTROL state, transition to growing
    if (plot.state == PLOT_NEEDS_PEST_CONTROL)
    {
        plot.state = PLOT_GROWING;
        CharacterDatabasePreparedStatement* stateStmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_PLAYER_FARM_PLOT_STATE);
        stateStmt->setUInt8(0, static_cast<uint8>(PLOT_GROWING));
        stateStmt->setUInt32(1, guidLow);
        stateStmt->setUInt8(2, plotId);
        trans->Append(stateStmt);
    }

    CharacterDatabase.CommitTransaction(trans);

    // Update local state
    plot.hasPests = false;

    TC_LOG_DEBUG("scripts", "TillersFarmMgr: Player %u removed pests from plot %u", guidLow, plotId);
}

void TillersFarmMgr::RepairPlot(Player* player, uint8 plotId)
{
    if (!player || !IsValidPlotId(plotId))
        return;

    uint32 guidLow = player->GetGUID().GetCounter();

    // Validate: player must have shovel in inventory
    if (!HasItemInInventory(player, SHOVEL_ITEM))
    {
        TC_LOG_DEBUG("scripts", "TillersFarmMgr: Player %u does not have shovel (item %u)", guidLow, SHOVEL_ITEM);
        return;
    }

    PlotMap& plots = _playerPlots[guidLow];
    auto pit = plots.find(plotId);
    if (pit == plots.end())
        return;

    FarmPlotData& plot = pit->second;

    // Only repair broken plots
    if (plot.state != PLOT_BROKEN)
        return;

    CharacterDatabaseTransaction trans = CharacterDatabase.BeginTransaction();

    CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_PLAYER_FARM_PLOT_REPAIR);
    stmt->setUInt8(0, static_cast<uint8>(PLOT_SOIL_PREPARED));
    stmt->setUInt32(1, 0);       // seed_entry cleared
    stmt->setUInt32(2, guidLow);
    stmt->setUInt8(3, plotId);
    trans->Append(stmt);

    CharacterDatabase.CommitTransaction(trans);

    // Update local state
    plot.state       = PLOT_SOIL_PREPARED;
    plot.seedEntry   = 0;

    TC_LOG_DEBUG("scripts", "TillersFarmMgr: Player %u repaired broken plot %u", guidLow, plotId);
}

bool TillersFarmMgr::IsPlayerFarmSpawned(Player* player) const
{
    if (!player)
        return false;

    uint32 guidLow = player->GetGUID().GetCounter();
    auto it = _playerPlots.find(guidLow);
    return it != _playerPlots.end() && !it->second.empty();
}

PlayerFarmState& TillersFarmMgr::GetPlayerState(uint32 guidLow)
{
    uint8 bucket = static_cast<uint8>(guidLow % TILLERS_FARM_MGR_MUTEX_BUCKETS);
    std::lock_guard<std::mutex> lock(_mutexes[bucket]);
    return _playerStates[guidLow];
}

void TillersFarmMgr::ForceGrowFarm(Player* player)
{
    if (!player)
        return;

    uint32 guidLow = player->GetGUID().GetCounter();
    uint8 bucket = static_cast<uint8>(guidLow % TILLERS_FARM_MGR_MUTEX_BUCKETS);
    std::lock_guard<std::mutex> lock(_mutexes[bucket]);

    auto stateIt = _playerStates.find(guidLow);
    if (stateIt == _playerStates.end())
        return;

    PlotMap& plots = _playerPlots[guidLow];
    if (plots.empty())
        return;

    bool anyGrown = false;
    for (auto& [plotId, plot] : plots)
    {
        if (plot.state == PLOT_SEEDED || plot.state == PLOT_GROWING)
        {
            plot.state = PLOT_READY_TO_HARVEST;
            plot.maturityTimestamp = 0;
            anyGrown = true;
        }
    }

    if (anyGrown)
    {
        CharacterDatabase.PExecute(
            "UPDATE player_farm_plots SET state = %u, maturity_timestamp = 0 "
            "WHERE guid = %u AND (state = %u OR state = %u)",
            static_cast<uint8>(PLOT_READY_TO_HARVEST), guidLow,
            static_cast<uint8>(PLOT_SEEDED), static_cast<uint8>(PLOT_GROWING));

        TC_LOG_INFO("scripts", "TillersFarmMgr: Force-matured crops for player %u", guidLow);
    }
}

void TillersFarmMgr::ResetPlayerFarm(uint32 guidLow)
{
    uint8 bucket = static_cast<uint8>(guidLow % TILLERS_FARM_MGR_MUTEX_BUCKETS);
    std::lock_guard<std::mutex> lock(_mutexes[bucket]);

    // Clear existing state, plots, and spawned objects
    _playerSoilGOs.erase(guidLow);
    _playerObstacleGOs.erase(guidLow);
    _playerSpawnedCreatures.erase(guidLow);
    _playerStates.erase(guidLow);
    _playerPlots.erase(guidLow);

   // Set default state
    PlayerFarmState defaultState;
    defaultState.farmState       = FARM_STATE_FULL;
    defaultState.plotsUnlocked   = 4;
    _playerStates[guidLow] = defaultState;

    // Initialize all plots to empty
    PlotMap& plots = _playerPlots[guidLow];
    for (uint8 i = 0; i < TILLERS_MAX_PLOTS; ++i)
    {
        FarmPlotData plot;
        plot.plotId         = i;
        plot.state          = PLOT_EMPTY;
        plot.seedEntry      = 0;
        plots[i] = plot;
    }
}

void TillersFarmMgr::LoadPlotPositions()
{
    _plotPositions.clear();

    QueryResult result = CharacterDatabase.PQuery(
        "SELECT `data` FROM creature_template WHERE `entry` = %u",
        PLOT_REFERENCE_CREATURE_ENTRY);

    if (!result)
    {
        TC_LOG_ERROR("scripts", "TillersFarmMgr: No creature_template entry for plot reference creature %u",
                     PLOT_REFERENCE_CREATURE_ENTRY);
        return;
    }

    // Plot positions are stored in the farm area on map 870
    // We load them from the world.creature table where map=870 and id=55626
    // Filter to farm cluster area (position_z between 164 and 166)
    result = WorldDatabase.PQuery(
        "SELECT `guid`, `position_x`, `position_y`, `position_z`, `orientation` FROM `world`.`creature` "
        "WHERE `map` = 870 AND `id` = %u AND `position_z` BETWEEN 164 AND 166 "
        "ORDER BY `position_x`, `position_y`",
        PLOT_REFERENCE_CREATURE_ENTRY);

    if (!result)
    {
        TC_LOG_WARN("scripts", "TillersFarmMgr: No plot position creatures found on map 870 (entry %u, z 164-166)",
                    PLOT_REFERENCE_CREATURE_ENTRY);
        return;
    }

    uint8 plotId = 0;
    do
    {
        Field* fields = result->Fetch();
        PlotPosition pos;
        pos.plotId = plotId;
        pos.posX = fields[1].GetFloat();
        pos.posY = fields[2].GetFloat();
        pos.posZ = fields[3].GetFloat();
        pos.orientation = fields[4].GetFloat();
        _plotPositions.push_back(pos);
        ++plotId;
    } while (result->NextRow());

    TC_LOG_INFO("scripts", "TillersFarmMgr: Loaded %zu plot positions from map 870", _plotPositions.size());
}

bool TillersFarmMgr::GetPlotPosition(uint8 plotId, PlotPosition& out) const
{
    if (plotId >= _plotPositions.size())
        return false;
    out = _plotPositions[plotId];
    return true;
}

void TillersFarmMgr::LoadYoonPosition()
{
    _yoonSpawnData.loaded = false;

    // Load ground-level Farmer Yoon position from creature table (map 870, entry 58646)
    // Filter by z to get the ground-level spawn (~165.5) rather than farmhouse spawn
    QueryResult result = WorldDatabase.PQuery(
        "SELECT `position_x`, `position_y`, `position_z`, `orientation` FROM `creature` "
        "WHERE `map` = 870 AND `id` = %u AND `position_z` BETWEEN 160 AND 170 "
        "ORDER BY `position_z` ASC LIMIT 1",
        FARMER_YOON_ENTRY);

    if (!result)
    {
        TC_LOG_ERROR("scripts", "TillersFarmMgr: No Farmer Yoon spawn found in creature table (map 870, entry %u)",
                     FARMER_YOON_ENTRY);
        return;
    }

    Field* fields = result->Fetch();
    _yoonSpawnData.posX = fields[0].GetFloat();
    _yoonSpawnData.posY = fields[1].GetFloat();
    _yoonSpawnData.posZ = fields[2].GetFloat();
    _yoonSpawnData.orientation = fields[3].GetFloat();
    _yoonSpawnData.loaded = true;

    TC_LOG_INFO("scripts", "TillersFarmMgr: Loaded Yoon spawn position (%.3f, %.3f, %.3f, %.3f)",
                _yoonSpawnData.posX, _yoonSpawnData.posY, _yoonSpawnData.posZ, _yoonSpawnData.orientation);
}

void TillersFarmMgr::LoadObstaclePositions()
{
    _obstaclePositions.clear();

    QueryResult result = WorldDatabase.PQuery(
        "SELECT `id`, `position_x`, `position_y`, `position_z`, `orientation` FROM `gameobject` "
        "WHERE `map` = 870 AND `id` IN (210443, 210444, 210445, 210446, 210447, 210448, 210462, 210451, 209572) "
        "AND `position_x` BETWEEN -250 AND -100 AND `position_y` BETWEEN 580 AND 700 "
        "ORDER BY `id`, `guid`");

    if (!result)
    {
        TC_LOG_ERROR("scripts", "TillersFarmMgr: No obstacle gameobjects found in farm area (map 870)");
        return;
    }

    do
    {
        Field* fields = result->Fetch();
        ObstacleSpawnData obs;
        obs.entry = fields[0].GetUInt32();
        obs.posX = fields[1].GetFloat();
        obs.posY = fields[2].GetFloat();
        obs.posZ = fields[3].GetFloat();
        obs.orientation = fields[4].GetFloat();
        _obstaclePositions.push_back(obs);
    } while (result->NextRow());

    TC_LOG_INFO("scripts", "TillersFarmMgr: Loaded %zu obstacle positions from gameobject table", _obstaclePositions.size());
}



void TillersFarmMgr::LoadBestFriendUnlockPositions()
{
    _bestFriendUnlockPositions.clear();

    static uint32 const bestFriendUnlockEntries[] = {
        SHAGGY_YAK_ENTRY, MISS_FIFI_MUSHAN_ENTRY, HILLPAW_CHICKENS_ENTRY,
        FARM_SHEEP_ENTRY, LUNA_CAT_ENTRY, PIGGY_PIG_ENTRY,
        ORANGE_TREE_ENTRY, FURNITURE_ENTRY, MAILBOX_ENTRY,
        LOST_DOG_ENTRY
    };

    for (uint32 entry : bestFriendUnlockEntries)
    {
        QueryResult result = WorldDatabase.PQuery(
            "SELECT `position_x`, `position_y`, `position_z`, `orientation` FROM `creature` "
            "WHERE `map` = 870 AND `id` = %u AND `position_z` BETWEEN 160 AND 170 "
            "ORDER BY `position_z` ASC LIMIT 1",
            entry);

        if (!result)
        {
            TC_LOG_WARN("scripts", "TillersFarmMgr: No best friend unlock position found for entry %u, trying gameobject table", entry);
            result = WorldDatabase.PQuery(
                "SELECT `position_x`, `position_y`, `position_z`, `orientation` FROM `gameobject` "
                "WHERE `map` = 870 AND `id` = %u "
                "ORDER BY `guid` LIMIT 1",
                entry);
        }

        if (result)
        {
            Field* fields = result->Fetch();
            BestFriendUnlockPosition pos;
            pos.entry = entry;
            pos.posX = fields[0].GetFloat();
            pos.posY = fields[1].GetFloat();
            pos.posZ = fields[2].GetFloat();
            pos.orientation = fields[3].GetFloat();
            _bestFriendUnlockPositions.push_back(pos);
        }
        else
        {
            TC_LOG_WARN("scripts", "TillersFarmMgr: No position found for best friend unlock entry %u (creature or gameobject)", entry);
            BestFriendUnlockPosition pos;
            pos.entry = entry;
            pos.posX = -175.0f;
            pos.posY = 640.0f;
            pos.posZ = 165.5f;
            pos.orientation = 0.0f;
            _bestFriendUnlockPositions.push_back(pos);
        }
    }

    TC_LOG_INFO("scripts", "TillersFarmMgr: Loaded %zu best friend unlock positions", _bestFriendUnlockPositions.size());
}

bool TillersFarmMgr::GetBestFriendUnlockPosition(uint32 entry, BestFriendUnlockPosition& out) const
{
    for (BestFriendUnlockPosition const& pos : _bestFriendUnlockPositions)
    {
        if (pos.entry == entry)
        {
            out = pos;
            return true;
        }
    }
    return false;
}

Creature* TillersFarmMgr::SpawnBestFriendUnlock(Player* player, uint32 entry, uint32 phaseMask, float posX, float posY, float posZ, float orientation)
{
    if (!player || !player->IsInWorld())
        return nullptr;

    Creature* unlock = player->SummonCreature(entry, Position(posX, posY, posZ, orientation), TEMPSUMMON_MANUAL_DESPAWN, 0, 0, player->GetGUID());
    if (unlock)
    {
        unlock->SetPhaseMask(phaseMask, true);
        unlock->SetPrivateObjectOwner(player->GetGUID());
        _playerBestFriendUnlocks[player->GetGUID().GetCounter()].push_back(unlock->GetGUID());
        TC_LOG_DEBUG("scripts", "TillersFarmMgr: Spawned best friend creature %u for player %u", entry, player->GetGUID().GetCounter());
    }
    return unlock;
}

GameObject* TillersFarmMgr::SpawnBestFriendUnlockGO(Player* player, uint32 entry, uint32 phaseMask, float posX, float posY, float posZ, float orientation)
{
    if (!player || !player->IsInWorld())
        return nullptr;

    GameObject* unlock = player->SummonGameObject(entry, posX, posY, posZ, orientation, { }, 0, GO_SUMMON_TIMED_OR_CORPSE_DESPAWN);
    if (unlock)
    {
        unlock->SetPhaseMask(phaseMask, true);
        _playerBestFriendUnlocks[player->GetGUID().GetCounter()].push_back(unlock->GetGUID());
        TC_LOG_DEBUG("scripts", "TillersFarmMgr: Spawned best friend GO %u for player %u", entry, player->GetGUID().GetCounter());
    }
    return unlock;
}

void TillersFarmMgr::SpawnPlayerFarmCompanions(Player* player, uint32 phaseMask)
{
    if (!player || !player->IsInWorld())
        return;

    uint32 guidLow = player->GetGUID().GetCounter();
    uint8 bucket = static_cast<uint8>(guidLow % TILLERS_FARM_MGR_MUTEX_BUCKETS);
    std::lock_guard<std::mutex> lock(_mutexes[bucket]);

    PlayerFarmState const& state = _playerStates[guidLow];

    for (uint8 i = 0; i < 10; ++i)
    {
        BestFriendData const& companion = BestFriendCompanions[i];
        if (!(state.bestFriendUnlocks & companion.bit))
            continue;

        BestFriendUnlockPosition pos;
        if (!GetBestFriendUnlockPosition(companion.entry, pos))
        {
            TC_LOG_WARN("scripts", "TillersFarmMgr: No position for companion entry %u", companion.entry);
            continue;
        }

        if (IsGameObjectEntry(companion.entry))
            SpawnBestFriendUnlockGO(player, companion.entry, phaseMask, pos.posX, pos.posY, pos.posZ, pos.orientation);
        else
            SpawnBestFriendUnlock(player, companion.entry, phaseMask, pos.posX, pos.posY, pos.posZ, pos.orientation);
    }

    TC_LOG_DEBUG("scripts", "TillersFarmMgr: Spawned companions for player %u", guidLow);
}

void TillersFarmMgr::RemoveBestFriendUnlocks(Player* player)
{
    if (!player || !player->IsInWorld())
        return;

    uint32 guidLow = player->GetGUID().GetCounter();
    auto it = _playerBestFriendUnlocks.find(guidLow);
    if (it == _playerBestFriendUnlocks.end())
        return;

    Map* map = player->GetMap();
    for (ObjectGuid const& guid : it->second)
    {
        if (guid.IsGameObject())
        {
            GameObject* go = map->GetGameObject(guid);
            if (go)
                go->ForcedDespawn(0);
        }
        else if (guid.IsCreature())
        {
            Creature* c = map->GetCreature(guid);
            if (c)
                c->DespawnOrUnsummon();
        }
    }

    TC_LOG_DEBUG("scripts", "TillersFarmMgr: Removed %zu best friend unlocks for player %u", it->second.size(), guidLow);
    _playerBestFriendUnlocks.erase(it);
}

bool TillersFarmMgr::IsBestFriend(Player* player, uint32 factionId) const
{
    if (!player || !factionId)
        return false;

    // Best Friend status requires Exalted reputation (42,000) with the NPC's friendship faction
    return player->GetReputationRank(factionId) >= REP_EXALTED;
}

void TillersFarmMgr::UpdateBestFriendUnlockState(Player* player)
{
    if (!player || !player->IsInWorld())
        return;

    uint32 guidLow = player->GetGUID().GetCounter();
    uint8 bucket = static_cast<uint8>(guidLow % TILLERS_FARM_MGR_MUTEX_BUCKETS);
    std::lock_guard<std::mutex> lock(_mutexes[bucket]);

    PlayerFarmState& state = _playerStates[guidLow];
    bool changed = false;

    // Check each per-NPC best friend via reputation faction
    for (uint8 i = 0; i < 9; ++i)
    {
        BestFriendData const& companion = BestFriendCompanions[i];
        if ((state.bestFriendUnlocks & companion.bit) == 0 && IsBestFriend(player, companion.factionId))
        {
            state.bestFriendUnlocks |= companion.bit;
            changed = true;
            TC_LOG_INFO("scripts", "TillersFarmMgr: Player %u unlocked companion (entry %u)", guidLow, companion.entry);
        }
    }

    // Lost Dog — quest-based, not reputation
    BestFriendData const& lostDog = BestFriendCompanions[9]; // index 9 = BEST_FRIEND_LOST_DOG
    if ((state.bestFriendUnlocks & lostDog.bit) == 0 && player->IsQuestRewarded(30526))
    {
        state.bestFriendUnlocks |= lostDog.bit;
        changed = true;
        TC_LOG_INFO("scripts", "TillersFarmMgr: Player %u unlocked Lost Dog (quest 30526)", guidLow);
    }

    if (changed)
    {
        CharacterDatabase.PExecute(
            "UPDATE player_farm_state SET best_friend_unlocks = %u WHERE guid = %u",
            state.bestFriendUnlocks, guidLow);
    }
}
