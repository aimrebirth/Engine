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
    object = (detail::IObjectBase *)o;
    text = object->getName();
    switch (object->getType())
    {
    case detail::EObjectType::Message:
    {
        auto m = ((detail::Message*)object);
        if (m->title)
            text = m->title->string;
        else
            text.clear();
    }
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
    SET_CHILD(journal, JournalThemes, INT_THEMES);

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

    for (auto i = 0; i < InfoTreeItem::HoldStoreMax; i++)
        hold_store.children.emplace_back(std::make_shared<InfoTreeItem>());
    SET_CHILD(hold_store, HoldStoreGoods, INT_HOLD_GOODS);

    /*for (auto i = 0; i < InfoTreeItem::HoldMax; i++)
        hold.children.emplace_back(std::make_shared<InfoTreeItem>());
    SET_CHILD(hold, HoldItems, INT_HOLD_ITEMS);
    SET_CHILD(hold, HoldGoods, INT_HOLD_GOODS);*/

    for (auto i = 0; i < InfoTreeItem::StoreMax; i++)
        goods_store.children.emplace_back(std::make_shared<InfoTreeItem>());
    SET_CHILD(goods_store, StoreHas, INT_BASE_SELL);
    SET_CHILD(goods_store, StoreWants, INT_BASE_BUY);
}

BuildingMenu::~BuildingMenu()
{
}

void BuildingMenu::setCurrentBuilding(detail::ModificationMapBuilding *b)
{
    if (!b)
        return;
    building = b;
    clearThemes();
}

void BuildingMenu::setCurrentMechanoid(detail::Mechanoid *m)
{
    if (!m)
        return;
    mechanoid = m;
}

void BuildingMenu::setCurrentScriptCallback(ScriptCallback sc)
{
    scriptCallback = sc;
}

void BuildingMenu::update()
{
    updateThemes();
    updateJournal();
    updateGlider();
    updateGliderStore();
    updateHoldStore();
    updateGoodsStore();
}

void BuildingMenu::updateThemes()
{
    // highlight categories
    for (auto i = 0; i < InfoTreeItem::ThemesMax; i++)
        themes.children[i]->highlight = true;
}

