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

#include <string>

#include <Polygon4/DataManager/dll.h>
#include <Polygon4/DataManager/String.h>

namespace polygon4
{
    
DLL_EXPORT
String read_orig_module_filename();

DLL_EXPORT
String read_old_module_filename();

DLL_EXPORT
String read_new_module_filename();

DLL_EXPORT
String read_orig_module_filename_store();

DLL_EXPORT
String read_old_module_filename_store();

DLL_EXPORT
String read_new_module_filename_store();

DLL_EXPORT
String read_ver_module_filename_store();

DLL_EXPORT
void write_module_last_write_time(String game_dir, String module_name);

DLL_EXPORT
String prepare_module_for_hotload(String game_dir, String module_name);

}