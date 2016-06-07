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

#include <Polygon4/Engine.h>

#include "ScriptLua.h"

#include <tools/Logger.h>
DECLARE_STATIC_LOGGER(logger, "script");

namespace polygon4
{

Script::Script()
{
}

void Script::loadFile(const path &p)
{
    LOG_TRACE(logger, "Trying to load '" << getScriptExtension() << "' script file: " << p.string());

    if (loadScriptFile(p))
        return;

    // try with extension
    auto p2 = p;
    p2 += "." + getScriptExtension();

    LOG_TRACE(logger, "Trying to load '" << getScriptExtension() << "' script file: " << p2.string());

    if (!loadScriptFile(p2))
        throw EXCEPTION("Unable to load '" + getScriptExtension() + "' script: " + p.string());
}

ScriptEngine::ScriptEngine(const path &p, ScriptLanguage language)
    : root(p / "Scripts"), language(language)
{
}

Script *ScriptEngine::getScript(const std::string &name)
{
    auto fn = root / name;

    LOG_TRACE(logger, "Creating '" << str(language).toString() << "' script executor on file: " << fn.string());

    std::unique_ptr<Script> script;
    try
    {
        switch (language)
        {
        case ScriptLanguage::Lua:
            script = std::make_unique<ScriptLua>();
            break;
        default:
            LOG_FATAL(logger, "This language '" << str(language) << "' is not supported!");
            script = std::make_unique<Script>();
            break;
        }
    }
    catch (const std::exception &e)
    {
        LOG_ERROR(logger, "Cannot load script: " << fn.string());
        script = std::make_unique<Script>();
    }

    // load common file, then load concrete file
    script->loadFile(root / "common");
    script->loadFile(fn);

    scripts[fn.string()] = std::move(script);
    return scripts[fn.string()].get();
}

} // namespace polygon4
