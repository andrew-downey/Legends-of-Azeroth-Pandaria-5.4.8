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
#include "ObjectMgr.h"
#include "QuestDef.h"

#include "tillers_farm.h"

// ============================================================================
// Shared vote gossip helpers
// ============================================================================

static void HandleVoteGossip(Player* player, Creature* creature, uint8 voteSlot)
{
    ObjectGuid playerGuid = player->GetGUID();
    PlayerFarmCache* data = GetPlayerFarmData(playerGuid);
    if (!data)
        return;

    auto const& vr = VoteRequirements[voteSlot];

    if (data->votesMask & vr.voteBit)
    {
        AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Thank you! I've already given you my vote.", GOSSIP_SENDER_MAIN, 0);
    }
    else
    {
        FactionEntry const* tillersFaction = sFactionStore.LookupEntry(FACTION_TILLERS);
        int32 rep = tillersFaction ? player->GetReputationMgr().GetReputation(tillersFaction) : 0;
        if (rep < vr.repThreshold)
        {
            AddGossipItemFor(player, GOSSIP_ICON_CHAT, "You need to prove yourself to the Tillers before I can give my vote.", GOSSIP_SENDER_MAIN, 0);
        }
        else
        {
            uint32 hasCount = player->GetItemCount(vr.cropItem, false);
            if (hasCount >= vr.requiredCount)
            {
                AddGossipItemFor(player, GOSSIP_ICON_MONEY_BAG,
                    std::string("Turn in ") + std::to_string(vr.requiredCount) + " " + vr.cropName + " to earn my vote (+350 rep)",
                    GOSSIP_SENDER_MAIN, 1);
            }
            else
            {
                AddGossipItemFor(player, GOSSIP_ICON_CHAT,
                    std::string("Bring me ") + std::to_string(vr.requiredCount) + " " + vr.cropName + " to earn my vote. (Have: " + std::to_string(hasCount) + ")",
                    GOSSIP_SENDER_MAIN, 0);
            }
        }
    }

    AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Goodbye.", GOSSIP_SENDER_MAIN, 99);
    SendGossipMenuFor(player, DEFAULT_GOSSIP_MESSAGE, creature->GetGUID());
}

static void HandleVoteSelect(Player* player, Creature* creature, uint8 voteSlot)
{
    ObjectGuid playerGuid = player->GetGUID();
    PlayerFarmCache* data = GetPlayerFarmData(playerGuid);
    if (!data)
        return;

    auto const& vr = VoteRequirements[voteSlot];

    if (data->votesMask & vr.voteBit)
    {
        player->GetSession()->SendNotification("You already earned %s's vote!", vr.name);
        return;
    }

    uint32 hasCount = player->GetItemCount(vr.cropItem, false);
    if (hasCount < vr.requiredCount)
    {
        player->GetSession()->SendNotification("You need %u %s to earn %s's vote.", vr.requiredCount, vr.cropName, vr.name);
        return;
    }

    player->DestroyItemCount(vr.cropItem, vr.requiredCount, true);
    player->GetReputationMgr().ModifyReputation(sFactionStore.LookupEntry(FACTION_TILLERS), VOTE_REP_GAIN);

    int32 npcFactionId = GetFactionIdForNpc(vr.npcEntry);
    FactionEntry const* npcFaction = sFactionStore.LookupEntry(npcFactionId);
    if (npcFaction)
        player->GetReputationMgr().ModifyReputation(npcFaction, VOTE_FRIENDSHIP_GAIN);

    data->votesMask |= vr.voteBit;
    CharacterDatabase.PExecute(
        "UPDATE character_tillers_farm_data SET votes_mask = %u WHERE guid = %u",
        data->votesMask, playerGuid.GetCounter());

    player->GetSession()->SendNotification("You earn %s's vote! +%d Tillers rep, +%d standing.", vr.name, VOTE_REP_GAIN, VOTE_FRIENDSHIP_GAIN);
}

// ============================================================================
// Mung-Mung (entry 58733) — vote slot 1
// ============================================================================

class npc_mung_mung : public CreatureScript
{
public:
    npc_mung_mung() : CreatureScript("npc_mung_mung") { }

    bool OnGossipHello(Player* player, Creature* creature) override
    {
        HandleVoteGossip(player, creature, 1);
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action) override
    {
        CloseGossipMenuFor(player);
        if (action == 99)
            return true;
        if (action == 1)
            HandleVoteSelect(player, creature, 1);
        return true;
    }
};

// ============================================================================
// Nana Mudclaw (entry 64597) — vote slot 3
// ============================================================================

class npc_nana_mudclaw : public CreatureScript
{
public:
    npc_nana_mudclaw() : CreatureScript("npc_nana_mudclaw") { }

    bool OnGossipHello(Player* player, Creature* creature) override
    {
        HandleVoteGossip(player, creature, 3);
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action) override
    {
        CloseGossipMenuFor(player);
        if (action == 99)
            return true;
        if (action == 1)
            HandleVoteSelect(player, creature, 3);
        return true;
    }
};

// ============================================================================
// Registration
// ============================================================================

void AddSC_tillers_vote()
{
    new npc_mung_mung();
    new npc_nana_mudclaw();
}
