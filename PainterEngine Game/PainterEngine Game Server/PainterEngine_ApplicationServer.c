#include "PainterEngine_ApplicationServer.h"

PX_Application App;

px_bool game_syncdata_server_write(PX_SyncData_Server *server,PX_Sync_Port port,px_void *data,px_int size)
{
	PX_UDP_ADDR toAddr;
	PX_UDP *pudp=(PX_UDP *)server->user;
	toAddr.ipv4=port.dw_atom[0];
	toAddr.port=port.dw_atom[1];
	return PX_UDPSend(pudp,toAddr,(px_byte *)data,size);
}
px_bool game_syncdata_server_read(struct _PX_SyncData_Server *server,PX_Sync_Port *port,px_void *data,px_int buffersize,px_int *readsize)
{
	PX_UDP_ADDR addr;
	PX_UDP *pudp=(PX_UDP *)server->user;
	if(PX_UDPReceived(pudp,&addr,(px_byte *)data,buffersize,readsize))
	{
		PX_memset(port,0,sizeof(PX_Sync_Port));
		port->dw_atom[0]=addr.ipv4;
		port->dw_atom[1]=addr.port;
		return PX_TRUE;
	}
	return PX_FALSE;
}

px_bool game_syncframe_server_write(struct _PX_SyncFrame_Server *server,PX_Sync_Port port,px_void *data,px_int size)
{
	PX_UDP_ADDR toAddr;
	PX_UDP *pudp=(PX_UDP *)server->user;
	toAddr.ipv4=port.dw_atom[0];
	toAddr.port=port.dw_atom[1];
	return PX_UDPSend(pudp,toAddr,(px_byte *)data,size);
}
px_bool game_syncframe_server_read(struct _PX_SyncFrame_Server *server,PX_Sync_Port *port,px_void *data,px_int buffersize,px_int *readsize)
{
	PX_UDP_ADDR addr;
	PX_UDP *pudp=(PX_UDP *)server->user;
	if(PX_UDPReceived(pudp,&addr,(px_byte *)data,buffersize,readsize))
	{
		PX_memset(port,0,sizeof(PX_Sync_Port));
		port->dw_atom[0]=addr.ipv4;
		port->dw_atom[1]=addr.port;
		return PX_TRUE;
	}
	return PX_FALSE;
}

px_void PX_ApplicationUpdateGranularity(PX_Application *App,px_dword elpased)
{
	px_int stampSize=0;
	PX_SyncFrame_InstrStream_StampIndex *pStampIndex=PX_NULL;
	PX_SyncFrame_InstrStream_Stamp *pStamp=PX_NULL;
	px_int syncIndex=0;
	px_int offset=0;

	//update controller
	if (App->syncTime%GAME_SYNC_DURATION==0)
	{
		syncIndex=App->syncTime/GAME_SYNC_DURATION;

		if (syncIndex>=PX_SyncFrameServerGetReadyFrameCount(&App->SyncFrameServer))
		{
			return;
		}

		pStampIndex=PX_VECTORAT(PX_SyncFrame_InstrStream_StampIndex,&App->SyncFrameServer.stampsIndexTable,syncIndex);
		if ((px_int)(pStampIndex->oft+pStampIndex->size)>App->SyncFrameServer.stampsInstrStream.usedsize)
		{
			return;
		}
		stampSize=pStampIndex->size;
		pStamp=(PX_SyncFrame_InstrStream_Stamp *)(App->SyncFrameServer.stampsInstrStream.buffer+pStampIndex->oft);

		while (offset<stampSize)
		{
			Game_Play_Instr *pInstr;
			px_int player,Instr_size,Instr_offset;
			pStamp=(PX_SyncFrame_InstrStream_Stamp *)(App->SyncFrameServer.stampsInstrStream.buffer+pStampIndex->oft+offset);
			player=pStamp->c_id;

			if (player>=PX_COUNTOF(App->play.Players))
			{
				offset+=pStamp->size+PX_STRUCT_OFFSET(PX_SyncFrame_InstrStream_Stamp,data);
				continue;
			}
			Instr_size=pStamp->size;
			Instr_offset=0;
			while (Instr_offset<Instr_size)
			{
				pInstr=(Game_Play_Instr *)(pStamp->data+Instr_offset);
				switch(pInstr->opcode)
				{
				case GAME_PLAY_INSTR_OPCODE_MOVE:
					{
						if ((px_int)(Instr_offset+sizeof(Game_Play_Instr))<=Instr_size)
						{
							PX_Object_Event e;
							PX_memset(&e,0,sizeof(PX_Object_Event));
							e.Event=GAME_OBJECT_EVENT_CONTROLLER;
							e.Param_int[0]=(px_int)pInstr->param0;
							e.Param_int[1]=(px_int)pInstr->param1;
							e.Param_int[2]=(px_int)pInstr->lButtonDown?1:0;
							PX_ObjectPostEvent(App->play.Players[player].pShipObject,e);
						}
						Instr_offset+=sizeof(Game_Play_Instr);
					}
					break;
				case GAME_PLAY_INSTR_OPCODE_SWITCH:
					{
						if ((px_int)(Instr_offset+sizeof(Game_Play_Instr))<=Instr_size)
						{
							PX_Object_Event e;
							PX_memset(&e,0,sizeof(PX_Object_Event));
							e.Event=GAME_OBJECT_EVENT_SWITCH;
							PX_ObjectPostEvent(App->play.Players[player].pShipObject,e);
						}
						Instr_offset+=sizeof(Game_Play_Instr);
					}
					break;
				case GAME_PLAY_INSTR_OPCODE_NAME:
					{
						if ((px_int)(Instr_offset+sizeof(Game_Play_NameInstr))<=Instr_size)
						{
							Game_Play_NameInstr *pNameInstr=(Game_Play_NameInstr *)(pStamp->data+Instr_offset);
							PX_Object_Event e;
							if (pNameInstr->Name[PX_COUNTOF(pNameInstr->Name)-1]==0)
							{
								PX_memset(&e,0,sizeof(PX_Object_Event));
								e.Event=GAME_OBJECT_EVENT_SETNAME;
								e.Param_ptr[0]=pNameInstr->Name;
								PX_ObjectPostEvent(App->play.Players[player].pShipObject,e);
							}
						}
						Instr_offset+=sizeof(Game_Play_NameInstr);
					}
					break;
				}
			}
			offset+=pStamp->size+PX_STRUCT_OFFSET(PX_SyncFrame_InstrStream_Stamp,data);
		}
	}


	//Game Update
	Game_PlayUpdate(&App->play,GAME_UPDATE_TIME);

	App->syncTime+=GAME_UPDATE_TIME;
}

