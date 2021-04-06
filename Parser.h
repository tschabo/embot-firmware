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

unsigned char Checksum(unsigned char *data, unsigned int length)
{
    static unsigned char Sum;
    static unsigned char *end;

    Sum = 0;
    end = data + length;

    do
    {
        Sum -= *data++;
    } while (data != end);
    return Sum;
}

struct Command
{
    enum : uint32_t // for right alignment
    {
        none = 0,
        move = 'm',
        enable_steppers = 'e',
        disable_steppers = 'd',
    } command{none};
    MoveValues xAxis{};
    MoveValues yAxis{};
    MoveValues motor{};
    uint32_t checksum{};

    bool isChecksumValid()
    {
        uint32_t sum{};
        uint32_t *current = reinterpret_cast<uint32_t *>(this);
        uint32_t *end = reinterpret_cast<uint32_t *>(this) + ((sizeof(Command) - sizeof(uint32_t)) / sizeof(uint32_t));
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
    Command *push(char c);

private:
    enum
    {
        findStart,
        fillBuffer,
    } _state{findStart};

    size_t _readIdx;
    Command _currentCommandBuffer{};
};
