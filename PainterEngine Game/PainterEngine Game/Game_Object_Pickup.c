#include "Game_Object_Pickup.h"

px_void Game_Object_PickupImpact(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	PX_Object_Event pe;
	Game_Object_Pickup *pPickup=(Game_Object_Pickup *)pObject->pObject;
	PX_WorldRemoveObject(pPickup->pWorld,pObject);
	Game_Object_PickupExplosionCreate(pPickup->pIns,pPickup->pWorld,PX_POINT(pObject->x,pObject->y,0),pPickup->type);

	switch(pPickup->type)
	{
	case GAME_OBJECT_PICKETUP_TYPE_BLACKHOLE:
		{
			pe.Event=GAME_OBJECT_EVENT_ADDWEAPON;
			pe.Param_int[0]=GAME_OBJECT_SHIP_WEAPON_TYPE_BLACKHOLE;
			if (pPickup->quality==GAME_OBJECT_PICKUP_QUILITY_NORMAL)
			{
				pe.Param_int[1]=1;
				pe.Param_int[2]=0;
				pe.Param_int[3]=0;
			}
			else
			{

				pe.Param_int[1]=3;
				pe.Param_int[2]=0;
				pe.Param_int[3]=0;
			}
			
		}
		break;
	case GAME_OBJECT_PICKETUP_TYPE_HEALTH:
		{
			pe.Event=GAME_OBJECT_EVENT_HEALTH;
			if (pPickup->quality==GAME_OBJECT_PICKUP_QUILITY_NORMAL)
			{
				pe.Param_float[0]=10;
				pe.Param_float[1]=10;
				pe.Param_float[2]=10;
				pe.Param_float[3]=10;
			}
			else
			{
				pe.Param_float[0]=80;
				pe.Param_float[1]=80;
				pe.Param_float[2]=80;
				pe.Param_float[3]=80;
			}
			
		}
		break;
	case GAME_OBJECT_PICKETUP_TYPE_MACHINEGUN:
		{
			pe.Event=GAME_OBJECT_EVENT_ADDWEAPON;
			pe.Param_int[0]=GAME_OBJECT_SHIP_WEAPON_TYPE_MACHINEGUN;
			if (pPickup->quality==GAME_OBJECT_PICKUP_QUILITY_NORMAL)
			{
				pe.Param_int[1]=6;
				pe.Param_int[2]=0;
				pe.Param_int[3]=0;
			}
			else
			{
				pe.Param_int[1]=30;
				pe.Param_int[2]=0;
				pe.Param_int[3]=0;
			}
		}
		break;
	case GAME_OBJECT_PICKETUP_TYPE_MINE:
		{
			pe.Event=GAME_OBJECT_EVENT_ADDWEAPON;
			pe.Param_int[0]=GAME_OBJECT_SHIP_WEAPON_TYPE_MINE;
			if (pPickup->quality==GAME_OBJECT_PICKUP_QUILITY_NORMAL)
			{
				pe.Param_int[1]=2;
				pe.Param_int[2]=0;
				pe.Param_int[3]=0;
			}
			else
			{
				pe.Param_int[1]=10;
				pe.Param_int[2]=0;
				pe.Param_int[3]=0;
			}
			
		}
		break;
	case GAME_OBJECT_PICKETUP_TYPE_MISSILE:
		{
			pe.Event=GAME_OBJECT_EVENT_ADDWEAPON;
			pe.Param_int[0]=GAME_OBJECT_SHIP_WEAPON_TYPE_MISSILE;
			if (pPickup->quality==GAME_OBJECT_PICKUP_QUILITY_NORMAL)
			{
				pe.Param_int[1]=6;
				pe.Param_int[2]=0;
				pe.Param_int[3]=0;
			}
			else
			{

				pe.Param_int[1]=36;
				pe.Param_int[2]=0;
				pe.Param_int[3]=0;
			}
			
		}
		break;
	case GAME_OBJECT_PICKETUP_TYPE_NUCLEAR:
		{
			pe.Event=GAME_OBJECT_EVENT_ADDWEAPON;
			pe.Param_int[0]=GAME_OBJECT_SHIP_WEAPON_TYPE_NUCLEAR;
			if (pPickup->quality==GAME_OBJECT_PICKUP_QUILITY_NORMAL)
			{
				pe.Param_int[1]=1;
				pe.Param_int[2]=0;
				pe.Param_int[3]=0;
			}
			else
			{
				pe.Param_int[1]=5;
				pe.Param_int[2]=0;
				pe.Param_int[3]=0;
			}
		}
		break;
	case GAME_OBJECT_PICKETUP_TYPE_RAILGUN:
		{
			pe.Event=GAME_OBJECT_EVENT_ADDWEAPON;
			pe.Param_int[0]=GAME_OBJECT_SHIP_WEAPON_TYPE_RAILGUN;
			if (pPickup->quality==GAME_OBJECT_PICKUP_QUILITY_NORMAL)
			{
				pe.Param_int[1]=6;
				pe.Param_int[2]=0;
				pe.Param_int[3]=0;
			}
			else
			{
				pe.Param_int[1]=35;
				pe.Param_int[2]=0;
				pe.Param_int[3]=0;
			}
		}
		break;
	case GAME_OBJECT_PICKETUP_TYPE_PIERCER:
		{
			pe.Event=GAME_OBJECT_EVENT_ADDWEAPON;
			pe.Param_int[0]=GAME_OBJECT_SHIP_WEAPON_TYPE_PIERCER;
			if (pPickup->quality==GAME_OBJECT_PICKUP_QUILITY_NORMAL)
			{
				pe.Param_int[1]=12;
				pe.Param_int[2]=0;
				pe.Param_int[3]=0;
			}
			else
			{
				pe.Param_int[1]=50;
				pe.Param_int[2]=0;
				pe.Param_int[3]=0;
			}
		}
		break;
	case GAME_OBJECT_PICKETUP_TYPE_PROTECTSHIELD:
		{
			pe.Event=GAME_OBJECT_EVENT_ADDWEAPON;
			pe.Param_int[0]=GAME_OBJECT_SHIP_WEAPON_TYPE_PROTECTSHIELD;
			pe.Param_int[1]=1;
			pe.Param_int[2]=0;
			pe.Param_int[3]=0;
		}
		break;
	case GAME_OBJECT_PICKETUP_TYPE_SHIELD:
		{
			pe.Event=GAME_OBJECT_EVENT_ADDSHIELD;
			if (pPickup->quality==GAME_OBJECT_PICKUP_QUILITY_NORMAL)
			{
				pe.Param_float[0]=10;
				pe.Param_float[1]=10;
				pe.Param_float[2]=0;
				pe.Param_float[3]=0;
			}
			else
			{
				pe.Param_float[0]=50;
				pe.Param_float[1]=50;
				pe.Param_float[2]=0;
				pe.Param_float[3]=0;
			}
		}
		break;
	case GAME_OBJECT_PICKETUP_TYPE_SPEEDUP:
		{
			pe.Event=GAME_OBJECT_EVENT_SPEEDRUP;
			if (pPickup->quality==GAME_OBJECT_PICKUP_QUILITY_NORMAL)
			{
				pe.Param_float[0]=5;
				pe.Param_float[1]=0;
				pe.Param_float[2]=0;
				pe.Param_float[3]=0;
			}
			else
			{
				pe.Param_float[0]=20;
				pe.Param_float[1]=0;
				pe.Param_float[2]=0;
				pe.Param_float[3]=0;
			}
		}
		break;
	case GAME_OBJECT_PICKETUP_TYPE_POWERUP:
		{
			pe.Event=GAME_OBJECT_EVENT_POWERUP;
			if (pPickup->quality==GAME_OBJECT_PICKUP_QUILITY_NORMAL)
			{
				pe.Param_float[0]=10;
				pe.Param_float[1]=0;
				pe.Param_float[2]=0;
				pe.Param_float[3]=0;
			}
			else
			{
				pe.Param_float[0]=50;
				pe.Param_float[1]=0;
				pe.Param_float[2]=0;
				pe.Param_float[3]=0;
			}
		}
		break;
	}
	PX_ObjectPostEvent((PX_Object *)e.Param_ptr[0],pe);
}

