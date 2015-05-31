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

#include "Game.h"

#include <boost/filesystem.hpp>

#include <tools/Logger.h>
DECLARE_STATIC_LOGGER(logger, "game");

#include <Polygon4/API.h>
#include <Polygon4/Types.h>

#include "Script.h"

namespace polygon4
{

Game::Game(std::shared_ptr<detail::Modification> data, std::shared_ptr<Script> script)
    : data(data), script(script)
{
    bindAPI();
}

Game::~Game()
{
}

GameState Game::getState() const
{
    return state;
}

void Game::run()
{
    if (!data->player_mechanoid)
    {
        LOG_ERROR(logger, "Player mechanoid is not set!");
        state = GameState::Bad;
        return;
    }
    if (!data->player_mechanoid->map)
    {
        LOG_ERROR(logger, "Player mechanoid map is not set!");
        state = GameState::Bad;
        return;
    }
    API_CALL(OpenLevel, data->player_mechanoid->map->resource.wstring());
    REGISTER_API_N_CALLS(OnOpenLevel, 1, [&](String resource)
    {
        API_CALL(SpawnStaticObjects, data->player_mechanoid->map);
        for (auto &level : data->maps)
            if (level->map->resource == resource)
                script->OnOpenLevel(this, level->map->text_id);
        API_CALL(SpawnMechanoid, data->player_mechanoid.ptr);
    });
}

void Game::bindAPI()
{
}

void Game::OpenLevel(std::string level)
{
    //std::shared_ptr<engine::Sector> object = std::static_pointer_cast<engine::Sector>(sectors[level]->object());
    //API_CALL_MSG(object->name.get(), OpenLevel, object->resource);
    //REGISTER_API_N_CALLS(OnOpenLevel, 1, std::bind(&Script::OnOpenLevel, script.get(), this, level));
}

void Game::SpawnPlayer(Vector v, Rotation r)
{
    //API_CALL(SpawnPlayer, v, r);
}

} // namespace polygon4

