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

#include "ScriptMgr.h"
#include "ScriptedGossip.h"
#include "Player.h"
#include "GameObject.h"
#include "TillersFarmMgr.h"

// Workstation GO entries at Sunsong Ranch
static uint32 const WORKBENCH_GO_ENTRY = 400000;
static uint32 const COMPOSTER_GO_ENTRY = 400001;

// ============================================================================
// Workbench - converts crops into advanced seeds or fertilizer
// ============================================================================

class go_tillers_workbench : public GameObjectScript
{
public:
    go_tillers_workbench() : GameObjectScript("go_tillers_workbench") { }

    bool OnGossipHello(Player* player, GameObject* go) override
    {
        if (!player || !go)
            return false;

        // TODO: Implement actual conversion logic (crop -> advanced seeds / fertilizer)
        // For now, show basic gossip menu

        AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Convert crops to advanced seeds (TODO)", 10, 0);
        AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Create fertilizer from crop waste (TODO)", 20, 0);
        AddGossipItemFor(player, GOSSIP_ICON_CHAT, "|cFF9D9D9FClose|r", 99, 0);

        SendGossipMenuFor(player, 100009, go->GetGUID()); // "Workbench" gossip menu ID
        return true;
    }
};

// ============================================================================
// Composter - converts crop waste into compost for soil enrichment
// ============================================================================

class go_tillers_composter : public GameObjectScript
{
public:
    go_tillers_composter() : GameObjectScript("go_tillers_composter") { }

    bool OnGossipHello(Player* player, GameObject* go) override
    {
        if (!player || !go)
            return false;

        // TODO: Implement actual composting logic (crop waste -> compost for soil enrichment)
        // For now, show basic gossip menu

        AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Add crop waste to composter (TODO)", 10, 0);
        AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Collect compost for soil enrichment (TODO)", 20, 0);
        AddGossipItemFor(player, GOSSIP_ICON_CHAT, "|cFF9D9D9FClose|r", 99, 0);

        SendGossipMenuFor(player, 100010, go->GetGUID()); // "Composter" gossip menu ID
        return true;
    }
};

void AddTillersWorkstation()
{
    new go_tillers_workbench();
    new go_tillers_composter();
}
