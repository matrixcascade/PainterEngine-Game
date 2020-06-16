#include "Game_Object_Nuclear.h"

px_void Game_Object_NuclearImpact(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	PX_Object_Event pe;
	PX_Object *pImpactObjects[GAME_OBJECT_NUCLEAR_SEARCH_COUNT];
	px_int count;
	Game_Object_Nuclear *pNuclear=(Game_Object_Nuclear *)pObject->pObject;
	PX_PlayLocalSoundFromResource(pNuclear->pIns,pNuclear->pWorld,pObject->x,pObject->y,GAME_TEXT_KEY_NUCLEAR_EXPLOSIONSOUND,PX_FALSE);

	PX_WorldRemoveObject(pNuclear->pWorld,pObject);
	Game_Object_FireExplosionCreate(pNuclear->pIns,pNuclear->pWorld,PX_POINT(pObject->x,pObject->y,0),100,PX_TRUE);

	if (count=PX_WorldSearchRegion(pNuclear->pWorld,pObject->x,pObject->y,GAME_OBJECT_NUCLEAR_SEARCH,pImpactObjects,GAME_OBJECT_NUCLEAR_SEARCH_COUNT,GAME_IMPACT_TYPE_ALL_OBJECTGROUP|GAME_IMPACT_TYPE_AMMO|GAME_IMPACT_TYPE_SPECIALAMMO|GAME_IMPACT_TYPE_PICKER))
	{
		px_int i;
		pe.Event=GAME_OBJECT_EVENT_DAMAGE;
		pe.Param_float[0]=GAME_OBJECT_NUCLEAR_DAMAGE;
		pe.Param_int[1]=pNuclear->Player;

		for (i=0;i<count;i++)
		{
			PX_ObjectPostEvent(pImpactObjects[i],pe);
		}
	}
	
}

px_void Game_Object_NuclearOnVelocityChange(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	Game_Object_Nuclear *pNuclear=(Game_Object_Nuclear *)pObject->pObject;
	pNuclear->velocity=PX_PointAdd(pNuclear->velocity,PX_POINT(e.Param_float[0],e.Param_float[1],0));
}

px_void Game_Object_NuclearOnDamage(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	PX_Object_Event pe;
	pe.Event=PX_OBJECT_EVENT_ANY;
	Game_Object_NuclearImpact(pObject,pe,PX_NULL);
}



px_void Game_Object_NuclearFree(PX_Object *pObject)
{
	Game_Object_Nuclear *pNuclear=(Game_Object_Nuclear *)pObject->pObject;
	PX_AnimationFree(&pNuclear->animation);
}


px_void Game_Object_NuclearUpdate(PX_Object *pObject,px_dword elpased)
{
	
	Game_Object_Nuclear *pNuclear=(Game_Object_Nuclear *)pObject->pObject;
	PX_World *pWorld=pNuclear->pWorld;

	pNuclear->elpased+=elpased;

	pObject->x+=pNuclear->velocity.x*elpased/1000;
	pObject->y+=pNuclear->velocity.y*elpased/1000;

	if (pNuclear->elpased>=GAME_OBJECT_NUCLEAR_ALIVE_TIME)
	{
		PX_Object_Event e;
		e.Event=PX_OBJECT_EVENT_IMPACT;
		e.Param_ptr[0]=PX_NULL;
		e.Param_ptr[1]=PX_NULL;
		e.Param_ptr[2]=PX_NULL;
		e.Param_ptr[3]=PX_NULL;
		Game_Object_NuclearImpact(pObject,e,PX_NULL);
	}
}


