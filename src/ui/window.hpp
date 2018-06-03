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

#include <QMainWindow>
#include <sample.hpp>

#include "ui_window.h"

class FramesModel;
class Sample;

class Window : public QMainWindow
{
    Q_OBJECT
public:
    explicit Window(FramesModel& frames_model);

signals:
    void start_capture(const std::string& filename);
    void open_file(const std::string& filename);
    void clear();
    void seek_frame(int);

public slots:
    void capture_started();
    void file_session_started(int frame_count);
    void frame_received();
    void sample_updated(const Sample& sample);

private slots:
    void start_capture_action();
    void open_file_action();

private:
    Ui::Window   ui;
    FramesModel& _frames_model;
    int _speed;
    int _speed_sum;
    int _speed_i;
};
