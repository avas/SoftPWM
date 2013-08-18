#include "SoftPWM.h"

void SoftPWM::attach(byte pin, unsigned int pwmPeriod, byte minPWMRate, byte maxPWMRate)
{
	setPin(pin);
	setPinState(LOW);
	
	
	_pwmPeriod = pwmPeriod;
	_pwmRate = 0;
	
	_isAttached = true;
	
	_minPWMRate = 0;
	_maxPWMRate = 255;
}
		
void SoftPWM::analogWrite(byte pwmRate)
{
	// If this instance is not attached to some pin, exiting.
	if (!_isAttached) return;

	// Else enabling PWM...
	_doingPWM = true;
	
	// ...getting pwmRate to fit range [_minPWMRate, _maxPWMRange]...
	byte truncatedPWMRate = (pwmRate < _minPWMRate) ? _minPWMRate : pwmRate;
	if (truncatedPWMRate > _maxPWMRate) truncatedPWMRate = _maxPWMRate;
	
	// ...and applying truncated PWM rate.
	_pwmRate = truncatedPWMRate;
}

void SoftPWM::stop()
{
	setPinState(LOW);
	_doingPWM = false;
}

void SoftPWM::refreshState()
{
	// If PWM is not active, exiting.
	if (!_doingPWM) return;

	// Else calculating pulse width...
	int requiredPulseWidth = _pwmRate * (_pwmPeriod / (_maxPWMRate - _minPWMRate + 1));
	int currentTime = micros() % _pwmPeriod;

	// ...and changing pin state (if needed).
	if ((currentTime >= requiredPulseWidth) && (_currentPinState == HIGH))
	{
		setPinState(LOW);
	}
	else if ((currentTime < requiredPulseWidth) && (_currentPinState == LOW))
	{
		setPinState(HIGH);
	}
}
