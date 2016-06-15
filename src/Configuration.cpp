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

#include <Polygon4/Configuration.h>

#include <Polygon4/ConfigurationWeapon.h>
#include <Polygon4/Engine.h>
#include <Polygon4/Mechanoid.h>

#include <tools/Logger.h>
DECLARE_STATIC_LOGGER(logger, "configuration");

namespace polygon4
{

Configuration::Configuration(const Base &rhs)
    : Base(rhs)
{
}

void Configuration::setMechanoid(Mechanoid *m)
{
    if (m)
        mechanoid = m;
}

void Configuration::addItem(IObjectBase *o, int quantity)
{
    using polygon4::detail::EObjectType;

    switch (o->getType())
    {
    case EObjectType::Equipment:
        addEquipment((detail::Equipment *)o, quantity);
        break;
    case EObjectType::Glider:
        addGlider((detail::Glider *)o);
        break;
    case EObjectType::Weapon:
        for (auto i = 0; i < quantity; i++)
            addWeapon((detail::Weapon *)o);
        break;
    case EObjectType::Modificator:
        addModificator((detail::Modificator *)o, quantity);
        break;
    case EObjectType::Good:
        addGood((detail::Good *)o, quantity);
        break;
    case EObjectType::Projectile:
        addProjectile((detail::Projectile *)o, quantity);
        break;
    default:
        return;
    }
}

void Configuration::addEquipment(detail::Equipment *o, int quantity)
{
    auto i = std::find_if(equipments.begin(), equipments.end(),
        [o](const auto &e) { return e->equipment.get() == o; });
    if (i != equipments.end())
    {
        auto e = *i;
        if (e->quantity == e->equipment->max_count)
        {
            mechanoid->sell(e->equipment->price * quantity);
            return;
        }
        e->quantity++;
        return;
    }
    auto s = getStorage();
    auto v = s->configurationEquipments.createAtEnd();
    v->configuration = this;
    v->equipment = o;
    v->quantity = quantity;
    equipments.push_back(v);
}

void Configuration::addGlider(detail::Glider *o)
{
    mechanoid->sell(glider->price);
    glider = o;
}

void Configuration::addGood(detail::Good *o, int quantity)
{
    auto i = std::find_if(goods.begin(), goods.end(),
        [o](const auto &e) { return e->good.get() == o; });
    if (i != goods.end())
    {
        auto e = *i;
        if (e->quantity == e->good->max_count)
        {
            mechanoid->sell(e->good->price * quantity);
            return;
        }
        e->quantity++;
        return;
    }
    auto s = getStorage();
    auto v = s->configurationGoods.createAtEnd();
    v->configuration = this;
    v->good = o;
    v->quantity = quantity;
    goods.push_back(v);
}

void Configuration::addModificator(detail::Modificator *o, int quantity)
{
    auto i = std::find_if(modificators.begin(), modificators.end(),
        [o](const auto &e) { return e->modificator.get() == o; });
    if (i != modificators.end())
    {
        auto e = *i;
        if (e->quantity == e->modificator->max_count)
        {
            mechanoid->sell(e->modificator->price * quantity);
            return;
        }
        e->quantity++;
        return;
    }
    auto s = getStorage();
    auto v = s->configurationModificators.createAtEnd();
    v->configuration = this;
    v->modificator = o;
    v->quantity = quantity;
    modificators.push_back(v);
}

void Configuration::addProjectile(detail::Projectile *o, int quantity)
{
    auto i = std::find_if(projectiles.begin(), projectiles.end(),
        [o](const auto &e) { return e->projectile.get() == o; });
    if (i != projectiles.end())
    {
        auto e = *i;
        e->quantity += quantity;
        return;
    }
    auto s = getStorage();
    auto v = s->configurationProjectiles.createAtEnd();
    v->configuration = this;
    v->projectile = o;
    v->quantity = quantity;
    projectiles.push_back(v);
}

void Configuration::addWeapon(detail::Weapon *w)
{
    // glider cannot take such weapon type)
    if (glider->standard < w->standard)
    {
        // found, so selling
        mechanoid->sell(w->price);
        return;
    }

    // we do not allow to transport weapons in the hold atm
    switch (glider->special_type)
    {
    case detail::GliderSpecialType::Normal:
    {
        // find same weapon
        auto i = std::find_if(weapons.begin(), weapons.end(),
            [w](const auto &e) { return e->weapon.get() == w; });
        if (i != weapons.end())
        {
            // found, so selling
            mechanoid->sell(w->price);
            return;
        }

        // find same weapon type
        i = std::find_if(weapons.begin(), weapons.end(),
            [w](const auto &e) { return e->weapon.get()->type == w->type; });
        if (i != weapons.end())
        {
            // found, so replacing
            auto e = *i;
            mechanoid->sell(e->weapon->price);
            e->weapon = w;
            return;
        }

        // not found, creating below...
    }
        break;
    case detail::GliderSpecialType::TwoLightWeapons:
    {
        // wrong type
        if (w->type == detail::WeaponType::Heavy)
        {
            // found, so selling
            mechanoid->sell(w->price);
            return;
        }

        // one pylon is empty, creating below...
        if (weapons.size() < 2)
            break;

        // find same weapon
        if (weapons[0]->weapon.get() == w)
        {
            if (weapons[1]->weapon.get() == w)
            {
                // both found, so selling
                mechanoid->sell(w->price);
                return;
            }
            // 1 found, so replacing
            mechanoid->sell(weapons[1]->weapon->price);
            weapons[1]->weapon = w;
            return;
        }
        else if (weapons[1]->weapon.get() == w)
        {
            if (weapons[0]->weapon.get() == w)
            {
                // both found, so selling
                mechanoid->sell(w->price);
                return;
            }
            // 1 found, so replacing
            mechanoid->sell(weapons[0]->weapon->price);
            weapons[0]->weapon = w;
            return;
        }

        // not found, creating below...
        // by default at first pos atm
        weapons.assign(weapons.begin() + 1, weapons.end());
    }
        break;
    case detail::GliderSpecialType::TwoHeavyWeapons:
    {
        // wrong type
        if (w->type == detail::WeaponType::Light)
        {
            // found, so selling
            mechanoid->sell(w->price);
            return;
        }

        // one pylon is empty, creating below...
        if (weapons.size() < 2)
            break;

        // find same weapon
        if (weapons[0]->weapon.get() == w)
        {
            if (weapons[1]->weapon.get() == w)
            {
                // both found, so selling
                mechanoid->sell(w->price);
                return;
            }
            // 1 found, so replacing
            mechanoid->sell(weapons[1]->weapon->price);
            weapons[1]->weapon = w;
            return;
        }
        else if (weapons[1]->weapon.get() == w)
        {
            if (weapons[0]->weapon.get() == w)
            {
                // both found, so selling
                mechanoid->sell(w->price);
                return;
            }
            // 1 found, so replacing
            mechanoid->sell(weapons[0]->weapon->price);
            weapons[0]->weapon = w;
            return;
        }

        // not found, creating below...
        // by default at first pos atm
        weapons.assign(weapons.begin() + 1, weapons.end());
    }
        break;
    case detail::GliderSpecialType::NoWeapons:
    {
        mechanoid->sell(w->price);
        return;
    }
        break;
    }
    auto s = getStorage();
    auto v = s->configurationWeapons.createAtEnd();
    replace<ConfigurationWeapon>(v);
    v->configuration = this;
    v->weapon = w;
    weapons.push_back(v);
}

bool Configuration::hasItem(const IObjectBase *o, int quantity) const
{
    if (glider.get() == o)
        return true;

#define CHECK_ITEMS(v)                                                         \
    do                                                                         \
    {                                                                          \
        auto i = std::find_if(v##s.begin(), v##s.end(),                        \
                              [o](const auto &e) { return e->v.get() == o; }); \
        if (i != v##s.end())                                                   \
        {                                                                      \
            auto e = *i;                                                       \
            if (e->quantity >= quantity)                                       \
                return true;                                                   \
            return false;                                                      \
        }                                                                      \
    } while (0)

#define CHECK_ITEMS_NO_QUANTITY(v)                                             \
    do                                                                         \
    {                                                                          \
        auto i = std::find_if(v##s.begin(), v##s.end(),                        \
                              [o](const auto &e) { return e->v.get() == o; }); \
        if (i != v##s.end())                                                   \
            return true;                                                       \
    } while (0)

    CHECK_ITEMS(equipment);
    CHECK_ITEMS(good);
    CHECK_ITEMS(modificator);
    CHECK_ITEMS(projectile);
    CHECK_ITEMS_NO_QUANTITY(weapon);

    return false;
}

float Configuration::getMass() const
{
    float mass = 0.0f;

#define ADD_MASS(m) for (auto &v : m ## s) mass += v->m->weight
    ADD_MASS(equipment);
    ADD_MASS(good);
    ADD_MASS(projectile);
    ADD_MASS(weapon);

    return mass;
}

float Configuration::getTotalMass() const
{
    return getMass() + glider->weight;
}

float Configuration::getCapacity() const
{
    return glider->getCapacity();
}

float Configuration::getCurrentEnergy() const
{
    return energy;
}

float Configuration::getMaxEnergy() const
{
    float max_energy = 0.0f;

    // additions from equipment
    for (auto &v : equipments)
    {
        if (v->equipment->type == detail::EquipmentType::Reactor)
            max_energy += v->equipment->value1 * 3 * 10;
    }

    return max_energy;
}

float Configuration::getCurrentEnergyShield() const
{
    return energy_shield;
}

float Configuration::getMaxEnergyShield() const
{
    float max_energy_shield = 0.0f;

    // additions from equipment
    for (auto &v : equipments)
    {
        if (v->equipment->type == detail::EquipmentType::Shield)
            max_energy_shield += v->equipment->value1;
    }

    return max_energy_shield;
}

float Configuration::getCurrentArmor() const
{
    return armor;
}

float Configuration::getMaxArmor() const
{
    float max_armor = glider->armor;

    // additions from equipment
    /*for (auto &v : equipments)
    {
    if (v->equipment->type == detail::EquipmentType::Armor)
    armor += v->equipment->value1;
    }*/

    return max_armor;
}

} // namespace polygon4
