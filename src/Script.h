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

#include <string>
#include <unordered_map>
#include <vector>

#include <Polygon4/DataManager/Types.h>

#include "Common.h"
#include "ScriptAPI.h"

namespace polygon4
{

using detail::ScriptLanguage;

using FunctionName = String;
using ScriptParameters = std::vector<String>;

class ScriptEngine;

class Script
{
public:
    ScriptData data;

public:
    Script();
    Script(const Script&) = delete;
    Script& operator=(const Script&) = delete;
    virtual ~Script() = default;

    virtual std::string getScriptExtension() const { return std::string(); }

    virtual void loadFile(const path &p);

public: /* API */
    virtual void call(const FunctionName &fn, const ScriptParameters &params = ScriptParameters()) {}

    void OnEnterBuilding();
    void RegisterQuests();

private:
    virtual bool loadScriptFile(const path &p) { return false; }
};

class ScriptEngine
{
public:
    ScriptEngine(const path &p, ScriptLanguage language);

    Script *getScript(const std::string &name);

private:
    path root;
    ScriptLanguage language;
    std::unordered_map<std::string, std::unique_ptr<Script>> scripts;
};

} // namespace polygon4