px_void Game_Object_PickupOnVelocityChange(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	Game_Object_Pickup *pPickup=(Game_Object_Pickup *)pObject->pObject;
	pPickup->velocity=PX_PointAdd(pPickup->velocity,PX_POINT(e.Param_float[0],e.Param_float[1],0));
}

px_void Game_Object_PickupOnDamage(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	Game_Object_Pickup *pPickup=(Game_Object_Pickup *)pObject->pObject;
	PX_WorldRemoveObject(pPickup->pWorld,pObject);
	Game_Object_PickupExplosionCreate(pPickup->pIns,pPickup->pWorld,PX_POINT(pObject->x,pObject->y,0),pPickup->type);
}


px_void Game_Object_PickupFree(PX_Object *pObject)
{
}


px_void Game_Object_PickupUpdate(PX_Object *pObject,px_dword elpased)
{

	Game_Object_Pickup *pPickup=(Game_Object_Pickup *)pObject->pObject;
	PX_World *pWorld=pPickup->pWorld;

	pPickup->elpased+=elpased;

	pObject->x+=pPickup->velocity.x*elpased/1000;
	pObject->y+=pPickup->velocity.y*elpased/1000;


	if (pPickup->elpased>=GAME_OBJECT_MISSILE_PICKUP_TIME)
	{
		PX_WorldRemoveObject(pWorld,pObject);
	}

	if (pObject->x<0||pObject->x>pPickup->pWorld->world_width||\
		pObject->y<0||pObject->y>pPickup->pWorld->world_height\
		)
	{
		PX_WorldRemoveObject(pPickup->pWorld,pObject);
	}
}

