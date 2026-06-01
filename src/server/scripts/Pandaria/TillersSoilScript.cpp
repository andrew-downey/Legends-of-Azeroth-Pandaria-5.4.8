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
#include "GameObject.h"
#include "Spell.h"
#include "SpellInfo.h"
#include "TillersFarmMgr.h"

// Spell IDs for farm interactions
static uint32 const SPELL_TILLING            = 114431;  // Tilling — cast on empty soil
static uint32 const SPELL_HARVEST            = 186315;  // Harvest — cast on ready crop
static uint32 const SPELL_WATERING_CAN       = 79104;  // Rusty Watering Can — cast on growing crop
static uint32 const SPELL_BUG_SPRAYER        = 80513;  // Vintage Bug Sprayer — cast on pest-infested crop
static uint32 const SPELL_SEED_WHEAT         = 79102;  // Wheat Seeds — cast on tilled soil
static uint32 const SPELL_SEED_RICE          = 110030; // Rice Seeds — cast on tilled soil
static uint32 const SPELL_SEED_ADVANCED      = 110031; // Advanced Seeds — cast on tilled soil

// Minigame crop spell IDs
static uint32 const SPELL_RUNTY_INTERACT     = 186316;  // Runty — right-click triggers buff
static uint32 const SPELL_WILD_SMACK         = 186317;  // Wild — right-click triggers smack minigame
static uint32 const SPELL_SMOOTHERD_SPAM     = 186318;  // Smothered — right-click triggers spam minigame
static uint32 const SPELL_TANGLED_RUN        = 186319;  // Tangled — right-click triggers run minigame

class tillers_soil_patch : public GameObjectScript
{
public:
    tillers_soil_patch() : GameObjectScript("tillers_soil_patch") { }

