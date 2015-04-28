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

#include <Polygon4/Mods.h>

#include <boost/filesystem.hpp>

#include "Game.h"
#include "Script.h"

#include <tools/Logger.h>
DECLARE_STATIC_LOGGER(logger, "mods");

namespace polygon4
{

Mod::~Mod()
{
    stopGame();
}
    
bool Mod::newGame()
{
    if (state == GameState::Bad)
    {
        LOG_ERROR(logger, "Game is in bad state!");
        return false;
    }
    if (state != GameState::None)
    {
        LOG_ERROR(logger, "Game is not in initial state!");
        return false;
    }
    try
    {
        //db = std::make_shared<Database>(to_string(databaseName.wstring()));
        //auto script = Script::createScript(getScriptName(path, mainScriptName), scriptLanguage);
        //game = std::make_shared<Game>(db, script);
        //game->run();
    }
    catch (std::exception e)
    {
        LOG_ERROR(logger, "Cannot start game: " << e.what());
        return false;
    }
    return true;
}

bool Mod::loadGame(String filename)
{
    return false;
}

bool Mod::stopGame()
{
    return true;
}

bool Mod::isOk() const
{
    return state != GameState::Bad;
}

bool Mod::operator<(const Mod &rhs) const
{
    return false;
    /*if (name.wstring() != rhs.name.wstring())
        return name.wstring() < rhs.name.wstring();
    return dir.wstring() < rhs.dir.wstring();*/
}

const Mods &enumerateMods(String dd, String cd)
{
    namespace bf = boost::filesystem;

    std::wstring dataDirectory = dd.wstring();
    dataDirectory = bf::absolute(dataDirectory).normalize().wstring();
    std::wstring contentDirectory = cd.wstring();
    contentDirectory = bf::absolute(contentDirectory).normalize().wstring();

    static Mods mods;
    mods.clear();
    
    /*std::string db_file = (bf::absolute(dataDirectory) / DB_FILENAME).string();
    std::shared_ptr<Database> db = std::make_shared<Database>(db_file);
    static engine::Storage storage(db);
    storage.clear();
    try
    {
        storage.load();
    }
    catch (std::exception e)
    {
        LOG_ERROR(logger, e.what());
        return mods;
    }*/

    if (!bf::exists(dataDirectory))
    {
        LOG_ERROR(logger, "Data directory does not exist: " << dataDirectory.c_str());
        return mods;
    }
    if (!bf::exists(contentDirectory))
    {
        LOG_ERROR(logger, "Content directory does not exist: " << contentDirectory.c_str());
        return mods;
    }
    
    /*for (auto &mod : storage.modifications)
    {
        LOG_INFO(logger, "Loading mod info: " << mod.second->name);
        
        /*auto initMod = [&](Mod &mod, bool noerror)
        {
            if (mod.getScriptLanguage().empty())
            {
                mod.state = GameState::Bad;
                LOG_ERROR(logger, "Script language is empty!");
            }
            if (mod.getMainScriptName().empty())
            {
                mod.state = GameState::Bad;
                LOG_ERROR(logger, "Main script name is empty!");
            }
        };*/

        /*Mod mod;
        //initMod(mod, false);
        mods.insert(mod);
    }*/
    return mods;
}

} // namespace polygon4

