#ifndef GAME_OBJECT_SHOTGUN_H
#define GAME_OBJECT_SHOTGUN_H

#include "Game_Object_Base.h"
#include "Game_Object_FireExplosion.h"

#define GAME_OBJECT_SHUTGUN_ALIVE_TIME 2000
#define GAME_OBJECT_SHUTGUN_DEFAULT_SPEED 600
#define GAME_OBJECT_SHUTGUN_DAMAGE 4
typedef struct
{
	PX_World *pWorld;
	PX_Instance *pIns;
	px_int Player;
	px_dword alive;
	px_dword show_impact_region;
	px_texture *tex;
	px_point velocity;
	px_point direction;
	
}Game_Object_ShotGun;

px_void Game_Object_ShotgunCreate(PX_Instance *pIns,PX_World *pWorld,px_point position,px_point direction,px_point refvelocity,px_int group,px_int Player);
#endif