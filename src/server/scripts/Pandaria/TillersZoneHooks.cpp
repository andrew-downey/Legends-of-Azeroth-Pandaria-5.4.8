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
#include "Player.h"
#include "QuestDef.h"
#include "TillersFarmMgr.h"

class tillers_zone_hooks : public PlayerScript
{
public:
    tillers_zone_hooks() : PlayerScript("tillers_zone_hooks") { }

    void OnUpdateZone(Player* player, uint32 newZone, uint32 /*newArea*/) override
    {
        if (newZone != TillersFarmMgr::VFW_ZONE_ID)
            return;

        // Check if farm is not yet spawned for this player
        if (!sTillersFarmMgr.IsPlayerFarmSpawned(player))
            sTillersFarmMgr.SpawnPlayerFarm(player);
    }

    void OnMapChanged(Player* player) override
    {
        // Despawn farm when leaving Valley of the Four Winds (map changed)
        if (player->GetZoneId() != TillersFarmMgr::VFW_ZONE_ID)
            sTillersFarmMgr.DespawnPlayerFarm(player);
    }

    void OnLogout(Player* player) override
    {
        // Save farm state before disconnect
        if (sTillersFarmMgr.IsPlayerFarmSpawned(player))
            sTillersFarmMgr.DespawnPlayerFarm(player);
    }

    void OnQuestRewarded(Player* player, const Quest* quest) override
    {
        if (!player || !quest)
            return;

        uint32 questId = quest->GetQuestId();

        // Spawn farm immediately when key progression quests are completed
        if ((questId == 30252 || questId == 31945) && player->GetZoneId() == TillersFarmMgr::VFW_ZONE_ID)
            sTillersFarmMgr.SpawnPlayerFarm(player);
    }
};

void AddTillersZoneHooks()
{
    new tillers_zone_hooks();
}
