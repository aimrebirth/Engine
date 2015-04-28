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

#include "dll.h"

#include <Polygon4/String.h>
#include <Polygon4/Storage.h>

namespace polygon4
{

class DLL_EXPORT Engine
{
public:
    virtual ~Engine();

    static std::shared_ptr<Engine> createEngine(String modsDirectory);

    std::shared_ptr<Storage> storage() const;

private:
    Engine(String modsDirectory);

    String modsDirectory;
    std::shared_ptr<Storage> storage_;
};

} // namespace polygon4
