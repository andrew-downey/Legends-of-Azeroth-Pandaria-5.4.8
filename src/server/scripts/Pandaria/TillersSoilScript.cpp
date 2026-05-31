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

class tillers_soil_patch : public GameObjectScript
{
public:
    tillers_soil_patch() : GameObjectScript("tillers_soil_patch") { }

    bool OnGossipHello(Player* player, GameObject* go) override
    {
        if (!player)
            return false;

        // Extract plotId from GO's SpellId (set during creation)
        uint32 plotIdVal = go->GetSpellId();
        uint8 plotId = static_cast<uint8>(plotIdVal);

        if (!TillersFarmMgr::IsValidPlotId(plotId))
        {
            TC_LOG_ERROR("scripts", "TillersSoilScript: Invalid plotId %u from GO %u for player %s",
                        plotId, go->GetEntry(), player->GetName().c_str());
            return false;
        }

        HandleSoilGossip(player, go, plotId);
        return true;
    }

private:
    void HandleSoilGossip(Player* player, GameObject* go, uint8 plotId)
    {
        PlotMap& plots = sTillersFarmMgr.GetPlayerPlots(player);
        auto pit = plots.find(plotId);
        if (pit == plots.end())
            return;

        FarmPlotData const& plot = pit->second;
        PlayerFarmState const& state = sTillersFarmMgr.GetPlayerState(player->GetGUID().GetCounter());

        bool plotsUnlocked = (plotId < GetPlotsUnlockedForPhase(state.farmPhase));
        if (!plotsUnlocked)
        {
            player->SEND_GOSSIP_MENU(100001, go->GetGUID()); // "This plot is not yet available."
            return;
        }

        switch (plot.state)
        {
            case PLOT_EMPTY:
            case PLOT_SOIL_PREPARED:
            {
                // Show planting menu - only if player has seeds in inventory
                // We check for any known crop seed items
                bool hasSeeds = player->HasItemCount(79102, 1) ||   // wheat seeds (example entry)
                                player->HasItemCount(110030, 1) ||   // rice seeds
                                player->HasItemCount(110031, 1);      // advanced seeds

                if (!hasSeeds)
                {
                    AddGossipItemFor(player, GOSSIP_ICON_CHAT, "This soil is ready for planting. You need seeds to plant here.", 0, plotId);
                }
                else
                {
                    AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Plant seeds here", 1, plotId);

                    // Show seed options based on what the player has
                    if (player->HasItemCount(79102, 1))
                        AddGossipItemFor(player, GOSSIP_ICON_CHAT, "|TInterface/Icons/inv_grain_wheat:30:30:-18:0|r Wheat Seeds", 2, plotId);

                    if (player->HasItemCount(110030, 1))
                        AddGossipItemFor(player, GOSSIP_ICON_CHAT, "|TInterface/Icons/inv_grain_rice:30:30:-18:0|r Rice Seeds", 3, plotId);

                    if (player->HasItemCount(110031, 1))
                        AddGossipItemFor(player, GOSSIP_ICON_CHAT, "|TInterface/Icons/inv_grain_advanced:30:30:-18:0|r Advanced Seeds", 4, plotId);

                    SendGossipMenuFor(player, 100002, go->GetGUID()); // "Soil Patch" gossip menu ID
                }
                break;
            }

            case PLOT_SEEDED:
            case PLOT_GROWING:
            {
                std::string statusMsg = "The seeds are growing...";

                AddGossipItemFor(player, GOSSIP_ICON_CHAT, statusMsg.c_str(), 0, plotId);

                if (plot.needsWatering)
                {
                    if (sTillersFarmMgr.HasItemInInventory(player, TillersFarmMgr::WATERING_CAN_ITEM))
                        AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Water this soil", 10, plotId);
                    else
                        AddGossipItemFor(player, GOSSIP_ICON_CHAT, "|cFFFF0000Need watering can to water|r", 0, plotId);
                }

                if (plot.hasPests)
                {
                    if (sTillersFarmMgr.HasItemInInventory(player, TillersFarmMgr::BUG_SPRAYER_ITEM))
                        AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Remove pests", 20, plotId);
                    else
                        AddGossipItemFor(player, GOSSIP_ICON_CHAT, "|cFFFF0000Need bug sprayer to remove pests|r", 0, plotId);
                }

                SendGossipMenuFor(player, 100003, go->GetGUID()); // "Growing Crop" gossip menu ID
                break;
            }

            case PLOT_NEEDS_WATER:
            {
                if (sTillersFarmMgr.HasItemInInventory(player, TillersFarmMgr::WATERING_CAN_ITEM))
                    AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Water the drying soil", 10, plotId);
                else
                    AddGossipItemFor(player, GOSSIP_ICON_CHAT, "|cFFFF0000Need watering can (item 79104)|r", 0, plotId);

                SendGossipMenuFor(player, 100004, go->GetGUID()); // "Drying Soil" gossip menu ID
                break;
            }

            case PLOT_NEEDS_PEST_CONTROL:
            {
                if (sTillersFarmMgr.HasItemInInventory(player, TillersFarmMgr::BUG_SPRAYER_ITEM))
                    AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Spray for pests", 20, plotId);
                else
                    AddGossipItemFor(player, GOSSIP_ICON_CHAT, "|cFFFF0000Need bug sprayer (item 80513)|r", 0, plotId);

                SendGossipMenuFor(player, 100005, go->GetGUID()); // "Pest Infestation" gossip menu ID
                break;
            }

            case PLOT_READY_TO_HARVEST:
            {
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Harvest your crop", 30, plotId);

                if (plot.needsWatering)
                    AddGossipItemFor(player, GOSSIP_ICON_CHAT, "|cFFFF0000Note: Soil is drying out - harvest soon|r", 0, plotId);

                SendGossipMenuFor(player, 100006, go->GetGUID()); // "Ready to Harvest" gossip menu ID
                break;
            }

            case PLOT_BROKEN:
            {
                if (sTillersFarmMgr.HasItemInInventory(player, TillersFarmMgr::SHOVEL_ITEM))
                    AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Repair this plot", 30, plotId);
                else
                    AddGossipItemFor(player, GOSSIP_ICON_CHAT, "|cFFFF0000Need shovel (item 89880) to repair|r", 0, plotId);

                SendGossipMenuFor(player, 100007, go->GetGUID()); // "Broken Plot" gossip menu ID
                break;
            }

            default:
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, "This plot appears to be in an unusual state.", 0, plotId);
                SendGossipMenuFor(player, 100008, go->GetGUID());
                break;
        }
    }

