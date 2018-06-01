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

#include "frame.hpp"
#include "sample.hpp"

namespace {

bool extract_gear(Sample& sample, uint8_t val)
{
    switch (val)
    {
    case 0x00: sample.gear = 0;   break; // Neutral ('-' on dashboard)
    case 0x20: sample.gear = 1;   break;
    case 0x40: sample.gear = 2;   break;
    case 0x60: sample.gear = 3;   break;
    case 0x80: sample.gear = 4;   break;
    case 0xa0: sample.gear = 5;   break;
    case 0xc0: sample.gear = 6;   break;
    case 0xe0: sample.gear = 255; break; // Blank display on dashboard
    default: return false;
    }
    return true;
}

void extract_throttle(Sample& sample, uint8_t val8)
{
    // 95 possible values (0x22 - 0x80)
    uint16_t val16 = static_cast<uint16_t>((val8 - 0x22) * 697);

    if (val8 == 0x80) {
        sample.throttle = 255;
    } else {
        sample.throttle = static_cast<uint8_t>(val16 / 256);
    }
}

void extract_engine_speed(Sample& sample, const uint8_t* data)
{
    sample.engine_speed = static_cast<uint16_t>(data[2] * 100);
}

bool extract_engine_temp(Sample& sample, uint8_t data)
{
    if (data < 0x70 || data > 0xea) {
        return false;
    }

    int val = 40 + (data - 0x70) * 625 / 1000;
    sample.engine_temp = static_cast<uint8_t>(val);

    return true;
}

bool extract_air_temp(Sample& sample, uint8_t data)
{
    if (data < 0x21 || data > 0xcf) {
        return false;
    }

    int val = -9 + (data - 0x21) * 625 / 1000;
    sample.air_temp = static_cast<int8_t>(val);

    return true;
}

}

bool decode_can_frame(const Frame& frame, Sample& sample)
{
    bool nok;

    switch (frame.id)
    {
    case 0x020a: // Engine speed (1 (2?) byte(s)) + ??? [DLC = 8]
        if (frame.size != 8) return false;
        extract_engine_speed(sample, frame.data);
        break;

    case 0x216: // Throttle + (speed?) + ??? [DLC = 7]
        if (frame.size != 7) return false;
        extract_throttle(sample, frame.data[0]);
        break;

    case 0x0236: // Gear [DLC = 1]
        if (frame.size != 1) return false;
        return extract_gear(sample, frame.data[0]);

    case 0x023a: // Diagnostic [DLC = 3]
        if (frame.size != 3) return false;
        sample.flags.engine_alert = frame.data[0] & 0x0c;
        return (frame.data[0] & 0xf0) == 0
               && frame.data[1] == 0xff
               && frame.data[2] == 0xff;

    case 0x023e: // Engine temp (1 byte) + air temp (1 byte) + ??? [DLC = 4]
        if (frame.size != 4) return false;
        nok = !extract_engine_temp(sample, frame.data[0]);
        nok |= !extract_air_temp(sample, frame.data[1]);
        nok |= frame.data[2] != 0x00;
        nok |= frame.data[3] != 0x00;
        return !nok;

    default: return false;
    }
    return true;
}
