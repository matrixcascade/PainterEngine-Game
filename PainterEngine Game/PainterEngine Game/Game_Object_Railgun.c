#include "Game_Object_Railgun.h"

px_void Game_Object_RailgunImpact(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	PX_Object_Event pe;
	Game_Object_Railgun *pRailgun=(Game_Object_Railgun *)pObject->pObject;
	PX_WorldRemoveObject(pRailgun->pWorld,pObject);

	Game_Object_RailgunExplosionCreate(pRailgun->pIns,pRailgun->pWorld,PX_POINT(pObject->x,pObject->y,0));

	pe.Event=GAME_OBJECT_EVENT_DAMAGE;
	pe.Param_float[0]=GAME_OBJECT_RAILGUN_DAMAGE;
	pe.Param_int[1]=pRailgun->Player;
	PX_ObjectPostEvent((PX_Object *)e.Param_ptr[0],pe);
}


px_void Game_Object_RailgunFree(PX_Object *pObject)
{
	Game_Object_Railgun *pRailgun=(Game_Object_Railgun *)pObject->pObject;
	PX_AnimationFree(&pRailgun->animation);
}


px_void Game_Object_RailgunUpdate(PX_Object *pObject,px_dword elpased)
{
	PX_Object *TargetObject;
	Game_Object_Railgun *pRailgun=(Game_Object_Railgun *)pObject->pObject;
	PX_World *pWorld=pRailgun->pWorld;

	if (PX_WorldSearchRegion(pWorld,pObject->x,pObject->y,GAME_OBJECT_RAILGUN_SEARCH,&TargetObject,1,pObject->impact_test_type))
	{
		px_point guider_vector=PX_POINT(TargetObject->x-pObject->x,TargetObject->y-pObject->y,0);
		pRailgun->velocity=PX_PointMul(PX_PointUnit(guider_vector),GAME_OBJECT_RAILGUN_FOLLOW_SPEED);
	}

	pObject->x+=pRailgun->velocity.x*elpased/1000;
	pObject->y+=pRailgun->velocity.y*elpased/1000;
	if (pRailgun->alive>elpased)
	{
		pRailgun->alive-=elpased;
	}
	else
	{
		PX_WorldRemoveObject(pWorld,pObject);
	}

	if (pObject->x<0||pObject->x>pRailgun->pWorld->world_width||\
		pObject->y<0||pObject->y>pRailgun->pWorld->world_height\
		)
	{
		PX_WorldRemoveObject(pRailgun->pWorld,pObject);
	}
}

px_void Game_Object_RailgunRender(px_surface *psurface,PX_Object *pObject,px_dword elpased)
{
	Game_Object_Railgun *pRailgun=(Game_Object_Railgun *)pObject->pObject;
	PX_AnimationUpdate(&pRailgun->animation,elpased);

	if (pRailgun->alive<300)
	{
		PX_TEXTURERENDER_BLEND blend;
		blend.hdr_R=1;
		blend.hdr_G=1;
		blend.hdr_B=1;
		blend.alpha=pRailgun->alive/300.0f;
		PX_AnimationRender(psurface,&pRailgun->animation,PX_POINT(pObject->x,pObject->y,0),PX_TEXTURERENDER_REFPOINT_CENTER,&blend);
	}
	else
	{
		PX_AnimationRender(psurface,&pRailgun->animation,PX_POINT(pObject->x,pObject->y,0),PX_TEXTURERENDER_REFPOINT_CENTER,PX_NULL);
	}
}



PX_Object * Game_Object_RailgunCreateObject(PX_Instance *pIns,PX_World *pWorld,px_point position,px_point direction,px_point velocity,px_dword impactTest,px_int player)
{
	PX_Object *pObject;
	PX_Animationlibrary *pAnimationLibrary;
	Game_Object_Railgun *pRailgun;

	pAnimationLibrary=PX_ResourceLibraryGetAnimationLibrary(&pIns->runtime.ResourceLibrary,GAME_TEXT_KEY_RAILGUN_AMMO);

	pRailgun=(Game_Object_Railgun *)MP_Malloc(&pIns->runtime.mp_game,sizeof(Game_Object_Railgun));

	if (pRailgun==PX_NULL)
	{
		PX_ASSERT();
		return PX_NULL;
	}

	pObject=(PX_Object *)PX_ObjectCreate(&pIns->runtime.mp_game,PX_NULL,0,0,0,0,0,0);

	if (!pObject)
	{
		MP_Free(&pIns->runtime.mp_game,pRailgun);
		return PX_NULL;
	}

	pObject->x=position.x;
	pObject->y=position.y;
	pObject->z=GAME_AMMO_Z;
	pObject->Width=10;
	pObject->Height=10;
	pObject->diameter=10;


	pObject->pObject=pRailgun;
	pObject->Type=GAME_OBJECT_TYPE_RAILGUN;
	pObject->ReceiveEvents=PX_TRUE;
	pObject->Func_ObjectFree=Game_Object_RailgunFree;
	pObject->Func_ObjectUpdate=Game_Object_RailgunUpdate;
	pObject->Func_ObjectRender=Game_Object_RailgunRender;
	pObject->User_ptr=PX_NULL;
	pObject->impact_test_type=impactTest;
	pObject->impact_Object_type=GAME_IMPACT_TYPE_SPECIALAMMO;

	pRailgun->pWorld=pWorld;
	pRailgun->pIns=pIns;
	pRailgun->alive=GAME_OBJECT_RAILGUN_ALIVE_TIME;
	PX_AnimationCreate(&pRailgun->animation,pAnimationLibrary);
	pRailgun->velocity=velocity;
	pRailgun->direction=direction;
	pRailgun->Player=player;

	PX_ObjectRegisterEvent(pObject,PX_OBJECT_EVENT_IMPACT,Game_Object_RailgunImpact,PX_NULL);


	return pObject;
}

px_void Game_Object_RailgunCreate(PX_Instance *pIns,PX_World *pWorld,px_point position,px_point direction,px_point refvelocity,px_int group,px_int player)
{
	PX_Object *pObject;
	px_point velocity;
	velocity=PX_PointMul(PX_PointUnit(direction),GAME_OBJECT_RAILGUN_DEFAULT_SPEED);
	velocity=PX_PointAdd(velocity,refvelocity);
	pObject=Game_Object_RailgunCreateObject(pIns,pWorld,position,direction,velocity,Game_Object_GroupImpactTestType(group)^(GAME_IMPACT_TYPE_NATURE),player);
	if (pObject)
	{
		PX_WorldAddObject(pWorld,pObject);
	}
}