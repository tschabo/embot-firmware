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

#include "Mover.h"

// nano cnc shield
#define STEPPER_X_DIR_PIN 8
#define STEPPER_X_STEP_PIN 5
#define STEPPER_X_ENABLE_PIN 2

#define STEPPER_Y_DIR_PIN 7
#define STEPPER_Y_STEP_PIN 4
#define STEPPER_Y_ENABLE_PIN 2

#define STEPPER_MACHINE_DIR_PIN 6
#define STEPPER_MACHINE_STEP_PIN 3
#define STEPPER_MACHINE_ENABLE_PIN 2

Mover::Mover() : _stepperX(SimpleStepper(STEPPER_X_ENABLE_PIN, STEPPER_X_DIR_PIN, STEPPER_X_STEP_PIN)),
                 _stepperY(SimpleStepper(STEPPER_Y_ENABLE_PIN, STEPPER_Y_DIR_PIN, STEPPER_Y_STEP_PIN)),
                 _machineMotor(SimpleStepper(STEPPER_MACHINE_ENABLE_PIN, STEPPER_MACHINE_DIR_PIN, STEPPER_MACHINE_STEP_PIN))
{
    _stepperX.disable();
    _stepperY.disable();
    _machineMotor.disable();
}

void Mover::move(MoveValues x, MoveValues y, MoveValues machineMotor)
{
    _stepperX.moveSteps(x.stepCount, x.stepInterval);
    _stepperY.moveSteps(y.stepCount, y.stepInterval);
    _machineMotor.moveSteps(machineMotor.stepCount, machineMotor.stepInterval);
}

void Mover::enable()
{
    _stepperX.enable();
    _stepperY.enable();
    _machineMotor.enable();
}

void Mover::disable()
{
    _stepperX.disable();
    _stepperY.disable();
    _machineMotor.disable();
}

bool Mover::isRunning() const
{
    return _stepperX.stepsToGo() != 0 || _stepperY.stepsToGo() != 0 || _machineMotor.stepsToGo() != 0;
}

void Mover::run()
{
    if (!isRunning())
        return;

    _stepperX.run();
    _stepperY.run();
    _machineMotor.run();
}