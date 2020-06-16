#include "PainterEngine_Lobby.h"
extern px_void PX_ApplicationMessageBoxAlertYesNo(const px_char content[],PX_MessageBoxCallBack yescallback,px_void *yesptr,PX_MessageBoxCallBack nocallback,px_void *noptr);
extern px_void PX_ApplicationMessageBoxAlertOk(const px_char content[]);
DWORD WINAPI PX_LobbyGameStartThread(px_void *ptr)
{
	px_char currentDir[MAX_PATH];
	px_dword processRet;
	PX_Lobby *pDesc=(PX_Lobby *)ptr;
	PX_GameClientStartUp startup_param;
	STARTUPINFO sti;
	PROCESS_INFORMATION proci;
	FILE *pf;
	memset(&sti,0,sizeof(STARTUPINFO));
	memset(&proci,0,sizeof(PROCESS_INFORMATION));
	sti.cb=sizeof(STARTUPINFO);

	

	PX_strset(startup_param.IpAddr,pDesc->serverIpAddr);
	startup_param.clientID=pDesc->userState.clientid;
	startup_param.serverID=pDesc->userState.serverid;
	startup_param.syncFramePort=pDesc->userState.game_port;
	startup_param.syncDataPort=pDesc->userState.data_port;

	pf=fopen("./bin/startup","wb");
	if (!pf)
	{
		MessageBox(NULL,"启动配置失败,请重新启动游戏","Error",MB_OK);
		exit(0);
	}
	fwrite(&startup_param,1,sizeof(startup_param),pf);
	fclose(pf);
	GetCurrentDirectory(sizeof(currentDir),currentDir);
	PX_strcat(currentDir,"/bin");
	processRet=CreateProcess("./bin/PainterEngine Game Client.exe","game ./startup",PX_NULL,PX_NULL,FALSE,PX_NULL,PX_NULL,currentDir,&sti,&proci);
	if (processRet)
	{
		CloseHandle(proci.hThread);
		WaitForSingleObject(proci.hProcess,INFINITE);
		pDesc->gameHandle=PX_NULL;
	}
	else
	{
		pDesc->gameHandle=PX_NULL;
		MessageBox(NULL,"严重错误:游戏文件缺失,请重新下载游戏!","Error",MB_OK);
		exit(0);
	}
	return 0;
}


px_void PX_LobbyBuyTicket(px_void *ptr)
{
	PX_Lobby_Packet_Command cmdPack;
	PX_AES aes;
	PX_Lobby *pDesc=(PX_Lobby *)ptr;

	if ((px_int)pDesc->userInfo.coin<1000)
	{
		PX_ApplicationMessageBoxAlertOk((px_char *)L"所需的金币不足");
		return;
	}


	pDesc->userinfo_elpased=PX_LOBBY_UPDATE_USERINFO_TIME-200;

	PX_memset(&cmdPack,0,sizeof(cmdPack));

	cmdPack.header.magic=PX_NETWORK_LOBBY_MAGIC;
	cmdPack.header.id=pDesc->id;
	cmdPack.header.mapid=pDesc->map_id;
	cmdPack.header.type=PX_NETWORK_LOBBY_TYPE_BUY_TICKET;

	PX_memcpy(cmdPack.content.cookie,pDesc->cookie,sizeof(pDesc->cookie));
	PX_AES_Initialize(&aes,PX_AES_KeySize_Bits256,pDesc->aes_code);
	PX_AES_CipherBuffer(&aes,(px_byte *)&cmdPack.content,sizeof(cmdPack.content),(px_byte *)&cmdPack.content);

	PX_UDPSend(&pDesc->udp,pDesc->serverAddr,(px_byte *)&cmdPack,sizeof(cmdPack));
}

px_void PX_LobbyUpgrade_AmmoGen(px_void *ptr)
{
	PX_Lobby_Packet_Command cmdPack;
	PX_AES aes;
	PX_Lobby *pDesc=(PX_Lobby *)ptr;
	px_int level=pDesc->userInfo.ShipStack[pDesc->userState.shipIndex].upgrade_ammo_generator;
	px_int pay=100*(1<<level);

	if (level>=10)
	{
		PX_ApplicationMessageBoxAlertOk((px_char *)L"等级已达到最大值");
		return;
	}

	if ((px_int)pDesc->userInfo.coin<pay)
	{
		PX_ApplicationMessageBoxAlertOk((px_char *)L"所需的金币不足");
		return;
	}

	
	pDesc->userinfo_elpased=PX_LOBBY_UPDATE_USERINFO_TIME;

	PX_memset(&cmdPack,0,sizeof(cmdPack));

	cmdPack.header.magic=PX_NETWORK_LOBBY_MAGIC;
	cmdPack.header.id=pDesc->id;
	cmdPack.header.mapid=pDesc->map_id;
	cmdPack.header.type=PX_NETWORK_LOBBY_TYPE_UPGRADE_AMMOGEN;

	PX_memcpy(cmdPack.content.cookie,pDesc->cookie,sizeof(pDesc->cookie));
	PX_AES_Initialize(&aes,PX_AES_KeySize_Bits256,pDesc->aes_code);
	PX_AES_CipherBuffer(&aes,(px_byte *)&cmdPack.content,sizeof(cmdPack.content),(px_byte *)&cmdPack.content);

	PX_UDPSend(&pDesc->udp,pDesc->serverAddr,(px_byte *)&cmdPack,sizeof(cmdPack));
}

px_void PX_LobbyUpgrade_AmmoRecovery(px_void *ptr)
{
	PX_Lobby_Packet_Command cmdPack;
	PX_AES aes;
	PX_Lobby *pDesc=(PX_Lobby *)ptr;
	px_int level=pDesc->userInfo.ShipStack[pDesc->userState.shipIndex].upgrade_lastfirerecovery;
	px_int pay=100*(1<<level);

	if (level>=10)
	{
		PX_ApplicationMessageBoxAlertOk((px_char *)L"等级已达到最大值");
		return;
	}

	if ((px_int)pDesc->userInfo.coin<pay)
	{
		PX_ApplicationMessageBoxAlertOk((px_char *)L"所需的金币不足");
		return;
	}
	pDesc->userinfo_elpased=PX_LOBBY_UPDATE_USERINFO_TIME;

	PX_memset(&cmdPack,0,sizeof(cmdPack));

	cmdPack.header.magic=PX_NETWORK_LOBBY_MAGIC;
	cmdPack.header.id=pDesc->id;
	cmdPack.header.mapid=pDesc->map_id;
	cmdPack.header.type=PX_NETWORK_LOBBY_TYPE_UPGRADE_AMMOREC;

	PX_memcpy(cmdPack.content.cookie,pDesc->cookie,sizeof(pDesc->cookie));
	PX_AES_Initialize(&aes,PX_AES_KeySize_Bits256,pDesc->aes_code);
	PX_AES_CipherBuffer(&aes,(px_byte *)&cmdPack.content,sizeof(cmdPack.content),(px_byte *)&cmdPack.content);

	PX_UDPSend(&pDesc->udp,pDesc->serverAddr,(px_byte *)&cmdPack,sizeof(cmdPack));
}

px_void PX_LobbyUpgrade_force(px_void *ptr)
{
	PX_Lobby_Packet_Command cmdPack;
	PX_AES aes;
	PX_Lobby *pDesc=(PX_Lobby *)ptr;
	px_int level=pDesc->userInfo.ShipStack[pDesc->userState.shipIndex].upgrade_force;
	px_int pay=100*(1<<level);

	if (level>=10)
	{
		PX_ApplicationMessageBoxAlertOk((px_char *)L"等级已达到最大值");
		return;
	}

	if ((px_int)pDesc->userInfo.coin<pay)
	{
		PX_ApplicationMessageBoxAlertOk((px_char *)L"所需的金币不足");
		return;
	}
	pDesc->userinfo_elpased=PX_LOBBY_UPDATE_USERINFO_TIME;

	PX_memset(&cmdPack,0,sizeof(cmdPack));

	cmdPack.header.magic=PX_NETWORK_LOBBY_MAGIC;
	cmdPack.header.id=pDesc->id;
	cmdPack.header.mapid=pDesc->map_id;
	cmdPack.header.type=PX_NETWORK_LOBBY_TYPE_UPGRADE_MAXFORCE;

	PX_memcpy(cmdPack.content.cookie,pDesc->cookie,sizeof(pDesc->cookie));
	PX_AES_Initialize(&aes,PX_AES_KeySize_Bits256,pDesc->aes_code);
	PX_AES_CipherBuffer(&aes,(px_byte *)&cmdPack.content,sizeof(cmdPack.content),(px_byte *)&cmdPack.content);

	PX_UDPSend(&pDesc->udp,pDesc->serverAddr,(px_byte *)&cmdPack,sizeof(cmdPack));
}

px_void PX_LobbyUpgrade_speed(px_void *ptr)
{
	PX_Lobby_Packet_Command cmdPack;
	PX_AES aes;
	PX_Lobby *pDesc=(PX_Lobby *)ptr;
	px_int level=pDesc->userInfo.ShipStack[pDesc->userState.shipIndex].upgrade_speed;
	px_int pay=100*(1<<level);

	if (level>=10)
	{
		PX_ApplicationMessageBoxAlertOk((px_char *)L"等级已达到最大值");
		return;
	}

	if ((px_int)pDesc->userInfo.coin<pay)
	{
		PX_ApplicationMessageBoxAlertOk((px_char *)L"所需的金币不足");
		return;
	}
	pDesc->userinfo_elpased=PX_LOBBY_UPDATE_USERINFO_TIME;

	PX_memset(&cmdPack,0,sizeof(cmdPack));

	cmdPack.header.magic=PX_NETWORK_LOBBY_MAGIC;
	cmdPack.header.id=pDesc->id;
	cmdPack.header.mapid=pDesc->map_id;
	cmdPack.header.type=PX_NETWORK_LOBBY_TYPE_UPGRADE_MAXSPEED;

	PX_memcpy(cmdPack.content.cookie,pDesc->cookie,sizeof(pDesc->cookie));
	PX_AES_Initialize(&aes,PX_AES_KeySize_Bits256,pDesc->aes_code);
	PX_AES_CipherBuffer(&aes,(px_byte *)&cmdPack.content,sizeof(cmdPack.content),(px_byte *)&cmdPack.content);

	PX_UDPSend(&pDesc->udp,pDesc->serverAddr,(px_byte *)&cmdPack,sizeof(cmdPack));
}

