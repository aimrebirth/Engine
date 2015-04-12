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

class sqlite3;
typedef int(*DbCallback)(int, char**, char**);

namespace polygon4
{

class Database
{
    Database();
public:
    Database(std::string dbname, bool usesCommonContent);
    ~Database();

    void loadDatabase(std::string dbname);
    bool isLoaded() const;

    template <typename F>
    void execute(const std::string &sql, void *object, F callback)
    {
        LOG_DEBUG("db", "Executing sql statement: " << sql);
        char *errmsg;
        sqlite3_exec(db, sql.c_str(), callback, object, &errmsg);
        if (errmsg)
        {
            LOG_ERROR("db", "Error executing sql statement: " << errmsg);
            sqlite3_free(errmsg);
        }
    }

private:
    sqlite3 *db = 0;
    bool usesCommonContent = false;

public:
    static void loadCommonDatabase(std::string dbname);
    static Database *getCommonDatabase();
};

}
