#ifndef GAME_OBJECT_LAUNCHFIRE_H
#define GAME_OBJECT_LAUNCHFIRE_H

#include "Game_Object_Base.h"

#define GAME_OBJECT_SHOTGUNFIREASH_ALIVE_TIME 300

typedef struct
{
	PX_Instance *pIns;
	PX_World *pWorld;
	PX_Partical_Launcher launcher;
	px_dword alive;
}Game_Object_LaunchFire;

px_void Game_Object_LaunchFireCreate(PX_Instance *pIns,PX_World *pWorld,px_point position,px_point direction,px_point velocity,const px_char sndKey[]);
#endif
