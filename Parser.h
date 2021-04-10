// embot-firmware - Firmware for a DIY embroidery machine
// Copyright (C) 2021 Sebastian Schmitt
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#pragma once
#include <stdint.h>
#include <stddef.h>
#include "SimpleStepper.h"
#include "Mover.h"

// do not change the order of the variables
// ... this is aligned without paddings.
// We don't want a packed Struct for optimisation reasons. 
struct Command
{
    uint32_t commandIdx{};
    MoveValues xAxis{};
    MoveValues yAxis{};
    MoveValues motor{};
    enum : uint16_t
    {
        none = 0,
        move = 'm',
        enable_steppers = 'e',
        disable_steppers = 'd',
    } command{none};

    uint16_t checksum{0xFFFF}; // initial invalid

    bool valid() const
    {
        uint16_t sum{};
        auto current = reinterpret_cast<const char *>(this);
        auto end = reinterpret_cast<const char *>(this) + (sizeof(Command) - sizeof(checksum));
        do
        {
            sum += *current++;
        } while (current != end);
        return sum == checksum;
    }
};


static_assert(sizeof(Command) == 32);

class Parser
{
public:
    Parser();

    // feeding bytewise ... returns Command if the Parser found a complete Command otherwise nullptr
    const Command *push(uint8_t c);

private:
    enum : char
    {
        findMagicNumberIdx0,
        findMagicNumberIdx1,
        findMagicNumberIdx2,
        findMagicNumberIdx3,
        fillBuffer,
    } _state{findMagicNumberIdx0};

    size_t _readIdx;
    Command _currentCommandBuffer{};
    uint8_t _findMagicTries{};
    static const uint8_t MAX_MAGIC_TRIES{sizeof(Command)+5};
};
