#ifndef GAME_OBJECT_RAILGUNFIREASH_H
#define GAME_OBJECT_RAILGUNFIREASH_H

#include "Game_Object_Base.h"


#define GAME_OBJECT_RAILGUNFIRE_ALIVE_TIME 480

typedef struct
{
	PX_Instance *pIns;
	PX_World *pWorld;
	PX_Animation Animation;
	px_dword alive;
}Game_Object_RailgunFire;

px_void Game_Object_RailgunFireCreate(PX_Instance *pIns,PX_World *pWorld,px_point position);


#endif