px_void PX_LobbyUpgrade_shield(px_void *ptr)
{
	PX_Lobby_Packet_Command cmdPack;
	PX_AES aes;
	PX_Lobby *pDesc=(PX_Lobby *)ptr;
	px_int level=pDesc->userInfo.ShipStack[pDesc->userState.shipIndex].upgrade_shield;
	px_int pay=100*(1<<level);

	if (level>=10)
	{
		PX_ApplicationMessageBoxAlertOk((px_char *)L"等级已达到最大值");
		return;
	}

	if ((px_int)pDesc->userInfo.coin<pay)
	{
		PX_ApplicationMessageBoxAlertOk((px_char *)L"所需的金币不足");
		return;
	}
	pDesc->userinfo_elpased=PX_LOBBY_UPDATE_USERINFO_TIME;

	PX_memset(&cmdPack,0,sizeof(cmdPack));

	cmdPack.header.magic=PX_NETWORK_LOBBY_MAGIC;
	cmdPack.header.id=pDesc->id;
	cmdPack.header.mapid=pDesc->map_id;
	cmdPack.header.type=PX_NETWORK_LOBBY_TYPE_UPGRADE_MAXSHIELD;

	PX_memcpy(cmdPack.content.cookie,pDesc->cookie,sizeof(pDesc->cookie));
	PX_AES_Initialize(&aes,PX_AES_KeySize_Bits256,pDesc->aes_code);
	PX_AES_CipherBuffer(&aes,(px_byte *)&cmdPack.content,sizeof(cmdPack.content),(px_byte *)&cmdPack.content);

	PX_UDPSend(&pDesc->udp,pDesc->serverAddr,(px_byte *)&cmdPack,sizeof(cmdPack));
}

px_void PX_LobbyUpgrade_life(px_void *ptr)
{
	PX_Lobby_Packet_Command cmdPack;
	PX_AES aes;
	PX_Lobby *pDesc=(PX_Lobby *)ptr;
	px_int level=pDesc->userInfo.ShipStack[pDesc->userState.shipIndex].upgrade_life;
	px_int pay=100*(1<<level);

	if (level>=10)
	{
		PX_ApplicationMessageBoxAlertOk((px_char *)L"等级已达到最大值");
		return;
	}

	if ((px_int)pDesc->userInfo.coin<pay)
	{
		PX_ApplicationMessageBoxAlertOk((px_char *)L"所需的金币不足");
		return;
	}
	pDesc->userinfo_elpased=PX_LOBBY_UPDATE_USERINFO_TIME;

	PX_memset(&cmdPack,0,sizeof(cmdPack));

	cmdPack.header.magic=PX_NETWORK_LOBBY_MAGIC;
	cmdPack.header.id=pDesc->id;
	cmdPack.header.mapid=pDesc->map_id;
	cmdPack.header.type=PX_NETWORK_LOBBY_TYPE_UPGRADE_MAXLIFE;

	PX_memcpy(cmdPack.content.cookie,pDesc->cookie,sizeof(pDesc->cookie));
	PX_AES_Initialize(&aes,PX_AES_KeySize_Bits256,pDesc->aes_code);
	PX_AES_CipherBuffer(&aes,(px_byte *)&cmdPack.content,sizeof(cmdPack.content),(px_byte *)&cmdPack.content);

	PX_UDPSend(&pDesc->udp,pDesc->serverAddr,(px_byte *)&cmdPack,sizeof(cmdPack));
}


px_void PX_LobbyUpload_Photo(px_void *ptr)
{
	typedef struct  
	{
		PX_Lobby_Packet_LargeDataHeader header;
		px_byte photo[PX_STARTUP_USERDATA_PHOTOSIZE];
	}PX_Lobby_Packet_LargeDataPack;
	px_byte tempBuffer[65535];
	PX_Lobby_Packet_LargeDataPack LargeDataPack;
	PX_AES aes;
	PX_Lobby *pDesc=(PX_Lobby *)ptr;
	px_texture tex;

	px_memorypool mp=MP_Create(tempBuffer,sizeof(tempBuffer));
	PX_memset(&LargeDataPack,0,sizeof(LargeDataPack));
	do 
	{
		px_int size;
		px_char *path=PX_OpenFileDialog("bmp\0*.bmp\0traw\0*.traw");
		if (path)
		{
			
			if(!PX_LoadTextureFromFile(&mp,&tex,path))
			{
				PX_ApplicationMessageBoxAlertOk((px_char *)L"无效的图像文件(必须为64x64的bmp或traw文件)");
				return;
			}
			if (tex.width!=64||tex.height!=64)
			{
				PX_ApplicationMessageBoxAlertOk((px_char *)L"无效的图像文件(必须为64x64的bmp或traw文件)");
				return;
			}
			PX_TRawBuild(&tex,LargeDataPack.photo,&size);
		}
		else
		{
			return;
		}
	} while (0);
	

	if ((px_int)pDesc->userInfo.coin<12800&&(px_int)pDesc->userInfo.ticket<12800)
	{
		PX_ApplicationMessageBoxAlertOk((px_char *)L"所需的金币或点券不足");
		return;
	}

	

	LargeDataPack.header.magic=PX_NETWORK_LOBBY_MAGIC;
	LargeDataPack.header.id=pDesc->id;
	LargeDataPack.header.mapid=pDesc->map_id;
	LargeDataPack.header.type=PX_NETWORK_LOBBY_TYPE_UPLOADPHOTO;

	PX_memcpy(LargeDataPack.header.cookie,pDesc->cookie,sizeof(pDesc->cookie));
	PX_AES_Initialize(&aes,PX_AES_KeySize_Bits256,pDesc->aes_code);
	PX_AES_CipherBuffer(&aes,(px_byte *)&LargeDataPack.header.cookie,sizeof(LargeDataPack.header.cookie),(px_byte *)&LargeDataPack.header.cookie);

	PX_UDPSend(&pDesc->udp,pDesc->serverLargeDataAddr,(px_byte *)&LargeDataPack,sizeof(LargeDataPack));
	PX_ApplicationMessageBoxAlertOk((px_char *)L"上传完成,重启后生效");
}

px_void PX_LobbyOnUploadPhoto(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	PX_Lobby *pDesc=(PX_Lobby *)ptr;
	PX_ApplicationMessageBoxAlertYesNo((px_char *)L"花费12800金币或点券上传新头像?",PX_LobbyUpload_Photo,pDesc,PX_NULL,PX_NULL);
}

px_void PX_LobbyOnBuyTicket(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	PX_Lobby *pDesc=(PX_Lobby *)ptr;
	PX_ApplicationMessageBoxAlertYesNo((px_char *)L"花费1000金币购买10张入场券?",PX_LobbyBuyTicket,pDesc,PX_NULL,PX_NULL);
}

px_void PX_LobbyOnUpgrade_AmmoGen(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	PX_Lobby *pDesc=(PX_Lobby *)ptr;
	static px_word content[64]={0};
	px_char numeric[32]={0};
	px_int level=pDesc->userInfo.ShipStack[pDesc->userState.shipIndex].upgrade_ammo_generator;
	px_int pay=100*(1<<level);

	if (level>=10)
	{
		PX_ApplicationMessageBoxAlertOk((px_char *)L"等级已达到最大值");
		return;
	}

	content[0]=0;
	PX_itoa(pay,numeric,sizeof(numeric),10);
	PX_wstrcat(content,(px_word *)L"花费");
	PX_FontModule_wastrcat(content,numeric);
	PX_wstrcat(content,(px_word *)L"金币或点券提升基础武器补充等级?");
	PX_ApplicationMessageBoxAlertYesNo((px_char *)content,PX_LobbyUpgrade_AmmoGen,pDesc,PX_NULL,PX_NULL);
}

px_void PX_LobbyOnUpgrade_AmmoRec(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	PX_Lobby *pDesc=(PX_Lobby *)ptr;
	static px_word content[64]={0};
	px_char numeric[32]={0};
	px_int level=pDesc->userInfo.ShipStack[pDesc->userState.shipIndex].upgrade_lastfirerecovery;
	px_int pay=100*(1<<level);

	if (level>=10)
	{
		PX_ApplicationMessageBoxAlertOk((px_char *)L"等级已达到最大值");
		return;
	}

	content[0]=0;
	PX_itoa(pay,numeric,sizeof(numeric),10);
	PX_wstrcat(content,(px_word *)L"花费");
	PX_FontModule_wastrcat(content,numeric);
	PX_wstrcat(content,(px_word *)L"金币或点券提升基础武器冷却等级?");
	PX_ApplicationMessageBoxAlertYesNo((px_char *)content,PX_LobbyUpgrade_AmmoRecovery,pDesc,PX_NULL,PX_NULL);
}

px_void PX_LobbyOnUpgrade_Force(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	PX_Lobby *pDesc=(PX_Lobby *)ptr;
	static px_word content[64]={0};
	px_char numeric[32]={0};
	px_int level=pDesc->userInfo.ShipStack[pDesc->userState.shipIndex].upgrade_force;
	px_int pay=100*(1<<level);

	if (level>=10)
	{
		PX_ApplicationMessageBoxAlertOk((px_char *)L"等级已达到最大值");
		return;
	}

	content[0]=0;
	PX_itoa(pay,numeric,sizeof(numeric),10);
	PX_wstrcat(content,(px_word *)L"花费");
	PX_FontModule_wastrcat(content,numeric);
	PX_wstrcat(content,(px_word *)L"金币或点券提升最大推力等级?");
	PX_ApplicationMessageBoxAlertYesNo((px_char *)content,PX_LobbyUpgrade_force,pDesc,PX_NULL,PX_NULL);
}

