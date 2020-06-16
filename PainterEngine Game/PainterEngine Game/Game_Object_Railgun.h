#ifndef GAME_OBJECT_RAILGUN_H
#define GAME_OBJECT_RAILGUN_H

#include "Game_Object_Base.h"
#include "Game_Object_RailgunExplosion.h"

#define GAME_OBJECT_RAILGUN_ALIVE_TIME 6800
#define GAME_OBJECT_RAILGUN_DEFAULT_SPEED 350
#define GAME_OBJECT_RAILGUN_FOLLOW_SPEED 200
#define GAME_OBJECT_RAILGUN_DAMAGE 5
#define GAME_OBJECT_RAILGUN_SEARCH 200
typedef struct
{
	PX_World *pWorld;
	PX_Instance *pIns;
	px_int Player;
	px_dword alive;
	PX_Animation animation;
	px_point velocity;
	px_point direction;
}Game_Object_Railgun;

px_void Game_Object_RailgunCreate(PX_Instance *pIns,PX_World *pWorld,px_point position,px_point direction,px_point refvelocity,px_int group,px_int Player);

#endif