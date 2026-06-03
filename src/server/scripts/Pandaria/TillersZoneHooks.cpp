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

        // Only spawn farm for players who have completed the entry quest
        // Pre-30252 players interact with the static ground Yoon at PUBLIC_FARM_MASK
        if (!player->IsQuestRewarded(30252))
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
        if ((questId == 30252 || questId == 30256) && player->GetZoneId() == TillersFarmMgr::VFW_ZONE_ID)
            sTillersFarmMgr.SpawnPlayerFarm(player);

        // Update farm state based on obstacle-clearing quests
        if (player->GetZoneId() == TillersFarmMgr::VFW_ZONE_ID)
        {
            uint32 guidLow = player->GetGUID().GetCounter();
            PlayerFarmState& state = sTillersFarmMgr.GetPlayerState(guidLow);
            uint8 oldState = state.farmState;
            uint8 newState = oldState;

            switch (questId)
            {
                case 30516: // Growing the Farm I: A Little Problem (weeds cleared)
                    newState = FARM_STATE_WEEDS_CLEARED;
                    break;
                case 30524: // Growing the Farm II: Knock on Wood (wagon cleared)
                    newState = FARM_STATE_WAGON_CLEARED;
                    break;
                case 30529: // Growing the Farm III: The Mossy Boulder (all cleared)
                    newState = FARM_STATE_ALL_CLEARED;
                    break;
                default:
                    return;
            }

            if (newState != oldState)
            {
                state.farmState = newState;
                state.plotsUnlocked = GetPlotsUnlockedForFarmState(newState);

                // Rebuild farm from state (Scene Builder handles phase + objects)
                sTillersFarmMgr.SpawnPlayerFarm(player);

                TC_LOG_INFO("scripts", "TillersZoneHooks: Player %u farm state changed: %u -> %u (plots: %u)",
                    guidLow, oldState, newState, state.plotsUnlocked);
            }
        }
    }
};

void AddTillersZoneHooks()
{
    new tillers_zone_hooks();
}
