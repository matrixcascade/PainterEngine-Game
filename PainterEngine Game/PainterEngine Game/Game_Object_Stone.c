#include "Game_Object_Stone.h"

static px_void Game_GenInitializePositionAndVelocity(PX_Instance *pIns,PX_World *pWorld,px_point *Position,px_point *velocty,px_float mod_velocity)
{
	px_int genx,geny,vpx,vpy;
	px_point gen_v;
	switch(Game_rand()%4)
	{
	case 0:
		{
			geny=-GAME_STONE_GEN_SPACE;
			genx=-GAME_STONE_GEN_SPACE+(px_int)(Game_rand()%(pWorld->world_width+2*GAME_STONE_GEN_SPACE));
			vpx=(px_int)(Game_rand()%pWorld->world_width);
			vpy=0;
			gen_v=PX_PointUnit(PX_POINT((px_float)(vpx-genx),(px_float)(vpy-geny),0));
			gen_v=PX_PointMul(gen_v,mod_velocity);
		}
		break;
	case 3:
		{
			geny=pWorld->world_height+GAME_STONE_GEN_SPACE;
			genx=-GAME_STONE_GEN_SPACE+(px_int)(Game_rand()%(pWorld->world_width+2*GAME_STONE_GEN_SPACE));
			vpx=(px_int)(Game_rand()%pWorld->world_width);
			vpy=pWorld->world_height;

			gen_v=PX_PointUnit(PX_POINT((px_float)(vpx-genx),(px_float)(vpy-geny),0));
			gen_v=PX_PointMul(gen_v,mod_velocity);
		}
		break;
	case 1:
		{
			geny=-GAME_STONE_GEN_SPACE+(px_int)(Game_rand()%(pWorld->world_height+2*GAME_STONE_GEN_SPACE));
			genx=-GAME_STONE_GEN_SPACE;
			vpx=0;
			vpy=(px_int)(Game_rand()%pWorld->world_height);

			gen_v=PX_PointUnit(PX_POINT((px_float)(vpx-genx),(px_float)(vpy-geny),0));
			gen_v=PX_PointMul(gen_v,mod_velocity);
		}
		break;
	case 2:
		{
			geny=-GAME_STONE_GEN_SPACE+(px_int)(Game_rand()%(pWorld->world_height+2*GAME_STONE_GEN_SPACE));
			genx=pWorld->world_width+GAME_STONE_GEN_SPACE;
			vpx=pWorld->world_width;
			vpy=(px_int)(Game_rand()%pWorld->world_height);

			gen_v=PX_PointUnit(PX_POINT((px_float)(vpx-genx),(px_float)(vpy-geny),0));
			gen_v=PX_PointMul(gen_v,mod_velocity);
		}
		break;
	}
	*Position=PX_POINT((px_float)genx,(px_float)geny,0);
	*velocty=gen_v;
}

Game_Object_Stone *Game_Object_GetStone(PX_Object *pObject)
{
	return (Game_Object_Stone *)pObject->pObject;
}

px_void Game_Object_StoneOnDamage(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	Game_Object_Stone *pst=Game_Object_GetStone(pObject);

	if (pst->life<=e.Param_float[0])
	{
		pst->life=0;
		//destroy stone
		PX_WorldRemoveObject(pst->pWorld,pObject);
		Game_Object_FireExplosionCreate(pst->pIns,pst->pWorld,PX_POINT(pObject->x,pObject->y,0),pObject->diameter*pst->scale/2.0f,PX_TRUE);
		//
		Game_Object_PickupRandCreate1(pst->pIns,pst->pWorld,PX_POINT(pObject->x,pObject->y,0));
	}
	else
	{
		pst->life-=e.Param_float[0];
		pst->beAttackElpased=0;
	}
}

px_void Game_Object_StoneOnVelocityChange(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	Game_Object_Stone *pStone=(Game_Object_Stone *)pObject->pObject;
	pStone->velocity=PX_PointAdd(pStone->velocity,PX_POINT(e.Param_float[0],e.Param_float[1],0));
}

px_void Game_Object_StoneOnImpact(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	
}


px_void Game_Object_StoneFree(PX_Object *pObject)
{

}

px_void Game_Object_StoneUpdate(PX_Object *pObject,px_dword elpased)
{
	Game_Object_Stone *pst=Game_Object_GetStone(pObject);

	pst->beAttackElpased+=elpased;

	pst->rotation+=pst->rotationSpeed*elpased/1000;
	pObject->x+=pst->velocity.x*elpased/1000;
	pObject->y+=pst->velocity.y*elpased/1000;

	if (pObject->x<-GAME_STONE_GEN_SPACE-1||pObject->x>pst->pWorld->world_width+GAME_STONE_GEN_SPACE+1||\
		pObject->y<-GAME_STONE_GEN_SPACE-1||pObject->y>pst->pWorld->world_height+GAME_STONE_GEN_SPACE+1\
		)
	{
		PX_WorldRemoveObject(pst->pWorld,pObject);

	}

}


