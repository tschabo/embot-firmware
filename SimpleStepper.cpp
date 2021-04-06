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

#include "SimpleStepper.h"
#include "pico/stdlib.h"

SimpleStepper::SimpleStepper(int enablePin,
                             int dirPin,
                             int stepPin) : _enablePin(enablePin),
                                                   _dirPin(dirPin),
                                                   _stepPin(stepPin)
{
    gpio_init(_enablePin);
    gpio_set_dir(_enablePin, GPIO_OUT);
    gpio_put(_enablePin, 1); // disable is default

    gpio_init(_dirPin);
    gpio_set_dir(_dirPin, GPIO_OUT);
    gpio_put(_dirPin, 0);

    gpio_init(_stepPin);
    gpio_set_dir(_stepPin, GPIO_OUT);
    gpio_put(_stepPin, 0);
}

void SimpleStepper::enable() const
{
    gpio_put(_enablePin, 0);
}

void SimpleStepper::disable() const
{
    gpio_put(_enablePin, 1);
}

uint32_t SimpleStepper::stepsToGo() const
{
    return _stepsToGo >= 0 ? _stepsToGo : -_stepsToGo;
}

void SimpleStepper::moveSteps(int32_t steps, uint32_t stepIntervalInMicroSeconds)
{
    gpio_put(_dirPin, steps > 0 ? 1 : 0);
    _stepInterval = stepIntervalInMicroSeconds;
    _stepsToGo = steps;
}

void SimpleStepper::run()
{
    if (_stepsToGo == 0)
        return;

    auto current = time_us_64();

    if ((current - _lastStepTime) >= _stepInterval)
    {
        // step !!!
        gpio_put(_stepPin, 1);
        sleep_us(2); // TODO perhaps we can get rid of this by using PIO
        gpio_put(_stepPin, 0);
        if (_stepsToGo > 0)
            --_stepsToGo;
        else if (_stepsToGo < 0)
            ++_stepsToGo;
        _lastStepTime = current;
    }
}