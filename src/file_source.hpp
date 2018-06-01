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

#include "frame.hpp"
#include "source_interface.hpp"

#include <string>
#include <fstream>

#include <QObject>

class Frame;

class FileSource : public QObject, public SourceInterface
{
    Q_OBJECT
public:
    explicit FileSource(const std::string& filename);

    void start() override;
    void stop() override;
    void seek(int frame_offset);

    int frame_count() const noexcept { return _frame_count; }

signals:
    void frame_received(Frame);

private slots:
    void on_next_frame();

private:
    bool          _run;
    std::ifstream _file;
    Frame         _to_send;
    bool          _is_first;
    int           _frame_count;
};
