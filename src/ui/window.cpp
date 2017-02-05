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

Window::Window(FramesModel& frames_model)
    : _frames_model(frames_model)
{
    ui.setupUi(this);
    ui.frames_table->setModel(&_frames_model);
    ui.frames_table->setColumnWidth(0, 80);
    ui.frames_table->setColumnWidth(1, 180);
    ui.frames_table->setColumnWidth(2, 350);
}
