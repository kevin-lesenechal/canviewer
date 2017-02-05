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

#include <QApplication>

#include <cstring>

Application::Application(QApplication& qapp, const std::string& devname)
    : _qapp(qapp),
      _frames_model(_frames),
      _window(_frames_model),
      _can_source(devname)
{
    connect(&_can_source, &SocketcanSource::frame_received,
            this, &Application::frame_received);
}

int Application::run()
{
    _window.show();
    return _qapp.exec();
}

void Application::frame_received(Frame frame)
{
    auto set_it = _frame_ids.find(frame.id);

    if (set_it == _frame_ids.end()) {
        auto vec_it = std::lower_bound(
            _frames.begin(), _frames.end(), frame.id,
            [](const Frame& lhs, uint32_t rhs) { return lhs.id < rhs; });
        int row = static_cast<int>(std::distance(_frames.begin(), vec_it));
        Frame* new_frame = &*(_frames.insert(vec_it, frame));
        _frame_ids.insert(frame.id);
        new_frame->count++;
        _frames_model.row_inserted(row);
    } else {
        int row = static_cast<int>(std::distance(_frame_ids.begin(), set_it));
        Frame& new_frame = _frames[row];
        new_frame.count++;
        new_frame.size = frame.size;
        std::memcpy(new_frame.data, frame.data, frame.size);
        _frames_model.row_updated(row);
    }
}