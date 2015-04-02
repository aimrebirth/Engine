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

#include <string>
#include <vector>

#include "dll.h"

namespace polygon4
{

class DLL_EXPORT Mod
{
public:
    Mod(){}
    
    std::wstring getName() const;
    void setName(std::wstring name);
    
    std::wstring getPath() const;
    void setPath(std::wstring path);
private:
    std::wstring name;
    std::wstring path;
};
typedef std::vector<Mod> Mods;

DLL_EXPORT
Mods enumerateMods(std::wstring rootDirectory);

} // namespace polygon4

