/*
 * Polygon-4 Engine
 * Copyright (C) 2015 lzwdgc
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <Polygon4/Modification.h>

#include <Polygon4/DataManager/Types.h>
#include <Polygon4/Engine.h>
#include <Polygon4/Settings.h>

#include "Script.h"

#include <tools/Logger.h>
DECLARE_STATIC_LOGGER(logger, "mods");

namespace polygon4
{

Modification::Modification(const Base &rhs)
    : Base(rhs)
{
}

Modification::~Modification()
{
}

bool Modification::newGame()
{
    if (directory.empty())
    {
        LOG_ERROR(logger, "Game directory is not set!");
        return false;
    }
    if (script_language == ScriptLanguage::None)
    {
        LOG_ERROR(logger, "Script language is not set!");
        return false;
    }

    try
    {
        const auto &path = getSettings().modsDir;
        scriptEngine = std::make_unique<ScriptEngine>(path + directory, script_language);

        detail::ModificationPlayer *this_player = nullptr;
        for (auto &p : players)
        {
            if (p->player == player)
            {
                this_player = p;
                break;
            }
        }
        if (!this_player)
        {
            LOG_ERROR(logger, "Cannot find a local player for this modification");
            return false;
        }
        auto &pmap = this_player->mechanoid->map;
        auto i = std::find_if(maps.begin(), maps.end(), [&pmap](const auto &map)
        {
            return map->map == pmap;
        });
        if (i == maps.end())
        {
            LOG_ERROR(logger, "Cannot find map: " << pmap->map->resource.toString());
            return false;
        }

        if (!pmap->map->loadLevel())
            return false;
        currentMap = pmap->map;

        getEngine()->HideMainMenu();
        getEngine()->LoadLevelObjects = [this, pmap]()
        {
            pmap->map->loadObjects();
            for (auto &b : pmap->buildings)
            {
                if (b->building)
                    b->building->setModificationMapBuilding(b);
            }
            spawnMechanoids();
        };

        // action will proceed on UE4 side
    }
    catch (std::exception &e)
    {
        LOG_ERROR(logger, "Cannot start game: " << e.what());
        return false;
    }
    getEngine()->setCurrentModification(this);
    return true;
}

bool Modification::loadGame(const String &filename)
{
    return false;
}

bool Modification::operator<(const Modification &rhs) const
{
    return directory < rhs.directory;
}

void Modification::spawnMechanoids()
{
    if (!currentMap)
        return;

    // add players to mechanoids
    for (auto &p : players)
    {
        if (p->mechanoid)
        {
            p->mechanoid->setPlayer(p);
            currentPlayer = p;
        }
        // for now we allow only one local player
        break;
    }
    for (auto &m : mechanoids)
    {
        if (m->map->map == *currentMap)
            m->spawn();
    }
}

void Modification::spawnCurrentPlayer()
{
    if (!currentPlayer)
        return;
    currentPlayer->mechanoid->building.clear();
    currentPlayer->mechanoid->spawn();
}

} // namespace polygon4
