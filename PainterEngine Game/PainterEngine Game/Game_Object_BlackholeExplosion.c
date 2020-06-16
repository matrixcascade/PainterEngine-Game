#include "Game_Object_BlackHoleExplosion.h"


px_void Game_Object_BlackHoleExplosionFree(PX_Object *pObject)
{
	
}

px_void Game_Object_BlackHoleExplosionUpdate(PX_Object *pObject,px_dword elpased)
{
	PX_Object *pTargetObjects[32];
	px_int count;
	Game_Object_BlackHoleExplosion *pDesc=(Game_Object_BlackHoleExplosion *)pObject->pObject;
	pDesc->elpased+=elpased;

	if (count=PX_WorldSearchRegion(pDesc->pWorld,pObject->x,pObject->y,GAME_OBJECT_BLACKHOLEEXPLOSION_SEARCH_REGION,pTargetObjects,PX_COUNTOF(pTargetObjects),pObject->impact_test_type))
	{
		px_int i;
		for (i=0;i<count;i++)
		{
			PX_Object_Event e;
			px_point v;
			
			v=PX_POINT(pObject->x-pTargetObjects[i]->x,pObject->y-pTargetObjects[i]->y,0);
			if (GAME_OBJECT_BLACKHOLEEXPLOSION_SEARCH_REGION>PX_PointMod(v))
			{
				v=PX_PointMul(PX_PointUnit(v),GAME_OBJECT_BLACKHOLEEXPLOSION_SEARCH_REGION-PX_PointMod(v));
				v=PX_PointMul(v,PX_PointMod(v)*elpased/1000.0f);
				e.Event=GAME_OBJECT_EVENT_CHANGEVELOCITY;
				e.Param_float[0]=v.x;
				e.Param_float[1]=v.y;
				e.Param_float[2]=v.z;
				PX_ObjectPostEvent(pTargetObjects[i],e);
			}
			
		}
	}
	
	if (count=PX_WorldSearchRegion(pDesc->pWorld,pObject->x,pObject->y,GAME_OBJECT_BLACKHOLEEXPLOSION_SEARCH_DAMAGEREGION,pTargetObjects,PX_COUNTOF(pTargetObjects),pObject->impact_test_type))
	{
		px_int i;
		for (i=0;i<count;i++)
		{
			PX_Object_Event e;
			e.Event=GAME_OBJECT_EVENT_DAMAGE;
			e.Param_float[0]=GAME_OBJECT_BLACKHOLEEXPLOSION_DAMAGE;
			e.Param_int[1]=pDesc->player;
			PX_ObjectPostEvent(pTargetObjects[i],e);
		}
	}


	if (pDesc->elpased>=GAME_OBJECT_BLACKHOLEEXPLOSION_ALIVE_TIME)
	{
		PX_WorldRemoveObject(pDesc->pWorld,pObject);
	}
}

