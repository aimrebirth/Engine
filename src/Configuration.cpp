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

#define FIND_ITEMS(v)                                                          \
    do                                                                         \
    {                                                                          \
        auto i = std::find_if(v##s.begin(), v##s.end(),                        \
                              [o](const auto &e) { return e->v.get() == o; }); \
        if (i != v##s.end())                                                   \
        {                                                                      \
            auto e = *i;                                                       \
            if (e->quantity >= quantity)                                       \
            {                                                                  \
                REMOVE_ACTION_QUANTITY(v);                                     \
                return true;                                                   \
            }                                                                  \
            return false;                                                      \
        }                                                                      \
    } while (0)

#define FIND_ITEMS_NO_QUANTITY(v)                                              \
    do                                                                         \
    {                                                                          \
        auto i = std::find_if(v##s.begin(), v##s.end(),                        \
                              [o](const auto &e) { return e->v.get() == o; }); \
        if (i != v##s.end())                                                   \
        {                                                                      \
            REMOVE_ACTION(v);                                                  \
            return true;                                                       \
        }                                                                      \
    } while (0)

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

#define REMOVE_ACTION(v)
#define REMOVE_ACTION_QUANTITY(v)

    FIND_ITEMS(equipment);
    FIND_ITEMS(good);
    FIND_ITEMS(modificator);
    FIND_ITEMS(projectile);
    FIND_ITEMS_NO_QUANTITY(weapon);

#undef REMOVE_ACTION_QUANTITY
#undef REMOVE_ACTION

    return false;
}

bool Configuration::removeItem(IObjectBase *o, int quantity)
{
    if (glider.get() == o)
    {
        glider.reset();
        return true;
    }

#define REMOVE_ACTION(v)                                                           \
    do                                                                             \
    {                                                                              \
        v##s.erase(std::remove_if(v##s.begin(), v##s.end(),                        \
                                  [o](const auto &e) { return e->v.get() == o; }), \
                   v##s.end());                                                    \
    } while (0)

#define REMOVE_ACTION_QUANTITY(v) \
    do                            \
    {                             \
        e->quantity -= quantity;  \
        if (e->quantity == 0)     \
        {                         \
            REMOVE_ACTION(v);     \
        }                         \
    } while (0)

    FIND_ITEMS(equipment);
    FIND_ITEMS(good);
    FIND_ITEMS(modificator);
    FIND_ITEMS(projectile);
    FIND_ITEMS_NO_QUANTITY(weapon);

#undef REMOVE_ACTION_QUANTITY
#undef REMOVE_ACTION

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
        if (v->equipment->type == detail::EquipmentType::EnergyShield)
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

bool Configuration::isDead() const
{
    return getCurrentArmor() <= 0;
}

void Configuration::hit(detail::Projectile *projectile)
{
    if (!projectile)
        return;

    auto damage = projectile->damage;

    // additions from equipment
    bool has_shield = false;
    for (auto &v : equipments)
    {
        if (v->equipment->type == detail::EquipmentType::EnergyShield)
        {
            has_shield = true;
            if (damage <= v->equipment->value2)
            {
                // full absorb
                energy_shield -= damage;
                if (energy_shield < 0)
                {
                    // pass to armor
                    armor -= -energy_shield;
                    energy_shield = 0;
                }
            }
            else
            {
                // partial absorb
                energy_shield -= v->equipment->value2;
                damage -= v->equipment->value2;
                if (energy_shield < 0)
                {
                    // pass to armor
                    armor -= -energy_shield;
                    energy_shield = 0;
                }
                armor -= damage;
            }
            // handle only one shield atm
            break;
        }
    }
    if (!has_shield)
    {
        armor -= damage;
    }

    if (armor < 0)
        armor = 0;

    if (armor == 0)
    {
        energy = 0;
        energy_shield = 0;
    }
}

void Configuration::tick(float delta_seconds)
{
    // TODO: fix calculations

    // shield energy consumption & restore
    for (auto &v : equipments)
    {
        if (v->equipment->type == detail::EquipmentType::EnergyShield)
        {
            // consumption
            energy -= v->equipment->power * delta_seconds;

            auto max = getMaxEnergyShield();
            if (energy > 0 && energy_shield < max)
            {
                // restore
                energy_shield += v->equipment->value3 * delta_seconds;
                if (energy_shield > max)
                    energy_shield = max;
            }

            // handle only one shield atm
            break;
        }
    }

    // energy restore
    for (auto &v : equipments)
    {
        if (v->equipment->type == detail::EquipmentType::Reactor)
        {
            energy += v->equipment->value1 * delta_seconds;
            auto max = getMaxEnergy();
            if (energy > max)
                energy = max;
            // handle only one generator atm
            break;
        }
    }

    // should be after restore
    if (energy < 0)
        energy = 0;

    // weapon timers
    for (auto &w : weapons)
        w->addTime(delta_seconds);
}

} // namespace polygon4
