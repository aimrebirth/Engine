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

class DLL_EXPORT MapBuilding : public detail::MapBuilding
{
    using Base = detail::MapBuilding;

public:
    MapBuilding(const Base &);

    virtual detail::ModificationMapBuilding *getModificationMapBuilding() const override final { return modificationMapBuilding; };
    virtual void setModificationMapBuilding(detail::ModificationMapBuilding *b) override final;

protected:
    detail::ModificationMapBuilding *modificationMapBuilding = nullptr;
};

} // namespace polygon4
