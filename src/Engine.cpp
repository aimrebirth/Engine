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

#include <Polygon4/Engine.h>

#include <Polygon4/DataManager/Storage.h>
#include <Polygon4/Modification.h>
#include <Polygon4/Settings.h>

#include "tools/Logger.h"
DECLARE_STATIC_LOGGER(logger, "engine");

#define DB_FILENAME "db.sqlite"

namespace polygon4
{

IEngine *getEngine(IEngine *engine)
{
    static IEngine *gEngine = nullptr;
    if (engine)
        gEngine = engine;
    return gEngine;
}

IEngine::~IEngine()
{
}

Engine::Engine(const String &modificationsDirectory)
    : IEngine()
{
    LOG_DEBUG(logger, "Initializing engine");

    // turn on this feature for all engines
    //  (data managers do not use it)
    detail::IObjectBase::replaceable = true;

    LOG_DEBUG(logger, "Modifications Directory: " << modificationsDirectory.toString());
    getSettings().modsDir = modificationsDirectory;
    reloadStorage();
}

bool Engine::reloadStorage()
{
    LOG_DEBUG(logger, "Reloading storage");
    try
    {
        auto s = initStorage(getSettings().modsDir.toString() + "/" DB_FILENAME);
        LOG_DEBUG(logger, "Loading data");
        s->load();
        storage = s;
        LOG_DEBUG(logger, "Storage loaded");
        LOG_DEBUG(logger, "Storage ptr is: " << storage);
    }
    catch (std::exception &e)
    {
        LOG_ERROR(logger, "Cannot load storage: " << e.what());
        return false;
    }
    return true;
}

bool Engine::reloadMods()
{
    if (!reloadStorage())
        return false;
    initChildren();
    return true;
}

void Engine::spawnCurrentPlayer()
{
    if (!currentModification)
        return;
    currentModification->spawnCurrentPlayer();
}

} // namespace polygon4
