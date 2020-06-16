#include "Game_Object_RailgunFire.h"


px_void Game_Object_RailgunFireFree(PX_Object *pObject)
{
	Game_Object_RailgunFire *pDesc=(Game_Object_RailgunFire *)pObject->pObject;
	PX_AnimationFree(&pDesc->Animation);
}

px_void Game_Object_RailgunFireAshUpdate(PX_Object *pObject,px_dword elpased)
{
	Game_Object_RailgunFire *pDesc=(Game_Object_RailgunFire *)pObject->pObject;
	if (elpased>0)
	{
		if (elpased>pDesc->alive)
		{
			pDesc->alive=0;
		}
		else
		{
			pDesc->alive-=elpased;
		}
	}

	if (pDesc->alive==0)
	{
		PX_WorldRemoveObject(pDesc->pWorld,pObject);
	}
}

px_void Game_Object_RailgunFireRender(px_surface *psurface,PX_Object *pObject,px_dword elpased)
{
	Game_Object_RailgunFire *pDesc=(Game_Object_RailgunFire *)pObject->pObject;
	PX_AnimationUpdate(&pDesc->Animation,elpased);
	PX_AnimationRender(psurface,&pDesc->Animation,PX_POINT(pObject->x,pObject->y,0),PX_TEXTURERENDER_REFPOINT_CENTER,PX_NULL);
}


PX_Object * Game_Object_RailgunFireCreateObject(PX_Instance *pIns,PX_World *pWorld,px_point position,px_dword alive)
{
	PX_Object *pObject=PX_NULL;
	PX_Animationlibrary *pLibrary;
	Game_Object_RailgunFire *pDesc;

	pDesc=(Game_Object_RailgunFire *)MP_Malloc(&pIns->runtime.explode_mp,sizeof(Game_Object_RailgunFire));
	pLibrary=PX_ResourceLibraryGetAnimationLibrary(&pIns->runtime.ResourceLibrary,GAME_TEXT_KEY_RAILGUN_FIRE);
	PX_AnimationCreate(&pDesc->Animation,pLibrary);
	pDesc->alive=alive;
	pDesc->pIns=pIns;
	pDesc->pWorld=pWorld;

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
	pObject->z=GAME_PARTICAL_FIRE_Z;
	pObject->Width=1;
	pObject->Height=1;


	pObject->pObject=pDesc;
	pObject->Type=GAME_OBJECT_TYPE_DISPLAY;
	pObject->ReceiveEvents=PX_FALSE;
	pObject->Func_ObjectFree=Game_Object_RailgunFireFree;
	pObject->Func_ObjectUpdate=Game_Object_RailgunFireAshUpdate;
	pObject->Func_ObjectRender=Game_Object_RailgunFireRender;
	pObject->User_ptr=PX_NULL;
	
	
	
	return pObject;
}




px_void Game_Object_RailgunFireCreate(PX_Instance *pIns,PX_World *pWorld,px_point position)
{
	PX_Object *pObject;

	pObject=Game_Object_RailgunFireCreateObject(pIns,pWorld,position,GAME_OBJECT_RAILGUNFIRE_ALIVE_TIME);

	if(!PX_WorldAddObject(pWorld,pObject))
	{
		PX_ASSERT();
	}
	PX_PlayLocalSoundFromResource(pIns,pWorld,position.x,position.y,GAME_TEXT_KEY_RAILGUN_FIRESOUND,PX_FALSE);
}

