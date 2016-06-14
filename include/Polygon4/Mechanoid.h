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

#include "Actions.h"

namespace polygon4
{

using detail::RatingType;

class DLL_EXPORT Mechanoid : public detail::Mechanoid
{
    using Base = detail::Mechanoid;

public:
    Mechanoid(const Base &);

    virtual detail::ModificationPlayer *getPlayer() const override final { return player; }
    virtual void setPlayer(detail::ModificationPlayer *in_player) override final { player = in_player; }
    virtual bool isPlayer() const override final { return !!player; }

    virtual void enterBuilding(detail::MapBuilding *building) override final;

    virtual detail::Configuration *getConfiguration() override final;

    virtual void addMoney(float m) override final;
    virtual float getMoney() const override final;
    virtual bool hasMoney(float m) const override final;
    virtual void setMoney(float m) override final;

    virtual float getRating(RatingType type = RatingType::Normal) const override final;
    virtual bool hasRating(float rating, RatingType type = RatingType::Normal) const override final;
    virtual void setRating(float rating, RatingType type = RatingType::Normal) override final;

    virtual int getRatingLevel(RatingType type = RatingType::Normal) const override final;
    virtual bool hasRatingLevel(int level, RatingType type = RatingType::Normal) const override final;
    virtual void setRatingLevel(int level, RatingType type = RatingType::Normal) override final;
    virtual String getRatingLevelName(RatingType type = RatingType::Normal) const override final;

    virtual bool buy(float money) override final;
    virtual void sell(float money) override final;

    virtual bool setName(const String &name) override final;

protected:
    detail::ModificationPlayer *player = nullptr;

private:
    static int _getRatingLevel(float rating);
    static float _setRatingLevel(int level);
};

DLL_EXPORT
float getRatingLevelCap(int level);

} // namespace polygon4

