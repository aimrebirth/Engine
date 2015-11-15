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

using Function = std::function<void()>;

class Modification;
class Save;

class DLL_EXPORT IEngine
{
public:
    virtual ~IEngine();

    virtual void initChildren() = 0;

    template <class T, class... Args>
    static std::shared_ptr<T> create(Args&&... args)
    {
        auto p = std::make_shared<T>(std::forward<Args>(args)...);
        p->initChildren();
        return p;
    }

    virtual bool reloadStorage() = 0;
    virtual bool reloadMods() = 0;

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
    virtual ~Engine();

    virtual bool reloadStorage() override;
    virtual bool reloadMods() override;

protected:
	std::shared_ptr<Storage> storage;
};

extern IEngine *gEngine;

} // namespace polygon4
