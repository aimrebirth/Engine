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

#include <Polygon4/Map.h>

#include <tools/Logger.h>
DECLARE_STATIC_LOGGER(logger, "maps");

namespace polygon4
{

Map::Map(const Base &rhs)
    : Base(rhs)
{
}

bool Map::loadObjects()
{
    for (auto &v : buildings)
    {
        if (v->enabled)
            v->spawn();
    }
    for (auto &v : objects)
    {
        if (v->enabled)
            v->spawn();
    }
    return true;
}

} // namespace polygon4
