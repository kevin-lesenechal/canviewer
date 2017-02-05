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

#include "frames_model.hpp"
#include "frame.hpp"

FramesModel::FramesModel(const std::vector<Frame>& frames)
    : _frames(frames)
{}

int FramesModel::rowCount(const QModelIndex&) const
{
    return _frames.size();
}

int FramesModel::columnCount(const QModelIndex&) const
{
    return 3;
}

QVariant FramesModel::headerData(int section,
                                 Qt::Orientation orientation,
                                 int role) const
{
    static QStringList headers{"Count", "ID", "Data"};

    if (orientation != Qt::Horizontal
        || section >= headers.size()) {
        return {};
    }

    if (role == Qt::DisplayRole) {
        return headers[section];
    } else if (role == Qt::FontRole) {
        return QFont("Hack", 14);
    } else {
        return {};
    }
}

QVariant FramesModel::data(const QModelIndex& index, int role) const
{
    if (static_cast<std::size_t>(index.row()) >= _frames.size()) {
        return {};
    }

    const Frame& frame = _frames[index.row()];
    QString s;

    if (role == Qt::DisplayRole) {
        switch (index.column()) {
        case 0: // Count
            return QString::number(frame.count);

        case 1: // ID
            return
                QString::number((frame.id & 0xffff'0000) >> 16, 16).rightJustified(4, '0')
                + " " + QString::number(frame.id & 0xffff, 16).rightJustified(4, '0');

        case 2: // Data
            for (int i = 0; i < frame.size; ++i) {
                s += QString::number(frame.data[i], 16).rightJustified(2, '0') + " ";
            }
            return s;

        default:
            return QVariant(":)");
        }
    } else if (role == Qt::FontRole) {
        if (index.column() == 0) {
            QFont font;
            font.setPointSize(13);
            return font;
        } else {
            return QFont("Hack", 16);
        }
    } else if (role == Qt::TextAlignmentRole) {
        if (index.column() == 0) {
            return Qt::AlignRight + Qt::AlignVCenter;
        } else if (index.column() == 1) {
            return Qt::AlignHCenter;
        } else {
            return Qt::AlignLeft;
        }
    } else {
        return {};
    }
}

void FramesModel::row_inserted(int row)
{
    beginInsertRows(QModelIndex(), row, row);
    endInsertRows();
}

void FramesModel::row_updated(int row)
{
    emit dataChanged(createIndex(row, 0), createIndex(row, 0));
    emit dataChanged(createIndex(row, 2), createIndex(row, 2));
}
