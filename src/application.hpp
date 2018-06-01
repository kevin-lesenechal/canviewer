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
#include "source_interface.hpp"

#include <set>
#include <memory>

class QApplication;

class Application : public QObject
{
    Q_OBJECT
public:
    explicit Application(QApplication& qapp);

    int run();

signals:
    void capture_started();

private slots:
    void start_capture(const std::string& filename);
    void open_file(const std::string& filename);
    void frame_received(Frame frame);
    void clear();

private:
    QApplication&       _qapp;
    std::vector<Frame>  _frames;
    std::set<uint32_t>  _frame_ids;
    FramesModel         _frames_model;
    Window              _window;
    std::unique_ptr<SourceInterface> _source;
};
