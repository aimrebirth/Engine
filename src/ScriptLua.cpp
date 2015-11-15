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

#include "ScriptLua.h"

#include <lua.hpp>

#include <API_lua.cpp>

#include <tools/Logger.h>
DECLARE_STATIC_LOGGER(logger, "script_lua");

namespace polygon4
{

ScriptLua::ScriptLua(const std::string &filename)
    : Script(filename)
{
    language = ScriptLanguage::Lua;

    L = luaL_newstate();
    luaopen_base(L);
    luaopen_Polygon4(L);
    if (luaL_loadfile(L, filename.c_str()))
    {
        LOG_ERROR(logger, "Error: " << lua_tostring(L, -1));
        throw std::exception(std::string("Unable to load lua script: " + filename).c_str());
    }
    if (lua_pcall(L, 0, 0, 0))
    {
        LOG_ERROR(logger, "Error: " << lua_tostring(L, -1));
        throw std::exception(std::string("Unable to execute lua script: " + filename).c_str());
    }
}

/*void ScriptLua::main(Game *game)
{
    LOG_DEBUG(logger, "main()");

    lua_getglobal(L, "main");
    SWIG_NewPointerObj(L, game, SWIGTYPE_p_polygon4__Game, 0);
    if (lua_pcall(L, 1, 0, 0))
    {
        LOG_ERROR(logger, "Error: " << lua_tostring(L, -1));
    }
}

void ScriptLua::OnOpenLevel(Game *game, std::string level)
{
    LOG_DEBUG(logger, "OnOpenLevel(): " << level);

    lua_getglobal(L, "OnOpenLevel");
    SWIG_NewPointerObj(L, game, SWIGTYPE_p_polygon4__Game, 0);
    lua_pushstring(L, level.c_str());
    if (lua_pcall(L, 2, 0, 0))
    {
        LOG_ERROR(logger, "Error: " << lua_tostring(L, -1));
    }
}*/

ScriptLua::~ScriptLua()
{
    lua_close(L);
}

} // namespace polygon4

