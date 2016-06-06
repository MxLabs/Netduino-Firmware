#ifndef _SECRETLABS_DEPLOYMENTTRANSPORT_H_
#define _SECRETLABS_DEPLOYMENTTRANSPORT_H_

#include <tinyhal.h>

const UINT8 TRANSPORT_NONE = 0;
const UINT8 TRANSPORT_COM1 = 1;
const UINT8 TRANSPORT_COM2 = 2;
const UINT8 TRANSPORT_USB1 = 3;
const UINT8 TRANSPORT_ETH1 = 4;

struct DEPLOYMENT_TRANSPORT_CONFIGURATION
{
    UINT8 DebuggerPort;
    UINT8 MessagingPort;
    UINT8 DebugTextPort;
    UINT8 stdio;
};

extern COM_HANDLE DeploymentTransport_Get();
extern void DeploymentTransport_Initialize();
extern void DeploymentTransport_Set(COM_HANDLE transport);

#endif  //_SECRETLABS_DEPLOYMENTTRANSPORT_H_