px_void PX_LobbyOnUpgrade_Speed(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	PX_Lobby *pDesc=(PX_Lobby *)ptr;
	static px_word content[64]={0};
	px_char numeric[32]={0};
	px_int level=pDesc->userInfo.ShipStack[pDesc->userState.shipIndex].upgrade_speed;
	px_int pay=100*(1<<level);

	if (level>=10)
	{
		PX_ApplicationMessageBoxAlertOk((px_char *)L"等级已达到最大值");
		return;
	}


	content[0]=0;
	PX_itoa(pay,numeric,sizeof(numeric),10);
	PX_wstrcat(content,(px_word *)L"花费");
	PX_FontModule_wastrcat(content,numeric);
	PX_wstrcat(content,(px_word *)L"金币提升最大速度等级?");
	PX_ApplicationMessageBoxAlertYesNo((px_char *)content,PX_LobbyUpgrade_speed,pDesc,PX_NULL,PX_NULL);
}

px_void PX_LobbyOnUpgrade_Life(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	PX_Lobby *pDesc=(PX_Lobby *)ptr;
	static px_word content[64]={0};
	px_char numeric[32]={0};
	px_int level=pDesc->userInfo.ShipStack[pDesc->userState.shipIndex].upgrade_life;
	px_int pay=100*(1<<level);
	if (level>=10)
	{
		PX_ApplicationMessageBoxAlertOk((px_char *)L"等级已达到最大值");
		return;
	}

	content[0]=0;
	PX_itoa(pay,numeric,sizeof(numeric),10);
	PX_wstrcat(content,(px_word *)L"花费");
	PX_FontModule_wastrcat(content,numeric);
	PX_wstrcat(content,(px_word *)L"金币提升最大生命等级?");
	PX_ApplicationMessageBoxAlertYesNo((px_char *)content,PX_LobbyUpgrade_life,pDesc,PX_NULL,PX_NULL);
}

px_void PX_LobbyOnUpgrade_Shield(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	PX_Lobby *pDesc=(PX_Lobby *)ptr;
	static px_word content[64]={0};
	px_char numeric[32]={0};
	px_int level=pDesc->userInfo.ShipStack[pDesc->userState.shipIndex].upgrade_shield;
	px_int pay=100*(1<<level);

	if (level>=10)
	{
		PX_ApplicationMessageBoxAlertOk((px_char *)L"等级已达到最大值");
		return;
	}

	content[0]=0;
	PX_itoa(pay,numeric,sizeof(numeric),10);
	PX_wstrcat(content,(px_word *)L"花费");
	PX_FontModule_wastrcat(content,numeric);
	PX_wstrcat(content,(px_word *)L"金币提升最大护盾等级?");
	PX_ApplicationMessageBoxAlertYesNo((px_char *)content,PX_LobbyUpgrade_shield,pDesc,PX_NULL,PX_NULL);
}

px_void PX_LobbyGameStart(PX_Lobby *pDesc)
{
	if (pDesc->gameHandle==PX_NULL)
	{
		DWORD threadId;
		pDesc->gameHandle=CreateThread(NULL, 0, PX_LobbyGameStartThread, pDesc, 0, &threadId);
	}
}

px_void PX_LobbyOnGameStart(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	PX_LobbyGameStart((PX_Lobby *)ptr);
}

px_void PX_LobbyOnRanking8P(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	PX_Lobby *pDesc=(PX_Lobby *)ptr;
	PX_Lobby_Packet_Command cmdPack;
	PX_AES aes;

	if (pDesc->userInfo.ticket<4)
	{
		extern px_void PX_ApplicationMessageBoxAlert(const px_char content[]);
		PX_ApplicationMessageBoxAlertOk((px_char *)L"你没有足够的入场券(4张)来参加这场比赛");
		return;
	}


	pDesc->userstate_elpased=0;
	pDesc->userState.game_readyPlayers=0;

	PX_memset(&cmdPack,0,sizeof(cmdPack));

	cmdPack.header.magic=PX_NETWORK_LOBBY_MAGIC;
	cmdPack.header.id=pDesc->id;
	cmdPack.header.mapid=pDesc->map_id;
	cmdPack.header.type=PX_NETWORK_LOBBY_TYPE_STARTGAME;

	PX_memcpy(cmdPack.content.cookie,pDesc->cookie,sizeof(pDesc->cookie));
	cmdPack.content.param[0]=GAME_PLAY_GAMETYPE_8P;//type
	cmdPack.content.param[1]=0;//room key

	PX_AES_Initialize(&aes,PX_AES_KeySize_Bits256,pDesc->aes_code);
	PX_AES_CipherBuffer(&aes,(px_byte *)&cmdPack.content,sizeof(cmdPack.content),(px_byte *)&cmdPack.content);

	PX_UDPSend(&pDesc->udp,pDesc->serverAddr,(px_byte *)&cmdPack,sizeof(cmdPack));

	pDesc->userState.userState=PX_STARTUP_GAMESTATE_RANKING;
}

px_void PX_LobbyOnRanking6P(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	PX_Lobby *pDesc=(PX_Lobby *)ptr;
	PX_Lobby_Packet_Command cmdPack;
	PX_AES aes;

	if (pDesc->userInfo.ticket<3)
	{
		extern px_void PX_ApplicationMessageBoxAlert(const px_char content[]);
		PX_ApplicationMessageBoxAlertOk((px_char *)L"你没有足够的入场券(3张)来参加这场比赛");
		return;
	}

	pDesc->userstate_elpased=0;
	pDesc->userState.game_readyPlayers=0;
	PX_memset(&cmdPack,0,sizeof(cmdPack));

	cmdPack.header.magic=PX_NETWORK_LOBBY_MAGIC;
	cmdPack.header.id=pDesc->id;
	cmdPack.header.mapid=pDesc->map_id;
	cmdPack.header.type=PX_NETWORK_LOBBY_TYPE_STARTGAME;

	PX_memcpy(cmdPack.content.cookie,pDesc->cookie,sizeof(pDesc->cookie));
	cmdPack.content.param[0]=GAME_PLAY_GAMETYPE_6P;//type
	cmdPack.content.param[1]=0;//room key

	PX_AES_Initialize(&aes,PX_AES_KeySize_Bits256,pDesc->aes_code);
	PX_AES_CipherBuffer(&aes,(px_byte *)&cmdPack.content,sizeof(cmdPack.content),(px_byte *)&cmdPack.content);

	PX_UDPSend(&pDesc->udp,pDesc->serverAddr,(px_byte *)&cmdPack,sizeof(cmdPack));

	pDesc->userState.userState=PX_STARTUP_GAMESTATE_RANKING;
}

px_void PX_LobbyOnRanking4P(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	PX_Lobby *pDesc=(PX_Lobby *)ptr;
	PX_Lobby_Packet_Command cmdPack;
	PX_AES aes;

	if (pDesc->userInfo.ticket<2)
	{
		extern px_void PX_ApplicationMessageBoxAlert(const px_char content[]);
		PX_ApplicationMessageBoxAlertOk((px_char *)L"你没有足够入场券(2张)来参加这场比赛");
		return;
	}

	pDesc->userstate_elpased=0;
	pDesc->userState.game_readyPlayers=0;
	PX_memset(&cmdPack,0,sizeof(cmdPack));

	cmdPack.header.magic=PX_NETWORK_LOBBY_MAGIC;
	cmdPack.header.id=pDesc->id;
	cmdPack.header.mapid=pDesc->map_id;
	cmdPack.header.type=PX_NETWORK_LOBBY_TYPE_STARTGAME;

	PX_memcpy(cmdPack.content.cookie,pDesc->cookie,sizeof(pDesc->cookie));
	cmdPack.content.param[0]=GAME_PLAY_GAMETYPE_4P;//type
	cmdPack.content.param[1]=0;//room key

	PX_AES_Initialize(&aes,PX_AES_KeySize_Bits256,pDesc->aes_code);
	PX_AES_CipherBuffer(&aes,(px_byte *)&cmdPack.content,sizeof(cmdPack.content),(px_byte *)&cmdPack.content);

	PX_UDPSend(&pDesc->udp,pDesc->serverAddr,(px_byte *)&cmdPack,sizeof(cmdPack));

	pDesc->userState.userState=PX_STARTUP_GAMESTATE_RANKING;
}

px_void PX_LobbyOnRanking2P(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	PX_Lobby *pDesc=(PX_Lobby *)ptr;
	PX_Lobby_Packet_Command cmdPack;
	PX_AES aes;

	if (pDesc->userInfo.ticket<1)
	{
		extern px_void PX_ApplicationMessageBoxAlert(const px_char content[]);
		PX_ApplicationMessageBoxAlertOk((px_char *)L"你没有足够的入场券(1张)来参加这场比赛");
		return;
	}

	pDesc->userstate_elpased=0;
	pDesc->userState.game_readyPlayers=0;
	PX_memset(&cmdPack,0,sizeof(cmdPack));

	cmdPack.header.magic=PX_NETWORK_LOBBY_MAGIC;
	cmdPack.header.id=pDesc->id;
	cmdPack.header.mapid=pDesc->map_id;
	cmdPack.header.type=PX_NETWORK_LOBBY_TYPE_STARTGAME;

	PX_memcpy(cmdPack.content.cookie,pDesc->cookie,sizeof(pDesc->cookie));
	cmdPack.content.param[0]=GAME_PLAY_GAMETYPE_2P;//type
	cmdPack.content.param[1]=0;//room key

	PX_AES_Initialize(&aes,PX_AES_KeySize_Bits256,pDesc->aes_code);
	PX_AES_CipherBuffer(&aes,(px_byte *)&cmdPack.content,sizeof(cmdPack.content),(px_byte *)&cmdPack.content);

	PX_UDPSend(&pDesc->udp,pDesc->serverAddr,(px_byte *)&cmdPack,sizeof(cmdPack));

	pDesc->userState.userState=PX_STARTUP_GAMESTATE_RANKING;
}



