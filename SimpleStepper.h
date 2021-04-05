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
                  int stepPin,
                  uint32_t stepsPerM);

    enum class direction
    {
        pos,
        neg,
    };

    void enable() const;
    void disable() const;

    void moveAbs(float pos);
    void moveSteps(uint32_t steps, direction dir);
    void setSpeed(float speed);
    void setPosition(float pos);

    uint32_t stepsToGo() const;

    void run();

private:
    const int _enablePin;
    const int _dirPin;
    const int _stepPin;
    const uint32_t _stepsPerM;

    // internal we calculate everything in steps
    int64_t _position{};
    uint32_t _stepsToGo{};
    uint32_t _speed{};
    direction _direction{direction::pos};

    // time is in micro seconds
    unsigned long _stepInterval{};
    unsigned long _lastStepTime{};
};