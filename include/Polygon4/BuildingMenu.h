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

#include <memory>
#include <vector>

#include <Polygon4/DataManager/String.h>
#include <Polygon4/DataManager/Types.h>

namespace polygon4
{

struct DLL_EXPORT InfoTreeItem
{
    enum
    {
        ThemesId = 0,
        ThemesMax,

        JournalInProgress = 0,
        JournalCompleted,
        JournalFailed,
        JournalId,
        JournalMax,

        GliderGeneral = 0,
        GliderId,
        GliderArmor,
        GliderWeapons,
        GliderEquipment,
        GliderAmmo,
        GliderMax,

        GliderStoreId = 0,
        GliderStoreEquipment,
        GliderStoreWeapons,
        GliderStoreAmmo,
        GliderStoreMax,

        HoldItems = 0,
        HoldGoods,
        HoldMax,

        HoldStoreGoods = 0,
        HoldStoreMax,

        StoreHas = 0,
        StoreWants,
        StoreMax,
    };

    InfoTreeItem *parent = nullptr;
    std::vector<std::shared_ptr<InfoTreeItem>> children;

    String text;
    detail::IObjectBase *object = nullptr;
    bool expanded = true;
    bool highlight = false;

    InfoTreeItem(const detail::IObjectBase *o = nullptr);

    InfoTreeItem *findChild(const detail::IObjectBase *o);

    InfoTreeItem &operator=(const detail::IObjectBase *o);

private:
    void assign(const detail::IObjectBase *o);
};

class DLL_EXPORT BuildingMenu
{
public:
    BuildingMenu();
    virtual ~BuildingMenu();

    const String &getText() const { return text; }
    String &getText() { return text; }

    void setCurrentBuilding(detail::ModificationMapBuilding *b);
    void setCurrentMechanoid(detail::Mechanoid *m);
    void setCurrentScriptCallback(std::function<void(const std::string &)> sc);

    virtual void refresh() = 0;

    void addMessage(const detail::Message *msg);
    void showMessage(const detail::Message *msg);

    void addText(const String &text);
    void addText(const String &title, const String &text);
    void showText(const String &text);
    void showText(const String &title, const String &text);
    void clearText();
    void clearThemes();

    void addTheme(const detail::Message *msg);
    void addTheme(const detail::IObjectBase *o);
    void addThemeBuilding(const String &bld);
    void addThemeItem(const String &obj);
    void addThemeMessage(const String &obj);
    bool checkAndAddThemeObject(const detail::IObjectBase *o);

    void update();
    void updateThemes();
    void updateGlider();
    void updateGliderStore();
    void updateJournal();
    void updateHoldStore();
    void updateGoodsStore();

    // information
    void JournalRecordAdded();
    void ItemAdded(detail::IObjectBase *item, int quantity);
    void MoneyAdded(int amount);

    void saveScreenText();
    void loadScreenText();

protected:
    detail::ModificationMapBuilding *building = nullptr;
    detail::Mechanoid *mechanoid = nullptr;
    std::function<void(const std::string &)> scriptCallback;

    InfoTreeItem themes;
    InfoTreeItem journal;
    InfoTreeItem glider;
    InfoTreeItem hold_store;
    InfoTreeItem map;
    InfoTreeItem glider_store;
    InfoTreeItem goods_store;
    InfoTreeItem clans;

private:
    String text;
    String mainScreenText;

    void printMessage(const detail::Message *msg);
    void printText(String text);
    void printTitle(const String &t);
};

} // namespace polygon4
