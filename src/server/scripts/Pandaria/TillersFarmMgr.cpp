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
#include "GameObject.h"
#include "GridNotifiers.h"
#include "GridNotifiersImpl.h"
#include "Log.h"
#include "ObjectAccessor.h"
#include "Player.h"
#include "ScriptMgr.h"
#include "World.h"

// Ground-level Farmer Yoon (58646) dynamically spawned in farm phase after quest 30252.
// Tools Yoon (58721) and farmhouse Yoon (58646) are duplicates — kept as static world spawns at phase 1.

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
        "SELECT farm_phase, plots_unlocked, last_growth_tick FROM player_farm_state WHERE guid = %u",
        guidLow);

    if (result)
    {
        Field* fields = result->Fetch();
        uint8 phase   = fields[0].GetUInt8();
        uint8 plots   = fields[1].GetUInt8();
        time_t tick   = static_cast<time_t>(fields[2].GetUInt64());

        // Validate loaded state
        if (phase > PHASE_LEGENDARY_CROPS)
        {
            TC_LOG_ERROR("scripts", "TillersFarmMgr: Player %u has invalid farm_phase %u, resetting to defaults", guidLow, phase);
            return false;
        }

        // plots_unlocked must be 4, 8, 12, or 16 (matching DB schema)
        if ((plots != 4 && plots != 8 && plots != 12 && plots != 16) || plots > TILLERS_MAX_PLOTS)
        {
            TC_LOG_ERROR("scripts", "TillersFarmMgr: Player %u has invalid plots_unlocked %u, resetting to defaults", guidLow, plots);
            return false;
        }

        // last_growth_tick should not be in the future by more than a day
        time_t now = time(nullptr);
        if (tick > now + 86400)
        {
            TC_LOG_ERROR("scripts", "TillersFarmMgr: Player %u has last_growth_tick in far future (%lu), resetting to defaults", guidLow, static_cast<unsigned long>(tick));
            return false;
        }

        PlayerFarmState state;
        state.farmPhase     = phase;
        state.plotsUnlocked = plots;
        state.lastGrowthTick = tick;

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
    defaultState.farmPhase     = PHASE_PLANTING;
    defaultState.plotsUnlocked = 4;   // Default matches DB schema (phase 1-4 unlocks 4 plots)
    defaultState.lastGrowthTick = time(nullptr);

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

void TillersFarmMgr::SpawnPlayerFarm(Player* player)
{
    if (!player || !player->IsInWorld())
        return;

    // Farm only spawns for players who completed "A Helping Hand" (30252)
    // Pre-quest players stay in default phase and see static world spawns (58721, rocks, etc.)
    if (!player->IsQuestRewarded(30252))
        return;

    uint32 guidLow = player->GetGUID().GetCounter();

    // Check if farm is already spawned (avoid double-spawn)
    {
        uint8 bucket = static_cast<uint8>(guidLow % TILLERS_FARM_MGR_MUTEX_BUCKETS);
        std::lock_guard<std::mutex> lock(_mutexes[bucket]);
        auto it = _playerPlots.find(guidLow);
        if (it != _playerPlots.end() && !it->second.empty())
        {
            // Upgrade path: if Gina's Vote (31945) was just completed, create soil GOs
            if (player->IsQuestRewarded(31945))
            {
                auto soilIt = _playerSoilGOs.find(guidLow);
                if (soilIt == _playerSoilGOs.end() || soilIt->second.empty())
                {
                    uint32 phaseMask = static_cast<uint32>((guidLow << 8) | (_playerStates[guidLow].farmPhase + 1));
                    TC_LOG_DEBUG("scripts", "TillersFarmMgr: Upgrading farm — creating soil GOs for player %u", guidLow);
                    CreateSoilGos(player, GetPlotsUnlockedForPhase(_playerStates[guidLow].farmPhase), phaseMask);
                    return;
                }
            }
            TC_LOG_DEBUG("scripts", "TillersFarmMgr: Farm already spawned for player %u, skipping", guidLow);
            return;
        }
    }

    // Load plot positions on first call (singleton init)
    static bool sPlotPositionsLoaded = false;
    if (!sPlotPositionsLoaded)
    {
        LoadPlotPositions();
        sPlotPositionsLoaded = true;
    }

    // Load state from DB
    if (!LoadPlayerState(player))
    {
        TC_LOG_ERROR("scripts", "TillersFarmMgr: Failed to load farm state for player %u, using defaults", guidLow);
        // Reset to defaults
        PlayerFarmState defaultState;
        defaultState.farmPhase     = PHASE_PLANTING;
        defaultState.plotsUnlocked = 4;   // Default matches DB schema

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
    uint8 plotsCount = GetPlotsUnlockedForPhase(state.farmPhase);

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

    // Compute per-player phase mask: (guid << 8) | phase
    // No PHASEMASK_NORMAL — this isolates the farm instance to this player only
    uint32 phaseMask = static_cast<uint32>((guidLow << 8) | (state.farmPhase + 1));

    // Set custom phase for this player's farm instance
    player->GetPhaseMgr().SetCustomPhase(phaseMask);

    // Spawn ground-level Farmer Yoon
    SpawnFarmerNPCs(player, phaseMask);

    // Spawn Tillers Shrine
    SpawnFarmGameObjects(player, phaseMask);

    // Soil GOs only after Gina's Vote (31945) — tutorial chain uses its own soil objects
    if (player->IsQuestRewarded(31945))
    {
        // Ensure unlocked plots have SOIL_PREPARED state if they're still empty
        for (uint8 i = 0; i < plotsCount && IsValidPlotId(i); ++i)
        {
            auto pit = plots.find(i);
            if (pit != plots.end())
            {
                uint8 initialUnlock = GetPlotsUnlockedForPhase(PHASE_PLANTING);
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

    TC_LOG_INFO("scripts", "TillersFarmMgr: Spawned farm for player %u (phase=%u, plots=%u, phaseMask=%u)",
                guidLow, state.farmPhase, plotsCount, phaseMask);
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

    // Remove soil GOs from the player's phase
    RemoveSoilGos(player);

    // Despawn all dynamically spawned creatures
    DespawnAllCreatures(player);

    // Despawn farm GOs (shrine, bowls)
    RemoveFarmGos(player);

    // Clear phase
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

    // Save player farm state (phase info)
    {
        CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_PLAYER_FARM_STATE);
        stmt->setUInt8(0, state.farmPhase);
        stmt->setUInt8(1, state.plotsUnlocked);
        stmt->setUInt64(2, static_cast<uint64>(state.lastGrowthTick));
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

    // Validate plot is unlocked (within phase bounds)
    uint8 plotsUnlocked = GetPlotsUnlockedForPhase(_playerStates[guidLow].farmPhase);
    if (plotId >= plotsUnlocked)
    {
        TC_LOG_DEBUG("scripts", "TillersFarmMgr: Plot %u not yet unlocked for player %u (max=%u)",
                     plotId, guidLow, plotsUnlocked);
        return false;
    }

    // Begin transaction for atomic seed consumption + state update
    CharacterDatabaseTransaction trans = CharacterDatabase.BeginTransaction();

    // Update plot state to SEEDED with maturity timestamp
    time_t maturity = GetMaturityTime(seedEntry, plotId);

    CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_PLAYER_FARM_PLOT_PLANT);
    stmt->setUInt8(0, static_cast<uint8>(PLOT_SEEDED));
    stmt->setUInt32(1, seedEntry);
    stmt->setBool(2, false);   // needs_watering = false initially
    stmt->setBool(3, false);   // has_pests = false initially
    stmt->setUInt64(4, static_cast<uint64>(maturity));
    stmt->setUInt32(5, guidLow);
    stmt->setUInt8(6, plotId);
    trans->Append(stmt);

    CharacterDatabase.CommitTransaction(trans);

    // Consume seed from inventory after DB commit succeeds
    player->DestroyItemCount(seedEntry, 1, true);

    // Update local state immediately (transaction committed above)
    plot.state       = PLOT_SEEDED;
    plot.seedEntry   = seedEntry;
    plot.needsWatering  = false;
    plot.hasPests      = false;
    plot.maturityTimestamp = maturity;

    TC_LOG_INFO("scripts", "TillersFarmMgr: Player %u planted seed %u on plot %u (matures at %lu)",
                guidLow, seedEntry, plotId, static_cast<unsigned long>(maturity));

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

    // Determine rewards based on crop type and friendship level (simplified)
    uint32 rewardItem = plot.seedEntry;  // Return same item type as planted
    uint8   rewardCount = 1;  // Standard harvest yield

    if (rewardItem == 0)
        return false;

    // Give rewards to player
    player->AddItem(rewardItem, rewardCount);

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

    // Clear existing state and plots
    _playerSoilGOs.erase(guidLow);
    _playerStates.erase(guidLow);
    _playerPlots.erase(guidLow);
    _playerCreatures.erase(guidLow);

    // Set default state
    PlayerFarmState defaultState;
    defaultState.farmPhase       = PHASE_PLANTING;
    defaultState.plotsUnlocked   = 4;
    defaultState.lastGrowthTick  = time(nullptr);
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

ObjectGuid TillersFarmMgr::SpawnCreature(Player* player, uint32 entry, uint8 plotId, bool visible, uint32 phaseMask)
{
    if (!player || !player->IsInWorld())
        return ObjectGuid();

    PlotPosition pos;
    if (!GetPlotPosition(plotId, pos))
        return ObjectGuid();

    return SpawnCreatureAt(player, entry, pos.posX, pos.posY, pos.posZ, pos.orientation, visible, phaseMask);
}

void TillersFarmMgr::DespawnCreature(ObjectGuid guid, Map* map)
{
    if (guid.IsEmpty())
        return;

    if (!map)
        return;

    Creature* creature = map->GetCreature(guid);
    if (!creature)
        return;

    creature->RemoveFromWorld();
    creature->AddObjectToRemoveList();

    TC_LOG_DEBUG("scripts", "TillersFarmMgr: Despawned creature %u (guid %s)",
                 creature->GetEntry(), guid.ToString().c_str());
}

void TillersFarmMgr::DespawnAllCreatures(Player* player)
{
    if (!player)
        return;

    uint32 guidLow = player->GetGUID().GetCounter();
    auto it = _playerCreatures.find(guidLow);
    if (it == _playerCreatures.end())
        return;

    Map* map = player->GetMap();
    if (!map)
        return;

    for (ObjectGuid const& guid : it->second)
    {
        Creature* creature = map->GetCreature(guid);
        if (creature)
        {
            creature->RemoveFromWorld();
            creature->AddObjectToRemoveList();
        }
    }

    TC_LOG_DEBUG("scripts", "TillersFarmMgr: Despawned %zu creatures for player %u",
                 it->second.size(), guidLow);
    _playerCreatures.erase(it);
}

ObjectGuid TillersFarmMgr::SpawnCreatureAt(Player* player, uint32 entry, float posX, float posY, float posZ, float orientation, bool visible, uint32 phaseMask)
{
    if (!player || !player->IsInWorld())
        return ObjectGuid();

    Map* map = player->GetMap();
    if (!map)
        return ObjectGuid();

    uint32 guidLow = player->GetGUID().GetCounter();

    Creature* creature = new Creature();
    creature->m_isTempWorldObject = true;

    if (!creature->Create(map->GenerateLowGuid<HighGuid::Unit>(), map, phaseMask, entry, 0, 0, posX, posY, posZ, orientation))
    {
        delete creature;
        return ObjectGuid();
    }

    // Set creature to not wander and be invisible to non-farm players
    creature->SetWanderDistance(0.0f);
    creature->SetDefaultMovementType(IDLE_MOTION_TYPE);

    if (!visible)
    {
        creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC);
        creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC);
    }

    creature->SetUnitFlag2(UNIT_FLAG2_UNK1);

    map->AddToMap(creature);

    _playerCreatures[guidLow].push_back(creature->GetGUID());

    TC_LOG_DEBUG("scripts", "TillersFarmMgr: Spawned creature %u at (%.2f, %.2f, %.2f) for player %u",
                 entry, posX, posY, posZ, guidLow);

    return creature->GetGUID();
}

void TillersFarmMgr::SpawnFarmerNPCs(Player* player, uint32 farmPhaseMask)
{
    if (!player || !player->IsInWorld())
        return;

    // Single ground-level Farmer Yoon (58646) — the quest hub version visible after "A Helping Hand"
    SpawnCreatureAt(player, 58646, -180.844f, 628.358f, 165.493f, 1.85448f, true, farmPhaseMask);

    TC_LOG_DEBUG("scripts", "TillersFarmMgr: Spawned ground Farmer Yoon for player %u",
                 player->GetGUID().GetCounter());
}

void TillersFarmMgr::SpawnFarmGameObjects(Player* player, uint32 phaseMask)
{
    if (!player || !player->IsInWorld())
        return;

    Map* map = player->GetMap();
    if (!map)
        return;

    uint32 guidLow = player->GetGUID().GetCounter();

    // Tillers Shrine (215705) — spawned dynamically in farm phase
    {
        GameObject* shrine = new GameObject();
        if (shrine->Create(map->GenerateLowGuid<HighGuid::GameObject>(), 215705, map,
            phaseMask, -187.592f, 637.087f, 165.409f, 6.23434f, { }, 300, GO_STATE_READY))
        {
            map->AddToMap(shrine);
            _playerFarmGOs[guidLow].push_back(shrine->GetGUID());
        }
        else
            delete shrine;
    }

    // Offering Bowls (215706) — 5 bowls encircling the shrine
    {
        static const struct { float x, y, z, o; } sBowlPositions[] = {
            { -185.858f, 637.865f, 165.409f, 0.0f },
            { -186.587f, 637.476f, 165.566f, 0.0f },
            { -185.894f, 636.998f, 165.409f, 0.0f },
            { -186.615f, 636.660f, 165.561f, 0.0f },
            { -185.971f, 636.161f, 165.409f, 0.0f },
        };

        for (auto const& pos : sBowlPositions)
        {
            GameObject* bowl = new GameObject();
            if (bowl->Create(map->GenerateLowGuid<HighGuid::GameObject>(), 215706, map,
                phaseMask, pos.x, pos.y, pos.z, pos.o, { }, 300, GO_STATE_READY))
            {
                map->AddToMap(bowl);
                _playerFarmGOs[guidLow].push_back(bowl->GetGUID());
            }
            else
                delete bowl;
        }
    }

    TC_LOG_DEBUG("scripts", "TillersFarmMgr: Created %zu farm GOs for player %u",
                 _playerFarmGOs[guidLow].size(), guidLow);
}

void TillersFarmMgr::RemoveFarmGos(Player* player)
{
    if (!player || !player->IsInWorld())
        return;

    Map* map = player->GetMap();
    if (!map)
        return;

    uint32 guidLow = player->GetGUID().GetCounter();
    auto it = _playerFarmGOs.find(guidLow);
    if (it == _playerFarmGOs.end())
        return;

    for (ObjectGuid const& guid : it->second)
    {
        GameObject* go = map->GetGameObject(guid);
        if (go)
            go->ForcedDespawn();
    }

    TC_LOG_DEBUG("scripts", "TillersFarmMgr: Removed %zu farm GOs for player %u",
                 it->second.size(), guidLow);
    _playerFarmGOs.erase(it);
}
