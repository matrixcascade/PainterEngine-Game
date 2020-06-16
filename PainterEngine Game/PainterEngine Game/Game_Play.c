#include "Game_Play.h"

px_void Game_PlayOnAddScore(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	Game_Play *pPlay=(Game_Play *)ptr;
	px_int player=e.Param_int[0];
	px_float score=e.Param_float[1];
	if (player<PX_COUNTOF(pPlay->Players))
	{
		pPlay->Players[player].Score+=score;
	}
}

px_void Game_PlayOnMessage(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	Game_Play_Message message;
	Game_Play *pPlay=(Game_Play *)ptr;
	PX_memcpy(message.Message,e.Param_ptr[0],sizeof(message.Message));
	PX_VectorPushback(&pPlay->Messages,&message);
}

px_void Game_PlayOnDefeat(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	Game_Play_Message message;
	Game_Play *pPlay=(Game_Play *)ptr;
	Game_Object_Ship *pAttackship=Game_Object_GetShip(pPlay->Players[e.Param_int[0]].pShipObject);
	Game_Object_Ship *pDefeatship=Game_Object_GetShip(pPlay->Players[e.Param_int[1]].pShipObject);

	PX_memset(&message,0,sizeof(message));
	PX_wstrcat(message.Message,pAttackship->PlayerName);
	PX_wstrcat(message.Message,(px_word *)L" ÌÔÌ­ÁË ");
	PX_wstrcat(message.Message,pDefeatship->PlayerName);

	PX_VectorPushback(&pPlay->Messages,&message);
}


px_bool Game_PlayInitialize(PX_Instance *Ins,Game_Play *pPlay)
{
	pPlay->pIns=Ins;

	PX_memset(pPlay->Players,0,sizeof(pPlay->Players));
	Game_PlayReset(pPlay);
	return PX_TRUE;
}

px_void Game_PlayReset(Game_Play *pPlay)
{
	//reset mp
	pPlay->pIns->runtime.partical_mp=MP_Create(MP_Malloc(&pPlay->pIns->runtime.mp_game,GAME_PARTICAL_MP_FIRE),GAME_PARTICAL_MP_FIRE);
	pPlay->pIns->runtime.explode_mp=MP_Create(MP_Malloc(&pPlay->pIns->runtime.mp_game,GAME_PARTICAL_MP_EXPLODE),GAME_PARTICAL_MP_EXPLODE);

	//message
	PX_VectorInit(&pPlay->pIns->runtime.mp_game,&pPlay->Messages,sizeof(Game_Play_Message),32);

	//Create World
	if(!PX_WorldInit(&pPlay->pIns->runtime.mp_game,&pPlay->world,GAME_WORLD_WIDTH,GAME_WORLD_HEIGHT,pPlay->pIns->runtime.RenderSurface.width,pPlay->pIns->runtime.RenderSurface.height,GAME_PARTICAL_MP_CALCULATE))
		return;
	
	pPlay->world.auxiliaryXSpacer=84;
	pPlay->world.auxiliaryYSpacer=84;

	pPlay->PlayerLinker=PX_ObjectCreate(&pPlay->pIns->runtime.mp_game,PX_NULL,0,0,0,0,0,0);
	pPlay->treasureGenElpased=0;
	pPlay->stoneGenElpased=0;
	pPlay->AlienGenElpased=0;
	pPlay->elpased=0;
	pPlay->GameOverWaitElpased=0;
	Game_PlaySetGameTime(pPlay,GAME_PLAY_DEFAULT_TIME);

	PX_ObjectRegisterEvent(pPlay->PlayerLinker,GAME_OBJECT_EVENT_ADDSCORE,Game_PlayOnAddScore,pPlay);
	PX_ObjectRegisterEvent(pPlay->PlayerLinker,GAME_OBJECT_EVENT_MESSAGE,Game_PlayOnMessage,pPlay);
	PX_ObjectRegisterEvent(pPlay->PlayerLinker,GAME_OBJECT_EVENT_DEFEAT,Game_PlayOnDefeat,pPlay);

	PX_WorldSetCamera(&pPlay->world,PX_POINT(GAME_WORLD_WIDTH/2,GAME_WORLD_HEIGHT/2,0));
}

