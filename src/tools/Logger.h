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

#if USE_LOGGER

#include <boost/log/trivial.hpp>

#define LOGGER_GLOBAL_INIT
#define LOGGER_GLOBAL_DESTROY
#define LOGGER_CONFIGURE(loglevel, filename) initLogger(loglevel, filename)

#define CREATE_LOGGER(name) const char *name
#define GET_LOGGER(module) module
#define INIT_LOGGER(name, module) name = GET_LOGGER(module)
#define DECLARE_LOGGER(name, module) CREATE_LOGGER(name)(GET_LOGGER(module))
#define DECLARE_STATIC_LOGGER(name, module) static DECLARE_LOGGER(name, module)

//#define LOG(logger, level, message)
#define LOG_BOOST_LOG_MESSAGE(logger, message) "[" << logger << "] " << message
#define LOG_TRACE(logger, message) BOOST_LOG_TRIVIAL(trace) << LOG_BOOST_LOG_MESSAGE(logger, message)
#define LOG_DEBUG(logger, message) BOOST_LOG_TRIVIAL(debug) << LOG_BOOST_LOG_MESSAGE(logger, message)
#define LOG_INFO(logger, message) BOOST_LOG_TRIVIAL(info) << LOG_BOOST_LOG_MESSAGE(logger, message)
#define LOG_WARN(logger, message) BOOST_LOG_TRIVIAL(warning) << LOG_BOOST_LOG_MESSAGE(logger, message)
#define LOG_ERROR(logger, message) BOOST_LOG_TRIVIAL(error) << LOG_BOOST_LOG_MESSAGE(logger, message)
#define LOG_FATAL(logger, message) BOOST_LOG_TRIVIAL(fatal) << LOG_BOOST_LOG_MESSAGE(logger, message)

void initLogger(std::string logLevel = "DEBUG", std::string logFile = "");

#else // !USE_LOGGER

#define LOGGER_GLOBAL_INIT
#define LOGGER_GLOBAL_DESTROY
#define LOGGER_CONFIGURE(loglevel, filename)

#define CREATE_LOGGER(name)
#define GET_LOGGER(module)
#define INIT_LOGGER(name, module)
#define DECLARE_LOGGER(name, module)
#define DECLARE_STATIC_LOGGER(name, module)

//#define LOG(logger, level, message)
#define LOG_TRACE(logger, message)
#define LOG_DEBUG(logger, message)
#define LOG_INFO(logger, message)
#define LOG_WARN(logger, message)
#define LOG_ERROR(logger, message)
#define LOG_FATAL(logger, message)

#define IS_LOG_TRACE_ENABLED(logger)
#define IS_LOG_DEBUG_ENABLED(logger)
#define IS_LOG_INFO_ENABLED(logger)
#define IS_LOG_WARN_ENABLED(logger)
#define IS_LOG_ERROR_ENABLED(logger)
#define IS_LOG_FATAL_ENABLED(logger)

#endif