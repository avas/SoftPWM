#ifndef SOFT_PWM_H
#define SOFT_PWM_H

#include "Arduino.h"

/**
 * Unlocks direct work with port registers instead of using digitalWrite() from Arduino.h.
 * This will make refreshState() to work a bit faster (for about 5 us), but the SoftPWM class 
 * will consume a bit more RAM (about +2 bytes per instance), and the code will no more be 
 * compatible with Arduino Due.
 */
#define PWM_USE_PORTS_DIRECTLY

/**
 * A class implementing software PWM. It can be used to regulate speed of motors, dim LEDs or
 * do other PWM-related tasks. It is less accurate than hardware PWM, but it can be used
 * on any Arduino pin, even on pins without hardware PWM support.
 */
class SoftPWM
{
	public:
		/**
		 * Constructor.
		 */
		SoftPWM()
		{
			_isAttached = false;
			_doingPWM = false;
		}
		
		/**
		 * Attaches this instance to some pin on Arduino board. Usually you should call this method
		 * inside of setup() function of your sketch.
		 *
		 * @param Arduino pin to do PWM on. Note that this class will do software PWM, even if
		 * passed pin is PWM-enabled.
		 *
		 * @param pwmPeriod Approximate period of generated PWM signal, in microseconds. Default value 
		 * is 1024 us. You can increase this value if you need more accurate PWM, but beware that 
		 * too large values may cause flickering. Note alse that the value of this parameter must be
		 * a multiple of (maxPWMRate - minPWMRate) + 1, otherwise it can work wrong. If you use default
		 * minPWMRate and maxPWMRate, it should be multiple of 256.
		 *
		 * @param minPWMRate Minimal allowed PWM rate. Default value is 0.
		 *
		 * @param maxPWMRate Maximal allowed PWM rate. Default value is 255.
		 */
		void attach(byte pin, unsigned int pwmPeriod = 1024, byte minPWMRate = 0, byte maxPWMRate = 255);
		
		/**
		 * Stops PWM and detaches this instance from associated pin. Note that this instance will 
		 * stop working until you call attach() again.
		 */
		void detach()
		{
			stop();
			_isAttached = false;
		}
		
		/**
		 * Shows whether this instance is attached to some board pin or not.
		 *
		 * @return true if this instance is attached to some pin.
		 */
		boolean isAttached() const
		{
			return _isAttached;
		}
		
		
		/**
		 * Returns current PWM rate of this instance.
		 *
		 * @return Current PWM rate of this instance.
		 */
		byte getCurrentPWMRate() const
		{
			return _pwmRate;
		}
		
		/**
		 * Enables the PWM and sets the PWM rate.
		 *
		 * @param pwmRate Required PWM rate.
		 */
		void analogWrite(byte pwmRate);
		
		/**
		 * Stops the PWM and sets pin state to LOW.
		 */ 
		void stop();
		
		/**
		 * Main method of this class. It generates PWM signal on attached pin.
		 * You should call it from timer interrupt handler or from loop() function of your sketch.
		 * The more often you will call this method, the more accurate will be PWM.
		 */
		void refreshState();
	
	private:
	#ifndef PWM_USE_PORTS_DIRECTLY
		byte _pin;
	#else
		volatile byte* _pinPort;
		byte _pinMask;
	#endif
		
		boolean _isAttached;
		
		byte _pwmRate;
		
		unsigned int _pwmPeriod;
		
		boolean _doingPWM;
		
		byte _minPWMRate;
		byte _maxPWMRate;
		
		byte _currentPinState;

		void setPin(byte pin)
		{
		#ifndef PWM_USE_PORTS_DIRECTLY
			_pin = pin;
			pinMode(pin, OUTPUT);
		#else
			_pinMask = digitalPinToBitMask(pin);
		
			byte portIndex = digitalPinToPort(pin);
			_pinPort = portOutputRegister(portIndex);

			volatile byte* pinDDR = portModeRegister(portIndex);
			*pinDDR |= _pinMask;
		#endif
		}
		
		void setPinState(byte state)
		{
		#ifndef PWM_USE_PORTS_DIRECTLY
			digitalWrite(_pin, state);
		#else
			if (state == LOW)
				*_pinPort &= ~_pinMask;
			else
				*_pinPort |= _pinMask;
		#endif
			
			_currentPinState = state;
		}		
};

#endif