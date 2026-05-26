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

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "ScriptedGossip.h"
#include "Player.h"
#include "Creature.h"
#include "CreatureAI.h"
#include "DatabaseEnv.h"
#include "ObjectMgr.h"
#include "World.h"
#include "SpellScript.h"
#include "SpellAuraEffects.h"
#include "GossipDef.h"
#include "Group.h"
#include "GameObject.h"
#include "GameObjectAI.h"
#include "ReputationMgr.h"
#include "DBCStores.h"
#include "ObjectAccessor.h"

#include "tillers_farm.h"
#include <mutex>

// ============================================================================
// Static Data Definitions
// ============================================================================

Position const FarmPlotPositions[16] =
{
    { 848.00f, 942.00f, 174.62f, 0.0f },
    { 852.00f, 944.50f, 174.62f, 0.0f },
    { 854.00f, 948.50f, 174.62f, 0.0f },
    { 850.50f, 951.00f, 174.62f, 0.0f },
    { 856.00f, 952.50f, 174.62f, 0.0f },
    { 858.50f, 946.00f, 174.62f, 0.0f },
    { 862.00f, 949.00f, 174.62f, 0.0f },
    { 855.00f, 956.00f, 174.62f, 0.0f },
    { 864.00f, 954.00f, 174.62f, 0.0f },
    { 860.00f, 958.00f, 174.62f, 0.0f },
    { 868.00f, 951.00f, 174.62f, 0.0f },
    { 866.50f, 956.50f, 174.62f, 0.0f },
    { 870.00f, 947.00f, 174.62f, 0.0f },
    { 872.00f, 953.00f, 174.62f, 0.0f },
    { 875.00f, 950.00f, 174.62f, 0.0f },
    { 870.00f, 956.00f, 174.62f, 0.0f },
};

uint32 const PlotsPerTier[4] = { 4, 8, 12, 16 };

std::map<CropType, uint32> const CropSeedMap =
{
    { CropType::GREEN_CABBAGE,      79102 },
    { CropType::SCALLION,           79103 },
    { CropType::JUICYCRUNCH_CARROT, 79321 },
    { CropType::WHITE_TURNIP,       79322 },
    { CropType::MOGU_PUMPKIN,       79323 },
    { CropType::STRIPED_MELON,      79324 },
    { CropType::RED_BLOSSOM_LEEK,   79325 },
    { CropType::SUCCULENT_PLUM,     79326 },
    { CropType::YELLOW_CABBAGE,     79327 },
    { CropType::WITCHBERRY,         79328 },
    { CropType::PINK_TURNIP,        79329 },
    { CropType::JADE_SQUASH,        79330 },
    // Profession seeds (Revered+ reputation)
    { CropType::SNAKEROOT,          85215 },
    { CropType::ENIGMA,             85216 },
    { CropType::MAGEBULB,           85217 },
    { CropType::WINDSHEAR_CACTUS,   89197 },
    { CropType::RAPTORLEAF,         89202 },
    { CropType::SONGBELL,           89233 },
    // Blossom tree saplings
    { CropType::AUTUMN_BLOSSOM,     85267 },
    { CropType::SPRING_BLOSSOM,     85268 },
    { CropType::WINTER_BLOSSOM,     85269 },
};

std::map<CropType, uint32> const CropHarvestMap =
{
    { CropType::GREEN_CABBAGE,      74847 },
    { CropType::SCALLION,           74848 },
    { CropType::JUICYCRUNCH_CARROT, 74849 },
    { CropType::WHITE_TURNIP,       74850 },
    { CropType::MOGU_PUMPKIN,       74851 },
    { CropType::STRIPED_MELON,      74852 },
    { CropType::RED_BLOSSOM_LEEK,   74853 },
    { CropType::SUCCULENT_PLUM,     74854 },
    { CropType::YELLOW_CABBAGE,     74855 },
    { CropType::WITCHBERRY,         74856 },
    { CropType::PINK_TURNIP,        74857 },
    { CropType::JADE_SQUASH,        74858 },
    // Profession seed harvest products
    { CropType::SNAKEROOT,          72092 },   // Ghost Iron Ore
    { CropType::ENIGMA,             0 },       // Special: random herb handled in harvest code
    { CropType::MAGEBULB,           74249 },   // Spirit Dust
    { CropType::WINDSHEAR_CACTUS,   72988 },   // Windwool Cloth
    { CropType::RAPTORLEAF,         72162 },   // Sha-Touched Leather
    { CropType::SONGBELL,           89112 },   // Mote of Harmony
    // Blossom tree saplings (mature tree versions)
    { CropType::AUTUMN_BLOSSOM,     85264 },   // Autumn Blossom Tree
    { CropType::SPRING_BLOSSOM,     85265 },   // Spring Blossom Tree
    { CropType::WINTER_BLOSSOM,     85266 },   // Winter Blossom Tree
};

// Pandaren herbs for Enigma seed harvest (random selection)
uint32 const EnigmaHerbs[5] =
{
    72234, // Green Tea Leaf
    72235, // Silkweed
    72237, // Rain Poppy
    72238, // Golden Lotus
    79010, // Snow Lily
};

CropType const BonusCropSchedule[10] =
{
    CropType::WHITE_TURNIP,
    CropType::WITCHBERRY,
    CropType::JADE_SQUASH,
    CropType::STRIPED_MELON,
    CropType::GREEN_CABBAGE,
    CropType::JUICYCRUNCH_CARROT,
    CropType::SCALLION,
    CropType::MOGU_PUMPKIN,
    CropType::PINK_TURNIP,
    CropType::RED_BLOSSOM_LEEK,
};

uint32 const DarkSoilTreasures[5] =
{
    ITEM_RUBY_SHARD,    // 79264
    ITEM_BLUE_FEATHER,  // 79265
    ITEM_JADE_CAT,      // 79266
    ITEM_LOVELY_APPLE,  // 79267
    ITEM_MARSH_LILY,    // 79268
};

char const* DarkSoilTreasureNames[5] =
{
    "Ruby Shard",
    "Blue Feather",
    "Jade Cat",
    "Lovely Apple",
    "Marsh Lily"
};

// ============================================================================
// Helper Functions
// ============================================================================

