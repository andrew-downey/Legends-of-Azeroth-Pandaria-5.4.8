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
constexpr uint32 ITEM_VINTAGE_BUG_SPRAYER = 80212;
constexpr uint32 ITEM_DENTED_SHOVEL = 80210;
constexpr uint32 ITEM_EARTH_SLASHER = 89304;
constexpr uint32 ITEM_JINYU_PRINCESS_SPRINKLER = 89230;

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

// Tillers faction
constexpr int32 FACTION_TILLERS = 1074;

// Harvest reputation gain (at level 90)
constexpr int32 HARVEST_REP_GAIN = 50;

// Tiller gift item (rare drop from harvest)
constexpr uint32 ITEM_TILLER_GIFT = 79246;

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

#endif
