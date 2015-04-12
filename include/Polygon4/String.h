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
#include <string>

#include "dll.h"
#include "Common.h"

namespace polygon4
{

class String
{
public:
    String()
    {
    }
    String(const char *s)
    {
        copy(s, strlen(s) + 1);
    }
    String(const wchar_t *s)
    {
        copy(s, wcslen(s) + 1, 2);
    }
    String(const std::string &s)
    {
        copy(s.c_str(), s.size() + 1);
    }
    String(const std::wstring &s)
    {
        copy(s.c_str(), s.size() + 1, 2);
    }
    String &String::operator=(const std::string &s)
    {
        copy(s.c_str(), s.size() + 1);
        return *this;
    }
    String &String::operator=(const std::wstring &s)
    {
        copy(s.c_str(), s.size() + 1, 2);
        return *this;
    }
    ~String()
    {

    }
    
    std::string string() const
    {
        return (const char *)data.get();
    }
    std::wstring wstring() const
    {
        return (const wchar_t *)data.get();
    }
    operator std::string() const
    {
        return string();
    }
    operator std::wstring() const
    {
        return wstring();
    }

    bool empty() const
    {
        return length == 0;
    }
private:
    std::shared_ptr<char> data;
    size_t length = 0;
    
    void copy(const void *data, size_t length, size_t multiplier = 1)
    {
        length *= multiplier;
        auto s = new char[length];
        memcpy(s, data, length);
        this->data = std::shared_ptr<char>(s, [](char *s){ delete[] s; });
        this->length = length;
    }
};

}
