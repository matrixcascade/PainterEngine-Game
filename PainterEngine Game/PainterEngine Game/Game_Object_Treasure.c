#include "Game_Object_Treasure.h"

Game_Object_Treasure *Game_Object_GetTreasure(PX_Object *pObject)
{
	return (Game_Object_Treasure *)pObject->pObject;
}

px_void Game_Object_TreasureOnDamage(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	Game_Object_Treasure *pst=Game_Object_GetTreasure(pObject);

	if (pst->life<=e.Param_float[0])
	{
		pst->life=0;
		//destroy Treasure
		PX_WorldRemoveObject(pst->pWorld,pObject);
		Game_Object_FireExplosionCreate(pst->pIns,pst->pWorld,PX_POINT(pObject->x,pObject->y,0),pObject->diameter/2.0f,PX_TRUE);
		//
		Game_Object_PickupRandCreate2(pst->pIns,pst->pWorld,PX_POINT(pObject->x,pObject->y,0));
	}
	else
	{
		pst->life-=e.Param_float[0];
		pst->beAttackElpased=0;
	}
}

px_void Game_Object_TreasureOnVelocityChange(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	Game_Object_Treasure *pTreasure=(Game_Object_Treasure *)pObject->pObject;
	pTreasure->velocity=PX_PointAdd(pTreasure->velocity,PX_POINT(e.Param_float[0],e.Param_float[1],0));
}


px_void Game_Object_TreasureFree(PX_Object *pObject)
{

}

px_void Game_Object_TreasureUpdate(PX_Object *pObject,px_dword elpased)
{
	Game_Object_Treasure *pst=Game_Object_GetTreasure(pObject);

	pst->beAttackElpased+=elpased;
	pst->elpased+=elpased;

	pst->rotation+=pst->rotationSpeed*elpased/1000;
	pObject->x+=pst->velocity.x*elpased/1000;
	pObject->y+=pst->velocity.y*elpased/1000;

	if (pObject->x<-1||pObject->x>pst->pWorld->world_width+1||\
		pObject->y<-1||pObject->y>pst->pWorld->world_height+1||\
		pst->elpased>GAME_TREASURE_DEFALUE_ALIVE_TIME
		)
	{
		PX_WorldRemoveObject(pst->pWorld,pObject);
	}

}


px_void Game_Object_TreasureRender(px_surface *psurface,PX_Object *pObject,px_dword elpased)
{
	Game_Object_Treasure *pst=Game_Object_GetTreasure(pObject);

	if (pst->beAttackElpased<50)
	{
		PX_TEXTURERENDER_BLEND blend;
		blend.alpha=1;
		blend.hdr_B=0;
		blend.hdr_G=0;
		blend.hdr_R=1;
		PX_TextureRenderEx(psurface,pst->pTexture,(px_int)pObject->x,(px_int)pObject->y,PX_TEXTURERENDER_REFPOINT_CENTER,&blend,1.0f,pst->rotation);
	}
	else
		PX_TextureRenderEx(psurface,pst->pTexture,(px_int)pObject->x,(px_int)pObject->y,PX_TEXTURERENDER_REFPOINT_CENTER,PX_NULL,1.0f,pst->rotation);

	PX_TextureRender(psurface,pst->centerTexture,(px_int)pObject->x,(px_int)pObject->y,PX_TEXTURERENDER_REFPOINT_CENTER,PX_NULL);

}

PX_Object* Game_Object_TreasureCreateObject(PX_Instance *pIns,PX_World *pWorld,px_float life,px_point position)
{
	PX_Object *pObject;
	px_texture *pTexture,*pCenterTexture;
	Game_Object_Treasure *pst;

	pTexture=PX_ResourceLibraryGetTexture(&pIns->runtime.ResourceLibrary,GAME_TEXT_KEY_TEX_TREASURE0);
	pCenterTexture=PX_ResourceLibraryGetTexture(&pIns->runtime.ResourceLibrary,GAME_TEXT_KEY_TEX_TREASURE1);

	pst=(Game_Object_Treasure *)MP_Malloc(&pIns->runtime.mp_game,sizeof(Game_Object_Treasure));

	if (pst==PX_NULL)
	{
		PX_ASSERT();
		return PX_NULL;
	}

	pObject=(PX_Object *)PX_ObjectCreate(&pIns->runtime.mp_game,PX_NULL,0,0,0,0,0,0);

	if (!pObject)
	{
		MP_Free(&pIns->runtime.mp_game,pst);
		PX_ASSERT();
		return PX_NULL;
	}

	pObject->x=position.x;
	pObject->y=position.y;

	pObject->z=GAME_TREASURE_Z;
	pObject->Width=96;
	pObject->diameter=96;
	pObject->Height=96;
	pObject->pObject=pst;
	pObject->Type=GAME_OBJECT_TYPE_TREASURE;
	pObject->ReceiveEvents=PX_TRUE;
	pObject->Func_ObjectFree=Game_Object_TreasureFree;
	pObject->Func_ObjectUpdate=Game_Object_TreasureUpdate;
	pObject->Func_ObjectRender=Game_Object_TreasureRender;
	pObject->User_ptr=PX_NULL;
	pObject->impact_Object_type=GAME_IMPACT_TYPE_NATURE;
	pObject->impact_test_type=0;

	pst->beAttackElpased=0;
	pst->life=life;
	pst->pTexture=pTexture;
	pst->centerTexture=pCenterTexture;
	pst->elpased=0;
	pst->rotation=0;
	pst->rotationSpeed=GAME_TREASURE_DEFALUE_MAX_ROTATION_SPEED;
	pst->velocity=PX_POINT(0,0,0);
	pst->pWorld=pWorld;
	pst->pIns=pIns;

	PX_ObjectRegisterEvent(pObject,GAME_OBJECT_EVENT_DAMAGE,Game_Object_TreasureOnDamage,PX_NULL);
	PX_ObjectRegisterEvent(pObject,GAME_OBJECT_EVENT_CHANGEVELOCITY,Game_Object_TreasureOnVelocityChange,PX_NULL);
	return pObject;
}

px_void Game_Object_TreasureCreate(PX_Instance *pIns,PX_World *pWorld,px_point position)
{
	PX_Object *pObject;
	pObject=Game_Object_TreasureCreateObject(pIns,pWorld,GAME_TREASURE_DEFALUE_LIFE,position);
	if (pObject)
	{
		PX_WorldAddObject(pWorld,pObject);
	}
}