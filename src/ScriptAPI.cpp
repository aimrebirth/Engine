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

#include "Script.h"

#include <tools/Logger.h>
DECLARE_STATIC_LOGGER(logger, "script_api");

namespace polygon4
{

namespace script
{

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

    player->mechanoid->money += amount;
}

void ScriptData::AddRating(float amount)
{
    LOG_TRACE(logger, "AddRating(" << amount << ")");

    player->mechanoid->rating += amount;
}

void ScriptData::AddRating(Rating type, float amount)
{
    LOG_TRACE(logger, "AddRating(" << amount << "), type: " << static_cast<int>(type));

    switch (type)
    {
    case Rating::Normal:
        AddRating(amount);
        break;
    case Rating::Courier:
        player->mechanoid->rating_courier += amount;
        break;
    case Rating::Fight:
        player->mechanoid->rating_fight += amount;
        break;
    case Rating::Trade:
        player->mechanoid->rating_trade += amount;
        break;
    }
}

int ScriptData::GetRatingLevel() const
{
    LOG_TRACE(logger, "GetRatingLevel()");

    return GetRatingLevel(player->mechanoid->rating);
}

int ScriptData::GetRatingLevel(Rating type) const
{
    LOG_TRACE(logger, "GetRatingLevel(), type: " << static_cast<int>(type));

    switch (type)
    {
    case Rating::Normal:
        return GetRatingLevel();
    case Rating::Courier:
        return GetRatingLevel(player->mechanoid->rating_courier);
    case Rating::Fight:
        return GetRatingLevel(player->mechanoid->rating_fight);
    case Rating::Trade:
        return GetRatingLevel(player->mechanoid->rating_trade);
    }
    return 0;
}

int ScriptData::GetRatingLevel(int rating) const
{
    if (0);
    else if (rating <         50) return 1;
    else if (rating <        200) return 2;
    else if (rating <        500) return 3;
    else if (rating <      1'500) return 4;
    else if (rating <      5'000) return 5;
    else if (rating <     15'000) return 6;
    else if (rating <     50'000) return 7;
    else if (rating <    200'000) return 8;
    else if (rating <  1'000'000) return 9;
    //else if (rating < 10'000'000) return 10;
    return 10;
}

bool ScriptData::HasMoney(float amount) const
{
    LOG_TRACE(logger, "HasMoney(" << amount << ")");

    return player->mechanoid->money >= amount;
}

bool ScriptData::HasRating(float amount) const
{
    LOG_TRACE(logger, "HasRating(" << amount << ")");

    return player->mechanoid->rating >= amount;
}

bool ScriptData::HasRating(Rating type, float amount) const
{
    LOG_TRACE(logger, "HasRating(" << amount << "), type: " << static_cast<int>(type));

    switch (type)
    {
    case Rating::Normal:
        return HasRating(amount);
    case Rating::Courier:
        return player->mechanoid->rating_courier >= amount;
    case Rating::Fight:
        return player->mechanoid->rating_fight >= amount;
    case Rating::Trade:
        return player->mechanoid->rating_trade >= amount;
    }
    return false;
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

} // namespace script

void ShowText(const std::string &text_id)
{

}

} // namespace polygon4
