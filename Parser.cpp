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

#include "Parser.h"
#include "stdlib.h"

Parser::Parser()
{
}

static const Command INVALID_COMMAND = {};

const Command *Parser::push(uint8_t c)
{
    if (_findMagicTries > MAX_MAGIC_TRIES)
    {
        return &INVALID_COMMAND;
        _findMagicTries = 0;
    }

    if (_state != fillBuffer)
        ++_findMagicTries;

    auto finished = false;
    switch (_state)
    {
    case findMagicNumberIdx0:
        if(c == 0xDE )
            _state = findMagicNumberIdx1;
        break;
    case findMagicNumberIdx1:
        _state = (c == 0xCA) ? findMagicNumberIdx2 : findMagicNumberIdx0;
        break;
    case findMagicNumberIdx2:
        _state = (c == 0xFB) ? findMagicNumberIdx3 : findMagicNumberIdx0;
        break;
    case findMagicNumberIdx3:
        _state = (c == 0xAD) ? fillBuffer : findMagicNumberIdx0;
        break;
    case fillBuffer:
        _findMagicTries = 0;
        reinterpret_cast<uint8_t*>(&_currentCommandBuffer)[_readIdx++] = c;
        if(_readIdx == sizeof(_currentCommandBuffer))
        {
          _readIdx = 0;
          _state = findMagicNumberIdx0;

          return &_currentCommandBuffer;
        }
        break;
    }

    return nullptr;
}