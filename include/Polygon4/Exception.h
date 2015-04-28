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

#include <exception>
#include <sstream>
#include <string>

#define EXCEPTION_PARAMS __FILE__, __FUNCTION__, __LINE__
#define EXCEPTION(msg) polygon4::Exception(EXCEPTION_PARAMS, msg)

namespace polygon4
{

class Exception : public std::exception
{
public:
    Exception(){}
    Exception(const char *file, const char *function, int line, const char *msg)
    {
        std::stringstream ss;
        ss << "Exception in file: " << file << " function: " << function << " line: " << line << ". "
           << "Error description: " << msg << ".";

        _what_str = ss.str();
    }
    virtual ~Exception() throw () {} 

    virtual const char *what() const throw ()
    {
        return _what_str.c_str();
    }
private:
    std::string _what_str;
};

} //namepsace gqp
