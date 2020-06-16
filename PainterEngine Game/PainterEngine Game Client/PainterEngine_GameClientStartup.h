#ifndef PAINTERENGINE_GAMECLIENTSTARTUP_H
#define PAINTERENGINE_GAMECLIENTSTARTUP_H

#include "../../PainterEngine/core/PX_Core.h"

typedef struct  
{
	px_char		IpAddr[32];
	px_ushort	syncDataPort;
	px_ushort	syncFramePort;
	px_dword    clientID;
	px_dword    serverID;
}PX_GameClientStartUp;

#endif