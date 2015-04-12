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
#include "String.h"

#define MOD_FILENAME "mod.json"

class sqlite3;

namespace polygon4
{

class Game;
class Database;

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
    static Mod &getCommonContent();

private:
    Mod(){}

public:
    ~Mod();

    bool newGame();
    bool loadGame(String filename);
    bool stopGame();
    
public:
    DECLARE_GET_SET(String, Dir);
    DECLARE_GET_SET(String, Path);
    DECLARE_GET_SET(String, DataDir);
    DECLARE_GET_SET(String, ContentDir);

    DECLARE_GET_SET(String, Name);
    DECLARE_GET_SET(String, Author);
    DECLARE_GET_SET(String, Version);
    DECLARE_GET_SET(String, Created);
    DECLARE_GET_SET(String, Modified);
    DECLARE_GET_SET(String, Comment);

    DECLARE_GET_SET(String, DatabaseName);
    
    DECLARE_GET_SET(std::string, ScriptLanguage);
    DECLARE_GET_SET(String, MainScriptName);

    DECLARE_GET_SET(bool, UsesCommonContent);

public:
    bool operator<(const Mod &rhs) const;

private:
    String dataDir;
    String contentDir;

    String dir;
    String path; // with dir

    String name;
    String author;
    String version;
    String date_created;
    String date_modified;
    String comment;

    String databaseName;

    std::string scriptLanguage;
    String mainScriptName;

    // game data
    std::shared_ptr<Database> db;
    std::shared_ptr<Game> game;
    GameState state = GameState::None;
    bool saved = false;
    bool usesCommonContent;
};

DLL_EXPORT
const Mods &enumerateMods(String dataDirectory, String contentDirectory);

} // namespace polygon4

