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

#include <memory>
#include <string>
#include <set>

#include <Polygon4/DataManager/Types.h>

namespace polygon4
{

class Mechanoid;

class DLL_EXPORT Configuration : public detail::Configuration
{
    using Base = detail::Configuration;

public:
    Configuration(const Base &);

    void setMechanoid(Mechanoid *mechanoid);

    virtual void addItem(IObjectBase *o, int quantity = 1) override final;
    virtual bool hasItem(const IObjectBase *o, int quantity = 1) const override final;

    virtual void addEquipment(detail::Equipment *e, int quantity = 1) override final;
    virtual void addGlider(detail::Glider *g) override final;
    virtual void addGood(detail::Good *g, int quantity = 1) override final;
    virtual void addModificator(detail::Modificator *m, int quantity = 1) override final;
    virtual void addProjectile(detail::Projectile *p, int quantity = 1) override final;
    virtual void addWeapon(detail::Weapon *w) override final;

    virtual float getMass() const override final;
    virtual float getTotalMass() const override final;
    virtual float getCapacity() const override final;

    virtual float getCurrentEnergy() const override final;
    virtual float getMaxEnergy() const override final;

    virtual float getCurrentEnergyShield() const override final;
    virtual float getMaxEnergyShield() const override final;

    virtual float getCurrentArmor() const override final;
    virtual float getMaxArmor() const override final;

private:
    Mechanoid *mechanoid = nullptr;
};

} // namespace polygon4

