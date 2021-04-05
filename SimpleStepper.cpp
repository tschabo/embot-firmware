#include "SimpleStepper.h"
#include "pico/stdlib.h"

SimpleStepper::SimpleStepper(int enablePin,
                             int dirPin,
                             int stepPin,
                             uint32_t stepsPerM) : _enablePin(enablePin),
                                                   _dirPin(dirPin),
                                                   _stepPin(stepPin),
                                                   _stepsPerM(stepsPerM)
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
    return _stepsToGo;
}

void SimpleStepper::moveAbs(float pos)
{
    int32_t posInStepsFromZero{(pos * (float)_stepsPerM) / 1000.0};
    if (posInStepsFromZero > _position)
    {
        _stepsToGo = posInStepsFromZero - _position;
        _direction = direction::pos;
    }
    else if (posInStepsFromZero < _position)
    {
        _stepsToGo = _position - posInStepsFromZero;
        _direction = direction::neg;
    }
    else
    {
        _stepsToGo = 0;
    }
    gpio_put(_dirPin, (_direction == direction::pos) ? 1 : 0);
}

void SimpleStepper::moveSteps(uint32_t steps, direction dir)
{
    _position = 0;
    _direction = dir;
    _stepsToGo = steps;
}

void SimpleStepper::setSpeed(float speed)
{
    _speed = (speed * (float)_stepsPerM) / 1000; // ticks/sec
    _stepInterval = 1000000 / _speed;
}

void SimpleStepper::setPosition(float pos)
{
    _position = (pos * (float)_stepsPerM) / 1000.0;
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
        sleep_us(2);
        gpio_put(_stepPin, 0);
        switch (_direction)
        {
        case direction::neg:
            --_position;
            break;
        default:
            ++_position;
            break;
        }
        --_stepsToGo;
        _lastStepTime = current;
    }
}