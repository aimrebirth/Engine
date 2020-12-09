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

#include <Polygon4/DataManager/Database.h>
#include <Polygon4/DataManager/Storage.h>
#include <Polygon4/Modification.h>

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
    LOG_DEBUG(logger, "Game Directory: " << gameDirectory.toString());

    // turn on this feature for all engines
    //  (data managers do not use it)
    detail::IObjectBase::replaceable = true;

    // initial settings
    getSettings().dirs.setGameDir(gameDirectory);

    reloadStorage();
}

Engine::~Engine()
{
}

Settings &Engine::getSettings()
{
    if (!getStorage())
        return settings;
    return getStorage()->getSettings();
}

const Settings &Engine::getSettings() const
{
    if (!getStorage())
        return settings;
    return getStorage()->getSettings();
}

void Engine::backupSettings()
{
    settings = getSettings();
}

void Engine::restoreSettings()
{
    getSettings() = settings;
}

bool Engine::reloadMods()
{
    if (!reloadStorage())
        return false;
    initChildren();
    return true;
}

static auto loadStorage(Database &db)
{
    auto s = initStorage();
    LOG_DEBUG(logger, "Loading data to storage");
    s->load(db, {});
    LOG_TRACE(logger, "Loaded data to storage");
    LOG_TRACE(logger, "Storage ptr is: " << s);
    return s;
}

bool Engine::reloadStorage()
{
    LOG_DEBUG(logger, "Reloading storage");

    backupSettings();

    try
    {
        auto p = path(getSettings().dirs.mods.c_str()) / DB_FILENAME;
        auto database = std::make_unique<Database>(p);
        storage = loadStorage(*database);
    }
    catch (std::exception &e)
    {
        LOG_ERROR(logger, "Cannot load storage: " << e.what());
        return false;
    }

    restoreSettings();
    postLoadStorage();

    return true;
}

void Engine::postLoadStorage()
{
    // get string maps
    messages = storage->messages.get_key_map(&polygon4::detail::Message::text_id);
    strings = storage->strings.get_key_map(&polygon4::detail::String::text_id);
    buildings = storage->buildings.get_key_map(&polygon4::detail::Building::text_id);

    // items
#define ADD_ITEMS(array, type)                                                 \
    do                                                                         \
    {                                                                          \
        auto v = storage->array.get_key_map(&polygon4::detail::type::text_id); \
        items.insert(v.begin(), v.end());                                      \
    } while (0)

    items.clear();
    ADD_ITEMS(equipments, Equipment);
    ADD_ITEMS(gliders, Glider);
    ADD_ITEMS(weapons, Weapon);
    ADD_ITEMS(projectiles, Projectile);
    ADD_ITEMS(goods, Good);
    ADD_ITEMS(modificators, Modificator);

    // general container for gettings descriptions
#define MERGE_OBJECTS(a) objects.insert(a.begin(), a.end())

    objects.clear();
    MERGE_OBJECTS(messages);
    MERGE_OBJECTS(strings);
    MERGE_OBJECTS(buildings);
    MERGE_OBJECTS(items);
}

SavedGames Engine::getSavedGames(bool save) const
{
    SavedGames games;
    path p = getSettings().dirs.saves.c_str();
    if (!fs::exists(p))
        return games;
    for (auto &f : fs::directory_iterator(p)) // non recursive
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
    path p = path(getEngine()->getSettings().dirs.saves.c_str()) / ((std::string)fn + SAVEGAME_EXT);
    return p;
}

bool Engine::_save(const String &fn) const
{
    std::lock_guard<std::mutex> lock(m_save);

    if (fn.empty() || !currentModification)
        return false;

    auto p = SaveName2path(fn);

    LOG_DEBUG(logger, "Saving game: " << fn.toString() << ", " << p.string());

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
        p = SaveName2path(unique_path().string());
    auto database = std::make_unique<Database>(p);
    storage->create(*database);
    storage->save(*database, {});
    database->save();
    if (e)
    {
        fs::copy_file(p, old_p, fs::copy_options::overwrite_existing);
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

    LOG_DEBUG(logger, "Loading savegame: " << fn.toString() << ", " << p.string());

    if (!fs::exists(p))
    {
        LOG_ERROR(logger, "No such savegame: " << fn.toString() << ", " << p.string());
        return false;
    }
    try
    {
        auto database = std::make_unique<Database>(p);
        auto s = loadStorage(*database);
        if (s->saveGames.empty())
        {
            LOG_ERROR(logger, "No mod started in this savegame: " << fn.toString() << ", " << p.string());
            return false;
        }

        backupSettings();

        // storage is not loaded into the engine
        // we load it
        storage = std::move(s);

        restoreSettings();
        postLoadStorage();
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
