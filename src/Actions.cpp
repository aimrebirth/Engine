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

#include <Polygon4/Actions.h>

namespace polygon4
{

const detail::EnumTextTable<ActionResult> &table_ActionResult()
{
    static detail::EnumTextTable<ActionResult> table_ActionResult =
    {
        { ActionResult::Unspecified, "Unspecified" },
        { ActionResult::Ok, "Ok" },
        { ActionResult::Error, "Error" },
        { ActionResult::JournalRecordAdded, "JournalRecordAdded" },
        { ActionResult::ItemAdded, "ItemAdded" },
    };
    return table_ActionResult;
}

String tr(ActionResult e)
{
    auto i = table_ActionResult().find(e);
    if (i != table_ActionResult().end())
        return tr(i->second);
    return MISSING_VALUE;
}

String str(ActionResult e)
{
    auto i = table_ActionResult().find(e);
    if (i != table_ActionResult().end())
        return (i->second);
    return MISSING_VALUE;
}

} // namespace polygon4