px_bool PX_ApplicationInitialize(PX_Application *App,px_ushort dataport,px_ushort gameport,PX_GameServerStartUp_PlayerNetInfo PlayerNet[],PX_GameStartUp_GameDesc *gameDesc,PX_GameStartUp_PlayerDataDesc PlayerDataDesc[],px_int PlayerCount)
{
	px_int i;

	printf("PainterEngine Game Server v%d\n",GAME_VERSION);
	printf("Code by DBinary(matrixcascade@gmail.com) All Rights Reserved.\n");
	printf("SyncDataPort:%d SyncFramePort:%d\n",dataport,gameport);
	printf("%d Players:\n",PlayerCount);
	for (i=0;i<PlayerCount;i++)
	{
		printf("Player%d: clientID:%ld serverID:%ld\n",i,PlayerNet[i].clientID,PlayerNet[i].serverID);
	}
	printf("Waiting for connection\n");
	App->connectionTimeOutElpased=0;

	//Load resource
	if (!Game_ResourcesLoaderInitialize(&App->Instance,GAME_RESOURCESLOADER_TYPE_SERVER))
	{
		return PX_FALSE;
	}

	//UDP Initialize
	if (!PX_UDPInit(&App->GameUDP,PX_UDP_IP_TYPE_IPV4))
	{
		return PX_FALSE;
	}

	if (!PX_UDPListen(&App->GameUDP,gameport))
	{
		return PX_FALSE;
	}

	if (!PX_UDPInit(&App->DataUDP,PX_UDP_IP_TYPE_IPV4))
	{
		return PX_FALSE;
	}

	if (!PX_UDPListen(&App->DataUDP,dataport))
	{
		return PX_FALSE;
	}

	//SyncFrame
	if(!PX_SyncFrameServerInit(&App->SyncFrameServer,&App->Instance.runtime.mp_game,100,game_syncframe_server_read,game_syncframe_server_write,&App->GameUDP))
	{
		return PX_FALSE;
	}
	PX_SyncFrameServerSetVersion(&App->SyncFrameServer,GAME_VERSION);
	for (i=0;i<PlayerCount;i++)
	{
		PX_SyncFrameServerAddClient(&App->SyncFrameServer,PlayerNet[i].serverID,PlayerNet[i].clientID,i);
	}
	PX_SyncFrameServerRun(&App->SyncFrameServer);
	App->syncTime=0;

	//SyncData
	if (!PX_SyncDataServerInit(&App->SyncDataServer,&App->Instance.runtime.mp_game,0,game_syncdata_server_read,game_syncdata_server_write,&App->DataUDP))
	{
		return PX_FALSE;
	}
	for (i=0;i<PlayerCount;i++)
	{
		PX_SyncDataServerAddClient(&App->SyncDataServer,PlayerNet[i].clientID);
	}
	//read data
	do 
	{
		px_byte *data=(px_byte *)malloc(sizeof(PX_GameStartUp_GameDesc)+PlayerCount*sizeof(PX_GameStartUp_PlayerDataDesc));
		PX_memcpy(data,gameDesc,sizeof(PX_GameStartUp_GameDesc));
		PX_memcpy(data+sizeof(PX_GameStartUp_GameDesc),PlayerDataDesc,PlayerCount*sizeof(PX_GameStartUp_PlayerDataDesc));

		PX_SyncDataServerSetSyncData(&App->SyncDataServer,data,PlayerCount*sizeof(PX_GameStartUp_PlayerDataDesc)+sizeof(PX_GameStartUp_GameDesc));
	} while (0);


	

	//Game
	if (!Game_PlayInitialize(&App->Instance,&App->play))
	{
		return PX_FALSE;
	}

	Game_PlayStart(&App->play,(GAME_PLAY_GAMETYPE)gameDesc->mode,PlayerDataDesc,PlayerCount);

	App->status=PX_APPLICATION_STATUS_CONNECTING;
	return PX_TRUE;
}

