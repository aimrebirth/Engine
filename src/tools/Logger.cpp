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

#include "Logger.h"

#include <boost/format.hpp>
#include <boost/log/common.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>

DECLARE_STATIC_LOGGER(logger, "logger");

BOOST_LOG_ATTRIBUTE_KEYWORD(timestamp, "TimeStamp", boost::posix_time::ptime)
BOOST_LOG_ATTRIBUTE_KEYWORD(thread_id, "ThreadID", boost::log::attributes::current_thread_id::value_type)
BOOST_LOG_ATTRIBUTE_KEYWORD(severity, "Severity", boost::log::trivial::severity_level)

void logFormatter(boost::log::record_view const& rec, boost::log::formatting_ostream& strm)
{
    static boost::thread_specific_ptr<boost::posix_time::time_facet> tss(0);
    if (!tss.get())
    {
        tss.reset(new boost::posix_time::time_facet);
        tss->set_iso_extended_format();
    }
    strm.imbue(std::locale(strm.getloc(), tss.get()));

    std::string s;
    boost::log::formatting_ostream ss(s);
    ss << "[" << rec[severity] << "]";

    strm << "[" << rec[timestamp] << "] " <<
            boost::format("[%08x] %-9s %s")
            % rec[thread_id]
            % s
            % rec[boost::log::expressions::smessage];
}

void initLogger(std::string logLevel, std::string logFile)
{
    typedef boost::log::sinks::text_file_backend tfb;
    typedef boost::log::sinks::synchronous_sink<tfb> sfs;

    try
    {
        bool disable_log = logLevel == "";
        boost::algorithm::to_lower(logLevel);

        boost::log::trivial::severity_level level;
        std::stringstream(logLevel) >> level;

        if (!disable_log)
        {
            auto c_log = boost::log::add_console_log();
            c_log->set_formatter(&logFormatter);
            c_log->set_filter(boost::log::trivial::severity >= level);
        }

        auto add_logger = [&logFile](const auto &log_level)
        {
            boost::log::trivial::severity_level level;
            std::stringstream(log_level) >> level;

            if (logFile != "")
            {
                auto backend = boost::make_shared<tfb>
                (
                    boost::log::keywords::file_name = logFile + ".log." + log_level,
                    boost::log::keywords::rotation_size = 10 * 1024 * 1024,
                    //boost::log::keywords::open_mode = std::ios_base::app,
                    boost::log::keywords::auto_flush = true
                );
                auto sink = boost::make_shared<sfs>(backend);
                sink->set_formatter(&logFormatter);
                sink->set_filter(boost::log::trivial::severity >= level);
                boost::log::core::get()->add_sink(sink);
            }
        };

        add_logger(logLevel);
        if (logLevel != "trace")
            add_logger("trace");
        if (logLevel != "info")
            add_logger("info");

        boost::log::add_common_attributes();
    }
    catch (const std::exception &e)
    {
        LOG_ERROR(logger, "logger initialization failed with exception " << e.what() << ", will use default logger settings");
    }
}
