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

#include "ScriptAPI.h"

#include <Polygon4/BuildingMenu.h>
#include <Polygon4/Engine.h>
#include <Polygon4/Modification.h>

#include "Script.h"

#include <tools/Logger.h>
DECLARE_STATIC_LOGGER(logger, "script_api");

namespace polygon4
{

const std::vector<float> rating_levels{
           50,
          200,
          500,
        1'500,
        5'000,
       15'000,
       50'000,
      200'000,
    1'000'000,
};

namespace script
{

String &getScreenText()
{
    auto bm = getEngine()->getBuildingMenu();
    return bm->getText();
}

float &ScriptData::getMoney() const
{
    return player->mechanoid->money;
}

float &ScriptData::getRating(Rating type) const
{
    switch (type)
    {
    case Rating::Courier:
        return player->mechanoid->rating_courier;
    case Rating::Fight:
        return player->mechanoid->rating_fight;
    case Rating::Trade:
        return player->mechanoid->rating_trade;
    }
    return player->mechanoid->rating;
}

int ScriptData::getRatingLevel(float rating)
{
    int i = 0;
    while (rating > rating_levels[i++] && i < rating_levels.size());
    return i;
}

void ScriptData::setRatingLevel(float &rating, int level)
{
    if (level > rating_levels.size())
        rating = rating_levels.size();
    rating = rating_levels[level - 1];
}

void ScriptData::AddObject(const std::string &oname)
{
    LOG_TRACE(logger, "AddObject(" << oname << ")");

    auto &objs = scriptEngine->getObjects();
    auto i = objs.find(oname);
    if (i == objs.end())
    {
        LOG_ERROR(logger, "Object '" << oname << "' was not found");
        return;
    }
    auto o = i->second;
    auto conf = player->mechanoid->getConfiguration();
    conf->addObject(o);
}

void ScriptData::AddMoney(float amount)
{
    LOG_TRACE(logger, "AddMoney(" << amount << ")");

    getMoney() += amount;
}

void ScriptData::AddRating(float amount)
{
    LOG_TRACE(logger, "AddRating(" << amount << ")");

    getRating() += amount;
}

void ScriptData::AddRating(Rating type, float amount)
{
    LOG_TRACE(logger, "AddRating(" << amount << "), type: " << static_cast<int>(type));

    getRating(type) += amount;
}

float ScriptData::GetRating() const
{
    LOG_TRACE(logger, "GetRating()");

    return getRating();
}

float ScriptData::GetRating(Rating type) const
{
    LOG_TRACE(logger, "GetRating(), type: " << static_cast<int>(type));

    return getRating(type);
}

int ScriptData::GetRatingLevel() const
{
    LOG_TRACE(logger, "GetRatingLevel()");

    return getRatingLevel(getRating());
}

int ScriptData::GetRatingLevel(Rating type) const
{
    LOG_TRACE(logger, "GetRatingLevel(), type: " << static_cast<int>(type));

    return getRatingLevel(getRating(type));
}

bool ScriptData::HasMoney(float amount) const
{
    LOG_TRACE(logger, "HasMoney(" << amount << ")");

    return getMoney() >= amount;
}

bool ScriptData::HasRating(float amount) const
{
    LOG_TRACE(logger, "HasRating(" << amount << ")");

    return getRating() >= amount;
}

bool ScriptData::HasRating(Rating type, float amount) const
{
    LOG_TRACE(logger, "HasRating(" << amount << "), type: " << static_cast<int>(type));

    return getRating(type) >= amount;
}

bool ScriptData::HasRatingLevel(int level) const
{
    LOG_TRACE(logger, "HasRatingLevel(" << level << ")");

    return GetRatingLevel() >= level;
}

bool ScriptData::HasRatingLevel(Rating type, int level) const
{
    LOG_TRACE(logger, "HasRatingLevel(" << level << "), type: " << static_cast<int>(type));

    return GetRatingLevel(type) >= level;
}

void ScriptData::SetRatingLevel(int level) const
{
    LOG_TRACE(logger, "SetRatingLevel(" << level << ")");

    setRatingLevel(getRating(), level);
}

void ScriptData::SetRatingLevel(Rating type, int level) const
{
    LOG_TRACE(logger, "SetRatingLevel(" << level << "), type: " << static_cast<int>(type));

    setRatingLevel(getRating(type), level);
}

void AddText(const std::string &text)
{
    LOG_TRACE(logger, "AddText(" << text << ")");

    auto &t = getScreenText();
    t += String(text);
}

void ShowText(const std::string &text)
{
    LOG_TRACE(logger, "ShowText(" << text << ")");

    auto &t = getScreenText();
    t.clear();
    t += String(text);
}

void ClearText()
{
    LOG_TRACE(logger, "ClearText()");

    auto &t = getScreenText();
    t.clear();
}

void AddMessage(::polygon4::String &t, polygon4::detail::Message *message)
{
    t += message->title->string;
    t += "\n";
    t += message->txt->string;
    t += "\n";
}

void AddMessage(const std::string &message_id)
{
    LOG_TRACE(logger, "AddMessage(" << message_id << ")");

    auto se = getEngine()->getCurrentModification()->getScriptEngine();
    auto &v = se->getMessages();
    auto i = v.find(message_id);
    if (i == v.end())
    {
        LOG_ERROR(logger, "Message '" << message_id << "' was not found");
        return;
    }
    auto &t = getScreenText();
    AddMessage(t, (polygon4::detail::Message*)i->second);
}

void ShowMessage(const std::string &message_id)
{
    LOG_TRACE(logger, "ShowMessage(" << message_id << ")");

    auto se = getEngine()->getCurrentModification()->getScriptEngine();
    auto &v = se->getMessages();
    auto i = v.find(message_id);
    if (i == v.end())
    {
        LOG_ERROR(logger, "Message '" << message_id << "' was not found");
        return;
    }
    auto &t = getScreenText();
    t.clear();
    AddMessage(t, (polygon4::detail::Message*)i->second);
}

ScreenText GetScreenText()
{
    LOG_TRACE(logger, "GetScreenText()");

    ScreenText text;
    text.screenText = &getScreenText();
    return text;
}

ScreenText ScreenText::operator+(const std::string &s)
{
    LOG_TRACE(logger, "ScreenText::operator+(" << s << ")");

    if (screenText)
        getScreenText() += String(s);
    return *this;
}

ScreenText ScreenText::__concat__(const std::string &s)
{
    return ScreenText::operator+(s);
}

} // namespace script

} // namespace polygon4