px_void Game_Object_StoneRender(px_surface *psurface,PX_Object *pObject,px_dword elpased)
{
	Game_Object_Stone *pst=Game_Object_GetStone(pObject);

	if (pst->beAttackElpased<50)
	{
		PX_TEXTURERENDER_BLEND blend;
		blend.alpha=1;
		blend.hdr_B=0;
		blend.hdr_G=0;
		blend.hdr_R=1;
		PX_TextureRenderEx(psurface,pst->pTexture,(px_int)pObject->x,(px_int)pObject->y,PX_TEXTURERENDER_REFPOINT_CENTER,&blend,pst->scale,pst->rotation);
	}
	else
		PX_TextureRenderEx(psurface,pst->pTexture,(px_int)pObject->x,(px_int)pObject->y,PX_TEXTURERENDER_REFPOINT_CENTER,PX_NULL,pst->scale,pst->rotation);


	if (pst->show_impact_region)
	{

		PX_GeoDrawSolidCircle(psurface,(px_int)pObject->x,(px_int)pObject->y,(px_int)(pObject->diameter/2),PX_COLOR(128,255,0,0));
	}
}

PX_Object* Game_Object_StoneCreateObject(PX_Instance *pIns,PX_World *pWorld,px_float life,px_float velocity)
{
	PX_Object *pObject;
	px_texture *pTexture;
	Game_Object_Stone *pst;
	px_float scale;
	px_point genPostion,genVelocity;


	switch(Game_rand()%3)
	{
	case 0:
		pTexture=PX_ResourceLibraryGetTexture(&pIns->runtime.ResourceLibrary,GAME_TEXT_KEY_TEX_STONE1);
		break;
	case 1:
		pTexture=PX_ResourceLibraryGetTexture(&pIns->runtime.ResourceLibrary,GAME_TEXT_KEY_TEX_STONE2);
		break;
	case 2:
		pTexture=PX_ResourceLibraryGetTexture(&pIns->runtime.ResourceLibrary,GAME_TEXT_KEY_TEX_STONE3);
		break;
	default:
		PX_ASSERT();
	}

	pst=(Game_Object_Stone *)MP_Malloc(&pIns->runtime.mp_game,sizeof(Game_Object_Stone));

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

	scale=0.8f+((px_int)(Game_rand()%300)/1000.0f);

	Game_GenInitializePositionAndVelocity(pIns,pWorld,&genPostion,&genVelocity,velocity);

	pObject->x=genPostion.x;
	pObject->y=genPostion.y;
	pObject->z=GAME_STONE_Z;
	pObject->Width=128*scale;
	pObject->diameter=(128*scale);
	pObject->Height=128*scale;
	pObject->pObject=pst;
	pObject->Type=GAME_OBJECT_TYPE_STONE;
	pObject->ReceiveEvents=PX_TRUE;
	pObject->Func_ObjectFree=Game_Object_StoneFree;
	pObject->Func_ObjectUpdate=Game_Object_StoneUpdate;
	pObject->Func_ObjectRender=Game_Object_StoneRender;
	pObject->User_ptr=PX_NULL;
	pObject->impact_Object_type=GAME_IMPACT_TYPE_NATURE;
	pObject->impact_test_type=0;

	pst->show_impact_region=PX_FALSE;
	pst->beAttackElpased=0;
	pst->life=life;
	pst->pTexture=pTexture;
	pst->rotation=0;
	pst->rotationSpeed=(px_float)(GAME_STONE_DEFALUE_MIN_ROTATION_SPEED+(Game_rand()%(GAME_STONE_DEFALUE_MAX_ROTATION_SPEED-GAME_STONE_DEFALUE_MIN_ROTATION_SPEED)));
	pst->scale=scale;
	pst->velocity=genVelocity;
	pst->pWorld=pWorld;
	pst->pIns=pIns;

	PX_ObjectRegisterEvent(pObject,GAME_OBJECT_EVENT_DAMAGE,Game_Object_StoneOnDamage,PX_NULL);
	PX_ObjectRegisterEvent(pObject,GAME_OBJECT_EVENT_CHANGEVELOCITY,Game_Object_StoneOnVelocityChange,PX_NULL);
	return pObject;
}

px_void Game_Object_StoneCreate(PX_Instance *pIns,PX_World *pWorld)
{
	PX_Object *pObject;
	pObject=Game_Object_StoneCreateObject(pIns,pWorld,60,(px_float)(100+Game_rand()%50));
	if (pObject)
	{
		PX_WorldAddObject(pWorld,pObject);
	}
}