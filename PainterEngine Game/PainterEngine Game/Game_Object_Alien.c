#include "Game_Object_Alien.h"

static px_void Game_AlienGenInitializePositionAndVelocity(PX_Instance *pIns,PX_World *pWorld,px_point *Position,px_point *velocty,px_float mod_velocity)
{
	px_int genx,geny,vpx,vpy;
	px_point gen_v;
	switch(Game_rand()%4)
	{
	case 0:
		{
			geny=-GAME_ALIEN_GEN_SPACE;
			genx=-GAME_ALIEN_GEN_SPACE+(px_int)(Game_rand()%(pWorld->world_width+2*GAME_ALIEN_GEN_SPACE));
			vpx=(px_int)(Game_rand()%pWorld->world_width);
			vpy=0;
			gen_v=PX_PointUnit(PX_POINT((px_float)(vpx-genx),(px_float)(vpy-geny),0));
			gen_v=PX_PointMul(gen_v,mod_velocity);
		}
		break;
	case 3:
		{
			geny=pWorld->world_height+GAME_ALIEN_GEN_SPACE;
			genx=-GAME_ALIEN_GEN_SPACE+(px_int)(Game_rand()%(pWorld->world_width+2*GAME_ALIEN_GEN_SPACE));
			vpx=(px_int)(Game_rand()%pWorld->world_width);
			vpy=pWorld->world_height;

			gen_v=PX_PointUnit(PX_POINT((px_float)(vpx-genx),(px_float)(vpy-geny),0));
			gen_v=PX_PointMul(gen_v,mod_velocity);
		}
		break;
	case 1:
		{
			geny=-GAME_ALIEN_GEN_SPACE+(px_int)(Game_rand()%(pWorld->world_height+2*GAME_ALIEN_GEN_SPACE));
			genx=-GAME_ALIEN_GEN_SPACE;
			vpx=0;
			vpy=(px_int)(Game_rand()%pWorld->world_height);

			gen_v=PX_PointUnit(PX_POINT((px_float)(vpx-genx),(px_float)(vpy-geny),0));
			gen_v=PX_PointMul(gen_v,mod_velocity);
		}
		break;
	case 2:
		{
			geny=-GAME_ALIEN_GEN_SPACE+(px_int)(Game_rand()%(pWorld->world_height+2*GAME_ALIEN_GEN_SPACE));
			genx=pWorld->world_width+GAME_ALIEN_GEN_SPACE;
			vpx=pWorld->world_width;
			vpy=(px_int)(Game_rand()%pWorld->world_height);

			gen_v=PX_PointUnit(PX_POINT((px_float)(vpx-genx),(px_float)(vpy-geny),0));
			gen_v=PX_PointMul(gen_v,mod_velocity);
		}
		break;
	}
	*Position=PX_POINT((px_float)genx,(px_float)geny,0);
	*velocty=gen_v;
}

Game_Object_Alien *Game_Object_GetAlien(PX_Object *pObject)
{
	return (Game_Object_Alien *)pObject->pObject;
}

px_void Game_Object_AlienOnDamage(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	Game_Object_Alien *pst=Game_Object_GetAlien(pObject);

	if (pst->life<=e.Param_float[0])
	{
		pst->life=0;
		//destroy Alien
		PX_WorldRemoveObject(pst->pWorld,pObject);
		Game_Object_FireExplosionCreate(pst->pIns,pst->pWorld,PX_POINT(pObject->x,pObject->y,0),30,PX_TRUE);
		//
		Game_Object_PickupRandCreate4(pst->pIns,pst->pWorld,PX_POINT(pObject->x,pObject->y,0));
	}
	else
	{
		pst->life-=e.Param_float[0];
		pst->beAttackElpased=0;
	}
}

px_void Game_Object_AlienOnVelocityChange(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	Game_Object_Alien *pAlien=(Game_Object_Alien *)pObject->pObject;
	pAlien->velocity=PX_PointAdd(pAlien->velocity,PX_POINT(e.Param_float[0],e.Param_float[1],0));
}

px_void Game_Object_AlienOnImpact(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	
}


px_void Game_Object_AlienFree(PX_Object *pObject)
{
	Game_Object_Alien *pDesc=Game_Object_GetAlien(pObject);
	PX_AnimationFree(&pDesc->animation);
}