px_void Game_PlaySetGameTime(Game_Play *pPlay,px_dword gameTime)
{
	pPlay->GameTime=gameTime;
}

px_void Game_PlayStart(Game_Play *pPlay,GAME_PLAY_GAMETYPE gameType,PX_GameStartUp_PlayerDataDesc PlayerDataDesc[],px_int playerCount)
{
	px_int i;
	pPlay->PlayerCount=playerCount;
	pPlay->GameType=gameType;
	//players Initialize
	for (i=0;i<playerCount;i++)
	{
		Game_Object_ShipDescription desc;
		PX_Object *pobject;
		Game_Object_Ship *pShip;
		PX_memset(&desc,0,sizeof(Game_Object_ShipDescription));
		desc.player=i;
		//base info
		PX_memset(pPlay->Players[i].account,0,sizeof(pPlay->Players[i].account));
		PX_strset(pPlay->Players[i].account,PlayerDataDesc[i].account);

		//desc
		desc.base_weapon_generator_time=PlayerDataDesc[i].base_weapon_generator_time;//200;
		desc.group=PlayerDataDesc[i].group;//i;
		desc.maxforce=PlayerDataDesc[i].maxforce;//100;
		desc.maxlife=PlayerDataDesc[i].maxlife;//200
		desc.maxshield=PlayerDataDesc[i].maxshield;//100;
		desc.maxspeed=PlayerDataDesc[i].maxspeed;//200;
		desc.last_fire_recovery=PlayerDataDesc[i].last_fire_recovery;//3000;
		desc.slientTime=PlayerDataDesc[i].slientTime;//10000;
		desc.Armor=PlayerDataDesc[i].Armor;//0
		desc.playerLinkerObject=pPlay->PlayerLinker;

		pobject=Game_Object_ShipCreate(pPlay->pIns,&pPlay->world,desc);
		pPlay->Players[i].pShipObject=pobject;
		

		pShip=Game_Object_GetShip(pobject);
		pShip->PlayerName[0]=0;
		PX_wstrcat(pShip->PlayerName,PlayerDataDesc[i].NickName);
		if(!PX_TextureCreateFromMemory(&pPlay->pIns->runtime.mp_resources,PlayerDataDesc[i].Photo_Data,sizeof(PlayerDataDesc[i].Photo_Data),&pShip->photo)) PX_ASSERT();
		if(!PX_TextureCreateScale(&pPlay->pIns->runtime.mp_resources,&pShip->photo,24,24,&pShip->photo_mini)) PX_ASSERT();

	}
}
px_void Game_PlayUpdate(Game_Play *pPlay,px_dword elpased)
{
	px_int i;
	//////////////////////////////////////////////////////////////////////////
	//Game Over Check

	if (Game_PlayIsGameOver(pPlay))
	{
		if (pPlay->GameOverWaitElpased>=GAME_PLAY_GAME_WAIT_ELPASED)
		{
			return;
		}
		else
		{
			pPlay->GameOverWaitElpased+=elpased;
		}
		
	}

	//////////////////////////////////////////////////////////////////////////
	//game time
	pPlay->elpased+=elpased;

	///////////////////////////////////////////////////////////////////////////
	//Player InfoUpdate
	for (i=0;i<PX_COUNTOF(pPlay->Players);i++)
	{
		if (pPlay->Players[i].pShipObject)
		{
			if (pPlay->Players[i].pShipObject->Enabled)
			{
				pPlay->Players[i].aliveTime+=elpased;
			}
		}
	}


	//////////////////////////////////////////////////////////////////////////
	//Stone
	
	pPlay->stoneGenElpased+=elpased;

	if (pPlay->stoneGenElpased>GAME_PLAY_STONE_GEN_DURATION)
	{
		pPlay->stoneGenElpased-=GAME_PLAY_STONE_GEN_DURATION;
		Game_Object_StoneCreate(pPlay->pIns,&pPlay->world);
	}


	//////////////////////////////////////////////////////////////////////////
	//Alien

	pPlay->AlienGenElpased+=elpased;

	if (pPlay->AlienGenElpased>GAME_PLAY_ALIEN_GEN_DURATION)
	{
		pPlay->AlienGenElpased-=GAME_PLAY_ALIEN_GEN_DURATION;
		Game_Object_AlienCreate(pPlay->pIns,&pPlay->world);
	}
	
	//////////////////////////////////////////////////////////////////////////
	//treasure
	pPlay->treasureGenElpased+=elpased;
	if (pPlay->treasureGenElpased>=GAME_PLAY_TREASURE_GEN_DURATION)
	{
		px_int i;
		pPlay->treasureGenElpased=0;
		for (i=0;i<GAME_PLAY_TREASURE_GEN_COUNT;i++)
		{
			px_float x=100.0f+Game_rand()%(GAME_WORLD_WIDTH-200);
			px_float y=100.0f+Game_rand()%(GAME_WORLD_HEIGHT-200);
			Game_Object_TreasureTagCreate(pPlay->pIns,&pPlay->world,PX_POINT(x,y,0));
		}
	}

	PX_WorldUpdate(&pPlay->world,elpased);
}

