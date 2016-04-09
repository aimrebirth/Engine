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

#include <deque>
#include <functional>
#include <memory>
#include <set>

#include <Polygon4/DataManager/String.h>
#include <Polygon4/DataManager/Storage.h>

#define DECLARE_MENU_VIRTUAL(name) \
public: \
    virtual void Show ## name ## Menu() = 0; \
    virtual void Hide ## name ## Menu() = 0; \
    virtual void Set ## name ## MenuVisibility(bool visibility) = 0

namespace polygon4
{

using Function = std::function<void(void)>;
using SavedGames = std::deque<String>;

class BuildingMenu;
class Modification;
class Save;

// 32-bit workaround
#if defined(WIN32) && !defined(_WIN64)
#pragma pack(push, 1)
#endif

class DLL_EXPORT IEngine
{
public:
    IEngine() = default;
    virtual ~IEngine();

    virtual void initChildren() = 0;

    virtual Storage* getStorage() const = 0;

    virtual Modification* getCurrentModification() const = 0;
    virtual void setCurrentModification(Modification *currentModification) = 0;

    virtual void spawnCurrentPlayer() = 0;

    virtual BuildingMenu *getBuildingMenu() = 0;
    DECLARE_MENU_VIRTUAL(Main);
    DECLARE_MENU_VIRTUAL(Building);
    DECLARE_MENU_VIRTUAL(Pause);
    virtual void SetBuildingMenuCurrentBuilding(detail::ModificationMapBuilding *currentBuilding) = 0;

    virtual void OnLevelLoaded() = 0;
    Function LoadLevelObjects;

public:
    template <class T, class... Args>
    static std::shared_ptr<T> create(Args&&... args)
    {
        auto p = std::make_shared<T>(std::forward<Args>(args)...);
        p->initChildren();
        return p;
    }
};

class DLL_EXPORT Engine : public IEngine
{
protected:
    Engine(const String &gameDirectory);

public:
    bool reloadMods();
    bool reloadStorage();

    SavedGames getSavedGames(bool save = false) const;
    bool save(const String &fn) const;
    bool saveAuto() const;
    bool saveQuick() const;
    bool load(const String &fn);
    void deleteSaveGame(const String &fn) const;

    virtual Storage* getStorage() const override final { return storage.get(); }

    virtual Modification* getCurrentModification() const override final { return currentModification; }
    virtual void setCurrentModification(Modification *currentModification) override final
    {
        this->currentModification = currentModification;
    }

    virtual void spawnCurrentPlayer() override;

protected:
	std::shared_ptr<Storage> storage;

    Modification *currentModification = nullptr;

    bool _save(const String &fn) const;
};

// 32-bit workaround
#if defined(WIN32) && !defined(_WIN64)
#pragma pack(pop)
#endif

DLL_EXPORT
Engine *getEngine(Engine *engine = nullptr);

} // namespace polygon4