    bool OnDummyEffect(Unit* caster, uint32 spellId, SpellEffIndex /*effIndex*/, GameObject* go) override
    {
        if (!caster || !go)
            return false;

        Player* player = caster->IsPlayer() ? caster->ToPlayer() : (caster->GetOwner() ? caster->GetOwner()->ToPlayer() : nullptr);

        if (!player || !player->IsInWorld())
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

        // Only process spells from players in the correct phase
        uint32 playerPhaseMask = player->GetPhaseMask();
        uint32 goPhaseMask = go->GetPhaseMask();
        if ((playerPhaseMask & goPhaseMask) == 0)
            return false;

        PlotMap& plots = sTillersFarmMgr.GetPlayerPlots(player);
        auto pit = plots.find(plotId);
        if (pit == plots.end())
            return false;

        FarmPlotData& plot = pit->second;
        PlayerFarmState& state = sTillersFarmMgr.GetPlayerState(player->GetGUID().GetCounter());

        bool plotsUnlocked = (plotId < GetPlotsUnlockedForPhase(state.farmPhase));
        if (!plotsUnlocked)
        {
            player->GetSession()->SendNotification("This plot is not yet available.");
            return false;
        }

        // Handle tilling: empty soil -> tilled soil
        if (spellId == SPELL_TILLING)
        {
            if (plot.state != PLOT_EMPTY && plot.state != PLOT_SOIL_PREPARED)
            {
                player->GetSession()->SendNotification("This soil is already prepared.");
                return false;
            }

            plot.state = PLOT_SOIL_PREPARED;

            TC_LOG_INFO("scripts", "TillersSoilScript: Player %u tilled plot %u",
                        player->GetGUID().GetCounter(), plotId);
            return true;
        }

        // Handle planting: tilled soil -> seeded
        if (spellId == SPELL_SEED_WHEAT || spellId == SPELL_SEED_RICE || spellId == SPELL_SEED_ADVANCED)
        {
            if (plot.state != PLOT_SOIL_PREPARED)
            {
                player->GetSession()->SendNotification("This soil needs to be tilled first.");
                return false;
            }

            // Validate plot is unlocked
            uint8 plotsUnlockedCount = GetPlotsUnlockedForPhase(state.farmPhase);
            if (plotId >= plotsUnlockedCount)
            {
                player->GetSession()->SendNotification("This plot is not yet unlocked.");
                return false;
            }

            // Consume seed from inventory
            uint32 seedEntry = spellId == SPELL_SEED_WHEAT ? SPELL_SEED_WHEAT :
                               spellId == SPELL_SEED_RICE ? SPELL_SEED_RICE : SPELL_SEED_ADVANCED;

            if (!player->HasItemCount(seedEntry, 1))
            {
                player->GetSession()->SendNotification("You do not have any seeds.");
                return false;
            }

            // Plant the seed
            if (sTillersFarmMgr.PlantSeed(player, plotId, seedEntry))
            {
                plot.state = PLOT_SEEDED;
                plot.seedEntry = seedEntry;
                plot.needsWatering = false;
                plot.hasPests = false;
                plot.maturityTimestamp = sTillersFarmMgr.GetMaturityTime(seedEntry, plotId);

                TC_LOG_INFO("scripts", "TillersSoilScript: Player %u planted seed %u on plot %u",
                            player->GetGUID().GetCounter(), seedEntry, plotId);
            }
            return true;
        }

        // Handle harvesting: ready crop -> soil prepared
        if (spellId == SPELL_HARVEST)
        {
            if (plot.state != PLOT_READY_TO_HARVEST)
            {
                player->GetSession()->SendNotification("This crop is not ready to harvest yet.");
                return false;
            }

            if (sTillersFarmMgr.HarvestCrop(player, plotId))
            {
                plot.state = PLOT_SOIL_PREPARED;
                plot.seedEntry = 0;
                plot.needsWatering = false;
                plot.hasPests = false;
                plot.maturityTimestamp = 0;

                TC_LOG_INFO("scripts", "TillersSoilScript: Player %u harvested plot %u",
                            player->GetGUID().GetCounter(), plotId);
            }
            return true;
        }

        // Handle watering: growing crop -> watered
        if (spellId == SPELL_WATERING_CAN)
        {
            if (plot.state != PLOT_NEEDS_WATER && plot.state != PLOT_SEEDED && plot.state != PLOT_GROWING)
            {
                player->GetSession()->SendNotification("This crop does not need water.");
                return false;
            }

            if (!sTillersFarmMgr.HasItemInInventory(player, TillersFarmMgr::WATERING_CAN_ITEM))
            {
                player->GetSession()->SendNotification("You need a rusty watering can to water crops.");
                return false;
            }

            sTillersFarmMgr.WaterPlot(player, plotId);
            plot.needsWatering = false;

            TC_LOG_INFO("scripts", "TillersSoilScript: Player %u watered plot %u",
                        player->GetGUID().GetCounter(), plotId);
            return true;
        }

        // Handle pest control: pest-infested crop -> clean
        if (spellId == SPELL_BUG_SPRAYER)
        {
            if (!plot.hasPests)
            {
                player->GetSession()->SendNotification("This crop has no pests.");
                return false;
            }

            if (!sTillersFarmMgr.HasItemInInventory(player, TillersFarmMgr::BUG_SPRAYER_ITEM))
            {
                player->GetSession()->SendNotification("You need a vintage bug sprayer to remove pests.");
                return false;
            }

            sTillersFarmMgr.RemovePests(player, plotId);
            plot.hasPests = false;

            TC_LOG_INFO("scripts", "TillersSoilScript: Player %u removed pests from plot %u",
                        player->GetGUID().GetCounter(), plotId);
            return true;
        }

        // Handle minigame crops: runty, wild, smothered, tangled
        // These trigger special interaction spells that require follow-up actions
        if (spellId == SPELL_RUNTY_INTERACT)
        {
            if (plot.state != PLOT_READY_TO_HARVEST)
            {
                player->GetSession()->SendNotification("This crop is not ready to harvest yet.");
                return false;
            }

            // Runty: player must press spacebar while mounted to catch
            // This is handled by the client — we just validate and let the
            // minigame proceed. The client sends a separate confirmation.
            player->GetSession()->SendNotification("Right-click and jump to catch the Runty!");
            return true;
        }

        if (spellId == SPELL_WILD_SMACK)
        {
            if (plot.state != PLOT_READY_TO_HARVEST)
            {
                player->GetSession()->SendNotification("This crop is not ready to harvest yet.");
                return false;
            }

            player->GetSession()->SendNotification("Right-click and spam ability to smack the Wild crop!");
            return true;
        }

        if (spellId == SPELL_SMOOTHERD_SPAM)
        {
            if (plot.state != PLOT_READY_TO_HARVEST)
            {
                player->GetSession()->SendNotification("This crop is not ready to harvest yet.");
                return false;
            }

            player->GetSession()->SendNotification("Right-click and keep pressing 1 to smother the crop!");
            return true;
        }

        if (spellId == SPELL_TANGLED_RUN)
        {
            if (plot.state != PLOT_READY_TO_HARVEST)
            {
                player->GetSession()->SendNotification("This crop is not ready to harvest yet.");
                return false;
            }

            player->GetSession()->SendNotification("Right-click and press 2 for Vine Slam to clear the Tangled crop!");
            return true;
        }

        return false;
    }
};

void AddTillersSoilScript()
{
    new tillers_soil_patch();
}
