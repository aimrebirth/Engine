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

#include "Db.h"

#include <sqlite3/sqlite3.h>

#include <tools/Logger.h>

DECLARE_STATIC_LOGGER(logger, "db");

namespace polygon4
{

Database::Database()
{
    db = 0;
}

Database::Database(std::string dbname, bool usesCommonContent)
{
    loadDatabase(dbname);
    this->usesCommonContent = usesCommonContent;
}

Database::~Database()
{
    sqlite3_close(db);
    db = 0;
}

void Database::loadDatabase(std::string dbname)
{
    if (db)
        return;
    if (sqlite3_open(dbname.c_str(), &db))
    {
        LOG_ERROR(logger, "Can't open database file: " << dbname << " error: " << sqlite3_errmsg(db));
    }
}

bool Database::isLoaded() const
{
    if (usesCommonContent)
        return db != 0 && getCommonDatabase()->isLoaded();
    return db != 0;
}

void Database::loadCommonDatabase(std::string dbname)
{
    Database *dbCommon = getCommonDatabase();
    if (!dbCommon->db)
    {
        dbCommon->loadDatabase(dbname);
    }
}

Database *Database::getCommonDatabase()
{
    static Database dbCommon;
    return &dbCommon;
}

}