px_void PX_LobbyOnRanking4V4(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	PX_Lobby *pDesc=(PX_Lobby *)ptr;
	PX_Lobby_Packet_Command cmdPack;
	PX_AES aes;

	if (pDesc->userInfo.ticket<4)
	{
		extern px_void PX_ApplicationMessageBoxAlert(const px_char content[]);
		PX_ApplicationMessageBoxAlertOk((px_char *)L"你没有足够的入场券(4张)来参加这场比赛");
		return;
	}

	pDesc->userstate_elpased=0;
	pDesc->userState.game_readyPlayers=0;
	PX_memset(&cmdPack,0,sizeof(cmdPack));

	cmdPack.header.magic=PX_NETWORK_LOBBY_MAGIC;
	cmdPack.header.id=pDesc->id;
	cmdPack.header.mapid=pDesc->map_id;
	cmdPack.header.type=PX_NETWORK_LOBBY_TYPE_STARTGAME;

	PX_memcpy(cmdPack.content.cookie,pDesc->cookie,sizeof(pDesc->cookie));
	cmdPack.content.param[0]=GAME_PLAY_GAMETYPE_4V4;//type
	cmdPack.content.param[1]=0;//room key

	PX_AES_Initialize(&aes,PX_AES_KeySize_Bits256,pDesc->aes_code);
	PX_AES_CipherBuffer(&aes,(px_byte *)&cmdPack.content,sizeof(cmdPack.content),(px_byte *)&cmdPack.content);

	PX_UDPSend(&pDesc->udp,pDesc->serverAddr,(px_byte *)&cmdPack,sizeof(cmdPack));

	pDesc->userState.userState=PX_STARTUP_GAMESTATE_RANKING;
}


px_void PX_LobbyOnGame1P(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	PX_Lobby *pDesc=(PX_Lobby *)ptr;
	PX_Lobby_Packet_Command cmdPack;
	PX_AES aes;
	pDesc->userstate_elpased=0;
	pDesc->userState.game_readyPlayers=0;
	PX_memset(&cmdPack,0,sizeof(cmdPack));

	cmdPack.header.magic=PX_NETWORK_LOBBY_MAGIC;
	cmdPack.header.id=pDesc->id;
	cmdPack.header.mapid=pDesc->map_id;
	cmdPack.header.type=PX_NETWORK_LOBBY_TYPE_STARTGAME;

	PX_memcpy(cmdPack.content.cookie,pDesc->cookie,sizeof(pDesc->cookie));
	cmdPack.content.param[0]=GAME_PLAY_GAMETYPE_1P;//type
	cmdPack.content.param[1]=0;//room key

	PX_AES_Initialize(&aes,PX_AES_KeySize_Bits256,pDesc->aes_code);
	PX_AES_CipherBuffer(&aes,(px_byte *)&cmdPack.content,sizeof(cmdPack.content),(px_byte *)&cmdPack.content);

	PX_UDPSend(&pDesc->udp,pDesc->serverAddr,(px_byte *)&cmdPack,sizeof(cmdPack));

	pDesc->userState.userState=PX_STARTUP_GAMESTATE_RANKING;
}


px_void PX_LobbyOnRankingCancel(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	PX_Lobby *pDesc=(PX_Lobby *)ptr;
	PX_Lobby_Packet_Command cmdPack;
	PX_AES aes;
	pDesc->userstate_elpased=0;

	PX_memset(&cmdPack,0,sizeof(cmdPack));

	cmdPack.header.magic=PX_NETWORK_LOBBY_MAGIC;
	cmdPack.header.id=pDesc->id;
	cmdPack.header.mapid=pDesc->map_id;
	cmdPack.header.type=PX_NETWORK_LOBBY_TYPE_CANCELMATCH;

	PX_memcpy(cmdPack.content.cookie,pDesc->cookie,sizeof(pDesc->cookie));

	PX_AES_Initialize(&aes,PX_AES_KeySize_Bits256,pDesc->aes_code);
	PX_AES_CipherBuffer(&aes,(px_byte *)&cmdPack.content,sizeof(cmdPack.content),(px_byte *)&cmdPack.content);

	PX_UDPSend(&pDesc->udp,pDesc->serverAddr,(px_byte *)&cmdPack,sizeof(cmdPack));

	pDesc->userState.userState=PX_STARTUP_GAMESTATE_STANDBY;
}


px_bool PX_LobbyInitialize_PanelUserInfoInitialize(PX_Lobby *pDesc,PX_Lobby_PanelUserInfo *panel_userinfo)
{
	if(!PX_LoadShapeFromFile(&pDesc->pIns->runtime.mp_resources,&panel_userinfo->shape_coin,"./resources/coin.traw")) goto _ERROR;
	if(!PX_LoadShapeFromFile(&pDesc->pIns->runtime.mp_resources,&panel_userinfo->shape_ticket,"./resources/ticket.traw")) goto _ERROR;

	panel_userinfo->root=PX_ObjectCreate(&pDesc->pIns->runtime.mp_ui,PX_NULL,0,0,0,0,0,0);
	panel_userinfo->root->x=PX_LOBBY_PANELUSERINFO_X;
	panel_userinfo->root->y=PX_LOBBY_PANELUSERINFO_X;
	panel_userinfo->root->Width=(px_float)pDesc->pIns->runtime.width;
	panel_userinfo->root->Height=PX_LOBBY_PANELUSERINFO_HEIGHT;

	panel_userinfo->ticket=PX_Object_LabelCreate(&pDesc->pIns->runtime.mp_ui,panel_userinfo->root,0,0,84,22,"0",PX_COLOR(255,0,0,0));
	PX_Object_LabelSetBackgroundColor(panel_userinfo->ticket,PX_COLOR(255,255,255,255));
	PX_Object_LabelSetAlign(panel_userinfo->ticket,PX_OBJECT_ALIGN_LEFT|PX_OBJECT_ALIGN_VCENTER);

	panel_userinfo->coin=PX_Object_LabelCreate(&pDesc->pIns->runtime.mp_ui,panel_userinfo->root,0,0,84,22,"0",PX_COLOR(255,0,0,0));
	PX_Object_LabelSetBackgroundColor(panel_userinfo->coin,PX_COLOR(255,255,255,255));
	PX_Object_LabelSetAlign(panel_userinfo->coin,PX_OBJECT_ALIGN_LEFT|PX_OBJECT_ALIGN_VCENTER);


	panel_userinfo->uploadPhoto=PX_Object_PushButtonCreate(&pDesc->pIns->runtime.mp_ui,panel_userinfo->root,0,0,64,64,"",PX_COLOR(0,0,0,0));
	PX_Object_PushButtonSetBorderColor(panel_userinfo->uploadPhoto,PX_COLOR(0,0,0,0));
	PX_Object_PushButtonSetCursorColor(panel_userinfo->uploadPhoto,PX_COLOR(0,0,0,0));
	PX_Object_PushButtonSetPushColor(panel_userinfo->uploadPhoto,PX_COLOR(0,0,0,0));
	PX_Object_PushButtonSetBackgroundColor(panel_userinfo->uploadPhoto,PX_COLOR(0,0,0,0));
	PX_ObjectRegisterEvent(panel_userinfo->uploadPhoto,PX_OBJECT_EVENT_EXECUTE,PX_LobbyOnUploadPhoto,pDesc);

	panel_userinfo->btn_buyTicket=PX_Object_PushButtonCreate(&pDesc->pIns->runtime.mp_ui,panel_userinfo->root,0,0,24,24,"+",PX_COLOR(255,0,0,0));
	PX_ObjectRegisterEvent(panel_userinfo->btn_buyTicket,PX_OBJECT_EVENT_EXECUTE,PX_LobbyOnBuyTicket,pDesc);
	return PX_TRUE;
_ERROR:
	return PX_FALSE;
}

