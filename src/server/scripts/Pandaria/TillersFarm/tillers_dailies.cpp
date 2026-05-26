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
#include "ScriptedGossip.h"
#include "ScriptedCreature.h"
#include "Player.h"
#include "Creature.h"
#include "DatabaseEnv.h"
#include "ReputationMgr.h"
#include "DBCStores.h"
#include "World.h"
#include "ObjectAccessor.h"
#include "QuestDef.h"

#include "tillers_farm.h"

// ============================================================================
// DailyQuestManager Implementation
// ============================================================================

bool DailyQuestManager::LoadFromDB(Player* player)
{
    ObjectGuid guid = player->GetGUID();
    std::lock_guard<std::mutex> lock(_mutex);

    _dailyData[guid] = DailyQuestData();
    auto& data = _dailyData[guid];

    QueryResult result = CharacterDatabase.PQuery(
        "SELECT daily_quests_mask, last_daily_reset, active_crop_daily, active_kill_daily, andi_gift_target "
        "FROM character_tillers_daily_quests WHERE guid = %u",
        guid.GetCounter());

    if (result)
    {
        Field* fields = result->Fetch();
        data.dailyQuestsMask = fields[0].GetUInt32();
        data.lastDailyReset = fields[1].GetUInt32();
        data.activeCropDaily = fields[2].GetUInt32();
        data.activeKillDaily = fields[3].GetUInt32();
        data.AndiGiftTarget = fields[4].GetUInt8();
    }
    else
    {
        CharacterDatabase.PExecute(
            "INSERT INTO character_tillers_daily_quests (guid, daily_quests_mask, last_daily_reset, active_crop_daily, active_kill_daily, andi_gift_target) "
            "VALUES (%u, 0, 0, 0, 0, 0)",
            guid.GetCounter());
    }

    return true;
}

void DailyQuestManager::SaveToDB(ObjectGuid guid)
{
    std::lock_guard<std::mutex> lock(_mutex);
    auto it = _dailyData.find(guid);
    if (it == _dailyData.end())
        return;

    auto& data = it->second;
    CharacterDatabase.PExecute(
        "UPDATE character_tillers_daily_quests SET daily_quests_mask = %u, last_daily_reset = %u, "
        "active_crop_daily = %u, active_kill_daily = %u, andi_gift_target = %u "
        "WHERE guid = %u",
        data.dailyQuestsMask, data.lastDailyReset,
        data.activeCropDaily, data.activeKillDaily, data.AndiGiftTarget,
        guid.GetCounter());
}

void DailyQuestManager::Unload(ObjectGuid guid)
{
    std::lock_guard<std::mutex> lock(_mutex);
    _dailyData.erase(guid);
}

bool DailyQuestManager::IsDailyComplete(ObjectGuid playerGuid, uint32 questId)
{
    std::lock_guard<std::mutex> lock(_mutex);
    auto it = _dailyData.find(playerGuid);
    if (it == _dailyData.end())
        return false;

    auto& data = it->second;

    if (questId >= QUEST_CROP_BASE && questId < QUEST_CROP_BASE + CROP_DAILY_COUNT)
        return data.activeCropDaily == questId && (data.dailyQuestsMask & (1 << (questId - QUEST_CROP_BASE)));

    if (questId >= QUEST_KILL_BASE && questId < QUEST_KILL_BASE + KILL_DAILY_COUNT)
        return data.activeKillDaily == questId && (data.dailyQuestsMask & (1 << (questId - QUEST_KILL_BASE)));

    if (questId == QUEST_ANDI_GIFT)
        return data.AndiGiftTarget > 0 && (data.dailyQuestsMask & (1 << 10));

    return false;
}

void DailyQuestManager::CompleteDaily(ObjectGuid playerGuid, uint32 questId)
{
    std::lock_guard<std::mutex> lock(_mutex);
    auto it = _dailyData.find(playerGuid);
    if (it == _dailyData.end())
        return;

    auto& data = it->second;

    if (questId >= QUEST_CROP_BASE && questId < QUEST_CROP_BASE + CROP_DAILY_COUNT)
    {
        if (data.activeCropDaily == questId)
            data.dailyQuestsMask |= (1 << (questId - QUEST_CROP_BASE));
    }
    else if (questId >= QUEST_KILL_BASE && questId < QUEST_KILL_BASE + KILL_DAILY_COUNT)
    {
        if (data.activeKillDaily == questId)
            data.dailyQuestsMask |= (1 << (questId - QUEST_KILL_BASE));
    }
    else if (questId == QUEST_ANDI_GIFT)
    {
        if (data.AndiGiftTarget > 0)
            data.dailyQuestsMask |= (1 << 10);
    }
}

void DailyQuestManager::ResetDailies(ObjectGuid playerGuid)
{
    std::lock_guard<std::mutex> lock(_mutex);
    auto it = _dailyData.find(playerGuid);
    if (it == _dailyData.end())
        return;

    auto& data = it->second;
    data.dailyQuestsMask = 0;
    data.lastDailyReset = uint32(sWorld->GetGameTime());
}

