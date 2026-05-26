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
#include "Player.h"
#include "Creature.h"
#include "DatabaseEnv.h"
#include "ReputationMgr.h"
#include "DBCStores.h"
#include "World.h"
#include "ObjectMgr.h"
#include "QuestDef.h"
#include "GossipDef.h"

#include "tillers_farm.h"

void StartFarmExpansion(Player* player, Creature* creature)
{
    PlayerFarmCache* data = GetPlayerFarmData(player->GetGUID());
    if (!data || data->expansionTimerEnd > 0)
        return;

    uint32 now = uint32(sWorld->GetGameTime());
    data->expansionTimerEnd = now + FARM_EXPANSION_TIME;

    CharacterDatabase.PExecute(
        "UPDATE character_tillers_farm_data SET expansion_timer_end = %u WHERE guid = %u",
        data->expansionTimerEnd, player->GetGUID().GetCounter());

    creature->AI()->Talk(0, player);
    player->GetSession()->SendNotification("Farmer Yoon starts working. Check back in 15 minutes!");
}

bool HasReputation(Player* player, uint32 threshold)
{
    FactionEntry const* tillersFaction = sFactionStore.LookupEntry(FACTION_TILLERS);
    if (!tillersFaction)
        return false;
    return player->GetReputationMgr().GetReputation(tillersFaction) >= threshold;
}

// ============================================================================
// Server-wide daily rotation helpers
// ============================================================================

static bool IsCropDailyActive(uint32 questId)
{
    uint32 todaySeed = GetTodaySeed();
    uint32 index = questId - QUEST_CROP_BASE;
    if (index >= CROP_DAILY_COUNT)
        return false;
    bool cropSecondHalf = (todaySeed % 2 == 1);
    if (cropSecondHalf)
        return index >= 5;
    else
        return index < 5;
}

static bool IsKillDailyActive(uint32 questId)
{
    uint32 todaySeed = GetTodaySeed();
    uint32 killIndex = todaySeed % KILL_DAILY_COUNT;
    return (questId - QUEST_KILL_BASE) == killIndex;
}

// ============================================================================
// Farmer Yoon NPC Script
// ============================================================================

class npc_farmer_yoon : public CreatureScript
{
public:
    npc_farmer_yoon() : CreatureScript("npc_farmer_yoon") { }

    bool OnGossipHello(Player* player, Creature* creature) override
    {
        // Check if timer expired first
        CheckFarmExpiration(player);

        PlayerFarmCache* data = GetPlayerFarmData(player->GetGUID());
        if (!data)
        {
            if (player->GetQuestRewardStatus(QUEST_TUTORIAL_GATE))
            {
                CreatePlayerFarm(player);
                data = GetPlayerFarmData(player->GetGUID());
                if (!data)
                {
                    player->GetSession()->SendNotification("You do not own a farm.");
                    return true;
                }
            }
            else
            {
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, "I need to learn the basics of farming first. Where do I start?", GOSSIP_SENDER_MAIN, 99);
                SendGossipMenuFor(player, DEFAULT_GOSSIP_MESSAGE, creature->GetGUID());
                return true;
            }
        }

        uint32 now = uint32(sWorld->GetGameTime());

