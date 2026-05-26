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

            // Tool installation options (after expansions are complete)
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
        // Daily Quest Assignment
        // ====================================================================
        ObjectGuid playerGuid = player->GetGUID();

        // Daily crop quest
        uint32 cropQuestId = sDailyQuest->GetRandomCropDaily(playerGuid);
        if (cropQuestId > 0 && !sDailyQuest->IsDailyComplete(playerGuid, cropQuestId))
        {
            for (uint8 i = 0; i < CROP_DAILY_COUNT; ++i)
            {
                if (CropDailies[i].questId == cropQuestId)
                {
                    std::string cropText = std::string("Plant ") + std::to_string(CropDailies[i].requiredCount) + " " + GetCropName(CropDailies[i].crop);
                    AddGossipItemFor(player, GOSSIP_ICON_CHAT, cropText + " (+350 rep)", GOSSIP_SENDER_MAIN, cropQuestId);
                    break;
                }
            }
        }

        // Daily kill quest
        uint32 killQuestId = sDailyQuest->GetRandomKillDaily(playerGuid);
        if (killQuestId > 0 && !sDailyQuest->IsDailyComplete(playerGuid, killQuestId))
        {
            char const* killName = nullptr;
            switch (killQuestId)
            {
                case QUEST_KILL_LESSER_EVILS: killName = "The Lesser of Two Evils"; break;
                case QUEST_KILL_STEALING:     killName = "Stealing is Bad..."; break;
                case QUEST_KILL_STALLING:     killName = "Stalling the Ravage"; break;
                case QUEST_KILL_HUNTER_CHIEF: killName = "The Kunzen Hunter-Chief"; break;
                case QUEST_KILL_SIMIAN:       killName = "Simian Sabotage"; break;
            }
            if (killName)
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, std::string(killName) + " (+275 rep)", GOSSIP_SENDER_MAIN, killQuestId);
        }

        // Andi gift quest
        if (!sDailyQuest->IsDailyComplete(playerGuid, QUEST_ANDI_GIFT))
        {
            uint32 giftTarget = sDailyQuest->GetAndiGiftTarget(playerGuid);
            char const* targetName = nullptr;
            switch (giftTarget)
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
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, std::string("Deliver gift to ") + targetName + " (+150 rep)", GOSSIP_SENDER_MAIN, QUEST_ANDI_GIFT);
        }

        // Visiting farmers
        uint32 visitingNpcs[8] =
        {
            NPC_CHEE_CHEE, NPC_ELLA, NPC_FARMER_FUNG, NPC_FISH_FELLREED,
            NPC_GINA_MUDCLAW, NPC_JOGU, NPC_SHO, NPC_TINA_MUDCLAW
        };
        char const* visitingNames[8] =
        {
            "Chee Chee", "Ella", "Farmer Fung", "Fish Fellreed",
            "Gina Mudclaw", "Jogu the Drunk", "Sho", "Tina Mudclaw"
        };
        char const* visitingQuestNames[8] =
        {
            "Not in Chee-Chee's Backyard", "You Have to Burn the Ropes", "Water, Water Everywhere",
            "The Kunzen Legend-Chief", "Money Matters", "Weed War II", "Where It Counts", "They Don't Even Wear Them"
        };

        uint32 farmer1 = 0, farmer2 = 0;
        uint8 sel = 0, attempts = 0;
        while (sel < 2 && attempts < 50)
        {
            uint8 idx = urand(0, 7);
            bool dup = (visitingNpcs[idx] == farmer1);
            if (!dup) { if (sel == 0) farmer1 = visitingNpcs[idx]; else farmer2 = visitingNpcs[idx]; sel++; }
            attempts++;
        }
        if (farmer1)
        {
            for (uint8 i = 0; i < 8; ++i)
                if (visitingNpcs[i] == farmer1)
                    AddGossipItemFor(player, GOSSIP_ICON_CHAT, visitingQuestNames[i] + std::string(" (") + visitingNames[i] + " visits) (+150 rep, +2000 friendship)", GOSSIP_SENDER_MAIN, farmer1);
        }
        if (farmer2)
        {
            for (uint8 i = 0; i < 8; ++i)
                if (visitingNpcs[i] == farmer2)
                    AddGossipItemFor(player, GOSSIP_ICON_CHAT, visitingQuestNames[i] + std::string(" (") + visitingNames[i] + " visits) (+150 rep, +2000 friendship)", GOSSIP_SENDER_MAIN, farmer2);
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

        // ====================================================================
        // Daily Quest Granting (actions = quest IDs or NPC entries)
        // ====================================================================
        ObjectGuid playerGuid = player->GetGUID();

        if (action >= QUEST_CROP_BASE && action < QUEST_CROP_BASE + CROP_DAILY_COUNT)
        {
            if (!sDailyQuest->IsDailyComplete(playerGuid, action))
            {
                Quest const* quest = sObjectMgr->GetQuestTemplate(action);
                if (quest && player->CanAddQuest(quest, true))
                {
                    player->AddQuest(quest, creature);
                    player->GetSession()->SendNotification("You accepted the daily crop quest.");
                }
            }
        }
        else if (action >= QUEST_KILL_BASE && action < QUEST_KILL_BASE + KILL_DAILY_COUNT)
        {
            if (!sDailyQuest->IsDailyComplete(playerGuid, action))
            {
                Quest const* quest = sObjectMgr->GetQuestTemplate(action);
                if (quest && player->CanAddQuest(quest, true))
                {
                    player->AddQuest(quest, creature);
                    player->GetSession()->SendNotification("You accepted the daily kill quest.");
                }
            }
        }
        else if (action == QUEST_ANDI_GIFT)
        {
            if (!sDailyQuest->IsDailyComplete(playerGuid, QUEST_ANDI_GIFT))
            {
                Quest const* quest = sObjectMgr->GetQuestTemplate(QUEST_ANDI_GIFT);
                if (quest && player->CanAddQuest(quest, true))
                {
                    player->AddQuest(quest, creature);
                    player->GetSession()->SendNotification("Andi gives you a gift to deliver!");
                }
            }
        }
        else if (action == NPC_CHEE_CHEE || action == NPC_ELLA || action == NPC_FARMER_FUNG ||
                 action == NPC_FISH_FELLREED || action == NPC_GINA_MUDCLAW || action == NPC_JOGU ||
                 action == NPC_SHO || action == NPC_TINA_MUDCLAW)
        {
            uint32 questId = 0;
            switch (action)
            {
                case NPC_CHEE_CHEE:    questId = QUEST_VISITING_CHEE_CHEE; break;
                case NPC_ELLA:         questId = QUEST_VISITING_ELLA; break;
                case NPC_FARMER_FUNG:  questId = QUEST_VISITING_FUNG; break;
                case NPC_FISH_FELLREED:questId = QUEST_VISITING_FELLREED; break;
                case NPC_GINA_MUDCLAW: questId = QUEST_VISITING_GINA; break;
                case NPC_JOGU:         questId = QUEST_VISITING_JOGU; break;
                case NPC_SHO:          questId = QUEST_VISITING_SHO; break;
                case NPC_TINA_MUDCLAW: questId = QUEST_VISITING_TINA; break;
            }
            if (questId > 0)
            {
                Quest const* quest = sObjectMgr->GetQuestTemplate(questId);
                if (quest && player->CanAddQuest(quest, true))
                {
                    player->AddQuest(quest, creature);
                    player->GetSession()->SendNotification("You accepted the visiting farmer quest!");
                }
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
