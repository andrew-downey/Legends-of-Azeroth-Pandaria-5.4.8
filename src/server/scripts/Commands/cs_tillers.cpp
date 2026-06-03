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
#include "Chat.h"
#include "Player.h"
#include "../Pandaria/TillersFarmMgr.h"

class tillers_commandscript : public CommandScript
{
public:
    tillers_commandscript() : CommandScript("tillers_commandscript") { }

    std::vector<ChatCommand> GetCommands() const override
    {
        static std::vector<ChatCommand> commandTable =
        {
            { "tillers", SEC_ADMINISTRATOR, true,
            {
                { "status",   SEC_ADMINISTRATOR, true,  &HandleTillersStatusCommand,       "" },
                { "reset",    SEC_ADMINISTRATOR, false, &HandleTillersResetCommand,        "" },
                { "setplots", SEC_ADMINISTRATOR, true,  &HandleTillersSetPlotsCommand,     "" },
                { "grow",     SEC_ADMINISTRATOR, false, &HandleTillersGrowCommand,         "" },
            }},
        };

        return commandTable;
    }

private:
    /**
     * .tillers status [player]
     * Shows the target player's farm phase, unlocked plots, and each plot's state.
     */
    static bool HandleTillersStatusCommand(ChatHandler* handler, const char* /*args*/)
    {
        Player* target = handler->getSelectedPlayer();
        if (!target)
            target = handler->GetSession()->GetPlayer();

        uint32 guidLow = target->GetGUID().GetCounter();
       PlotMap& plots = sTillersFarmMgr.GetPlayerPlots(target);
         PlayerFarmState const& state = sTillersFarmMgr.GetPlayerState(guidLow);

handler->PSendSysMessage("|cFF00FF00=== Tillers Farm Status for %s ===|r", target->GetName().c_str());
        handler->PSendSysMessage("  Farm State:       %u (%s)", state.farmState, GetFarmStateName(state.farmState).c_str());
        handler->PSendSysMessage("  Plots Unlocked:   %u / 16", state.plotsUnlocked);

        handler->PSendSysMessage("");
        handler->PSendSysMessage("  Plot States:");

        for (auto const& [plotId, plot] : plots)
        {
            if (plotId >= 8)
                continue;

            bool unlocked = (plotId < state.plotsUnlocked);
            std::string stateName = GetPlotStateName(plot.state);

            handler->PSendSysMessage("    Plot %u: |cFF%s%s|r", plotId,
                unlocked ? "00FF00" : "9D9D9D",
                stateName.c_str());

            if (unlocked && plot.seedEntry > 0)
            {
                handler->PSendSysMessage("      Seed:           %u", plot.seedEntry);

                if (plot.needsWatering)
                    handler->PSendSysMessage("      |cFFFF0000Needs Watering|r");

                if (plot.hasPests)
                    handler->PSendSysMessage("      |cFFFF0000Has Pests|r");

                if (plot.maturityTimestamp > 0)
                {
                    time_t now = time(nullptr);
                    long remaining = static_cast<long>(plot.maturityTimestamp - now);
                    if (remaining > 0)
                        handler->PSendSysMessage("      Matures in:     %lds", remaining);
                    else
                        handler->PSendSysMessage("      |cFF00FF00Ready to harvest!|r");
                }
            }

            if (!unlocked)
                handler->PSendSysMessage("      (Locked - not yet available)");
        }

        return true;
    }

    /**
     * .tillers reset [player]
     * Resets the target player's farm to initial state (all plots empty, 4 plots).
     */
    static bool HandleTillersResetCommand(ChatHandler* handler, const char* /*args*/)
    {
        Player* target = handler->getSelectedPlayer();
        if (!target)
            target = handler->GetSession()->GetPlayer();

        uint32 guidLow = target->GetGUID().GetCounter();

        // Clear player state and plots via thread-safe method
        sTillersFarmMgr.ResetPlayerFarm(guidLow);

        handler->PSendSysMessage("|cFF00FF00Tillers farm reset for %s.|r", target->GetName().c_str());
        handler->PSendSysMessage("  Farm has been reset to initial state with 4 plots and all plots empty.");

        return true;
    }

