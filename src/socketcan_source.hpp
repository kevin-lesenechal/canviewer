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

#include "source_interface.hpp"

#include <string>
#include <memory>
#include <fstream>

#include <QSocketNotifier>

#include <linux/can.h>

class Frame;

class SocketcanSource : public QObject, public SourceInterface
{
    Q_OBJECT
public:
    SocketcanSource(const std::string& ifname, const std::string& filename);
    ~SocketcanSource() override;

    void start() override;
    void stop() override;

signals:
    void frame_received(Frame);

private slots:
    void socket_ready(int sockfd);

private:
    static int can_ifindex(int sockfd, const std::string& ifname);

private:
    int           _sockfd;
    sockaddr_can  _canaddr;
    std::unique_ptr<QSocketNotifier> _notifier;
};
