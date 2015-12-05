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
#include <set>

#include <Polygon4/DataManager/String.h>
#include <Polygon4/DataManager/Storage.h>

namespace polygon4
{

using Function = std::function<void(void)>;

class Modification;
class Save;

#if defined(WIN32) && !defined(_WIN64)
#pragma pack(push, 1)
#endif

class DLL_EXPORT IEngine
{
public:
    IEngine() = default;
    virtual ~IEngine();

    virtual void initChildren() = 0;

    template <class T, class... Args>
    static std::shared_ptr<T> create(Args&&... args)
    {
        auto p = std::make_shared<T>(std::forward<Args>(args)...);
        p->initChildren();
        return p;
    }

    virtual void ShowMainMenu() = 0;
    virtual void HideMainMenu() = 0;

    virtual void OnLevelLoaded() = 0;
    Function LoadLevelObjects;
};

class DLL_EXPORT Engine : public IEngine
{
protected:
    Engine(const String &modificationsDirectory);

public:
    bool reloadStorage();
    bool reloadMods();

protected:
	std::shared_ptr<Storage> storage;
};

#if defined(WIN32) && !defined(_WIN64)
#pragma pack(pop)
#endif

extern IEngine *gEngine;

} // namespace polygon4