    /**
     * .tillers setplots <player> <plots>
     * Sets a player's farm plots directly (0/4/8/12/16).
     * 0 = reset to initial state (4 plots, all empty).
     */
    static bool HandleTillersSetPlotsCommand(ChatHandler* handler, const char* args)
    {
        char* playerStr = nullptr;
        char* plotsStr = nullptr;
        handler->extractOptFirstArg((char*)args, &playerStr, &plotsStr);

        if (!plotsStr)
        {
            handler->PSendSysMessage("You must specify a plots value (0/4/8/12/16).");
            return false;
        }

        uint8 newPlots = static_cast<uint8>(atoi(plotsStr));
        if (newPlots != 0 && newPlots != 4 && newPlots != 8 && newPlots != 12 && newPlots != 16)
        {
            handler->PSendSysMessage("Invalid plots. Must be one of: 0, 4, 8, 12, 16.");
            return false;
        }

        Player* target = handler->getSelectedPlayer();
        if (!target)
            target = handler->GetSession()->GetPlayer();

        uint32 guidLow = target->GetGUID().GetCounter();

        // Get or create state
        PlotMap& plots = sTillersFarmMgr.GetPlayerPlots(target);
        PlayerFarmState& state = sTillersFarmMgr.GetPlayerState(guidLow);

        uint8 oldPlots = state.plotsUnlocked;
        uint8 oldState = state.farmState;

        if (newPlots == 0)
        {
            // Reset: clear everything
            sTillersFarmMgr.ResetPlayerFarm(guidLow);
            state = sTillersFarmMgr.GetPlayerState(guidLow);
            plots = sTillersFarmMgr.GetPlayerPlots(target);
            newPlots = 4;
        }
        else
        {
            state.farmState = GetFarmStateForPlots(newPlots);
            state.plotsUnlocked = newPlots;
        }

        // Rebuild farm from state via SpawnPlayerFarm
        sTillersFarmMgr.SpawnPlayerFarm(target);

        handler->PSendSysMessage("|cFF00FF00Tillers farm plots set for %s.|r", target->GetName().c_str());
        handler->PSendSysMessage("  Plots: %u -> %u (farmState: %u -> %u)",
            oldPlots, newPlots, oldState, state.farmState);

        return true;
    }

    /**
     * .tillers grow [player]
     * Force all growing crops on the target player's farm to mature immediately.
     */
    static bool HandleTillersGrowCommand(ChatHandler* handler, const char* /*args*/)
    {
        Player* target = handler->getSelectedPlayer();
        if (!target)
        {
            handler->PSendSysMessage("No player selected. Select a player or target yourself.");
            return false;
        }

        sTillersFarmMgr.ForceGrowFarm(target);

        handler->PSendSysMessage("|cFF00FF00All growing crops on %s's farm have matured.|r", target->GetName().c_str());
        return true;
    }

    static std::string GetPlotStateName(FarmPlotState state)
    {
        switch (state)
        {
            case PLOT_EMPTY:              return "Empty";
            case PLOT_SOIL_PREPARED:      return "Soil Prepared";
            case PLOT_SEEDED:             return "Seeded";
            case PLOT_GROWING:            return "Growing";
            case PLOT_NEEDS_WATER:        return "Needs Water";
            case PLOT_NEEDS_PEST_CONTROL: return "Needs Pest Control";
            case PLOT_READY_TO_HARVEST:   return "Ready to Harvest";
            case PLOT_BROKEN:             return "Broken";
            default:                      return "Unknown";
        }
    }

    static std::string GetFarmStateName(uint8 farmState)
    {
        switch (farmState)
        {
            case FARM_STATE_FULL:         return "Full (4 plots)";
            case FARM_STATE_WEEDS_CLEARED: return "Weeds Cleared (8 plots)";
            case FARM_STATE_WAGON_CLEARED: return "Wagon Cleared (12 plots)";
            case FARM_STATE_ALL_CLEARED:  return "All Cleared (16 plots)";
            default:                      return "Unknown";
        }
    }
};

void AddTillersCommands()
{
    new tillers_commandscript();
}
