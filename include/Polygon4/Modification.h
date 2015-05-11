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
#include <string>
#include <set>

#include "dll.h"

#include <Polygon4/String.h>

namespace polygon4
{

namespace detail
{
    class Modification;
}

class Game;

class DLL_EXPORT Modification
{
public:
    std::shared_ptr<detail::Modification> data;

public:
	Modification(){}
    Modification(std::shared_ptr<detail::Modification> modification);
	~Modification();

    bool newGame();
    bool loadGame(String filename);

public:
	bool operator<(const Modification &rhs) const;

private:
    // game data
    std::shared_ptr<Game> game;
};

} // namespace polygon4

