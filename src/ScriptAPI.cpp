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
#include <Polygon4/Mechanoid.h>
#include <Polygon4/Modification.h>

#include "Script.h"

#include <tools/Logger.h>
DECLARE_STATIC_LOGGER(logger, "script_api");

namespace polygon4
{

namespace script
{

String &getScreenText()
{
    return GET_BUILDING_MENU()->getText();
}

polygon4::detail::Message *get_message_by_id(const std::string &message_id)
{
    auto &v = getEngine()->getMessages();
    auto i = v.find(message_id);
    if (i == v.end())
    {
        LOG_ERROR(logger, "Message '" << message_id << "' was not found");
        return nullptr;
    }
    return (polygon4::detail::Message*)i->second;
}

void ScriptData::AddItem(const std::string &oname, int quantity)
{
    LOG_TRACE(logger, "AddItem(" << oname << ", n = " << quantity << ")");

    auto &objs = getEngine()->getItems();
    auto i = objs.find(oname);
    if (i == objs.end())
    {
        LOG_ERROR(logger, "Item '" << oname << "' was not found");
        return;
    }
    auto o = i->second;
    auto conf = player->mechanoid->getConfiguration();
    conf->addItem(o, quantity);
    BM_TEXT_ADD_ITEM(o, quantity);
}

bool ScriptData::HasItem(const std::string &oname, int quantity)
{
    LOG_TRACE(logger, "HasItem(" << oname << ", n = " << quantity << ")");

    auto &objs = getEngine()->getItems();
    auto i = objs.find(oname);
    if (i == objs.end())
    {
        LOG_ERROR(logger, "Item '" << oname << "' was not found");
        return false;
    }
    auto o = i->second;
    auto conf = player->mechanoid->getConfiguration();
    return conf->hasItem(o, quantity);
}

bool ScriptData::RemoveItem(const std::string &oname, int quantity)
{
    LOG_TRACE(logger, "RemoveItem(" << oname << ", n = " << quantity << ")");

    auto &objs = getEngine()->getItems();
    auto i = objs.find(oname);
    if (i == objs.end())
    {
        LOG_ERROR(logger, "Item '" << oname << "' was not found");
        return false;
    }
    auto o = i->second;
    auto conf = player->mechanoid->getConfiguration();
    return conf->removeItem(o, quantity);
}

void ScriptData::AddMoney(float amount)
{
    LOG_TRACE(logger, "AddMoney(" << amount << ")");

    SetMoney(GetMoney() + amount);
    BM_TEXT_ADD_MONEY(amount);
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

void ScriptData::AddJournalRecord(const std::string &message_id, QuestRecord type)
{
    LOG_TRACE(logger, "AddJournalRecord(" << message_id << ")");

    MarkJournalRecord(message_id, type);

    GET_BUILDING_MENU()->JournalRecordAdded();
}

void ScriptData::MarkJournalRecord(const std::string &message_id, QuestRecord type)
{
    LOG_TRACE(logger, "MarkJournalRecord(" << message_id << ", type = " << (int)type << ")");

    auto m = get_message_by_id(message_id);
    if (!m)
        return;

    if (player->records.count(message_id) != 0)
    {
        auto &r = player->records[message_id];
        r->type = (detail::QuestRecordType)type;
        return;
    }

    auto s = player->getStorage();
    auto r = s->addJournalRecord(player);
    r->text_id = m->text_id;
    r->message = m;
    r->type = (detail::QuestRecordType)type;
    r->time = getEngine()->getSettings().playtime;
    player->records.insert_to_data(r);
}

void ScriptData::MarkJournalRecordCompleted(const std::string &message_id)
{
    LOG_TRACE(logger, "MarkJournalRecordCompleted(" << message_id << ")");

    MarkJournalRecord(message_id, QuestRecord::Completed);
}

int ScriptData::GetVar(const std::string &var)
{
    LOG_TRACE(logger, "GetVar(" << var << ")");

    auto v = std::find_if(player->variables.begin(), player->variables.end(), [&var](const auto &v)
    {
        return v->key == var;
    });
    if (v != player->variables.end())
    {
        LOG_TRACE(logger, "GetVar(val = " << (*v)->value_int << ")");
        return (*v)->value_int;
    }
    LOG_TRACE(logger, "GetVar(val = " << 0 << ")");
    return 0;
}

void ScriptData::SetVar(const std::string &var, int i)
{
    LOG_TRACE(logger, "SetVar(" << var << ", val = " << i << ")");

    auto v = std::find_if(player->variables.begin(), player->variables.end(), [&var](const auto &v)
    {
        return v->key == var;
    });
    if (v != player->variables.end())
    {
        (*v)->value_int = i;
        return;
    }

    auto sv = GET_STORAGE()->scriptVariables.createAtEnd();
    sv->player = player;
    sv->key = var;
    sv->value_int = i;
    player->variables.insert(sv);
}

void ScriptData::SetVar(const std::string &var, const std::string &val)
{
    LOG_TRACE(logger, "SetVar(" << var << ", val = " << val << ")");

    auto v = std::find_if(player->variables.begin(), player->variables.end(), [&var](const auto &v)
    {
        return v->key == var;
    });
    if (v != player->variables.end())
    {
        (*v)->value_text = val;
        return;
    }

    auto sv = GET_STORAGE()->scriptVariables.createAtEnd();
    sv->player = player;
    sv->key = var;
    sv->value_text = val;
    player->variables.insert(sv);
}

void ScriptData::UnsetVar(const std::string &var)
{
    LOG_TRACE(logger, "UnsetVar(" << var << ")");

    auto v = std::find_if(player->variables.begin(), player->variables.end(), [&var](const auto &v)
    {
        return v->key == var;
    });
    if (v == player->variables.end())
        return;
    player->variables.erase(v);
}

bool ScriptData::CheckVar(const std::string &var)
{
    LOG_TRACE(logger, "CheckVar(" << var << ")");

    auto v = std::find_if(player->variables.begin(), player->variables.end(), [&var](const auto &v)
    {
        return v->key == var;
    });
    if (v != player->variables.end())
    {
        LOG_TRACE(logger, "CheckVar(true)");
        return true;
    }
    LOG_TRACE(logger, "CheckVar(false)");
    return false;
}

bool ScriptData::RunOnce(const std::string &var)
{
    LOG_TRACE(logger, "RunOnce(" << var << ")");

    if (!CheckVar(var))
    {
        SetVar(var);
        return true;
    }
    return false;
}

std::string ScriptData::GetName() const
{
    LOG_TRACE(logger, "GetName()");

    if (player->mechanoid->name)
        return player->mechanoid->name->string.str().toString();
    return "";
}

void ScriptData::SetName(const std::string &name) const
{
    LOG_TRACE(logger, "SetName(" << name << ")");

    player->mechanoid->setName(name);
}

void ScriptData::SetPointer(const std::string &bld)
{
    LOG_TRACE(logger, "SetPointer(" << bld << ")");

    // mark building as known
    auto iter = player->buildings.find_if([bld](const auto &vb)
    {
        return vb->building->text_id == bld;
    });
    if (iter == player->buildings.end())
    {
        auto vb = GET_STORAGE()->modificationPlayerBuildings.createAtEnd();
        vb->player = player;
        vb->building = building;
        vb->know_location = true;
        player->buildings.insert(vb);
    }
    else
    {
        (*iter)->know_location = true;
    }
}

bool ScriptData::IsDamaged(float percent) const
{
    LOG_TRACE(logger, "IsDamaged(" << percent << "%)");

    percent /= 50.0f;
    auto c = player->mechanoid->getConfiguration();
    auto p = 1.0f - c->getCurrentArmor() / c->getMaxArmor();
    if (percent == 0.0f && p == 0.0f)
        return false;
    return p >= percent;
}

bool ScriptData::IsDamagedHigh() const
{
    LOG_TRACE(logger, "IsDamagedHigh()");

    return IsDamaged(50);
}

bool ScriptData::IsDamagedLow() const
{
    LOG_TRACE(logger, "IsDamagedLow()");

    return !IsDamagedHigh() && IsDamaged();
}

void ScriptData::RegisterQuest(const std::string &name)
{
    LOG_TRACE(logger, "RegisterQuest(" << name << ")");

    if (next_quest || CheckVar(name + ".COMPLETED"))
        return; // already completed

    auto o = getEngine()->getObjects()[name];
    if (!o)
    {
        LOG_ERROR(logger, "Quest '" << name << "' not found");
        return; // no such quest
    }

    next_quest = (polygon4::detail::Message*)o;
}

bool ScriptData::IsQuestAvailable() const
{
    LOG_TRACE(logger, "IsQuestAvailable()");

    return !!next_quest;
}

void ScriptData::ListAvailableQuests() const
{
    LOG_TRACE(logger, "ListAvailableQuests()");

    if (!next_quest)
        return;

    auto bm = GET_BUILDING_MENU();
    bm->addQuestMessage(next_quest);
    AddMessage("S_ACCEPT_QUEST");
    bm->removeMessage(get_message_by_id("S_TASK"));
}

void ScriptData::AcceptQuest(const std::string &name)
{
    LOG_ERROR(logger, "AcceptQuest(" << name << ")");

    auto bm = GET_BUILDING_MENU();
    bm->removeMessage(get_message_by_id(name));
    bm->removeMessage(get_message_by_id("S_ACCEPT_QUEST"));
    //AddMessage(name + "_GET");
    script->call(name); // call script function
}

void ScriptData::AddQuestRecord(const std::string &message_id, QuestRecord type)
{
    LOG_TRACE(logger, "AddQuestRecord(" << message_id << ")");
}

void ScriptData::MarkQuestRecord(const std::string &message_id, QuestRecord type)
{
    LOG_TRACE(logger, "MarkQuestRecord(" << message_id << ", type = " << (int)type << ")");
}

void ScriptData::MarkQuestRecordCompleted(const std::string &message_id)
{
    LOG_TRACE(logger, "MarkQuestRecordCompleted(" << message_id << ")");
}

void ScriptData::StartTimer(const std::string &name, int seconds)
{
    LOG_ERROR(logger, "StartTimer(" << name << ", sec = " << seconds << ")");

    StartTimerMs(name, seconds * 1000);
}

void ScriptData::StartTimerMs(const std::string &name, int ms)
{
    LOG_ERROR(logger, "StartTimerMs(" << name << ", ms = " << ms << ")");

    SetVar(name, GET_SETTINGS().playtime + ms);
}

void ScriptData::StartTimerMin(const std::string &name, int min)
{
    LOG_ERROR(logger, "StartTimerMin(" << name << ", min = " << min << ")");

    StartTimer(name, min * 60);
}

bool ScriptData::IsTimerExpired(const std::string &name)
{
    LOG_ERROR(logger, "IsTimerExpired(" << name << ")");

    auto v = GetVar(name);
    if (v == 0)
        return false;
    return v < GET_SETTINGS().playtime;
}

void AddText(const std::string &text)
{
    LOG_TRACE(logger, "AddText(" << text << ")");

    auto &t = getScreenText();
    t += String(text);
}

void AddTextOnce(const std::string &text)
{
    LOG_TRACE(logger, "AddTextOnce(" << text << ")");

    auto &t = getScreenText();
    auto s = String(text);
    auto pos = t.rfind(s);
    if (pos == t.npos || pos + s.size() != t.size())
        AddText(text);
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

    GET_BUILDING_MENU()->clearText();
}

void ClearThemes()
{
    LOG_TRACE(logger, "ClearThemes()");

    GET_BUILDING_MENU()->clearThemes();
}

void Clear()
{
    LOG_TRACE(logger, "Clear()");

    ClearText();
    ClearThemes();
}

static void AddMessage(const std::string &message_id, bool clear)
{
    if (clear)
        ClearText();
    GET_BUILDING_MENU()->addTheme(get_message_by_id(message_id));
}

void AddTheme(const std::string &message_id)
{
    LOG_TRACE(logger, "AddTheme(" << message_id << ")");

    AddMessage(message_id, false);
}

void AddMessage(const std::string &message_id)
{
    LOG_TRACE(logger, "AddMessage(" << message_id << ")");

    GET_BUILDING_MENU()->addMessage(get_message_by_id(message_id));
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

void Log(const std::string &text)
{
    LOG_DEBUG("script_log", text);
}

} // namespace script

} // namespace polygon4
