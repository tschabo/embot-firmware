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

Command *Parser::push(char c)
{
    auto finished = false;
    switch (_state)
    {
    case findStart:
        if((unsigned char)c == 0xEB)
            _state = fillBuffer;
        break;
    case fillBuffer:
        reinterpret_cast<char*>(&_currentCommandBuffer)[_readIdx++] = c;
        if(_readIdx == sizeof(_currentCommandBuffer))
        {
          _readIdx = 0;
          _state = findStart;

          return _currentCommandBuffer.isChecksumValid() ? &_currentCommandBuffer : nullptr;
        }
        break;
    }

    return nullptr;
}