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

// Farm state bitmask values — each bit represents an obstacle type
// visibility: (playerPhase & obstaclePhase) != 0
// Obstacle phaseMasks: weeds=2, wagon=4, boulder=8
enum FarmState : uint8
{
    FARM_STATE_FULL              = 14,   // 0b1110 — all obstacles (4 plots)
    FARM_STATE_WEEDS_CLEARED     = 12,   // 0b1100 — wagon + boulder (8 plots)
    FARM_STATE_WAGON_CLEARED     = 8,    // 0b1000 — boulder only (12 plots)
    FARM_STATE_ALL_CLEARED       = 0     // 0b0000 — no obstacles (16 plots)
};

inline uint8 GetPlotsUnlockedForFarmState(uint8 farmState)
{
    switch (farmState)
    {
        case FARM_STATE_FULL:       return 4;
        case FARM_STATE_WEEDS_CLEARED: return 8;
        case FARM_STATE_WAGON_CLEARED: return 12;
        case FARM_STATE_ALL_CLEARED: return 16;
        default:                    return 0;
    }
}

inline uint8 GetFarmStateForPlots(uint8 plots)
{
    if (plots <= 4)
        return FARM_STATE_FULL;
    if (plots <= 8)
        return FARM_STATE_WEEDS_CLEARED;
    if (plots <= 12)
        return FARM_STATE_WAGON_CLEARED;
    return FARM_STATE_ALL_CLEARED;
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
    uint8   farmState       = FARM_STATE_FULL;
    uint8   plotsUnlocked   = 4;        // number of unlocked patches
    uint16  bestFriendUnlocks   = 0;    // bitmask of unlocked best friends
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

// Obstacle spawn data loaded from gameobject table
struct ObstacleSpawnData
{
    uint32 entry = 0;
    float posX = 0.0f;
    float posY = 0.0f;
    float posZ = 0.0f;
    float orientation = 0.0f;

    bool IsWeed() const
    {
        return entry == 210443 || entry == 210444 || entry == 210445 ||
               entry == 210446 || entry == 210447 || entry == 210448 || entry == 210462;
    }
    bool IsWagon() const { return entry == 210451; }
    bool IsBoulder() const { return entry == 209572; }
};

// Best Friend unlock spawn data loaded from creature/GO table
struct BestFriendUnlockPosition
{
    uint32 entry = 0;
    float posX = 0.0f;
    float posY = 0.0f;
    float posZ = 0.0f;
    float orientation = 0.0f;
};

// Best Friend companion metadata — maps bitmask → entry → faction
struct BestFriendData
{
    uint16      bit;
    uint32      entry;
    uint32      factionId;  // 0 = quest-based (Lost Dog 30526)
};

// Yoon spawn data loaded from creature table
struct YoonSpawnData
{
    bool loaded = false;
    float posX = 0.0f;
    float posY = 0.0f;
    float posZ = 0.0f;
    float orientation = 0.0f;
};

// Creature entries used for farm visual elements
static inline uint32 const PLOT_REFERENCE_CREATURE_ENTRY = 55626;   // Bunny — plot position reference

// Best Friend unlock bitmask — each bit represents a Best Friend or quest reward
#define BEST_FRIEND_SHAGGY          (1 << 0)  // Farmer Fung Best Friend — Yak
#define BEST_FRIEND_FIFI            (1 << 1)  // Haohan Mudclaw Best Friend — Mushan
#define BEST_FRIEND_CHICKENS        (1 << 2)  // Old Hillpaw Best Friend — Chickens
#define BEST_FRIEND_SHEEP           (1 << 3)  // Chee Chee Best Friend — Sheep
#define BEST_FRIEND_LUNA            (1 << 4)  // Ella Best Friend — Cat
#define BEST_FRIEND_PIGGY           (1 << 5)  // Fish Fellreed Best Friend — Pigs
#define BEST_FRIEND_ORANGE_TREE     (1 << 6)  // Sho Best Friend — Orange tree
#define BEST_FRIEND_FURNITURE       (1 << 7)  // Tina Mudclaw Best Friend — Furniture
#define BEST_FRIEND_MAILBOX         (1 << 8)  // Gina Mudclaw Best Friend — Mailbox
#define BEST_FRIEND_LOST_DOG        (1 << 9)  // Lost Dog quest (30526) — Dog

// Best Friend unlock creature/GO entries
static inline uint32 const SHAGGY_YAK_ENTRY             = 85814;
static inline uint32 const MISS_FIFI_MUSHAN_ENTRY       = 85791;
static inline uint32 const HILLPAW_CHICKENS_ENTRY       = 85820;
static inline uint32 const FARM_SHEEP_ENTRY             = 85808;
static inline uint32 const LUNA_CAT_ENTRY               = 85818;
static inline uint32 const PIGGY_PIG_ENTRY              = 85802;
static inline uint32 const ORANGE_TREE_ENTRY            = 237243;
static inline uint32 const FURNITURE_ENTRY              = 237244;
static inline uint32 const MAILBOX_ENTRY                = 237242;
static inline uint32 const LOST_DOG_ENTRY               = 85826;

// Best Friend companion lookup — maps bitmask to creature/GO entry and faction ID
// Order matches the bitmask constants above for indexed access
static inline BestFriendData const BestFriendCompanions[10] =
{
    { BEST_FRIEND_SHAGGY,       SHAGGY_YAK_ENTRY,       1283 },
    { BEST_FRIEND_FIFI,         MISS_FIFI_MUSHAN_ENTRY, 1279 },
    { BEST_FRIEND_CHICKENS,     HILLPAW_CHICKENS_ENTRY, 1276 },
    { BEST_FRIEND_SHEEP,        FARM_SHEEP_ENTRY,       1277 },
    { BEST_FRIEND_LUNA,         LUNA_CAT_ENTRY,         1275 },
    { BEST_FRIEND_PIGGY,        PIGGY_PIG_ENTRY,        1282 },
    { BEST_FRIEND_ORANGE_TREE,  ORANGE_TREE_ENTRY,      1278 },
    { BEST_FRIEND_FURNITURE,    FURNITURE_ENTRY,        1280 },
    { BEST_FRIEND_MAILBOX,      MAILBOX_ENTRY,          1281 },
    { BEST_FRIEND_LOST_DOG,     LOST_DOG_ENTRY,         0 },
};

// Helper: true if the entry is a gameobject (not a creature)
inline bool IsGameObjectEntry(uint32 entry)
{
    return entry == ORANGE_TREE_ENTRY || entry == FURNITURE_ENTRY || entry == MAILBOX_ENTRY;
}

// Seed item → vegetable item mapping for Tillers farming rewards
// Retail: base harvest yield is 5 vegetables per crop
static inline uint32 const TILLERS_VEGETABLE_GREEN_CABBAGE = 74840;
static inline uint32 const TILLERS_VEGETABLE_JUICYCRUNCH_CARROT = 74841;
static inline uint32 const TILLERS_VEGETABLE_MOGU_PUMPKIN = 74842;
static inline uint32 const TILLERS_VEGETABLE_SCALLIONS = 74843;
static inline uint32 const TILLERS_VEGETABLE_RED_BLOSSOM_LEEK = 74844;
static inline uint32 const TILLERS_VEGETABLE_JADE_SQUASH = 74847;
static inline uint32 const TILLERS_VEGETABLE_STRIPED_MELON = 74848;
static inline uint32 const TILLERS_VEGETABLE_PINK_TURNIP = 74849;
static inline uint32 const TILLERS_VEGETABLE_WHITE_TURNIP = 74850;

// Default harvest yield per crop (base retail value)
static inline uint8 const TILLERS_HARVEST_YIELD = 5;

// Seed item → vegetable item mapping
inline uint32 GetVegetableForSeed(uint32 seedEntry)
{
    switch (seedEntry)
    {
        case 79102:  return TILLERS_VEGETABLE_GREEN_CABBAGE;     // Green Cabbage Seeds
        case 80590:  return TILLERS_VEGETABLE_JUICYCRUNCH_CARROT; // Juicycrunch Carrot Seeds
        case 80591:  return TILLERS_VEGETABLE_SCALLIONS;          // Scallion Seeds
        case 80592:  return TILLERS_VEGETABLE_MOGU_PUMPKIN;       // Mogu Pumpkin Seeds
        case 80593:  return TILLERS_VEGETABLE_RED_BLOSSOM_LEEK;   // Red Blossom Leek Seeds
        case 80594:  return TILLERS_VEGETABLE_PINK_TURNIP;        // Pink Turnip Seeds
        case 80595:  return TILLERS_VEGETABLE_WHITE_TURNIP;       // White Turnip Seeds
        case 89328:  return TILLERS_VEGETABLE_JADE_SQUASH;        // Jade Squash Seeds
        case 89329:  return TILLERS_VEGETABLE_STRIPED_MELON;      // Striped Melon Seeds
        default:     return 0;  // unknown seed
    }
}

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
     * Calculate maturity timestamp based on seed type and current growth stage.
     */
    time_t GetMaturityTime(uint32 seedEntry, uint8 plotId);

    /** Validate that a plot ID is within bounds for the current farm configuration. */
    static bool IsValidPlotId(uint8 plotId) { return plotId < TILLERS_MAX_PLOTS; }

    /**
     * Get cached obstacle spawn positions.
     */
    std::vector<ObstacleSpawnData> const& GetObstaclePositions() const { return _obstaclePositions; }

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
     * Spawn ground-level Farmer Yoon in the player's farm phase.
     */
    void SpawnYoon(Player* player, uint32 phaseMask);

    /**
     * Remove all dynamically spawned farm creatures (Yoon).
     */
    void RemoveSpawnedCreatures(Player* player);

    /**
     * Spawn farm obstacles (weeds, wagon, boulder) based on farmState.
     * Only creates GOs whose type is still visible at the current farmState.
     */
    void SpawnObstacles(Player* player, uint8 farmState, uint32 phaseMask);

    /**
     * Remove all dynamically spawned farm obstacles.
     */
    void RemoveObstacles(Player* player);

    /**
     * Spawn all unlocked best friend companions for a player's farm.
     * Iterates bestFriendUnlocks bitmask, spawning creatures and GOs.
     */
    void SpawnPlayerFarmCompanions(Player* player, uint32 phaseMask);

    /**
     * Spawn a best friend unlock creature.
     */
    Creature* SpawnBestFriendUnlock(Player* player, uint32 entry, uint32 phaseMask, float posX, float posY, float posZ, float orientation);

    /**
     * Spawn a best friend unlock gameobject.
     */
    GameObject* SpawnBestFriendUnlockGO(Player* player, uint32 entry, uint32 phaseMask, float posX, float posY, float posZ, float orientation);

    /**
     * Remove all dynamically spawned best friend unlock creatures/GOs for a player.
     */
    void RemoveBestFriendUnlocks(Player* player);

    /**
     * Load best friend unlock positions from creature/GO table.
     * Called once at startup, cached in _bestFriendUnlockPositions.
     */
    void LoadBestFriendUnlockPositions();

    /**
     * Get cached best friend unlock positions.
     */
    std::vector<BestFriendUnlockPosition> const& GetBestFriendUnlockPositions() const { return _bestFriendUnlockPositions; }

    /**
     * Get best friend unlock position by entry. Returns true if found.
     */
    bool GetBestFriendUnlockPosition(uint32 entry, BestFriendUnlockPosition& out) const;

    /**
     * Check if a player has Best Friend status with a specific Tillers NPC (Exalted reputation, factionId).
     * For Lost Dog (factionId=0), returns false — use IsQuestRewarded instead.
     */
    bool IsBestFriend(Player* player, uint32 factionId) const;

    /**
     * Update best friend unlock state based on current Tillers reputations.
     * Called after state load to detect newly unlocked best friends.
     */
    void UpdateBestFriendUnlockState(Player* player);

    /**
     * Load ground-level Yoon spawn position from creature table.
     * Called once at startup, cached in _yoonSpawnData.
     */
    void LoadYoonPosition();

    /**
     * Load obstacle positions from gameobject table.
     * Called once at startup, cached in _obstaclePositions.
     */
    void LoadObstaclePositions();

    // Player state storage: guid low -> farm phase info
    StateStore _playerStates;

    // Plot data storage: guid low -> (plot_id -> plot_data)
    std::unordered_map<uint32, PlotMap> _playerPlots;

    // Soil GO GUID tracking: guidLow -> list of spawned soil GO GUIDs for removal
    std::unordered_map<uint32, std::vector<ObjectGuid>> _playerSoilGOs;

    // Spawned creature tracking: guidLow -> list of spawned creature GUIDs for removal
    std::unordered_map<uint32, std::vector<ObjectGuid>> _playerSpawnedCreatures;

    // Obstacle GO tracking: guidLow -> list of spawned obstacle GO GUIDs for removal
    std::unordered_map<uint32, std::vector<ObjectGuid>> _playerObstacleGOs;

    // Best friend unlock tracking: guidLow -> list of spawned best friend unlock GUIDs for removal
    std::unordered_map<uint32, std::vector<ObjectGuid>> _playerBestFriendUnlocks;

    // Cached plot positions loaded from creature table (map 870, entry 55626)
    std::vector<PlotPosition> _plotPositions;

    // Cached Yoon spawn position loaded from creature table
    YoonSpawnData _yoonSpawnData;

    // Cached obstacle positions loaded from gameobject table
    std::vector<ObstacleSpawnData> _obstaclePositions;

    // Cached best friend unlock positions loaded from creature/GO table
    std::vector<BestFriendUnlockPosition> _bestFriendUnlockPositions;

    // 64-bucket striped mutex for thread-safe per-player access
    std::mutex _mutexes[TILLERS_FARM_MGR_MUTEX_BUCKETS];

public:
    // Soil GO entry
    static inline uint32 const SOIL_GO_ENTRY = 186314;

    // Farm NPC entries
    static inline uint32 const FARMER_YOON_ENTRY = 58646;

    // Tool item entries (publicly accessible from scripts)
    static inline uint32 const WATERING_CAN_ITEM = 79104;
    static inline uint32 const BUG_SPRAYER_ITEM  = 80513;
    static inline uint32 const SHOVEL_ITEM       = 89880;

    // Valley of Four Winds zone ID
    static inline uint32 const VFW_ZONE_ID = 1023;

    // Public farm mask — static farm objects visible to pre-farm players via phase definition
    static inline uint32 const PUBLIC_FARM_MASK = 128;
};

#define sTillersFarmMgr TillersFarmMgr::getInstance()

#endif // TILLERS_FARM_MGR_H
