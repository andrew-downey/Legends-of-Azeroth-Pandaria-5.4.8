/*
 * This file is part of the Legends of Azeroth Pandaria Project. See THANKS file for Copyright information
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

#ifndef TILLERS_FARM_H
#define TILLERS_FARM_H

#include "Define.h"
#include "ObjectGuid.h"
#include "Position.h"
#include <vector>
#include <map>

// Zone/Area IDs
constexpr uint32 VALLEY_OF_FOUR_WINDS_ZONE = 5805;
constexpr uint32 SUNSONG_RANCH_AREA = 5805;

// The creature entry used for all farm plot states (ScriptName: 'npc_farm_plot')
constexpr uint32 NPC_FARM_PLOT = 59984;

// Farm plot tool item IDs
constexpr uint32 ITEM_RUSTY_WATERING_CAN = 79104;
constexpr uint32 ITEM_VINTAGE_BUG_SPRAYER = 80513;
constexpr uint32 ITEM_DENTED_SHOVEL = 89880;
constexpr uint32 ITEM_EARTH_SLASHER = 89814;
constexpr uint32 ITEM_JINYU_PRINCESS_SPRINKLER = 89812;
constexpr uint32 ITEM_THUNDER_KING_PEST_REPELLERS = 89813;

constexpr uint32 ITEM_MASTER_PLOW = 89815;

// Farming spell IDs
constexpr uint32 SPELL_WATER_PLOT = 111108;
constexpr uint32 SPELL_SPRAY_PLOT = 125005;
constexpr uint32 SPELL_TILL_SOIL = 125012;
constexpr uint32 SPELL_DIG_UP_PLANT = 125013;
constexpr uint32 SPELL_HARVEST = 125014;
constexpr uint32 SPELL_PLANT_SEED = 125015;

// Max plots
constexpr uint8 MAX_FARM_PLOTS = 16;

// Growth time: 24 hours in seconds
constexpr uint32 FARM_GROWTH_TIME = 86400;

// Expansion timer: 15 minutes in seconds (Growing the Farm quests)
constexpr uint32 FARM_EXPANSION_TIME = 900;

// Farmer Yoon NPC entry
constexpr uint32 NPC_FARMER_YOON = 58646;

// Reputation thresholds
constexpr uint32 REP_TILLERS_HONORED = 9000;
constexpr uint32 REP_TILLERS_REVERED = 21000;
constexpr uint32 REP_TILLERS_EXALTED = 42000;
constexpr uint32 REP_TILLERS_NEUTRAL_MIN = 3000;

// Tillers faction
constexpr int32 FACTION_TILLERS = 1074;

// Harvest reputation gain (at level 90)
constexpr int32 HARVEST_REP_GAIN = 50;

// Tiller gift item (rare drop from harvest)
constexpr uint32 ITEM_TILLER_GIFT = 79246;

// Pandaren herb item IDs (Enigma seed harvest)
constexpr uint32 HERB_GREEN_TEA_LEAF = 72234;
constexpr uint32 HERB_SILKWEED = 72235;
constexpr uint32 HERB_RAIN_POPPY = 72237;
constexpr uint32 HERB_GOLDEN_LOTUS = 72238;
constexpr uint32 HERB_SNOW_LILY = 79010;

// Summoned NPC entries for crop condition mini-games
constexpr uint32 NPC_PLAINSHAWK = 58910;
constexpr uint32 NPC_VIRMEN = 55483;

// Post-harvest soil state chances
constexpr uint32 CHANCE_STUBBORN_SOIL = 15;
constexpr uint32 CHANCE_OCCUPIED_SOIL = 10;
constexpr uint32 CHANCE_TILLER_GIFT = 5;

enum class FarmPlotState : uint8
{
    UNTILLED = 0,
    TILLED = 1,
    GROWING = 2,
    RIPE = 3,
    STUBBORN = 4,
    OCCUPIED = 5,
    MAX
};

enum class CropType : uint8
{
    NONE = 0,
    GREEN_CABBAGE = 1,
    SCALLION = 2,
    JUICYCRUNCH_CARROT = 3,
    WHITE_TURNIP = 4,
    MOGU_PUMPKIN = 5,
    STRIPED_MELON = 6,
    RED_BLOSSOM_LEEK = 7,
    SUCCULENT_PLUM = 8,
    YELLOW_CABBAGE = 9,
    WITCHBERRY = 10,
    PINK_TURNIP = 11,
    JADE_SQUASH = 12,
    SNAKEROOT = 13,
    ENIGMA = 14,
    MAGEBULB = 15,
    WINDSHEAR_CACTUS = 16,
    RAPTORLEAF = 17,
    SONGBELL = 18,
    AUTUMN_BLOSSOM = 19,
    SPRING_BLOSSOM = 20,
    WINTER_BLOSSOM = 21,
    MAX
};

enum class FarmCondition : uint8
{
    HEALTHY = 0,
    PARCHED = 1,
    PESTS = 2,
    WEEDS = 3,
    ALLURING = 4,
    WIGGLING = 5,
    SMOTHERED = 6,
    WILD = 7,
    RUNTY = 8,
    TANGLED = 9,
    MAX
};

enum class SpecialCrop : uint8
{
    NONE = 0,
    PLUMP = 1,
    BURSTING = 2,
};

struct FarmPlotData
{
    uint8 plotId;
    FarmPlotState state;
    CropType crop;
    uint32 plantedAt;
    FarmCondition condition;
    SpecialCrop special;
    ObjectGuid creatureGuid;

    FarmPlotData() : plotId(0), state(FarmPlotState::UNTILLED), crop(CropType::NONE),
                     plantedAt(0), condition(FarmCondition::HEALTHY), special(SpecialCrop::NONE) {}
};

struct FarmToolUpgrades
{
    bool hasIrrigation;
    bool hasAntipest;
    bool hasPlow;

    FarmToolUpgrades() : hasIrrigation(false), hasAntipest(false), hasPlow(false) {}
};

struct PlayerFarmCache
{
    uint8                             unlockedPlots;
    FarmToolUpgrades                  upgrades;
    uint8                             votesMask;
    uint32                            expansionTimerEnd;
    std::map<uint8, FarmPlotData>     plots;

    PlayerFarmCache()
        : unlockedPlots(4), votesMask(0), expansionTimerEnd(0) {}
};

// Fixed spawn positions for farm plots at Sunsong Ranch
extern Position const FarmPlotPositions[MAX_FARM_PLOTS];

// Number of plots unlocked at each reputation tier
extern uint32 const PlotsPerTier[4];

// Map of crop type to item ID (seeds)
extern std::map<CropType, uint32> const CropSeedMap;

// Map of crop type to item ID (harvested food)
extern std::map<CropType, uint32> const CropHarvestMap;

// Bonus crop rotation schedule (crop type for each day of the 10-day cycle)
extern CropType const BonusCropSchedule[10];

// Helper: get the display ID for a given state/crop combination
[[nodiscard]] uint32 GetFarmPlotDisplayId(FarmPlotState state, CropType crop, FarmCondition condition, SpecialCrop special);

// Helper: get the crop item name (for gossip texts)
[[nodiscard]] char const* GetCropName(CropType crop);

// Helper: roll for condition when growth completes
[[nodiscard]] FarmCondition RollFarmCondition();

// Helper: roll for special crop at harvest
[[nodiscard]] SpecialCrop RollSpecialCrop();

// Helper: update growth timers and advance GROWING->RIPE if ready
void UpdateGrowthTimers(Player* player, std::map<uint8, FarmPlotData>& plots);

// Helper: check if farm expansion timer expired, advance to next tier if so
bool CheckFarmExpiration(Player* player);

// Access farm data for a player (thread-safe)
PlayerFarmCache* GetPlayerFarmData(ObjectGuid guid);

// ============================================================================
// Friendship System
// ============================================================================

constexpr uint32 FACTION_TILLER_CHEE_CHEE = 2091;
constexpr uint32 FACTION_TILLER_ELLA = 2092;
constexpr uint32 FACTION_TILLER_FARMER_FUNG = 2093;
constexpr uint32 FACTION_TILLER_FISH_FELLREED = 2094;
constexpr uint32 FACTION_TILLER_GINA_MUDCLAW = 2095;
constexpr uint32 FACTION_TILLER_HAOHAN_MUDCLAW = 2096;
constexpr uint32 FACTION_TILLER_JOGU = 2097;
constexpr uint32 FACTION_TILLER_OLD_HILLPAW = 2098;
constexpr uint32 FACTION_TILLER_SHO = 2099;
constexpr uint32 FACTION_TILLER_TINA_MUDCLAW = 2100;

constexpr uint32 NPC_CHEE_CHEE = 58712;
constexpr uint32 NPC_ELLA = 58703;
constexpr uint32 NPC_FARMER_FUNG = 58707;
constexpr uint32 NPC_FISH_FELLREED = 58706;
constexpr uint32 NPC_GINA_MUDCLAW = 58547;
constexpr uint32 NPC_HAOHAN_MUDCLAW = 58746;
constexpr uint32 NPC_JOGU = 58704;
constexpr uint32 NPC_OLD_HILLPAW = 58705;
constexpr uint32 NPC_SHO = 58709;
constexpr uint32 NPC_TINA_MUDCLAW = 58711;

// Friendship gift items (Dark Soil treasures)
constexpr uint32 ITEM_RUBY_SHARD = 79264;
constexpr uint32 ITEM_BLUE_FEATHER = 79265;
constexpr uint32 ITEM_JADE_CAT = 79266;
constexpr uint32 ITEM_LOVELY_APPLE = 79267;
constexpr uint32 ITEM_MARSH_LILY = 79268;

// Tiller NPC preferred gift items (entry → item ID)
constexpr uint32 GIFT_CHEE_CHEE = ITEM_BLUE_FEATHER;
constexpr uint32 GIFT_ELLA = ITEM_JADE_CAT;
constexpr uint32 GIFT_FARMER_FUNG = ITEM_MARSH_LILY;
constexpr uint32 GIFT_FISH_FELLREED = ITEM_JADE_CAT;
constexpr uint32 GIFT_GINA_MUDCLAW = ITEM_MARSH_LILY;
constexpr uint32 GIFT_HAOHAN_MUDCLAW = ITEM_RUBY_SHARD;
constexpr uint32 GIFT_JOGU = ITEM_LOVELY_APPLE;
constexpr uint32 GIFT_OLD_HILLPAW = ITEM_BLUE_FEATHER;
constexpr uint32 GIFT_SHO = ITEM_LOVELY_APPLE;
constexpr uint32 GIFT_TINA_MUDCLAW = ITEM_RUBY_SHARD;

// Dark Soil treasure items
extern uint32 const DarkSoilTreasures[5];
extern char const* DarkSoilTreasureNames[5];

// Tiller NPC preferred food dishes (entry → item ID)
constexpr uint32 FOOD_CHEE_CHEE = 74647;
constexpr uint32 FOOD_ELLA = 74651;
constexpr uint32 FOOD_FARMER_FUNG = 74654;
constexpr uint32 FOOD_FISH_FELLREED = 74655;
constexpr uint32 FOOD_GINA_MUDCLAW = 74644;
constexpr uint32 FOOD_HAOHAN_MUDCLAW = 74642;
constexpr uint32 FOOD_JOGU = 74643;
constexpr uint32 FOOD_OLD_HILLPAW = 74649;
constexpr uint32 FOOD_SHO = 74645;
constexpr uint32 FOOD_TINA_MUDCLAW = 74652;

enum class FriendshipRank : int32
{
    STRANGER = 0,
    ACQUAINTANCE = 8400,
    BUDDY = 16800,
    FRIEND = 25200,
    GOOD_FRIEND = 33600,
    BEST_FRIEND = 42000,
};

constexpr int32 FRIENDSHIP_DAILY_FOOD_GAIN = 1800;
constexpr int32 FRIENDSHIP_DAILY_GIFT_GAIN = 540;
constexpr int32 FRIENDSHIP_IDEAL_GIFT_GAIN = 900;
constexpr uint32 FRIENDSHIP_DAILY_RESET_INTERVAL = 86400;

[[nodiscard]] char const* GetFriendshipRankName(FriendshipRank rank);
[[nodiscard]] FriendshipRank GetFriendshipRank(int32 standing);

struct FriendshipEntry
{
    int32 standing;
    uint32 lastDailyFoodTime;
    uint32 lastDailyGiftTime;

    FriendshipEntry() : standing(0), lastDailyFoodTime(0), lastDailyGiftTime(0) {}
};

class FriendshipManager
{
public:
    static FriendshipManager* instance()
    {
        static FriendshipManager instance;
        return &instance;
    }

    bool LoadFromDB(Player* player);
    void SaveToDB(ObjectGuid guid);
    void Unload(ObjectGuid guid);

    FriendshipEntry* GetFriendship(ObjectGuid playerGuid, uint32 npcEntry);
    void ModifyStanding(ObjectGuid playerGuid, uint32 npcEntry, int32 amount);
    bool UpdateDailyFoodTimer(ObjectGuid playerGuid, uint32 npcEntry);
    bool UpdateDailyGiftTimer(ObjectGuid playerGuid, uint32 npcEntry);

private:
    std::map<ObjectGuid, std::map<uint32, FriendshipEntry>> _friendships;
    std::mutex _mutex;
};

#define sFriendship FriendshipManager::instance()

// ============================================================================
// Daily Quest System
// ============================================================================

constexpr uint32 NPC_ANDI = 58708;

// Tutorial gate — Learn and Grow V: Halfhill Market
// Complete this quest to unlock your farm
constexpr uint32 QUEST_TUTORIAL_GATE = 30257;

constexpr uint32 QUEST_CROP_BASE = 32000;
constexpr uint32 QUEST_CROP_PUMPKIN = 32000;
constexpr uint32 QUEST_CROP_MELON = 32001;
constexpr uint32 QUEST_CROP_LEEK = 32002;
constexpr uint32 QUEST_CROP_SQUASH = 32003;
constexpr uint32 QUEST_CROP_CARROT = 32004;
constexpr uint32 QUEST_CROP_CABBAGE = 32005;
constexpr uint32 QUEST_CROP_PINK_TURNIP = 32006;
constexpr uint32 QUEST_CROP_WHITE_TURNIP = 32007;
constexpr uint32 QUEST_CROP_WITCHBERRY = 32008;
constexpr uint32 QUEST_CROP_SCALLION = 32009;

constexpr uint32 QUEST_ANDI_GIFT = 32010;

constexpr uint32 QUEST_VISITING_BASE = 32011;
constexpr uint32 QUEST_VISITING_CHEE_CHEE = 32011;
constexpr uint32 QUEST_VISITING_ELLA = 32012;
constexpr uint32 QUEST_VISITING_FUNG = 32013;
constexpr uint32 QUEST_VISITING_FELLREED = 32014;
constexpr uint32 QUEST_VISITING_GINA = 32015;
constexpr uint32 QUEST_VISITING_JOGU = 32016;
constexpr uint32 QUEST_VISITING_SHO = 32017;
constexpr uint32 QUEST_VISITING_TINA = 32018;

constexpr uint32 QUEST_KILL_BASE = 32019;
constexpr uint32 QUEST_KILL_LESSER_EVILS = 32019;
constexpr uint32 QUEST_KILL_STEALING = 32020;
constexpr uint32 QUEST_KILL_STALLING = 32021;
constexpr uint32 QUEST_KILL_HUNTER_CHIEF = 32022;
constexpr uint32 QUEST_KILL_SIMIAN = 32023;

constexpr uint32 ITEM_ANDI_GIFT = 79247;

constexpr uint32 DAILY_CROP_REWARD_REP = 350;
constexpr uint32 DAILY_ANDI_REWARD_REP = 150;
constexpr uint32 DAILY_ANDI_FRIENDSHIP_GAIN = 1400;
constexpr uint32 DAILY_VISITING_REWARD_REP = 150;
constexpr uint32 DAILY_VISITING_FRIENDSHIP_GAIN = 2000;
constexpr uint32 DAILY_KILL_REWARD_REP = 275;

constexpr uint8 CROP_DAILY_COUNT = 10;
constexpr uint8 VISITING_FARMER_COUNT = 8;
constexpr uint8 KILL_DAILY_COUNT = 5;

constexpr uint8 VISITING_FARMER_EXCLUDED[] =
{
    NPC_HAOHAN_MUDCLAW,
    NPC_OLD_HILLPAW
};

struct DailyQuestData
{
    uint32 activeCropDaily;
    uint32 activeKillDaily;
    uint8 AndiGiftTarget;
    uint8 activeVisitingFarmers[2];
    uint32 dailyQuestsMask;
    uint32 lastDailyReset;

    DailyQuestData()
        : activeCropDaily(0), activeKillDaily(0), AndiGiftTarget(0),
          dailyQuestsMask(0), lastDailyReset(0)
    {
        activeVisitingFarmers[0] = 0;
        activeVisitingFarmers[1] = 0;
    }
};

class DailyQuestManager
{
public:
    static DailyQuestManager* instance()
    {
        static DailyQuestManager instance;
        return &instance;
    }

    bool LoadFromDB(Player* player);
    void SaveToDB(ObjectGuid guid);
    void Unload(ObjectGuid guid);

    bool IsDailyComplete(ObjectGuid playerGuid, uint32 questId);
    void CompleteDaily(ObjectGuid playerGuid, uint32 questId);
    void ResetDailies(ObjectGuid playerGuid);

    uint32 GetRandomCropDaily(ObjectGuid playerGuid);
    uint32 GetRandomKillDaily(ObjectGuid playerGuid);
    uint32 GetAndiGiftTarget(ObjectGuid playerGuid);
    uint8 GetActiveVisitingFarmers(ObjectGuid playerGuid, uint8 index);

    DailyQuestData* GetDailyData(ObjectGuid playerGuid);

private:
    std::map<ObjectGuid, DailyQuestData> _dailyData;
    std::mutex _mutex;
};

#define sDailyQuest DailyQuestManager::instance()

// Free function wrappers declared in header so all Tillers scripts can use them
void CreatePlayerFarm(Player* player);

struct CropDailyAssignment
{
    uint32 questId;
    CropType crop;
    uint8 requiredCount;
    char const* questName;
};

// ============================================================================
// Vote Questlines (Tilliers Union)
// ============================================================================

constexpr uint32 NPC_MUNG_MUNG = 58733;
constexpr uint32 NPC_NANA_MUDCLAW = 64597;

// Vote bit definitions (mapped to votesMask in PlayerFarmCache)
constexpr uint8 VOTE_GINA        = 1;   // bit 0
constexpr uint8 VOTE_MUNG_MUNG   = 2;   // bit 1
constexpr uint8 VOTE_FARMER_FUNG = 4;   // bit 2
constexpr uint8 VOTE_NANA        = 8;   // bit 3
constexpr uint8 VOTE_HAOHAN      = 16;  // bit 4
constexpr uint8 VOTE_ALL         = 31;  // all 5 bits

// Vote crop requirement items and quantities
constexpr uint32 VOTE_ITEM_GINA        = 74843; // Scallions
constexpr uint32 VOTE_ITEM_MUNG_MUNG   = 74841; // Juicycrunch Carrot
constexpr uint32 VOTE_ITEM_FARMER_FUNG = 74840; // Green Cabbage
constexpr uint32 VOTE_ITEM_NANA        = 74846; // Witchberries
constexpr uint32 VOTE_ITEM_HAOHAN      = 74848; // Striped Melon

constexpr uint8 VOTE_QTY_GINA        = 5;
constexpr uint8 VOTE_QTY_MUNG_MUNG   = 10;
constexpr uint8 VOTE_QTY_FARMER_FUNG = 15;
constexpr uint8 VOTE_QTY_NANA        = 15;
constexpr uint8 VOTE_QTY_HAOHAN      = 20;

// Reputation thresholds for vote availability
constexpr int32 VOTE_REP_GINA        = REP_TILLERS_NEUTRAL_MIN;
constexpr int32 VOTE_REP_MUNG_MUNG   = 5500;  // mid-Honored
constexpr int32 VOTE_REP_FARMER_FUNG = 13000; // early-Revered
constexpr int32 VOTE_REP_NANA        = 17000; // mid-Revered
constexpr int32 VOTE_REP_HAOHAN      = 28000; // late-Revered

// Vote reward constants
constexpr int32 VOTE_REP_GAIN        = 350;
constexpr int32 VOTE_FRIENDSHIP_GAIN = 2000;

struct VoteData
{
    uint8   voteBit;
    uint32  npcEntry;
    uint32  cropItem;
    uint8   requiredCount;
    int32   repThreshold;
    char const* name;
    char const* cropName;
};

extern VoteData const VoteRequirements[5];

extern CropDailyAssignment const CropDailies[CROP_DAILY_COUNT];

void AddSC_tillers_friendship();
void AddSC_tillers_shrine();
void AddSC_tillers_dailies();
void AddSC_npc_farmer_yoon();

#endif
