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

#include <Polygon4/MapBuilding.h>

#include <tools/Logger.h>
DECLARE_STATIC_LOGGER(logger, "map_buildings");

namespace polygon4
{

MapBuilding::MapBuilding(const Base &rhs)
    : Base(rhs)
{
}

void MapBuilding::setModificationMapBuilding(detail::ModificationMapBuilding *b)
{
    modificationMapBuilding = b;
    if (modificationMapBuilding)
        initModificationMapBuilding();
}

} // namespace polygon4
