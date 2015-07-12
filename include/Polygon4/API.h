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
#include <memory>

#include "dll.h"
#include "UnrealTypes.h"
#include <Polygon4/Types.h>

#ifndef UE_LOG
#define UE_LOG(...)
#endif

#define API_INFINITE_CALLS -1

namespace polygon4
{

struct API
{
#define API_FUNCTION(name, type) \
    std::shared_ptr<std::function<type>> name; \
    int n_##name = API_INFINITE_CALLS
#include "API_functions.h"
#undef API_FUNCTION

    API(){}
    ~API(){}
};

DLL_EXPORT
API &getAPI();

}

#ifndef POLYGON4_ENGINE

#define LOG_DEBUG(...)
#define LOG_TRACE(...)
#define LOG_ERROR(...)

#endif

#define __API_CALL(func, ...) \
    do { \
        try { \
            auto &api =  polygon4::getAPI(); \
            if (api.func && *api.func.get() && api.n_##func != 0) \
            { \
                api.func->operator()( __VA_ARGS__ ); \
                api.n_##func--; \
            } \
            else \
            { \
                LOG_ERROR(logger, "API call is not set: " ## #func); \
                UE_LOG(Polygon4, Error, L"API call is not set: %s", L#func); \
            } \
        } \
        catch (const std::exception &e) \
        { \
            LOG_ERROR(logger, "Error during API call '" ## #func ## "': " << e.what()); \
            UE_LOG(Polygon4, Error, TEXT("Error during API call '%s': %s"), L#func, e.what()); \
        } \
    } while (0)

#define API_CALL(func, ...) \
    do { \
        LOG_DEBUG(logger, "API call: " << #func); \
        UE_LOG(Polygon4, Log, TEXT("API call: %s"), L#func); \
        __API_CALL(func, __VA_ARGS__ ); \
    } while (0)

#define API_CALL_MSG(msg, func, ...) \
    do { \
        LOG_DEBUG(logger, "API call: " << #func << ": " << msg); \
        UE_LOG(Polygon4, Log, TEXT("API call: %s"), L#func); \
        __API_CALL(func, __VA_ARGS__ ); \
    } while (0)

#define REGISTER_API(api, function) \
    polygon4::getAPI().api = std::make_shared<decltype(polygon4::API::api)::element_type>(function); \
    polygon4::getAPI().n_##api = API_INFINITE_CALLS; \
    LOG_TRACE("api", "Register API: " << #api)

#define REGISTER_API_N_CALLS(api, n, function) \
    polygon4::getAPI().api = std::make_shared<decltype(polygon4::API::api)::element_type>(function); \
    polygon4::getAPI().n_##api = n; \
    LOG_TRACE("api", "Register API: " << #api)

#define UNREGISTER_API(api) \
    polygon4::getAPI().api = nullptr; \
    polygon4::getAPI().n_##api = API_INFINITE_CALLS; \
    LOG_TRACE("api", "Unregister API: " << #api)
