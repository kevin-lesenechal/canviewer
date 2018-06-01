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

#define _DEFAULT_SOURCE

#include "socketcan_source.hpp"
#include "frame.hpp"

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <unistd.h>
#include <linux/can.h>

#include <system_error>
#include <cstring>

    : _sockfd(-1)
SocketcanSource::SocketcanSource(const std::string& ifname,
                                 const std::string& filename)
{
    if ((_sockfd = ::socket(PF_CAN, SOCK_RAW | SOCK_NONBLOCK, CAN_RAW)) == -1) {
        throw std::system_error({errno, std::system_category()},
                                "socket: failed to create socket");
    }

    std::memset(&_canaddr, 0, sizeof _canaddr);
    _canaddr.can_family = PF_CAN;
    if ((_canaddr.can_ifindex = can_ifindex(_sockfd, ifname)) == -1) {
        ::close(_sockfd);
        throw std::system_error({errno, std::system_category()},
                                "failed to retrieve CAN device");
    }
    _notifier = std::make_unique<QSocketNotifier>(_sockfd, QSocketNotifier::Read);
    connect(_notifier.get(), &QSocketNotifier::activated,
            this, &SocketcanSource::socket_ready);
}

SocketcanSource::~SocketcanSource()
{
    ::close(_sockfd);
}

void SocketcanSource::socket_ready(int sockfd)
{
    can_frame raw_frame;

    while (read(sockfd, &raw_frame, sizeof raw_frame) > 0) {
        Frame frame;
        frame.id = raw_frame.can_id;
        frame.count = 0;
        frame.size = raw_frame.can_dlc;
        std::memcpy(frame.data, raw_frame.data, frame.size);

        emit frame_received(frame);
    }
    if (errno != EWOULDBLOCK && errno != EAGAIN) {
        throw std::system_error({errno, std::system_category()},
                                "read: failed to read CAN raw_frame");
    }
}

int SocketcanSource::can_ifindex(int sockfd, const std::string& ifname)
{
    ifreq req;

    std::strcpy(req.ifr_name, ifname.c_str());
    if (ioctl(sockfd, SIOCGIFINDEX, &req) == -1) {
        return -1;
    }

    return req.ifr_ifindex;
}

void SocketcanSource::start()
{
    if (::bind(_sockfd, reinterpret_cast<sockaddr*>(&_canaddr),
             sizeof _canaddr) == -1) {
        throw std::system_error({errno, std::system_category()},
                                "bind: failed to bind CAN address");
    }
}

void SocketcanSource::stop()
{
}
