#ifndef GAME_OBJECT_BLACKHOLEEXPLOSION_H
#define GAME_OBJECT_BLACKHOLEEXPLOSION_H

#include "Game_Object_Base.h"


#define GAME_OBJECT_BLACKHOLEEXPLOSION_STAGE1  600
#define GAME_OBJECT_BLACKHOLEEXPLOSION_STAGE2  14000
#define GAME_OBJECT_BLACKHOLEEXPLOSION_STAGE3  400

#define GAME_OBJECT_BLACKHOLEEXPLOSION_ALIVE_TIME 15000
#define GAME_OBJECT_BLACKHOLEEXPLOSION_SEARCH_REGION 300
#define GAME_OBJECT_BLACKHOLEEXPLOSION_SEARCH_DAMAGEREGION 50

#define GAME_OBJECT_BLACKHOLEEXPLOSION_DAMAGE	100

typedef struct 
{
	px_float resx,resy;
	px_float resSize;
	px_dword alive;
}Game_Object_BlackHoleExplosion_echo;

typedef struct
{
	PX_Instance *pIns;
	PX_World *pWorld;
	px_int player;
	px_dword elpased;
	px_dword soundPlayElpased;
	px_dword lastEcho;
	Game_Object_BlackHoleExplosion_echo echo[10];
}Game_Object_BlackHoleExplosion;

px_void Game_Object_BlackHoleExplosionCreate(PX_Instance *pIns,PX_World *pWorld,px_point position,px_int player);


#endif
