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

#include <Polygon4/API.h>

#include <assert.h>

#include <tools/Logger.h>
DECLARE_STATIC_LOGGER(logger, "api");

namespace polygon4
{

void RegisterAPI(API api)
{
    getAPI().registerAPI(api);
}

void UnregisterAPI(API api)
{
    getAPI().unregisterAPI(api);
}

void API::registerAPI(API rhs)
{
#define API_FUNCTION(type, name) \
    if (rhs.name) \
    { \
        name = rhs.name; \
        LOG_TRACE(logger, "Register API: " << #type << " " << #name); \
    }
#include <Polygon4/API_functions.h>
#undef API_FUNCTION
}

void API::unregisterAPI(API rhs)
{
#define API_FUNCTION(type, name) \
    if (rhs.name) \
    { \
        name.swap(type()); \
        LOG_TRACE(logger, "Unregister API: " << #type << " " << #name); \
    }
#include <Polygon4/API_functions.h>
#undef API_FUNCTION
}

API &getAPI()
{
    static API api;
    return api;
}

}

