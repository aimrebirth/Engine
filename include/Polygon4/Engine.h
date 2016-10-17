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
#include <mutex>
#include <set>

#include <Polygon4/DataManager/String.h>
#include <Polygon4/DataManager/Storage.h>

#include <Polygon4/DataManager/Settings.h>

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
    IEngine(const IEngine &) = delete;
    IEngine &operator=(const IEngine &) = delete;
    virtual ~IEngine();

    virtual void initChildren() = 0;

    virtual Storage* getStorage() const = 0;

    virtual Modification* getCurrentModification() const = 0;
    virtual void setCurrentModification(Modification *currentModification) = 0;

    virtual void spawnCurrentPlayer() = 0;

    virtual BuildingMenu *getBuildingMenu() = 0;
    virtual void DestroyBuildingMenu() = 0;

    DECLARE_MENU_VIRTUAL(Main);
    DECLARE_MENU_VIRTUAL(Building);
    DECLARE_MENU_VIRTUAL(Pause);

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
    virtual ~Engine();

public:
    bool reloadMods();
    bool reloadStorage();

    Settings &getSettings();
    const Settings &getSettings() const;

    SavedGames getSavedGames(bool save = false) const;
    bool save(const String &fn) const;
    bool saveAuto() const;
    bool saveQuick() const;
    bool load(const String &fn);
    void deleteSaveGame(const String &fn) const;

    virtual Storage* getStorage() const override final { return storage.get(); }

    virtual Modification* getCurrentModification() const override final { return currentModification; }
    virtual void setCurrentModification(Modification *currentModificationIn) override final
    {
        currentModification = currentModificationIn;
    }

    virtual void spawnCurrentPlayer() override;

protected:
	std::shared_ptr<Storage> storage;
    Modification *currentModification = nullptr;

    bool _save(const String &fn) const;

private:
    // temp settings
    Settings settings;

    mutable std::mutex m_save;

    void backupSettings();
    void restoreSettings();

    void postLoadStorage();

#define GET_KEY_MAP(mf, m) \
public: \
    const KeyMap<String> &get ## mf() const { return m; } \
    KeyMap<String> &get ## mf() { return m; } \
private: \
    KeyMap<String> m

    GET_KEY_MAP(Messages, messages);
    GET_KEY_MAP(Strings, strings);
    GET_KEY_MAP(Items, items);
    GET_KEY_MAP(Buildings, buildings);
    GET_KEY_MAP(Objects, objects);
};

// 32-bit workaround
#if defined(WIN32) && !defined(_WIN64)
#pragma pack(pop)
#endif

DLL_EXPORT
#ifdef _MSC_VER
__declspec(noinline)
#endif
Engine *getEngine(Engine *engine = nullptr);

// useful macros used across the engine
#define GET_STORAGE() ::polygon4::getEngine()->getStorage()
#define GET_SETTINGS() ::polygon4::getEngine()->getSettings()

#define GET_BUILDING_MENU() ::polygon4::getEngine()->getBuildingMenu()

#define GET_MESSAGE(m) ((::polygon4::detail::Message *)::polygon4::getEngine()->getMessages()[(m)])
#define GET_MESSAGE_TEXT(m) (GET_MESSAGE(m)->txt->string.str())
#define GET_MESSAGE_TEXT_LOC(m, l) (GET_MESSAGE(m)->txt->string.str(l))

#define BM_TEXT_ADD_ITEM(o, q) GET_BUILDING_MENU()->ItemAdded(o, q)
#define BM_TEXT_ADD_MONEY(m) GET_BUILDING_MENU()->MoneyAdded((int)(m))

} // namespace polygon4
