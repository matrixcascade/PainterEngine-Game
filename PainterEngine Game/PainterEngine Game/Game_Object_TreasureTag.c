#include "Game_Object_TreasureTag.h"

Game_Object_TreasureTag *Game_Object_GetTreasureTag(PX_Object *pObject)
{
	return (Game_Object_TreasureTag *)pObject->pObject;
}


px_void Game_Object_TreasureTagOnVelocityChange(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	Game_Object_TreasureTag *pTreasureTag=(Game_Object_TreasureTag *)pObject->pObject;
	pTreasureTag->velocity=PX_PointAdd(pTreasureTag->velocity,PX_POINT(e.Param_float[0],e.Param_float[1],0));
}


px_void Game_Object_TreasureTagFree(PX_Object *pObject)
{

}

px_void Game_Object_TreasureTagUpdate(PX_Object *pObject,px_dword elpased)
{
	Game_Object_TreasureTag *pst=Game_Object_GetTreasureTag(pObject);

	pst->elpased+=elpased;

	pst->rotation+=pst->rotationSpeed*elpased/1000;
	pObject->x+=pst->velocity.x*elpased/1000;
	pObject->y+=pst->velocity.y*elpased/1000;

	if (pst->elpased>GAME_TREASURETAG_DEFALUE_ALIVE_TIME)
	{
		PX_WorldRemoveObject(pst->pWorld,pObject);
		Game_Object_TreasureCreate(pst->pIns,pst->pWorld,PX_POINT(pObject->x,pObject->y,0));
	}

}


px_void Game_Object_TreasureTagRender(px_surface *psurface,PX_Object *pObject,px_dword elpased)
{
	Game_Object_TreasureTag *pst=Game_Object_GetTreasureTag(pObject);
	px_char content[16]={0};
	px_word wconetnt[32]={0};
	px_dword alive=GAME_TREASURETAG_DEFALUE_ALIVE_TIME-pst->elpased;

	if (GAME_TREASURETAG_DEFALUE_ALIVE_TIME-pst->elpased<500)
	{
		if (GAME_TREASURETAG_DEFALUE_ALIVE_TIME>pst->elpased)
		{
			px_float scale=	(GAME_TREASURETAG_DEFALUE_ALIVE_TIME-pst->elpased)/500.f;
			PX_ShapeRenderEx(psurface,pst->pShape,(px_int)pObject->x,(px_int)pObject->y,PX_TEXTURERENDER_REFPOINT_CENTER,PX_COLOR(128,192,0,0),scale,pst->rotation);
		}
		
	}
	else
		PX_ShapeRenderEx(psurface,pst->pShape,(px_int)pObject->x,(px_int)pObject->y,PX_TEXTURERENDER_REFPOINT_CENTER,PX_COLOR(128,192,0,0),1.0f,pst->rotation);


	PX_sprintf1(content,sizeof(content),"%1.2s",PX_STRINGFORMAT_FLOAT((GAME_TREASURETAG_DEFALUE_ALIVE_TIME-pst->elpased)/1000.0f));
	PX_wstrcat(wconetnt,(const px_word *)L"¼´½«¿ªÆô:");
	PX_FontModule_wastrcat(wconetnt,content);
	PX_FontModuleDrawText(psurface,(px_int)pObject->x,(px_int)pObject->y-32,wconetnt,PX_COLOR(255,192,0,0),&pst->pIns->FontModule18,PX_FONT_ALIGN_XCENTER);

}

PX_Object* Game_Object_TreasureTagCreateObject(PX_Instance *pIns,PX_World *pWorld,px_point position)
{
	PX_Object *pObject;
	px_shape *pShape;
	Game_Object_TreasureTag *pst;
	

	pShape=PX_ResourceLibraryGetShape(&pIns->runtime.ResourceLibrary,GAME_TEXT_KEY_TREASURETAG);

	pst=(Game_Object_TreasureTag *)MP_Malloc(&pIns->runtime.mp_game,sizeof(Game_Object_TreasureTag));

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
	pObject->Type=GAME_OBJECT_TYPE_TREASURETAG;
	pObject->ReceiveEvents=PX_TRUE;
	pObject->Func_ObjectFree=Game_Object_TreasureTagFree;
	pObject->Func_ObjectUpdate=Game_Object_TreasureTagUpdate;
	pObject->Func_ObjectRender=Game_Object_TreasureTagRender;
	pObject->User_ptr=PX_NULL;
	pObject->impact_Object_type=0;
	pObject->impact_test_type=0;

	pst->pShape=pShape;
	pst->elpased=0;
	pst->rotation=0;
	pst->rotationSpeed=GAME_TREASURETAG_DEFALUE_MAX_ROTATION_SPEED;
	pst->velocity=PX_POINT(0,0,0);
	pst->pWorld=pWorld;
	pst->pIns=pIns;

	//PX_ObjectRegisterEvent(pObject,GAME_OBJECT_EVENT_CHANGEVELOCITY,Game_Object_TreasureTagOnVelocityChange,PX_NULL);
	return pObject;
}

px_void Game_Object_TreasureTagCreate(PX_Instance *pIns,PX_World *pWorld,px_point position)
{
	PX_Object *pObject;
	pObject=Game_Object_TreasureTagCreateObject(pIns,pWorld,position);
	if (pObject)
	{
		PX_WorldAddObject(pWorld,pObject);
	}
}