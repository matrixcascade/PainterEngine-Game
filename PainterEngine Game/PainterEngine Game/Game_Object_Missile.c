#include "Game_Object_missile.h"

px_void Game_Object_MissileImpact(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	PX_Object_Event pe;
	Game_Object_Missile *pMissile=(Game_Object_Missile *)pObject->pObject;
	PX_WorldRemoveObject(pMissile->pWorld,pObject);

	Game_Object_FireExplosionCreate(pMissile->pIns,pMissile->pWorld,PX_POINT(pObject->x,pObject->y,0),30,PX_TRUE);

	pe.Event=GAME_OBJECT_EVENT_DAMAGE;
	pe.Param_float[0]=GAME_OBJECT_MISSILE_DAMAGE;
	pe.Param_int[1]=pMissile->Player;
	PX_ObjectPostEvent((PX_Object *)e.Param_ptr[0],pe);
}

px_void Game_Object_MissileOnVelocityChange(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	Game_Object_Missile *pMissile=(Game_Object_Missile *)pObject->pObject;
	pMissile->velocity=PX_PointAdd(pMissile->velocity,PX_POINT(e.Param_float[0],e.Param_float[1],0));
}

px_void Game_Object_MissileOnDamage(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	Game_Object_Missile *pMissile=(Game_Object_Missile *)pObject->pObject;
	PX_WorldRemoveObject(pMissile->pWorld,pObject);
	Game_Object_FireExplosionCreate(pMissile->pIns,pMissile->pWorld,PX_POINT(pObject->x,pObject->y,0),30,PX_TRUE);
}


px_void Game_Object_MissileFree(PX_Object *pObject)
{
	Game_Object_Missile *pMissile=(Game_Object_Missile *)pObject->pObject;
	PX_AnimationFree(&pMissile->animation);
}


px_void Game_Object_MissileUpdate(PX_Object *pObject,px_dword elpased)
{
	px_float v;
	Game_Object_Missile *pMissile=(Game_Object_Missile *)pObject->pObject;
	PX_World *pWorld=pMissile->pWorld;

	pMissile->elpased+=elpased;

	if (pMissile->elpased<GAME_OBJECT_MISSILE_PREPARE_TIME)
	{
		v=(GAME_OBJECT_MISSILE_PREPARE_TIME-pMissile->elpased)*1.0f/GAME_OBJECT_MISSILE_PREPARE_TIME*GAME_OBJECT_MISSILE_DEFAULT_SPEED1;
		pMissile->velocity=PX_PointMul(PX_PointUnit(pMissile->direction),v);
	}
	else
	{
		pMissile->velocity=PX_PointAdd(pMissile->velocity,PX_PointMul(PX_PointUnit(pMissile->direction),elpased/2.0f));
		if (PX_PointMod(pMissile->velocity)>GAME_OBJECT_MISSILE_MAX_SPEED2)
		{
			pMissile->velocity=PX_PointMul(PX_PointUnit(pMissile->velocity),GAME_OBJECT_MISSILE_MAX_SPEED2);
		}
	}

	
	pObject->x+=pMissile->velocity.x*elpased/1000;
	pObject->y+=pMissile->velocity.y*elpased/1000;

	if (pMissile->elpased>=GAME_OBJECT_MISSILE_ALIVE_TIME)
	{
		PX_WorldRemoveObject(pWorld,pObject);
	}

	if (pObject->x<0||pObject->x>pMissile->pWorld->world_width||\
		pObject->y<0||pObject->y>pMissile->pWorld->world_height\
		)
	{
		PX_WorldRemoveObject(pMissile->pWorld,pObject);
	}
}

