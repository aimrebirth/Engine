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

#include <memory>
#include <string>
#include <stdint.h>

#include <Polygon4/Common.h>

namespace polygon4
{

enum class ScriptLanguage : uint8_t
{
    Unknown,

    Lua,
};

class Script
{
    DISALLOW_COPY_CONSTRUCTORS(Script);

public:
    Script(const std::string &filename);
    virtual ~Script(){}

    static std::shared_ptr<Script> createScript(const std::string &filename, std::string language);

public: /* API*/
    //virtual void main(Game *game) = 0;

    //virtual void OnOpenLevel(Game *game, std::string level) = 0;

protected:
    std::string filename;
    ScriptLanguage language = ScriptLanguage::Unknown;
};

std::string getScriptName(std::wstring path, std::wstring scriptName);

} // namespace polygon4

