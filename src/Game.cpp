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
#include <sqlite3/sqlite3.h>

#include <Polygon4/API.h>

#include <tools/Logger.h>

#include "tools/Hotpatch.h"
#include "Db.h"
#include "Sector.h"

DECLARE_STATIC_LOGGER(logger, "game");

namespace polygon4
{

Game::Game(std::shared_ptr<Database> db)
    : db(db)
{
    loadSectors();
}

void Game::loadSectors()
{
    auto callback = [](void *object, int ncols, char **cols, char **names) -> int
    {
        Game *game = (Game *)object;
        std::shared_ptr<Sector> s = std::make_shared<Sector>();
        if (s->load(ncols, cols, names))
            game->sectors[cols[1]] = s;
        return 0;
    };
    db->execute("select * from sectors;", this, callback);
}

void Game::OpenLevel(std::string level)
{
    API_CALL(OpenLevel, sectors[level]->getDisplayedName(), sectors[level]->getResourceName());
}

} // namespace polygon4