uint32 DailyQuestManager::GetRandomCropDaily(ObjectGuid playerGuid)
{
    std::lock_guard<std::mutex> lock(_mutex);
    auto it = _dailyData.find(playerGuid);
    if (it == _dailyData.end())
        return 0;

    auto& data = it->second;
    if (data.activeCropDaily == 0)
    {
        uint32 pool[10] =
        {
            QUEST_CROP_PUMPKIN, QUEST_CROP_MELON, QUEST_CROP_LEEK, QUEST_CROP_SQUASH,
            QUEST_CROP_CARROT, QUEST_CROP_CABBAGE, QUEST_CROP_PINK_TURNIP,
            QUEST_CROP_WHITE_TURNIP, QUEST_CROP_WITCHBERRY, QUEST_CROP_SCALLION
        };
        data.activeCropDaily = pool[urand(0, CROP_DAILY_COUNT - 1)];
    }
    return data.activeCropDaily;
}

uint32 DailyQuestManager::GetRandomKillDaily(ObjectGuid playerGuid)
{
    std::lock_guard<std::mutex> lock(_mutex);
    auto it = _dailyData.find(playerGuid);
    if (it == _dailyData.end())
        return 0;

    auto& data = it->second;
    if (data.activeKillDaily == 0)
    {
        uint32 pool[5] =
        {
            QUEST_KILL_LESSER_EVILS, QUEST_KILL_STEALING, QUEST_KILL_STALLING,
            QUEST_KILL_HUNTER_CHIEF, QUEST_KILL_SIMIAN
        };
        data.activeKillDaily = pool[urand(0, KILL_DAILY_COUNT - 1)];
    }
    return data.activeKillDaily;
}

uint32 DailyQuestManager::GetAndiGiftTarget(ObjectGuid playerGuid)
{
    std::lock_guard<std::mutex> lock(_mutex);
    auto it = _dailyData.find(playerGuid);
    if (it == _dailyData.end())
        return 0;

    auto& data = it->second;
    if (data.AndiGiftTarget == 0)
    {
        uint32 npcs[10] =
        {
            NPC_CHEE_CHEE, NPC_ELLA, NPC_FARMER_FUNG, NPC_FISH_FELLREED, NPC_GINA_MUDCLAW,
            NPC_HAOHAN_MUDCLAW, NPC_JOGU, NPC_OLD_HILLPAW, NPC_SHO, NPC_TINA_MUDCLAW
        };
        data.AndiGiftTarget = npcs[urand(0, 9)];
    }
    return data.AndiGiftTarget;
}

uint8 DailyQuestManager::GetActiveVisitingFarmers(ObjectGuid playerGuid, uint8 index)
{
    if (index >= 2)
        return 0;

    std::lock_guard<std::mutex> lock(_mutex);
    auto it = _dailyData.find(playerGuid);
    if (it == _dailyData.end())
        return 0;

    return it->second.activeVisitingFarmers[index];
}

DailyQuestData* DailyQuestManager::GetDailyData(ObjectGuid playerGuid)
{
    std::lock_guard<std::mutex> lock(_mutex);
    auto it = _dailyData.find(playerGuid);
    if (it == _dailyData.end())
        return nullptr;
    return &it->second;
}

// ============================================================================
// Daily Quest Pool Assignments
// ============================================================================

CropDailyAssignment const CropDailies[CROP_DAILY_COUNT] =
{
    { QUEST_CROP_PUMPKIN,    CropType::MOGU_PUMPKIN,      5, "A Pumpkin-y Perfume" },
    { QUEST_CROP_MELON,      CropType::STRIPED_MELON,     5, "It's Melon Time" },
    { QUEST_CROP_LEEK,       CropType::RED_BLOSSOM_LEEK,  5, "Red Blossom Leeks, You Make the Croc-in' World Go Down" },
    { QUEST_CROP_SQUASH,     CropType::JADE_SQUASH,       5, "Squash Those Foul Odors" },
    { QUEST_CROP_CARROT,     CropType::JUICYCRUNCH_CARROT, 5, "That Dangling Carrot" },
    { QUEST_CROP_CABBAGE,    CropType::GREEN_CABBAGE,     5, "The Cabbage Test" },
    { QUEST_CROP_PINK_TURNIP,CropType::PINK_TURNIP,       5, "The Pink Turnip Challenge" },
    { QUEST_CROP_WHITE_TURNIP,CropType::WHITE_TURNIP,     5, "The White Turnip Treatment" },
    { QUEST_CROP_WITCHBERRY, CropType::WITCHBERRY,        5, "Which Berries? Witchberries." },
    { QUEST_CROP_SCALLION,   CropType::SCALLION,          5, "Why Not Scallions?" },
};



// ============================================================================
// Vote Requirements Table
// ============================================================================

