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

#include "Script.h"

#include <boost/filesystem.hpp>
#include <lua.hpp>

#include <API_lua.cpp>

#include <tools/Logger.h>

DECLARE_STATIC_LOGGER(logger, "script");

namespace polygon4
{

std::string getScriptName(std::wstring path, std::wstring scriptName)
{
    boost::filesystem::path p = path;
    p = p / "Scripts" / scriptName;
    return boost::filesystem::absolute(p).normalize().string();
}

bool runScript(const std::string &filename, std::string language, Game *game)
{
    std::transform(language.begin(), language.end(), language.begin(), ::tolower);

    LOG_DEBUG(logger, "Executing " << language << " script: " << filename);

    if (language == "lua")
    {
        lua_State *L;
        L = luaL_newstate();
        luaopen_base(L);
        luaopen_Polygon4(L);
        int rc = luaL_loadfile(L, filename.c_str());
        if (rc == 0)
        {
            rc = lua_pcall(L, 0, 0, 0);
            if (rc)
            {
                LOG_ERROR(logger, "Error: " << lua_tostring(L, -1));
            }

            lua_getglobal(L, "main");
            SWIG_NewPointerObj(L, game, SWIGTYPE_p_polygon4__Game, 0);
            rc = lua_pcall(L, 1, 0, 0);
            if (rc)
            {
                LOG_ERROR(logger, "Error: " << lua_tostring(L, -1));
            }
        }
        else
        {
            LOG_ERROR(logger, "Unable to load script: " << lua_tostring(L, -1));
        }
        ::lua_close(L);
        return rc == 0;
    }
    return false;
}

} // namespace polygon4

