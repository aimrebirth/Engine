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

#include <Polygon4/Mods.h>

#include <boost/filesystem.hpp>

namespace polygon4
{
    
Mods enumerateMods(std::wstring rootDirectory)
{
    using namespace boost::filesystem;

    path p(rootDirectory);

    Mods mods;
    directory_iterator end;
    for (directory_iterator iter(p); iter != end; ++iter)
    {
        if (!is_directory(*iter))
            continue;
        Mod mod;
        path p = absolute(iter->path()).normalize();
        mod.setPath(p.wstring());
        mod.setName(p.filename().wstring());
        if (mod.getName() == L"Common")
            continue;
        mods.push_back(mod);
    }
    return mods;
}

std::wstring Mod::getPath() const
{
    return path;
}

void Mod::setPath(std::wstring path)
{
    this->path = path;
}

std::wstring Mod::getName() const
{
    return name;
}

void Mod::setName(std::wstring name)
{
    this->name = name;
}

} // namespace polygon4

