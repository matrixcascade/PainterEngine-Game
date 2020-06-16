#include "Game_Object_mine.h"

px_void Game_Object_MineImpact(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	PX_Object_Event pe;
	Game_Object_Mine *pMine=(Game_Object_Mine *)pObject->pObject;
	PX_WorldRemoveObject(pMine->pWorld,pObject);

	Game_Object_FireExplosionCreate(pMine->pIns,pMine->pWorld,PX_POINT(pObject->x,pObject->y,0),30,PX_TRUE);

	pe.Event=GAME_OBJECT_EVENT_DAMAGE;
	pe.Param_float[0]=GAME_OBJECT_MINE_DAMAGE;
	pe.Param_int[1]=pMine->Player;
	PX_ObjectPostEvent((PX_Object *)e.Param_ptr[0],pe);
}

px_void Game_Object_MineOnVelocityChange(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	Game_Object_Mine *pMine=(Game_Object_Mine *)pObject->pObject;
	pMine->velocity=PX_PointAdd(pMine->velocity,PX_POINT(e.Param_float[0],e.Param_float[1],0));
}

px_void Game_Object_MineOnDamage(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	Game_Object_Mine *pMine=(Game_Object_Mine *)pObject->pObject;
	PX_WorldRemoveObject(pMine->pWorld,pObject);
	Game_Object_FireExplosionCreate(pMine->pIns,pMine->pWorld,PX_POINT(pObject->x,pObject->y,0),30,PX_TRUE);
}

px_void Game_Object_MineFree(PX_Object *pObject)
{
	Game_Object_Mine *pMine=(Game_Object_Mine *)pObject->pObject;
	PX_AnimationFree(&pMine->animation);
}


px_void Game_Object_MineUpdate(PX_Object *pObject,px_dword elpased)
{
	
	Game_Object_Mine *pMine=(Game_Object_Mine *)pObject->pObject;
	PX_World *pWorld=pMine->pWorld;
	px_float v;
	pMine->elpased+=elpased;

	v=PX_PointMod(pMine->velocity);
	if (v>0)
	{
		v-=elpased;
		if (v<=0)
		{
			v=0;
			pMine->velocity=PX_POINT(0,0,0);
		}
		else
		{
			pMine->velocity=PX_PointMul(PX_PointUnit(pMine->velocity),v);
		}
		
	}

	if (pMine->elpased>=GAME_OBJECT_MINE_PREPARE_TIME)
	{
		PX_Object *pTarget;
		if(PX_WorldSearchRegion(pWorld,pObject->x,pObject->y,GAME_OBJECT_MINE_SEARCH,&pTarget,1,pObject->impact_test_type))
		{
			px_point direction;
			direction=PX_POINT(pTarget->x-pObject->x,pTarget->y-pObject->y,0);
			pMine->velocity=PX_PointMul(PX_PointUnit(direction),GAME_OBJECT_MINE_DEFAULT_SPEED);
		}
	}
	

	pObject->x+=pMine->velocity.x*elpased/1000;
	pObject->y+=pMine->velocity.y*elpased/1000;

	if (pMine->elpased>=GAME_OBJECT_MINE_ALIVE_TIME)
	{
		Game_Object_FireExplosionCreate(pMine->pIns,pMine->pWorld,PX_POINT(pObject->x,pObject->y,0),30,PX_TRUE);
		PX_WorldRemoveObject(pWorld,pObject);
	}
}

