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

#include <Polygon4/DataManager/Types.h>

namespace polygon4
{

class ScriptEngine;

namespace script
{

enum class RatingType
{
    Normal,
    Courier,
    Fight,
    Trade,
};

enum class JournalRecord
{
    InProgress = 1,
    Completed = 2,
};

struct ScriptData
{
#ifndef SWIG
    ScriptEngine *scriptEngine;
#endif
    polygon4::detail::ModificationPlayer *player;
    std::string building_name;

    void AddObject(const std::string &o, int quantity = 1);

    void AddRating(float amount, RatingType type = RatingType::Normal);
    float GetRating(RatingType type = RatingType::Normal) const;
    bool HasRating(float amount, RatingType type = RatingType::Normal) const;
    void SetRating(float amount, RatingType type = RatingType::Normal);

    int GetRatingLevel(RatingType type = RatingType::Normal) const;
    bool HasRatingLevel(int level, RatingType type = RatingType::Normal) const;
    void SetRatingLevel(int level, RatingType type = RatingType::Normal) const;

    void AddMoney(float amount);
    float GetMoney() const;
    bool HasMoney(float amount) const;
    void SetMoney(float amount);

    // add_quest or start_quest
    // set_target (_mark) or set_pointer
    // set_event = set quest stage
    void AddJournalRecord(const std::string &message_id, JournalRecord type = JournalRecord::InProgress);
};

struct ScreenText
{
    ScreenText operator+(const std::string &s);
    ScreenText __concat__(const std::string &s);

#ifndef SWIG
    String *screenText = nullptr;
#endif
};

void AddMessage(const std::string &message_id);
void AddText(const std::string &text);

void ShowMessage(const std::string &message_id);
void ShowText(const std::string &text);

void ClearText();

ScreenText GetScreenText();

} // namespace script

using script::ScriptData;

} // namespace polygon4
