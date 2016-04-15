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

#include <chrono>

#include <Polygon4/BuildingMenu.h>
#include <Polygon4/Engine.h>
#include <Polygon4/Modification.h>
#include <Polygon4/Settings.h>

#include "Script.h"

#include <tools/Logger.h>
DECLARE_STATIC_LOGGER(logger, "script_api");

namespace polygon4
{

namespace script
{

String &getScreenText()
{
    auto bm = getEngine()->getBuildingMenu();
    return bm->getText();
}

polygon4::detail::Message *get_message_by_id(const std::string &message_id)
{
    auto se = getEngine()->getCurrentModification()->getScriptEngine();
    auto &v = se->getMessages();
    auto i = v.find(message_id);
    if (i == v.end())
    {
        LOG_ERROR(logger, "Message '" << message_id << "' was not found");
        return nullptr;
    }
    return (polygon4::detail::Message*)i->second;
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

    SetMoney(GetMoney() + amount);
}

bool ScriptData::HasMoney(float amount) const
{
    LOG_TRACE(logger, "HasMoney(" << amount << ")");

    return GetMoney() >= amount;
}

float ScriptData::GetMoney() const
{
    return player->mechanoid->getMoney();
}

void ScriptData::SetMoney(float m)
{
    return player->mechanoid->setMoney(m);
}

void ScriptData::AddRating(float amount, RatingType type)
{
    LOG_TRACE(logger, "AddRating(" << amount << "), type: " << static_cast<int>(type));

    SetRating(GetRating(type) + amount);
}

bool ScriptData::HasRating(float amount, RatingType type) const
{
    LOG_TRACE(logger, "HasRating(" << amount << "), type: " << static_cast<int>(type));

    return GetRating(type) >= amount;
}

float ScriptData::GetRating(RatingType type) const
{
    LOG_TRACE(logger, "GetRating(), type: " << static_cast<int>(type));

    return player->mechanoid->getRating((polygon4::detail::RatingType)type);
}

void ScriptData::SetRating(float amount, RatingType type)
{
    LOG_TRACE(logger, "SetRating(), type: " << static_cast<int>(type));

    return player->mechanoid->setRating(amount, (polygon4::detail::RatingType)type);
}

int ScriptData::GetRatingLevel(RatingType type) const
{
    LOG_TRACE(logger, "GetRatingLevel(), type: " << static_cast<int>(type));

    return player->mechanoid->getRatingLevel((polygon4::detail::RatingType)type);
}

bool ScriptData::HasRatingLevel(int level, RatingType type) const
{
    LOG_TRACE(logger, "HasRatingLevel(" << level << "), type: " << static_cast<int>(type));

    return GetRatingLevel(type) >= level;
}

void ScriptData::SetRatingLevel(int level, RatingType type) const
{
    LOG_TRACE(logger, "SetRatingLevel(" << level << "), type: " << static_cast<int>(type));

    player->mechanoid->setRatingLevel(level, (polygon4::detail::RatingType)type);
}

void ScriptData::AddJournalRecord(const std::string &message_id, JournalRecord type)
{
    LOG_TRACE(logger, "AddJournalRecord(" << message_id << ")");

    auto m = get_message_by_id(message_id);
    if (!m)
        return;

    if (player->records.count(message_id) != 0)
    {
        auto &r = player->records[message_id];
        r->type = (detail::JournalRecordType)type;
        return;
    }

    auto s = player->getStorage();
    auto r = s->addJournalRecord(player);
    r->text_id = m->text_id;
    r->message = m;
    r->type = (detail::JournalRecordType)type;
    r->time = getSettings().playtime;
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

void AddMessage(const std::string &message_id, bool clear)
{
    if (clear)
        ClearText();
    getEngine()->getBuildingMenu()->addTheme(get_message_by_id(message_id));
}

void AddMessage(const std::string &message_id)
{
    LOG_TRACE(logger, "AddMessage(" << message_id << ")");

    AddMessage(message_id, false);
}

void ShowMessage(const std::string &message_id)
{
    LOG_TRACE(logger, "ShowMessage(" << message_id << ")");

    AddMessage(message_id, true);
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
