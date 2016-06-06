////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
//
//  Copyright (c) Microsoft Corporation. All rights reserved.
//  Implementation for SAM7X: Copyright (c) Secret Labs LLC.
//
//  *** PWM Driver ***
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <tinyhal.h>
#include "..\..\..\..\DeviceCode\Targets\Native\AT91\DeviceCode\AT91.h"

#if defined(ADS_LINKER_BUG__NOT_ALL_UNUSED_VARIABLES_ARE_REMOVED)
#pragma arm section zidata = "g_SAM7X_PWM_Initialized"
#endif

BOOL g_SAM7X_PWM_Initialized = FALSE;

#if defined(ADS_LINKER_BUG__NOT_ALL_UNUSED_VARIABLES_ARE_REMOVED)
#pragma arm section zidata
#endif

#define TOTAL_PWM_CHANNELS 4

//--//

BOOL PWM_Initialize(PWM_CHANNEL channel)
{
	GPIO_PIN pwmPin = PWM_GetPinForChannel(channel);

    // verify that this is a valid PWM pin; otherwise, fail
    if ((UINT32)channel >= TOTAL_PWM_CHANNELS) 
	{
		return FALSE;
	}

    AT91_GPIO_Driver::DisablePin(pwmPin, RESISTOR_DISABLED, 0, GPIO_ALT_MODE_1);

    AT91_PWM &pwm = AT91::PWM();

    if (!g_SAM7X_PWM_Initialized)
    {
	    // Power Management Controller
	    AT91_PMC &pmc = AT91::PMC();
	    pmc.EnablePeriphClock(AT91C_ID_PWMC);

		// set PWM clock A frequency to 1MHz, turn off PWM clock B
		pwm.PWM_MR = (UINT32)(48 + (0 << 8)); // PWM divisor is 48 (48 MHz / 1 MHz = 48); PWM prescaler is zero (MCK/(2^0))
	}
			
	// if PWM channel is somehow enabled, hold one moment while we disable it...
	if (pwm.PWM_SR & (1 << (UINT32)channel))
	{
		pwm.PWM_DIS = 1 << (UINT32)channel;
		// wait for channel to be disabled
		while(pwm.PWM_SR & (1 << (UINT32)channel));
	}

	// set PWM channel settings (1MHz clock using clka, left aligned, start waveforms at low level)
	AT91_PWM_CH &pwmch = AT91::PWM_CH((UINT32)channel);
	pwmch.PWM_CMR = AT91_PWM_CH::PWM_CMR_CPRE_CLKA;
	// start with our period at 100 cycles and our dutycycle at zero.  [i.e. off mode]
	pwmch.PWM_CPRD = 100;
	pwmch.PWM_CDTY = 0;
    // enable PWM channel
    pwm.PWM_ENA = 1 << (UINT32)channel;
	
	return TRUE;
}

BOOL PWM_Uninitialize(PWM_CHANNEL channel)
{
    // disable PWM channel
    AT91_PWM &pwm = AT91::PWM();
    pwm.PWM_DIS = 1 << (UINT32)channel;

    return TRUE;
}