px_void PX_ApplicationUpdate(PX_Application *App,px_dword elpased)
{
	px_int i;
	PX_SyncDataServerUpdate(&App->SyncDataServer,elpased);
	PX_SyncFrameServerUpdate(&App->SyncFrameServer,elpased);

	if (App->SyncFrameServer.status==PX_SYNC_SERVER_STATUS_PROCESSING)
	{
		if (App->status==PX_APPLICATION_STATUS_CONNECTING)
		{
			App->status=PX_APPLICATION_STATUS_GAMMING;
			printf("Game Running\n");
		}
		PX_ApplicationUpdateGranularity(App,elpased);
		if (Game_PlayIsGameOver(&App->play))
		{
			App->gameovrtelpased+=elpased;
			if (App->gameovrtelpased>PX_APPLICATION_GAMEOVER_DELAY)
			{
				//result
				for (i=0;i<App->play.PlayerCount;i++)
				{
					Game_Object_Ship *pShip;
					PX_GameServer_PlayerResult result;
					pShip=Game_Object_GetShip(App->play.Players[i].pShipObject);
					if (pShip)
					{
						PX_UDP_ADDR addr;
						PX_memset(&result,0,sizeof(result));
						PX_strset(result.account,App->play.Players[i].account);
						result.gameType=App->play.GameType;
						result.group=pShip->group;
						result.rank=(px_dword)Game_PlayGetPlayerRank(&App->play,i);
						result.score=(px_dword)App->play.Players[i].Score;
						
						addr.ipv4=PX_inet_addr("127.0.0.1");
						addr.port=PAINTERENGINE_GAMESERVER_RESULT_PORT;

						PX_UDPSend(&App->GameUDP,addr,&result,sizeof(result));

					}
					
				}


				exit(0);
			}
		}
		else
		{
			px_int i;
			px_bool alloffline=PX_TRUE;
			
			//all offline
			for (i=0;i<App->SyncFrameServer.clients.size;i++)
			{
				PX_SyncFrame_Server_Clients *pClient=PX_VECTORAT(PX_SyncFrame_Server_Clients,&App->SyncFrameServer.clients,i);
				if (pClient->lastInstrElpased<6000)
				{
					alloffline=PX_FALSE;
					break;
				}
			}
			if (alloffline)
			{
				exit(0);
			}
		}
	}
	else if(App->SyncFrameServer.status==PX_SYNC_SERVER_STATUS_CONNECT)
	{
		App->connectionTimeOutElpased+=elpased;
		if (App->connectionTimeOutElpased>=PX_APPLICATION_CONNECTIONTIMEOUT_DELAY)
		{
			//connection timeout
			printf("Connection Timeout.\n");
			Sleep(1000);
			exit(0);
		}
	}
	
}

px_void PX_ApplicationRender(PX_Application *App,px_dword elpased)
{

}

px_void PX_ApplicationPostEvent(PX_Application *App,PX_Object_Event e)
{
	
}

