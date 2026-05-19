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

#ifndef BATTLE_PET_TRAINER_MGR
#define BATTLE_PET_TRAINER_MGR

#include "Common.h"
#include "BattlePet.h"

struct TrainerBattlePet
{
    uint32 Species;
    uint8  Level;
    uint8  Quality;
    uint8  Breed;
};

typedef std::vector<TrainerBattlePet> TrainerBattlePetStore;
//            trainer entry   team
typedef std::map<uint32, TrainerBattlePetStore> BattlePetTrainerContainer;

class PetBattleTeam;

// handles teams of battle pets assigned to NPC trainers
class BattlePetTrainerMgr
{
public:

    static BattlePetTrainerMgr* instance()
    {
        static BattlePetTrainerMgr _instance;
        return &_instance;
    }

    void LoadBattleTrainerPets();

    TrainerBattlePetStore const* GetTrainerTeam(uint32 entry) const;

    void CreateTrainerPets(TrainerBattlePetStore const& team, PetBattleTeam* petBattleTeam) const;

private:
    BattlePetTrainerContainer m_trainerPets;
};

#define sBattlePetTrainerMgr BattlePetTrainerMgr::instance()

#endif
