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
#include <stdint.h>

#define DECLARE_GET_SET(type, name) \
    public: \
        type get##name() const; \
    private: \
        void set##name(type var)

#define DEFINE_GET_SET(class, type, name, var) \
    type class::get##name() const { return var; } \
    void class::set##name(type var) { this->var = var; }

#define DISALLOW_COPY_CONSTRUCTORS(name) \
    private: \
        name(const name &) = delete; \
        name &operator=(const name &) = delete