px_void Game_Object_PickupRender(px_surface *psurface,PX_Object *pObject,px_dword elpased)
{
	px_color clr;
	Game_Object_Pickup *pPickup=(Game_Object_Pickup *)pObject->pObject;
	px_float Interpolation=pPickup->elpased%2000>=1000?(1-(pPickup->elpased%1000)/1000.0f):((pPickup->elpased%1000)/1000.0f);

	if (pPickup->elpased>GAME_OBJECT_MISSILE_PICKUP_TIME-5000)
	{
		
		if ((pPickup->elpased/300)&1)
		{
			if (pPickup->quality==GAME_OBJECT_PICKUP_QUILITY_NORMAL)
			{
				clr._argb.a=255;
				clr._argb.r=96;
				clr._argb.g=128;
				clr._argb.b=96;
				PX_ShapeRender(psurface,pPickup->logo,(px_int)pObject->x,(px_int)pObject->y,PX_TEXTURERENDER_REFPOINT_CENTER,clr);
			}
			else
			{
				clr._argb.a=255;
				clr._argb.r=(px_uchar)((1-Interpolation)*128)+128;
				clr._argb.g=255-clr._argb.r;
				clr._argb.b=0;
				PX_ShapeRender(psurface,pPickup->logo,(px_int)pObject->x,(px_int)pObject->y,PX_TEXTURERENDER_REFPOINT_CENTER,clr);
			}
			
		}
		else
		{
			if (pPickup->quality==GAME_OBJECT_PICKUP_QUILITY_NORMAL)
			{
				clr._argb.a=128;
				clr._argb.r=96;
				clr._argb.g=128;
				clr._argb.b=96;
				PX_ShapeRender(psurface,pPickup->logo,(px_int)pObject->x,(px_int)pObject->y,PX_TEXTURERENDER_REFPOINT_CENTER,clr);
			}
			else
			{
				clr._argb.a=128;
				clr._argb.r=(px_uchar)((1-Interpolation)*128)+128;
				clr._argb.g=255-clr._argb.r;
				clr._argb.b=0;
				PX_ShapeRender(psurface,pPickup->logo,(px_int)pObject->x,(px_int)pObject->y,PX_TEXTURERENDER_REFPOINT_CENTER,clr);
			}
			
		}
	}
	else
	{
		if (pPickup->quality==GAME_OBJECT_PICKUP_QUILITY_NORMAL)
		{
			clr._argb.a=255;
			clr._argb.r=96;
			clr._argb.g=128;
			clr._argb.b=96;
			PX_ShapeRender(psurface,pPickup->logo,(px_int)pObject->x,(px_int)pObject->y,PX_TEXTURERENDER_REFPOINT_CENTER,clr);
		}
		else
		{
			clr._argb.a=255;
			clr._argb.r=(px_uchar)((1-Interpolation)*128)+128;
			clr._argb.g=255-clr._argb.r;
			clr._argb.b=0;
			PX_ShapeRender(psurface,pPickup->logo,(px_int)pObject->x,(px_int)pObject->y,PX_TEXTURERENDER_REFPOINT_CENTER,clr);
		}
		
	}
}



