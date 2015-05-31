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

#include <Polygon4/API.h>
#include <Polygon4/Types.h>

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
    if (data->directory.empty())
    {
        LOG_ERROR(logger, "Game directory is not set!");
        return false;
    }
    if (data->script_language.empty())
    {
        LOG_ERROR(logger, "Script language is not set!");
        return false;
    }
    if (data->script_main.empty())
    {
        LOG_ERROR(logger, "Main script name is not set!");
        return false;
    }

    String path;
    API_CALL(GetModsDir, path);
    try
    {
        auto script = Script::createScript(getScriptName(path.wstring() + data->directory.wstring(), data->script_main), data->script_language);
        game = std::make_shared<Game>(data, script);
        game->run();
        if (game->getState() == GameState::Bad)
        {
            LOG_ERROR(logger, "Game did not start correctly!");
            return false;
        }
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

} // namespace polygon4

