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

Script::Script(const path &p, const ScriptEngine *scriptEngine)
    : p(p), scriptEngine(scriptEngine)
{
}

ScriptEngine::ScriptEngine(const path &p, ScriptLanguage language)
    : root(p / "Scripts"), language(language)
{
#define ADD_OBJECTS(array, type) \
    do { \
        auto v = s->array.get_key_map(&polygon4::detail::type::text_id); \
        objects.insert(v.begin(), v.end()); \
    } while (0)

    auto s = gEngine->getStorage();
    ADD_OBJECTS(equipments, Equipment);
    ADD_OBJECTS(gliders, Glider);
    ADD_OBJECTS(weapons, Weapon);
    ADD_OBJECTS(projectiles, Projectile);
    ADD_OBJECTS(goods, Good);
    ADD_OBJECTS(modificators, Modificator);
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
            script = std::make_unique<ScriptLua>(fn, this);
            break;
        default:
            LOG_ERROR(logger, "This language '" << str(language) << "' is not supported!");
            script = std::make_unique<Script>(fn, this);
            break;
        }
    }
    catch (const std::exception &e)
    {
        LOG_ERROR(logger, "Cannot load script: " << fn.string());
        script = std::make_unique<Script>(fn, this);
    }
    scripts[fn.string()] = std::move(script);
    return scripts[fn.string()].get();
}

} // namespace polygon4