px_void Game_Object_AlienUpdate(PX_Object *pObject,px_dword elpased)
{
	Game_Object_Alien *pDesc=Game_Object_GetAlien(pObject);

	pDesc->beAttackElpased+=elpased;


	pObject->x+=pDesc->velocity.x*elpased/1000;
	pObject->y+=pDesc->velocity.y*elpased/1000;

	if (pObject->x<-GAME_ALIEN_GEN_SPACE-1||pObject->x>pDesc->pWorld->world_width+GAME_ALIEN_GEN_SPACE+1||\
		pObject->y<-GAME_ALIEN_GEN_SPACE-1||pObject->y>pDesc->pWorld->world_height+GAME_ALIEN_GEN_SPACE+1\
		)
	{
		PX_WorldRemoveObject(pDesc->pWorld,pObject);
	}

}


px_void Game_Object_AlienRender(px_surface *psurface,PX_Object *pObject,px_dword elpased)
{
	Game_Object_Alien *pDesc=Game_Object_GetAlien(pObject);
	PX_AnimationUpdate(&pDesc->animation,elpased);

	if (!pDesc->animationTag&&pDesc->life!=GAME_ALIEN_DEFAULT_LIFE)
	{
		pDesc->animationTag=PX_TRUE;
		PX_AnimationSetCurrentPlayAnimation(&pDesc->animation,2);
	}

	PX_AnimationRender(psurface,&pDesc->animation,PX_POINT(pObject->x,pObject->y,0),PX_TEXTURERENDER_REFPOINT_CENTER,PX_NULL);
}

PX_Object* Game_Object_AlienCreateObject(PX_Instance *pIns,PX_World *pWorld,px_float life,px_float velocity)
{
	PX_Object *pObject;
	PX_Animationlibrary *pLib;
	Game_Object_Alien *pDesc;
	px_point genPostion,genVelocity;

	pLib=PX_ResourceLibraryGetAnimationLibrary(&pIns->runtime.ResourceLibrary,GAME_TEXT_KEY_ALIEN);

	pDesc=(Game_Object_Alien *)MP_Malloc(&pIns->runtime.mp_game,sizeof(Game_Object_Alien));

	if (pDesc==PX_NULL)
	{
		PX_ASSERT();
		return PX_NULL;
	}

	pObject=(PX_Object *)PX_ObjectCreate(&pIns->runtime.mp_game,PX_NULL,0,0,0,0,0,0);

	if (!pObject)
	{
		MP_Free(&pIns->runtime.mp_game,pDesc);
		PX_ASSERT();
		return PX_NULL;
	}
	Game_AlienGenInitializePositionAndVelocity(pIns,pWorld,&genPostion,&genVelocity,velocity);

	pObject->x=genPostion.x;
	pObject->y=genPostion.y;
	pObject->z=GAME_STONE_Z;
	pObject->Width=86;
	pObject->Height=71;
	pObject->pObject=pDesc;
	pObject->Type=GAME_OBJECT_TYPE_ALIEN;
	pObject->ReceiveEvents=PX_TRUE;
	pObject->Func_ObjectFree=Game_Object_AlienFree;
	pObject->Func_ObjectUpdate=Game_Object_AlienUpdate;
	pObject->Func_ObjectRender=Game_Object_AlienRender;
	pObject->User_ptr=PX_NULL;
	pObject->impact_Object_type=GAME_IMPACT_TYPE_NATURE;
	pObject->impact_test_type=0;

	pDesc->beAttackElpased=0;
	pDesc->life=life;
	PX_AnimationCreate(&pDesc->animation,pLib);
	
	pDesc->velocity=genVelocity;
	pDesc->pWorld=pWorld;
	pDesc->pIns=pIns;
	pDesc->animationTag=PX_FALSE;

	PX_ObjectRegisterEvent(pObject,GAME_OBJECT_EVENT_DAMAGE,Game_Object_AlienOnDamage,PX_NULL);
	PX_ObjectRegisterEvent(pObject,GAME_OBJECT_EVENT_CHANGEVELOCITY,Game_Object_AlienOnVelocityChange,PX_NULL);
	return pObject;
}

px_void Game_Object_AlienCreate(PX_Instance *pIns,PX_World *pWorld)
{
	PX_Object *pObject;
	pObject=Game_Object_AlienCreateObject(pIns,pWorld,GAME_ALIEN_DEFAULT_LIFE,GAME_ALIEN_SPEED);
	if (pObject)
	{
		PX_WorldAddObject(pWorld,pObject);
	}
}