static px_void PX_LobbyStandRender_UserInfo(PX_Lobby *pDesc,px_dword elpased)
{
	PX_ObjectUpdate(pDesc->panel_userinfo.root,elpased);
	if (elpased>200)
	{
		elpased=200;
	}

	if (PX_ABS(pDesc->panel_userinfo.root->x-PX_LOBBY_PANELUSERINFO_X)>2)
	{
		pDesc->panel_userinfo.root->x+=(PX_LOBBY_PANELUSERINFO_X-pDesc->panel_userinfo.root->x)*(1.0f*elpased/200);
	}
	else
		pDesc->panel_userinfo.root->x=PX_LOBBY_PANELUSERINFO_X;

	if (PX_ABS(pDesc->panel_userinfo.root->y-PX_LOBBY_PANELUSERINFO_Y)>2)
	{
		pDesc->panel_userinfo.root->y+=(PX_LOBBY_PANELUSERINFO_Y-pDesc->panel_userinfo.root->y)*(1.0f*elpased/200);
	}
	else
		pDesc->panel_userinfo.root->y=PX_LOBBY_PANELUSERINFO_Y;

	
	//Photo
	if (!pDesc->photo.surfaceBuffer)
	{
		pDesc->photo_loading_angle+=elpased/1000.0f*360;
		PX_GeoDrawRing(&pDesc->pIns->runtime.RenderSurface,(px_int)pDesc->panel_userinfo.root->x+48,(px_int)pDesc->panel_userinfo.root->y+32+8,28,8,PX_COLOR(255,0,0,0),(px_int)pDesc->photo_loading_angle,(px_int)pDesc->photo_loading_angle+60);
		PX_GeoDrawRing(&pDesc->pIns->runtime.RenderSurface,(px_int)pDesc->panel_userinfo.root->x+48,(px_int)pDesc->panel_userinfo.root->y+32+8,28,8,PX_COLOR(255,0,0,0),(px_int)pDesc->photo_loading_angle+120,(px_int)pDesc->photo_loading_angle+180);
		PX_GeoDrawRing(&pDesc->pIns->runtime.RenderSurface,(px_int)pDesc->panel_userinfo.root->x+48,(px_int)pDesc->panel_userinfo.root->y+32+8,28,8,PX_COLOR(255,0,0,0),(px_int)pDesc->photo_loading_angle+240,(px_int)pDesc->photo_loading_angle+320);
	}
	else
	{
		PX_TextureRender(&pDesc->pIns->runtime.RenderSurface,&pDesc->photo,(px_int)pDesc->panel_userinfo.root->x+48,(px_int)pDesc->panel_userinfo.root->y+32+8,PX_TEXTURERENDER_REFPOINT_CENTER,PX_NULL);
	}

	pDesc->panel_userinfo.uploadPhoto->x=pDesc->panel_userinfo.root->x+16;
	pDesc->panel_userinfo.uploadPhoto->y=pDesc->panel_userinfo.root->y+8;
	
	//NickName
	PX_FontModuleDrawText(&pDesc->pIns->runtime.RenderSurface,(px_int)pDesc->panel_userinfo.root->x+48+48,(px_int)pDesc->panel_userinfo.root->y+32+8,pDesc->userInfo.nickname,PX_COLOR(255,0,0,0),&pDesc->pIns->fontmodule32,PX_FONT_ALIGN_XLEFT);


	//rank
	if (pDesc->displayRankPt>(px_int)pDesc->userInfo.rank_pt)
	{
		if (pDesc->displayRankPt-pDesc->userInfo.rank_pt>10000)
		{
			pDesc->displayRankPt=pDesc->userInfo.rank_pt;
		}
		else
		{
			pDesc->displayRankPt+=elpased;
			if (pDesc->displayRankPt>(px_int)pDesc->userInfo.rank_pt)
				pDesc->displayRankPt=pDesc->userInfo.rank_pt;
		}
	}
	else if (pDesc->displayRankPt<(px_int)pDesc->userInfo.rank_pt)
	{
		if (PX_ABS(pDesc->displayRankPt-(px_int)pDesc->userInfo.rank_pt)>10000)
		{
			pDesc->displayRankPt=pDesc->userInfo.rank_pt;
		}
		else
		{
			pDesc->displayRankPt-=elpased;
			if (pDesc->displayRankPt<(px_int)pDesc->userInfo.rank_pt)
				pDesc->displayRankPt=pDesc->userInfo.rank_pt;
		}
	}
	do 
	{
		px_char content[32];
		px_word wcontent[32];
		PX_sprintf1(content,sizeof(content),"Rank:%1",PX_STRINGFORMAT_INT(pDesc->displayRankPt));
		PX_FontModule_atow(content,wcontent);
		PX_FontModuleDrawText(&pDesc->pIns->runtime.RenderSurface,(px_int)pDesc->panel_userinfo.root->x+64+256,(px_int)pDesc->panel_userinfo.root->y+52,wcontent,PX_COLOR(255,0,0,0),&pDesc->pIns->fontmodule32,PX_FONT_ALIGN_XLEFT);

	} while (0);

	//coin
	do 
	{
		px_char Content[32];
		PX_itoa(pDesc->userInfo.coin,Content,sizeof(Content),10);
		PX_Object_LabelSetText(pDesc->panel_userinfo.coin,Content);
		PX_ObjectSetPosition(pDesc->panel_userinfo.coin,pDesc->panel_userinfo.root->x+576,pDesc->panel_userinfo.root->y+28,0);
	} while (0);
	
	//ticket
	do 
	{
		px_char Content[32];
		PX_itoa(pDesc->userInfo.ticket,Content,sizeof(Content),10);
		PX_Object_LabelSetText(pDesc->panel_userinfo.ticket,Content);
		PX_ObjectSetPosition(pDesc->panel_userinfo.ticket,pDesc->panel_userinfo.root->x+705,pDesc->panel_userinfo.root->y+28,0);
		pDesc->panel_userinfo.btn_buyTicket->x=pDesc->panel_userinfo.root->x+755;
		pDesc->panel_userinfo.btn_buyTicket->y=pDesc->panel_userinfo.root->y+28;
	} while (0);
	


	//userinfo border
	PX_GeoDrawBorder(&pDesc->pIns->runtime.RenderSurface,\
		(px_int)pDesc->panel_userinfo.root->x,\
		(px_int)pDesc->panel_userinfo.root->y,\
		(px_int)(pDesc->panel_userinfo.root->x+pDesc->panel_userinfo.root->Width-1),\
		(px_int)(pDesc->panel_userinfo.root->y+pDesc->panel_userinfo.root->Height-1),\
		3,\
		PX_COLOR(255,0,0,0)\
		);
	PX_ObjectRender(&pDesc->pIns->runtime.RenderSurface,pDesc->panel_userinfo.root,elpased);

	PX_ShapeRender(&pDesc->pIns->runtime.RenderSurface,&pDesc->panel_userinfo.shape_coin,(px_int)pDesc->panel_userinfo.root->x+560,(px_int)pDesc->panel_userinfo.root->y+39,PX_TEXTURERENDER_REFPOINT_CENTER,PX_COLOR(255,0,0,0));
	PX_ShapeRender(&pDesc->pIns->runtime.RenderSurface,&pDesc->panel_userinfo.shape_ticket,(px_int)pDesc->panel_userinfo.root->x+680,(px_int)pDesc->panel_userinfo.root->y+39,PX_TEXTURERENDER_REFPOINT_CENTER,PX_COLOR(255,0,0,0));
}


static px_void PX_LobbyStandRender_UserState(PX_Lobby *pDesc,px_dword elpased)
{
	
	PX_GeoDrawRing(&pDesc->pIns->runtime.RenderSurface,pDesc->pIns->runtime.width/2,pDesc->pIns->runtime.height/2,96,16,PX_COLOR(255,0,0,0),(px_uint)pDesc->state_angle,(px_uint)pDesc->state_angle+90);
	PX_GeoDrawRing(&pDesc->pIns->runtime.RenderSurface,pDesc->pIns->runtime.width/2,pDesc->pIns->runtime.height/2,96,16,PX_COLOR(255,0,0,0),(px_uint)pDesc->state_angle+120,(px_uint)pDesc->state_angle+210);
	PX_GeoDrawRing(&pDesc->pIns->runtime.RenderSurface,pDesc->pIns->runtime.width/2,pDesc->pIns->runtime.height/2,96,16,PX_COLOR(255,0,0,0),(px_uint)pDesc->state_angle+240,(px_uint)pDesc->state_angle+330);
	
	switch(pDesc->userState.userState)
	{
	case PX_STARTUP_GAMESTATE_STANDBY:
		{
			pDesc->state_angle=0;
			PX_FontModuleDrawText(&pDesc->pIns->runtime.RenderSurface,pDesc->pIns->runtime.width/2,pDesc->pIns->runtime.height/2+8,(px_word *)L"待命中",PX_COLOR(255,0,0,0),&pDesc->pIns->fontmodule32,PX_FONT_ALIGN_XCENTER);
		}
		break;
	case PX_STARTUP_GAMESTATE_RANKING:
		{
			pDesc->state_angle+=elpased/1000.0f*90;
			PX_FontModuleDrawText(&pDesc->pIns->runtime.RenderSurface,pDesc->pIns->runtime.width/2,pDesc->pIns->runtime.height/2+8,(px_word *)L"排位中",PX_COLOR(255,0,0,0),&pDesc->pIns->fontmodule32,PX_FONT_ALIGN_XCENTER);
		}
		break;
	case PX_STARTUP_GAMESTATE_GAMING:
		{
			pDesc->state_angle+=elpased/1000.0f*90;
			PX_FontModuleDrawText(&pDesc->pIns->runtime.RenderSurface,pDesc->pIns->runtime.width/2,pDesc->pIns->runtime.height/2+8,(px_word *)L"游戏中",PX_COLOR(255,0,0,0),&pDesc->pIns->fontmodule32,PX_FONT_ALIGN_XCENTER);
		}
		break;
	}

}

