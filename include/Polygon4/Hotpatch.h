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

#include <filesystem>
namespace fs = std::filesystem;
using path = fs::path;

#include <Polygon4/DataManager/String.h>

namespace polygon4
{

P4_ENGINE_API
path read_orig_module_filename();

P4_ENGINE_API
path read_old_module_filename();

P4_ENGINE_API
path read_new_module_filename();

P4_ENGINE_API
path read_orig_module_filename_store();

P4_ENGINE_API
path read_old_module_filename_store();

P4_ENGINE_API
path read_new_module_filename_store();

P4_ENGINE_API
path read_ver_module_filename_store();

P4_ENGINE_API
void write_module_last_write_time(path game_dir, path module_name);

P4_ENGINE_API
path prepare_module_for_hotload(path game_dir, path module_name);

}
