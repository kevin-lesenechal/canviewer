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

#pragma once

#include "ui/window.hpp"
#include "ui/frames_model.hpp"
#include "frame.hpp"
#include "socketcan_source.hpp"

#include <set>

class QApplication;

class Application : public QObject
{
    Q_OBJECT
public:
    Application(QApplication& qapp, const std::string& devname);

    int run();

private slots:
    void frame_received(Frame frame);

private:
    QApplication&       _qapp;
    std::vector<Frame>  _frames;
    std::set<uint32_t>  _frame_ids;
    FramesModel         _frames_model;
    Window              _window;
    SocketcanSource     _can_source;
};
