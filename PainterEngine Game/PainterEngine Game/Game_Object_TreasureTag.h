#ifndef GAME_OBJECT_TREASURETAG_H
#define GAME_OBJECT_TREASURETAG_H

#include "Game_Object_Base.h"
#include "Game_Object_Treasure.h"

#define GAME_TREASURETAG_DEFALUE_MAX_ROTATION_SPEED 60 
#define GAME_TREASURETAG_DEFALUE_ALIVE_TIME 10000 
typedef struct
{
	px_point velocity;
	px_float rotation;
	px_float rotationSpeed;
	px_shape *pShape;
	px_dword elpased;
	PX_Instance *pIns;
	PX_World *pWorld;
}Game_Object_TreasureTag;

px_void Game_Object_TreasureTagCreate(PX_Instance *pIns,PX_World *pWorld,px_point position);

#endif