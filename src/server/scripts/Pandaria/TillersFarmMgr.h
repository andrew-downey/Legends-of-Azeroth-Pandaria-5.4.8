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

#ifndef TILLERS_FARM_MGR_H
#define TILLERS_FARM_MGR_H

#include "Common.h"
#include "DatabaseEnv.h"
#include "ObjectGuid.h"
#include "Player.h"
#include "World.h"
#include <mutex>
#include <unordered_map>
#include <map>
#include <vector>

class Player;
class GameObject;
class Creature;
class Map;

// Maximum number of plots per player farm instance (matches DB schema)
static uint8 const TILLERS_MAX_PLOTS = 16;

enum FarmPlotState : uint8
{
    PLOT_EMPTY              = 0,
    PLOT_SOIL_PREPARED      = 1,
    PLOT_SEEDED             = 2,
    PLOT_GROWING            = 3,
    PLOT_NEEDS_WATER        = 4,
    PLOT_NEEDS_PEST_CONTROL = 5,
    PLOT_READY_TO_HARVEST   = 6,
    PLOT_BROKEN             = 7
};

enum FarmPhase : uint8
{
    PHASE_UNAVAILABLE       = 0,
    PHASE_PLANTING          = 1,
    PHASE_GROWING           = 2,
    PHASE_HARVESTING        = 3,
    PHASE_FERTILIZING       = 4,
    PHASE_IRRIGATION        = 5,
    PHASE_COMPOSTING        = 6,
    PHASE_ENRICHMENT        = 7,
    PHASE_CROP_ROTATION     = 8,
    PHASE_SEED_BREEDING     = 9,
    PHASE_HYBRID_CROPS      = 10,
    PHASE_GREENHOUSE        = 11,
    PHASE_TERRACED_FARMING  = 12,
    PHASE_MASS_PRODUCTION   = 13,
    PHASE_COMMERCIAL_AGRIC  = 14,
    PHASE_EXPORT_QUALITY    = 15,
    PHASE_LEGENDARY_CROPS   = 16
};

// Number of plots unlocked per phase group: phases 1-4 unlock 4, 5-8 unlock 8, etc.
inline uint8 GetPlotsUnlockedForPhase(uint8 phase)
{
    if (phase <= 0 || phase > PHASE_LEGENDARY_CROPS)
        return 0;
    // Each group of 4 phases unlocks 4 more plots: 4, 8, 12, or 16
    uint8 plots = ((phase - 1) / 4 + 1) * 4;
    if (plots > TILLERS_MAX_PLOTS)
        plots = TILLERS_MAX_PLOTS;
    return plots;
}

struct FarmPlotData
{
    uint8   plotId           = 0;       // 0-15 per player farm instance
    FarmPlotState state      = PLOT_EMPTY;
    uint32  seedEntry        = 0;       // item entry of planted crop (0=none)
    bool    needsWatering    = false;
    bool    hasPests         = false;
    time_t  maturityTimestamp = 0;      // absolute timestamp when ready to harvest
};

struct PlayerFarmState
{
    uint8   farmPhase       = PHASE_PLANTING;
    uint8   plotsUnlocked   = 4;        // number of unlocked patches (matches DB default)
    time_t  lastGrowthTick  = 0;        // absolute timestamp for drift-free scheduling
};

typedef std::map<uint8, FarmPlotData> PlotMap;       // plot_id -> data
typedef std::unordered_map<uint32, PlayerFarmState> StateStore;  // player guid low -> state

// Fixed plot position on map 870 (loaded from creature table)
struct PlotPosition
{
    uint8  plotId     = 0;
    float  posX       = 0.0f;
    float  posY       = 0.0f;
    float  posZ       = 0.0f;
    float  orientation = 0.0f;
};

// Creature entries used for farm visual elements
static inline uint32 const PLOT_REFERENCE_CREATURE_ENTRY = 55626;   // Bunny — plot position reference

#define TILLERS_FARM_MGR_MUTEX_BUCKETS 64

class TillersFarmMgr
{
public:
    static TillersFarmMgr& getInstance()
    {
        static TillersFarmMgr instance;
        return instance;
    }

    /**
     * Spawn a player's personal farm when entering Valley of Four Winds (zone 1023).
     * Loads state from DB, creates soil GOs in the player's custom phase.
     */
    void SpawnPlayerFarm(Player* player);

    /**
     * Despawn and save all plot states for a player leaving the zone.
     * Saves atomically per-player using CharacterDatabaseTransaction.
     */
    void DespawnPlayerFarm(Player* player);

    /**
     * Plant a seed on a plot. Validates inventory, checks plot state, consumes seed.
     * Returns true on success, false on failure (player keeps seed).
     */
    bool PlantSeed(Player* player, uint8 plotId, uint32 seedEntry);

    /**
     * Harvest a mature crop. Validates plot is READY_TO_HARVEST, gives rewards.
     * Returns true on success, false on failure.
     */
    bool HarvestCrop(Player* player, uint8 plotId);

    /**
     * Water a drying plot. Requires watering can (item 79104) in inventory.
     * Sets needsWatering=false and applies growth boost.
     */
    void WaterPlot(Player* player, uint8 plotId);

    /**
     * Remove pests from a plot. Requires bug sprayer (item 80513) in inventory.
     * Sets hasPests=false and restores growth.
     */
    void RemovePests(Player* player, uint8 plotId);

    /**
     * Repair a broken plot. Requires shovel (item 89880) in inventory.
     * Resets plot to SOIL_PREPARED state.
     */
    void RepairPlot(Player* player, uint8 plotId);

