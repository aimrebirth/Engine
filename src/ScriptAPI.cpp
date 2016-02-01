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
    using polygon4::detail::EObjectType;

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
    // also conf->addObject should be used as grand dispatcher
    switch (o->getType())
    {
    case EObjectType::Equipment:
        //conf->equipments.push_back((polygon4::detail::Equipment*)o);
        //conf->addEquipment();
        /*
        auto ce = storage->ConfigurationEquipments.createAtEnd()
        ce -> conf = this;
        ce -> equ = obj;
        */
        break;
    case EObjectType::Glider:
        break;
    case EObjectType::Weapon:
        break;
    case EObjectType::Modificator:
        break;
    case EObjectType::Good:
        break;
    case EObjectType::Projectile:
        break;
    default:
        return;
    }
}

} // namespace script

} // namespace polygon4
