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
    return 10;
}

QVariant FramesModel::headerData(int section,
                                 Qt::Orientation orientation,
                                 int role) const
{
    static QStringList headers{"Count", "ID", "1", "2", "3", "4", "5", "6", "7", "8"};

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
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
        case 9: {
            int i = index.column() - 2;
            if (i < frame.size) {
                return QString::number(frame.data[i], 16).rightJustified(2, '0');
            } else {
                return {};
            }
        }

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
        } else {
            return Qt::AlignHCenter;
        }
    } else if (role == Qt::ForegroundRole) {
        if (index.column() > 1 && is_byte_hidden(frame, index.column() - 2)) {
            return QColor(50, 50, 50);
        } else if (index.column() > 1 && is_byte_normal(frame, index.column() - 2)) {
            return QColor(100, 100, 100);
        } else {
            return {};
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
    emit dataChanged(createIndex(row, 2), createIndex(row, 9));
}

void FramesModel::cleared()
{
    emit layoutChanged();
}

bool FramesModel::is_byte_hidden(const Frame& frame, uint8_t offset)
{
    if (frame.id == 0x236 && offset == 0
        && (frame.data[0] == 0x00 || frame.data[0] == 0x20
            || frame.data[0] == 0x40 || frame.data[0] == 0x60
            || frame.data[0] == 0x80 || frame.data[0] == 0xa0
            || frame.data[0] == 0xc0 || frame.data[0] == 0xe0)) {
        return true;
    }
    if (frame.id == 0x23e && (offset == 0 || offset == 1)) {
        return true;
    }
    if (frame.id == 0x216 && offset == 0
        && frame.data[0] >= 0x22 && frame.data[0] <= 0x80) {
        return true;
    }
    if (frame.id == 0x20a && (offset == 2 || offset == 4)) {
        return true;
    }

    return false;
}

bool FramesModel::is_byte_normal(const Frame& frame, uint8_t offset)
{
    if (frame.id == 0x20a
        && ((offset == 0 && frame.data[0] == 0x8f)
            || (offset == 1 && frame.data[1] == 0xa3)
            || (offset == 5 && frame.data[5] == 0x01))) {
        return true;
    }
    if (frame.id == 0x216
        && ((offset == 1 && frame.data[1] == 0x00)
            || (offset == 2 && frame.data[2] == 0x80)
            || (offset == 3 && frame.data[3] == 0x00)
            || (offset == 6 && frame.data[6] == 0x00))) {
        return true;
    }
    if (frame.id == 0x22e
        && ((offset == 0 && frame.data[0] == 0x00)
            || (offset == 1 && frame.data[1] == 0x00))) {
        return true;
    }
    if (frame.id == 0x23a
        && ((offset == 0 && frame.data[0] == 0x02)
            || (offset == 1 && frame.data[1] == 0xff)
            || (offset == 2 && frame.data[2] == 0xff))) {
        return true;
    }
    if (frame.id == 0x23e
        && ((offset == 2 && frame.data[2] == 0x00))) {
        return true;
    }

    return false;
}
