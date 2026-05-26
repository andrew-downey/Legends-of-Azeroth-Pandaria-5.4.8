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

// ============================================================================
// NPC entry -> preferred food item map
// ============================================================================

std::map<uint32, uint32> const TillerFoodMap =
{
    { NPC_CHEE_CHEE,     FOOD_CHEE_CHEE },
    { NPC_ELLA,          FOOD_ELLA },
    { NPC_FARMER_FUNG,   FOOD_FARMER_FUNG },
    { NPC_FISH_FELLREED, FOOD_FISH_FELLREED },
    { NPC_GINA_MUDCLAW,  FOOD_GINA_MUDCLAW },
    { NPC_HAOHAN_MUDCLAW, FOOD_HAOHAN_MUDCLAW },
    { NPC_JOGU,          FOOD_JOGU },
    { NPC_OLD_HILLPAW,   FOOD_OLD_HILLPAW },
    { NPC_SHO,           FOOD_SHO },
    { NPC_TINA_MUDCLAW,  FOOD_TINA_MUDCLAW },
};

std::map<uint32, uint32> const TillerGiftMap =
{
    { NPC_CHEE_CHEE,     GIFT_CHEE_CHEE },
    { NPC_ELLA,          GIFT_ELLA },
    { NPC_FARMER_FUNG,   GIFT_FARMER_FUNG },
    { NPC_FISH_FELLREED, GIFT_FISH_FELLREED },
    { NPC_GINA_MUDCLAW,  GIFT_GINA_MUDCLAW },
    { NPC_HAOHAN_MUDCLAW, GIFT_HAOHAN_MUDCLAW },
    { NPC_JOGU,          GIFT_JOGU },
    { NPC_OLD_HILLPAW,   GIFT_OLD_HILLPAW },
    { NPC_SHO,           GIFT_SHO },
    { NPC_TINA_MUDCLAW,  GIFT_TINA_MUDCLAW },
};

// ============================================================================
// Friendship rank helpers
// ============================================================================

char const* GetFriendshipRankName(FriendshipRank rank)
{
    switch (rank)
    {
        case FriendshipRank::STRANGER:      return "Stranger";
        case FriendshipRank::ACQUAINTANCE:  return "Acquaintance";
        case FriendshipRank::BUDDY:         return "Buddy";
        case FriendshipRank::FRIEND:        return "Friend";
        case FriendshipRank::GOOD_FRIEND:   return "Good Friend";
        case FriendshipRank::BEST_FRIEND:   return "Best Friend";
        default:                            return "Unknown";
    }
}

FriendshipRank GetFriendshipRank(int32 standing)
{
    if (standing >= int32(FriendshipRank::BEST_FRIEND))   return FriendshipRank::BEST_FRIEND;
    if (standing >= int32(FriendshipRank::GOOD_FRIEND))   return FriendshipRank::GOOD_FRIEND;
    if (standing >= int32(FriendshipRank::FRIEND))        return FriendshipRank::FRIEND;
    if (standing >= int32(FriendshipRank::BUDDY))         return FriendshipRank::BUDDY;
    if (standing >= int32(FriendshipRank::ACQUAINTANCE))  return FriendshipRank::ACQUAINTANCE;
    return FriendshipRank::STRANGER;
}

// ============================================================================
// Tiller Friendship NPC Script - handles all 10 NPCs
// ============================================================================

class npc_tillers_friendship : public CreatureScript
{
public:
    npc_tillers_friendship() : CreatureScript("npc_tillers_friendship") { }

