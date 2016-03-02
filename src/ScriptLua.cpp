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

#include <Polygon4/DataManager/Exception.h>

#include <lua.hpp>

#include <ScriptAPI_lua.cpp>

#include <tools/Logger.h>
DECLARE_STATIC_LOGGER(logger, "script_lua");

namespace polygon4
{

ScriptLua::ScriptLua(const path &filename, const ScriptEngine *scriptEngine)
    : Script(filename, scriptEngine)
{
    L = luaL_newstate();
    luaopen_base(L);
    luaopen_Polygon4(L);
    // load file
    if (luaL_loadfile(L, p.string().c_str()))
    {
        auto p2 = p;
        p2 += "." + getScriptExtension();
        if (luaL_loadfile(L, p2.string().c_str()))
        {
            LOG_ERROR(logger, "Error: " << lua_tostring(L, -1));
            throw EXCEPTION("Unable to load lua script: " + p.string());
        }
    }
    // execute global statements
    if (lua_pcall(L, 0, 0, 0))
    {
        LOG_ERROR(logger, "Error: " << lua_tostring(L, -1));
        throw EXCEPTION("Unable to execute lua script: " + p.string());
    }
}

ScriptLua::~ScriptLua()
{
    lua_close(L);
}

void ScriptLua::OnEnterBuilding(ScriptData &data)
{
    LOG_TRACE(logger, "OnEnterBuilding(" << data.building_name << ")");

    lua_getglobal(L, "OnEnterBuilding");
    SWIG_NewPointerObj(L, &data, SWIGTYPE_p_polygon4__script__ScriptData, 0);
    if (lua_pcall(L, 1, 0, 0))
    {
        LOG_ERROR(logger, "Error: " << lua_tostring(L, -1));
    }
}

} // namespace polygon4
