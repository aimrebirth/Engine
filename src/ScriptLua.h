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

#pragma once

#include "Script.h"

struct lua_State;

namespace polygon4
{

class ScriptLua : public Script
{
public:
    ScriptLua();
    virtual ~ScriptLua();

    virtual std::string getScriptExtension() const override { return "lua"; }

private:
    virtual bool loadScriptFile(const path &p) override;

public: /* API */
    virtual void call(const FunctionName &fn, const ScriptParameters &params = ScriptParameters()) override;

private:
    lua_State *L;
};

} // namespace polygon4

