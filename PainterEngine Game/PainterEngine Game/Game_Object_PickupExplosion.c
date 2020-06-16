#include "Game_Object_PickupExplosion.h"



px_void Game_Object_PickupExplosionFree(PX_Object *pObject)
{
}


px_void Game_Object_PickupExplosionUpdate(PX_Object *pObject,px_dword elpased)
{
	Game_Object_PickupExplosion *pPickup=(Game_Object_PickupExplosion *)pObject->pObject;
	PX_World *pWorld=pPickup->pWorld;

	pPickup->elpased+=elpased;

	if (pPickup->elpased>=GAME_OBJECT_PICKUPEXPLOSION_ALIVE)
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

px_void Game_Object_PickupExplosionRender(px_surface *psurface,PX_Object *pObject,px_dword elpased)
{
	Game_Object_PickupExplosion *pPickup=(Game_Object_PickupExplosion *)pObject->pObject;

	PX_ShapeRenderEx(psurface,pPickup->logo,(px_int)pObject->x,(px_int)pObject->y,PX_TEXTURERENDER_REFPOINT_CENTER,\
		PX_COLOR((px_uchar)((1-pPickup->elpased*1.0f/GAME_OBJECT_PICKUPEXPLOSION_ALIVE)*255),255,0,0),\
		pPickup->elpased*1.0f/GAME_OBJECT_PICKUPEXPLOSION_ALIVE*1.5f,\
		0);
}



PX_Object * Game_Object_PickupExplosionCreateObject(PX_Instance *pIns,PX_World *pWorld,px_point position,GAME_OBJECT_PICKETUP_TYPE type)
{
	PX_Object *pObject;

	Game_Object_PickupExplosion *pPickup;

	pPickup=(Game_Object_PickupExplosion *)MP_Malloc(&pIns->runtime.mp_game,sizeof(Game_Object_PickupExplosion));

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
	pObject->z=GAME_EXPLOSION_Z;
	pObject->Width=8;
	pObject->Height=8;
	pObject->diameter=8;


	pObject->pObject=pPickup;
	pObject->Type=GAME_OBJECT_TYPE_MISSILE;
	pObject->ReceiveEvents=PX_TRUE;
	pObject->Func_ObjectFree=Game_Object_PickupExplosionFree;
	pObject->Func_ObjectUpdate=Game_Object_PickupExplosionUpdate;
	pObject->Func_ObjectRender=Game_Object_PickupExplosionRender;
	pObject->User_ptr=PX_NULL;
	pObject->impact_test_type=0;
	pObject->impact_Object_type=0;

	pPickup->pWorld=pWorld;
	pPickup->pIns=pIns;
	pPickup->elpased=0;
	pPickup->type=type;
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
		pPickup->logo=PX_ResourceLibraryGetShape(&pPickup->pIns->runtime.ResourceLibrary,GAME_TEXT_KEY_REPAIR_LOGO);
		break;
	case GAME_OBJECT_PICKETUP_TYPE_PROTECTSHIELD:
		pPickup->logo=PX_ResourceLibraryGetShape(&pPickup->pIns->runtime.ResourceLibrary,GAME_TEXT_KEY_PROTECTSHIELD_LOGO);
		break;
	case GAME_OBJECT_PICKETUP_TYPE_PIERCER:
		pPickup->logo=PX_ResourceLibraryGetShape(&pPickup->pIns->runtime.ResourceLibrary,GAME_TEXT_KEY_PIERCER_LOGO);
		break;

	}
	return pObject;
}

px_void Game_Object_PickupExplosionCreate(PX_Instance *pIns,PX_World *pWorld,px_point position,GAME_OBJECT_PICKETUP_TYPE type)
{
	PX_Object *pObject;
	pObject=Game_Object_PickupExplosionCreateObject(pIns,pWorld,position,type);
	if (pObject)
	{
		PX_PlayLocalSoundFromResource(pIns,pWorld,position.x,position.y,GAME_TEXT_KEY_PICKUPEXPLOSION_SOUND,PX_FALSE);
		PX_WorldAddObject(pWorld,pObject);
	}
}