VoteData const VoteRequirements[5] =
{
    { VOTE_GINA,        NPC_GINA_MUDCLAW,   VOTE_ITEM_GINA,        VOTE_QTY_GINA,        VOTE_REP_GINA,        "Gina Mudclaw",      "Scallions" },
    { VOTE_MUNG_MUNG,   NPC_MUNG_MUNG,      VOTE_ITEM_MUNG_MUNG,   VOTE_QTY_MUNG_MUNG,   VOTE_REP_MUNG_MUNG,   "Mung-Mung",         "Juicycrunch Carrots" },
    { VOTE_FARMER_FUNG, NPC_FARMER_FUNG,    VOTE_ITEM_FARMER_FUNG, VOTE_QTY_FARMER_FUNG, VOTE_REP_FARMER_FUNG, "Farmer Fung",       "Green Cabbages" },
    { VOTE_NANA,        NPC_NANA_MUDCLAW,   VOTE_ITEM_NANA,        VOTE_QTY_NANA,        VOTE_REP_NANA,        "Nana Mudclaw",      "Witchberries" },
    { VOTE_HAOHAN,      NPC_HAOHAN_MUDCLAW, VOTE_ITEM_HAOHAN,      VOTE_QTY_HAOHAN,      VOTE_REP_HAOHAN,      "Haohan Mudclaw",    "Striped Melons" },
};

// ============================================================================
// Andi NPC - Gift Delivery Daily Quest Handler
// ============================================================================

class npc_andi_gift : public CreatureScript
{
public:
    npc_andi_gift() : CreatureScript("npc_andi_gift") { }

    bool OnGossipHello(Player* player, Creature* creature) override
    {
        ObjectGuid playerGuid = player->GetGUID();

        if (sDailyQuest->IsDailyComplete(playerGuid, QUEST_ANDI_GIFT))
        {
            AddGossipItemFor(player, GOSSIP_ICON_CHAT, "You've already delivered your gift today. Come back tomorrow!", GOSSIP_SENDER_MAIN, 99);
        }
        else
        {
            uint32 targetNpc = sDailyQuest->GetAndiGiftTarget(playerGuid);
            char const* targetName = nullptr;
            switch (targetNpc)
            {
                case NPC_CHEE_CHEE:      targetName = "Chee Chee"; break;
                case NPC_ELLA:           targetName = "Ella"; break;
                case NPC_FARMER_FUNG:    targetName = "Farmer Fung"; break;
                case NPC_FISH_FELLREED:  targetName = "Fish Fellreed"; break;
                case NPC_GINA_MUDCLAW:   targetName = "Gina Mudclaw"; break;
                case NPC_HAOHAN_MUDCLAW: targetName = "Haohan Mudclaw"; break;
                case NPC_JOGU:           targetName = "Jogu the Drunk"; break;
                case NPC_OLD_HILLPAW:    targetName = "Old Hillpaw"; break;
                case NPC_SHO:            targetName = "Sho"; break;
                case NPC_TINA_MUDCLAW:   targetName = "Tina Mudclaw"; break;
            }

            if (targetName)
            {
                AddGossipItemFor(player, GOSSIP_ICON_CHAT,
                    std::string("Deliver the gift to ") + targetName + " (turn in to complete Andi's daily)",
                    GOSSIP_SENDER_MAIN, 1);
            }
        }

        AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Goodbye.", GOSSIP_SENDER_MAIN, 99);
        SendGossipMenuFor(player, DEFAULT_GOSSIP_MESSAGE, creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action) override
    {
        CloseGossipMenuFor(player);
        ObjectGuid playerGuid = player->GetGUID();

        if (action == 99)
            return true;

        if (action == 1)
        {
            uint32 targetNpc = sDailyQuest->GetAndiGiftTarget(playerGuid);
            Creature* targetCreature = player->FindNearestCreature(targetNpc, 5.0f);

            if (!targetCreature)
            {
                player->GetSession()->SendNotification("The recipient is not nearby. Find them first!");
                return true;
            }

            // Check if player has the gift item
            if (!player->HasItemCount(ITEM_ANDI_GIFT, 1))
            {
                player->GetSession()->SendNotification("You don't have the gift from Andi.");
                return true;
            }

            // Deliver the gift
            player->DestroyItemCount(ITEM_ANDI_GIFT, 1, true);
            sDailyQuest->CompleteDaily(playerGuid, QUEST_ANDI_GIFT);

            // Rewards
            player->GetReputationMgr().ModifyReputation(sFactionStore.LookupEntry(FACTION_TILLERS), DAILY_ANDI_REWARD_REP);

            // Friendship with the target NPC
            sFriendship->ModifyStanding(playerGuid, targetNpc, DAILY_ANDI_FRIENDSHIP_GAIN);

            // Also give friendship with Andi (NPC_ANDI)
            sFriendship->ModifyStanding(playerGuid, NPC_ANDI, DAILY_ANDI_FRIENDSHIP_GAIN);

            player->GetSession()->SendNotification("You deliver the gift and gain %d Tillers reputation and %d friendship!", DAILY_ANDI_REWARD_REP, DAILY_ANDI_FRIENDSHIP_GAIN);
        }

        return true;
    }
};



// ============================================================================
// Registration
// ============================================================================

void AddSC_tillers_dailies()
{
    new npc_andi_gift();
}
