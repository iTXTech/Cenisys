/*
 * Launcher for the server.
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

#include "config.h"

#include <iostream>
#include <locale>
#include <memory>
#include <string>
#include <thread>
#include <vector>
#include <boost/fiber/algo/asio.hpp>
#include <boost/fiber/asio/use_future.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/locale/format.hpp>
#include <boost/locale/generator.hpp>
#include <boost/locale/message.hpp>
#include <boost/program_options.hpp>
#include "server/servermanager.h"

int main(int argc, char *argv[])
{
    boost::locale::generator localeGen;
    localeGen.add_messages_path(PACKAGE_LOCALE_DIR);
    localeGen.set_default_messages_domain(GETTEXT_PACKAGE);
    boost::program_options::options_description desc;
    std::string configFile;
    desc.add_options()(
        "help,h",
        boost::locale::translate("display this help and exit").str().c_str())(
        "version,V",
        boost::locale::translate("output version information and exit")
            .str()
            .c_str())(
        "config,c",
        boost::program_options::value<std::string>(&configFile)
            ->value_name(boost::locale::translate("configuration file"))
            ->default_value(CONFIG_DIR "/cenisys.conf"),
        boost::locale::translate("set the configuration file").str().c_str());
    boost::program_options::variables_map vm;
    boost::program_options::store(
        boost::program_options::parse_command_line(argc, argv, desc), vm);
    boost::program_options::notify(vm);

    if(vm.count("help"))
    {
        std::cout << desc;
        return 0;
    }
    if(vm.count("version"))
    {
        std::locale oldCoutLoc = std::cout.imbue(localeGen(""));
        std::cout
            << boost::locale::format(
                   boost::locale::translate("Cenisys {1}\n")) %
                   SERVER_VERSION
            << boost::locale::translate(
                   "Copyright (C) 2016 iTX Technologies.\n"
                   "License AGPLv3+: GNU AGPL version 3 or later "
                   "<https://gnu.org/licenses/agpl.html>.\n"
                   "This is free software: you are free to change and "
                   "redistribute it.\n"
                   "There is NO WARRANTY, to the extent permitted by law.\n");
        std::cout.imbue(oldCoutLoc);
        return 0;
    }

    Cenisys::ServerManager manager(configFile);
    return manager.exec();
}
