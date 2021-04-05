#include "Parser.h"
#include "stdlib.h"

Parser::Parser()
{
}

void Parser::parseCommand(char command)
{
    auto c = static_cast<decltype(Command().command)>(command);
    switch (c)
    {
    case Command::move:
        _state = findX;
        _currentCommandBuffer.command = c;
        break;
    case 'd': // disable steppers
    case 'e': // enable steppers
        _currentCommandBuffer.command = c;
        _finished = true;
        _state = findStart;
        break;
    default:
        break;
    }
}


bool Parser::parseFloat(char floatPart, float &thePlaceToPut)
{
    if (floatPart == ';')
    {
        thePlaceToPut = atof(_scratch_buffer.data());
        _scratch_buffer.clear();
        return true; // the caller has to set the new State
    }
    if (_scratch_buffer.full() || ((floatPart < '0' || floatPart > '9') && floatPart != '.'))
    {
        _state = findStart;
        _scratch_buffer.clear();
        return false;
    }
    _scratch_buffer.push_back(floatPart);
    return false;
}

bool Parser::parseUInt(char uintPart, uint32_t &thePlaceToPut)
{
    if (uintPart == ';')
    {
        thePlaceToPut = strtoul(_scratch_buffer.data(), nullptr, 10);
        _scratch_buffer.clear();
        return true; // the caller has to set the new State
    }
    if (_scratch_buffer.full() || uintPart < '0' || uintPart > '9')
    {
        _state = findStart;
        _scratch_buffer.clear();
        return false;
    }
    _scratch_buffer.push_back(uintPart);
    return false;
}

Command *Parser::push(char c)
{
    _finished = false;
    switch (_state)
    {
    case findStart:
        if (c != '>')
            break;
        _currentCommandBuffer = {};
        _state = evalCommand;
        break;
    case evalCommand:
        parseCommand(c);
        break;
    case findX:
        if (parseFloat(c, _currentCommandBuffer.var0))
            _state = findY;
        break;
    case findY:
        if (parseFloat(c, _currentCommandBuffer.var1))
            _state = findMachineTicks;
        break;
    case findMachineTicks:
        if(parseUInt(c, _currentCommandBuffer.var3))
            _state = findSpeed;
        break;
    case findSpeed:
        if (parseFloat(c, _currentCommandBuffer.var4))
        {
            _finished = true;
            _state = findStart;
        }
        break;
    }

    return _finished ? &_currentCommandBuffer : nullptr;
}