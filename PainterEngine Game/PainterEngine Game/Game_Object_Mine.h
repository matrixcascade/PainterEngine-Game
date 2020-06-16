#ifndef GAME_OBJECT_MINE_H
#define GAME_OBJECT_MINE_H

#include "Game_Object_Base.h"
#include "Game_Object_FireExplosion.h"

#define GAME_OBJECT_MINE_PREPARE_TIME 3000
#define GAME_OBJECT_MINE_ALIVE_TIME 20000
#define GAME_OBJECT_MINE_LAUNCH_SPEED 600
#define GAME_OBJECT_MINE_DEFAULT_SPEED 200
#define GAME_OBJECT_MINE_DAMAGE 35
#define GAME_OBJECT_MINE_SEARCH 150

typedef struct
{
	PX_World *pWorld;
	PX_Instance *pIns;
	px_int Player;
	px_dword elpased;
	px_bool brun;
	PX_Animation animation;
	px_point velocity;
	px_float angle;
}Game_Object_Mine;

px_void Game_Object_MineCreate(PX_Instance *pIns,PX_World *pWorld,px_point position,px_point direction,px_int group,px_int player);

#endif