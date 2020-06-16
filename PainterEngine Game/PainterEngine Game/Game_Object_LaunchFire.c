#include "Game_Object_LaunchFire.h"

px_void Game_Object_LaunchFireFree(PX_Object *pObject)
{
	Game_Object_LaunchFire *pDesc=(Game_Object_LaunchFire *)pObject->pObject;
	PX_ParticalLauncherFree(&pDesc->launcher);
}

px_void Game_Object_LaunchFireUpdate(PX_Object *pObject,px_dword elpased)
{
	Game_Object_LaunchFire *pDesc=(Game_Object_LaunchFire *)pObject->pObject;
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
	PX_ParticalLauncherUpdate(&pDesc->launcher,elpased);

	if (pDesc->alive==0)
	{
		PX_WorldRemoveObject(pDesc->pWorld,pObject);
	}
}

px_void Game_Object_LaunchFireRender(px_surface *psurface,PX_Object *pObject,px_dword elpased)
{
	Game_Object_LaunchFire *pDesc=(Game_Object_LaunchFire *)pObject->pObject;
	PX_ParticalLauncherRender(psurface,&pDesc->launcher,PX_POINT(pObject->x,pObject->y,pObject->z));
}

PX_Partical_Atom Game_Object_LaunchFireCreatePartical(struct _PX_Partical_Launcher *launcher,px_int index)
{
	PX_Partical_Atom atom;
	atom.aliveTime=300;
	atom.alpha=1;
	atom.alphaIncrement=-3.0f;
	atom.elpasedTime=0;
	atom.hdrB=1;
	atom.hdrG=1;
	atom.hdrR=1;
	atom.mass=1;
	atom.position=PX_POINT(0,0,0);
	atom.roatationSpeed=0;
	atom.rotation=0;
	atom.size=1;
	atom.sizeIncrement=0.0f;
	atom.velocity=PX_POINT((px_float)((px_int)(PX_rand()%100)+150),(px_float)(-55+(px_int)(PX_rand()%110)),0);
	return atom;
}

PX_Object * Game_Object_LaunchFireCreateObject(PX_Instance *pIns,PX_World *pWorld,PX_ParticalLauncher_InitializeInfo init_info,px_point position,px_point direction,px_point velocity,px_dword alive)
{
	PX_Object *pObject=PX_NULL;
	px_texture *pTexture=init_info.tex;
	Game_Object_LaunchFire *pDesc;

	pDesc=(Game_Object_LaunchFire *)MP_Malloc(&pIns->runtime.explode_mp,sizeof(Game_Object_LaunchFire));

	if (pDesc==PX_NULL)
	{
		return PX_NULL;
	}

	pDesc->pWorld=pWorld;
	pDesc->pIns=pIns;
	pDesc->alive=alive;

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
	pObject->Func_ObjectFree=Game_Object_LaunchFireFree;
	pObject->Func_ObjectUpdate=Game_Object_LaunchFireUpdate;
	pObject->Func_ObjectRender=Game_Object_LaunchFireRender;
	pObject->User_ptr=PX_NULL;
	pObject->impact_Object_type=0;
	pObject->impact_test_type=0;
	
	PX_ParticalLauncherCreateEx(&pDesc->launcher,&pIns->runtime.explode_mp,init_info);
	pDesc->launcher.launcherVelocity=velocity;
	pDesc->launcher.direction=direction;
	pDesc->launcher.user=pDesc;
	
	return pObject;
}




px_void Game_Object_LaunchFireCreate(PX_Instance *pIns,PX_World *pWorld,px_point position,px_point direction,px_point velocity,const px_char sndKey[])
{
	PX_Object *pObject;
	PX_ParticalLauncher_InitializeInfo partical_info;
	partical_info.Create_func=Game_Object_LaunchFireCreatePartical;
	partical_info.force=PX_POINT(0,0,0);
	partical_info.generateDuration=0;
	partical_info.launchCount=30;
	partical_info.maxCount=30;
	partical_info.resistanceK=0;
	partical_info.tex=PX_ResourceLibraryGetTexture(&pIns->runtime.ResourceLibrary,GAME_TEXT_KEY_PARTICAL_SHOTGUNASH);
	partical_info.Update_func=PX_NULL;

	pObject=Game_Object_LaunchFireCreateObject(pIns,pWorld,partical_info,position,direction,velocity,GAME_OBJECT_SHOTGUNFIREASH_ALIVE_TIME);

	if(!PX_WorldAddObject(pWorld,pObject))
	{
		PX_ASSERT();
	}
	PX_PlayLocalSoundFromResource(pIns,pWorld,position.x,position.y,sndKey,PX_FALSE);
}