px_bool PX_LobbyInitialize(PX_Instance *pIns,PX_Lobby *pDesc,const px_char LobbyServerIpAddr[],px_ushort port,px_ushort largedataport)
{
	PX_memset(pDesc,0,sizeof(PX_Lobby));

	pDesc->pIns=pIns;
	PX_memset(pDesc->cookie,0,sizeof(pDesc->cookie));
	pDesc->id=0;
	pDesc->map_id=0;

	if (!PX_UDPInit(&pDesc->udp,PX_UDP_IP_TYPE_IPV4))
	{
		goto _ERROR;
	}
	PX_strcat(pDesc->serverIpAddr,LobbyServerIpAddr);
	pDesc->serverAddr.ipv4=PX_inet_addr(LobbyServerIpAddr);
	pDesc->serverAddr.port=port;
	pDesc->serverLargeDataAddr=pDesc->serverAddr;
	pDesc->serverLargeDataAddr.port=largedataport;
	pDesc->hb_elpased=0;
	pDesc->connection_elpased=0;
	pDesc->userinfo_elpased=0;

	PX_LobbyInitialize_PanelUserInfoInitialize(pDesc,&pDesc->panel_userinfo);
	PX_memset(&pDesc->userInfo,0,sizeof(pDesc->userInfo));

	do 
	{
		px_int oft=80;
		pDesc->ui_oproot=PX_ObjectCreate(&pDesc->pIns->runtime.mp_ui,PX_NULL,0,0,0,0,0,0);
		pDesc->btn_matchRanking8p=PX_Object_CursorButtonCreate(&pDesc->pIns->runtime.mp_ui,pDesc->ui_oproot,oft,500,128,32,"8人排位战",PX_COLOR(255,0,0,0));
		pDesc->btn_matchRanking6p=PX_Object_CursorButtonCreate(&pDesc->pIns->runtime.mp_ui,pDesc->ui_oproot,oft+138,500,128,32,"6人排位战",PX_COLOR(255,0,0,0));
		pDesc->btn_matchRanking4p=PX_Object_CursorButtonCreate(&pDesc->pIns->runtime.mp_ui,pDesc->ui_oproot,oft+138*2,500,128,32,"4人排位战",PX_COLOR(255,0,0,0));
		pDesc->btn_EnterGame=PX_Object_CursorButtonCreate(&pDesc->pIns->runtime.mp_ui,pDesc->ui_oproot,120+148+74,500,128,32,"进入游戏",PX_COLOR(255,0,0,0));
		pDesc->btn_matchCancel=PX_Object_CursorButtonCreate(&pDesc->pIns->runtime.mp_ui,pDesc->ui_oproot,120+148+74,500,128,32,"取消",PX_COLOR(255,0,0,0));
		pDesc->btn_matchCancel->Visible=PX_FALSE;
		pDesc->btn_match2p=PX_Object_CursorButtonCreate(&pDesc->pIns->runtime.mp_ui,pDesc->ui_oproot,oft+138*3,500,128,32,"2人排位战",PX_COLOR(255,0,0,0));
		pDesc->btn_match1p=PX_Object_CursorButtonCreate(&pDesc->pIns->runtime.mp_ui,pDesc->ui_oproot,oft+138*4,500,128,32,"单人训练",PX_COLOR(255,0,0,0));

		pDesc->autoText_Content=PX_Object_AutoTextCreate(&pDesc->pIns->runtime.mp_ui,pDesc->ui_oproot,585,135,172);

		PX_ObjectRegisterEvent(PX_Object_GetCursorButton(pDesc->btn_EnterGame)->pushbutton,PX_OBJECT_EVENT_EXECUTE,PX_LobbyOnGameStart,pDesc);
		PX_ObjectRegisterEvent(PX_Object_GetCursorButton(pDesc->btn_matchRanking8p)->pushbutton,PX_OBJECT_EVENT_EXECUTE,PX_LobbyOnRanking8P,pDesc);
		PX_ObjectRegisterEvent(PX_Object_GetCursorButton(pDesc->btn_matchRanking6p)->pushbutton,PX_OBJECT_EVENT_EXECUTE,PX_LobbyOnRanking6P,pDesc);
		PX_ObjectRegisterEvent(PX_Object_GetCursorButton(pDesc->btn_matchRanking4p)->pushbutton,PX_OBJECT_EVENT_EXECUTE,PX_LobbyOnRanking4P,pDesc);
		PX_ObjectRegisterEvent(PX_Object_GetCursorButton(pDesc->btn_matchCancel)->pushbutton,PX_OBJECT_EVENT_EXECUTE,PX_LobbyOnRankingCancel,pDesc);
		PX_ObjectRegisterEvent(PX_Object_GetCursorButton(pDesc->btn_match2p)->pushbutton,PX_OBJECT_EVENT_EXECUTE,PX_LobbyOnRanking2P,pDesc);
		PX_ObjectRegisterEvent(PX_Object_GetCursorButton(pDesc->btn_match1p)->pushbutton,PX_OBJECT_EVENT_EXECUTE,PX_LobbyOnGame1P,pDesc);

		oft=32;
		pDesc->btn_upgrade_ammogen=PX_Object_PushButtonCreate(&pDesc->pIns->runtime.mp_ui,pDesc->ui_oproot,oft,132,128,20,"升级基础武器恢复",PX_COLOR(255,0,0,0));
		pDesc->btn_upgrade_ammorec=PX_Object_PushButtonCreate(&pDesc->pIns->runtime.mp_ui,pDesc->ui_oproot,oft,132+54*1,128,20,"升级基础武器冷却",PX_COLOR(255,0,0,0));
		pDesc->btn_upgrade_force=PX_Object_PushButtonCreate(&pDesc->pIns->runtime.mp_ui,pDesc->ui_oproot,oft,132+54*2,128,20,"升级推力",PX_COLOR(255,0,0,0));
		pDesc->btn_upgrade_speed=PX_Object_PushButtonCreate(&pDesc->pIns->runtime.mp_ui,pDesc->ui_oproot,oft,132+54*3,128,20,"升级速度",PX_COLOR(255,0,0,0));
		pDesc->btn_upgrade_life=PX_Object_PushButtonCreate(&pDesc->pIns->runtime.mp_ui,pDesc->ui_oproot,oft,132+54*4,128,20,"升级生命最大值",PX_COLOR(255,0,0,0));
		pDesc->btn_upgrade_shield=PX_Object_PushButtonCreate(&pDesc->pIns->runtime.mp_ui,pDesc->ui_oproot,oft,132+54*5,128,20,"升级护盾最大值",PX_COLOR(255,0,0,0));

		PX_ObjectRegisterEvent(pDesc->btn_upgrade_ammogen,PX_OBJECT_EVENT_EXECUTE,PX_LobbyOnUpgrade_AmmoGen,pDesc);
		PX_ObjectRegisterEvent(pDesc->btn_upgrade_ammorec,PX_OBJECT_EVENT_EXECUTE,PX_LobbyOnUpgrade_AmmoRec,pDesc);
		PX_ObjectRegisterEvent(pDesc->btn_upgrade_force,PX_OBJECT_EVENT_EXECUTE,PX_LobbyOnUpgrade_Force,pDesc);
		PX_ObjectRegisterEvent(pDesc->btn_upgrade_speed,PX_OBJECT_EVENT_EXECUTE,PX_LobbyOnUpgrade_Speed,pDesc);
		PX_ObjectRegisterEvent(pDesc->btn_upgrade_life,PX_OBJECT_EVENT_EXECUTE,PX_LobbyOnUpgrade_Life,pDesc);
		PX_ObjectRegisterEvent(pDesc->btn_upgrade_shield,PX_OBJECT_EVENT_EXECUTE,PX_LobbyOnUpgrade_Shield,pDesc);
		
		
		//PX_ObjectRegisterEvent(pDesc->btn_matchJoin,PX_OBJECT_EVENT_EXECUTE,PX_LobbyOnMatchJoin,pDesc);
	} while (0);
	
	

	return PX_TRUE;
_ERROR:
	return PX_FALSE;
}

px_void PX_LobbySetUserContent(PX_Lobby *pl,px_byte cookie[32],px_byte aes_code[32],px_int map_id,px_int id)
{
	PX_memcpy(pl->cookie,cookie,32);
	PX_memcpy(pl->aes_code,aes_code,32);
	pl->map_id=map_id;
	pl->id=id;
}

px_void PX_LobbyPostEvent(PX_Lobby *pDesc,PX_Object_Event e)
{
	PX_ObjectPostEvent(pDesc->panel_userinfo.root,e);
	PX_ObjectPostEvent(pDesc->ui_oproot,e);
}

px_void PX_LobbyUpdate_heartBeat(PX_Lobby *pDesc,px_dword elpased)
{
	pDesc->hb_elpased+=elpased;
	if (pDesc->hb_elpased>=PX_LOBBY_HEARTBEAT_TIME)
	{
		PX_Lobby_Packet_Command cmdPack;
		PX_memset(&cmdPack,0,sizeof(cmdPack));
		pDesc->hb_elpased=0;
		cmdPack.header.magic=PX_NETWORK_LOBBY_MAGIC;
		cmdPack.header.id=pDesc->id;
		cmdPack.header.mapid=pDesc->map_id;
		cmdPack.header.type=PX_NETWORK_LOBBY_TYPE_HEARTBEAT;
		PX_UDPSend(&pDesc->udp,pDesc->serverAddr,(px_byte *)&cmdPack,sizeof(cmdPack));
	}

}
px_void PX_LobbyUpdate_Packets_headBeatAck(PX_Lobby *pDesc,PX_UDP_ADDR addr,px_byte data[],px_int readsize)
{
	pDesc->connection_elpased=0;
}

px_void PX_LobbyUpdate_Packets_userInfoAck(PX_Lobby *pDesc,PX_UDP_ADDR addr,px_byte data[],px_int readsize)
{
	PX_memcpy(&pDesc->userInfo,data+sizeof(PX_Lobby_Packet_Header),sizeof(pDesc->userInfo));
}

px_void PX_LobbyUpdate_Packets_PhotoAck(PX_Lobby *pDesc,PX_UDP_ADDR addr,px_byte data[],px_int readsize)
{
	PX_Lobby_Packet_Header *pHeader=(PX_Lobby_Packet_Header *)data;
	px_int BlockCount=PX_STARTUP_USERDATA_PHOTOSIZE%1024?PX_STARTUP_USERDATA_PHOTOSIZE/1024+1:PX_STARTUP_USERDATA_PHOTOSIZE/1024;
	px_dword index=pHeader->param;
	if (!pDesc->photo.surfaceBuffer)
	{
		if ((px_int)index>BlockCount-1)
		{
			return;
		}

		if (index==pDesc->photo_block)
		{
			if (pHeader->size+index*1024<=PX_STARTUP_USERDATA_PHOTOSIZE)
			{
				PX_memcpy(pDesc->photo_cache+index*1024,data+sizeof(PX_Lobby_Packet_Header),pHeader->size);
			}
			pDesc->photo_block++;
			pDesc->photo_elpased=PX_LOBBY_UPDATE_PHOTO_TIME;
		}

		if (pDesc->photo_block==BlockCount)
		{
			if(!PX_TextureCreateFromMemory(&pDesc->pIns->runtime.mp_ui,pDesc->photo_cache,PX_STARTUP_USERDATA_PHOTOSIZE,&pDesc->photo))
				pDesc->photo_block=0;
		}
	}
}

px_void PX_LobbyUpdate_Packets_userStateAck(PX_Lobby *pDesc,PX_UDP_ADDR addr,px_byte data[],px_int readsize)
{
	PX_memcpy(&pDesc->userState,data+sizeof(PX_Lobby_Packet_Header),sizeof(pDesc->userState));
}

px_void PX_LobbyUpdate_Packets_newsAck(PX_Lobby *pDesc,PX_UDP_ADDR addr,px_byte data[],px_int readsize)
{
	PX_memcpy(&pDesc->news,data+sizeof(PX_Lobby_Packet_Header),sizeof(pDesc->news));
}

px_void PX_LobbyUpdate_Packets(PX_Lobby *pDesc,PX_UDP_ADDR addr,px_byte data[],px_int readsize)
{
	PX_Lobby_Packet_Header *pPack=(PX_Lobby_Packet_Header *)data;
	if (pPack->magic!=PX_NETWORK_LOBBY_MAGIC)
	{
		return;
	}

	if (pPack->size!=readsize-sizeof(PX_Lobby_Packet_Header))
	{
		return;
	}

	switch(pPack->type)
	{
	case PX_NETWORK_LOBBY_TYPE_HEARTBEATACK:
		{
			PX_LobbyUpdate_Packets_headBeatAck(pDesc,addr,data,readsize);
		}
		break;
	case PX_NETWORK_LOBBY_TYPE_QUERYUSERINFOACK:
		{
			PX_LobbyUpdate_Packets_userInfoAck(pDesc,addr,data,readsize);
		}
		break;
	case PX_NETWORK_LOBBY_TYPE_QUERYPHOTOACK:
		{
			PX_LobbyUpdate_Packets_PhotoAck(pDesc,addr,data,readsize);
		}
		break;
	case PX_NETWORK_LOBBY_TYPE_QUERYSTATEACK:
		{
			PX_LobbyUpdate_Packets_userStateAck(pDesc,addr,data,readsize);
		}
		break;
	case PX_NETWORK_LOBBY_TYPE_QUERYNEWSACK:
		{
			PX_LobbyUpdate_Packets_newsAck(pDesc,addr,data,readsize);
		}
		break;
	}
}

