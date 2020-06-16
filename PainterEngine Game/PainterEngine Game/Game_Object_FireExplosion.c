#include "Game_Object_FireExplosion.h"

PX_Partical_Atom Game_Object_FireExplosionCreatePartical(struct _PX_Partical_Launcher *launcher,px_int index)
{
	PX_Partical_Atom atom;
	px_float x_v,y_v;

	Game_Object_FireExplosion *pL=(Game_Object_FireExplosion *)launcher->user;

	x_v=-pL->range+(2*pL->range)*((PX_rand()%1000)/1000.f);
	y_v=PX_sqrt(pL->range*pL->range-x_v*x_v);
	y_v=-y_v+(2*y_v)*((PX_rand()%1000)/1000.f);

	atom.aliveTime=1000;
	atom.alpha=1;
	atom.alphaIncrement=-1.0f;
	atom.elpasedTime=0;
	atom.hdrB=1;
	atom.hdrG=1;
	atom.hdrR=1;
	atom.mass=1;
	atom.position=PX_POINT(x_v,y_v,0);
	atom.roatationSpeed=0;
	atom.rotation=0;
	atom.size=pL->range/100+(PX_rand()%1000)/4000.f;
	atom.sizeIncrement=0.1f;
	atom.velocity=PX_PointMul(PX_PointUnit(atom.position),300*((PX_rand()%1000)/1000.f));
	return atom;
}

px_void Game_Object_FireExplosionFree(PX_Object *pObject)
{
	Game_Object_FireExplosion *pFire=(Game_Object_FireExplosion *)pObject->pObject;
	PX_ParticalLauncherFree(&pFire->launcher);
}

px_void Game_Object_FireExplosionUpdate(PX_Object *pObject,px_dword elpased)
{
	Game_Object_FireExplosion *pFire=(Game_Object_FireExplosion *)pObject->pObject;
	if (elpased>0)
	{
		if (elpased>pFire->alive)
		{
			pFire->alive=0;
		}
		else
		{
			pFire->alive-=elpased;
		}
	}
	PX_ParticalLauncherUpdate(&pFire->launcher,elpased);

	if (pFire->alive==0)
	{
		PX_WorldRemoveObject(pFire->pWorld,pObject);
	}
}

px_void Game_Object_FireExplosionRender(px_surface *psurface,PX_Object *pObject,px_dword elpased)
{
	Game_Object_FireExplosion *pfe=(Game_Object_FireExplosion *)pObject->pObject;

	if (pfe->ring)
	{
		px_float fr=(GAME_OBJECT_FIREEXPLOSION_DEFAULT_TIME-pfe->alive)*1.0f/GAME_OBJECT_FIREEXPLOSION_DEFAULT_TIME;
		PX_GeoDrawCircle(psurface,(px_int)pObject->x,(px_int)pObject->y,(px_int)(PX_sqrt(fr)*pfe->range*4),(px_int)(fr*pfe->range*0.25f),PX_COLOR((px_uchar)(255*(1-fr)),255,(px_uchar)(255*(1-fr)),0));
	}

	PX_ParticalLauncherRender(psurface,&pfe->launcher,PX_POINT(pObject->x,pObject->y,pObject->z));
}


PX_Object* Game_Object_FireExplosionCreateObject(PX_Instance *pIns,PX_World *pWorld,PX_ParticalLauncher_InitializeInfo init_info,px_point position,px_float range,px_dword alive,px_bool ring)
{
	PX_Object *pObject=PX_NULL;
	px_texture *pTexture=init_info.tex;
	Game_Object_FireExplosion *pFireExplosion;

	pFireExplosion=(Game_Object_FireExplosion *)MP_Malloc(&pIns->runtime.explode_mp,sizeof(Game_Object_FireExplosion));

	if (pFireExplosion==PX_NULL)
	{
		return PX_NULL;
	}

	pObject=(PX_Object *)PX_ObjectCreate(&pIns->runtime.explode_mp,PX_NULL,0,0,0,0,0,0);

	if (!pObject)
	{
		MP_Free(&pIns->runtime.explode_mp,pFireExplosion);
		return PX_NULL;
	}

	pObject->x=position.x;
	pObject->y=position.y;
	pObject->z=GAME_PARTICAL_FIRE_Z;
	pObject->Width=1;
	pObject->Height=1;


	pObject->pObject=pFireExplosion;
	pObject->Type=GAME_OBJECT_TYPE_DISPLAY;
	pObject->ReceiveEvents=PX_FALSE;
	pObject->Func_ObjectFree=Game_Object_FireExplosionFree;
	pObject->Func_ObjectUpdate=Game_Object_FireExplosionUpdate;
	pObject->Func_ObjectRender=Game_Object_FireExplosionRender;
	pObject->User_ptr=PX_NULL;
	pObject->impact_Object_type=0;
	pObject->impact_test_type=0;


	pFireExplosion->pIns=pIns;
	pFireExplosion->pWorld=pWorld;
	pFireExplosion->alive=alive;
	pFireExplosion->range=range;
	pFireExplosion->launcher.launcherPosition=PX_POINT(0,0,0);
	pFireExplosion->ring=ring;

	PX_ParticalLauncherCreateEx(&pFireExplosion->launcher,&pIns->runtime.explode_mp,init_info);
	pFireExplosion->launcher.user=pFireExplosion;


	return pObject;
}


px_void Game_Object_FireExplosionCreate(PX_Instance *pIns,PX_World *pWorld,px_point position,px_float range,px_bool ring)
{
	PX_ParticalLauncher_InitializeInfo partical_info;
	PX_Object *pObject;

	PX_memset(&partical_info,0,sizeof(PX_ParticalLauncher_InitializeInfo));

	partical_info.Create_func=Game_Object_FireExplosionCreatePartical;
	partical_info.force=PX_POINT(0,0,0);
	partical_info.generateDuration=0;
	if (range<16)
	{
		partical_info.launchCount=(px_int)16;
		partical_info.maxCount=(px_int)16;
	}
	else
	{
		partical_info.launchCount=(px_int)range;
		partical_info.maxCount=(px_int)range;
	}

	partical_info.resistanceK=2.0f;
	partical_info.tex=PX_ResourceLibraryGetTexture(&pIns->runtime.ResourceLibrary,GAME_TEXT_KEY_TEX_EXPLODE_PARTICAL);
	partical_info.Update_func=PX_NULL;

	pObject=Game_Object_FireExplosionCreateObject(pIns,pWorld,partical_info,position,range,GAME_OBJECT_FIREEXPLOSION_DEFAULT_TIME,ring);
	
	if (pObject)
	{
		PX_WorldAddObject(pWorld,pObject);
	}
	else
	{
		PX_ASSERT();
	}
	
	switch(PX_rand()%3)
	{
	case 0:
		PX_PlayLocalSoundFromResource(pIns,pWorld,position.x,position.y,GAME_TEXT_KEY_SND_EXPLODE01,PX_FALSE);
		break;
	case 1:
		PX_PlayLocalSoundFromResource(pIns,pWorld,position.x,position.y,GAME_TEXT_KEY_SND_EXPLODE02,PX_FALSE);
		break;
	case 2:
		PX_PlayLocalSoundFromResource(pIns,pWorld,position.x,position.y,GAME_TEXT_KEY_SND_EXPLODE02,PX_FALSE);
		break;
	}
}

