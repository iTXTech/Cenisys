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
#ifndef CENISYS_SERVERMANAGER_H
#define CENISYS_SERVERMANAGER_H

#include <list>
#include <boost/asio/io_service.hpp>

namespace Cenisys
{

class ServerManager
{
public:
    ServerManager();

    static ServerManager instance;

    boost::asio::io_service &getIoService();

private:
    boost::asio::io_service service_;
};

} // namespace Cenisys

#endif // CENISYS_SERVERMANAGER_H
