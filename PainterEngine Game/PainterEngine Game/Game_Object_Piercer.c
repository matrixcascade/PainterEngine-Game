#include "Game_Object_Piercer.h"


Game_Object_Piercer * Game_Object_GetPiercer(PX_Object *pObject)
{
	return (Game_Object_Piercer *)pObject->pObject;
}

px_void Game_Object_PiercerFree(PX_Object *pObject)
{
	
}

px_void Game_Object_PiercerImpact(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	PX_Object_Event pe;
	Game_Object_Piercer *psg=Game_Object_GetPiercer(pObject);
	PX_WorldRemoveObject(psg->pWorld,pObject);

	Game_Object_FireExplosionCreate(psg->pIns,psg->pWorld,PX_POINT(pObject->x,pObject->y,0),10,PX_TRUE);
	pe.Event=GAME_OBJECT_EVENT_DAMAGE;
	pe.Param_float[0]=GAME_OBJECT_PIERCER_DAMAGE;
	pe.Param_int[1]=psg->Player;
	PX_ObjectPostEvent((PX_Object *)e.Param_ptr[0],pe);
}

px_void Game_Object_PiercerOnVelocityChange(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	Game_Object_Piercer *psg=(Game_Object_Piercer *)pObject->pObject;
	psg->velocity=PX_PointAdd(psg->velocity,PX_POINT(e.Param_float[0],e.Param_float[1],0));
}

px_void Game_Object_PiercerOnDamage(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	if (e.Param_float[0]>50)
	{
		Game_Object_Piercer *psg=(Game_Object_Piercer *)pObject->pObject;
		PX_WorldRemoveObject(psg->pWorld,pObject);
		Game_Object_FireExplosionCreate(psg->pIns,psg->pWorld,PX_POINT(pObject->x,pObject->y,0),10,PX_TRUE);
	}
}

px_void Game_Object_PiercerUpdate(PX_Object *pObject,px_dword elpased)
{
	Game_Object_Piercer *psg=Game_Object_GetPiercer(pObject);
	PX_World *pWorld=psg->pWorld;

	pObject->x+=psg->velocity.x*elpased/1000;
	pObject->y+=psg->velocity.y*elpased/1000;
	if (psg->alive>elpased)
	{
		psg->alive-=elpased;
	}
	else
	{
		PX_WorldRemoveObject(pWorld,pObject);
	}

	if (pObject->x<0||pObject->x>psg->pWorld->world_width||\
		pObject->y<0||pObject->y>psg->pWorld->world_height\
		)
	{
		PX_WorldRemoveObject(psg->pWorld,pObject);
	}

}



px_void Game_Object_PiercerRender(px_surface *psurface,PX_Object *pObject,px_dword elpased)
{
	Game_Object_Piercer *pDesc=Game_Object_GetPiercer(pObject);

	PX_AnimationUpdate(&pDesc->animation,elpased);

	if (pDesc->alive<300)
	{
		PX_TEXTURERENDER_BLEND blend;
		blend.hdr_R=1;
		blend.hdr_G=1;
		blend.hdr_B=1;
		blend.alpha=pDesc->alive/300.0f;
		PX_AnimationRender_vector(psurface,&pDesc->animation,PX_POINT(pObject->x,pObject->y,0),pDesc->direction,PX_TEXTURERENDER_REFPOINT_CENTER,&blend);
	}
	else
	{
		PX_AnimationRender_vector(psurface,&pDesc->animation,PX_POINT(pObject->x,pObject->y,0),pDesc->direction,PX_TEXTURERENDER_REFPOINT_CENTER,PX_NULL);
	}
}


PX_Object * Game_Object_PiercerCreateObject(PX_Instance *pIns,PX_World *pWorld,px_point position,px_point direction,px_point velocity,px_dword impactTest,px_int player)
{
	PX_Object *pObject;
	PX_Animationlibrary *pLib;
	Game_Object_Piercer *pDesc;

	pLib=PX_ResourceLibraryGetAnimationLibrary(&pIns->runtime.ResourceLibrary,GAME_TEXT_KEY_PIERCER);

	pDesc=(Game_Object_Piercer *)MP_Malloc(&pIns->runtime.mp_game,sizeof(Game_Object_Piercer));

	if (pDesc==PX_NULL)
	{
		PX_ASSERT();
		return PX_NULL;
	}

	pObject=(PX_Object *)PX_ObjectCreate(&pIns->runtime.mp_game,PX_NULL,0,0,0,0,0,0);

	if (!pObject)
	{
		MP_Free(&pIns->runtime.mp_game,pDesc);
		return PX_NULL;
	}

	pObject->x=position.x;
	pObject->y=position.y;
	pObject->z=GAME_AMMO_Z;
	pObject->Width=8;
	pObject->Height=8;
	pObject->diameter=8;


	pObject->pObject=pDesc;
	pObject->Type=GAME_OBJECT_TYPE_PIERCER;
	pObject->ReceiveEvents=PX_TRUE;
	pObject->Func_ObjectFree=Game_Object_PiercerFree;
	pObject->Func_ObjectUpdate=Game_Object_PiercerUpdate;
	pObject->Func_ObjectRender=Game_Object_PiercerRender;
	pObject->User_ptr=PX_NULL;
	pObject->impact_test_type=impactTest;
	pObject->impact_Object_type=GAME_IMPACT_TYPE_AMMO;

	pDesc->pWorld=pWorld;
	pDesc->pIns=pIns;
	pDesc->alive=GAME_OBJECT_PIERCER_ALIVE_TIME;
	PX_AnimationCreate(&pDesc->animation,pLib);
	pDesc->velocity=velocity;
	pDesc->direction=direction;
	pDesc->Player=player;
	
	PX_ObjectRegisterEvent(pObject,PX_OBJECT_EVENT_IMPACT,Game_Object_PiercerImpact,PX_NULL);
	PX_ObjectRegisterEvent(pObject,GAME_OBJECT_EVENT_DAMAGE,Game_Object_PiercerOnDamage,PX_NULL);
	PX_ObjectRegisterEvent(pObject,GAME_OBJECT_EVENT_CHANGEVELOCITY,Game_Object_PiercerOnVelocityChange,PX_NULL);

	return pObject;
}

px_void Game_Object_PiercerCreate(PX_Instance *pIns,PX_World *pWorld,px_point position,px_point direction,px_point refvelocity,px_int group,px_int player)
{
	PX_Object *pObject;
	px_point velocity;
	velocity=PX_PointMul(PX_PointUnit(direction),GAME_OBJECT_PIERCER_DEFAULT_SPEED);
	velocity=PX_PointAdd(velocity,refvelocity);
	pObject=Game_Object_PiercerCreateObject(pIns,pWorld,position,direction,velocity,Game_Object_GroupImpactTestType(group)|GAME_IMPACT_TYPE_AMMO,player);

	if (pObject)
	{
		PX_WorldAddObject(pWorld,pObject);
	}
}

