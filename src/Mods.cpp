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

#include <Polygon4/Mods.h>

#include <fstream>

#include <boost/filesystem.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <sqlite3/sqlite3.h>

#include "Game.h"
#include "Db.h"
#include "Script.h"

#include <tools/Logger.h>

DECLARE_STATIC_LOGGER(logger, "mods");

namespace polygon4
{

Mod::~Mod()
{
    stopGame();
}
    
bool Mod::newGame()
{
    if (state == GameState::Bad)
    {
        LOG_ERROR(logger, "Game is in bad state!");
        return false;
    }
    if (state != GameState::None)
    {
        LOG_ERROR(logger, "Game is not in initial state!");
        return false;
    }
    try
    {
        Database::loadCommonDatabase(to_string(getCommonContent().getDatabaseName()));
        db = std::make_shared<Database>(to_string(databaseName), usesCommonContent);
        auto script = Script::createScript(getScriptName(path, mainScriptName), scriptLanguage);
        game = std::make_shared<Game>(db, script);
        game->run();
    }
    catch (std::exception e)
    {
        LOG_ERROR(logger, "Cannot start game: " << e.what());
        return false;
    }
    return true;
}

bool Mod::loadGame(String filename)
{
    return false;
}

bool Mod::stopGame()
{
    return true;
}

bool Mod::operator<(const Mod &rhs) const
{
    if (name.wstring() != rhs.name.wstring())
        return name.wstring() < rhs.name.wstring();
    return dir.wstring() < rhs.dir.wstring();
}

DEFINE_GET_SET(Mod, String, DataDir, dataDir);
DEFINE_GET_SET(Mod, String, ContentDir, contentDir);

DEFINE_GET_SET(Mod, String, Dir, dir);
DEFINE_GET_SET(Mod, String, Path, path);

DEFINE_GET_SET(Mod, String, Name, name);
DEFINE_GET_SET(Mod, String, Author, author);
DEFINE_GET_SET(Mod, String, Version, version);
DEFINE_GET_SET(Mod, String, Created, date_created);
DEFINE_GET_SET(Mod, String, Modified, date_modified);
DEFINE_GET_SET(Mod, String, Comment, comment);

DEFINE_GET_SET(Mod, String, DatabaseName, databaseName);

DEFINE_GET_SET(Mod, std::string, ScriptLanguage, scriptLanguage);
DEFINE_GET_SET(Mod, String, MainScriptName, mainScriptName);

DEFINE_GET_SET(Mod, bool, UsesCommonContent, usesCommonContent);

Mod &Mod::getCommonContent()
{
    static Mod common;
    return common;
}

const Mods &enumerateMods(String dd, String cd)
{
    namespace bf = boost::filesystem;
    namespace pt = boost::property_tree;

    std::wstring dataDirectory = dd.wstring();
    dataDirectory = bf::absolute(dataDirectory).normalize().wstring();
    std::wstring contentDirectory = cd.wstring();
    contentDirectory = bf::absolute(contentDirectory).normalize().wstring();

    static Mods mods;
    mods.clear();
    
    bf::path p(dataDirectory);
    bf::directory_iterator end;
    for (bf::directory_iterator iter(p); iter != end; ++iter)
    {
        if (!bf::is_directory(*iter))
            continue;

        bf::path p = bf::absolute(iter->path()).normalize();
        LOG_INFO(logger, "Loading mod info: " << p.string());

        pt::ptree pt;
        auto mod_fn = p / MOD_FILENAME;
        std::ifstream mod_json(mod_fn.wstring());
        if (!mod_json)
        {
            LOG_WARN(logger, "Cannot open mod file: " << mod_fn.wstring());
            continue;
        }
        try
        {
            pt::json_parser::read_json(mod_json, pt);
        }
        catch (pt::json_parser::json_parser_error e)
        {
            LOG_WARN(logger, "Error while reading mod file: " << mod_fn.wstring() << "\n" << e.what());
            continue;
        }

        auto initMod = [&](Mod &mod, bool noerror)
        {
            mod.setDataDir(dataDirectory);
            mod.setDataDir(contentDirectory);

            mod.setPath(p.wstring());
            mod.setDir(p.filename().wstring());

            mod.setName(to_wstring(pt.get("name", "")));
            mod.setAuthor(to_wstring(pt.get("author", "")));
            mod.setVersion(to_wstring(pt.get("version", "")));
            mod.setCreated(to_wstring(pt.get("date.created", "")));
            mod.setModified(to_wstring(pt.get("date.modified", "")));
            mod.setComment(to_wstring(pt.get("comment", "")));
        
            bf::path db = p / pt.get("database", "");
            mod.setDatabaseName(db.wstring());

            mod.setScriptLanguage(pt.get("scripts.language", ""));
            mod.setMainScriptName(to_wstring(pt.get("scripts.main", "")));

            mod.setUsesCommonContent(pt.get("uses_common_content", false));

            if (noerror)
                return;

            if (mod.getScriptLanguage().empty())
            {
                mod.state = GameState::Bad;
                LOG_ERROR(logger, "Script language is empty!");
            }
            if (mod.getMainScriptName().empty())
            {
                mod.state = GameState::Bad;
                LOG_ERROR(logger, "Main script name is empty!");
            }
            if (mod.getDatabaseName().empty())
            {
                mod.state = GameState::Bad;
                LOG_ERROR(logger, "Database name is empty!");
            }
        };
        
        if (p.filename().wstring() == L"Common")
        {
            initMod(Mod::getCommonContent(), true);
            continue;
        }

        Mod mod;
        initMod(mod, false);
        mods.insert(mod);
    }
    return mods;
}

} // namespace polygon4

