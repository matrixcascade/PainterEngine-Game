#include "Game_Object_Shotgun.h"


Game_Object_ShotGun * Game_Object_GetShotGun(PX_Object *pObject)
{
	return (Game_Object_ShotGun *)pObject->pObject;
}

px_void Game_Object_ShotGunFree(PX_Object *pObject)
{

}

px_void Game_Object_ShotGunImpact(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	PX_Object_Event pe;
	Game_Object_ShotGun *psg=Game_Object_GetShotGun(pObject);
	PX_WorldRemoveObject(psg->pWorld,pObject);

	Game_Object_FireExplosionCreate(psg->pIns,psg->pWorld,PX_POINT(pObject->x,pObject->y,0),10,PX_FALSE);
	pe.Event=GAME_OBJECT_EVENT_DAMAGE;
	pe.Param_float[0]=GAME_OBJECT_SHUTGUN_DAMAGE;
	pe.Param_int[1]=psg->Player;
	PX_ObjectPostEvent((PX_Object *)e.Param_ptr[0],pe);
}

px_void Game_Object_ShotGunOnVelocityChange(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	Game_Object_ShotGun *psg=(Game_Object_ShotGun *)pObject->pObject;
	psg->velocity=PX_PointAdd(psg->velocity,PX_POINT(e.Param_float[0],e.Param_float[1],0));
}

px_void Game_Object_ShotGunOnDamage(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	Game_Object_ShotGun *psg=(Game_Object_ShotGun *)pObject->pObject;
	PX_WorldRemoveObject(psg->pWorld,pObject);
	Game_Object_FireExplosionCreate(psg->pIns,psg->pWorld,PX_POINT(pObject->x,pObject->y,0),10,PX_TRUE);
}

px_void Game_Object_ShotGunUpdate(PX_Object *pObject,px_dword elpased)
{
	Game_Object_ShotGun *psg=Game_Object_GetShotGun(pObject);
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



px_void Game_Object_ShotGunRender(px_surface *psurface,PX_Object *pObject,px_dword elpased)
{
	Game_Object_ShotGun *psg=Game_Object_GetShotGun(pObject);
	if (psg->alive<300)
	{
		PX_TEXTURERENDER_BLEND blend;
		blend.hdr_R=1;
		blend.hdr_G=1;
		blend.hdr_B=1;
		blend.alpha=psg->alive/300.0f;
		PX_TextureRenderRotation_vector(psurface,psg->tex,(px_int)pObject->x,(px_int)pObject->y,PX_TEXTURERENDER_REFPOINT_CENTER,&blend,psg->direction);
	}
	else
	{
		PX_TextureRenderRotation_vector(psurface,psg->tex,(px_int)pObject->x,(px_int)pObject->y,PX_TEXTURERENDER_REFPOINT_CENTER,PX_NULL,psg->direction);
	}
	
	if (psg->show_impact_region)
	{
		PX_GeoDrawSolidCircle(psurface,(px_int)pObject->x,(px_int)pObject->y,(px_int)pObject->diameter/2,PX_COLOR(128,255,0,0));
	}
}


PX_Object * Game_Object_ShotGunCreateObject(PX_Instance *pIns,PX_World *pWorld,px_point position,px_point direction,px_point velocity,px_dword impactTest,px_int player)
{
	PX_Object *pObject;
	px_texture *pTexture;
	Game_Object_ShotGun *psg;

	pTexture=PX_ResourceLibraryGetTexture(&pIns->runtime.ResourceLibrary,GAME_TEXT_KEY_TEX_MACHINEGUN_AMMO);

	psg=(Game_Object_ShotGun *)MP_Malloc(&pIns->runtime.mp_game,sizeof(Game_Object_ShotGun));

	if (psg==PX_NULL)
	{
		PX_ASSERT();
		return PX_NULL;
	}

	pObject=(PX_Object *)PX_ObjectCreate(&pIns->runtime.mp_game,PX_NULL,0,0,0,0,0,0);

	if (!pObject)
	{
		MP_Free(&pIns->runtime.mp_game,psg);
		return PX_NULL;
	}

	pObject->x=position.x;
	pObject->y=position.y;
	pObject->z=GAME_AMMO_Z;
	pObject->Width=8;
	pObject->Height=8;
	pObject->diameter=8;


	pObject->pObject=psg;
	pObject->Type=GAME_OBJECT_TYPE_SHOTGUN;
	pObject->ReceiveEvents=PX_TRUE;
	pObject->Func_ObjectFree=Game_Object_ShotGunFree;
	pObject->Func_ObjectUpdate=Game_Object_ShotGunUpdate;
	pObject->Func_ObjectRender=Game_Object_ShotGunRender;
	pObject->User_ptr=PX_NULL;
	pObject->impact_test_type=impactTest;
	pObject->impact_Object_type=GAME_IMPACT_TYPE_AMMO;

	psg->pWorld=pWorld;
	psg->pIns=pIns;
	psg->alive=GAME_OBJECT_SHUTGUN_ALIVE_TIME;
	psg->tex=pTexture;
	psg->show_impact_region=PX_FALSE;
	psg->velocity=velocity;
	psg->direction=direction;
	psg->Player=player;
	
	PX_ObjectRegisterEvent(pObject,PX_OBJECT_EVENT_IMPACT,Game_Object_ShotGunImpact,PX_NULL);
	PX_ObjectRegisterEvent(pObject,GAME_OBJECT_EVENT_DAMAGE,Game_Object_ShotGunOnDamage,PX_NULL);
	PX_ObjectRegisterEvent(pObject,GAME_OBJECT_EVENT_CHANGEVELOCITY,Game_Object_ShotGunOnVelocityChange,PX_NULL);

	return pObject;
}

px_void Game_Object_ShotgunCreate(PX_Instance *pIns,PX_World *pWorld,px_point position,px_point direction,px_point refvelocity,px_int group,px_int player)
{
	PX_Object *pObject;
	px_point velocity;
	velocity=PX_PointMul(PX_PointUnit(direction),GAME_OBJECT_SHUTGUN_DEFAULT_SPEED);
	velocity=PX_PointAdd(velocity,refvelocity);
	pObject=Game_Object_ShotGunCreateObject(pIns,pWorld,position,direction,velocity,Game_Object_GroupImpactTestType(group)|GAME_IMPACT_TYPE_AMMO,player);

	if (pObject)
	{
		PX_WorldAddObject(pWorld,pObject);
	}
}

