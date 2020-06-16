#ifndef PAINTERENGINE_GAMESERVERSTARTUP_H
#define PAINTERENGINE_GAMESERVERSTARTUP_H

#include "../../PainterEngine/core/PX_Core.h"
#define PAINTERENGINE_GAMESERVER_RESULT_PORT 47760


typedef struct  
{
	px_ushort dataport;
	px_ushort gameport;
	px_dword playerCount;
}PX_GameServerStartUp_Info;

typedef struct  
{
	px_dword clientID;
	px_dword serverID;
}PX_GameServerStartUp_PlayerNetInfo;

typedef struct
{
	px_char account[32];
	px_dword group;
	px_dword rank;
	px_dword gameType;
	px_dword score;
}PX_GameServer_PlayerResult;

//Game_Play_PlayerDataDesc

#endif