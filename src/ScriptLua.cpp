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

ScriptLua::ScriptLua()
{
    L = luaL_newstate();
    luaopen_base(L);
    luaopen_Polygon4(L);
}

ScriptLua::~ScriptLua()
{
    lua_close(L);
}

bool ScriptLua::loadScriptFile(const path &p)
{
    // load file
    if (luaL_loadfile(L, p.string().c_str()))
    {
        LOG_ERROR(logger, "Error during load file: " << lua_tostring(L, -1));
        return false;
    }
    // execute global statements
    if (lua_pcall(L, 0, 0, 0))
    {
        LOG_ERROR(logger, "Error during load file: " << lua_tostring(L, -1));
        return false;
    }
    return true;
}

void ScriptLua::call(const FunctionName &fn, const ScriptParameters &params)
{
    LOG_TRACE(logger, "call(fn = " << fn.toString() << ")");

    // find function
    lua_getglobal(L, fn.toString().c_str());

    // push script data
    SWIG_NewPointerObj(L, &data, SWIGTYPE_p_polygon4__script__ScriptData, 0);

    // push other args (string only for now)
    std::vector<std::string> params_store;
    params_store.reserve(params.size());
    for (auto &p : params)
    {
        params_store.emplace_back(p.toString());
        lua_pushstring(L, params_store.back().c_str());
    }

    if (lua_pcall(L, params.size() + 1, 0, 0))
    {
        LOG_ERROR(logger, "Error during call to '" << fn.toString() << "': " << lua_tostring(L, -1));
    }
}

} // namespace polygon4