px_void Game_Object_BlackHoleExplosionRender(px_surface *psurface,PX_Object *pObject,px_dword elpased)
{
	px_dword eTime;
	Game_Object_BlackHoleExplosion *pDesc=(Game_Object_BlackHoleExplosion *)pObject->pObject;
	if (pDesc->soundPlayElpased+=elpased)
	{
		if (pDesc->soundPlayElpased>1000)
		{
			pDesc->soundPlayElpased=0;
			PX_PlayLocalSoundFromResource(pDesc->pIns,pDesc->pWorld,pObject->x+pDesc->pWorld->offsetx,pObject->y+pDesc->pWorld->offsety,GAME_TEXT_KEY_BLACKHOLE_EXPLOSIONSOUND,PX_FALSE);
		}
	}

	if (pDesc->elpased<GAME_OBJECT_BLACKHOLEEXPLOSION_STAGE1)
	{
		eTime=pDesc->elpased;
		PX_GeoDrawSolidCircle(psurface,(px_int)pObject->x,(px_int)pObject->y,(px_int)(eTime*1.0f/GAME_OBJECT_BLACKHOLEEXPLOSION_STAGE1*GAME_OBJECT_BLACKHOLEEXPLOSION_SEARCH_DAMAGEREGION),PX_COLOR(255,0,0,0));
	}
	else if (pDesc->elpased<GAME_OBJECT_BLACKHOLEEXPLOSION_STAGE1+GAME_OBJECT_BLACKHOLEEXPLOSION_STAGE2)
	{
		px_int i;
		eTime=pDesc->elpased-GAME_OBJECT_BLACKHOLEEXPLOSION_STAGE1;
		if (pDesc->lastEcho!=(pDesc->elpased/100))
		{
			px_int index;
			px_point pt;
			pDesc->lastEcho=(pDesc->elpased/100);
			index=(pDesc->elpased/100)%10;
			pt.x=0;
			pt.y=(px_float)PX_randRange(GAME_OBJECT_BLACKHOLEEXPLOSION_SEARCH_DAMAGEREGION+100,GAME_OBJECT_BLACKHOLEEXPLOSION_SEARCH_REGION);
			pt.z=0;
			pt=PX_PointRotate(pt,(px_float)PX_randRange(0,360));
			pDesc->echo[index].resx=pt.x;
			pDesc->echo[index].resy=pt.y;
			pDesc->echo[index].alive=1000;
			pDesc->echo[index].resSize=(px_float)PX_randRange(30,50);
		}

		for (i=0;i<PX_COUNTOF(pDesc->echo);i++)
		{
			if (pDesc->echo[i].alive)
			{
				px_float schedule;
				if (pDesc->echo[i].alive>elpased)
				{
					pDesc->echo[i].alive-=elpased;
				}
				else
				{
					pDesc->echo[i].alive=0;
				}

				schedule=pDesc->echo[i].alive*1.0f/1000;

				PX_GeoDrawSolidCircle(psurface,\
					(px_int)(pObject->x+pDesc->echo[i].resx*schedule),\
					(px_int)(pObject->y+pDesc->echo[i].resy*schedule),\
					(px_int)(pDesc->echo[i].resSize*schedule),\
					PX_COLOR((px_uchar)(255*(1-schedule)),0,0,0));
			}
		}

		PX_GeoDrawSolidCircle(psurface,(px_int)(pObject->x),(px_int)(pObject->y),GAME_OBJECT_BLACKHOLEEXPLOSION_SEARCH_DAMAGEREGION+(PX_rand()%6),PX_COLOR(255,0,0,0));
	}
	else if (pDesc->elpased<GAME_OBJECT_BLACKHOLEEXPLOSION_STAGE1+GAME_OBJECT_BLACKHOLEEXPLOSION_STAGE2+GAME_OBJECT_BLACKHOLEEXPLOSION_STAGE3)
	{
		eTime=pDesc->elpased-GAME_OBJECT_BLACKHOLEEXPLOSION_STAGE1-GAME_OBJECT_BLACKHOLEEXPLOSION_STAGE2;
		PX_GeoDrawSolidCircle(psurface,(px_int)(pObject->x),(px_int)(pObject->y),\
			(px_int)((1-eTime*1.0f/GAME_OBJECT_BLACKHOLEEXPLOSION_STAGE3)*GAME_OBJECT_BLACKHOLEEXPLOSION_SEARCH_DAMAGEREGION),\
			PX_COLOR(255,0,0,0));
	}

}


PX_Object * Game_Object_BlackHoleExplosionCreateObject(PX_Instance *pIns,PX_World *pWorld,px_point position,px_int player)
{
	PX_Object *pObject=PX_NULL;
	Game_Object_BlackHoleExplosion *pDesc;

	pDesc=(Game_Object_BlackHoleExplosion *)MP_Malloc(&pIns->runtime.explode_mp,sizeof(Game_Object_BlackHoleExplosion));
	PX_memset(pDesc->echo,0,sizeof(pDesc->echo));
	pDesc->elpased=0;
	pDesc->lastEcho=-1;
	pDesc->pIns=pIns;
	pDesc->pWorld=pWorld;
	pDesc->soundPlayElpased=0;
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

	pObject->x=position.x;
	pObject->y=position.y;
	pObject->z=GAME_BLOCKHOLE_Z;
	pObject->Width=1;
	pObject->Height=1;


	pObject->pObject=pDesc;
	pObject->Type=GAME_OBJECT_TYPE_BLACKHOLE_REGION;
	pObject->ReceiveEvents=PX_FALSE;
	pObject->Func_ObjectFree=Game_Object_BlackHoleExplosionFree;
	pObject->Func_ObjectUpdate=Game_Object_BlackHoleExplosionUpdate;
	pObject->Func_ObjectRender=Game_Object_BlackHoleExplosionRender;
	pObject->User_ptr=PX_NULL;
	pObject->impact_test_type=GAME_IMPACT_TYPE_ALL_OBJECTGROUP|GAME_IMPACT_TYPE_AMMO|GAME_IMPACT_TYPE_SPECIALAMMO|GAME_IMPACT_TYPE_PICKER;
	pObject->impact_Object_type=GAME_IMPACT_TYPE_NONE;
	
	
	return pObject;
}




px_void Game_Object_BlackHoleExplosionCreate(PX_Instance *pIns,PX_World *pWorld,px_point position,px_int player)
{
	PX_Object *pObject;

	pObject=Game_Object_BlackHoleExplosionCreateObject(pIns,pWorld,position,player);

	if(!PX_WorldAddObject(pWorld,pObject))
	{
		PX_ASSERT();
	}
}

