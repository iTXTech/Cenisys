/*
 * ServerManager
 * Copyright (C) 2016 iTX Technologies
 *
 * This file is part of Cenisys.
 *
 * Cenisys is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * Cenisys is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with Cenisys.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "server/servermanager.h"
#include <fstream>
#include <boost/assert.hpp>

namespace Cenisys
{

ServerManager::ServerManager(const std::string &configPath)
    : sched_data_(service_)
{
    BOOST_ASSERT(!instance_);
    instance_ = this;
    std::ifstream conf(configPath);
    toml::ParseResult result = toml::parse(conf);
    ConfigParser<MasterConfig>()(result.value, config_);
}

ServerManager::~ServerManager()
{
    BOOST_ASSERT(instance_);
    instance_ = nullptr;
}

int ServerManager::exec()
{
    return 0;
}

boost::asio::io_service &ServerManager::getIoService()
{
    return service_;
}

ServerManager *ServerManager::getInstance()
{
    return instance_;
}

ServerManager *ServerManager::instance_{};

} // namespace Cenisys