px_void Game_PlayRender(px_surface *psurface,Game_Play *pPlay,px_dword elpased)
{
	PX_WorldRender(psurface,&pPlay->world,elpased);
}

px_void Game_PlayPostEvent(Game_Play *pPlay,PX_Object_Event e)
{
	
}

px_int Game_PlayGetPlayerRank(Game_Play *pPlay,px_int Player)
{
	px_int sortcount=0;
	px_int i,rank=1;
	Game_Object_Ship *pShip;
	PX_Object *pShipObject;

	if (pPlay->Players[Player].pShipObject)
	{
		if (pPlay->Players[Player].pShipObject->Enabled)
		{
			for (i=0;i<pPlay->PlayerCount;i++)
			{
				if (pPlay->Players[i].pShipObject)
				{
					pShipObject=pPlay->Players[i].pShipObject;
					pShip=Game_Object_GetShip(pPlay->Players[i].pShipObject);
					if (pShipObject->Enabled)
					{
						if (pPlay->Players[i].Score>pPlay->Players[Player].Score)
						{
							rank++;
						}
					}
				}
			}
			return rank;
		}
		else
		{
			for (i=0;i<pPlay->PlayerCount;i++)
			{
				if (pPlay->Players[i].pShipObject)
				{
					pShipObject=pPlay->Players[i].pShipObject;
					pShip=Game_Object_GetShip(pPlay->Players[i].pShipObject);
					if (pShipObject->Enabled)
					{
						rank++;
					}
				}
			}

			for (i=0;i<pPlay->PlayerCount;i++)
			{
				if (pPlay->Players[i].pShipObject)
				{
					pShipObject=pPlay->Players[i].pShipObject;
					pShip=Game_Object_GetShip(pPlay->Players[i].pShipObject);
					if (!pShipObject->Enabled)
					{
						if (pPlay->Players[i].aliveTime>pPlay->Players[Player].aliveTime)
						{
							rank++;
						}
					}
				}
			}
			return rank;
		}
	}
	return -1;
}

px_int Game_PlayIsPlayerAlive(Game_Play *pPlay,px_int Player)
{
	return pPlay->Players[Player].pShipObject&&pPlay->Players[Player].pShipObject->Enabled;
}


px_bool Game_PlayIsGameOver(Game_Play *pPlay)
{
	px_int i;
	px_int AliveCount=0;
	px_int group=-1;

	if (pPlay->elpased>=pPlay->GameTime)
	{
		return PX_TRUE;
	}
	if (pPlay->PlayerCount<=1)
	{
		return PX_FALSE;
	}

	for (i=0;i<pPlay->PlayerCount;i++)
	{
		if (Game_PlayIsPlayerAlive(pPlay,i))
		{
			if (group==-1)
			{
				group=Game_Object_GetShip(pPlay->Players[i].pShipObject)->group;
			}
			else
			{
				if (group!=Game_Object_GetShip(pPlay->Players[i].pShipObject)->group)
				{
					return PX_FALSE;
				}
			}
			
		}
	}
	return PX_TRUE;
}
