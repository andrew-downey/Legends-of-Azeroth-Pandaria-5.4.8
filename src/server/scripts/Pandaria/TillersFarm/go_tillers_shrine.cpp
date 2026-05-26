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
#include "GameObject.h"
#include "DatabaseEnv.h"
#include "ReputationMgr.h"
#include "DBCStores.h"
#include "World.h"

#include "tillers_farm.h"

// ============================================================================
// Tillers Shrine GameObject Script
// Shows per-NPC standing via reputation API for all 10 Tiller NPCs
// ============================================================================

class go_tillers_shrine : public GameObjectScript
{
public:
    go_tillers_shrine() : GameObjectScript("go_tillers_shrine") { }

    bool OnGossipHello(Player* player, GameObject* go) override
    {
        uint32 gossipCount = 0;

        // Show overall Tillers faction reputation
        FactionEntry const* tillersFaction = sFactionStore.LookupEntry(FACTION_TILLERS);
        if (tillersFaction)
        {
            uint32 rep = player->GetReputationMgr().GetReputation(tillersFaction);
            char repBuf[128];
            snprintf(repBuf, sizeof(repBuf), "Tillers Reputation: %u", rep);
            AddGossipItemFor(player, GOSSIP_ICON_MONEY_BAG, repBuf, GOSSIP_SENDER_MAIN, 0);
            gossipCount++;
        }

        // Show per-NPC standings
        uint32 const npcEntries[10] =
        {
            NPC_CHEE_CHEE, NPC_ELLA, NPC_FARMER_FUNG, NPC_FISH_FELLREED, NPC_GINA_MUDCLAW,
            NPC_HAOHAN_MUDCLAW, NPC_JOGU, NPC_OLD_HILLPAW, NPC_SHO, NPC_TINA_MUDCLAW
        };

        char const* npcNames[10] =
        {
            "Chee Chee", "Ella", "Farmer Fung", "Fish Fellreed", "Gina Mudclaw",
            "Haohan Mudclaw", "Jogu", "Old Hillpaw", "Sho", "Tina Mudclaw"
        };

        for (uint8 i = 0; i < 10; ++i)
        {
            int32 factionId = GetFactionIdForNpc(npcEntries[i]);
            FactionEntry const* faction = sFactionStore.LookupEntry(factionId);
            int32 standing = faction ? player->GetReputationMgr().GetReputation(faction) : 0;
            FriendlyRank rank = GetFriendlyRank(standing);

            char standBuf[256];
            snprintf(standBuf, sizeof(standBuf), "%s: %s (%d standing)", npcNames[i], GetFriendlyRankName(rank), standing);
            AddGossipItemFor(player, GOSSIP_ICON_CHAT, standBuf, GOSSIP_SENDER_MAIN, 0);
            gossipCount++;
        }

        AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Close.", GOSSIP_SENDER_MAIN, 99);
        SendGossipMenuFor(player, DEFAULT_GOSSIP_MESSAGE, go->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, GameObject* go, uint32 /*sender*/, uint32 action) override
    {
        CloseGossipMenuFor(player);
        if (action == 99)
            return true;
        return OnGossipHello(player, go);
    }
};

// ============================================================================
// Registration
// ============================================================================

void AddSC_tillers_shrine()
{
    new go_tillers_shrine();
}