PX_Object * Game_Object_PickupCreateObject(PX_Instance *pIns,PX_World *pWorld,px_point position,GAME_OBJECT_PICKETUP_TYPE type,px_dword impactTest,GAME_OBJECT_PICKUP_QUALITY quality)
{
	PX_Object *pObject;
	Game_Object_Pickup *pPickup;

	pPickup=(Game_Object_Pickup *)MP_Malloc(&pIns->runtime.mp_game,sizeof(Game_Object_Pickup));

	if (pPickup==PX_NULL)
	{
		PX_ASSERT();
		return PX_NULL;
	}

	pObject=(PX_Object *)PX_ObjectCreate(&pIns->runtime.mp_game,PX_NULL,0,0,0,0,0,0);

	if (!pObject)
	{
		MP_Free(&pIns->runtime.mp_game,pPickup);
		return PX_NULL;
	}

	pObject->x=position.x;
	pObject->y=position.y;
	pObject->z=GAME_PICKUP_Z;
	pObject->Width=32;
	pObject->Height=32;
	pObject->diameter=32;


	pObject->pObject=pPickup;
	pObject->Type=GAME_OBJECT_TYPE_PICKUP;
	pObject->ReceiveEvents=PX_TRUE;
	pObject->Func_ObjectFree=Game_Object_PickupFree;
	pObject->Func_ObjectUpdate=Game_Object_PickupUpdate;
	pObject->Func_ObjectRender=Game_Object_PickupRender;
	pObject->User_ptr=PX_NULL;
	pObject->impact_test_type=impactTest;
	pObject->impact_Object_type=GAME_IMPACT_TYPE_PICKER;

	pPickup->pWorld=pWorld;
	pPickup->pIns=pIns;
	pPickup->elpased=0;
	pPickup->velocity=PX_POINT(Game_rand()%10-20.0f,Game_rand()%10-20.0f,0);
	pPickup->type=type;
	pPickup->quality=quality;
	switch (pPickup->type)
	{
	case GAME_OBJECT_PICKETUP_TYPE_BLACKHOLE:
		pPickup->logo=PX_ResourceLibraryGetShape(&pPickup->pIns->runtime.ResourceLibrary,GAME_TEXT_KEY_BLACKHOLE_LOGO);
		break;
	case GAME_OBJECT_PICKETUP_TYPE_MINE:
		pPickup->logo=PX_ResourceLibraryGetShape(&pPickup->pIns->runtime.ResourceLibrary,GAME_TEXT_KEY_MINE_LOGO);
		break;
	case GAME_OBJECT_PICKETUP_TYPE_MISSILE:
		pPickup->logo=PX_ResourceLibraryGetShape(&pPickup->pIns->runtime.ResourceLibrary,GAME_TEXT_KEY_MISSILE_LOGO);
		break;
	case GAME_OBJECT_PICKETUP_TYPE_NUCLEAR:
		pPickup->logo=PX_ResourceLibraryGetShape(&pPickup->pIns->runtime.ResourceLibrary,GAME_TEXT_KEY_NUCLEAR_LOGO);
		break;
	case GAME_OBJECT_PICKETUP_TYPE_RAILGUN:
		pPickup->logo=PX_ResourceLibraryGetShape(&pPickup->pIns->runtime.ResourceLibrary,GAME_TEXT_KEY_RAILGUN_LOGO);
		break;
	case GAME_OBJECT_PICKETUP_TYPE_MACHINEGUN:
		pPickup->logo=PX_ResourceLibraryGetShape(&pPickup->pIns->runtime.ResourceLibrary,GAME_TEXT_KEY_MACHINEGUN_LOGO);
		break;
	case GAME_OBJECT_PICKETUP_TYPE_HEALTH:
		pPickup->logo=PX_ResourceLibraryGetShape(&pPickup->pIns->runtime.ResourceLibrary,GAME_TEXT_KEY_REPAIR_LOGO);
		break;
	case GAME_OBJECT_PICKETUP_TYPE_SHIELD:
		pPickup->logo=PX_ResourceLibraryGetShape(&pPickup->pIns->runtime.ResourceLibrary,GAME_TEXT_KEY_SHIELD_LOGO);
		break;
	case GAME_OBJECT_PICKETUP_TYPE_SPEEDUP:
		pPickup->logo=PX_ResourceLibraryGetShape(&pPickup->pIns->runtime.ResourceLibrary,GAME_TEXT_KEY_SPEEDUP_LOGO);
		break;
	case GAME_OBJECT_PICKETUP_TYPE_POWERUP:
		pPickup->logo=PX_ResourceLibraryGetShape(&pPickup->pIns->runtime.ResourceLibrary,GAME_TEXT_KEY_POWERUP_LOGO);
		break;
	case GAME_OBJECT_PICKETUP_TYPE_PROTECTSHIELD:
		pPickup->logo=PX_ResourceLibraryGetShape(&pPickup->pIns->runtime.ResourceLibrary,GAME_TEXT_KEY_PROTECTSHIELD_LOGO);
		break;
	case GAME_OBJECT_PICKETUP_TYPE_PIERCER:
		pPickup->logo=PX_ResourceLibraryGetShape(&pPickup->pIns->runtime.ResourceLibrary,GAME_TEXT_KEY_PIERCER_LOGO);
		break;
	}

	PX_ObjectRegisterEvent(pObject,PX_OBJECT_EVENT_IMPACT,Game_Object_PickupImpact,PX_NULL);
	PX_ObjectRegisterEvent(pObject,GAME_OBJECT_EVENT_DAMAGE,Game_Object_PickupOnDamage,PX_NULL);
	PX_ObjectRegisterEvent(pObject,GAME_OBJECT_EVENT_CHANGEVELOCITY,Game_Object_PickupOnVelocityChange,PX_NULL);

	return pObject;
}

