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

#include <functional>

#include "dll.h"
#include "Common.h"

namespace polygon4
{

struct API
{
    std::function<void(std::string)> OpenLevel;
};

extern API api;

DLL_EXPORT
void InitAPI(API api);

}

#define API_CALL(func, msg, ...) \
    LOG_DEBUG(logger, #func << msg); \
    if (api.func) \
        api.func( ( __VA_ARGS__ ) ); \
    else \
        LOG_ERROR(logger, "API call is not set: " << #func)

