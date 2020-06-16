#ifndef GAME_OBJECT_FIREEXPLOSION_H
#define GAME_OBJECT_FIREEXPLOSION_H

#include "Game_Object_Base.h"

#define GAME_OBJECT_FIREEXPLOSION_DEFAULT_TIME 1000
typedef struct
{
	PX_World *pWorld;
	PX_Instance *pIns;
	px_dword alive;
	px_float range;
	px_bool ring;
	PX_Partical_Launcher launcher;
}Game_Object_FireExplosion;

px_void Game_Object_FireExplosionCreate(PX_Instance *pIns,PX_World *pWorld,px_point position,px_float range,px_bool ring);

#endif