uint32 GetFarmPlotDisplayId(FarmPlotState state, CropType crop, FarmCondition condition, SpecialCrop special)
{
    switch (state)
    {
        case FarmPlotState::UNTILLED: return 38429;
        case FarmPlotState::TILLED:   return 38430;
        case FarmPlotState::GROWING:
            if (condition == FarmCondition::PARCHED)   return 38431;
            if (condition == FarmCondition::PESTS)     return 38432;
            if (condition == FarmCondition::WEEDS)     return 38433;
            if (condition == FarmCondition::ALLURING)  return 38432;
            if (condition == FarmCondition::WIGGLING)  return 38432;
            if (condition == FarmCondition::SMOTHERED) return 38433;
            if (condition == FarmCondition::WILD)      return 38434;
            if (condition == FarmCondition::RUNTY)     return 38434;
            if (condition == FarmCondition::TANGLED)   return 38434;
            return 38434;
        case FarmPlotState::RIPE:
            if (special == SpecialCrop::PLUMP)    return 38435;
            if (special == SpecialCrop::BURSTING) return 38436;
            return 38437;
        case FarmPlotState::STUBBORN: return 38429;
        case FarmPlotState::OCCUPIED: return 38429;
        default:
            return 38429;
    }
}

char const* GetCropName(CropType crop)
{
    switch (crop)
    {
        case CropType::GREEN_CABBAGE:      return "Green Cabbage";
        case CropType::SCALLION:           return "Scallion";
        case CropType::JUICYCRUNCH_CARROT: return "Juicycrunch Carrot";
        case CropType::WHITE_TURNIP:       return "White Turnip";
        case CropType::MOGU_PUMPKIN:       return "Mogu Pumpkin";
        case CropType::STRIPED_MELON:      return "Striped Melon";
        case CropType::RED_BLOSSOM_LEEK:   return "Red Blossom Leek";
        case CropType::SUCCULENT_PLUM:     return "Succulent Plum";
        case CropType::YELLOW_CABBAGE:     return "Yellow Cabbage";
        case CropType::WITCHBERRY:         return "Witchberry";
        case CropType::PINK_TURNIP:        return "Pink Turnip";
        case CropType::JADE_SQUASH:        return "Jade Squash";
        case CropType::SNAKEROOT:          return "Snakeroot";
        case CropType::ENIGMA:             return "Enigma";
        case CropType::MAGEBULB:           return "Magebulb";
        case CropType::WINDSHEAR_CACTUS:   return "Windshear Cactus";
        case CropType::RAPTORLEAF:         return "Raptorleaf";
        case CropType::SONGBELL:           return "Songbell";
        case CropType::AUTUMN_BLOSSOM:     return "Autumn Blossom";
        case CropType::SPRING_BLOSSOM:     return "Spring Blossom";
        case CropType::WINTER_BLOSSOM:     return "Winter Blossom";
        default:                      return "Unknown";
    }
}

FarmCondition RollFarmCondition()
{
    uint32 roll = urand(1, 100);
    if (roll <= 55) return FarmCondition::HEALTHY;
    if (roll <= 68) return FarmCondition::PARCHED;
    if (roll <= 78) return FarmCondition::PESTS;
    if (roll <= 86) return FarmCondition::WEEDS;
    if (roll <= 90) return FarmCondition::ALLURING;
    if (roll <= 94) return FarmCondition::WIGGLING;
    if (roll <= 96) return FarmCondition::SMOTHERED;
    if (roll <= 98) return FarmCondition::WILD;
    if (roll <= 99) return FarmCondition::RUNTY;
    return FarmCondition::TANGLED;
}

 SpecialCrop RollSpecialCrop()
   {
       uint32 roll = urand(1, 100);
       if (roll <= 88) return SpecialCrop::NONE;
       if (roll <= 98) return SpecialCrop::PLUMP;
       return SpecialCrop::BURSTING;
   }

// ============================================================================
// Farm Data Manager (in-memory cache, thread-safe)
// ============================================================================

class FarmDataManager
{
public:
    static FarmDataManager* instance()
    {
        static FarmDataManager instance;
        return &instance;
    }

    // Load data from DB for a player
    // Returns true if the player already has a farm, false otherwise
    bool LoadFromDB(Player* player)
    {
        ObjectGuid guid = player->GetGUID();
        std::lock_guard<std::mutex> lock(_mutex);

        // Load farm_data
        QueryResult result = CharacterDatabase.PQuery(
            "SELECT unlocked_plots, has_irrigation, has_antipest, has_plow, votes_mask, expansion_timer_end "
            "FROM character_tillers_farm_data WHERE guid = %u",
            guid.GetCounter());

        if (!result)
            return false;

        _farmData[guid] = PlayerFarmCache();
        auto& data = _farmData[guid];

        Field* fields = result->Fetch();
        data.unlockedPlots          = fields[0].GetUInt8();
        data.upgrades.hasIrrigation = fields[1].GetBool();
        data.upgrades.hasAntipest   = fields[2].GetBool();
        data.upgrades.hasPlow       = fields[3].GetBool();
        data.votesMask              = fields[4].GetUInt8();
        data.expansionTimerEnd      = fields[5].GetUInt32();

        // Load plot data
        result = CharacterDatabase.PQuery(
            "SELECT plot_id, state, crop, planted_at, `condition`, is_special "
            "FROM character_tillers_farm WHERE guid = %u ORDER BY plot_id",
            guid.GetCounter());

        if (result)
        {
            do
            {
                Field* fields = result->Fetch();
                FarmPlotData info;
                info.plotId    = fields[0].GetUInt8();
                info.state     = FarmPlotState(fields[1].GetUInt8());
                info.crop      = CropType(fields[2].GetUInt8());
                info.plantedAt = fields[3].GetUInt32();
                info.condition = FarmCondition(fields[4].GetUInt8());
                info.special   = SpecialCrop(fields[5].GetUInt8());
                data.plots[info.plotId] = info;
            } while (result->NextRow());
        }

        // Ensure all unlocked plots exist
        for (uint8 i = 0; i < data.unlockedPlots; ++i)
        {
            if (data.plots.find(i) == data.plots.end())
            {
                FarmPlotData defaultPlot;
                defaultPlot.plotId = i;
                data.plots[i] = defaultPlot;

                CharacterDatabase.PExecute(
                    "INSERT INTO character_tillers_farm (guid, plot_id, state, crop, planted_at, `condition`, is_special) "
                    "VALUES (%u, %u, 0, 0, 0, 0, 0)",
                    guid.GetCounter(), i);
            }
        }

        return true;
    }

