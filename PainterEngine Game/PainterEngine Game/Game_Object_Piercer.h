#ifndef GAME_OBJECT_PIERCER_H
#define GAME_OBJECT_PIERCER_H

#include "Game_Object_Base.h"
#include "Game_Object_FireExplosion.h"

#define GAME_OBJECT_PIERCER_ALIVE_TIME 2000
#define GAME_OBJECT_PIERCER_DEFAULT_SPEED 650
#define GAME_OBJECT_PIERCER_DAMAGE 6

typedef struct
{
	PX_World *pWorld;
	PX_Instance *pIns;
	px_int Player;
	px_dword alive;
	PX_Animation animation;
	px_point velocity;
	px_point direction;
	
}Game_Object_Piercer;

px_void Game_Object_PiercerCreate(PX_Instance *pIns,PX_World *pWorld,px_point position,px_point direction,px_point refvelocity,px_int group,px_int Player);
#endif