px_void Game_Object_MineRender(px_surface *psurface,PX_Object *pObject,px_dword elpased)
{
	Game_Object_Mine *pMine=(Game_Object_Mine *)pObject->pObject;
	px_dword alive=GAME_OBJECT_MINE_ALIVE_TIME-pMine->elpased;
	PX_AnimationUpdate(&pMine->animation,elpased);
	if (pMine->elpased>=GAME_OBJECT_MINE_PREPARE_TIME)
	{
		if (!pMine->brun)
		{
			pMine->brun=PX_TRUE;
			PX_AnimationSetCurrentPlayAnimation(&pMine->animation,2);
		}
	}
	if (PX_PointMod(pMine->velocity)>0.1)
	{
		pMine->angle+=elpased/2.0f;
		PX_AnimationRenderRotation(psurface,&pMine->animation,PX_POINT(pObject->x,pObject->y,0),(px_int)pMine->angle,PX_TEXTURERENDER_REFPOINT_CENTER,PX_NULL);
	}
	else
	{
		PX_AnimationRender(psurface,&pMine->animation,PX_POINT(pObject->x,pObject->y,0),PX_TEXTURERENDER_REFPOINT_CENTER,PX_NULL);
	}
	
}



PX_Object * Game_Object_MineCreateObject(PX_Instance *pIns,PX_World *pWorld,px_point position,px_point direction,px_dword impactTest,px_int player)
{
	PX_Object *pObject;
	PX_Animationlibrary *pAnimationLibrary;
	Game_Object_Mine *pMine;

	pAnimationLibrary=PX_ResourceLibraryGetAnimationLibrary(&pIns->runtime.ResourceLibrary,GAME_TEXT_KEY_MINE_AMMO);

	pMine=(Game_Object_Mine *)MP_Malloc(&pIns->runtime.mp_game,sizeof(Game_Object_Mine));

	if (pMine==PX_NULL)
	{
		PX_ASSERT();
		return PX_NULL;
	}

	pObject=(PX_Object *)PX_ObjectCreate(&pIns->runtime.mp_game,PX_NULL,0,0,0,0,0,0);

	if (!pObject)
	{
		MP_Free(&pIns->runtime.mp_game,pMine);
		return PX_NULL;
	}

	pObject->x=position.x;
	pObject->y=position.y;
	pObject->z=GAME_AMMO_Z;
	pObject->Width=10;
	pObject->Height=10;
	pObject->diameter=10;


	pObject->pObject=pMine;
	pObject->Type=GAME_OBJECT_TYPE_MINE;
	pObject->ReceiveEvents=PX_TRUE;
	pObject->Func_ObjectFree=Game_Object_MineFree;
	pObject->Func_ObjectUpdate=Game_Object_MineUpdate;
	pObject->Func_ObjectRender=Game_Object_MineRender;
	pObject->User_ptr=PX_NULL;
	pObject->impact_test_type=impactTest;
	pObject->impact_Object_type=GAME_IMPACT_TYPE_SPECIALAMMO;

	pMine->pWorld=pWorld;
	pMine->pIns=pIns;
	pMine->elpased=0;
	pMine->brun=PX_FALSE;
	PX_AnimationCreate(&pMine->animation,pAnimationLibrary);
	PX_AnimationSetCurrentPlayAnimation(&pMine->animation,0);
	pMine->velocity=PX_PointMul(PX_PointUnit(direction),GAME_OBJECT_MINE_LAUNCH_SPEED);
	pMine->Player=player;
	pMine->angle=0;

	PX_ObjectRegisterEvent(pObject,PX_OBJECT_EVENT_IMPACT,Game_Object_MineImpact,PX_NULL);
	PX_ObjectRegisterEvent(pObject,GAME_OBJECT_EVENT_DAMAGE,Game_Object_MineOnDamage,PX_NULL);
	PX_ObjectRegisterEvent(pObject,GAME_OBJECT_EVENT_CHANGEVELOCITY,Game_Object_MineOnVelocityChange,PX_NULL);

	return pObject;
}

px_void Game_Object_MineCreate(PX_Instance *pIns,PX_World *pWorld,px_point position,px_point direction,px_int group,px_int player)
{
	PX_Object *pObject;
	pObject=Game_Object_MineCreateObject(pIns,pWorld,position,direction,Game_Object_GroupImpactTestType(group)^GAME_IMPACT_TYPE_NATURE,player);
	if (pObject)
	{
		PX_WorldAddObject(pWorld,pObject);
	}
}