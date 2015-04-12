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

#include "Object.h"

namespace polygon4
{

class DbObject : public Object
{
public:
    DbObject();
    virtual ~DbObject();

public:
    ObjectName getResourceName() const;
    void setResourceName(const ObjectName &name);

    virtual bool load(int ncols, char **cols, char **names) = 0;
    virtual bool save() const = 0;
protected:
    bool loaded = false;

    // fields
    int id = 0;
    ObjectName resourceName;
};

}

