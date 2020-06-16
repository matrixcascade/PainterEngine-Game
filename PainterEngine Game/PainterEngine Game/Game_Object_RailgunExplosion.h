#ifndef GAME_OBJECT_RAILGUNEXPLOSION_H
#define GAME_OBJECT_RAILGUNEXPLOSION_H

#include "Game_Object_Base.h"

#define GAME_OBJECT_RAILGUNEXPLOSION_ALIVE_TIME 600

typedef struct
{
	PX_Instance *pIns;
	PX_World *pWorld;
	PX_Animation Animation;
	px_dword alive;
}Game_Object_RailgunExplosion;

px_void Game_Object_RailgunExplosionCreate(PX_Instance *pIns,PX_World *pWorld,px_point position);


#endif