    // Create a new farm for a player (must have completed the tutorial gate quest)
    void CreateFarm(Player* player)
    {
        ObjectGuid guid = player->GetGUID();
        std::lock_guard<std::mutex> lock(_mutex);

        if (_farmData.find(guid) != _farmData.end())
            return;

        CharacterDatabase.PExecute(
            "INSERT INTO character_tillers_farm_data (guid, unlocked_plots, has_irrigation, has_antipest, has_plow, votes_mask) "
            "VALUES (%u, 4, 0, 0, 0, 0)",
            guid.GetCounter());

        for (uint8 i = 0; i < 4; ++i)
            CharacterDatabase.PExecute(
                "INSERT INTO character_tillers_farm (guid, plot_id, state, crop, planted_at, `condition`, is_special) "
                "VALUES (%u, %u, 0, 0, 0, 0, 0)",
                guid.GetCounter(), i);

        _farmData[guid] = PlayerFarmCache();
        auto& data = _farmData[guid];
        data.unlockedPlots = 4;
        for (uint8 i = 0; i < 4; ++i)
        {
            FarmPlotData defaultPlot;
            defaultPlot.plotId = i;
            data.plots[i] = defaultPlot;
        }
    }

    // Save a single plot's state to DB (caller must hold lock)
    void SavePlot_NoLock(ObjectGuid guid, FarmPlotData const& plot)
    {
        CharacterDatabase.PExecute(
            "UPDATE character_tillers_farm SET state = %u, crop = %u, planted_at = %u, `condition` = %u, is_special = %u "
            "WHERE guid = %u AND plot_id = %u",
            plot.state, plot.crop, plot.plantedAt, plot.condition, plot.special,
            guid.GetCounter(), plot.plotId);
    }

    // Save a single plot's state to DB (thread-safe public API)
    void SavePlot(ObjectGuid guid, FarmPlotData const& plot)
    {
        std::lock_guard<std::mutex> lock(_mutex);
        SavePlot_NoLock(guid, plot);
    }

    // Save all data for a player
    void SaveToDB(ObjectGuid guid)
    {
        std::lock_guard<std::mutex> lock(_mutex);
        auto it = _farmData.find(guid);
        if (it == _farmData.end())
            return;

        auto& data = it->second;
        CharacterDatabase.PExecute(
            "UPDATE character_tillers_farm_data SET unlocked_plots = %u, has_irrigation = %u, has_antipest = %u, has_plow = %u, votes_mask = %u, expansion_timer_end = %u "
            "WHERE guid = %u",
            data.unlockedPlots,
            data.upgrades.hasIrrigation ? 1 : 0,
            data.upgrades.hasAntipest ? 1 : 0,
            data.upgrades.hasPlow ? 1 : 0,
            data.votesMask,
            data.expansionTimerEnd,
            guid.GetCounter());

        for (auto& pair : data.plots)
            SavePlot_NoLock(guid, pair.second);
    }

    // Remove player from cache
    void Unload(ObjectGuid guid)
    {
        std::lock_guard<std::mutex> lock(_mutex);
        _farmData.erase(guid);
    }

    // Accessors
    PlayerFarmCache* GetData(ObjectGuid guid)
    {
        std::lock_guard<std::mutex> lock(_mutex);
        auto it = _farmData.find(guid);
        if (it != _farmData.end())
            return &it->second;
        return nullptr;
    }

    FarmPlotData* GetPlot(ObjectGuid guid, uint8 plotId)
    {
        std::lock_guard<std::mutex> lock(_mutex);
        auto it = _farmData.find(guid);
        if (it == _farmData.end())
            return nullptr;
        auto pit = it->second.plots.find(plotId);
        if (pit == it->second.plots.end())
            return nullptr;
        return &pit->second;
    }

private:
    std::map<ObjectGuid, PlayerFarmCache> _farmData;
    std::mutex _mutex;
};

#define sFarmData FarmDataManager::instance()

PlayerFarmCache* GetPlayerFarmData(ObjectGuid guid)
{
    return sFarmData->GetData(guid);
}

void CreatePlayerFarm(Player* player)
{
    sFarmData->CreateFarm(player);
}

void UpdateGrowthTimers(Player* player, std::map<uint8, FarmPlotData>& plots)
{
    uint32 now = uint32(sWorld->GetGameTime());
    for (auto& pair : plots)
    {
        auto& plot = pair.second;
        if (plot.state == FarmPlotState::GROWING && plot.plantedAt > 0)
        {
            uint32 elapsed = now - plot.plantedAt;
            if (elapsed >= FARM_GROWTH_TIME)
            {
                plot.condition = RollFarmCondition();
                plot.special = RollSpecialCrop();
                plot.state = FarmPlotState::RIPE;
                if (player)
                    sFarmData->SavePlot(player->GetGUID(), plot);
            }
        }
    }
}

// ============================================================================
// Farm Expansion Timer Check
// Called from PlayerScript zone/login updates and Farmer Yoon's gossip
// ============================================================================

bool CheckFarmExpiration(Player* player)
{
    if (!player)
        return false;

    PlayerFarmCache* data = sFarmData->GetData(player->GetGUID());
    if (!data)
        return false;

    if (data->expansionTimerEnd == 0)
        return false;

    uint32 now = uint32(sWorld->GetGameTime());
    if (now < data->expansionTimerEnd)
        return false;

    uint8 currentPlots = data->unlockedPlots;

    uint8 nextTier = 0;
    if (currentPlots < 8)
        nextTier = 8;
    else if (currentPlots < 12)
        nextTier = 12;
    else if (currentPlots < 16)
        nextTier = 16;

    if (nextTier == 0)
    {
        data->expansionTimerEnd = 0;
        CharacterDatabase.PExecute(
            "UPDATE character_tillers_farm_data SET expansion_timer_end = 0 WHERE guid = %u",
            player->GetGUID().GetCounter());
        return false;
    }

    data->unlockedPlots = nextTier;
    data->expansionTimerEnd = 0;
    CharacterDatabase.PExecute(
        "UPDATE character_tillers_farm_data SET unlocked_plots = %u, expansion_timer_end = 0 WHERE guid = %u",
        nextTier, player->GetGUID().GetCounter());

    player->GetSession()->SendNotification("Farmer Yoon finishes his work! You now have %u farm plots.", nextTier);

    return true;
}

// ============================================================================
// PlayerScript - Handle farm spawn/despawn on zone change
// ============================================================================

class player_tillers_farm : public PlayerScript
{
public:
    player_tillers_farm() : PlayerScript("player_tillers_farm") { }

    void OnLogin(Player* player) override
    {
        if (!player)
            return;

        bool hasFarm = sFarmData->LoadFromDB(player);
        if (!hasFarm && player->GetQuestRewardStatus(QUEST_TUTORIAL_GATE))
            sFarmData->CreateFarm(player);

        sFriendship->LoadFromDB(player);
        CheckFarmExpiration(player);
    }

