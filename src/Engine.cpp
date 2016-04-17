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

#include "Common.h"

#include "tools/Logger.h"
DECLARE_STATIC_LOGGER(logger, "engine");

#define AUTOSAVE_NAME "autosave"
#define QUICKSAVE_NAME "quicksave"
#define DB_EXT ".sqlite"
#define SAVEGAME_EXT DB_EXT

#define DB_FILENAME "db" DB_EXT

namespace polygon4
{

Engine *getEngine(Engine *engine)
{
    static Engine *gEngine = nullptr;
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

Engine::~Engine()
{
}

bool Engine::reloadMods()
{
    if (!reloadStorage())
        return false;
    initChildren();
    return true;
}

std::shared_ptr<Storage> loadStorage(const path &p)
{
    auto s = initStorage(p.string());
    LOG_DEBUG(logger, "Loading data");
    s->load();
    LOG_DEBUG(logger, "Storage loaded");
    LOG_DEBUG(logger, "Storage ptr is: " << s);
    return s;
}

bool Engine::reloadStorage()
{
    LOG_DEBUG(logger, "Reloading storage");
    try
    {
        storage = loadStorage(path(getSettings().dirs.mods) / DB_FILENAME);
    }
    catch (std::exception &e)
    {
        LOG_ERROR(logger, "Cannot load storage: " << e.what());
        return false;
    }

    // get string maps
    messages = storage->messages.get_key_map(&polygon4::detail::Message::text_id);
    strings = storage->strings.get_key_map(&polygon4::detail::String::text_id);

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
        if (fp.extension() != SAVEGAME_EXT) // filter saves only (sqlite db)
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

path SaveName2path(const String &fn)
{
    path p = path(getSettings().dirs.saves) / (fn + SAVEGAME_EXT);
    return p;
}

bool Engine::_save(const String &fn) const
{
    std::lock_guard<std::mutex> lock(m_save);

    if (fn.empty())
        return false;
    if (!currentModification)
        return false;

    auto p = SaveName2path(fn);

    IdPtr<detail::SaveGame> s;
    if (storage->saveGames.empty())
        s = storage->addSaveGame();
    else
        s = storage->saveGames[1];

    s->modification = currentModification;
    s->playtime = getSettings().playtime;

    bool e = fs::exists(p);
    auto old_p = p;
    if (e)
        p = SaveName2path(fs::unique_path().string());
    storage->save(p.string());
    if (e)
    {
        fs::copy_file(p, old_p, fs::copy_option::overwrite_if_exists);
        fs::remove(p);
    }
    return true;
}

bool Engine::save(const String &fn) const
{
    if (fn == AUTOSAVE_NAME || fn == QUICKSAVE_NAME)
        return false;
    return _save(fn);
}

bool Engine::saveAuto() const
{
    return _save(AUTOSAVE_NAME);
}

bool Engine::saveQuick() const
{
    std::unique_lock<std::mutex> lock(m_save, std::try_to_lock);
    if (!lock)
        return false; // deny multiple quicksaves
    return _save(QUICKSAVE_NAME);
}

bool Engine::load(const String &fn)
{
    if (fn.empty())
        return false;
    auto p = SaveName2path(fn);
    LOG_DEBUG(logger, "Loading savegame: " << fn << ", " << p.string());
    if (!fs::exists(p))
    {
        LOG_ERROR(logger, "No such savegame: " << fn << ", " << p.string());
        return false;
    }
    try
    {
        auto s = loadStorage(p);
        if (s->saveGames.empty())
        {
            LOG_ERROR(logger, "No mod started in this savegame: " << fn << ", " << p.string());
            return false;
        }

        storage = s;
        initChildren();

        auto sg = storage->saveGames[1];
        getSettings().playtime = sg->playtime;
        sg->modification->newGame();
    }
    catch (std::exception &e)
    {
        LOG_ERROR(logger, "Cannot load storage: " << e.what());
        return false;
    }
    return true;
}

void Engine::deleteSaveGame(const String &fn) const
{
    if (fn.empty())
        return;
    auto p = SaveName2path(fn);
    if (fs::exists(p))
        fs::remove(p);
}

} // namespace polygon4
