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

#include "Sector.h"

#include <Polygon4/Common.h>

#include <tools/Logger.h>

DECLARE_STATIC_LOGGER(logger, "sector");

namespace polygon4
{
    
bool Sector::load(int ncols, char **cols, char **names)
{
    if (loaded)
        return false;
    id = std::stoi(cols[0]);
    setInternalName("SEC_" + std::string(cols[1]));
    setDisplayedName(cols[2]);
    setResourceName(cols[3]);
    addObject();
    loaded = true;
    return true;
}

bool Sector::save() const
{
    return true;
}

}