    /**
     * Get or create a player's farm state and plot map.
     * Thread-safe via bucket mutex.
     */
    PlotMap& GetPlayerPlots(Player* player)
    {
        uint32 guidLow = player->GetGUID().GetCounter();
        uint8 bucket = static_cast<uint8>(guidLow % TILLERS_FARM_MGR_MUTEX_BUCKETS);
        std::lock_guard<std::mutex> lock(_mutexes[bucket]);
        PlayerFarmState& state = _playerStates[guidLow];
        return _playerPlots[guidLow];
    }

    /**
     * Check if a player has an item in inventory.
     */
    static bool HasItemInInventory(Player* player, uint32 entry, uint32 count = 1)
    {
        return player->HasItemCount(entry, count);
    }

   /**
    * Check if the farm is already spawned for a player (for zone hooks).
    */
    bool IsPlayerFarmSpawned(Player* player) const;

    /**
     * Get or create a reference to a player's state. Thread-safe via bucket mutex.
     */
    PlayerFarmState& GetPlayerState(uint32 guidLow);

    /**
     * Force all growing crops to mature immediately. Used by .tillers grow GM command.
     */
    void ForceGrowFarm(Player* player);

    /**
     * Reset a player's farm to initial state. Thread-safe.
     */
    void ResetPlayerFarm(uint32 guidLow);

    /**
     * Load plot positions from creature table (map 870, entry 55626, z~165).
     * Returns the list of plot positions for the farm area.
     * Called once at startup, cached in _plotPositions.
     */
    void LoadPlotPositions();

    /**
     * Get cached plot positions.
     */
    std::vector<PlotPosition> const& GetPlotPositions() const { return _plotPositions; }

    /**
     * Get plot position by plot ID. Returns true if found.
     */
    bool GetPlotPosition(uint8 plotId, PlotPosition& out) const;

    /**
     * Spawn a creature dynamically at a plot position for this player's phase.
     * Returns the spawned creature GUID.
     */
    ObjectGuid SpawnCreature(Player* player, uint32 entry, uint8 plotId, bool visible, uint32 phaseMask);

    /**
     * Spawn a creature at fixed world coordinates.
     * Returns the spawned creature GUID.
     */
    ObjectGuid SpawnCreatureAt(Player* player, uint32 entry, float posX, float posY, float posZ, float orientation, bool visible, uint32 phaseMask);

    /**
     * Despawn a creature by GUID.
     */
    void DespawnCreature(ObjectGuid guid, Map* map);

    /**
     * Despawn all dynamically spawned creatures for a player.
     */
    void DespawnAllCreatures(Player* player);

    /**
     * Calculate maturity timestamp based on seed type and current growth stage.
     */
    time_t GetMaturityTime(uint32 seedEntry, uint8 plotId);

    void SpawnFarmerNPCs(Player* player, uint32 farmPhaseMask);

    /** Validate that a plot ID is within bounds for the current farm configuration. */
    static bool IsValidPlotId(uint8 plotId) { return plotId < TILLERS_MAX_PLOTS; }

private:
    TillersFarmMgr() = default;
    ~TillersFarmMgr() = default;

    // Prevent copying
    TillersFarmMgr(TillersFarmMgr const&) = delete;
    TillersFarmMgr& operator=(TillersFarmMgr const&) = delete;

    /**
     * Load player farm state from database.
     * Returns true if valid state was loaded, false if defaults should be used.
     */
    bool LoadPlayerState(Player* player);

    /**
     * Save all plot states for a player to the database atomically.
     */
    void SavePlayerFarm(Player* player);

    /**
     * Create soil GameObjects for each unlocked plot in the player's phase.
     */
    void CreateSoilGos(Player* player, uint8 plotsCount, uint32 phaseMask);

    /**
     * Remove all soil GameObjects for a player from their phase.
     */
    void RemoveSoilGos(Player* player);

    /**
     * Spawn Tillers Shrine and Offering Bowls in the player's farm phase.
     */
    void SpawnFarmGameObjects(Player* player, uint32 phaseMask);

    /**
     * Remove all farm GameObjects (shrine, bowls) for a player.
     */
    void RemoveFarmGos(Player* player);

    // Player state storage: guid low -> farm phase info
    StateStore _playerStates;

    // Plot data storage: guid low -> (plot_id -> plot_data)
    std::unordered_map<uint32, PlotMap> _playerPlots;

    // Soil GO GUID tracking: guidLow -> list of spawned soil GO GUIDs for removal
    std::unordered_map<uint32, std::vector<ObjectGuid>> _playerSoilGOs;

    // Creature GUID tracking: guidLow -> list of spawned creature GUIDs for removal
    std::unordered_map<uint32, std::vector<ObjectGuid>> _playerCreatures;

    // Cached plot positions loaded from creature table (map 870, entry 55626)
    std::vector<PlotPosition> _plotPositions;

    // Farm GO tracking: guidLow -> list of spawned farm GO GUIDs (shrine, bowls, etc.)
    std::unordered_map<uint32, std::vector<ObjectGuid>> _playerFarmGOs;

    // 64-bucket striped mutex for thread-safe per-player access
    std::mutex _mutexes[TILLERS_FARM_MGR_MUTEX_BUCKETS];

public:
    // Soil GO entry
    static inline uint32 const SOIL_GO_ENTRY = 186314;

    // Tool item entries (publicly accessible from scripts)
    static inline uint32 const WATERING_CAN_ITEM = 79104;
    static inline uint32 const BUG_SPRAYER_ITEM  = 80513;
    static inline uint32 const SHOVEL_ITEM       = 89880;

    // Valley of Four Winds zone ID
    static inline uint32 const VFW_ZONE_ID = 1023;
};

#define sTillersFarmMgr TillersFarmMgr::getInstance()

#endif // TILLERS_FARM_MGR_H