    void OnLogout(Player* player) override
    {
        if (!player)
            return;

        DespawnFarm(player);
        sFarmData->SaveToDB(player->GetGUID());
        sFarmData->Unload(player->GetGUID());
        sFriendship->SaveToDB(player->GetGUID());
        sFriendship->Unload(player->GetGUID());
    }

    void OnUpdateZone(Player* player, uint32 newZone, uint32 newArea) override
    {
        if (!player)
            return;

        if (newZone == VALLEY_OF_FOUR_WINDS_ZONE)
        {
            CheckFarmExpiration(player);
            SpawnFarm(player);
        }
        else
            DespawnFarm(player);
    }

    void OnSave(Player* player) override
    {
        if (!player)
            return;

        sFarmData->SaveToDB(player->GetGUID());
    }

private:
    void SpawnFarm(Player* player)
    {
        if (!player)
            return;

        if (_spawnedPlots.find(player->GetGUID()) != _spawnedPlots.end())
            return;

        PlayerFarmCache* data = sFarmData->GetData(player->GetGUID());
        if (!data)
        {
            if (sFarmData->LoadFromDB(player))
                data = sFarmData->GetData(player->GetGUID());
            if (!data && player->GetQuestRewardStatus(QUEST_TUTORIAL_GATE))
            {
                sFarmData->CreateFarm(player);
                data = sFarmData->GetData(player->GetGUID());
            }
            if (!data)
                return;
        }

        // Process growth timers
        UpdateGrowthTimers(player, data->plots);

        // Auto-clear conditions based on upgrade flags
        if (data->upgrades.hasIrrigation || data->upgrades.hasAntipest)
        {
            for (auto& pair : data->plots)
            {
                auto& plot = pair.second;
                if (plot.state != FarmPlotState::GROWING)
                    continue;

                if (data->upgrades.hasIrrigation && plot.condition == FarmCondition::PARCHED)
                {
                    plot.condition = FarmCondition::HEALTHY;
                    sFarmData->SavePlot(player->GetGUID(), plot);
                }
                else if (data->upgrades.hasAntipest && plot.condition == FarmCondition::PESTS)
                {
                    plot.condition = FarmCondition::HEALTHY;
                    sFarmData->SavePlot(player->GetGUID(), plot);
                }
            }
        }

        // Spawn creatures
        std::vector<ObjectGuid> spawned;
        for (uint8 i = 0; i < data->unlockedPlots && i < MAX_FARM_PLOTS; ++i)
        {
            auto& plot = data->plots[i];
            uint32 displayId = GetFarmPlotDisplayId(plot.state, plot.crop, plot.condition, plot.special);

            if (TempSummon* summon = player->SummonCreature(NPC_FARM_PLOT, FarmPlotPositions[i], TEMPSUMMON_MANUAL_DESPAWN))
            {
                Creature* creature = summon;
                creature->SetDisplayId(displayId);
                creature->ToTempSummon()->SetPrivateObjectOwner(player->GetGUID());
                creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);
                creature->AI()->SetData(0, i);

                plot.creatureGuid = creature->GetGUID();
                spawned.push_back(creature->GetGUID());
            }
        }

        _spawnedPlots[player->GetGUID()] = spawned;
    }

    void DespawnFarm(Player* player)
    {
        if (!player)
            return;

        auto it = _spawnedPlots.find(player->GetGUID());
        if (it == _spawnedPlots.end())
            return;

        for (auto& guid : it->second)
        {
            if (Creature* creature = ObjectAccessor::GetCreature(*player, guid))
                creature->DespawnOrUnsummon();
        }

        _spawnedPlots.erase(it);
    }

    std::map<ObjectGuid, std::vector<ObjectGuid>> _spawnedPlots;
};

// ============================================================================
// Farm Plot Creature Script
// ============================================================================

class npc_farm_plot : public CreatureScript
{
public:
    npc_farm_plot() : CreatureScript("npc_farm_plot") { }

    enum InteractionType : uint8
    {
        INTERACTION_NONE = 0,
        INTERACTION_CHANNEL,
        INTERACTION_SUMMON,
        INTERACTION_TIMER,
        INTERACTION_MOVE_AWAY
    };

    struct npc_farm_plotAI : public ScriptedAI
    {
        npc_farm_plotAI(Creature* creature) : ScriptedAI(creature), plotId(0), _interactionType(INTERACTION_NONE), _interactionTimer(0), _interactionAction(0) { }

        uint8 plotId;
        InteractionType _interactionType;
        uint32 _interactionTimer;
        ObjectGuid _interactionPlayerGuid;
        uint32 _interactionAction;
        ObjectGuid _summonedNpcGuid;

        void SetData(uint32 /*type*/, uint32 data) override
        {
            plotId = uint8(data);
        }

        void Reset() override
        {
            plotId = 0;
            _interactionType = INTERACTION_NONE;
            _interactionTimer = 0;
            _interactionPlayerGuid.Clear();
            _interactionAction = 0;
            _summonedNpcGuid.Clear();
        }

        bool StartChannel(Player* player, uint32 actionId, uint32 durationMs)
        {
            _interactionType = INTERACTION_CHANNEL;
            _interactionTimer = durationMs;
            _interactionPlayerGuid = player->GetGUID();
            _interactionAction = actionId;
            return true;
        }

        bool StartSummon(Player* player, uint32 actionId, uint32 npcEntry)
        {
            if (TempSummon* summon = me->SummonCreature(npcEntry, me->GetPosition(), TEMPSUMMON_CORPSE_TIMED_DESPAWN, 30000))
            {
                _interactionType = INTERACTION_SUMMON;
                _interactionAction = actionId;
                _interactionPlayerGuid = player->GetGUID();
                _summonedNpcGuid = summon->GetGUID();
                summon->SetPrivateObjectOwner(player->GetGUID());
                Creature* npc = summon;
                npc->SetReactState(REACT_AGGRESSIVE);
                npc->AI()->AttackStart(player);
                return true;
            }
            return false;
        }

        bool StartTimer(Player* player, uint32 actionId, uint32 durationMs)
        {
            _interactionType = INTERACTION_TIMER;
            _interactionTimer = durationMs;
            _interactionPlayerGuid = player->GetGUID();
            _interactionAction = actionId;
            return true;
        }

        bool StartMoveAway(Player* player, uint32 actionId)
        {
            _interactionType = INTERACTION_MOVE_AWAY;
            _interactionPlayerGuid = player->GetGUID();
            _interactionAction = actionId;
            return true;
        }

