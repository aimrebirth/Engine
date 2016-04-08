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

#include <boost/range.hpp>

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

Engine::Engine(const String &gameDirectory)
    : IEngine()
{
    LOG_DEBUG(logger, "Initializing engine");

    // turn on this feature for all engines
    //  (data managers do not use it)
    detail::IObjectBase::replaceable = true;

    LOG_DEBUG(logger, "Game Directory: " << gameDirectory.toString());
    getSettings().dirs.setGameDir(gameDirectory);
    reloadStorage();
}

bool Engine::reloadStorage()
{
    LOG_DEBUG(logger, "Reloading storage");
    try
    {
        auto s = initStorage((getSettings().dirs.mods / DB_FILENAME).string());
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

SavedGames Engine::getSavedGames(bool save) const
{
    SavedGames games;
    path p = getSettings().dirs.saves;
    if (!fs::exists(p))
        return games;
    for (auto &f : boost::make_iterator_range(fs::directory_iterator(p), {})) // non recursive
    {
        if (!fs::is_regular_file(f)) // filter files only
            continue;
        auto fp = f.path();
        if (fp.extension() != ".sqlite") // filter saves only (sqlite db)
            continue;
        auto fn = fp.filename().stem().string();
        if (save && (fn == AUTOSAVE_NAME || fn == QUICKSAVE_NAME)) // if we want to save game, disallow reserved words
            continue;
        games.push_back(fn);
    }
    return games;
}

void Engine::spawnCurrentPlayer()
{
    if (!currentModification)
        return;
    currentModification->spawnCurrentPlayer();
}

} // namespace polygon4
