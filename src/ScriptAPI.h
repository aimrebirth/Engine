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

class Script;

namespace script
{

enum class RatingType
{
    Normal,
    Courier,
    Fight,
    Trade,
};

enum class QuestRecord
{
    InProgress = 1,
    Completed = 2,
};

struct ScriptData
{
#ifndef SWIG
    // script won't see any data
    Script *script = nullptr;
    polygon4::detail::ModificationPlayer *player = nullptr;
    polygon4::detail::ModificationMapBuilding *building = nullptr;
    polygon4::detail::Message *next_quest = nullptr;
#endif

    // main
    void AddItem(const std::string &o, int quantity = 1);
    bool HasItem(const std::string &o, int quantity = 1);
    bool RemoveItem(const std::string &o, int quantity = 1);

    // rating
    void AddRating(float amount, RatingType type = RatingType::Normal);
    float GetRating(RatingType type = RatingType::Normal) const;
    bool HasRating(float amount, RatingType type = RatingType::Normal) const;
    void SetRating(float amount, RatingType type = RatingType::Normal);

    int GetRatingLevel(RatingType type = RatingType::Normal) const;
    bool HasRatingLevel(int level, RatingType type = RatingType::Normal) const;
    void SetRatingLevel(int level, RatingType type = RatingType::Normal) const;

    // money
    void AddMoney(float amount);
    float GetMoney() const;
    bool HasMoney(float amount) const;
    void SetMoney(float amount);

    // journal
    void AddJournalRecord(const std::string &message_id, QuestRecord type = QuestRecord::InProgress);
    void MarkJournalRecord(const std::string &message_id, QuestRecord type);
    void MarkJournalRecordCompleted(const std::string &message_id);

    // variable methods
    int GetVar(const std::string &var);
    void SetVar(const std::string &var, int i = 1);
    void SetVar(const std::string &var, const std::string &val);
    void UnsetVar(const std::string &var);
    bool CheckVar(const std::string &var);
    bool RunOnce(const std::string &var);

    // name
    std::string GetName() const;
    void SetName(const std::string &name) const;

    // map
    void SetPointer(const std::string &bld);

    // state
    bool IsDamaged(float percent = 0.0f) const; // is damaged <= %
    bool IsDamagedHigh() const;
    bool IsDamagedLow() const;

    // quest
    // add_quest or start_quest
    // set_event = set quest stage
    void RegisterQuest(const std::string &name);
    bool IsQuestAvailable() const;
    void ListAvailableQuests() const;
    void AcceptQuest(const std::string &name);
    void AddQuestRecord(const std::string &message_id, QuestRecord type = QuestRecord::InProgress);
    void MarkQuestRecord(const std::string &message_id, QuestRecord type);
    void MarkQuestRecordCompleted(const std::string &message_id);

    // timer
    void StartTimer(const std::string &name, int seconds);
    void StartTimerMs(const std::string &name, int ms);
    void StartTimerMin(const std::string &name, int min);
    bool IsTimerExpired(const std::string &name);
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
void AddTheme(const std::string &message_id);
void AddText(const std::string &text);
void AddTextOnce(const std::string &text);

void ShowMessage(const std::string &message_id);
void ShowText(const std::string &text);

void ClearText();
void ClearThemes();
void Clear();

ScreenText GetScreenText();

void Log(const std::string &text);

} // namespace script

using script::ScriptData;

} // namespace polygon4