        void FinishInteraction(Player* player, PlayerFarmCache* data)
        {
            _interactionType = INTERACTION_NONE;
            _interactionTimer = 0;
            _summonedNpcGuid.Clear();

            if (!player || !data)
                return;

            auto it = data->plots.find(plotId);
            if (it == data->plots.end())
                return;

            FarmPlotData& plot = it->second;

            switch (_interactionAction)
            {
                case 5:
                    if (plot.condition == FarmCondition::ALLURING)
                    {
                        plot.condition = FarmCondition::HEALTHY;
                        player->GetSession()->SendNotification("You shoo the birds away.");
                    }
                    break;
                case 6:
                    if (plot.condition == FarmCondition::WIGGLING)
                    {
                        plot.condition = FarmCondition::HEALTHY;
                        player->GetSession()->SendNotification("You dig out the wiggling virmen.");
                    }
                    break;
                case 7:
                    if (plot.condition == FarmCondition::SMOTHERED)
                    {
                        plot.condition = FarmCondition::HEALTHY;
                        player->GetSession()->SendNotification("You clear the smothering weeds.");
                    }
                    break;
                case 8:
                    if (plot.condition == FarmCondition::WILD)
                    {
                        plot.condition = FarmCondition::HEALTHY;
                        player->GetSession()->SendNotification("You wrestle the wild crop back in place.");
                    }
                    break;
                case 9:
                    if (plot.condition == FarmCondition::RUNTY)
                    {
                        plot.condition = FarmCondition::HEALTHY;
                        player->GetSession()->SendNotification("You pull up the runty crop.");
                    }
                    break;
                case 10:
                    if (plot.condition == FarmCondition::TANGLED)
                    {
                        plot.condition = FarmCondition::HEALTHY;
                        player->GetSession()->SendNotification("You untangle the vines from the crop.");
                    }
                    break;
                case 11:
                    if (plot.state == FarmPlotState::STUBBORN)
                    {
                        plot.state = FarmPlotState::UNTILLED;
                        player->GetSession()->SendNotification("You pull the stubborn soil loose.");
                    }
                    break;
                case 12:
                    if (plot.state == FarmPlotState::OCCUPIED)
                    {
                        plot.state = FarmPlotState::UNTILLED;
                        player->GetSession()->SendNotification("You clear the occupied plot.");
                    }
                    break;
            }

            sFarmData->SavePlot(player->GetGUID(), plot);
            me->SetDisplayId(GetFarmPlotDisplayId(plot.state, plot.crop, plot.condition, plot.special));
        }

        void UpdateAI(uint32 diff) override
        {
            if (_interactionType == INTERACTION_NONE)
                return;

            Player* player = ObjectAccessor::GetPlayer(*me, _interactionPlayerGuid);
            if (!player)
            {
                Reset();
                return;
            }

            PlayerFarmCache* data = sFarmData->GetData(player->GetGUID());
            if (!data)
            {
                Reset();
                return;
            }

            switch (_interactionType)
            {
                case INTERACTION_CHANNEL:
                case INTERACTION_TIMER:
                    if (_interactionTimer <= diff)
                        FinishInteraction(player, data);
                    else
                        _interactionTimer -= diff;
                    break;
                case INTERACTION_SUMMON:
                {
                    Creature* summon = ObjectAccessor::GetCreature(*me, _summonedNpcGuid);
                    if (!summon || !summon->IsAlive())
                        FinishInteraction(player, data);
                    break;
                }
                case INTERACTION_MOVE_AWAY:
                    if (me->GetDistance2d(player) >= 15.0f)
                        FinishInteraction(player, data);
                    break;
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_farm_plotAI(creature);
    }

    bool OnGossipHello(Player* player, Creature* creature) override
    {
        PlayerFarmCache* data = sFarmData->GetData(player->GetGUID());
        if (!data)
        {
            player->GetSession()->SendNotification("Your farm data is not loaded.");
            return true;
        }

        npc_farm_plotAI* ai = static_cast<npc_farm_plotAI*>(creature->AI());
        if (!ai)
            return true;

        uint8 plotId = ai->plotId;
        auto it = data->plots.find(plotId);
        if (it == data->plots.end())
        {
            player->GetSession()->SendNotification("Farm plot not found.");
            return true;
        }

        FarmPlotData& plot = it->second;

        if (creature->GetPrivateObjectOwner() != player->GetGUID())
        {
            player->GetSession()->SendNotification("This is not your farm plot.");
            return true;
        }

        if (ai->_interactionType != INTERACTION_NONE)
        {
            player->GetSession()->SendNotification("You are already occupied with a farm task.");
            return true;
        }

        uint32 now = uint32(sWorld->GetGameTime());

        // Update growth for this plot before showing gossip
        UpdateGrowthTimers(player, data->plots);
        creature->SetDisplayId(GetFarmPlotDisplayId(plot.state, plot.crop, plot.condition, plot.special));

        uint32 gossipCount = 0;

        switch (plot.state)
        {
            case FarmPlotState::UNTILLED:
            {
                AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "Till the soil.", GOSSIP_SENDER_MAIN, 0);
                gossipCount++;
                break;
            }
            case FarmPlotState::TILLED:
            {
                // Profession seeds require Revered reputation
                FactionEntry const* tillersFaction = sFactionStore.LookupEntry(FACTION_TILLERS);
                uint32 tillersRep = tillersFaction ? player->GetReputationMgr().GetReputation(tillersFaction) : 0;

                bool hasAnySeed = false;
                for (auto& seedPair : CropSeedMap)
                {
                    // Gate profession seeds (type >= SNAKEROOT) by Revered reputation
                    if (uint32(seedPair.first) >= uint32(CropType::SNAKEROOT) && tillersRep < 21000)
                        continue;

                    if (player->HasItemCount(seedPair.second, 1))
                    {
                        std::string msg = std::string("Plant ") + GetCropName(seedPair.first) + " seed.";
                        AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, msg, GOSSIP_SENDER_MAIN, 100 + uint32(seedPair.first));
                        hasAnySeed = true;
                        gossipCount++;
                    }
                }

                if (!hasAnySeed)
                {
                    AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "No seeds available. Buy seeds from Merchant Greenfield.", GOSSIP_SENDER_MAIN, 0);
                    gossipCount++;
                }
                break;
            }
            case FarmPlotState::GROWING:
            {
                uint32 elapsed = now - plot.plantedAt;
                uint32 remaining = (elapsed >= FARM_GROWTH_TIME) ? 0 : (FARM_GROWTH_TIME - elapsed);

                uint32 hours = remaining / 3600;
                uint32 mins = (remaining % 3600) / 60;
                std::string status = "Crop is growing. Time remaining: " + std::to_string(hours) + " hours, " + std::to_string(mins) + " minutes.";
                AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, status, GOSSIP_SENDER_MAIN, 0);