px_void Game_Object_NuclearRender(px_surface *psurface,PX_Object *pObject,px_dword elpased)
{
	px_char content[16]={0};
	px_word wconetnt[32]={0};
	Game_Object_Nuclear *pNuclear=(Game_Object_Nuclear *)pObject->pObject;
	px_dword alive=GAME_OBJECT_NUCLEAR_ALIVE_TIME-pNuclear->elpased;
	PX_AnimationUpdate(&pNuclear->animation,elpased);
	PX_AnimationRender_vector(psurface,&pNuclear->animation,PX_POINT(pObject->x,pObject->y,0),pNuclear->direction,PX_TEXTURERENDER_REFPOINT_CENTER,PX_NULL);

	PX_sprintf1(content,sizeof(content),"%1.2s",PX_STRINGFORMAT_FLOAT((GAME_OBJECT_NUCLEAR_ALIVE_TIME-pNuclear->elpased)/1000.0f));
	PX_wstrcat(wconetnt,(const px_word *)L"µ¹¼ÆÊ±");
	PX_FontModule_wastrcat(wconetnt,content);
	PX_FontModuleDrawText(psurface,(px_int)pObject->x,(px_int)pObject->y-32,wconetnt,PX_COLOR(255,192,0,0),&pNuclear->pIns->FontModule18,PX_FONT_ALIGN_XCENTER);

}



PX_Object * Game_Object_NuclearCreateObject(PX_Instance *pIns,PX_World *pWorld,px_point position,px_point direction,px_dword impactTest,px_int player)
{
	PX_Object *pObject;
	PX_Animationlibrary *pAnimationLibrary;
	Game_Object_Nuclear *pNuclear;

	pAnimationLibrary=PX_ResourceLibraryGetAnimationLibrary(&pIns->runtime.ResourceLibrary,GAME_TEXT_KEY_NUCLEAR_AMMO);

	pNuclear=(Game_Object_Nuclear *)MP_Malloc(&pIns->runtime.mp_game,sizeof(Game_Object_Nuclear));

	if (pNuclear==PX_NULL)
	{
		PX_ASSERT();
		return PX_NULL;
	}

	pObject=(PX_Object *)PX_ObjectCreate(&pIns->runtime.mp_game,PX_NULL,0,0,0,0,0,0);

	if (!pObject)
	{
		MP_Free(&pIns->runtime.mp_game,pNuclear);
		return PX_NULL;
	}

	pObject->x=position.x;
	pObject->y=position.y;
	pObject->z=GAME_AMMO_Z;
	pObject->Width=10;
	pObject->Height=10;
	pObject->diameter=10;


	pObject->pObject=pNuclear;
	pObject->Type=GAME_OBJECT_TYPE_NUCLEAR;
	pObject->ReceiveEvents=PX_TRUE;
	pObject->Func_ObjectFree=Game_Object_NuclearFree;
	pObject->Func_ObjectUpdate=Game_Object_NuclearUpdate;
	pObject->Func_ObjectRender=Game_Object_NuclearRender;
	pObject->User_ptr=PX_NULL;
	pObject->impact_test_type=impactTest;
	pObject->impact_Object_type=GAME_IMPACT_TYPE_SPECIALAMMO;

	pNuclear->pWorld=pWorld;
	pNuclear->pIns=pIns;
	pNuclear->elpased=0;
	PX_AnimationCreate(&pNuclear->animation,pAnimationLibrary);
	PX_AnimationSetCurrentPlayAnimation(&pNuclear->animation,0);
	pNuclear->velocity=PX_PointMul(PX_PointUnit(direction),GAME_OBJECT_NUCLEAR_DEFAULT_SPEED);
	pNuclear->direction=direction;
	pNuclear->Player=player;

	PX_ObjectRegisterEvent(pObject,PX_OBJECT_EVENT_IMPACT,Game_Object_NuclearImpact,PX_NULL);
	PX_ObjectRegisterEvent(pObject,GAME_OBJECT_EVENT_DAMAGE,Game_Object_NuclearOnDamage,PX_NULL);
	PX_ObjectRegisterEvent(pObject,GAME_OBJECT_EVENT_CHANGEVELOCITY,Game_Object_NuclearOnVelocityChange,PX_NULL);

	return pObject;
}

px_void Game_Object_NuclearCreate(PX_Instance *pIns,PX_World *pWorld,px_point position,px_point direction,px_int group,px_int player)
{
	PX_Object *pObject;
	pObject=Game_Object_NuclearCreateObject(pIns,pWorld,position,direction,Game_Object_GroupImpactTestType(group),player);
	if (pObject)
	{
		PX_PlayLocalSoundFromResource(pIns,pWorld,pObject->x,pObject->y,GAME_TEXT_KEY_NUCLEAR_LAUNCHSOUND,PX_FALSE);
		PX_WorldAddObject(pWorld,pObject);
	}
}