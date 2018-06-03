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

#include "file_source.hpp"

#include <cstring>

#include <QTimer>
#include <QDebug>
#include <QFile>
#include <QtEndian>

FileSource::FileSource(const std::string& filename)
    : _run(true),
      _file(filename),
      _is_first(true),
      _prev_time(0)
{
    if (!_file) {
        throw std::runtime_error("failed to open input file");
    }

    _frame_count = static_cast<int>(QFile(filename.c_str()).size() / 19);
    _to_send.count = 0;
}

void FileSource::start()
{
    _is_first = true;
    _run = true;
    on_next_frame();
}

void FileSource::stop()
{
    _run = false;
}

void FileSource::on_next_frame()
{
    struct {
        uint32_t time;
        uint16_t time_ms;
        uint32_t id;
        uint8_t  size;
        char     data[8];
    } __attribute__((packed)) line;

    if (!_run) {
        return;
    }
    if (!_is_first) {
        emit frame_received(_to_send);
    } else {
        _is_first = false;
    }

    if (!_file.read(reinterpret_cast<char*>(&line), sizeof line)
        || line.size > 8) {
        return;
    }

    if (_prev_time == 0) {
        _prev_time = line.time;
        _prev_time_ms = line.time_ms;
    }

    _to_send.id = line.id;
    _to_send.size = line.size;
    std::memcpy(_to_send.data, line.data, _to_send.size);

    uint32_t delay = (line.time - _prev_time) * 1000
                     + (line.time_ms - _prev_time_ms);
    _prev_time = line.time;
    _prev_time_ms = line.time_ms;

    QTimer::singleShot(delay, this, &FileSource::on_next_frame);
}

void FileSource::seek(int frame_offset)
{
    _file.seekg(frame_offset * 19);
    _is_first = true;
    _prev_time = 0;
}
