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

#include <boost/filesystem.hpp>

#include "Game.h"
#include "Script.h"

#include <tools/Logger.h>
DECLARE_STATIC_LOGGER(logger, "mods");

namespace polygon4
{

Modification::Modification(Ptr<detail::Modification> modification)
	: data(modification)
{

}

Modification::~Modification()
{
}
    
bool Modification::newGame()
{
    try
    {
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

bool Modification::loadGame(String filename)
{
    return false;
}

bool Modification::operator<(const Modification &rhs) const
{
    if (data == rhs.data || data)
        return false;
    if (!data)
        return true;
    return data->directory < rhs.data->directory;
}

//const Mods &enumerateMods(String dd, String cd)
//{
//    namespace bf = boost::filesystem;
//
//    std::wstring dataDirectory = dd.wstring();
//    dataDirectory = bf::absolute(dataDirectory).normalize().wstring();
//    std::wstring contentDirectory = cd.wstring();
//    contentDirectory = bf::absolute(contentDirectory).normalize().wstring();
//
//    
//    /*for (auto &mod : storage.modifications)
//    {
//        LOG_INFO(logger, "Loading mod info: " << mod.second->name);
//        
//        /*auto initMod = [&](Mod &mod, bool noerror)
//        {
//            if (mod.getScriptLanguage().empty())
//            {
//                mod.state = GameState::Bad;
//                LOG_ERROR(logger, "Script language is empty!");
//            }
//            if (mod.getMainScriptName().empty())
//            {
//                mod.state = GameState::Bad;
//                LOG_ERROR(logger, "Main script name is empty!");
//            }
//        };*/
//
//        /*Mod mod;
//        //initMod(mod, false);
//        mods.insert(mod);
//    }*/
//    return mods;
//}

} // namespace polygon4

