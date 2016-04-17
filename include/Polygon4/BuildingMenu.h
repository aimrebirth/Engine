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

        HoldId = 0,
        HoldMax,
    };

    InfoTreeItem *parent = nullptr;
    std::vector<std::shared_ptr<InfoTreeItem>> children;

    String text;
    detail::IObjectBase *object = nullptr;
    bool expanded = true;

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

    void SetCurrentBuilding(detail::ModificationMapBuilding *b);

    void SetCurrentMechanoid(detail::Mechanoid *m)
    {
        if (!m)
            return;
        mechanoid = m;
    }

    virtual void refresh() = 0;

    void addMessage(const detail::Message *msg);
    void showMessage(const detail::Message *msg);

    void addTheme(const detail::Message *msg);

    void update();
    void updateGlider();
    void updateGliderStore();
    void updateJournal();

protected:
    detail::ModificationMapBuilding *building = nullptr;
    detail::Mechanoid *mechanoid;

    InfoTreeItem themes;
    InfoTreeItem journal;
    InfoTreeItem glider;
    InfoTreeItem hold;
    InfoTreeItem map;
    InfoTreeItem glider_store;
    InfoTreeItem goods_store;
    InfoTreeItem clans;

private:
    String text;

    void printMessage(const detail::Message *msg);
};

} // namespace polygon4
