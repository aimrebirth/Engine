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

#include <Polygon4/Engine.h>

#include <Polygon4/Storage.h>
#include <Polygon4/Modification.h>

#include "tools/Logger.h"
DECLARE_STATIC_LOGGER(logger, "engine");

#define DB_FILENAME "db.sqlite"

namespace polygon4
{

std::shared_ptr<Engine> Engine::createEngine(String modificationsDirectory)
{
	static std::shared_ptr<Storage> storage;
	try
	{
		storage = initStorage(modificationsDirectory.string() + "/" DB_FILENAME);
		storage->load();
	}
	catch (std::exception &e)
	{
        LOG_ERROR(logger, "Cannot load storage: " << e.what());
		return std::shared_ptr<Engine>(0);
	}
	return std::shared_ptr<Engine>(new Engine(storage));
}

Engine::Engine(std::shared_ptr<Storage> storage)
	: storage(storage)
{
}

Engine::~Engine()
{
}

Vector<Modification> Engine::getModifications() const
{
	Vector<Modification> mods;
	for (auto &mod : storage->modifications)
		mods.push_back(mod.second);
	return mods;
}

/*std::set<Save> Engine::getSaves() const
{
	return std::set<Save>();
}*/

} // namespace polygon4