void BuildingMenu::updateJournal()
{
    auto p = mechanoid->getPlayer();
    if (!p)
        return;

    std::sort(p->records.begin(), p->records.end(),
        [](const auto &r1, const auto &r2)
    {
        if (r1->type == detail::QuestRecordType::InProgress &&
            r2->type == detail::QuestRecordType::Completed)
            return true;
        if (r2->type == detail::QuestRecordType::InProgress &&
            r1->type == detail::QuestRecordType::Completed)
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

    journal.children[InfoTreeItem::JournalThemes]->children.clear();
    journal.children[InfoTreeItem::JournalThemes]->hidden_if_empty = true;

    // highlight categories
    for (auto i = 0; i < InfoTreeItem::JournalMax; i++)
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
        if (w->weapon->hidden_in_menu)
            continue;
        glider.children[InfoTreeItem::GliderWeapons]->children.push_back(std::make_shared<InfoTreeItem>(w));
    }

    glider.children[InfoTreeItem::GliderArmor]->children.clear();
    glider.children[InfoTreeItem::GliderEquipment]->children.clear();
    for (auto &e : c->equipments)
    {
        switch (e->equipment->type)
        {
        case detail::EquipmentType::Armor:
        case detail::EquipmentType::EnergyShield:
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
    for (auto i = (int)InfoTreeItem::GliderId; i < InfoTreeItem::GliderMax; i++)
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
    for (auto i = 0; i < InfoTreeItem::GliderStoreMax; i++)
        glider_store.children[i]->highlight = true;
}

void BuildingMenu::updateHoldStore()
{
    auto c = mechanoid->configuration;

    hold_store.children[InfoTreeItem::HoldStoreGoods]->children.clear();
    for (auto &g : c->goods)
    {
        hold_store.children[InfoTreeItem::HoldStoreGoods]->children.push_back(std::make_shared<InfoTreeItem>(g));
    }

    // highlight categories
    for (auto i = 0; i < InfoTreeItem::HoldStoreMax; i++)
        hold_store.children[i]->highlight = true;
}

void BuildingMenu::updateGoodsStore()
{
    goods_store.children[InfoTreeItem::StoreHas]->children.clear();
    goods_store.children[InfoTreeItem::StoreWants]->children.clear();
    for (auto &g : building->goods)
    {
        goods_store.children[g->sell ? InfoTreeItem::StoreHas : InfoTreeItem::StoreWants]
            ->children.push_back(std::make_shared<InfoTreeItem>(g));
    }

    // sort by name
    auto sort = [](auto &v)
    {
        std::sort(v->children.begin(), v->children.end(), [](const auto &c1, const auto &c2)
        {
            return c1->object->getName() < c2->object->getName();
        });
    };
    sort(goods_store.children[InfoTreeItem::StoreHas]);
    sort(goods_store.children[InfoTreeItem::StoreWants]);

    // highlight categories
    for (auto i = 0; i < InfoTreeItem::StoreMax; i++)
        goods_store.children[i]->highlight = true;
}

void BuildingMenu::refreshText()
{
    themes.children[InfoTreeItem::ThemesId]->children.clear();
    auto old = showedObjects;
    clearText();
    for (auto &o : old)
        addTheme(o);
}

void BuildingMenu::removeMessage(const detail::IObjectBase *obj)
{
    if (!obj)
        return;
    showedObjects.erase(
        std::remove(showedObjects.begin(), showedObjects.end(), obj),
        showedObjects.end());
    refreshText();
}

bool BuildingMenu::checkAndAddThemeObject(const detail::IObjectBase *o)
{
    auto c = themes.children[InfoTreeItem::ThemesId]->findChild(o);
    if (c)
        return false;
    themes.children[InfoTreeItem::ThemesId]->children.push_back(std::make_shared<InfoTreeItem>(o));
    return true;
}

void BuildingMenu::addTheme(const detail::Message *m)
{
    if (!m)
        return;
    if (checkAndAddThemeObject(m))
        addMessage(m);
}

void BuildingMenu::addTheme(const detail::IObjectBase *o)
{
    if (!o)
        return;
    if (o->getType() == detail::EObjectType::Message)
        return addTheme((detail::Message*)o);

    if (checkAndAddThemeObject(o))
    {
        addText(o->getName(), o->getDescription());
        showedObjects.push_back(o);
    }
}

void BuildingMenu::addTheme(const String &obj)
{
    auto o = getEngine()->getObjects()[obj];
    if (o)
        addTheme(o);
    else
        addText(obj + u" reference was not found");
}

void BuildingMenu::addThemeBuilding(const String &obj)
{
    auto o = getEngine()->getBuildings()[obj];
    if (o)
        addTheme(o);
    else
        addText(obj + u" reference was not found");
}

void BuildingMenu::addThemeItem(const String &obj)
{
    auto o = getEngine()->getItems()[obj];
    if (o)
        addTheme(o);
    else
        addText(obj + u" reference was not found");
}

void BuildingMenu::addThemeMessage(const String &obj)
{
    auto o = GET_MESSAGE(obj);
    if (o)
        addTheme(o);
    else
        addText(obj + u" reference was not found");
}

void BuildingMenu::addMessage(const detail::Message *m)
{
    printMessage(m);
    showedObjects.push_back(m);
}

void BuildingMenu::showMessage(const detail::Message *m)
{
    clearText();
    printMessage(m);
}

void BuildingMenu::addQuestMessage(const detail::Message *m)
{
    if (!m)
        return;
    addMessage(m);
    currentQuest = m->getTextId();
}

void BuildingMenu::printMessage(const detail::Message *m)
{
    if (m->type != detail::MessageType::Text && m->title)
        printTitle(m->title->string);
    printText(m->txt->string);
}

void BuildingMenu::addText(const String &t)
{
    printText(t);
}

void BuildingMenu::addText(const String &ti, const String &t)
{
    printTitle(ti);
    printText(t);
}

void BuildingMenu::showText(const String &t)
{
    clearText();
    printText(t);
}

void BuildingMenu::showText(const String &ti, const String &t)
{
    clearText();
    printTitle(ti);
    printText(t);
}

void BuildingMenu::printTitle(const String &t)
{
    if (t.empty())
        return;
    auto title = String(BIG_SPACE) + u"<span color=\"yellow\">" + t + u"</>";
    printText(title);
}

void BuildingMenu::clearText()
{
    text.clear();
    showedObjects.clear();
}

void BuildingMenu::clearThemes()
{
    themes.children[InfoTreeItem::ThemesId]->children.clear();
}

void BuildingMenu::printText(String t)
{
    if (t.empty())
        return;

    // format here!

    boost::algorithm::trim_right(t);
    boost::algorithm::replace_all(t, L"<icon:POINT>", BIG_SPACE);
    boost::algorithm::replace_all(t, L"<p>", SMALL_DELIMETER BIG_SPACE);
    if (mechanoid->name)
        boost::algorithm::replace_all(t, L"%NAME", mechanoid->name->string.str());
    else
        boost::algorithm::replace_all(t, L"%NAME", L"Unnamed");
    boost::algorithm::replace_all(t, L"%BUILDINGNAME", building->getName());
    boost::algorithm::replace_all(t, L"%BUILDINGREF", building->building->building->getTextId());
    boost::algorithm::replace_all(t, L"%RATINGNAME", mechanoid->getRatingLevelName());
    if (mechanoid->clan && mechanoid->clan->member_name)
        boost::algorithm::replace_all(t, L"%ORGMEMBER", mechanoid->clan->member_name->string.str());
    boost::algorithm::replace_all(t, L"%QUEST", currentQuest);

    text += t;
    text += String(SMALL_DELIMETER);
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

void BuildingMenu::saveScreenText()
{
    if (!mainScreenText.empty())
        return;
    mainScreenText = text;
}

void BuildingMenu::loadScreenText()
{
    text = mainScreenText;
    mainScreenText.clear();
}

} // namespace polygon4
