#include "Game_Object_ProtectShield.h"


px_void Game_Object_ProtectShieldFree(PX_Object *pObject)
{

}

px_void Game_Object_ProtectShieldUpdate(PX_Object *pObject,px_dword elpased)
{
	Game_Object_ProtectShield *pDesc=(Game_Object_ProtectShield *)pObject->pObject;

	pObject->x=pDesc->attachShipObject->x;
	pObject->y=pDesc->attachShipObject->y;
	pDesc->elpased+=elpased;

	if (pDesc->elpased>=GAME_OBJECT_PROTECTSHIELD_ALIVE_TIME)
	{
		PX_WorldRemoveObject(pDesc->pWorld,pObject);
	}

}

px_void Game_Object_ProtectShieldRender(px_surface *psurface,PX_Object *pObject,px_dword elpased)
{
	Game_Object_ProtectShield *pDesc=(Game_Object_ProtectShield *)pObject->pObject;
	pDesc->angle+=elpased/1000.0f*360;

	if (pDesc->elpased<=500)
	{
		px_float scale=pDesc->elpased/500.0f;
		PX_GeoDrawRing(psurface,(px_int)pObject->x,(px_int)pObject->y,(px_int)(pObject->diameter/2*scale),5,PX_COLOR(255,192,0,0),(px_uint)pDesc->angle,(px_uint)pDesc->angle+60);
		PX_GeoDrawRing(psurface,(px_int)pObject->x,(px_int)pObject->y,(px_int)(pObject->diameter/2*scale),5,PX_COLOR(255,192,0,0),(px_uint)pDesc->angle+120,(px_uint)pDesc->angle+180);
		PX_GeoDrawRing(psurface,(px_int)pObject->x,(px_int)pObject->y,(px_int)(pObject->diameter/2*scale),5,PX_COLOR(255,192,0,0),(px_uint)pDesc->angle+240,(px_uint)pDesc->angle+300);
	}
	else if(GAME_OBJECT_PROTECTSHIELD_ALIVE_TIME-pDesc->elpased<1000)
	{
		px_float scale=(GAME_OBJECT_PROTECTSHIELD_ALIVE_TIME-pDesc->elpased)/1000.0f;
		PX_GeoDrawRing(psurface,(px_int)pObject->x,(px_int)pObject->y,(px_int)(pObject->diameter/2),5,PX_COLOR((px_uchar)(scale*255),192,0,0),(px_uint)pDesc->angle,(px_uint)pDesc->angle+60);
		PX_GeoDrawRing(psurface,(px_int)pObject->x,(px_int)pObject->y,(px_int)(pObject->diameter/2),5,PX_COLOR((px_uchar)(scale*255),192,0,0),(px_uint)pDesc->angle+120,(px_uint)pDesc->angle+180);
		PX_GeoDrawRing(psurface,(px_int)pObject->x,(px_int)pObject->y,(px_int)(pObject->diameter/2),5,PX_COLOR((px_uchar)(scale*255),192,0,0),(px_uint)pDesc->angle+240,(px_uint)pDesc->angle+300);
	}
	else
	{
		PX_GeoDrawRing(psurface,(px_int)pObject->x,(px_int)pObject->y,(px_int)(pObject->diameter/2),5,PX_COLOR(255,192,0,0),(px_uint)pDesc->angle,(px_uint)pDesc->angle+60);
		PX_GeoDrawRing(psurface,(px_int)pObject->x,(px_int)pObject->y,(px_int)(pObject->diameter/2),5,PX_COLOR(255,192,0,0),(px_uint)pDesc->angle+120,(px_uint)pDesc->angle+180);
		PX_GeoDrawRing(psurface,(px_int)pObject->x,(px_int)pObject->y,(px_int)(pObject->diameter/2),5,PX_COLOR(255,192,0,0),(px_uint)pDesc->angle+240,(px_uint)pDesc->angle+300);
	}
	


}

px_void Game_Object_ProtectShieldImpact(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	PX_Object_Event pe;
	Game_Object_ProtectShield *pDesc=(Game_Object_ProtectShield *)pObject->pObject;
	PX_Object *pTargetObject=(PX_Object *)e.Param_ptr[0];
	if (pTargetObject->Type==GAME_OBJECT_TYPE_STONE)
	{
		pe.Event=GAME_OBJECT_EVENT_DAMAGE;
		pe.Param_float[0]=1000;
		pe.Param_int[1]=pDesc->player;
		PX_ObjectPostEvent(pTargetObject,pe);
	}
	
}

PX_Object * Game_Object_ProtectShieldCreateObject(PX_Instance *pIns,PX_World *pWorld,PX_Object *pShipObject,px_int player)
{
	PX_Object *pObject=PX_NULL;
	Game_Object_ProtectShield *pDesc;

	pDesc=(Game_Object_ProtectShield *)MP_Malloc(&pIns->runtime.explode_mp,sizeof(Game_Object_ProtectShield));
	pDesc->angle=0;
	pDesc->attachShipObject=pShipObject;
	pDesc->elpased=0;
	pDesc->pIns=pIns;
	pDesc->pWorld=pWorld;
	pDesc->player=player;
	if (pDesc==PX_NULL)
	{
		return PX_NULL;
	}


	pObject=(PX_Object *)PX_ObjectCreate(&pIns->runtime.explode_mp,PX_NULL,0,0,0,0,0,0);

	if (!pObject)
	{
		MP_Free(&pIns->runtime.explode_mp,pDesc);
		return PX_NULL;
	}

	pObject->x=pShipObject->x;
	pObject->y=pShipObject->y;
	pObject->z=GAME_SHIP_Z;
	pObject->Width=GAME_OBJECT_PROTECTSHIELD_DIAMETER;
	pObject->Height=GAME_OBJECT_PROTECTSHIELD_DIAMETER;
	pObject->diameter=GAME_OBJECT_PROTECTSHIELD_DIAMETER;


	pObject->pObject=pDesc;
	pObject->Type=GAME_OBJECT_TYPE_SHIP;
	pObject->ReceiveEvents=PX_TRUE;
	pObject->Func_ObjectFree=Game_Object_ProtectShieldFree;
	pObject->Func_ObjectUpdate=Game_Object_ProtectShieldUpdate;
	pObject->Func_ObjectRender=Game_Object_ProtectShieldRender;
	pObject->User_ptr=PX_NULL;
	
	pObject->impact_Object_type=pShipObject->impact_Object_type;
	pObject->impact_test_type=GAME_IMPACT_TYPE_NATURE;
	
	PX_ObjectRegisterEvent(pObject,PX_OBJECT_EVENT_IMPACT,Game_Object_ProtectShieldImpact,PX_NULL);

	return pObject;
}




px_void Game_Object_ProtectShieldCreate(PX_Instance *pIns,PX_World *pWorld,PX_Object *pShipObject,px_int player)
{
	PX_Object *pObject;

	pObject=Game_Object_ProtectShieldCreateObject(pIns,pWorld,pShipObject,player);

	if(!PX_WorldAddObject(pWorld,pObject))
	{
		PX_ASSERT();
	}

	PX_PlayLocalSoundFromResource(pIns,pWorld,pShipObject->x,pShipObject->y,GAME_TEXT_KEY_PROTECTSHIELDSOUND,PX_FALSE);
}

