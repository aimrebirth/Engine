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

#include "Common.h"

namespace polygon4
{

class Object
{
public:
    Object();
    virtual ~Object();

public:
    ObjectName getInternalName() const;
    ObjectName getDisplayedName() const;

    bool hasObject(const ObjectName &name);
    Object *getObject(const ObjectName &name);

protected:
    void setInternalName(const ObjectName &name);
    void setDisplayedName(const ObjectName &name);
    
    bool addObject();
    bool addObject(const ObjectName &name, Object *object);
    bool removeObject();
    bool removeObject(const ObjectName &name);

    template <typename T>
    static T addObjectPrefix(T name, T prefix)
    {
        return name + "_" + prefix;
    }

private:
    ObjectName internalName;
    ObjectName displayedName;

    static Objects &getObjects();
};

}
