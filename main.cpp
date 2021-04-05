

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
    bool isCommandComplete{};

    int serialData = PICO_ERROR_TIMEOUT; // means ready to read

    while (true)
    {
        if (serialData == PICO_ERROR_TIMEOUT)
            serialData = getchar_timeout_us(0); // if nothing is read ... data remains -1

        isCommandComplete = false;
        if ((serialData != -1) && !g_commandBuffer.isFull())
        {
            isCommandComplete = g_commandBuffer.push(serialData);
            serialData = PICO_ERROR_TIMEOUT; // reset
        }

        if (g_commandBuffer.isFull() && !wait)
        {
            // this is the case, when a command has completed and the buffer got full
            putchar('!'); // signal buffer full
            wait = true;
        }
        else if (!g_commandBuffer.isFull() && wait && (serialData == PICO_ERROR_TIMEOUT))
        {
            // this is the case when the buffer was previosly full and the caller is waiting for sending more data.
            // but before we let the caller send more data ... read the pending data (if any) from the serial buffer.
            putchar('+'); // signal there is space left
            wait = false;
        }
        else if (isCommandComplete)
        {
            putchar('>'); // normal ACK
        }

        if (!g_hoopMover.isRunning() && !g_commandBuffer.isEmpty())
        {
            auto &command = g_commandBuffer.pop();
            switch (command.command)
            {
            case Command::move:
                g_hoopMover.moveTo(command.var0, command.var1, command.var3, command.var4);
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
