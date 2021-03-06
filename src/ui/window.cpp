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

#include "window.hpp"
#include "frames_model.hpp"

#include <QFileDialog>

Window::Window(FramesModel& frames_model)
    : _frames_model(frames_model),
      _speed(0),
      _speed_sum(0),
      _speed_i(0)
{
    ui.setupUi(this);
    ui.frames_table->setModel(&_frames_model);
    ui.frames_table->setColumnWidth(0, 80);
    ui.frames_table->setColumnWidth(1, 180);
    for (int i = 2; i < 10; ++i) {
        ui.frames_table->setColumnWidth(i, 42);
    }

    connect(ui.action_clear, &QAction::triggered, this, &Window::clear);
    connect(ui.action_start, &QAction::triggered,
            this, &Window::start_capture_action);
    connect(ui.action_open, &QAction::triggered,
            this, &Window::open_file_action);
    connect(ui.frame_slider, &QSlider::valueChanged, this, &Window::seek_frame);
}

void Window::capture_started()
{
    ui.button_back->setEnabled(false);
    ui.button_stop->setEnabled(true);
    ui.button_forward->setEnabled(false);
    ui.action_start->setEnabled(false);
    ui.frame_slider->setEnabled(false);
    ui.frame_slider->setMaximum(0);
    ui.frame_slider->setValue(0);
}

void Window::file_session_started(int frame_count)
{
    ui.button_back->setEnabled(true);
    ui.button_stop->setEnabled(true);
    ui.button_forward->setEnabled(true);
    ui.action_start->setEnabled(true);
    ui.frame_slider->setEnabled(true);
    ui.frame_slider->setMaximum(frame_count);
    ui.frame_slider->setValue(0);
}

void Window::frame_received()
{
    if (ui.frame_slider->maximum() > 0) {
        ui.frame_slider->blockSignals(true);
        ui.frame_slider->setValue(ui.frame_slider->value() + 1);
        ui.frame_slider->blockSignals(false);
        ui.statusbar->showMessage(QString("Frame %1 / %2")
                                      .arg(ui.frame_slider->value())
                                      .arg(ui.frame_slider->maximum()));
    }
}

void Window::sample_updated(const Sample& sample)
{
    ui.data_throttle->setValue(qRound(sample.throttle / 255.0 * 100.0));

    if (sample.gear == 0) {
        ui.data_gear->setText("N");
    } else if (sample.gear == 255) {
        ui.data_gear->setText("???");
    } else {
        ui.data_gear->setText(QString::number(sample.gear));
    }

    _speed_sum += sample.indicated_speed;
    if (++_speed_i >= 40) {
        _speed = qRound(_speed_sum / 40.0);
        _speed_i = 0;
        _speed_sum = 0;
    }

    ui.data_speed->setText(QString::number(_speed) + " km/h");
    ui.data_engine_speed->setValue(sample.engine_speed);
    ui.data_engine_temp->setText(QString::number(sample.engine_temp) + " °C");
    ui.data_air_temp->setText(QString::number(sample.air_temp) + " °C");
}

void Window::start_capture_action()
{
    QString file = QFileDialog::getSaveFileName(this, "Select log file");

    if (!file.isEmpty()) {
        emit start_capture(file.toStdString());
    }
}

void Window::open_file_action()
{
    QString file = QFileDialog::getOpenFileName(this, "Select log file");

    if (!file.isEmpty()) {
        emit open_file(file.toStdString());
    }
}
