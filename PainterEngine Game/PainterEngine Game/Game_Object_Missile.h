#ifndef GAME_OBJECT_MISSILE_H
#define GAME_OBJECT_MISSILE_H

#include "Game_Object_Base.h"
#include "Game_Object_FireExplosion.h"

#define GAME_OBJECT_MISSILE_PREPARE_TIME 500
#define GAME_OBJECT_MISSILE_ALIVE_TIME 10000
#define GAME_OBJECT_MISSILE_DEFAULT_SPEED1 500
#define GAME_OBJECT_MISSILE_MAX_SPEED2 1000
#define GAME_OBJECT_MISSILE_DAMAGE 25
#define GAME_OBJECT_MISSILE_SEARCH 200

typedef struct
{
	PX_World *pWorld;
	PX_Instance *pIns;
	px_int Player;
	px_dword elpased;
	px_bool brun;
	PX_Animation animation;
	px_point velocity;
	px_point direction;
}Game_Object_Missile;

px_void Game_Object_MissileCreate(PX_Instance *pIns,PX_World *pWorld,px_point position,px_point direction,px_int group,px_int player);

#endif