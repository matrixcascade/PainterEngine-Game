#ifndef GAME_OBJECT_PROTECTSHIELD_H
#define GAME_OBJECT_PROTECTSHIELD_H

#include "Game_Object_Base.h"


#define GAME_OBJECT_PROTECTSHIELD_ALIVE_TIME 10000
#define GAME_OBJECT_PROTECTSHIELD_DIAMETER 96
typedef struct
{
	PX_Instance *pIns;
	PX_World *pWorld;
	px_float angle;
	px_dword elpased;
	PX_Object *attachShipObject;
	px_int player;
}Game_Object_ProtectShield;

px_void Game_Object_ProtectShieldCreate(PX_Instance *pIns,PX_World *pWorld,PX_Object *pShipObject,px_int player);

#endif
