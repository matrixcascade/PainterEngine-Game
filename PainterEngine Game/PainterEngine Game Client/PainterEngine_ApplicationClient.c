#include "PainterEngine_ApplicationClient.h"

PX_Application App;

px_bool game_syncdata_client_write(struct _PX_SyncData_Client *client,PX_Sync_Port port,px_void *data,px_int size)
{
	PX_UDP_ADDR toAddr;
	PX_UDP *pudp=(PX_UDP *)client->user;
	toAddr.ipv4=port.dw_atom[0];
	toAddr.port=port.dw_atom[1];
	return PX_UDPSend(pudp,toAddr,(px_byte *)data,size);
}

px_bool game_syncdata_client_read(struct _PX_SyncData_Client *client,PX_Sync_Port *port,px_void *data,px_int buffersize,px_int *readsize)
{
	PX_UDP_ADDR addr;
	PX_UDP *pudp=(PX_UDP *)client->user;
	if(PX_UDPReceived(pudp,&addr,(px_byte *)data,buffersize,readsize))
	{
		PX_memset(port,0,sizeof(PX_Sync_Port));
		port->dw_atom[0]=addr.ipv4;
		port->dw_atom[1]=addr.port;
		return PX_TRUE;
	}
	return PX_FALSE;
}


px_bool game_syncframe_client_write(struct _PX_SyncFrame_Client *client,PX_Sync_Port port,px_void *data,px_int size)
{
	PX_UDP_ADDR toAddr;
	PX_UDP *pudp=(PX_UDP *)client->user;
	toAddr.ipv4=port.dw_atom[0];
	toAddr.port=port.dw_atom[1];
	return PX_UDPSend(pudp,toAddr,(px_byte *)data,size);
}
px_bool game_syncframe_client_read(struct _PX_SyncFrame_Client *client,PX_Sync_Port *port,px_void *data,px_int buffersize,px_int *readsize)
{
	PX_UDP_ADDR addr;
	PX_UDP *pudp=(PX_UDP *)client->user;
	if(PX_UDPReceived(pudp,&addr,(px_byte *)data,buffersize,readsize))
	{
		PX_memset(port,0,sizeof(PX_Sync_Port));
		port->dw_atom[0]=addr.ipv4;
		port->dw_atom[1]=addr.port;
		return PX_TRUE;
	}
	return PX_FALSE;
}
px_void PX_ApplicationUpdateController(PX_Application *App,px_dword elpased)
{
	POINT cursorPt=PX_MousePosition();

	if (cursorPt.x<0)
	{
		cursorPt.x=0;
	}
	if (cursorPt.y<0)
	{
		cursorPt.y=0;
	}

	if (cursorPt.x>App->Instance.runtime.width-1)
	{
		cursorPt.x=App->Instance.runtime.width-1;
	}

	if (cursorPt.y>App->Instance.runtime.height-1)
	{
		cursorPt.y=App->Instance.runtime.height-1;
	}

	App->Instance.Controller.Cursor=PX_POINT((px_float)cursorPt.x,(px_float)cursorPt.y,0);
	App->Instance.Controller.LeftButtonDown=PX_MouseLButtonDown();
	App->Instance.Controller.RightButtonDown=PX_MouseRButtonDown();
}