                if (plot.condition == FarmCondition::PARCHED)
                {
                    if (player->HasItemCount(ITEM_RUSTY_WATERING_CAN, 1))
                        AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "Water the crop (Rusty Watering Can).", GOSSIP_SENDER_MAIN, 2);
                }
                else if (plot.condition == FarmCondition::PESTS)
                {
                    if (player->HasItemCount(ITEM_VINTAGE_BUG_SPRAYER, 1))
                        AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "Spray the pests (Vintage Bug Sprayer).", GOSSIP_SENDER_MAIN, 3);
                }
                else if (plot.condition == FarmCondition::WEEDS)
                {
                    AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "Pull the weeds.", GOSSIP_SENDER_MAIN, 4);
                }
                else if (plot.condition == FarmCondition::ALLURING)
                {
                    AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "Shoo the birds away!", GOSSIP_SENDER_MAIN, 5);
                }
                else if (plot.condition == FarmCondition::WIGGLING)
                {
                    AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "Dig out the wiggling virmen!", GOSSIP_SENDER_MAIN, 6);
                }
                else if (plot.condition == FarmCondition::SMOTHERED)
                {
                    AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "Clear the smothering weeds.", GOSSIP_SENDER_MAIN, 7);
                }
                else if (plot.condition == FarmCondition::WILD)
                {
                    AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "Wrestle the wild crop back in place.", GOSSIP_SENDER_MAIN, 8);
                }
                else if (plot.condition == FarmCondition::RUNTY)
                {
                    AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "Pull up the runty crop.", GOSSIP_SENDER_MAIN, 9);
                }
                else if (plot.condition == FarmCondition::TANGLED)
                {
                    AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "Untangle the vines from the crop.", GOSSIP_SENDER_MAIN, 10);
                }

                if (player->HasItemCount(ITEM_DENTED_SHOVEL, 1))
                    AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "Remove crop with Dented Shovel.", GOSSIP_SENDER_MAIN, 13);

                gossipCount++;
                break;
            }
            case FarmPlotState::RIPE:
            {
                std::string harvestMsg = std::string("Harvest ") + GetCropName(plot.crop) + "!";

                if (plot.special == SpecialCrop::PLUMP)
                    AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "Harvest Plump crop (bonus yield)!", GOSSIP_SENDER_MAIN, 1);
                else if (plot.special == SpecialCrop::BURSTING)
                    AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "Harvest Bursting crop (instant harvest)!", GOSSIP_SENDER_MAIN, 1);
                else
                    AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, harvestMsg, GOSSIP_SENDER_MAIN, 1);

                gossipCount++;
                break;
            }
            case FarmPlotState::STUBBORN:
            {
                AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "Pull the stubborn soil loose.", GOSSIP_SENDER_MAIN, 11);
                gossipCount++;
                break;
            }
            case FarmPlotState::OCCUPIED:
            {
                AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "Expose the virmen occupying this plot!", GOSSIP_SENDER_MAIN, 12);
                gossipCount++;
                break;
            }
        }

        if (gossipCount == 0)
            AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "Nothing to do here.", GOSSIP_SENDER_MAIN, 0);

        SendGossipMenuFor(player, DEFAULT_GOSSIP_MESSAGE, creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action) override
    {
        CloseGossipMenuFor(player);

        PlayerFarmCache* data = sFarmData->GetData(player->GetGUID());
        if (!data)
            return true;

        npc_farm_plotAI* ai = static_cast<npc_farm_plotAI*>(creature->AI());
        if (!ai)
            return true;

        uint8 plotId = ai->plotId;
        auto it = data->plots.find(plotId);
        if (it == data->plots.end())
            return true;

        FarmPlotData& plot = it->second;
        uint32 now = uint32(sWorld->GetGameTime());

        switch (action)
        {
            case 0: // Till soil
            {
                if (plot.state == FarmPlotState::UNTILLED)
                {
                    plot.state = FarmPlotState::TILLED;
                    sFarmData->SavePlot(player->GetGUID(), plot);
                    creature->SetDisplayId(GetFarmPlotDisplayId(plot.state, plot.crop, plot.condition, plot.special));
                    player->GetSession()->SendNotification("You till the soil.");
                }
                break;
            }
            case 1: // Harvest
            {
                if (plot.state == FarmPlotState::RIPE && plot.crop != CropType::NONE)
                {
                    uint32 yield = 5;
                    if (plot.special == SpecialCrop::PLUMP)
                        yield = 8;
                    else if (plot.special == SpecialCrop::BURSTING)
                        yield = 3;

                    uint32 day = (now / 86400) % 10;
                    if (BonusCropSchedule[day] == plot.crop)
                        yield += 2;

                    char const* cropName = GetCropName(plot.crop);

                    if (plot.crop == CropType::ENIGMA)
                    {
                        uint32 herbId = EnigmaHerbs[urand(0, 4)];
                        player->AddItem(herbId, yield);
                    }
                    else
                    {
                        auto harvestIt = CropHarvestMap.find(plot.crop);
                        if (harvestIt != CropHarvestMap.end())
                            player->AddItem(harvestIt->second, yield);
                    }

            if (urand(1, 100) <= 50)
                    {
                        auto seedIt = CropSeedMap.find(plot.crop);
                        if (seedIt != CropSeedMap.end())
                        {
                            uint32 seedCount = urand(1, 3);
                            player->AddItem(seedIt->second, seedCount);
                        }
                    }

                    // Tiller gift rare drop
                    if (urand(1, 100) <= CHANCE_TILLER_GIFT)
                        player->AddItem(ITEM_TILLER_GIFT, 1);

                    // Post-harvest soil state
                    uint32 soilRoll = urand(1, 100);
                    if (soilRoll <= CHANCE_OCCUPIED_SOIL)
                        plot.state = FarmPlotState::OCCUPIED;
                    else if (soilRoll <= CHANCE_OCCUPIED_SOIL + CHANCE_STUBBORN_SOIL)
                        plot.state = FarmPlotState::STUBBORN;
                    else
                        plot.state = FarmPlotState::UNTILLED;

                    plot.crop = CropType::NONE;
                    plot.plantedAt = 0;
                    plot.condition = FarmCondition::HEALTHY;
                    plot.special = SpecialCrop::NONE;
                    sFarmData->SavePlot(player->GetGUID(), plot);
                    creature->SetDisplayId(GetFarmPlotDisplayId(plot.state, plot.crop, plot.condition, plot.special));

                    player->GetReputationMgr().ModifyReputation(sFactionStore.LookupEntry(FACTION_TILLERS), HARVEST_REP_GAIN);
                    player->GetSession()->SendNotification(("You harvest " + std::to_string(yield) + " " + cropName + "!").c_str());
                }
                break;
            }
            case 2: // Water (fix parched)
            {
                if (plot.condition == FarmCondition::PARCHED && player->HasItemCount(ITEM_RUSTY_WATERING_CAN, 1))
                {
                    plot.condition = FarmCondition::HEALTHY;
                    sFarmData->SavePlot(player->GetGUID(), plot);
                    creature->SetDisplayId(GetFarmPlotDisplayId(plot.state, plot.crop, plot.condition, plot.special));
                    player->GetSession()->SendNotification("You water the crop.");
                }
                break;
            }
            case 3: // Spray (fix pests)
            {
                if (plot.condition == FarmCondition::PESTS && player->HasItemCount(ITEM_VINTAGE_BUG_SPRAYER, 1))
                {
                    plot.condition = FarmCondition::HEALTHY;
                    sFarmData->SavePlot(player->GetGUID(), plot);
                    creature->SetDisplayId(GetFarmPlotDisplayId(plot.state, plot.crop, plot.condition, plot.special));
                    player->GetSession()->SendNotification("You spray the pests.");
                }
                break;
            }
            case 4: // Pull weeds
            {
                if (plot.condition == FarmCondition::WEEDS)
                {
                    plot.condition = FarmCondition::HEALTHY;
                    sFarmData->SavePlot(player->GetGUID(), plot);
                    creature->SetDisplayId(GetFarmPlotDisplayId(plot.state, plot.crop, plot.condition, plot.special));
                    player->GetSession()->SendNotification("You pull the weeds.");
                }
                break;
            }
            case 5: // Shoo birds (alluring) → summon Plainshawk
            {
                if (plot.condition == FarmCondition::ALLURING)
                {
                    if (ai && ai->StartSummon(player, 5, NPC_PLAINSHAWK))
                        player->GetSession()->SendNotification("You disturb the birds from the crop...");
                    else
                    {
                        plot.condition = FarmCondition::HEALTHY;
                        sFarmData->SavePlot(player->GetGUID(), plot);
                        creature->SetDisplayId(GetFarmPlotDisplayId(plot.state, plot.crop, plot.condition, plot.special));
                        player->GetSession()->SendNotification("You shoo the birds away.");
                    }
                }
                break;
            }
            case 6: // Dig out virmen (wiggling) → summon Virmen
            {
                if (plot.condition == FarmCondition::WIGGLING)
                {
                    if (ai && ai->StartSummon(player, 6, NPC_VIRMEN))
                        player->GetSession()->SendNotification("You dig at the wiggling mound...");
                    else
                    {
                        plot.condition = FarmCondition::HEALTHY;
                        sFarmData->SavePlot(player->GetGUID(), plot);
                        creature->SetDisplayId(GetFarmPlotDisplayId(plot.state, plot.crop, plot.condition, plot.special));
                        player->GetSession()->SendNotification("You dig out the wiggling virmen.");
                    }
                }
                break;
            }
            case 7: // Clear smothered weeds → channel 3s
            {
                if (plot.condition == FarmCondition::SMOTHERED)
                {
                    if (ai)
                        ai->StartChannel(player, 7, 3000);
                    player->GetSession()->SendNotification("You start clearing the smothering weeds...");
                }
                break;
            }
            case 8: // Wrestle wild crop → channel 4s
            {
                if (plot.condition == FarmCondition::WILD)
                {
                    if (ai)
                        ai->StartChannel(player, 8, 4000);
                    player->GetSession()->SendNotification("You start wrestling the wild crop...");
                }
                break;
            }
            case 9: // Pull up runty crop → timer 1s
            {
                if (plot.condition == FarmCondition::RUNTY)
                {
                    if (ai)
                        ai->StartTimer(player, 9, 1000);
                    player->GetSession()->SendNotification("You start pulling the runty crop...");
                }
                break;
            }
            case 10: // Untangle vines → move 15y away
            {
                if (plot.condition == FarmCondition::TANGLED)
                {
                    if (ai)
                        ai->StartMoveAway(player, 10);
                    player->GetSession()->SendNotification("Move away from the plot to untangle the vines!");
                }
                break;
            }
            case 11: // Pull stubborn soil → channel 3s
            {
                if (plot.state == FarmPlotState::STUBBORN)
                {
                    if (ai)
                        ai->StartChannel(player, 11, 3000);
                    player->GetSession()->SendNotification("You start pulling the stubborn soil...");
                }
                break;
            }
            case 12: // Expose virmen (occupied) → summon Virmen
            {
                if (plot.state == FarmPlotState::OCCUPIED)
                {
                    if (ai && ai->StartSummon(player, 12, NPC_VIRMEN))
                        player->GetSession()->SendNotification("You disturb the virmen in the soil...");
                    else
                    {
                        plot.state = FarmPlotState::UNTILLED;
                        sFarmData->SavePlot(player->GetGUID(), plot);
                        creature->SetDisplayId(GetFarmPlotDisplayId(plot.state, plot.crop, plot.condition, plot.special));
                        player->GetSession()->SendNotification("You expose the virmen occupying this plot.");
                    }
                }
                break;
            }
            case 13: // Remove crop with Dented Shovel
            {
                if (plot.state == FarmPlotState::GROWING && player->HasItemCount(ITEM_DENTED_SHOVEL, 1))
                {
                    plot.state = FarmPlotState::UNTILLED;
                    plot.crop = CropType::NONE;
                    plot.plantedAt = 0;
                    plot.condition = FarmCondition::HEALTHY;
                    plot.special = SpecialCrop::NONE;
                    sFarmData->SavePlot(player->GetGUID(), plot);
                    creature->SetDisplayId(GetFarmPlotDisplayId(plot.state, plot.crop, plot.condition, plot.special));
                    player->GetSession()->SendNotification("You remove the crop with your Dented Shovel.");
                }
                break;
            }
            default: // Plant seed (action = 100 + CropType)
            {
                if (action >= 100)
                {
                    uint8 cropType = uint8(action - 100);
                    if (cropType > 0 && cropType < uint8(CropType::MAX) && plot.state == FarmPlotState::TILLED)
                    {
                        CropType crop = CropType(cropType);
                        auto seedIt = CropSeedMap.find(crop);
                        if (seedIt != CropSeedMap.end() && player->HasItemCount(seedIt->second, 1))
                        {
                            player->DestroyItemCount(seedIt->second, 1, true);
                            plot.crop = crop;
                            plot.state = FarmPlotState::GROWING;
                            plot.plantedAt = now;
                            plot.condition = FarmCondition::HEALTHY;
                            plot.special = SpecialCrop::NONE;
                            sFarmData->SavePlot(player->GetGUID(), plot);
                            creature->SetDisplayId(GetFarmPlotDisplayId(plot.state, plot.crop, plot.condition, plot.special));
                            player->GetSession()->SendNotification(("You plant " + std::string(GetCropName(crop)) + ".").c_str());
                        }
                    }
                }
                break;
            }
        }

        return true;
    }
};