px_void PX_LobbyUpdate_Connection(PX_Lobby *pDesc,px_dword elpased)
{
	//////////////////////////////////////////////////////////////////////////
	//connection
	pDesc->connection_elpased+=elpased;
	if (pDesc->connection_elpased>=PX_LOBBY_CONNECTION_TIME)
	{
		PX_LobbyDisconnect(pDesc);
		return;
	}
	//////////////////////////////////////////////////////////////////////////
}

px_void PX_LobbyUpdate_QueryUserInfo(PX_Lobby *pDesc,px_dword elpased)
{
	//////////////////////////////////////////////////////////////////////////
	//userInfo
	pDesc->userinfo_elpased+=elpased;
	if (pDesc->userinfo_elpased>=PX_LOBBY_UPDATE_USERINFO_TIME)
	{
		PX_Lobby_Packet_Command cmdPack;
		pDesc->userinfo_elpased=0;

		PX_memset(&cmdPack,0,sizeof(cmdPack));

		cmdPack.header.magic=PX_NETWORK_LOBBY_MAGIC;
		cmdPack.header.id=pDesc->id;
		cmdPack.header.mapid=pDesc->map_id;
		cmdPack.header.type=PX_NETWORK_LOBBY_TYPE_QUERYUSERINFO;

		PX_UDPSend(&pDesc->udp,pDesc->serverAddr,(px_byte *)&cmdPack,sizeof(cmdPack));

		return;
	}
	//////////////////////////////////////////////////////////////////////////
}


px_void PX_LobbyUpdate_QueryUserState(PX_Lobby *pDesc,px_dword elpased)
{
	//////////////////////////////////////////////////////////////////////////
	//userInfo
	pDesc->userstate_elpased+=elpased;
	if (pDesc->userstate_elpased>=PX_LOBBY_UPDATE_USERSTATE_TIME)
	{
		PX_Lobby_Packet_Command cmdPack;
		pDesc->userstate_elpased=0;

		PX_memset(&cmdPack,0,sizeof(cmdPack));

		cmdPack.header.magic=PX_NETWORK_LOBBY_MAGIC;
		cmdPack.header.id=pDesc->id;
		cmdPack.header.mapid=pDesc->map_id;
		cmdPack.header.type=PX_NETWORK_LOBBY_TYPE_QUERYSTATE;

		PX_UDPSend(&pDesc->udp,pDesc->serverAddr,(px_byte *)&cmdPack,sizeof(cmdPack));

		return;
	}
	//////////////////////////////////////////////////////////////////////////
}

px_void PX_LobbyUpdate_QueryNews(PX_Lobby *pDesc,px_dword elpased)
{
	//////////////////////////////////////////////////////////////////////////
	//news
	pDesc->news_elpased+=elpased;
	if (pDesc->news_elpased>=PX_LOBBY_UPDATE_NEWS_TIME)
	{
		PX_Lobby_Packet_Command cmdPack;
		pDesc->news_elpased=0;

		PX_memset(&cmdPack,0,sizeof(cmdPack));

		cmdPack.header.magic=PX_NETWORK_LOBBY_MAGIC;
		cmdPack.header.id=pDesc->id;
		cmdPack.header.mapid=pDesc->map_id;
		cmdPack.header.type=PX_NETWORK_LOBBY_TYPE_QUERYNEWS;

		PX_UDPSend(&pDesc->udp,pDesc->serverAddr,(px_byte *)&cmdPack,sizeof(cmdPack));

		return;
	}
	//////////////////////////////////////////////////////////////////////////
}

px_void PX_LobbyUpdate_QueryPhoto(PX_Lobby *pDesc,px_dword elpased)
{
	//////////////////////////////////////////////////////////////////////////
	//photo
	if (pDesc->photo.surfaceBuffer==PX_NULL)
	{
		pDesc->photo_elpased+=elpased;
		if (pDesc->photo_elpased>=PX_LOBBY_UPDATE_PHOTO_TIME)
		{
			PX_Lobby_Packet_Command cmdPack;
			pDesc->userinfo_elpased=0;

			PX_memset(&cmdPack,0,sizeof(cmdPack));

			cmdPack.header.magic=PX_NETWORK_LOBBY_MAGIC;
			cmdPack.header.id=pDesc->id;
			cmdPack.header.mapid=pDesc->map_id;
			cmdPack.header.type=PX_NETWORK_LOBBY_TYPE_QUERYPHOTO;

			cmdPack.content.param[0]=pDesc->photo_block;

			PX_UDPSend(&pDesc->udp,pDesc->serverAddr,(px_byte *)&cmdPack,sizeof(cmdPack));

			return;
		}
	}
	//////////////////////////////////////////////////////////////////////////
}


px_void PX_LobbyUpdate(PX_Lobby *pDesc,px_dword elpased)
{
	px_byte data[PX_NETWORK_PACKET_MAX_SIZE];
	px_int readsize;
	PX_UDP_ADDR addr;

	PX_LobbyUpdate_heartBeat(pDesc,elpased);
	PX_LobbyUpdate_Connection(pDesc,elpased);
	PX_LobbyUpdate_QueryUserInfo(pDesc,elpased);
	PX_LobbyUpdate_QueryPhoto(pDesc,elpased);
	PX_LobbyUpdate_QueryUserState(pDesc,elpased);
	PX_LobbyUpdate_QueryNews(pDesc,elpased);

	while (PX_UDPReceived(&pDesc->udp,&addr,data,sizeof(data),&readsize))
	{
		PX_LobbyUpdate_Packets(pDesc,addr,data,readsize);
	}

	
}


px_void PX_LobbyRender_OpUi(PX_Lobby *pDesc,px_dword elpased)
{
	switch(pDesc->userState.userState)
	{
	case PX_STARTUP_GAMESTATE_STANDBY:
		{
			px_int i;
			px_int x,y;

			pDesc->btn_match2p->Visible=PX_TRUE;
			pDesc->btn_match1p->Visible=PX_TRUE;
			pDesc->btn_matchRanking4p->Visible=PX_TRUE;
			pDesc->btn_matchRanking6p->Visible=PX_TRUE;
			pDesc->btn_matchRanking8p->Visible=PX_TRUE;
			pDesc->btn_EnterGame->Visible=PX_FALSE;

			pDesc->btn_upgrade_ammogen->Visible=PX_TRUE;
			//border
			x=(px_int)pDesc->btn_upgrade_ammogen->x;
			y=(px_int)pDesc->btn_upgrade_ammogen->y+22;
			
			for (i=0;i<pDesc->userInfo.ShipStack[pDesc->userState.shipIndex].upgrade_ammo_generator;i++)
			{
				PX_GeoDrawRect(&pDesc->pIns->runtime.RenderSurface,x+16*i,y,x+16*(i+1),y+16,PX_COLOR(255,255,0,0));
			}
			for (i=0;i<10;i++)
			{
				PX_GeoDrawBorder(&pDesc->pIns->runtime.RenderSurface,x+16*i,y,x+16*(i+1),y+16,1,PX_COLOR(255,0,0,0));
			}

			pDesc->btn_upgrade_ammorec->Visible=PX_TRUE;
			x=(px_int)pDesc->btn_upgrade_ammorec->x;
			y=(px_int)pDesc->btn_upgrade_ammorec->y+22;

			for (i=0;i<pDesc->userInfo.ShipStack[pDesc->userState.shipIndex].upgrade_lastfirerecovery;i++)
			{
				PX_GeoDrawRect(&pDesc->pIns->runtime.RenderSurface,x+16*i,y,x+16*(i+1),y+16,PX_COLOR(255,255,0,0));
			}
			for (i=0;i<10;i++)
			{
				PX_GeoDrawBorder(&pDesc->pIns->runtime.RenderSurface,x+16*i,y,x+16*(i+1),y+16,1,PX_COLOR(255,0,0,0));
			}


			pDesc->btn_upgrade_force->Visible=PX_TRUE;
			x=(px_int)pDesc->btn_upgrade_force->x;
			y=(px_int)pDesc->btn_upgrade_force->y+22;

			for (i=0;i<pDesc->userInfo.ShipStack[pDesc->userState.shipIndex].upgrade_force;i++)
			{
				PX_GeoDrawRect(&pDesc->pIns->runtime.RenderSurface,x+16*i,y,x+16*(i+1),y+16,PX_COLOR(255,255,0,0));
			}
			for (i=0;i<10;i++)
			{
				PX_GeoDrawBorder(&pDesc->pIns->runtime.RenderSurface,x+16*i,y,x+16*(i+1),y+16,1,PX_COLOR(255,0,0,0));
			}

			pDesc->btn_upgrade_speed->Visible=PX_TRUE;
			x=(px_int)pDesc->btn_upgrade_speed->x;
			y=(px_int)pDesc->btn_upgrade_speed->y+22;
			for (i=0;i<pDesc->userInfo.ShipStack[pDesc->userState.shipIndex].upgrade_speed;i++)
			{
				PX_GeoDrawRect(&pDesc->pIns->runtime.RenderSurface,x+16*i,y,x+16*(i+1),y+16,PX_COLOR(255,255,0,0));
			}
			for (i=0;i<10;i++)
			{
				PX_GeoDrawBorder(&pDesc->pIns->runtime.RenderSurface,x+16*i,y,x+16*(i+1),y+16,1,PX_COLOR(255,0,0,0));
			}

			pDesc->btn_upgrade_life->Visible=PX_TRUE;
			x=(px_int)pDesc->btn_upgrade_life->x;
			y=(px_int)pDesc->btn_upgrade_life->y+22;
			for (i=0;i<pDesc->userInfo.ShipStack[pDesc->userState.shipIndex].upgrade_life;i++)
			{
				PX_GeoDrawRect(&pDesc->pIns->runtime.RenderSurface,x+16*i,y,x+16*(i+1),y+16,PX_COLOR(255,255,0,0));
			}
			for (i=0;i<10;i++)
			{
				PX_GeoDrawBorder(&pDesc->pIns->runtime.RenderSurface,x+16*i,y,x+16*(i+1),y+16,1,PX_COLOR(255,0,0,0));
			}

			pDesc->btn_upgrade_shield->Visible=PX_TRUE;
			x=(px_int)pDesc->btn_upgrade_shield->x;
			y=(px_int)pDesc->btn_upgrade_shield->y+22;
			for (i=0;i<pDesc->userInfo.ShipStack[pDesc->userState.shipIndex].upgrade_shield;i++)
			{
				PX_GeoDrawRect(&pDesc->pIns->runtime.RenderSurface,x+16*i,y,x+16*(i+1),y+16,PX_COLOR(255,255,0,0));
			}
			for (i=0;i<10;i++)
			{
				PX_GeoDrawBorder(&pDesc->pIns->runtime.RenderSurface,x+16*i,y,x+16*(i+1),y+16,1,PX_COLOR(255,0,0,0));
			}



			pDesc->btn_matchCancel->Visible=PX_FALSE;
		}
		break;
	case PX_STARTUP_GAMESTATE_RANKING:
		{

			pDesc->btn_match2p->Visible=PX_FALSE;
			pDesc->btn_match1p->Visible=PX_FALSE;
			pDesc->btn_matchRanking4p->Visible=PX_FALSE;
			pDesc->btn_matchRanking6p->Visible=PX_FALSE;
			pDesc->btn_matchRanking8p->Visible=PX_FALSE;
			pDesc->btn_matchCancel->Visible=PX_TRUE;

			pDesc->btn_upgrade_ammogen->Visible=PX_FALSE;
			pDesc->btn_upgrade_ammorec->Visible=PX_FALSE;
			pDesc->btn_upgrade_force->Visible=PX_FALSE;
			pDesc->btn_upgrade_speed->Visible=PX_FALSE;
			pDesc->btn_upgrade_life->Visible=PX_FALSE;
			pDesc->btn_upgrade_shield->Visible=PX_FALSE;
			pDesc->btn_EnterGame->Visible=PX_FALSE;
		}
		break;
	case PX_STARTUP_GAMESTATE_GAMING:
		{
			pDesc->btn_match2p->Visible=PX_FALSE;
			pDesc->btn_match1p->Visible=PX_FALSE;
			pDesc->btn_matchRanking4p->Visible=PX_FALSE;
			pDesc->btn_matchRanking6p->Visible=PX_FALSE;
			pDesc->btn_matchRanking8p->Visible=PX_FALSE;
			pDesc->btn_matchCancel->Visible=PX_FALSE;

			pDesc->btn_upgrade_ammogen->Visible=PX_FALSE;
			pDesc->btn_upgrade_ammorec->Visible=PX_FALSE;
			pDesc->btn_upgrade_force->Visible=PX_FALSE;
			pDesc->btn_upgrade_speed->Visible=PX_FALSE;
			pDesc->btn_upgrade_life->Visible=PX_FALSE;
			pDesc->btn_upgrade_shield->Visible=PX_FALSE;

			if (pDesc->gameHandle)
			{
				pDesc->btn_EnterGame->Visible=PX_FALSE;
			}
			else
			{
				pDesc->btn_EnterGame->Visible=PX_TRUE;
			}
		}
		break;
	}

	PX_ObjectUpdate(pDesc->ui_oproot,elpased);
}