px_bool PX_ApplicationInitialize(PX_Application *App,const px_char ServerIpAddr[],px_ushort dataPort,px_ushort syncPort,px_dword client_id,px_dword server_id)
{
	PX_Sync_Port syncport;

	printf("PainterEngine Game Server v%d\n",GAME_VERSION);
	printf("Code by DBinary(matrixcascade@gmail.com) All Rights Reserved.\n");
	printf("SyncDataPort:%d SyncFramePort:%d\n",dataPort,syncPort);
	printf("ClientID:%d ServerID:%d\n",client_id,server_id);
	App->gameTime=0;
	App->syncTime=0;
	App->controllerSyncTime=GAME_SYNC_DURATION;
	App->remianUpdateTime=0;
	App->LastCursorInstr.opcode=GAME_PLAY_INSTR_OPCODE_MOVE;
	App->LastCursorInstr.param0=GAME_WORLD_WIDTH/2;
	App->LastCursorInstr.param1=GAME_WORLD_HEIGHT/2;
	App->LastCursorInstr.lButtonDown=PX_FALSE;
	App->status=PX_APPLICATION_STATUS_DOWNLOADING;
	App->ObMonitorPlayer=0;
	App->avg_fps=0;
	App->sum_fps=0;
	App->fps_elpased=0;
	App->connectionTimeOutElpased=0;
	App->gameOverElpased=0;
	App->enterGameOverElpased=0;

	
	PX_memset(&App->LastCursorInstr,0,sizeof(Game_Play_Instr));

	if (!Game_ResourcesLoaderInitialize(&App->Instance,GAME_RESOURCESLOADER_TYPE_CLIENT))
	{
		return PX_FALSE;
	}
	
	if (!PX_MessageBoxInitialize(&App->Instance.runtime,&App->messagebox,&App->Instance.FontModule32,App->Instance.runtime.width,App->Instance.runtime.height))
	{
		return PX_FALSE;
	}

	if (!PX_MessageBoxInitialize(&App->Instance.runtime,&App->SyncMessageBox,&App->Instance.FontModule32,App->Instance.runtime.width,App->Instance.runtime.height))
	{
		return PX_FALSE;
	}

	if (!Game_PlayInitialize(&App->Instance,&App->play))
	{
		return PX_FALSE;
	}

	if (!Game_UIInitialize(&App->Instance,&App->ui,&App->play))
	{
		return PX_FALSE;
	}

	if (!PX_UDPInit(&App->udp,PX_UDP_IP_TYPE_IPV4))
	{
		return PX_FALSE;
	}

	PX_memset(&syncport,0,sizeof(PX_Sync_Port));
	syncport.dw_atom[0]=PX_inet_addr(ServerIpAddr);
	syncport.dw_atom[1]=dataPort;
	if (!PX_SyncDataClientInit(&App->SyncDataClient,&App->Instance.runtime.mp_game,0,client_id,syncport,game_syncdata_client_read,game_syncdata_client_write,&App->udp))
	{
		return PX_FALSE;
	}


	PX_memset(&syncport,0,sizeof(PX_Sync_Port));
	syncport.dw_atom[0]=PX_inet_addr(ServerIpAddr);
	syncport.dw_atom[1]=syncPort;

	if (!PX_SyncFrameClientInit(&App->SyncFrameClient,&App->Instance.runtime.mp_game,GAME_SYNC_DURATION,syncport,server_id,client_id,game_syncframe_client_read,game_syncframe_client_write,&App->udp))
	{
		return PX_FALSE;
	}
	PX_SyncFrameClientSetVersion(&App->SyncFrameClient,GAME_VERSION);

	

	return PX_TRUE;
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
		
		if (syncIndex>=PX_SyncFrameClientGetReadyFrameCount(&App->SyncFrameClient))
		{
			return;
		}

		pStampIndex=PX_VECTORAT(PX_SyncFrame_InstrStream_StampIndex,&App->SyncFrameClient.stampsIndexTable,syncIndex);
		if ((px_int)(pStampIndex->oft+pStampIndex->size)>App->SyncFrameClient.stampsInstrStream.usedsize)
		{
			return;
		}
		stampSize=pStampIndex->size;
		pStamp=(PX_SyncFrame_InstrStream_Stamp *)(App->SyncFrameClient.stampsInstrStream.buffer+pStampIndex->oft);

		while (offset<stampSize)
		{
			Game_Play_Instr *pInstr;
			px_int player,Instr_size,Instr_offset;
			pStamp=(PX_SyncFrame_InstrStream_Stamp *)(App->SyncFrameClient.stampsInstrStream.buffer+pStampIndex->oft+offset);
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

px_void PX_ApplicationGameStart(PX_Application *App)
{
	PX_GameStartUp_GameDesc *pGameDesc;
	PX_GameStartUp_PlayerDataDesc *pPlayerDesc;
	px_int PlayerCount;
	px_int localPlayer;

	if (App->status==PX_APPLICATION_STATUS_GAMING)
	{
		return;
	}
	App->status=PX_APPLICATION_STATUS_GAMING;
	PX_PlayGlobalSoundFromResource(&App->Instance,GAME_TEXT_KEY_SND_BGM,PX_TRUE);
	PX_MessageBoxClose(&App->messagebox);

	//PX_GameStartUp_GameDesc *1
	//PX_GameStartUp_PlayerDataDesc *n
	pGameDesc=(PX_GameStartUp_GameDesc *)(App->SyncDataClient.data);
	PlayerCount=(App->SyncDataClient.datasize-sizeof(PX_GameStartUp_GameDesc))/sizeof(PX_GameStartUp_PlayerDataDesc);
	pPlayerDesc=(PX_GameStartUp_PlayerDataDesc *)(App->SyncDataClient.data+sizeof(PX_GameStartUp_GameDesc));

	Game_PlayStart(&App->play,(GAME_PLAY_GAMETYPE)pGameDesc->mode,pPlayerDesc,PlayerCount);
	
	//Set LocalShip
	localPlayer=App->SyncFrameClient.c_id;
	if (App->play.Players[localPlayer].pShipObject)
	{
		Game_Object_GetShip(App->play.Players[localPlayer].pShipObject)->bLocalController=PX_TRUE;
	}
	
}

px_void PX_ApplicationUpdateGameOver(PX_Application *App,px_dword elpased)
{
	if (!App->messagebox.show)
	{
		App->gameOverElpased+=elpased;
		if (App->gameOverElpased>5000)
		{
			PX_ApplicationSaveReplay(App);
			exit(0);
		}
	}
	
}

px_void PX_ApplicationUpdateDownloading(PX_Application *App,px_dword elpased)
{
	static px_dword aniElpased;
	static px_word wprocContent[32]={0};
	static px_char procContent[16];
	const px_char *Content[]={(const char *)L"数据同步中.",(const char *)L"数据同步中..",(const char *)L"数据同步中..."};

	if (!App->messagebox.show)
	{
		PX_MessageBoxAlert(&App->messagebox,Content[0]);
	}

	aniElpased+=elpased;
	if (aniElpased>=1500)
	{
		aniElpased=0;
	}

	wprocContent[0]='\0';
	PX_sprintf1(procContent,sizeof(procContent),"%1.2%",PX_STRINGFORMAT_FLOAT(App->SyncDataClient.acceptBlock*100.0f/(App->SyncDataClient.blockCount?App->SyncDataClient.blockCount:1)));
	PX_wstrcat(wprocContent,(const px_word *)Content[aniElpased/500]);
	PX_FontModule_wastrcat(wprocContent,procContent);

	App->messagebox.Message=(const px_char *)wprocContent;

	PX_MessageBoxUpdate(&App->messagebox,elpased);
	PX_SyncDataClientUpdate(&App->SyncDataClient,elpased);

	if (PX_SyncDataClientIsCompleted(&App->SyncDataClient))
	{
		App->status=PX_APPLICATION_STATUS_CONNECTING;
	}
}


px_void PX_ApplicationUpdateConnecting(PX_Application *App,px_dword elpased)
{
	static px_dword aniElpased;

	const px_char *Content[]={(const char *)L"连接中.",(const char *)L"连接中..",(const char *)L"连接中..."};

	
	if (!App->messagebox.show)
	{
		PX_MessageBoxAlert(&App->messagebox,Content[0]);
	}

	
	aniElpased+=elpased;
	if (aniElpased>=1500)
	{
		aniElpased=0;
	}
	

	App->messagebox.Message=Content[aniElpased/500];
	
	PX_MessageBoxUpdate(&App->messagebox,elpased);
	PX_SyncFrameClientUpdate(&App->SyncFrameClient,elpased);

	switch (App->SyncFrameClient.status)
	{
	case PX_SYNC_CLIENT_STATUS_WAITING:
		{
			App->status=PX_APPLICATION_STATUS_WAITING;
		}
		break;
	case PX_SYNC_CLIENT_STATUS_PROCESSING:
		{
			PX_ApplicationGameStart(App);
		}
		break;
	}
}

px_void PX_ApplicationUpdateWaiting(PX_Application *App,px_dword elpased)
{
	static px_dword aniElpased;
	static px_word wprocContent[32]={0};
	static px_char procContent[16];
	const px_char *Content[]={(const char *)L"等待其它玩家.",(const char *)L"等待其它玩家..",(const char *)L"等待其它玩家..."};
	aniElpased+=elpased;
	if (aniElpased>=1500)
	{
		aniElpased=0;
	}

	wprocContent[0]='\0';
	PX_sprintf2(procContent,sizeof(procContent),"%1/%2",PX_STRINGFORMAT_INT(App->SyncFrameClient.connectCount),PX_STRINGFORMAT_INT(App->SyncFrameClient.connectSumCount));
	PX_wstrcat(wprocContent,(const px_word *)Content[aniElpased/500]);
	PX_FontModule_wastrcat(wprocContent,procContent);

	App->messagebox.Message=(const px_char *)wprocContent;

	PX_MessageBoxUpdate(&App->messagebox,elpased);
	PX_SyncFrameClientUpdate(&App->SyncFrameClient,elpased);

	switch (App->SyncFrameClient.status)
	{
	case PX_SYNC_CLIENT_STATUS_PROCESSING:
		{
			PX_ApplicationGameStart(App);
		}
		break;
	}
}
px_void PX_ApplicationUpdateReSync(PX_Application *App,px_dword elpased)
{
	static px_char Numeric[16]={0};
	static px_word Contentw[32]={0};
	px_dword serverSyncTime=PX_SyncFrameClientGetReadyFrameCount(&App->SyncFrameClient)*GAME_SYNC_DURATION;

	if (serverSyncTime>2000&&App->syncTime<serverSyncTime-GAME_SYNC_DELAY*10)
	{
		if (!App->SyncMessageBox.show)
		{
			PX_MessageBoxAlert(&App->SyncMessageBox,(const px_char *)Contentw);
		}
		Contentw[0]=0;
		PX_wstrcat(Contentw,(px_word *)L"同步中(");
		PX_ftoa(App->syncTime*100.0f/(serverSyncTime),Numeric,sizeof(Numeric),2);
		PX_FontModule_wastrcat(Contentw,Numeric);
		PX_wstrcat(Contentw,(px_word *)L"%)");
	}
	else
	{
		if (App->SyncMessageBox.show)
		{
			PX_MessageBoxClose(&App->SyncMessageBox);
		}
	}
	


}
px_void PX_ApplicationUpdateGaming(PX_Application *App,px_dword elpased)
{
	PX_MessageBoxUpdate(&App->messagebox,elpased);
	PX_MessageBoxUpdate(&App->SyncMessageBox,elpased);

	//Update Local Controller
	PX_ApplicationUpdateController(App,elpased);

	//Game Over Check
	if (Game_PlayIsGameOver(&App->play)&&App->play.GameOverWaitElpased>=GAME_PLAY_GAME_WAIT_ELPASED)
	{
		if (App->enterGameOverElpased<5000)
		{
			App->enterGameOverElpased+=elpased;
		}
		else
		{
			App->status=PX_APPLICATION_STATUS_GAMEOVER;
		}
	}
	else
	{
		px_dword serverSyncTime;
		px_dword maxUpdateTimes=GAME_SYNC_RECONNECT_XSPEED;

		//////////////////////////////////////////////////////////////////////////
		//real time
		App->gameTime+=elpased;


		//////////////////////////////////////////////////////////////////////////
		//Synchronization
		//////////////////////////////////////////////////////////////////////////
		//remote
		PX_SyncFrameClientUpdate(&App->SyncFrameClient,elpased);

		PX_ApplicationUpdateReSync(App,elpased);

		if (App->controllerSyncTime<GAME_SYNC_DURATION)
		{
			App->LastCursorInstr.param0=(px_short)(App->Instance.Controller.Cursor.x+App->play.world.offsetx);
			App->LastCursorInstr.param1=(px_short)(App->Instance.Controller.Cursor.y+App->play.world.offsety);
			if (App->Instance.Controller.LeftButtonDown)
			{
				App->LastCursorInstr.lButtonDown=PX_TRUE;
			}

			App->controllerSyncTime+=elpased;
		}
		else
		{
			PX_SyncFrameClientAddInstr(&App->SyncFrameClient,&App->LastCursorInstr,sizeof(Game_Play_Instr));
			App->LastCursorInstr.lButtonDown=PX_FALSE;
			App->controllerSyncTime%=GAME_SYNC_DURATION;
		}

		serverSyncTime=PX_SyncFrameClientGetReadyFrameCount(&App->SyncFrameClient)*GAME_SYNC_DURATION;
		//accelerate update for synchronization
		while (serverSyncTime>GAME_SYNC_DELAY&&App->syncTime<serverSyncTime-GAME_SYNC_DELAY)
		{
			PX_ApplicationUpdateGranularity(App,elpased);
			if (maxUpdateTimes>0)
			{
				maxUpdateTimes--;
			}
			else
			{
				break;
			}
			App->remianUpdateTime=0;
		}

		//normal Update
		App->remianUpdateTime+=elpased;
		while (App->remianUpdateTime>=GAME_UPDATE_TIME)
		{
			PX_ApplicationUpdateGranularity(App,elpased);
			App->remianUpdateTime-=GAME_UPDATE_TIME;
		}
	}
}
px_void PX_ApplicationUpdate(PX_Application *App,px_dword elpased)
{

	switch(App->status)
	{
	case PX_APPLICATION_STATUS_DOWNLOADING:
		{
			App->connectionTimeOutElpased+=elpased;
			PX_ApplicationUpdateDownloading(App,elpased);
		}
		break;
	case PX_APPLICATION_STATUS_CONNECTING:
		{
			App->connectionTimeOutElpased+=elpased;
			PX_ApplicationUpdateConnecting(App,elpased);
		}
		break;
	case PX_APPLICATION_STATUS_WAITING:
		{
			App->connectionTimeOutElpased+=elpased;
			PX_ApplicationUpdateWaiting(App,elpased);
		}
		break;
	case PX_APPLICATION_STATUS_GAMING:
		{
			PX_ApplicationUpdateGaming(App,elpased);
		}
		break;
	case PX_APPLICATION_STATUS_GAMEOVER:
		{
			PX_ApplicationUpdateGameOver(App,elpased);
		}
		break;
	}

	if (App->connectionTimeOutElpased>=PX_APPLICATION_CONNECTIONTIMEOUT_DELAY)
	{
		exit(0);
	}
}



px_void PX_ApplicationGamingRender(px_surface *renderSurface,PX_Application *App,px_dword elpased)
{
	//////////////////////////////////////////////////////////////////////////
	//Game Over
	if (Game_PlayIsGameOver(&App->play)&&App->play.GameOverWaitElpased>=GAME_PLAY_GAME_WAIT_ELPASED)
	{
		//Game Render
		Game_PlayRender(renderSurface,&App->play,elpased);
		PX_FontModuleDrawText(renderSurface,App->Instance.runtime.width/2,128,(px_word *)L"游戏即将结束",PX_COLOR(255,255,0,0),&App->Instance.FontModule32,PX_FONT_ALIGN_XCENTER);
	}
	else
	{
		px_int MonitorPlayer,LocalPlayer;
		PX_Object *pShipObject;

		//////////////////////////////////////////////////////////////////////////

		LocalPlayer=App->SyncFrameClient.c_id;

		//Local Data

		if (!Game_PlayIsPlayerAlive(&App->play,LocalPlayer))
		{
			MonitorPlayer=App->ObMonitorPlayer;
		}
		else
		{
			MonitorPlayer=LocalPlayer;
		}

		pShipObject=App->play.Players[MonitorPlayer].pShipObject;

		if (pShipObject)
		{
			//game player camera
			if (pShipObject->Enabled)
			{
				PX_WorldSetCamera(&App->play.world,PX_POINT(pShipObject->x,pShipObject->y,0));
			}
		}

		//Game Render
		Game_PlayRender(renderSurface,&App->play,elpased);

		//local UI
		App->fps_elpased+=elpased;
		App->sum_fps++;
		if (App->fps_elpased>2000)
		{
			App->avg_fps=App->sum_fps/2;
			App->sum_fps=0;
			App->fps_elpased=0;
		}

		Game_UIRender(renderSurface,&App->ui,MonitorPlayer,App->avg_fps,elpased);
	}

}


px_void PX_ApplicationGameOverRender(px_surface *renderSurface,PX_Application *App,px_dword elpased)
{
	px_int rank=Game_PlayGetPlayerRank(&App->play,App->SyncFrameClient.c_id);
	static const px_word *rankContent[]={\
		(const px_word *)L"DBinary(matrixcascade@gmail.com) All Rights Reserved",\
		(const px_word *)L"第一名 最强老司机!",\
		(const px_word *)L"第二名 老司机!",\
		(const px_word *)L"第三名 大佬!",\
		(const px_word *)L"第四名 旗鼓相当的对手",\
		(const px_word *)L"第五名 弟弟",\
		(const px_word *)L"第六名 菜鸡",\
		(const px_word *)L"第七名 弱鸡",\
		(const px_word *)L"第八名 渣渣",\
		(const px_word *)L"第九名",\
		(const px_word *)L"第十名",\
		(const px_word *)L"第十一名",\
		(const px_word *)L"第十二名",\
		(const px_word *)L"第十三名",\
		(const px_word *)L"第十四名",\
		(const px_word *)L"第十五名",\
		(const px_word *)L"第十六名",\
	};

	if (rank>0&&rank<PX_COUNTOF(rankContent))
	{
		if (App->gameOverElpased<1500)
		{
			PX_FontModuleDrawText(renderSurface,App->Instance.runtime.width/2,App->Instance.runtime.height/2,rankContent[rank],PX_COLOR((px_uchar)(App->gameOverElpased/1500.f*255),0,0,0),\
				&App->Instance.FontModule32,PX_FONT_ALIGN_XCENTER);
		}
		else
		{
			PX_FontModuleDrawText(renderSurface,App->Instance.runtime.width/2,App->Instance.runtime.height/2,rankContent[rank],PX_COLOR(255,0,0,0),\
				&App->Instance.FontModule32,PX_FONT_ALIGN_XCENTER);
		}
	}
	else
	{
		if (App->gameOverElpased<1500)
		{
			PX_FontModuleDrawText(renderSurface,App->Instance.runtime.width/2,App->Instance.runtime.height/2,(px_word *)L"Game Over",PX_COLOR((px_uchar)(App->gameOverElpased/1500.f*255),0,0,0),\
				&App->Instance.FontModule32,PX_FONT_ALIGN_XCENTER);
		}
		else
		{
			PX_FontModuleDrawText(renderSurface,App->Instance.runtime.width/2,App->Instance.runtime.height/2,(px_word *)L"Game Over",PX_COLOR(255,0,0,0),\
				&App->Instance.FontModule32,PX_FONT_ALIGN_XCENTER);
		}
	}

	
}

px_void PX_ApplicationRender(PX_Application *App,px_dword elpased)
{
	px_surface *renderSurface=&App->Instance.runtime.RenderSurface;
	PX_RuntimeRenderClear(&App->Instance.runtime,PX_COLOR(255,255,255,255));

	//////////////////////////////////////////////////////////////////////////
	//messagebox
	if (App->messagebox.show)
	{
		PX_MessageBoxRender(renderSurface,&App->messagebox,elpased);
		return;
	}

	if (App->SyncMessageBox.show)
	{
		PX_MessageBoxRender(renderSurface,&App->SyncMessageBox,elpased);
		return;
	}

	switch(App->status)
	{
	case PX_APPLICATION_STATUS_GAMING:
		PX_ApplicationGamingRender(renderSurface,App,elpased);
		break;
	case PX_APPLICATION_STATUS_GAMEOVER:
		PX_ApplicationGameOverRender(renderSurface,App,elpased);
		Game_UI_RankPanelRender(renderSurface,&App->ui.uiRankPanel,elpased);
		break;
	}
	
}

px_void PX_ApplicationPostEvent(PX_Application *App,PX_Object_Event e)
{
	px_int LocalPlayer=App->SyncFrameClient.c_id;

	if (App->messagebox.show)
	{
		PX_MessageBoxPostEvent(&App->messagebox,e);
		return;
	}

	if (App->SyncMessageBox.show)
	{
		PX_MessageBoxPostEvent(&App->SyncMessageBox,e);
		return;
	}

	if (App->status==PX_APPLICATION_STATUS_GAMING)
	{
		if (Game_PlayIsPlayerAlive(&App->play,LocalPlayer))
		{
			if (e.Event==PX_OBJECT_EVENT_CURSORRDOWN)
			{
				Game_Play_Instr Instr;
				Instr.opcode=GAME_PLAY_INSTR_OPCODE_SWITCH;
				Instr.param0=0;
				Instr.param1=0;
				Instr.param2=0;
				PX_SyncFrameClientAddInstr(&App->SyncFrameClient,&Instr,sizeof(Game_Play_Instr));
			}
		}
		else
		{
			px_int i;
			if (e.Event==PX_OBJECT_EVENT_CURSORDOWN)
			{
				for (i=0;i<App->play.PlayerCount+1;i++)
				{
					App->ObMonitorPlayer++;
					if (App->ObMonitorPlayer>=App->play.PlayerCount)
					{
						App->ObMonitorPlayer=0;
					}
					if (Game_PlayIsPlayerAlive(&App->play,App->ObMonitorPlayer))
					{
						break;
					}
				}
			}
			
		}


		Game_UIPostEvent(&App->ui,e);
	}
}



px_void PX_ApplicationSaveReplay(PX_Application *App)
{
	px_char *path="./Game.replay";//=PX_SaveFileDialog("游戏录像文件(Replay File)\0*.reply\0\0",".replay");
	if (path)
	{
		FILE *pf;
		px_dword magicnumber=477608346;
		px_dword indexsize;
		px_dword streamsize;
		indexsize=App->SyncFrameClient.stampsIndexTable.size;
		streamsize=App->SyncFrameClient.stampsInstrStream.usedsize;
		pf=fopen(path,"wb");
		if (pf)
		{
			fwrite(&magicnumber,1,sizeof(px_dword),pf);
			fwrite(&indexsize,1,sizeof(px_dword),pf);
			fwrite(&streamsize,1,sizeof(px_dword),pf);
			fwrite(App->SyncFrameClient.stampsIndexTable.data,1,indexsize*indexsize,pf);
			fwrite(App->SyncFrameClient.stampsInstrStream.buffer,1,streamsize,pf);
			fclose(pf);
		}
	}
}
