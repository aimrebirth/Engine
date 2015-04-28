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

#include <string>
#include <set>

#include "dll.h"

#include "Common.h"

#include <Polygon4/String.h>

namespace polygon4
{

class Game;

enum class GameState : uint8_t
{
    None            =   0x0,
    Initialized,
    Started,
    Ended,
    Bad
};
    
typedef std::set<class Mod> Mods;

class DLL_EXPORT Mod
{
    DLL_EXPORT
    friend const Mods &enumerateMods(String dataDirectory, String contentDirectory);

private:
    Mod(){}

public:
    ~Mod();

    bool isOk() const;

    bool newGame();
    bool loadGame(String filename);
    bool stopGame();

public:
    bool operator<(const Mod &rhs) const;

private:
    // game data
    std::shared_ptr<Game> game;
    GameState state = GameState::None;
    bool saved = false;
};

DLL_EXPORT
const Mods &enumerateMods(String dataDirectory, String contentDirectory);

} // namespace polygon4