    bool OnGossipHello(Player* player, Creature* creature) override
    {
        HandleGossip(player, creature);
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action) override
    {
        CloseGossipMenuFor(player);
        HandleSelect(player, creature, action);
        return true;
    }

private:
    void HandleGossip(Player* player, Creature* creature)
    {
        uint32 npcEntry = creature->GetEntry();
        ObjectGuid playerGuid = player->GetGUID();

        PlayerFarmCache* data = GetPlayerFarmData(playerGuid);

        FriendshipEntry* friendship = sFriendship->GetFriendship(playerGuid, npcEntry);
        if (!friendship)
        {
            sFriendship->ModifyStanding(playerGuid, npcEntry, 0);
            friendship = sFriendship->GetFriendship(playerGuid, npcEntry);
        }

        FriendshipRank rank = GetFriendshipRank(friendship->standing);
        int32 nextRankValue = 0;
        switch (rank)
        {
            case FriendshipRank::STRANGER:      nextRankValue = int32(FriendshipRank::ACQUAINTANCE); break;
            case FriendshipRank::ACQUAINTANCE:  nextRankValue = int32(FriendshipRank::BUDDY); break;
            case FriendshipRank::BUDDY:         nextRankValue = int32(FriendshipRank::FRIEND); break;
            case FriendshipRank::FRIEND:        nextRankValue = int32(FriendshipRank::GOOD_FRIEND); break;
            case FriendshipRank::GOOD_FRIEND:   nextRankValue = int32(FriendshipRank::BEST_FRIEND); break;
            case FriendshipRank::BEST_FRIEND:   nextRankValue = 0; break;
        }

        std::string rankText = std::string("Friendship: ") + GetFriendshipRankName(rank);
        if (nextRankValue > 0)
        {
            int32 remaining = nextRankValue - friendship->standing;
            rankText += std::string(" (") + std::to_string(remaining) + " to next rank)";
        }
        else
            rankText += " (Maximum friendship!)";

        AddGossipItemFor(player, GOSSIP_ICON_CHAT, rankText, GOSSIP_SENDER_MAIN, 0);

        // Check food daily timer
        bool canGiveFood = sFriendship->UpdateDailyFoodTimer(playerGuid, npcEntry);

        auto foodIt = TillerFoodMap.find(npcEntry);
        if (foodIt != TillerFoodMap.end())
        {
            if (canGiveFood && player->HasItemCount(foodIt->second, 1))
            {
                std::string foodText = std::string("Offer your ") + GetItemLink(foodIt->second, *player) + " (+1800 standing)";
                AddGossipItemFor(player, GOSSIP_ICON_MONEY_BAG, foodText, GOSSIP_SENDER_MAIN, 1);
            }
            else if (!canGiveFood)
            {
                std::string foodText = std::string("You have already offered food today. (Next: ") + GetItemLink(foodIt->second, *player) + ")";
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, foodText, GOSSIP_SENDER_MAIN, 0);
            }
        }

        // Check gift daily timer
        bool canGiveGift = sFriendship->UpdateDailyGiftTimer(playerGuid, npcEntry);

