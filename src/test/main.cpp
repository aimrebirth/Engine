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

#include <iostream>
#include <locale>
#include <string>

#include <Polygon4/Engine.h>
#include <Polygon4/Modification.h>

class Modification : public polygon4::detail::Modification
{
    using Base = polygon4::detail::Modification;
public:
    Modification() = default;
    Modification(const polygon4::detail::Modification &m)
        : Base(m)
    {
    }
};

int main(int argc, char *argv[])
{
    polygon4::detail::CTable<polygon4::detail::Modification> m;
    auto v = m.createAtEnd();
    v->directory = "a";
    v->replace<Modification>(v.get());
    return 0;
}