        // Show active timer if running
        if (data->expansionTimerEnd > 0)
        {
            uint32 remaining = (now < data->expansionTimerEnd) ? (data->expansionTimerEnd - now) : 0;
            uint32 mins = remaining / 60;
            uint32 secs = remaining % 60;
            char timerBuf[64];
            snprintf(timerBuf, 64, "Farmer Yoon is working... (%u:%02u remaining)", mins, secs);
            AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, timerBuf, GOSSIP_SENDER_MAIN, 0);
        }
        else
        {
            // Expansion options based on unlocked plots + reputation
            if (data->unlockedPlots < 8 && HasReputation(player, REP_TILLERS_HONORED))
                AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "Clear the weeds and expand the farm.", GOSSIP_SENDER_MAIN, 10);

            else if (data->unlockedPlots < 12 && HasReputation(player, REP_TILLERS_REVERED))
                AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "Fix the broken wagon to expand the farm.", GOSSIP_SENDER_MAIN, 11);

            else if (data->unlockedPlots < 16 && HasReputation(player, REP_TILLERS_EXALTED) && data->votesMask >= 31)
                AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "Move the mossy boulder to expand the farm.", GOSSIP_SENDER_MAIN, 12);

            // Tool installation options
            if (data->unlockedPlots >= 8 && !data->upgrades.hasIrrigation)
            {
                if (player->HasItemCount(ITEM_JINYU_PRINCESS_SPRINKLER, 1))
                    AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "Install the Jinyu Princess Sprinkler System.", GOSSIP_SENDER_MAIN, 20);
                else
                    AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "Bring me a Jinyu Princess Sprinkler to install.", GOSSIP_SENDER_MAIN, 0);
            }

            if (data->unlockedPlots >= 12 && !data->upgrades.hasAntipest)
            {
                if (player->HasItemCount(ITEM_THUNDER_KING_PEST_REPELLERS, 1))
                    AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "Install the Thunder King Pest Repellers.", GOSSIP_SENDER_MAIN, 21);
                else
                    AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "Bring me Thunder King Pest Repellers to install.", GOSSIP_SENDER_MAIN, 0);
            }

            if (data->unlockedPlots >= 16 && !data->upgrades.hasPlow)
            {
                if (player->HasItemCount(ITEM_EARTH_SLASHER, 1))
                    AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "Install the Earth-Slasher Master Plow.", GOSSIP_SENDER_MAIN, 22);
                else
                    AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "Bring me the Earth-Slasher assembly kit to install.", GOSSIP_SENDER_MAIN, 0);
            }
        }

        // ====================================================================
        // Daily Quest Rotation — uses creature_queststarter relations + filtering
        // ====================================================================

        if (creature->IsQuestGiver())
        {
            QuestRelationResult objectQR = sObjectMgr->GetCreatureQuestRelations(creature->GetEntry());
            QuestMenu& qm = player->PlayerTalkClass->GetQuestMenu();
            qm.ClearMenu();

            // Turn-in quests (involved relations)
            QuestRelationResult objectQIR = sObjectMgr->GetCreatureQuestInvolvedRelations(creature->GetEntry());
            for (uint32 questId : objectQIR)
            {
                QuestStatus status = player->GetQuestStatus(questId);
                if (status == QUEST_STATUS_COMPLETE)
                    qm.AddMenuItem(questId, 4);
                else if (status == QUEST_STATUS_INCOMPLETE)
                    qm.AddMenuItem(questId, 4);
            }

            // Available quests — filter by rotation
            for (uint32 questId : objectQR)
            {
                Quest const* quest = sObjectMgr->GetQuestTemplate(questId);
                if (!quest || !player->CanTakeQuest(quest, false))
                    continue;

                bool active = false;
                if (questId >= QUEST_CROP_BASE && questId < QUEST_CROP_BASE + CROP_DAILY_COUNT)
                    active = IsCropDailyActive(questId);
                else if (questId >= QUEST_KILL_BASE && questId < QUEST_KILL_BASE + KILL_DAILY_COUNT)
                    active = IsKillDailyActive(questId);
                else if (questId == QUEST_ANDI_GIFT)
                    active = true;

                if (!active)
                    continue;

                if (player->GetQuestStatus(questId) == QUEST_STATUS_NONE)
                    qm.AddMenuItem(questId, 2);
                else if (quest->IsRepeatable() && player->GetQuestStatus(questId) == QUEST_STATUS_COMPLETE)
                    qm.AddMenuItem(questId, 4);
            }
        }

        AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Goodbye.", GOSSIP_SENDER_MAIN, 99);
        SendGossipMenuFor(player, DEFAULT_GOSSIP_MESSAGE, creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 sender, uint32 action) override
    {
        CloseGossipMenuFor(player);

        if (action == 99)
            return true;

        PlayerFarmCache* data = GetPlayerFarmData(player->GetGUID());
        if (!data)
            return true;

        switch (action)
        {
            case 10: // Expansion I (plots: 4 -> 8)
            case 11: // Expansion II (plots: 8 -> 12)
            case 12: // Expansion III (plots: 12 -> 16)
            {
                StartFarmExpansion(player, creature);
                break;
            }

            case 20: // Install Jinyu Princess Sprinkler
            {
                if (!player->HasItemCount(ITEM_JINYU_PRINCESS_SPRINKLER, 1))
                    break;

                player->DestroyItemCount(ITEM_JINYU_PRINCESS_SPRINKLER, 1, true);
                data->upgrades.hasIrrigation = true;
                CharacterDatabase.PExecute(
                    "UPDATE character_tillers_farm_data SET has_irrigation = 1 WHERE guid = %u",
                    player->GetGUID().GetCounter());
                creature->AI()->Talk(1, player);
                player->GetSession()->SendNotification("Irrigation system installed! Parched crops will be watered automatically.");
                break;
            }

            case 21: // Install Thunder King Pest Repellers
            {
                if (!player->HasItemCount(ITEM_THUNDER_KING_PEST_REPELLERS, 1))
                    break;

                player->DestroyItemCount(ITEM_THUNDER_KING_PEST_REPELLERS, 1, true);
                data->upgrades.hasAntipest = true;
                CharacterDatabase.PExecute(
                    "UPDATE character_tillers_farm_data SET has_antipest = 1 WHERE guid = %u",
                    player->GetGUID().GetCounter());
                creature->AI()->Talk(2, player);
                player->GetSession()->SendNotification("Pest repellers installed! Infested crops will be cleared automatically.");
                break;
            }

            case 22: // Install Earth-Slasher Master Plow
            {
                if (!player->HasItemCount(ITEM_EARTH_SLASHER, 1))
                    break;

                player->DestroyItemCount(ITEM_EARTH_SLASHER, 1, true);
                data->upgrades.hasPlow = true;
                CharacterDatabase.PExecute(
                    "UPDATE character_tillers_farm_data SET has_plow = 1 WHERE guid = %u",
                    player->GetGUID().GetCounter());
                creature->AI()->Talk(3, player);
                player->GetSession()->SendNotification("Earth-Slasher installed! Here is the Master Plow.");
                player->AddItem(ITEM_MASTER_PLOW, 1);
                break;
            }
        }

        return true;
    }
};

// ============================================================================
// Registration
// ============================================================================

void AddSC_npc_farmer_yoon()
{
    new npc_farmer_yoon();
}
