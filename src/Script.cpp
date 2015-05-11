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

#include "ScriptLua.h"

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

std::shared_ptr<Script> Script::createScript(const std::string &filename, std::string language)
{
    std::transform(language.begin(), language.end(), language.begin(), ::tolower);

    LOG_DEBUG(logger, "Creating " << language << " script executor on file: " << filename);

    std::shared_ptr<Script> script;
    if (language == "lua")
    {
        script = std::make_shared<ScriptLua>(filename);
    }
    return script;
}

Script::Script(const std::string &filename)
    : filename(filename)
{
}

} // namespace polygon4

