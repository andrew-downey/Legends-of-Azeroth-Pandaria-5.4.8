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

#include "BattlePetTrainerMgr.h"
#include "Database/DatabaseEnv.h"
#include "DB2Stores.h"
#include "PetBattle.h"
#include "BattlePet.h"

void BattlePetTrainerMgr::LoadBattleTrainerPets()
{
    uint32 oldMSTime = getMSTime();

    m_trainerPets.clear(); // just in case of reload

    QueryResult result = WorldDatabase.Query("SELECT entry, species, level, quality, breed FROM battle_pet_trainer");
    if (!result)
    {
        TC_LOG_INFO("server.loading", ">> Loaded 0 battle pet trainers. DB table `battle_pet_trainer` is empty.");
        return;
    }

    uint32 count = 0;
    do
    {
        Field* fields = result->Fetch();
        uint32 entry = fields[0].GetUInt32();
        TrainerBattlePet pet;
        pet.Species = fields[1].GetUInt32();
        pet.Level   = fields[2].GetUInt8();
        pet.Quality = fields[3].GetUInt8();
        pet.Breed   = fields[4].GetUInt8();

        m_trainerPets[entry].push_back(pet);
        ++count;
    } while (result->NextRow());

    TC_LOG_INFO("server.loading", ">> Loaded %u battle pet trainer pets in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
}

TrainerBattlePetStore const* BattlePetTrainerMgr::GetTrainerTeam(uint32 entry) const
{
    auto itr = m_trainerPets.find(entry);
    if (itr != m_trainerPets.end())
        return &itr->second;
    return nullptr;
}

void BattlePetTrainerMgr::CreateTrainerPets(TrainerBattlePetStore const& team, PetBattleTeam* petBattleTeam) const
{
    for (auto const& trainerPet : team)
    {
        auto speciesEntry = sBattlePetSpeciesStore.LookupEntry(trainerPet.Species);
        ASSERT(speciesEntry);

        auto battlePet = new BattlePet(0, trainerPet.Species, speciesEntry->FamilyId, trainerPet.Level, trainerPet.Quality, trainerPet.Breed, nullptr);
        battlePet->SetBattleInfo(PetBattleTeamIndex(petBattleTeam->GetTeamIndex()), petBattleTeam->ConvertToGlobalIndex(petBattleTeam->BattlePets.size()));
        battlePet->InitialiseAbilities(false);

        petBattleTeam->BattlePets.push_back(battlePet);
    }

    if (!petBattleTeam->BattlePets.empty())
        petBattleTeam->SetActivePet(petBattleTeam->GetPet(0));
}
