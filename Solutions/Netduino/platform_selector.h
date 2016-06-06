////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Microsoft Corporation.  All rights reserved.
// Portions Copyright (c) Secret Labs LLC.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _PLATFORM_Netduino_SELECTOR_H_
#define _PLATFORM_Netduino_SELECTOR_H_ 1

/////////////////////////////////////////////////////////
//
// processor and features
//
#if defined(PLATFORM_ARM_Netduino)

#define HAL_SYSTEM_NAME             "Netduino"
#define PLATFORM_ARM_SAM7_ANY       1
#define PLATFORM_ARM_SAM7X_ANY      1
#define PLATFORM_ARM_SAM7X512       1

//
// processor and features
//
/////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////
//
#define AT91EMAC_SOCKETS_ENABLED               0

#if (AT91EMAC_SOCKETS_ENABLED)

#define AT91EMAC_DEVICE                     50

#define PLATFORM_DEPENDENT__SOCKETS_MAX_SEND_LENGTH 8192

#define NETWORK_INTERFACE_INDEX_AT91EMAC    0
#define NETWORK_INTERFACE_COUNT             1

#define NETWORK_MEMORY_PROFILE__small       1

#if defined(TCPIP_LWIP)
#include <pal\net\Network_defines_lwip.h>
#else
#include <pal\net\Network_Defines.h>
#endif

#define NETWORK_USE_LOOPBACK                1
#define NETWORK_USE_DHCP                    1

#endif

/////////////////////////////////////////////////////////
//
// constants
//
#define SYSTEM_CYCLE_CLOCK_HZ           48000000
#define SYSTEM_PERIPHERAL_CLOCK_HZ      SYSTEM_CYCLE_CLOCK_HZ
#define SYSTEM_CLOCK_HZ                 48000000
#define CLOCK_COMMON_FACTOR             1000000
//TClk 5 ( Select MCK/1024)
#define SLOW_CLOCKS_PER_SECOND          46875
#define SLOW_CLOCKS_TEN_MHZ_GCD         15625
#define SLOW_CLOCKS_MILLISECOND_GCD     125

#define FLASH_MEMORY_Base   0x00100000
#define FLASH_MEMORY_Size   0x00080000

#define SRAM1_MEMORY_Base   0x00000000
#define SRAM1_MEMORY_Size   0x00020000

#define TXPROTECTRESISTOR               RESISTOR_DISABLED
#define RXPROTECTRESISTOR               RESISTOR_DISABLED
#define CTSPROTECTRESISTOR              RESISTOR_DISABLED
#define RTSPROTECTRESISTOR              RESISTOR_DISABLED

#define INSTRUMENTATION_H_GPIO_PIN      GPIO_PIN_NONE

#if 1
    #define DEFAULT_DEPLOYMENT_PORT    USB1
#else
    #define DEFAULT_DEPLOYMENT_PORT    COM1
#endif
#define DEBUG_TEXT_PORT    DEFAULT_DEPLOYMENT_PORT
#define STDIO              DEFAULT_DEPLOYMENT_PORT
#define DEBUGGER_PORT      DEFAULT_DEPLOYMENT_PORT
#define MESSAGING_PORT     DEFAULT_DEPLOYMENT_PORT

//
// constants
/////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////
//
// macros
//

#define GLOBAL_LOCK(x)             SmartPtr_IRQ x
#define DISABLE_INTERRUPTS()       SmartPtr_IRQ::ForceDisabled()
#define ENABLE_INTERRUPTS()        SmartPtr_IRQ::ForceEnabled()
#define INTERRUPTS_ENABLED_STATE() SmartPtr_IRQ::GetState()
#define GLOBAL_LOCK_SOCKETS(x)     SmartPtr_IRQ x

#if defined(_DEBUG)
#define ASSERT_IRQ_MUST_BE_OFF()   ASSERT(!SmartPtr_IRQ::GetState())
#define ASSERT_IRQ_MUST_BE_ON()    ASSERT( SmartPtr_IRQ::GetState())
#else
#define ASSERT_IRQ_MUST_BE_OFF()
#define ASSERT_IRQ_MUST_BE_ON()
#endif
//
// macros
//
/////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////
//
// global functions
//

//
// global functions
//
/////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////
// communication facilities
//
// Port definitions: use only one to save RAM in the PAL layer
//#define TOTAL_USART_PORT       1
#define TOTAL_USART_PORT       2
#define COM1                   ConvertCOM_ComHandle(0)
#define COM2                   ConvertCOM_ComHandle(1)

#define TOTAL_USB_CONTROLLER   1
#define USB1                   ConvertCOM_UsbHandle(0)

#define TOTAL_SOCK_PORT        0

#define TOTAL_DEBUG_PORT       1
#define COM_DEBUG              ConvertCOM_DebugHandle(0)

#define COM_MESSAGING          ConvertCOM_MessagingHandle(0)

#define USART_TX_IRQ_INDEX(x)       ( (x) ? 0 : 0 )     // TODO set right indexes
#define USART_DEFAULT_PORT          COM1
#define USART_DEFAULT_BAUDRATE      115200

#define USB_IRQ_INDEX               0                   // TODO set right index

#define PLATFORM_DEPENDENT_TX_USART_BUFFER_SIZE     512  // there is one TX for each usart port
#define PLATFORM_DEPENDENT_RX_USART_BUFFER_SIZE     512  // there is one RX for each usart port
#define PLATFORM_DEPENDENT_USB_QUEUE_PACKET_COUNT     8  // there is one queue for each pipe of each endpoint and the size of a single packet is sizeof(USB_PACKET64) == 68 bytes
//
// communicaiton facilities
/////////////////////////////////////////////////////////

#define PLATFORM_DEPENDENT_FATFS_SECTORCACHE_MAXSIZE  3
#define PLATFORM_DEPENDENT_FATFS_SECTORCACHE_LINESIZE 512
#define PLATFORM_DEPENDENT_FATFS_MAX_OPEN_HANDLES     4
#define PLATFORM_DEPENDENT_FATFS_MAX_VOLUMES          2

////////////////////////////////////////////////
// CLR runtime memory configuration 
//
#define RUNTIME_MEMORY_PROFILE__extrasmall 1
//
//
////////////////////////////////////////////////


#include <processor_selector.h>

#endif // PLATFORM_ARM_Netduino
#endif // _PLATFORM_Netduino_SELECTOR_H_

