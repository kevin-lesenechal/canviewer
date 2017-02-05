/*************************************************************************
 * Copyright © 2018 Kévin Lesénéchal <kevin.lesenechal@gmail.com>        *
 *                                                                       *
 * canviewer is free software: you can redistribute it and/or modify it  *
 * under the terms of the GNU General Public License as published by the *
 * Free Software Foundation, either version 3 of the License, or (at     *
 * your option) any later version.                                       *
 *                                                                       *
 * canviewer is distributed in the hope that it will be useful, but      *
 * WITHOUT ANY WARRANTY; without even the implied warranty of            *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU      *
 * General Public License for more details.                              *
 *                                                                       *
 * You should have received a copy of the GNU General Public License     *
 * along with canviewer. If not, see <http://www.gnu.org/licenses/>.     *
 *************************************************************************/

#include "application.hpp"

#include <iostream>

#include <QApplication>
#include <QMessageBox>

int main(int argc, char** argv)
{
    QApplication qapp(argc, argv);

    std::string devname = argc > 1 ? argv[1] : "can0";

    try {
        Application app(qapp, devname);
        return app.run();
    } catch (const std::exception& e) {
        QString msg = "Oops! An internal error occurred...\n\n";
        msg += e.what();
        std::cerr << msg.toStdString() << std::endl;
        QMessageBox::critical(nullptr, "canviewer - Unhandled exception", msg);
    }

    return 2;
}
