#ifndef GAME_OBJECT_PICKUP_H
#define GAME_OBJECT_PICKUP_H

#include "Game_Object_Base.h"
#include "Game_Object_PickupExplosion.h"

#define GAME_OBJECT_MISSILE_PICKUP_TIME 30000
typedef enum
{
	GAME_OBJECT_PICKUP_QUILITY_NORMAL,
	GAME_OBJECT_PICKUP_QUILITY_HIGH,
}GAME_OBJECT_PICKUP_QUALITY;

typedef struct
{
	PX_World *pWorld;
	PX_Instance *pIns;
	px_shape *logo;
	px_dword elpased;
	px_point velocity;
	GAME_OBJECT_PICKUP_QUALITY quality;
	GAME_OBJECT_PICKETUP_TYPE type;
}Game_Object_Pickup;

px_void Game_Object_PickupCreate(PX_Instance *pIns,PX_World *pWorld,px_point position,GAME_OBJECT_PICKETUP_TYPE type,GAME_OBJECT_PICKUP_QUALITY quality);
px_void Game_Object_PickupRandCreate1(PX_Instance *pIns,PX_World *pWorld,px_point position);
px_void Game_Object_PickupRandCreate2(PX_Instance *pIns,PX_World *pWorld,px_point position);
px_void Game_Object_PickupRandCreate3(PX_Instance *pIns,PX_World *pWorld,px_point position);
px_void Game_Object_PickupRandCreate4(PX_Instance *pIns,PX_World *pWorld,px_point position);
#endif