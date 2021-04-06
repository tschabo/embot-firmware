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

/// \brief SimpleStepper controls a stepper without acceleration
/// We try to keep it simple, to drive the stepper as fast as possible.
/// Only implement acceleration if we need it to reach higher speeds!
class SimpleStepper
{
public:
    SimpleStepper(int enablePin,
                  int dirPin,
                  int stepPin);

    enum class direction
    {
        pos,
        neg,
    };

    void enable() const;
    void disable() const;

    void moveSteps(int32_t steps, uint32_t stepIntervalInMicroSeconds);

    uint32_t stepsToGo() const;

    void run();

private:
    const int _enablePin;
    const int _dirPin;
    const int _stepPin;

    // internal we calculate everything in steps
    int32_t _stepsToGo{};

    // time is in micro seconds
    unsigned long _stepInterval{};
    unsigned long _lastStepTime{};
};