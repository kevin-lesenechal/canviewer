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

#include <QWidget>

#include "ui_window.h"

class FramesModel;

class Window : public QWidget
{
    Q_OBJECT
public:
    explicit Window(FramesModel& frames_model);

signals:
    void start_capture(const std::string& filename);

public slots:
    void capture_started();
    void frame_received();

private slots:
    void start_capture_action();

private:
    Ui::Window   ui;
    FramesModel& _frames_model;
};