// ============================================================================
// SpellScripts for Farming Tools
// ============================================================================

class spell_tillers_water : public SpellScriptLoader
{
public:
    spell_tillers_water() : SpellScriptLoader("spell_tillers_water") { }

    class spell_tillers_water_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_tillers_water_SpellScript);

        void HandleEffect(SpellEffIndex /*effIndex*/)
        {
            if (Unit* target = GetHitUnit())
            {
                if (target->HasAura(115824))
                    target->RemoveAura(115824);
            }
        }

        void Register() override
        {
            OnEffectHitTarget += SpellEffectFn(spell_tillers_water_SpellScript::HandleEffect, EFFECT_0, SPELL_EFFECT_DUMMY);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_tillers_water_SpellScript();
    }
};

// ============================================================================
// Registration
// ============================================================================

// ============================================================================
// FriendshipManager Implementation
// ============================================================================

bool FriendshipManager::LoadFromDB(Player* player)
{
    ObjectGuid guid = player->GetGUID();
    std::lock_guard<std::mutex> lock(_mutex);

    QueryResult result = CharacterDatabase.PQuery(
        "SELECT npc_entry, standing, daily_food_time, daily_gift_time "
        "FROM character_tillers_friendship WHERE guid = %u",
        guid.GetCounter());

    if (result)
    {
        do
        {
            Field* fields = result->Fetch();
            uint32 npcEntry = fields[0].GetUInt32();
            _friendships[guid][npcEntry] = FriendshipEntry();
            _friendships[guid][npcEntry].standing = fields[1].GetInt32();
            _friendships[guid][npcEntry].lastDailyFoodTime = fields[2].GetUInt32();
            _friendships[guid][npcEntry].lastDailyGiftTime = fields[3].GetUInt32();
        } while (result->NextRow());
    }

    return true;
}

