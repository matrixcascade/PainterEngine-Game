#include "Game_Object_RailgunExplosion.h"


px_void Game_Object_RailgunExplosionFree(PX_Object *pObject)
{
	Game_Object_RailgunExplosion *pDesc=(Game_Object_RailgunExplosion *)pObject->pObject;
	PX_AnimationFree(&pDesc->Animation);
}

px_void Game_Object_RailgunExplosionUpdate(PX_Object *pObject,px_dword elpased)
{
	Game_Object_RailgunExplosion *pDesc=(Game_Object_RailgunExplosion *)pObject->pObject;
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

px_void Game_Object_RailgunExplosionRender(px_surface *psurface,PX_Object *pObject,px_dword elpased)
{
	px_float scale;

	Game_Object_RailgunExplosion *pDesc=(Game_Object_RailgunExplosion *)pObject->pObject;

	PX_AnimationUpdate(&pDesc->Animation,elpased);

	scale=pDesc->alive*1.0f/GAME_OBJECT_RAILGUNEXPLOSION_ALIVE_TIME;

	PX_AnimationRenderEx(psurface,&pDesc->Animation,PX_POINT(pObject->x,pObject->y,0),scale,PX_POINT(1,0,0),PX_TEXTURERENDER_REFPOINT_CENTER,PX_NULL);
}


PX_Object * Game_Object_RailgunExplosionCreateObject(PX_Instance *pIns,PX_World *pWorld,px_point position,px_dword alive)
{
	PX_Object *pObject=PX_NULL;
	PX_Animationlibrary *pLibrary;
	Game_Object_RailgunExplosion *pDesc;

	pDesc=(Game_Object_RailgunExplosion *)MP_Malloc(&pIns->runtime.explode_mp,sizeof(Game_Object_RailgunExplosion));
	pLibrary=PX_ResourceLibraryGetAnimationLibrary(&pIns->runtime.ResourceLibrary,GAME_TEXT_KEY_RAILGUN_EXPLOSION);
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
	pObject->z=GAME_EXPLOSION_Z;
	pObject->Width=1;
	pObject->Height=1;


	pObject->pObject=pDesc;
	pObject->Type=GAME_OBJECT_TYPE_DISPLAY;
	pObject->ReceiveEvents=PX_FALSE;
	pObject->Func_ObjectFree=Game_Object_RailgunExplosionFree;
	pObject->Func_ObjectUpdate=Game_Object_RailgunExplosionUpdate;
	pObject->Func_ObjectRender=Game_Object_RailgunExplosionRender;
	pObject->User_ptr=PX_NULL;
	
	
	
	return pObject;
}




px_void Game_Object_RailgunExplosionCreate(PX_Instance *pIns,PX_World *pWorld,px_point position)
{
	PX_Object *pObject;

	pObject=Game_Object_RailgunExplosionCreateObject(pIns,pWorld,position,GAME_OBJECT_RAILGUNEXPLOSION_ALIVE_TIME);

	if(!PX_WorldAddObject(pWorld,pObject))
	{
		PX_ASSERT();
	}
	PX_PlayLocalSoundFromResource(pIns,pWorld,position.x,position.y,GAME_TEXT_KEY_RAILGUN_EXPOLSIONSOUND,PX_FALSE);
}