        auto giftIt = TillerGiftMap.find(npcEntry);
        if (giftIt != TillerGiftMap.end())
        {
            if (canGiveGift)
            {
                // Check if player has preferred gift
                if (player->HasItemCount(giftIt->second, 1))
                {
                    std::string giftText = std::string("Offer your ") + GetItemLink(giftIt->second, *player) + " (+900 standing)";
                    AddGossipItemFor(player, GOSSIP_ICON_MONEY_BAG, giftText, GOSSIP_SENDER_MAIN, 2);
                }

                // Check for other friendship items
                for (auto& itemPair : TillerGiftMap)
                {
                    if (itemPair.first == npcEntry)
                        continue;
                    if (player->HasItemCount(itemPair.second, 1))
                    {
                        std::string giftText = std::string("Offer your ") + GetItemLink(itemPair.second, *player) + " (+540 standing)";
                        AddGossipItemFor(player, GOSSIP_ICON_MONEY_BAG, giftText, GOSSIP_SENDER_MAIN, 100 + itemPair.second);
                    }
                }
            }
            else
            {
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, "You have already given a gift today.", GOSSIP_SENDER_MAIN, 0);
            }
        }

        // ====================================================================
        // Visiting Farmer Quest Completion (if player has an active quest)
        // ====================================================================
        uint32 visitQuestId = 0;
        switch (npcEntry)
        {
            case NPC_CHEE_CHEE:    visitQuestId = QUEST_VISITING_CHEE_CHEE; break;
            case NPC_ELLA:         visitQuestId = QUEST_VISITING_ELLA; break;
            case NPC_FARMER_FUNG:  visitQuestId = QUEST_VISITING_FUNG; break;
            case NPC_FISH_FELLREED:visitQuestId = QUEST_VISITING_FELLREED; break;
            case NPC_GINA_MUDCLAW: visitQuestId = QUEST_VISITING_GINA; break;
            case NPC_JOGU:         visitQuestId = QUEST_VISITING_JOGU; break;
            case NPC_SHO:          visitQuestId = QUEST_VISITING_SHO; break;
            case NPC_TINA_MUDCLAW: visitQuestId = QUEST_VISITING_TINA; break;
        }
        if (visitQuestId > 0 && player->GetQuestStatus(visitQuestId) == QUEST_STATUS_INCOMPLETE)
        {
            if (player->CanCompleteQuest(visitQuestId))
            {
                int32 vfGain = (npcEntry == NPC_GINA_MUDCLAW) ? 2600 : 2000;
                AddGossipItemFor(player, GOSSIP_ICON_CHAT,
                    std::string("Turn in your quest (+150 rep, +") + std::to_string(vfGain) + " friendship)",
                    GOSSIP_SENDER_MAIN, 3);
            }
        }

        // ====================================================================
        // Vote Questlines (Gina, Farmer Fung, Haohan)
        // ====================================================================
        uint8 npcVoteIndex = 0xFF;
        for (uint8 i = 0; i < 5; ++i)
        {
            if (VoteRequirements[i].npcEntry == npcEntry)
            {
                npcVoteIndex = i;
                break;
            }
        }
        if (npcVoteIndex != 0xFF)
        {
            auto const& vr = VoteRequirements[npcVoteIndex];
            if (!(data->votesMask & vr.voteBit))
            {
                FactionEntry const* tillersFaction = sFactionStore.LookupEntry(FACTION_TILLERS);
                int32 rep = tillersFaction ? player->GetReputationMgr().GetReputation(tillersFaction) : 0;
                if (rep >= vr.repThreshold)
                {
                    uint32 hasCount = player->GetItemCount(vr.cropItem, false);
                    if (hasCount >= vr.requiredCount)
                    {
                        AddGossipItemFor(player, GOSSIP_ICON_MONEY_BAG,
                            std::string("Turn in ") + std::to_string(vr.requiredCount) + " " + vr.cropName + " to earn " + vr.name + "'s vote (+350 rep)",
                            GOSSIP_SENDER_MAIN, 4);
                    }
                    else
                    {
                        AddGossipItemFor(player, GOSSIP_ICON_CHAT,
                            std::string("Bring me ") + std::to_string(vr.requiredCount) + " " + vr.cropName + " to earn my vote. (Have: " + std::to_string(hasCount) + ")",
                            GOSSIP_SENDER_MAIN, 0);
                    }
                }
                else
                {
                    AddGossipItemFor(player, GOSSIP_ICON_CHAT,
                        std::string("You need to prove yourself to the Tillers before I can give my vote."),
                        GOSSIP_SENDER_MAIN, 0);
                }
            }
            else
            {
                AddGossipItemFor(player, GOSSIP_ICON_CHAT,
                    std::string("Thank you! I've already given you my vote."),
                    GOSSIP_SENDER_MAIN, 0);
            }
        }

        AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Goodbye.", GOSSIP_SENDER_MAIN, 99);
        SendGossipMenuFor(player, DEFAULT_GOSSIP_MESSAGE, creature->GetGUID());
    }

    void HandleSelect(Player* player, Creature* creature, uint32 action)
    {
        if (action == 99)
            return;

        uint32 npcEntry = creature->GetEntry();
        ObjectGuid playerGuid = player->GetGUID();

        if (action == 1)
        {
            // Food turn-in
            auto foodIt = TillerFoodMap.find(npcEntry);
            if (foodIt != TillerFoodMap.end() && player->HasItemCount(foodIt->second, 1))
            {
                if (sFriendship->UpdateDailyFoodTimer(playerGuid, npcEntry))
                {
                    player->DestroyItemCount(foodIt->second, 1, true);
                    sFriendship->ModifyStanding(playerGuid, npcEntry, FRIENDSHIP_DAILY_FOOD_GAIN);
                    player->GetSession()->SendNotification("You offer food and gain 1800 friendship standing.");
                }
            }
            return;
        }

        if (action == 2)
        {
            // Preferred gift turn-in
            auto giftIt = TillerGiftMap.find(npcEntry);
            if (giftIt != TillerGiftMap.end() && player->HasItemCount(giftIt->second, 1))
            {
                if (sFriendship->UpdateDailyGiftTimer(playerGuid, npcEntry))
                {
                    player->DestroyItemCount(giftIt->second, 1, true);
                    sFriendship->ModifyStanding(playerGuid, npcEntry, FRIENDSHIP_IDEAL_GIFT_GAIN);
                    player->GetSession()->SendNotification("You offer a preferred gift and gain 900 friendship standing.");
                }
            }
            return;
        }

        if (action >= 100)
        {
            // Other gift turn-in (action = 100 + item ID)
            uint32 itemId = action - 100;
            if (player->HasItemCount(itemId, 1))
            {
                if (sFriendship->UpdateDailyGiftTimer(playerGuid, npcEntry))
                {
                    player->DestroyItemCount(itemId, 1, true);
                    sFriendship->ModifyStanding(playerGuid, npcEntry, FRIENDSHIP_DAILY_GIFT_GAIN);
                    player->GetSession()->SendNotification("You offer a gift and gain 540 friendship standing.");
                }
            }
            return;
        }

        if (action == 3)
        {
            // Visiting farmer quest completion
            uint32 questId = 0;
            int32 friendshipGain = 2000;
            switch (npcEntry)
            {
                case NPC_CHEE_CHEE:    questId = QUEST_VISITING_CHEE_CHEE;    friendshipGain = 2000; break;
                case NPC_ELLA:         questId = QUEST_VISITING_ELLA;         friendshipGain = 2000; break;
                case NPC_FARMER_FUNG:  questId = QUEST_VISITING_FUNG;         friendshipGain = 2000; break;
                case NPC_FISH_FELLREED:questId = QUEST_VISITING_FELLREED;     friendshipGain = 2000; break;
                case NPC_GINA_MUDCLAW: questId = QUEST_VISITING_GINA;         friendshipGain = 2600; break;
                case NPC_JOGU:         questId = QUEST_VISITING_JOGU;         friendshipGain = 2000; break;
                case NPC_SHO:          questId = QUEST_VISITING_SHO;          friendshipGain = 2000; break;
                case NPC_TINA_MUDCLAW: questId = QUEST_VISITING_TINA;         friendshipGain = 2000; break;
            }
            if (questId > 0 && player->GetQuestStatus(questId) == QUEST_STATUS_INCOMPLETE)
            {
                player->CompleteQuest(questId, true);
                player->GetReputationMgr().ModifyReputation(sFactionStore.LookupEntry(FACTION_TILLERS), DAILY_VISITING_REWARD_REP);
                sFriendship->ModifyStanding(playerGuid, npcEntry, friendshipGain);
                player->GetSession()->SendNotification("Quest complete! +%d Tillers rep, +%d friendship.", DAILY_VISITING_REWARD_REP, friendshipGain);
            }
            return;
        }

        if (action == 4)
        {
            // Vote crop turn-in
            uint8 voteIndex = 0xFF;
            for (uint8 i = 0; i < 5; ++i)
            {
                if (VoteRequirements[i].npcEntry == npcEntry)
                {
                    voteIndex = i;
                    break;
                }
            }
            if (voteIndex != 0xFF)
            {
                auto const& vr = VoteRequirements[voteIndex];
                uint32 hasCount = player->GetItemCount(vr.cropItem, false);
                if (hasCount >= vr.requiredCount)
                {
                    player->DestroyItemCount(vr.cropItem, vr.requiredCount, true);
                    player->GetReputationMgr().ModifyReputation(sFactionStore.LookupEntry(FACTION_TILLERS), VOTE_REP_GAIN);
                    sFriendship->ModifyStanding(playerGuid, npcEntry, VOTE_FRIENDSHIP_GAIN);

                    PlayerFarmCache* farmData = GetPlayerFarmData(playerGuid);
                    if (farmData)
                    {
                        farmData->votesMask |= vr.voteBit;
                        CharacterDatabase.PExecute(
                            "UPDATE character_tillers_farm_data SET votes_mask = %u WHERE guid = %u",
                            farmData->votesMask, playerGuid.GetCounter());
                    }

                    player->GetSession()->SendNotification("You earn %s's vote! +%d Tillers rep, +%d friendship.", vr.name, VOTE_REP_GAIN, VOTE_FRIENDSHIP_GAIN);
                }
            }
            return;
        }
    }

    std::string GetItemLink(uint32 itemId, Player const& player) const
    {
        ItemTemplate const* item = sObjectMgr->GetItemTemplate(itemId);
        if (!item)
            return "item";
        return std::string("|cff33ff33|Hitem:") + std::to_string(itemId) + ":0:0:0:0:0:0:0|h[" + item->Name1 + "]|h|h|r";
    }
};

// ============================================================================
// Registration
// ============================================================================

void AddSC_tillers_friendship()
{
    new npc_tillers_friendship();
}
