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

#include <Polygon4/DataManager/Types.h>

namespace polygon4
{

class ScriptEngine;

namespace script
{

struct ScriptData
{
#ifndef SWIG
    ScriptEngine *scriptEngine;
#endif
    polygon4::detail::ModificationPlayer *player;
    std::string building_name;

    void AddObject(const std::string &o);
    // add_quest or start_quest
    // set_target (_mark) or set_pointer
    // set_event = set quest stage
    // info = show proper text
};

} // namespace script

using script::ScriptData;

} // namespace polygon4