px_void Game_Object_PickupCreate(PX_Instance *pIns,PX_World *pWorld,px_point position,GAME_OBJECT_PICKETUP_TYPE type,GAME_OBJECT_PICKUP_QUALITY quality)
{
	PX_Object *pObject;
	pObject=Game_Object_PickupCreateObject(pIns,pWorld,position,type,GAME_IMPACT_TYPE_ALL_PLAYERGROUP,quality);
	if (pObject)
	{
		PX_WorldAddObject(pWorld,pObject);
	}
}

px_void Game_Object_PickupRandCreate1(PX_Instance *pIns,PX_World *pWorld,px_point position)
{
	px_int GameRand;
	GameRand=(Game_rand()%11);
	switch(GameRand)
	{
	case 0:
		Game_Object_PickupCreate(pIns,pWorld,position,GAME_OBJECT_PICKETUP_TYPE_MACHINEGUN,GAME_OBJECT_PICKUP_QUILITY_NORMAL);
		break;
	case 1:
		Game_Object_PickupCreate(pIns,pWorld,position,GAME_OBJECT_PICKETUP_TYPE_MACHINEGUN,GAME_OBJECT_PICKUP_QUILITY_NORMAL);
		break;
	case 2:
		Game_Object_PickupCreate(pIns,pWorld,position,GAME_OBJECT_PICKETUP_TYPE_SPEEDUP,GAME_OBJECT_PICKUP_QUILITY_NORMAL);
		break;
	case 3:
		Game_Object_PickupCreate(pIns,pWorld,position,GAME_OBJECT_PICKETUP_TYPE_POWERUP,GAME_OBJECT_PICKUP_QUILITY_NORMAL);
		break;
	case 4:
		Game_Object_PickupCreate(pIns,pWorld,position,GAME_OBJECT_PICKETUP_TYPE_MINE,GAME_OBJECT_PICKUP_QUILITY_NORMAL);
		break;
	case 5:
		Game_Object_PickupCreate(pIns,pWorld,position,GAME_OBJECT_PICKETUP_TYPE_MISSILE,GAME_OBJECT_PICKUP_QUILITY_NORMAL);
		break;
	case 6:
		Game_Object_PickupCreate(pIns,pWorld,position,GAME_OBJECT_PICKETUP_TYPE_RAILGUN,GAME_OBJECT_PICKUP_QUILITY_NORMAL);
		break;
	case 7:
		Game_Object_PickupCreate(pIns,pWorld,position,GAME_OBJECT_PICKETUP_TYPE_MACHINEGUN,GAME_OBJECT_PICKUP_QUILITY_NORMAL);
		break;
	case 8:
		Game_Object_PickupCreate(pIns,pWorld,position,GAME_OBJECT_PICKETUP_TYPE_HEALTH,GAME_OBJECT_PICKUP_QUILITY_NORMAL);
		break;
	case 9:
		Game_Object_PickupCreate(pIns,pWorld,position,GAME_OBJECT_PICKETUP_TYPE_SHIELD,GAME_OBJECT_PICKUP_QUILITY_NORMAL);
		break;
	case 10:
		Game_Object_PickupCreate(pIns,pWorld,position,GAME_OBJECT_PICKETUP_TYPE_PIERCER,GAME_OBJECT_PICKUP_QUILITY_NORMAL);
		break;
	default:
		PX_ASSERT();
	}
}