px_void Game_Object_MissileRender(px_surface *psurface,PX_Object *pObject,px_dword elpased)
{
	Game_Object_Missile *pMissile=(Game_Object_Missile *)pObject->pObject;
	px_dword alive=GAME_OBJECT_MISSILE_ALIVE_TIME-pMissile->elpased;
	PX_AnimationUpdate(&pMissile->animation,elpased);
	if (pMissile->elpased>=GAME_OBJECT_MISSILE_PREPARE_TIME)
	{
		if (!pMissile->brun)
		{
			pMissile->brun=PX_TRUE;
			PX_AnimationSetCurrentPlayAnimation(&pMissile->animation,1);
			PX_PlayLocalSoundFromResource(pMissile->pIns,pMissile->pWorld,pObject->x+pMissile->pWorld->offsetx,pObject->y+pMissile->pWorld->offsety,GAME_TEXT_KEY_MISSILE_LAUNCH,PX_FALSE);
		}
	}

	if (alive<300)
	{
		PX_TEXTURERENDER_BLEND blend;
		blend.hdr_R=1;
		blend.hdr_G=1;
		blend.hdr_B=1;
		blend.alpha=alive/300.0f;
		PX_AnimationRender_vector(psurface,&pMissile->animation,PX_POINT(pObject->x,pObject->y,0),pMissile->direction,PX_TEXTURERENDER_REFPOINT_CENTER,&blend);
	}
	else
	{
		PX_AnimationRender_vector(psurface,&pMissile->animation,PX_POINT(pObject->x,pObject->y,0),pMissile->direction,PX_TEXTURERENDER_REFPOINT_CENTER,PX_NULL);
	}
}



PX_Object * Game_Object_MissileCreateObject(PX_Instance *pIns,PX_World *pWorld,px_point position,px_point direction,px_dword impactTest,px_int player)
{
	PX_Object *pObject;
	PX_Animationlibrary *pAnimationLibrary;
	Game_Object_Missile *pMissile;

	pAnimationLibrary=PX_ResourceLibraryGetAnimationLibrary(&pIns->runtime.ResourceLibrary,GAME_TEXT_KEY_MISSILE_AMMO);

	pMissile=(Game_Object_Missile *)MP_Malloc(&pIns->runtime.mp_game,sizeof(Game_Object_Missile));

	if (pMissile==PX_NULL)
	{
		PX_ASSERT();
		return PX_NULL;
	}

	pObject=(PX_Object *)PX_ObjectCreate(&pIns->runtime.mp_game,PX_NULL,0,0,0,0,0,0);

	if (!pObject)
	{
		MP_Free(&pIns->runtime.mp_game,pMissile);
		return PX_NULL;
	}

	pObject->x=position.x;
	pObject->y=position.y;
	pObject->z=GAME_AMMO_Z;
	pObject->Width=8;
	pObject->Height=8;
	pObject->diameter=8;


	pObject->pObject=pMissile;
	pObject->Type=GAME_OBJECT_TYPE_MISSILE;
	pObject->ReceiveEvents=PX_TRUE;
	pObject->Func_ObjectFree=Game_Object_MissileFree;
	pObject->Func_ObjectUpdate=Game_Object_MissileUpdate;
	pObject->Func_ObjectRender=Game_Object_MissileRender;
	pObject->User_ptr=PX_NULL;
	pObject->impact_test_type=impactTest;
	pObject->impact_Object_type=GAME_IMPACT_TYPE_AMMO;

	pMissile->pWorld=pWorld;
	pMissile->pIns=pIns;
	pMissile->elpased=0;
	pMissile->brun=PX_FALSE;
	PX_AnimationCreate(&pMissile->animation,pAnimationLibrary);
	PX_AnimationSetCurrentPlayAnimation(&pMissile->animation,0);
	pMissile->velocity=PX_POINT(0,0,0);
	pMissile->direction=direction;
	pMissile->Player=player;

	PX_ObjectRegisterEvent(pObject,PX_OBJECT_EVENT_IMPACT,Game_Object_MissileImpact,PX_NULL);
	PX_ObjectRegisterEvent(pObject,GAME_OBJECT_EVENT_DAMAGE,Game_Object_MissileOnDamage,PX_NULL);
	PX_ObjectRegisterEvent(pObject,GAME_OBJECT_EVENT_CHANGEVELOCITY,Game_Object_MissileOnVelocityChange,PX_NULL);

	return pObject;
}

px_void Game_Object_MissileCreate(PX_Instance *pIns,PX_World *pWorld,px_point position,px_point direction,px_int group,px_int player)
{
	PX_Object *pObject;
	pObject=Game_Object_MissileCreateObject(pIns,pWorld,position,direction,Game_Object_GroupImpactTestType(group),player);
	if (pObject)
	{
		PX_WorldAddObject(pWorld,pObject);
	}
}