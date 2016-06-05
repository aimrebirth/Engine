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

#include <Polygon4/BuildingMenu.h>

#include <algorithm>

#include <boost/algorithm/string.hpp>

#include <Polygon4/Engine.h>

#define SMALL_DELIMETER "\n"
#define BIG_DELIMETER SMALL_DELIMETER SMALL_DELIMETER

#define SMALL_SPACE "  "
#define BIG_SPACE SMALL_SPACE SMALL_SPACE

namespace polygon4
{

InfoTreeItem::InfoTreeItem(const detail::IObjectBase *o)
{
    assign(o);
}

InfoTreeItem *InfoTreeItem::findChild(const detail::IObjectBase *o)
{
    auto i = std::find_if(children.begin(), children.end(),
        [o](const auto &e) {return e->object == o; });
    if (i != children.end())
        return i->get();
    return nullptr;
}

InfoTreeItem &InfoTreeItem::operator=(const detail::IObjectBase *o)
{
    assign(o);
    return *this;
}

void InfoTreeItem::assign(const detail::IObjectBase *o)
{
    if (!o)
        return;
    text = o->getName();
    object = (detail::IObjectBase *)o;
    switch (o->getType())
    {
    case detail::EObjectType::Message:
        text = ((detail::Message*)o)->title->string;
        break;
    }
}

BuildingMenu::BuildingMenu()
{
    auto &messages = getEngine()->getMessages();

#define SET_CHILD(v, e, m) *(v.children[InfoTreeItem::e]) = messages[#m]

    for (auto i = 0; i < InfoTreeItem::ThemesMax; i++)
        themes.children.emplace_back(std::make_shared<InfoTreeItem>());
    SET_CHILD(themes, ThemesId, INT_THEMES);

    for (auto i = 0; i < InfoTreeItem::JournalMax; i++)
        journal.children.emplace_back(std::make_shared<InfoTreeItem>());
    SET_CHILD(journal, JournalInProgress, INT_QUESTS_ACTIVE);
    SET_CHILD(journal, JournalCompleted, INT_QUESTS_COMPLETED);
    SET_CHILD(journal, JournalFailed, INT_QUESTS_FAILED);
    SET_CHILD(journal, JournalId, INT_JOURNAL);

    for (auto i = 0; i < InfoTreeItem::GliderMax; i++)
        glider.children.emplace_back(std::make_shared<InfoTreeItem>());
    SET_CHILD(glider, GliderGeneral, INT_PMENU_GLIDER_INFO);
    SET_CHILD(glider, GliderId, INT_PMENU_GLIDER_GLIDER);
    SET_CHILD(glider, GliderArmor, INT_PMENU_GLIDER_ARMOR);
    SET_CHILD(glider, GliderWeapons, INT_PMENU_GLIDER_WEAPONS);
    SET_CHILD(glider, GliderEquipment, INT_PMENU_GLIDER_EQUIPMENT);
    SET_CHILD(glider, GliderAmmo, INT_PMENU_GLIDER_AMMO);

    for (auto i = 0; i < InfoTreeItem::GliderStoreMax; i++)
        glider_store.children.emplace_back(std::make_shared<InfoTreeItem>());
    SET_CHILD(glider_store, GliderStoreId, INT_BASE_GLIDERS);
    SET_CHILD(glider_store, GliderStoreEquipment, INT_BASE_EQUIPMENT);
    SET_CHILD(glider_store, GliderStoreWeapons, INT_BASE_WEAPONS);
    SET_CHILD(glider_store, GliderStoreAmmo, INT_BASE_AMMO);
}

BuildingMenu::~BuildingMenu()
{
}

void BuildingMenu::SetCurrentBuilding(detail::ModificationMapBuilding *b)
{
    if (!b)
        return;
    building = b;
    themes.children[InfoTreeItem::ThemesId]->children.clear();
}

void BuildingMenu::update()
{
    updateJournal();
    updateGlider();
    updateGliderStore();
}

void BuildingMenu::updateJournal()
{
    auto p = mechanoid->getPlayer();
    if (!p)
        return;

    std::sort(p->records.begin(), p->records.end(),
        [](const auto &r1, const auto &r2)
    {
        if (r1->type == detail::JournalRecordType::InProgress &&
            r2->type == detail::JournalRecordType::Completed)
            return true;
        if (r2->type == detail::JournalRecordType::InProgress &&
            r1->type == detail::JournalRecordType::Completed)
            return false;
        return r1->time > r2->time;
    });

    journal.children[InfoTreeItem::JournalId]->children.clear();
    for (auto &r : p->records)
    {
        auto c = std::make_shared<InfoTreeItem>();
        c->object = r;
        journal.children[InfoTreeItem::JournalId]->children.push_back(c);
    }


    // highlight categories
    for (auto i = (int)InfoTreeItem::JournalInProgress; i < (int)InfoTreeItem::JournalMax; i++)
        journal.children[i]->highlight = true;
}

void BuildingMenu::updateGlider()
{
    auto c = mechanoid->configuration;

    glider.children[InfoTreeItem::GliderId]->children.clear();
    glider.children[InfoTreeItem::GliderId]->children.push_back(std::make_shared<InfoTreeItem>(c->glider));

    glider.children[InfoTreeItem::GliderWeapons]->children.clear();
    for (auto &w : c->weapons)
    {
        glider.children[InfoTreeItem::GliderWeapons]->children.push_back(std::make_shared<InfoTreeItem>(w));
    }

    glider.children[InfoTreeItem::GliderArmor]->children.clear();
    glider.children[InfoTreeItem::GliderEquipment]->children.clear();
    for (auto &e : c->equipments)
    {
        switch (e->equipment->type)
        {
        case detail::EquipmentType::Armor:
        case detail::EquipmentType::Generator:
            glider.children[InfoTreeItem::GliderArmor]->children.push_back(std::make_shared<InfoTreeItem>(e));
            break;
        case detail::EquipmentType::Reactor:
        case detail::EquipmentType::Engine:
            glider.children[InfoTreeItem::GliderId]->children.push_back(std::make_shared<InfoTreeItem>(e));
            break;
        default:
            glider.children[InfoTreeItem::GliderEquipment]->children.push_back(std::make_shared<InfoTreeItem>(e));
            break;
        }
    }

    glider.children[InfoTreeItem::GliderAmmo]->children.clear();
    for (auto &p : c->projectiles)
    {
        glider.children[InfoTreeItem::GliderAmmo]->children.push_back(std::make_shared<InfoTreeItem>(p));
    }

    // maybe sort
    // armor: armor, generator
    // glider: glider, reactor, engine,
    // weapons: light, heavy
    // equ: ?
    // ammo: ?

    // highlight categories
    for (auto i = (int)InfoTreeItem::GliderId; i < (int)InfoTreeItem::GliderMax; i++)
        glider.children[i]->highlight = true;
}

void BuildingMenu::updateGliderStore()
{
    glider_store.children[InfoTreeItem::GliderStoreId]->children.clear();
    for (auto &g : building->gliders)
    {
        glider_store.children[InfoTreeItem::GliderStoreId]->children.push_back(std::make_shared<InfoTreeItem>(g));
    }

    glider_store.children[InfoTreeItem::GliderStoreWeapons]->children.clear();
    for (auto &w : building->weapons)
    {
        glider_store.children[InfoTreeItem::GliderStoreWeapons]->children.push_back(std::make_shared<InfoTreeItem>(w));
    }

    glider_store.children[InfoTreeItem::GliderStoreEquipment]->children.clear();
    for (auto &e : building->equipments)
    {
        glider_store.children[InfoTreeItem::GliderStoreEquipment]->children.push_back(std::make_shared<InfoTreeItem>(e));
    }

    glider_store.children[InfoTreeItem::GliderStoreAmmo]->children.clear();
    for (auto &p : building->projectiles)
    {
        glider_store.children[InfoTreeItem::GliderStoreAmmo]->children.push_back(std::make_shared<InfoTreeItem>(p));
    }

    // maybe sort
    // armor: armor, generator
    // glider: glider, reactor, engine,
    // weapons: light, heavy
    // equ: ?
    // ammo: ?

    // highlight categories
    for (auto i = (int)InfoTreeItem::GliderStoreId; i < (int)InfoTreeItem::GliderStoreMax; i++)
        glider_store.children[i]->highlight = true;
}

void BuildingMenu::addTheme(const detail::Message *m)
{
    auto c = themes.findChild(m);
    if (c)
        return;
    themes.children[InfoTreeItem::ThemesId]->children.push_back(std::make_shared<InfoTreeItem>(m));
    addMessage(m);
}

void BuildingMenu::addMessage(const detail::Message *m)
{
    if (!text.empty())
        text += SMALL_DELIMETER;
    printMessage(m);
}

void BuildingMenu::showMessage(const detail::Message *m)
{
    text.clear();
    printMessage(m);
}

void BuildingMenu::printMessage(const detail::Message *m)
{
    printTitle(m->title->string);
    printText(m->txt->string);
}

void BuildingMenu::addText(const String &t)
{
    if (!text.empty())
        text += SMALL_DELIMETER;
    printText(t);
}

void BuildingMenu::addText(const String &ti, const String &t)
{
    if (!text.empty())
        text += SMALL_DELIMETER;
    printTitle(ti);
    printText(t);
}

void BuildingMenu::showText(const String &t)
{
    text.clear();
    printText(t);
}

void BuildingMenu::showText(const String &ti, const String &t)
{
    text.clear();
    printTitle(ti);
    printText(t);
}

void BuildingMenu::printTitle(const String &t)
{
    if (t.empty())
        return;
    auto title = String(BIG_SPACE) + L"<span color=\"yellow\">" + t + L"</>";
    printText(title);
}

void BuildingMenu::clearText()
{
    text.clear();
}

void BuildingMenu::printText(String t)
{
    if (t.empty())
        return;

    // TODO: format here!
    // maybe boost format

    boost::algorithm::trim_right(t);
    boost::algorithm::replace_all(t, L"<p>", SMALL_DELIMETER BIG_SPACE);

    text += t;
    text += SMALL_DELIMETER;
}

void BuildingMenu::JournalRecordAdded()
{
    printMessage(GET_MESSAGE("INT_JOURNAL_UPDATED"));
}

void BuildingMenu::ItemAdded(detail::IObjectBase *item, int quantity)
{
    if (quantity == 0)
        return;
    std::string s = "INT_PLAYER_GOT_OBJECT";
    if (quantity != 1)
        s += "_MULTY";
    auto t = GET_MESSAGE_TEXT(s);
    auto name = item->getName();
    auto text_id = item->getTextId();
    boost::algorithm::ireplace_all(t, "%OBJECTID", text_id);
    boost::algorithm::ireplace_all(t, "%OBJECTNAME", name);
    if (quantity != 1)
        boost::algorithm::ireplace_all(t, "%OBJECTCOUNT", std::to_wstring(quantity));
    printText(t);
}

void BuildingMenu::MoneyAdded(int amount)
{
    auto t = GET_MESSAGE_TEXT("INT_PLAYER_ADD_BALANCE");
    boost::algorithm::ireplace_all(t, "%COUNT", std::to_wstring(amount));
    printText(t);
}

} // namespace polygon4
