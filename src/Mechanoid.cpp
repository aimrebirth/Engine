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

#include <regex>

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
        10'000'000,
    };
    return rating_levels;
}

namespace polygon4
{

float getRatingLevelCap(int level)
{
    if (level <= 0)
        return 0;
    if (level > get_rating_levels().size())
        return 0;
    return get_rating_levels()[level - 1];
}

Mechanoid::Mechanoid(const Base &rhs)
    : Base(rhs)
{
}

detail::Configuration *Mechanoid::getConfiguration()
{
    bool created = false;
    if (!configuration)
    {
        // do not create new configuration while in db tool mode
        if (getSettings().flags[gfDbTool])
            return initial_configuration;

        configuration = getStorage()->configurations.createAtEnd(*initial_configuration);
        configuration->deepCopyFrom(*initial_configuration);

        // to differ from initial configurations in DB Tool
        configuration->text_id += L" - " + getName();

        created = true;
    }

    auto c = replace<Configuration>(configuration.get());

    if (created)
    {
        c->setMechanoid(this);
        c->armor = c->getMaxArmor();
    }
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
    auto e = getEngine();

    // set building, mechanoid
    auto bm = e->getBuildingMenu();
    bm->setCurrentBuilding(mmb);
    bm->setCurrentMechanoid(this);
    bm->clearText();

    // now run scripts
    auto se = mmb->map->modification->getScriptEngine();
    path script_file = path("maps") / mmb->map->script_dir.toString() / mmb->script_name.toString();
    auto s = se->getScript(script_file.string());

    // set player visit
    auto iter = player->buildings.find_if([mmb](const auto &vb)
    {
        return vb->building.get() == mmb;
    });
    if (iter == player->buildings.end())
    {
        auto vb = e->getStorage()->modificationPlayerBuildings.createAtEnd();
        vb->player = player;
        vb->building = mmb;
        vb->know_location = true;
        vb->visited = true;
        player->buildings.insert(vb);
    }
    else
    {
        (*iter)->know_location = true;
        (*iter)->visited = true;
    }

    // set script data
    auto data = std::make_shared<ScriptData>();
    data->scriptEngine = se;
    data->player = player;
    data->building = mmb;

    // set script callback
    // pass script ptr and data object, because it can be called outside of this function
    bm->setCurrentScriptCallback([s, data](const auto &fn) mutable { s->call(fn, *data); });

    // register available building quests
    s->RegisterQuests(*data);

    // main script call
    s->OnEnterBuilding(*data);

    // update building menu
    bm->refresh();
    e->ShowBuildingMenu();

    // do async save to not freeze the game
    // the ideal algorithm here is:
    // 1. freeze the game
    // 2. async({ save(); unfreeze(); });
    getTaskExecutor().push([e]()
    {
        e->saveAuto();
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

String Mechanoid::getRatingLevelName(RatingType type) const
{
    auto level = getRatingLevel(type);
    auto m = (detail::Message*)getEngine()->getObjects()[L"RATING." + std::to_wstring(level)];
    if (m->txt)
        return m->txt->string;
    return m->getName();
}

void Mechanoid::addMoney(float m)
{
    setMoney(getMoney() + m);
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

bool Mechanoid::buy(float money)
{
    if (money < 0 || !hasMoney(money))
        return false;
    addMoney(-money);
    return true;
}

void Mechanoid::sell(float money)
{
    addMoney(money);
}

bool Mechanoid::setName(const String &n)
{
    // name: min 2, max 30 symbols
    static std::wregex r_name(LR"([\w\d][\w\d-_]+)");
    if (!std::regex_match(n, r_name) || n.size() > 30)
        return false;
    if (!name)
    {
        auto s = GET_STORAGE()->strings.createAtEnd();
        s->object = detail::EObjectType::Mechanoid;
        name = s;
    }
    name->string = n;
    return true;
}

} // namespace polygon4