px_void PX_LobbyRender_Online(PX_Lobby *pDesc,px_dword elpased)
{
	switch(pDesc->userState.userState)
	{
	case PX_STARTUP_GAMESTATE_STANDBY:
		{
			px_char numeric[16];
			px_word content[32]={0};
			PX_itoa(pDesc->userState.game_onlineCount,numeric,sizeof(numeric),10);
			PX_wstrcat(content,(px_word *)L"游戏大厅在线:");
			PX_FontModule_wastrcat(content,numeric);
			PX_FontModuleDrawText(&pDesc->pIns->runtime.RenderSurface,pDesc->pIns->runtime.width/2,164,content,PX_COLOR(255,0,0,0),&pDesc->pIns->fontmodule32,PX_FONT_ALIGN_XCENTER);
		}
		break;
	case PX_STARTUP_GAMESTATE_RANKING:
		{
			px_char numeric[16];
			px_word content[32]={0};
			PX_itoa(pDesc->userState.game_readyPlayers,numeric,sizeof(numeric),10);
			PX_wstrcat(content,(px_word *)L"玩家已加入:");
			PX_FontModule_wastrcat(content,numeric);
			PX_FontModuleDrawText(&pDesc->pIns->runtime.RenderSurface,pDesc->pIns->runtime.width/2,164,content,PX_COLOR(255,0,0,0),&pDesc->pIns->fontmodule32,PX_FONT_ALIGN_XCENTER);
		}
		break;
	}
	
}

px_void PX_LobbyStandRender_Content(PX_Lobby *pDesc,px_dword elpased)
{
	px_int i;
	px_char Content[512]={0};
	px_char subContent[32];
	px_surface *renderSurface=&pDesc->pIns->runtime.RenderSurface;

	if (pDesc->userState.userState!=PX_STARTUP_GAMESTATE_STANDBY)
	{
		pDesc->autoText_Content->Visible=PX_FALSE;
		return;
	}
	pDesc->autoText_Content->Visible=PX_TRUE;

	PX_GeoDrawBorder(renderSurface,570,130,786,460,2,PX_COLOR(255,0,0,0));

	if (pDesc->news[0])
	{
		PX_strcat(Content,pDesc->news);
		PX_strcat(Content,"\n\n");
	}
	
	for (i=0;i<PX_COUNTOF(pDesc->userState.RoomPlayerCount);i++)
	{
		if (pDesc->userState.RoomPlayerCount[i])
		{
			switch (i)
			{
			case GAME_PLAY_GAMETYPE_8P:
				PX_sprintf1(subContent,sizeof(subContent),"8人排位等待:%1人\n",PX_STRINGFORMAT_INT(pDesc->userState.RoomPlayerCount[i]));
				PX_strcat(Content,subContent);
				break;
			case GAME_PLAY_GAMETYPE_6P:
				PX_sprintf1(subContent,sizeof(subContent),"6人排位等待:%1人\n",PX_STRINGFORMAT_INT(pDesc->userState.RoomPlayerCount[i]));
				PX_strcat(Content,subContent);
				break;
			case GAME_PLAY_GAMETYPE_4P:
				PX_sprintf1(subContent,sizeof(subContent),"4人排位等待:%1人\n",PX_STRINGFORMAT_INT(pDesc->userState.RoomPlayerCount[i]));
				PX_strcat(Content,subContent);
				break;
			case GAME_PLAY_GAMETYPE_2P:
				PX_sprintf1(subContent,sizeof(subContent),"2人排位等待:%1人\n",PX_STRINGFORMAT_INT(pDesc->userState.RoomPlayerCount[i]));
				PX_strcat(Content,subContent);
				break;
			case GAME_PLAY_GAMETYPE_1P:
				break;
			case GAME_PLAY_GAMETYPE_5V5:
				PX_sprintf1(subContent,sizeof(subContent),"5V5排位等待:%1人\n",PX_STRINGFORMAT_INT(pDesc->userState.RoomPlayerCount[i]));
				PX_strcat(Content,subContent);
				break;
			case GAME_PLAY_GAMETYPE_4V4:
				PX_sprintf1(subContent,sizeof(subContent),"4V4排位等待:%1人\n",PX_STRINGFORMAT_INT(pDesc->userState.RoomPlayerCount[i]));
				PX_strcat(Content,subContent);
				break;
			case GAME_PLAY_GAMETYPE_3V3:
				PX_sprintf1(subContent,sizeof(subContent),"3V3排位等待:%1人\n",PX_STRINGFORMAT_INT(pDesc->userState.RoomPlayerCount[i]));
				PX_strcat(Content,subContent);
				break;
			case GAME_PLAY_GAMETYPE_2V2:
				PX_sprintf1(subContent,sizeof(subContent),"2V2排位等待:%1人\n",PX_STRINGFORMAT_INT(pDesc->userState.RoomPlayerCount[i]));
				PX_strcat(Content,subContent);
				break;
			}
		}
	}
	PX_Object_AutoTextSetText(pDesc->autoText_Content,Content);

}
px_void PX_LobbyRender(PX_Lobby *pDesc,px_dword elpased)
{
	
	PX_LobbyStandRender_UserInfo(pDesc,elpased);
	PX_LobbyStandRender_UserState(pDesc,elpased);
	PX_LobbyRender_OpUi(pDesc,elpased);
	PX_LobbyRender_Online(pDesc,elpased);
	PX_LobbyStandRender_Content(pDesc,elpased);

	PX_ObjectRender(&pDesc->pIns->runtime.RenderSurface,pDesc->ui_oproot,elpased);
}

px_void PX_LobbyReset(PX_Lobby *pDesc)
{
	pDesc->userState.userState=PX_STARTUP_GAMESTATE_STANDBY;

	pDesc->panel_userinfo.root->y=-48;
	pDesc->hb_elpased=0;
	pDesc->connection_elpased=0;
	pDesc->photo_elpased=PX_LOBBY_UPDATE_PHOTO_TIME;
	pDesc->userinfo_elpased=PX_LOBBY_UPDATE_USERINFO_TIME;
	pDesc->photo_block=0;
	if (pDesc->photo.surfaceBuffer)
	{
		PX_TextureFree(&pDesc->photo);
	}
}

px_void PX_LobbyDisconnect(PX_Lobby *pDesc)
{
	extern px_void PX_ApplicationSwitchLogin(px_char account[],px_char pwd[]);
	PX_LobbyReset(pDesc);
	PX_ApplicationSwitchLogin("","");
}
