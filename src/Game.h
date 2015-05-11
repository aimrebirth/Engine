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

#include "Common.h"
#include <Polygon4/UnrealTypes.h>

namespace polygon4
{

namespace detail
{
    class Modification;
}

class Script;

enum class GameState : uint8_t
{
	None = 0x0,
	Initialized,
	Started,
	Ended,
	Bad
};

class Game
{
public:
    Game(){}
    Game(std::shared_ptr<detail::Modification> data, std::shared_ptr<Script> script);
    virtual ~Game();

    void run();

public: /* API*/
    void OpenLevel(std::string level);
    void SpawnPlayer(Vector v, Rotation r);

private:
    std::shared_ptr<detail::Modification> data;
    std::shared_ptr<Script> script;

	GameState state = GameState::None;
	bool saved = false;

    void bindAPI();
};

} // namespace polygon4

polygon4::Game *ptrToGame(void *ptr);

