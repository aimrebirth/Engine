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

enum class Rating
{
    Normal,
    Courier,
    Fight,
    Trade,
};

struct ScriptData
{
#ifndef SWIG
    ScriptEngine *scriptEngine;
#endif
    polygon4::detail::ModificationPlayer *player;
    std::string building_name;

    void AddObject(const std::string &o);
    void AddMoney(float amount);
    void AddRating(float amount);
    void AddRating(Rating type, float amount);

    // add_quest or start_quest
    // set_target (_mark) or set_pointer
    // set_event = set quest stage
    //void AddJournalRecord();

    float GetRating() const;
    float GetRating(Rating type) const;

    int GetRatingLevel() const;
    int GetRatingLevel(Rating type) const;

    bool HasMoney(float amount) const;
    bool HasRating(float amount) const;
    bool HasRating(Rating type, float amount) const;
    bool HasRatingLevel(int level) const;
    bool HasRatingLevel(Rating type, int level) const;

    void SetRatingLevel(int level) const;
    void SetRatingLevel(Rating type, int level) const;

private:
    static int getRatingLevel(float rating);
    static void setRatingLevel(float &rating, int level);
    float &getRating(Rating type = Rating::Normal) const;
    float &getMoney() const;
};

struct ScreenText
{
    ScreenText operator+(const std::string &s);
    ScreenText __concat__(const std::string &s);

#ifndef SWIG
    String *screenText = nullptr;
#endif
};

void AddMessage(::polygon4::String &screenText, polygon4::detail::Message *message);
void AddMessage(const std::string &message_id);
void AddText(const std::string &text);

void ShowMessage(const std::string &message_id);
void ShowText(const std::string &text);

void ClearText();

ScreenText GetScreenText();

} // namespace script

using script::ScriptData;

} // namespace polygon4
