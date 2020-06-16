#ifndef GAME_OBJECT_NUCLEAR_H
#define GAME_OBJECT_NUCLEAR_H

#include "Game_Object_Base.h"
#include "Game_Object_FireExplosion.h"

#define GAME_OBJECT_NUCLEAR_ALIVE_TIME 8000
#define GAME_OBJECT_NUCLEAR_DEFAULT_SPEED 80
#define GAME_OBJECT_NUCLEAR_DAMAGE 150
#define GAME_OBJECT_NUCLEAR_SEARCH 400
#define GAME_OBJECT_NUCLEAR_SEARCH_COUNT 64
typedef struct
{
	PX_World *pWorld;
	PX_Instance *pIns;
	px_int Player;
	px_dword elpased;
	PX_Animation animation;
	px_point velocity;
	px_point direction;
}Game_Object_Nuclear;

px_void Game_Object_NuclearCreate(PX_Instance *pIns,PX_World *pWorld,px_point position,px_point direction,px_int group,px_int player);

#endif