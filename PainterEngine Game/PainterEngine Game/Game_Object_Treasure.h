#ifndef GAME_OBJECT_TREASURE_H
#define GAME_OBJECT_TREASURE_H

#include "Game_Object_Base.h"
#include "Game_Object_FireExplosion.h"
#include "Game_Object_Pickup.h"

#define GAME_TREASURE_DEFALUE_MAX_ROTATION_SPEED 60 
#define GAME_TREASURE_DEFALUE_ALIVE_TIME 30000 
#define GAME_TREASURE_DEFALUE_LIFE 60 
typedef struct
{
	px_float life; 
	px_point velocity;
	px_float rotation;
	px_float rotationSpeed;
	px_texture *pTexture;
	px_texture *centerTexture;
	px_dword beAttackElpased;
	px_dword elpased;
	PX_Instance *pIns;
	PX_World *pWorld;
}Game_Object_Treasure;

px_void Game_Object_TreasureCreate(PX_Instance *pIns,PX_World *pWorld,px_point position);

#endif