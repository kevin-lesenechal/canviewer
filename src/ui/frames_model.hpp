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

#include <QStandardItemModel>

class Frame;

class FramesModel : public QAbstractTableModel
{
public:
    explicit FramesModel(const std::vector<Frame>& frames);

    int      rowCount(const QModelIndex& index) const override;
    int      columnCount(const QModelIndex& index) const override;
    QVariant headerData(int section,
                        Qt::Orientation orientation,
                        int role) const override;
    QVariant data(const QModelIndex& index,
                  int role) const override;

    void row_inserted(int row);
    void row_updated(int row);
    void cleared();

private:
    static bool is_byte_hidden(const Frame& frame, uint8_t offset);
    static bool is_byte_normal(const Frame& frame, uint8_t offset);

private:
    const std::vector<Frame>& _frames;
};
