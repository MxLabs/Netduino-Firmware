////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
//
//  Copyright (c) Microsoft Corporation. All rights reserved.
//  Implementation for SAM7X: Copyright (c) Secret Labs LLC.
//
//  *** ADC Driver ***
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <tinyhal.h>
#include "..\..\..\..\DeviceCode\Targets\Native\AT91\DeviceCode\AT91.h"

#if defined(ADS_LINKER_BUG__NOT_ALL_UNUSED_VARIABLES_ARE_REMOVED)
#pragma arm section zidata = "g_SAM7X_ADC_Initialized"
#endif

BOOL g_SAM7X_ADC_Initialized = FALSE;

#if defined(ADS_LINKER_BUG__NOT_ALL_UNUSED_VARIABLES_ARE_REMOVED)
#pragma arm section zidata
#endif

#if defined(PLATFORM_ARM_Netduino) || defined(PLATFORM_ARM_NetduinoPlus)
#define TOTAL_ADC_CHANNELS 6
#elif defined(PLATFORM_ARM_NetduinoMini)
#define TOTAL_ADC_CHANNELS 4
#endif

//--//

BOOL AD_Initialize( ANALOG_CHANNEL channel, INT32 precisionInBits )
{
	GPIO_PIN analogPin = AD_GetPinForChannel(channel);

    // verify that this is a valid ADC pin; otherwise, fail
    if ((UINT32)channel >= TOTAL_ADC_CHANNELS)
    {
        return FALSE;
    }

#if defined(PLATFORM_ARM_Netduino) || defined(PLATFORM_ARM_NetduinoPlus)
    // mux pins were reserved by our C# class, but are in digital selection; activate MUXes to select analog pins.
	if ((UINT32)channel == 4)
	{
   	    ::CPU_GPIO_EnableOutputPin(AT91_GPIO_Driver::PA14, TRUE); // MUX1 -- select AD4
	}
	else if ((UINT32)channel == 5)
	{
	    ::CPU_GPIO_EnableOutputPin(AT91_GPIO_Driver::PA15, TRUE); // MUX2 -- select AD5
	}
	else
    {
        // switch this GPIO pin to the alternate B function (ADC)
        AT91_GPIO_Driver::DisablePin(analogPin, RESISTOR_DISABLED, 0, GPIO_ALT_MODE_2);
    }
#endif
	
    AT91_ADC& ADC = AT91::ADC();

    if (!g_SAM7X_ADC_Initialized)
    {
        // enable ADC peripheral clock
        AT91_PMC &pmc = AT91::PMC();
        pmc.EnablePeriphClock(AT91C_ID_ADC);
    
        // reset the ADC
        ADC.ADC_CR = AT91_ADC::ADC_SWRST;    

		// configure the adc...
        const unsigned int adcfreq = 5000000;
        unsigned int prescal = ((SYSTEM_CLOCK_HZ / (2 * adcfreq)) - 1) & 0x3F;
        unsigned int startup = (((adcfreq / 1000000) * 10 / 8) - 1) & 0x1F;
        unsigned int shtim = ((((adcfreq / 1000000) * 1200) / 1000) - 1) & 0xF;
        ADC.ADC_MR = (prescal << 8) | (startup << 16) | (shtim << 24);

        g_SAM7X_ADC_Initialized = TRUE;
    }

    // enable ADC channel
    ADC.ADC_CHER = (1 << (UINT32)channel);

	return TRUE;
}

INT32 AD_Read( ANALOG_CHANNEL channel )
{
    AT91_ADC& ADC = AT91::ADC();

    // start the adc conversion
    ADC.ADC_CR = (0x1 <<  1);

    // wait while the conversion is completed
    while((ADC.ADC_SR & (1 << (UINT32)channel)) == 0);

    // return the conversion value
    switch((UINT32)channel) {
        case 0:
            return ADC.ADC_CDR0;    
        case 1: 
            return ADC.ADC_CDR1;
        case 2:
            return ADC.ADC_CDR2;
        case 3: 
            return ADC.ADC_CDR3;
#if defined(PLATFORM_ARM_Netduino) || defined(PLATFORM_ARM_NetduinoPlus)
        case 4:
            return ADC.ADC_CDR4;
        case 5:
            return ADC.ADC_CDR5;
#endif
//        case 6:
//            return ADC.ADC_CDR6;
//        case 7:
//            return ADC.ADC_CDR7;
        default:
            return 0; // we will never get here
    }
}

UINT32 AD_ADChannels()
{
    return TOTAL_ADC_CHANNELS;
}

GPIO_PIN AD_GetPinForChannel( ANALOG_CHANNEL channel )
{
    if ((UINT32)channel >= 0 && (UINT32)channel <= 3)
    {
        // AD0-AD3 (peripheral B on pins PB27-PB30)
        return (UINT32)channel + 59;
    }
#if defined(PLATFORM_ARM_Netduino) || defined(PLATFORM_ARM_NetduinoPlus)
    else if ((UINT32)channel >= 4 && (UINT32)channel <= 5)
    {
        // AD4-AD5 (multiplexed externally to MCU with TWD/TWCK)
        return (UINT32)channel + 6;
    }
#endif
    else
    {
           return GPIO_PIN_NONE;
    }
}

BOOL AD_GetAvailablePrecisionsForChannel( ANALOG_CHANNEL channel, INT32* precisions, UINT32& size )
{
    if(precisions == NULL || size < 1) // 1 option for precision
        return FALSE;

	if((UINT32)channel >= TOTAL_ADC_CHANNELS)
		return FALSE;

	// return 
	precisions[0] = 10; // 10-bit precision
	size = 1; // 1 option for precision

	return TRUE;
}

