#ifndef GAME_OBJECT_STONE_H
#define GAME_OBJECT_STONE_H

#include "Game_Object_Base.h"
#include "Game_Object_FireExplosion.h"
#include "Game_Object_Pickup.h"

#define GAME_STONE_GEN_SPACE 300 
#define GAME_STONE_DEFALUE_MIN_ROTATION_SPEED 10 
#define GAME_STONE_DEFALUE_MAX_ROTATION_SPEED 360 

typedef struct
{
	px_float life; 
	px_float scale;
	px_point velocity;
	px_float rotation;
	px_float rotationSpeed;
	px_texture *pTexture;
	px_bool show_impact_region;
	px_dword beAttackElpased;

	PX_Instance *pIns;
	PX_World *pWorld;
}Game_Object_Stone;
px_void Game_Object_StoneCreate(PX_Instance *pIns,PX_World *pWorld);

#endif