px_void Game_Object_PickupRandCreate2(PX_Instance *pIns,PX_World *pWorld,px_point position)
{
	px_int GameRand;
	GameRand=(Game_rand()%12);

	switch(GameRand)
	{
	case 0:
		Game_Object_PickupCreate(pIns,pWorld,position,GAME_OBJECT_PICKETUP_TYPE_NUCLEAR,GAME_OBJECT_PICKUP_QUILITY_HIGH);
		break;
	case 1:
		Game_Object_PickupCreate(pIns,pWorld,position,GAME_OBJECT_PICKETUP_TYPE_BLACKHOLE,GAME_OBJECT_PICKUP_QUILITY_HIGH);
		break;
	case 2:
		Game_Object_PickupCreate(pIns,pWorld,position,GAME_OBJECT_PICKETUP_TYPE_SPEEDUP,GAME_OBJECT_PICKUP_QUILITY_HIGH);
		break;
	case 3:
		Game_Object_PickupCreate(pIns,pWorld,position,GAME_OBJECT_PICKETUP_TYPE_POWERUP,GAME_OBJECT_PICKUP_QUILITY_HIGH);
		break;
	case 4:
		Game_Object_PickupCreate(pIns,pWorld,position,GAME_OBJECT_PICKETUP_TYPE_MINE,GAME_OBJECT_PICKUP_QUILITY_HIGH);
		break;
	case 5:
		Game_Object_PickupCreate(pIns,pWorld,position,GAME_OBJECT_PICKETUP_TYPE_MISSILE,GAME_OBJECT_PICKUP_QUILITY_HIGH);
		break;
	case 6:
		Game_Object_PickupCreate(pIns,pWorld,position,GAME_OBJECT_PICKETUP_TYPE_RAILGUN,GAME_OBJECT_PICKUP_QUILITY_HIGH);
		break;
	case 7:
		Game_Object_PickupCreate(pIns,pWorld,position,GAME_OBJECT_PICKETUP_TYPE_MACHINEGUN,GAME_OBJECT_PICKUP_QUILITY_HIGH);
		break;
	case 8:
		Game_Object_PickupCreate(pIns,pWorld,position,GAME_OBJECT_PICKETUP_TYPE_HEALTH,GAME_OBJECT_PICKUP_QUILITY_HIGH);
		break;
	case 9:
		Game_Object_PickupCreate(pIns,pWorld,position,GAME_OBJECT_PICKETUP_TYPE_SHIELD,GAME_OBJECT_PICKUP_QUILITY_HIGH);
		break;
	case 10:
		Game_Object_PickupCreate(pIns,pWorld,position,GAME_OBJECT_PICKETUP_TYPE_PROTECTSHIELD,GAME_OBJECT_PICKUP_QUILITY_NORMAL);
		break;
	case 11:
		Game_Object_PickupCreate(pIns,pWorld,position,GAME_OBJECT_PICKETUP_TYPE_PIERCER,GAME_OBJECT_PICKUP_QUILITY_HIGH);
		break;
	default:
		PX_ASSERT();
	}

}


