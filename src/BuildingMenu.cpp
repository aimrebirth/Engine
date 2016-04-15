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

namespace polygon4
{

InfoTreeItem *InfoTreeItem::findChild(detail::IObjectBase *o)
{
    auto i = std::find_if(children.begin(), children.end(),
        [o](const auto &e) {return e->object == o; });
    if (i != children.end())
        return i->get();
    return nullptr;
}

BuildingMenu::BuildingMenu()
{
    for (auto i = 0; i < InfoTreeItem::ThemesMax; i++)
        themes.children.emplace_back(std::make_shared<InfoTreeItem>());
    themes.children[InfoTreeItem::ThemesId]->text = tr("Themes");

    for (auto i = 0; i < InfoTreeItem::JournalMax; i++)
        journal.children.emplace_back(std::make_shared<InfoTreeItem>());
    journal.children[InfoTreeItem::JournalInProgress]->text = tr("Active quests");
    journal.children[InfoTreeItem::JournalCompleted]->text = tr("Completed quests");
    journal.children[InfoTreeItem::JournalFailed]->text = tr("Failed quests");
    journal.children[InfoTreeItem::JournalId]->text = tr("Journal");

    for (auto i = 0; i < InfoTreeItem::GliderMax; i++)
        glider.children.emplace_back(std::make_shared<InfoTreeItem>());
    glider.children[InfoTreeItem::GliderGeneral]->text = tr("General information");
    glider.children[InfoTreeItem::GliderId]->text = tr("Glider");
    glider.children[InfoTreeItem::GliderArmor]->text = tr("Armor");
    glider.children[InfoTreeItem::GliderWeapons]->text = tr("Weapons");
    glider.children[InfoTreeItem::GliderEquipment]->text = tr("Equipment");
    glider.children[InfoTreeItem::GliderAmmo]->text = tr("General information");
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

void BuildingMenu::updateJournal()
{

}

void BuildingMenu::addTheme(detail::Message *m)
{
    auto c = themes.findChild(m);
    if (c)
        return;

    if (!text.empty())
        text += "\n\n";
    text += m->title->string;
    text += "\n";
    text += m->txt->string;
    text += "\n";

    auto t = std::make_shared<InfoTreeItem>();
    t->object = m;
    t->text = m->title->string;
    themes.children[InfoTreeItem::ThemesId]->children.push_back(t);
}

} // namespace polygon4
