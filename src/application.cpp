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
#include "socketcan_source.hpp"
#include "file_source.hpp"
#include "can_decoder.hpp"

#include <QApplication>

#include <cstring>

Application::Application(QApplication& qapp)
    : _qapp(qapp),
      _frames_model(_frames),
      _window(_frames_model)
{
    connect(&_window, &Window::start_capture,
            this, &Application::start_capture);
    connect(&_window, &Window::open_file, this, &Application::open_file);
    connect(&_window, &Window::clear, this, &Application::clear);
    connect(&_window, &Window::seek_frame, this, &Application::seek_frame);
    std::memset(&_sample, 0, sizeof _sample);
}

int Application::run()
{
    _window.show();
    return _qapp.exec();
}

void Application::start_capture(const std::string& filename)
{
    auto source = std::make_unique<SocketcanSource>("vcan0", filename);
    connect(source.get(), &SocketcanSource::frame_received,
            this, &Application::frame_received);
    source->start();
    _source = std::move(source);
    _window.capture_started();
}

void Application::open_file(const std::string& filename)
{
    auto source = std::make_unique<FileSource>(filename);
    connect(source.get(), &FileSource::frame_received,
            this, &Application::frame_received);
    source->start();
    int frame_count = source->frame_count();
    _source = std::move(source);
    _window.file_session_started(frame_count);
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

    _window.frame_received();
    decode_can_frame(frame, _sample);
    _window.sample_updated(_sample);
}

void Application::clear()
{
    _frame_ids.clear();
    _frames.clear();
    _frames_model.cleared();
}

void Application::seek_frame(int frame_offset)
{
    FileSource* source = dynamic_cast<FileSource*>(_source.get());
    if (source != nullptr) {
        clear();
        source->seek(frame_offset);
    }
}