BOOL PWM_ApplyConfiguration(PWM_CHANNEL channel, GPIO_PIN pin, UINT32& period, UINT32& duration, PWM_SCALE_FACTOR& scale, BOOL invert)
{
	// adjust our period and duration to microseconds if necessary
	UINT32 periodMs, durationMs;
	if (scale == PWM_NANOSECONDS)
	{
		periodMs = period / 1000;
		durationMs = duration / 1000;
	}
	else if (scale == PWM_MICROSECONDS)
	{
		periodMs = period;
		durationMs = duration;
	}
	else if (scale == PWM_MILLISECONDS)
	{
		periodMs = period * 1000;
		durationMs = duration * 1000;
	}
	
	// validate that our inputs are within acceptable range
	if (periodMs >= 0x10000) // only first 16 bits of period are significant; our max period w/ 1MHz clock is ~16 periods/second
		return FALSE;
	if (durationMs >= 0x10000) // only first 16 bits of duration are significant; also, must be <= period
		return FALSE;

    AT91_PWM &pwm = AT91::PWM();
	AT91_PWM_CH &pwmch = AT91::PWM_CH((UINT32)channel);

	// if channel is disabled use CPRD/CDTY; otherwise use the update register
	if (pwm.PWM_SR & (1 << (UINT32)channel))
	{
		pwmch.PWM_CPRD = periodMs;
		pwmch.PWM_CDTY = durationMs;
	}
	else
	{
		// write period to the update register
		pwmch.PWM_CMR |= AT91_PWM_CH::PWM_CMR_CPD; // update register will hold period
		pwmch.PWM_CUPD = periodMs;
		// wait for period to be updated
		while(pwmch.PWM_CPRD & 0xFFFF != periodMs);
		
		// write duration to the update register
		pwmch.PWM_CMR &= ~AT91_PWM_CH::PWM_CMR_CPD; // update register will hold duration
		pwmch.PWM_CUPD = durationMs;
		// wait for duration to be updated
		while(pwmch.PWM_CDTY & 0xFFFF != durationMs);
	}
	
	// if the channel is being inverted from its current polarity, then disable and then re-enable the channel
	if (((pwmch.PWM_CMR & AT91_PWM_CH::PWM_CMR_CPOL) > 0) == invert)
	{
		// channel polarity is being reversed
		BOOL disableAndEnableChannel = FALSE;
		if (pwm.PWM_SR & (1 << (UINT32)channel))
			disableAndEnableChannel = true; // we must disable and then re-enable the channel to reverse polarity
			
		if (disableAndEnableChannel)
		    pwm.PWM_DIS = 1 << (UINT32)channel;	// disable channel
		
		// set polarity
		if (!invert)
			pwmch.PWM_CMR |= AT91_PWM_CH::PWM_CMR_CPOL;
		else
			pwmch.PWM_CMR &= ~AT91_PWM_CH::PWM_CMR_CPOL;

		if (disableAndEnableChannel)
		    pwm.PWM_ENA = 1 << (UINT32)channel;	// enable channel
	}

    return TRUE;
}

BOOL PWM_Start(PWM_CHANNEL channel, GPIO_PIN pin)
{
    // enable PWM channel
    AT91_PWM &pwm = AT91::PWM();
    pwm.PWM_ENA = 1 << (UINT32)channel;

    return TRUE;
}

void PWM_Stop(PWM_CHANNEL channel, GPIO_PIN pin)
{
    // disable PWM channel
    AT91_PWM &pwm = AT91::PWM();
    pwm.PWM_DIS = 1 << (UINT32)channel;
}

BOOL PWM_Start(PWM_CHANNEL* channel, GPIO_PIN* pin, UINT32 count)
{
	// enable PWM channels simultaneously
    AT91_PWM &pwm = AT91::PWM();
	UINT32 channelMask = 0;
	for (int iChannel = 0; iChannel < count; iChannel++)
	{
		channelMask |= 1 << (UINT32)channel;
	}
    pwm.PWM_ENA = channelMask;
		
    return TRUE;
}

void PWM_Stop(PWM_CHANNEL* channel, GPIO_PIN* pin, UINT32 count)
{
	// disable PWM channels simultaneously
    AT91_PWM &pwm = AT91::PWM();
	UINT32 channelMask = 0;
	for (int iChannel = 0; iChannel < count; iChannel++)
	{
		channelMask |= 1 << (UINT32)channel;
	}
    pwm.PWM_DIS = channelMask;
}

UINT32 PWM_PWMChannels() 
{
    return TOTAL_PWM_CHANNELS;
}

GPIO_PIN PWM_GetPinForChannel( PWM_CHANNEL channel )
{
	if ((UINT32)channel < TOTAL_PWM_CHANNELS)
    	return (GPIO_PIN)((UINT32)channel + 51);
	else
	    return GPIO_PIN_NONE;

}