public:
    // Gossip action handlers called by the gossip menu system
    static void HandleSoilAction(Player* player, GameObject* go, uint32 sender, uint32 plotId)
    {
        if (!player || !go)
            return;

        if (!TillersFarmMgr::IsValidPlotId(plotId))
            return;

        switch (sender)
        {
            case 1: // Plant seeds - generic, need to know which seed type from action param
                break;
            case 2: // Wheat Seeds
                sTillersFarmMgr.PlantSeed(player, plotId, 79102);
                break;
            case 3: // Rice Seeds
                sTillersFarmMgr.PlantSeed(player, plotId, 110030);
                break;
            case 4: // Advanced Seeds
                sTillersFarmMgr.PlantSeed(player, plotId, 110031);
                break;
            case 10: // Water plot
                sTillersFarmMgr.WaterPlot(player, plotId);
                break;
            case 20: // Remove pests
                sTillersFarmMgr.RemovePests(player, plotId);
                break;
            case 30: // Harvest / Repair (same action ID for both states)
            {
                PlotMap& plots = sTillersFarmMgr.GetPlayerPlots(player);
                auto pit = plots.find(plotId);
                if (pit != plots.end())
                {
                    if (pit->second.state == PLOT_READY_TO_HARVEST)
                        sTillersFarmMgr.HarvestCrop(player, plotId);
                    else if (pit->second.state == PLOT_BROKEN)
                        sTillersFarmMgr.RepairPlot(player, plotId);
                }
                break;
            }
        }

        // Refresh gossip after action
        player->SEND_GOSSIP_MENU(100001, go->GetGUID());
    }
};

void AddTillersSoilScript()
{
    new tillers_soil_patch();
}
