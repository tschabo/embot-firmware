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

#include <stdio.h>
#include "pico/stdlib.h"
#include <stdint.h>
#include "CommandBuffer.h"
#include "Mover.h"

int main()
{
    stdio_init_all();

    Mover g_hoopMover;

    CommandBuffer<20> g_commandBuffer;

    bool wait = false;

    int serialData = PICO_ERROR_TIMEOUT; // means ready to read

    Command *pCompleteCommand{};

    while (true)
    {
        pCompleteCommand = nullptr;
        
        if (serialData == PICO_ERROR_TIMEOUT)
            serialData = getchar_timeout_us(0); // if nothing is read ... data remains -1

        if ((serialData != -1) && !g_commandBuffer.isFull())
        {
            pCompleteCommand = g_commandBuffer.push(serialData);
            serialData = PICO_ERROR_TIMEOUT; // reset
        }

        if (g_commandBuffer.isFull() && !wait)
        {
            
            // this is the case, when a command has completed and the buffer got full
            putchar('!'); // signal buffer full
            if(pCompleteCommand && !pCompleteCommand->valid())
                putchar('E'); // error
            wait = true;
        }
        else if (!g_commandBuffer.isFull() && wait && (serialData == PICO_ERROR_TIMEOUT))
        {
            // this is the case when the buffer was previosly full and the caller is waiting for sending more data.
            // but before we let the caller send more data ... read the pending data (if any) from the serial buffer.
            putchar('+'); // signal there is space left
            if(pCompleteCommand && !pCompleteCommand->valid())
                putchar('E'); // error
            wait = false;
        }
        else if (pCompleteCommand)
        {
            putchar(pCompleteCommand->valid() ? '>': 'E'); // normal ACK or Error ACK
        }

        if (!g_hoopMover.isRunning() && !g_commandBuffer.isEmpty())
        {
            auto &command = g_commandBuffer.pop_back();
            switch (command.command)
            {
            case Command::move:
                g_hoopMover.move(command.xAxis, command.yAxis, command.motor);
                break;
            case Command::disable_steppers:
                g_hoopMover.disable();
                putchar('<'); // signal we are finished
                break;
            case Command::enable_steppers:
                g_hoopMover.enable();
                break;
            default:
                break;
            }
        }
        g_hoopMover.run();
    }
    return 0;
}