void FriendshipManager::SaveToDB(ObjectGuid guid)
{
    std::lock_guard<std::mutex> lock(_mutex);
    auto it = _friendships.find(guid);
    if (it == _friendships.end())
        return;

    for (auto& pair : it->second)
    {
        CharacterDatabase.PExecute(
            "INSERT INTO character_tillers_friendship (guid, npc_entry, standing, daily_food_time, daily_gift_time) "
            "VALUES (%u, %u, %d, %u, %u) "
            "ON DUPLICATE KEY UPDATE standing = %d, daily_food_time = %u, daily_gift_time = %u",
            guid.GetCounter(), pair.first,
            pair.second.standing, pair.second.lastDailyFoodTime, pair.second.lastDailyGiftTime,
            pair.second.standing, pair.second.lastDailyFoodTime, pair.second.lastDailyGiftTime);
    }
}

void FriendshipManager::Unload(ObjectGuid guid)
{
    std::lock_guard<std::mutex> lock(_mutex);
    _friendships.erase(guid);
}

FriendshipEntry* FriendshipManager::GetFriendship(ObjectGuid playerGuid, uint32 npcEntry)
{
    std::lock_guard<std::mutex> lock(_mutex);
    auto pit = _friendships.find(playerGuid);
    if (pit == _friendships.end())
        return nullptr;
    auto nit = pit->second.find(npcEntry);
    if (nit == pit->second.end())
        return nullptr;
    return &nit->second;
}

void FriendshipManager::ModifyStanding(ObjectGuid playerGuid, uint32 npcEntry, int32 amount)
{
    std::lock_guard<std::mutex> lock(_mutex);
    auto& entry = _friendships[playerGuid][npcEntry];
    entry.standing += amount;
    if (entry.standing < 0)
        entry.standing = 0;
}

bool FriendshipManager::UpdateDailyFoodTimer(ObjectGuid playerGuid, uint32 npcEntry)
{
    std::lock_guard<std::mutex> lock(_mutex);
    auto& entry = _friendships[playerGuid][npcEntry];
    uint32 now = uint32(sWorld->GetGameTime());
    if (now - entry.lastDailyFoodTime < FRIENDSHIP_DAILY_RESET_INTERVAL)
        return false;
    entry.lastDailyFoodTime = now;
    return true;
}

bool FriendshipManager::UpdateDailyGiftTimer(ObjectGuid playerGuid, uint32 npcEntry)
{
    std::lock_guard<std::mutex> lock(_mutex);
    auto& entry = _friendships[playerGuid][npcEntry];
    uint32 now = uint32(sWorld->GetGameTime());
    if (now - entry.lastDailyGiftTime < FRIENDSHIP_DAILY_RESET_INTERVAL)
        return false;
    entry.lastDailyGiftTime = now;
    return true;
}

// ============================================================================
// Dark Soil GameObject Script
// ============================================================================

class go_dark_soil : public GameObjectScript
{
public:
    go_dark_soil() : GameObjectScript("go_dark_soil") { }

    bool OnGossipHello(Player* player, GameObject* go) override
    {
        AddGossipItemFor(player, GOSSIP_ICON_MONEY_BAG, "Dig in the dark soil...", GOSSIP_SENDER_MAIN, 1);
        AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Leave.", GOSSIP_SENDER_MAIN, 0);
        SendGossipMenuFor(player, DEFAULT_GOSSIP_MESSAGE, go->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, GameObject* go, uint32 /*sender*/, uint32 action) override
    {
        CloseGossipMenuFor(player);

        if (action == 0)
            return true;

        if (action == 1)
        {
            uint32 treasureIdx = urand(0, 4);
            uint32 itemId = DarkSoilTreasures[treasureIdx];

            if (player->AddItem(itemId, 1))
                player->GetSession()->SendNotification("You dig in the dark soil and find a %s!", DarkSoilTreasureNames[treasureIdx]);
            else
                player->GetSession()->SendNotification("Your bags are full! You cannot carry the %s.", DarkSoilTreasureNames[treasureIdx]);

            go->Delete();
            return true;
        }

        return OnGossipHello(player, go);
    }
};

void AddSC_tillers_farm()
{
    new player_tillers_farm();
    new npc_farm_plot();
    new spell_tillers_water();
    new go_dark_soil();
}