px_void Game_Object_PickupRandCreate3(PX_Instance *pIns,PX_World *pWorld,px_point position)
{
	px_point pt;
	pt=position;
	pt.x+=32;
	pt.y+=32;
	Game_Object_PickupCreate(pIns,pWorld,pt,GAME_OBJECT_PICKETUP_TYPE_SHIELD,GAME_OBJECT_PICKUP_QUILITY_HIGH);
	pt=position;
	pt.x-=32;
	pt.y+=32;
	Game_Object_PickupCreate(pIns,pWorld,pt,GAME_OBJECT_PICKETUP_TYPE_HEALTH,GAME_OBJECT_PICKUP_QUILITY_HIGH);
	pt=position;
	pt.y-=32;
	Game_Object_PickupCreate(pIns,pWorld,pt,GAME_OBJECT_PICKETUP_TYPE_MACHINEGUN,GAME_OBJECT_PICKUP_QUILITY_HIGH);
}

px_void Game_Object_PickupRandCreate4(PX_Instance *pIns,PX_World *pWorld,px_point position)
{
	px_int GameRand;
	GameRand=(Game_rand()%6);
	switch(GameRand)
	{
	case 0:
		Game_Object_PickupCreate(pIns,pWorld,position,GAME_OBJECT_PICKETUP_TYPE_MINE,GAME_OBJECT_PICKUP_QUILITY_NORMAL);
		break;
	case 1:
		Game_Object_PickupCreate(pIns,pWorld,position,GAME_OBJECT_PICKETUP_TYPE_MISSILE,GAME_OBJECT_PICKUP_QUILITY_NORMAL);
		break;
	case 2:
		Game_Object_PickupCreate(pIns,pWorld,position,GAME_OBJECT_PICKETUP_TYPE_RAILGUN,GAME_OBJECT_PICKUP_QUILITY_NORMAL);
		break;
	case 3:
		Game_Object_PickupCreate(pIns,pWorld,position,GAME_OBJECT_PICKETUP_TYPE_HEALTH,GAME_OBJECT_PICKUP_QUILITY_NORMAL);
		break;
	case 4:
		Game_Object_PickupCreate(pIns,pWorld,position,GAME_OBJECT_PICKETUP_TYPE_SHIELD,GAME_OBJECT_PICKUP_QUILITY_NORMAL);
		break;
	case 5:
		Game_Object_PickupCreate(pIns,pWorld,position,GAME_OBJECT_PICKETUP_TYPE_PIERCER,GAME_OBJECT_PICKUP_QUILITY_NORMAL);
		break;
	default:
		PX_ASSERT();
	}
}