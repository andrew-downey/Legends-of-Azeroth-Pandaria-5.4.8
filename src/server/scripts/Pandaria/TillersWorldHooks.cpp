#include "ScriptMgr.h"
#include "Player.h"
#include "TillersFarmMgr.h"
#include "World.h"

class tillers_world_hooks : public WorldScript
{
public:
    tillers_world_hooks() : WorldScript("tillers_world_hooks") { }

    void OnUpdate(uint32 /*diff*/) override
    {
        time_t const nextReset = sWorld->GetNextDailyQuestsResetTime();

        // Process exactly once per daily reset: the first tick after
        // m_NextDailyQuestReset is advanced (incremented by ResetDailyQuests).
        // On server startup this runs once as a catch-up — harmless because
        // no SEEDED/GROWING crops will have timestamps in the past unless
        // a reset was genuinely missed.
        if (nextReset <= _lastNextReset)
            return;

        _lastNextReset = nextReset;

        TC_LOG_INFO("scripts", "TillersWorldHooks: Daily reset processing — maturing all crops (next reset at %lu)",
                    static_cast<unsigned long>(nextReset));

        // Batch advance all SEEDED/GROWING crops past maturity
        CharacterDatabase.PExecute(
            "UPDATE player_farm_plots SET state = %u, maturity_timestamp = 0 "
            "WHERE (state = %u OR state = %u) AND maturity_timestamp > 0 AND maturity_timestamp <= UNIX_TIMESTAMP()",
            static_cast<uint8>(PLOT_READY_TO_HARVEST),
            static_cast<uint8>(PLOT_SEEDED),
            static_cast<uint8>(PLOT_GROWING));

        // Refresh farm state for players currently in Valley of the Four Winds
        SessionMap const& sessions = sWorld->GetAllSessions();
        for (auto&& session : sessions)
        {
            if (Player* player = session.second->GetPlayer())
            {
                if (player->GetZoneId() == TillersFarmMgr::VFW_ZONE_ID &&
                    sTillersFarmMgr.IsPlayerFarmSpawned(player))
                {
                    sTillersFarmMgr.SpawnPlayerFarm(player);
                }
            }
        }
    }

private:
    time_t _lastNextReset = 0;
};

void AddTillersWorldHooks()
{
    new tillers_world_hooks();
}
