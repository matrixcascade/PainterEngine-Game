#ifndef GAME_OBJECT_ALIEN_H
#define GAME_OBJECT_ALIEN_H

#include "Game_Object_Base.h"
#include "Game_Object_FireExplosion.h"
#include "Game_Object_Pickup.h"

#define GAME_ALIEN_GEN_SPACE 300 
#define GAME_ALIEN_SPEED 80 
#define  GAME_ALIEN_DEFAULT_LIFE 80

typedef struct
{
	px_float life; 
	px_point velocity;
	PX_Animation animation;
	px_bool animationTag;
	px_dword beAttackElpased;

	PX_Instance *pIns;
	PX_World *pWorld;
}Game_Object_Alien;

px_void Game_Object_AlienCreate(PX_Instance *pIns,PX_World *pWorld);

#endif