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

#include <Polygon4/Mechanoid.h>

#include <Polygon4/BuildingMenu.h>
#include <Polygon4/Configuration.h>
#include <Polygon4/Engine.h>

#include "Executor.h"
#include "Script.h"

#include <tools/Logger.h>
DECLARE_STATIC_LOGGER(logger, "mechanoid");

const std::vector<float> &get_rating_levels()
{
    static const std::vector<float> rating_levels{
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
    return rating_levels;
}

namespace polygon4
{

Mechanoid::Mechanoid(const Base &rhs)
    : Base(rhs)
{
}

detail::Configuration *Mechanoid::getConfiguration()
{
    if (!configuration)
    {
        configuration = getStorage()->configurations.createAtEnd(*initial_configuration);
        configuration->deepCopyFrom(*initial_configuration);
    }
    replace<Configuration>(configuration.get());
    return configuration;
}

void Mechanoid::enterBuilding(detail::MapBuilding *bld)
{
    if (!bld)
        return;
    auto mmb = bld->getModificationMapBuilding();
    if (!mmb)
        return;
    building = mmb;
    if (!isPlayer())
    {
        // handle bot's building visit
        return;
    }

    // player is now officially in the building
    // we need to run scripts, setup texts etc.

    // set building, mechanoid
    auto bm = getEngine()->getBuildingMenu();
    bm->SetCurrentBuilding(mmb);
    bm->SetCurrentMechanoid(this);

    // now run scripts
    auto se = mmb->map->modification->getScriptEngine();
    path script_file = path("maps") / mmb->map->script_dir.toString() / mmb->script_name.toString();
    auto s = se->getScript(script_file.string());

    ScriptData data;
    data.scriptEngine = se;
    data.player = player;
    data.building_name = mmb->text_id;
    s->OnEnterBuilding(data);

    // update building menu
    bm->refresh();
    getEngine()->ShowBuildingMenu();

    // do async save to not freeze the game
    // the ideal algorithm here is:
    // 1. freeze the game
    // 2. async({ save(); unfreeze(); });
    getTaskExecutor().push([]()
    {
        getEngine()->saveAuto();
    });
}

int Mechanoid::getRatingLevel(RatingType type) const
{
    return _getRatingLevel(getRating(type));
}

bool Mechanoid::hasRatingLevel(int level, RatingType type) const
{
    return getRatingLevel(type) >= level;
}

void Mechanoid::setRatingLevel(int level, RatingType type)
{
    setRating(_setRatingLevel(level), type);
}

int Mechanoid::_getRatingLevel(float rating)
{
    int i = 0;
    while (rating > get_rating_levels()[i++] && i < get_rating_levels().size());
    return i;
}

float Mechanoid::_setRatingLevel(int level)
{
    float rating = 0.0f;
    if (level > get_rating_levels().size())
        rating = get_rating_levels().size();
    rating = get_rating_levels()[level - 1];
    return rating;
}

float Mechanoid::getRating(RatingType type) const
{
    switch (type)
    {
    case RatingType::Courier:
        return rating_courier;
    case RatingType::Fight:
        return rating_fight;
    case RatingType::Trade:
        return rating_trade;
    }
    return rating;
}

bool Mechanoid::hasRating(float r, RatingType type) const
{
    return getRating(type) >= r;
}

void Mechanoid::setRating(float r, RatingType type)
{
    float *pr = &rating;
    switch (type)
    {
    case RatingType::Courier:
        pr = &rating_courier;
        break;
    case RatingType::Fight:
        pr = &rating_fight;
        break;
    case RatingType::Trade:
        pr = &rating_trade;
        break;
    }
    *pr = r;
    if (*pr < 1)
        *pr = 1;
}

float Mechanoid::getMoney() const
{
    return money;
}

bool Mechanoid::hasMoney(float m) const
{
    return money >= m;
}

void Mechanoid::setMoney(float m)
{
    money = m;
    if (money < 0)
        money = 0;
}

float getRatingLevelCap(int level)
{
    if (level <= 0)
        return 0;
    if (level > get_rating_levels().size())
        return 0;
    return get_rating_levels()[level - 1];
}

} // namespace polygon4
