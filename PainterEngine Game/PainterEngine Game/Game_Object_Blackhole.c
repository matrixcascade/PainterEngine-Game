#include "Game_Object_Blackhole.h"


px_void Game_Object_BlackHoleFree(PX_Object *pObject)
{
	Game_Object_BlackHole *pBlackHole=(Game_Object_BlackHole *)pObject->pObject;
	PX_AnimationFree(&pBlackHole->animation);
}


px_void Game_Object_BlackHoleUpdate(PX_Object *pObject,px_dword elpased)
{
	
	Game_Object_BlackHole *pBlackHole=(Game_Object_BlackHole *)pObject->pObject;
	PX_World *pWorld=pBlackHole->pWorld;

	pBlackHole->elpased+=elpased;

	pObject->x+=pBlackHole->velocity.x*elpased/1000;
	pObject->y+=pBlackHole->velocity.y*elpased/1000;

	if (pBlackHole->elpased>=GAME_OBJECT_BLACKHOLE_ALIVE_TIME)
	{
		PX_Object_Event e;
		e.Event=PX_OBJECT_EVENT_IMPACT;
		e.Param_ptr[0]=PX_NULL;
		e.Param_ptr[1]=PX_NULL;
		e.Param_ptr[2]=PX_NULL;
		e.Param_ptr[3]=PX_NULL;
		PX_WorldRemoveObject(pWorld,pObject);
		Game_Object_BlackHoleExplosionCreate(pBlackHole->pIns,pBlackHole->pWorld,PX_POINT(pObject->x,pObject->y,0),pBlackHole->Player);
	}
}


px_void Game_Object_BlackHoleRender(px_surface *psurface,PX_Object *pObject,px_dword elpased)
{
	px_char content[16]={0};
	px_word wconetnt[32]={0};
	px_float angle;
	Game_Object_BlackHole *pBlackHole=(Game_Object_BlackHole *)pObject->pObject;
	px_dword alive=GAME_OBJECT_BLACKHOLE_ALIVE_TIME-pBlackHole->elpased;
	pBlackHole->angle+=elpased/2;
	if (pBlackHole->angle>360)
	{
		pBlackHole->angle=0;
	}
	
	angle=pBlackHole->angle;
	PX_AnimationUpdate(&pBlackHole->animation,elpased);
	PX_AnimationRender(psurface,&pBlackHole->animation,PX_POINT(pObject->x,pObject->y,0),PX_TEXTURERENDER_REFPOINT_CENTER,PX_NULL);
	


	PX_GeoDrawRing(psurface,(px_int)pObject->x,(px_int)pObject->y,36,2,PX_COLOR(255,0,0,0),(px_uint)angle,(px_uint)angle+60);
	angle+=120;

	PX_GeoDrawRing(psurface,(px_int)pObject->x,(px_int)pObject->y,36,2,PX_COLOR(255,0,0,0),(px_uint)angle,(px_uint)angle+60);
	angle+=120;

	PX_GeoDrawRing(psurface,(px_int)pObject->x,(px_int)pObject->y,36,2,PX_COLOR(255,0,0,0),(px_uint)angle,(px_uint)angle+60);


	PX_sprintf1(content,sizeof(content),"%1.2s",PX_STRINGFORMAT_FLOAT((GAME_OBJECT_BLACKHOLE_ALIVE_TIME-pBlackHole->elpased)/1000.0f));
	PX_wstrcat(wconetnt,(const px_word *)L"µ¹¼ÆÊ±");
	PX_FontModule_wastrcat(wconetnt,content);
	PX_FontModuleDrawText(psurface,(px_int)pObject->x,(px_int)pObject->y-36,wconetnt,PX_COLOR(255,192,0,0),&pBlackHole->pIns->FontModule18,PX_FONT_ALIGN_XCENTER);

}



PX_Object * Game_Object_BlackHoleCreateObject(PX_Instance *pIns,PX_World *pWorld,px_point position,px_point direction,px_dword impactTest,px_int player)
{
	PX_Object *pObject;
	PX_Animationlibrary *pAnimationLibrary;
	Game_Object_BlackHole *pBlackHole;

	pAnimationLibrary=PX_ResourceLibraryGetAnimationLibrary(&pIns->runtime.ResourceLibrary,GAME_TEXT_KEY_BLACKHOLE_AMMO);

	pBlackHole=(Game_Object_BlackHole *)MP_Malloc(&pIns->runtime.mp_game,sizeof(Game_Object_BlackHole));

	if (pBlackHole==PX_NULL)
	{
		PX_ASSERT();
		return PX_NULL;
	}

	pObject=(PX_Object *)PX_ObjectCreate(&pIns->runtime.mp_game,PX_NULL,0,0,0,0,0,0);

	if (!pObject)
	{
		MP_Free(&pIns->runtime.mp_game,pBlackHole);
		return PX_NULL;
	}

	pObject->x=position.x;
	pObject->y=position.y;
	pObject->z=GAME_AMMO_Z;
	pObject->Width=10;
	pObject->Height=10;
	pObject->diameter=10;


	pObject->pObject=pBlackHole;
	pObject->Type=GAME_OBJECT_TYPE_BLACKHOLE;
	pObject->ReceiveEvents=PX_TRUE;
	pObject->Func_ObjectFree=Game_Object_BlackHoleFree;
	pObject->Func_ObjectUpdate=Game_Object_BlackHoleUpdate;
	pObject->Func_ObjectRender=Game_Object_BlackHoleRender;
	pObject->User_ptr=PX_NULL;
	pObject->impact_test_type=impactTest;
	pObject->impact_Object_type=GAME_IMPACT_TYPE_NONE;

	pBlackHole->pWorld=pWorld;
	pBlackHole->pIns=pIns;
	pBlackHole->elpased=0;
	PX_AnimationCreate(&pBlackHole->animation,pAnimationLibrary);
	PX_AnimationSetCurrentPlayAnimation(&pBlackHole->animation,0);
	pBlackHole->velocity=PX_PointMul(PX_PointUnit(direction),GAME_OBJECT_BLACKHOLE_DEFAULT_SPEED);
	pBlackHole->direction=direction;
	pBlackHole->Player=player;
	pBlackHole->angle=0;
	return pObject;
}

px_void Game_Object_BlackHoleCreate(PX_Instance *pIns,PX_World *pWorld,px_point position,px_point direction,px_int group,px_int player)
{
	PX_Object *pObject;
	pObject=Game_Object_BlackHoleCreateObject(pIns,pWorld,position,direction,0,player);
	if (pObject)
	{
		PX_WorldAddObject(pWorld,pObject);
	}
}