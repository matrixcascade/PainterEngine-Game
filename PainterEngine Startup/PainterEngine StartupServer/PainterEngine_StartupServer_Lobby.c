#include "PainterEngine_StartupServer_Lobby.h"

extern px_void PX_Application_ConsolePrint(const px_char *Text);



DWORD WINAPI PX_LobbyServerGameStartThread(px_void *ptr)
{
	px_char startup_param[MAX_PATH];
	px_char currentDir[MAX_PATH];
	px_dword processRet;
	PX_StartupServer_Lobby_GameInstance *pDesc=(PX_StartupServer_Lobby_GameInstance *)ptr;
	//PX_GameClientStartUp startup_param;
	STARTUPINFO sti;
	PROCESS_INFORMATION proci;

	memset(&sti,0,sizeof(STARTUPINFO));
	memset(&proci,0,sizeof(PROCESS_INFORMATION));
	sti.cb=sizeof(STARTUPINFO);
	GetCurrentDirectory(sizeof(currentDir),currentDir);
	PX_strcat(currentDir,"/bin");

	PX_sprintf1(startup_param,sizeof(startup_param),"server %1",PX_STRINGFORMAT_INT(pDesc->gameport));

	processRet=CreateProcess("./bin/PainterEngine Game Server.exe",startup_param,PX_NULL,PX_NULL,FALSE,PX_NULL,PX_NULL,currentDir,&sti,&proci);
	if (processRet)
	{
		CloseHandle(proci.hThread);
		WaitForSingleObject(proci.hProcess,INFINITE);
		pDesc->hGameProcess=PX_NULL;
	}
	
	pDesc->activated=PX_FALSE;
	PX_memset(pDesc,0,sizeof(PX_StartupServer_Lobby_GameInstance));
	return 0;
}

static px_void PX_StartupServer_LobbyUpdateClientUserInfo(PX_StartupServer_Lobby *pDesc,PX_StartupServer_LobbyClient *pClient)
{
	PX_StartupServer_Database_ReadUserData(pDesc->pDataBase,pClient->account,&pClient->userdata);
}

static px_void PX_LobbyServerGame_ClientData2PlayerDesc(PX_StartupServer_Lobby *pDesc,PX_StartupServer_LobbyClient *pClient,PX_GameStartUp_PlayerDataDesc *pPlayerDataDesc)
{
	px_int shipIndex;
	PX_Json_Value *pJsonValue;
	PX_memset(pPlayerDataDesc,0,sizeof(PX_GameStartUp_PlayerDataDesc));

	shipIndex=pClient->userState.shipIndex;
	if (shipIndex>=PX_COUNTOF(pClient->userdata.ShipStack)||pClient->userdata.ShipStack[shipIndex].unlock==PX_FALSE)
	{
		pClient->userState.shipIndex=0;
		shipIndex=0;
	}
	PX_strset(pPlayerDataDesc->account,pClient->account);
	PX_memcpy(pPlayerDataDesc->NickName,pClient->userdata.nickname,sizeof(pPlayerDataDesc->NickName));
	PX_memcpy(pPlayerDataDesc->Photo_Data,pClient->photoData,sizeof(pClient->photoData));


	switch(pClient->userdata.ShipStack[shipIndex].type)
	{
	case PX_STARTUP_SHIP_TYPE_ALPHA:
		{
			pJsonValue=PX_JsonGetValue(&pDesc->json_config,"ship_info.alpha.base_weapon_generator_time");
			pPlayerDataDesc->base_weapon_generator_time=(px_dword)(pJsonValue?pJsonValue->_number:100);
			pJsonValue=PX_JsonGetValue(&pDesc->json_config,"ship_info.alpha.maxforce");
			pPlayerDataDesc->maxforce=(px_float)(pJsonValue?pJsonValue->_number:100);
			pJsonValue=PX_JsonGetValue(&pDesc->json_config,"ship_info.alpha.maxlife");
			pPlayerDataDesc->maxlife=(px_float)(pJsonValue?pJsonValue->_number:100);
			pJsonValue=PX_JsonGetValue(&pDesc->json_config,"ship_info.alpha.maxshield");
			pPlayerDataDesc->maxshield=(px_float)(pJsonValue?pJsonValue->_number:100);
			pJsonValue=PX_JsonGetValue(&pDesc->json_config,"ship_info.alpha.maxspeed");
			pPlayerDataDesc->maxspeed=(px_float)(pJsonValue?pJsonValue->_number:100);
			pJsonValue=PX_JsonGetValue(&pDesc->json_config,"ship_info.alpha.last_fire_recovery");
			pPlayerDataDesc->last_fire_recovery=(px_dword)(pJsonValue?pJsonValue->_number:100);
			pJsonValue=PX_JsonGetValue(&pDesc->json_config,"ship_info.alpha.slientTime");
			pPlayerDataDesc->slientTime=(px_dword)(pJsonValue?pJsonValue->_number:10000);
			
		}
		break;
	default:
		pPlayerDataDesc->base_weapon_generator_time=200;
		pPlayerDataDesc->group=0;
		pPlayerDataDesc->maxforce=100;
		pPlayerDataDesc->maxlife=200;
		pPlayerDataDesc->maxshield=100;
		pPlayerDataDesc->maxspeed=200;
		pPlayerDataDesc->last_fire_recovery=3000;
		pPlayerDataDesc->slientTime=10000;
		pPlayerDataDesc->Armor=0;
		break;
	}
	//upgrade
	pPlayerDataDesc->base_weapon_generator_time-=(pPlayerDataDesc->base_weapon_generator_time/50*(px_int)pClient->userdata.ShipStack[shipIndex].upgrade_ammo_generator);
	pPlayerDataDesc->last_fire_recovery-=(pPlayerDataDesc->last_fire_recovery/50*(px_int)pClient->userdata.ShipStack[shipIndex].upgrade_lastfirerecovery);

	pPlayerDataDesc->maxforce+=(pPlayerDataDesc->maxforce/50*(px_int)pClient->userdata.ShipStack[shipIndex].upgrade_force);
	pPlayerDataDesc->maxlife+=(pPlayerDataDesc->maxlife/50*(px_int)pClient->userdata.ShipStack[shipIndex].upgrade_life);
	pPlayerDataDesc->maxshield+=(pPlayerDataDesc->maxshield/50*(px_int)pClient->userdata.ShipStack[shipIndex].upgrade_shield);
	pPlayerDataDesc->maxspeed+=(pPlayerDataDesc->maxshield/50*(px_int)pClient->userdata.ShipStack[shipIndex].upgrade_speed);
	

	//shiptype
	pPlayerDataDesc->Armor=0;
	pPlayerDataDesc->group=0;
	pPlayerDataDesc->shiptype=pClient->userdata.ShipStack[shipIndex].type;
}

static px_void PX_LobbyServerGameStart(PX_StartupServer_Lobby *pDesc,PX_StartupServer_Lobby_GameInstance *pGameInstance)
{
	switch (pGameInstance->type)
	{
	case GAME_PLAY_GAMETYPE_1P:
		{
			px_int i;
			typedef struct  
			{
				PX_GameServerStartUp_Info startupInfo;
				PX_GameServerStartUp_PlayerNetInfo netInfo[1];
				PX_GameStartUp_GameDesc gamedesc;
				PX_GameStartUp_PlayerDataDesc playerDesc[1];
			}serverstartup;
			serverstartup startup_param;
			startup_param.startupInfo.dataport=pGameInstance->dataport;
			startup_param.startupInfo.gameport=pGameInstance->gameport;
			startup_param.startupInfo.playerCount=PX_COUNTOF(startup_param.playerDesc);

			for (i=0;i<(px_int)startup_param.startupInfo.playerCount;i++)
			{
				px_int mapid=pGameInstance->PlayerStack[i].mapid;
				startup_param.netInfo[i].clientID=pGameInstance->clientIDArrary[i];
				startup_param.netInfo[i].serverID=pGameInstance->serverIDArrary[i];
			}

			startup_param.gamedesc.mode=GAME_PLAY_GAMETYPE_1P;
			
			for (i=0;i<(px_int)startup_param.startupInfo.playerCount;i++)
			{
				px_int mapid=pGameInstance->PlayerStack[i].mapid;
				PX_LobbyServerGame_ClientData2PlayerDesc(pDesc,&pDesc->clients[mapid],&startup_param.playerDesc[i]);
				startup_param.playerDesc[i].group=0;
			}

			do 
			{
				FILE *pf;
				px_char paramPath[MAX_PATH];
				PX_sprintf1(paramPath,sizeof(paramPath),"./bin/%1",PX_STRINGFORMAT_INT(pGameInstance->gameport));
				pf=fopen(paramPath,"wb");
				fwrite(&startup_param,1,sizeof(startup_param),pf);
				fclose(pf);
			} while (0);
		}
		break;
	case GAME_PLAY_GAMETYPE_2P:
		{
			px_int i;
			typedef struct  
			{
				PX_GameServerStartUp_Info startupInfo;
				PX_GameServerStartUp_PlayerNetInfo netInfo[2];
				PX_GameStartUp_GameDesc gamedesc;
				PX_GameStartUp_PlayerDataDesc playerDesc[2];
			}serverstartup;
			serverstartup startup_param;
			startup_param.startupInfo.dataport=pGameInstance->dataport;
			startup_param.startupInfo.gameport=pGameInstance->gameport;
			startup_param.startupInfo.playerCount=PX_COUNTOF(startup_param.playerDesc);

			for (i=0;i<(px_int)startup_param.startupInfo.playerCount;i++)
			{
				px_int mapid=pGameInstance->PlayerStack[i].mapid;
				startup_param.netInfo[i].clientID=pGameInstance->clientIDArrary[i];
				startup_param.netInfo[i].serverID=pGameInstance->serverIDArrary[i];
			}

			startup_param.gamedesc.mode=GAME_PLAY_GAMETYPE_2P;

			for (i=0;i<(px_int)startup_param.startupInfo.playerCount;i++)
			{
				px_int mapid=pGameInstance->PlayerStack[i].mapid;
				PX_LobbyServerGame_ClientData2PlayerDesc(pDesc,&pDesc->clients[mapid],&startup_param.playerDesc[i]);
				startup_param.playerDesc[i].group=i;
			}

			do 
			{
				FILE *pf;
				px_char paramPath[MAX_PATH];
				PX_sprintf1(paramPath,sizeof(paramPath),"./bin/%1",PX_STRINGFORMAT_INT(pGameInstance->gameport));
				pf=fopen(paramPath,"wb");
				fwrite(&startup_param,1,sizeof(startup_param),pf);
				fclose(pf);
			} while (0);
		}
		break;
	case GAME_PLAY_GAMETYPE_4P:
		{
			px_int i;
			typedef struct  
			{
				PX_GameServerStartUp_Info startupInfo;
				PX_GameServerStartUp_PlayerNetInfo netInfo[4];
				PX_GameStartUp_GameDesc gamedesc;
				PX_GameStartUp_PlayerDataDesc playerDesc[4];
			}serverstartup;
			serverstartup startup_param;
			startup_param.startupInfo.dataport=pGameInstance->dataport;
			startup_param.startupInfo.gameport=pGameInstance->gameport;
			startup_param.startupInfo.playerCount=PX_COUNTOF(startup_param.playerDesc);

			for (i=0;i<(px_int)startup_param.startupInfo.playerCount;i++)
			{
				px_int mapid=pGameInstance->PlayerStack[i].mapid;
				startup_param.netInfo[i].clientID=pGameInstance->clientIDArrary[i];
				startup_param.netInfo[i].serverID=pGameInstance->serverIDArrary[i];
			}

			startup_param.gamedesc.mode=GAME_PLAY_GAMETYPE_4P;

			for (i=0;i<(px_int)startup_param.startupInfo.playerCount;i++)
			{
				px_int mapid=pGameInstance->PlayerStack[i].mapid;
				PX_LobbyServerGame_ClientData2PlayerDesc(pDesc,&pDesc->clients[mapid],&startup_param.playerDesc[i]);
				startup_param.playerDesc[i].group=i;
			}

			do 
			{
				FILE *pf;
				px_char paramPath[MAX_PATH];
				PX_sprintf1(paramPath,sizeof(paramPath),"./bin/%1",PX_STRINGFORMAT_INT(pGameInstance->gameport));
				pf=fopen(paramPath,"wb");
				fwrite(&startup_param,1,sizeof(startup_param),pf);
				fclose(pf);
			} while (0);
		}
		break;
	case GAME_PLAY_GAMETYPE_6P:
		{
			px_int i;
			typedef struct  
			{
				PX_GameServerStartUp_Info startupInfo;
				PX_GameServerStartUp_PlayerNetInfo netInfo[6];
				PX_GameStartUp_GameDesc gamedesc;
				PX_GameStartUp_PlayerDataDesc playerDesc[6];
			}serverstartup;
			serverstartup startup_param;
			startup_param.startupInfo.dataport=pGameInstance->dataport;
			startup_param.startupInfo.gameport=pGameInstance->gameport;
			startup_param.startupInfo.playerCount=PX_COUNTOF(startup_param.playerDesc);

			for (i=0;i<(px_int)startup_param.startupInfo.playerCount;i++)
			{
				px_int mapid=pGameInstance->PlayerStack[i].mapid;
				startup_param.netInfo[i].clientID=pGameInstance->clientIDArrary[i];
				startup_param.netInfo[i].serverID=pGameInstance->serverIDArrary[i];
			}

			startup_param.gamedesc.mode=GAME_PLAY_GAMETYPE_6P;

			for (i=0;i<(px_int)startup_param.startupInfo.playerCount;i++)
			{
				px_int mapid=pGameInstance->PlayerStack[i].mapid;
				PX_LobbyServerGame_ClientData2PlayerDesc(pDesc,&pDesc->clients[mapid],&startup_param.playerDesc[i]);
				startup_param.playerDesc[i].group=i;
			}

			do 
			{
				FILE *pf;
				px_char paramPath[MAX_PATH];
				PX_sprintf1(paramPath,sizeof(paramPath),"./bin/%1",PX_STRINGFORMAT_INT(pGameInstance->gameport));
				pf=fopen(paramPath,"wb");
				fwrite(&startup_param,1,sizeof(startup_param),pf);
				fclose(pf);
			} while (0);
		}
		break;
	case GAME_PLAY_GAMETYPE_8P:
		{
			px_int i;
			typedef struct  
			{
				PX_GameServerStartUp_Info startupInfo;
				PX_GameServerStartUp_PlayerNetInfo netInfo[8];
				PX_GameStartUp_GameDesc gamedesc;
				PX_GameStartUp_PlayerDataDesc playerDesc[8];
			}serverstartup;
			serverstartup startup_param;
			startup_param.startupInfo.dataport=pGameInstance->dataport;
			startup_param.startupInfo.gameport=pGameInstance->gameport;
			startup_param.startupInfo.playerCount=PX_COUNTOF(startup_param.playerDesc);

			for (i=0;i<(px_int)startup_param.startupInfo.playerCount;i++)
			{
				px_int mapid=pGameInstance->PlayerStack[i].mapid;
				startup_param.netInfo[i].clientID=pGameInstance->clientIDArrary[i];
				startup_param.netInfo[i].serverID=pGameInstance->serverIDArrary[i];
			}

			startup_param.gamedesc.mode=GAME_PLAY_GAMETYPE_8P;

			for (i=0;i<(px_int)startup_param.startupInfo.playerCount;i++)
			{
				px_int mapid=pGameInstance->PlayerStack[i].mapid;
				PX_LobbyServerGame_ClientData2PlayerDesc(pDesc,&pDesc->clients[mapid],&startup_param.playerDesc[i]);
				startup_param.playerDesc[i].group=i;
			}

			do 
			{
				FILE *pf;
				px_char paramPath[MAX_PATH];
				PX_sprintf1(paramPath,sizeof(paramPath),"./bin/%1",PX_STRINGFORMAT_INT(pGameInstance->gameport));
				pf=fopen(paramPath,"wb");
				fwrite(&startup_param,1,sizeof(startup_param),pf);
				fclose(pf);
			} while (0);
		}
		break;
	case GAME_PLAY_GAMETYPE_2V2:
		{
			px_int i;
			typedef struct  
			{
				PX_GameServerStartUp_Info startupInfo;
				PX_GameServerStartUp_PlayerNetInfo netInfo[4];
				PX_GameStartUp_GameDesc gamedesc;
				PX_GameStartUp_PlayerDataDesc playerDesc[4];
			}serverstartup;
			serverstartup startup_param;
			startup_param.startupInfo.dataport=pGameInstance->dataport;
			startup_param.startupInfo.gameport=pGameInstance->gameport;
			startup_param.startupInfo.playerCount=PX_COUNTOF(startup_param.playerDesc);

			for (i=0;i<(px_int)startup_param.startupInfo.playerCount;i++)
			{
				px_int mapid=pGameInstance->PlayerStack[i].mapid;
				startup_param.netInfo[i].clientID=pGameInstance->clientIDArrary[i];
				startup_param.netInfo[i].serverID=pGameInstance->serverIDArrary[i];
			}

			startup_param.gamedesc.mode=GAME_PLAY_GAMETYPE_2V2;

			for (i=0;i<(px_int)startup_param.startupInfo.playerCount;i++)
			{
				px_int mapid=pGameInstance->PlayerStack[i].mapid;
				PX_LobbyServerGame_ClientData2PlayerDesc(pDesc,&pDesc->clients[mapid],&startup_param.playerDesc[i]);
				if (i<(px_int)startup_param.startupInfo.playerCount/2)
				{
					startup_param.playerDesc[i].group=0;
				}
				else
				{
					startup_param.playerDesc[i].group=1;
				}
				
			}

			do 
			{
				FILE *pf;
				px_char paramPath[MAX_PATH];
				PX_sprintf1(paramPath,sizeof(paramPath),"./bin/%1",PX_STRINGFORMAT_INT(pGameInstance->gameport));
				pf=fopen(paramPath,"wb");
				fwrite(&startup_param,1,sizeof(startup_param),pf);
				fclose(pf);
			} while (0);
		}
		break;
	case GAME_PLAY_GAMETYPE_3V3:
		{
			px_int i;
			typedef struct  
			{
				PX_GameServerStartUp_Info startupInfo;
				PX_GameServerStartUp_PlayerNetInfo netInfo[6];
				PX_GameStartUp_GameDesc gamedesc;
				PX_GameStartUp_PlayerDataDesc playerDesc[6];
			}serverstartup;
			serverstartup startup_param;
			startup_param.startupInfo.dataport=pGameInstance->dataport;
			startup_param.startupInfo.gameport=pGameInstance->gameport;
			startup_param.startupInfo.playerCount=PX_COUNTOF(startup_param.playerDesc);

			for (i=0;i<(px_int)startup_param.startupInfo.playerCount;i++)
			{
				px_int mapid=pGameInstance->PlayerStack[i].mapid;
				startup_param.netInfo[i].clientID=pGameInstance->clientIDArrary[i];
				startup_param.netInfo[i].serverID=pGameInstance->serverIDArrary[i];
			}

			startup_param.gamedesc.mode=GAME_PLAY_GAMETYPE_3V3;

			for (i=0;i<(px_int)startup_param.startupInfo.playerCount;i++)
			{
				px_int mapid=pGameInstance->PlayerStack[i].mapid;
				PX_LobbyServerGame_ClientData2PlayerDesc(pDesc,&pDesc->clients[mapid],&startup_param.playerDesc[i]);
				if (i<(px_int)startup_param.startupInfo.playerCount/2)
				{
					startup_param.playerDesc[i].group=0;
				}
				else
				{
					startup_param.playerDesc[i].group=1;
				}

			}

			do 
			{
				FILE *pf;
				px_char paramPath[MAX_PATH];
				PX_sprintf1(paramPath,sizeof(paramPath),"./bin/%1",PX_STRINGFORMAT_INT(pGameInstance->gameport));
				pf=fopen(paramPath,"wb");
				fwrite(&startup_param,1,sizeof(startup_param),pf);
				fclose(pf);
			} while (0);
		}
		break;
	case GAME_PLAY_GAMETYPE_4V4:
		{
			px_int i;
			typedef struct  
			{
				PX_GameServerStartUp_Info startupInfo;
				PX_GameServerStartUp_PlayerNetInfo netInfo[8];
				PX_GameStartUp_GameDesc gamedesc;
				PX_GameStartUp_PlayerDataDesc playerDesc[8];
			}serverstartup;
			serverstartup startup_param;
			startup_param.startupInfo.dataport=pGameInstance->dataport;
			startup_param.startupInfo.gameport=pGameInstance->gameport;
			startup_param.startupInfo.playerCount=PX_COUNTOF(startup_param.playerDesc);

			for (i=0;i<(px_int)startup_param.startupInfo.playerCount;i++)
			{
				px_int mapid=pGameInstance->PlayerStack[i].mapid;
				startup_param.netInfo[i].clientID=pGameInstance->clientIDArrary[i];
				startup_param.netInfo[i].serverID=pGameInstance->serverIDArrary[i];
			}

			startup_param.gamedesc.mode=GAME_PLAY_GAMETYPE_4V4;

			for (i=0;i<(px_int)startup_param.startupInfo.playerCount;i++)
			{
				px_int mapid=pGameInstance->PlayerStack[i].mapid;
				PX_LobbyServerGame_ClientData2PlayerDesc(pDesc,&pDesc->clients[mapid],&startup_param.playerDesc[i]);
				if (i<(px_int)startup_param.startupInfo.playerCount/2)
				{
					startup_param.playerDesc[i].group=0;
				}
				else
				{
					startup_param.playerDesc[i].group=1;
				}

			}

			do 
			{
				FILE *pf;
				px_char paramPath[MAX_PATH];
				PX_sprintf1(paramPath,sizeof(paramPath),"./bin/%1",PX_STRINGFORMAT_INT(pGameInstance->gameport));
				pf=fopen(paramPath,"wb");
				fwrite(&startup_param,1,sizeof(startup_param),pf);
				fclose(pf);
			} while (0);
		}
		break;
	case GAME_PLAY_GAMETYPE_5V5:
		{
			px_int i;
			typedef struct  
			{
				PX_GameServerStartUp_Info startupInfo;
				PX_GameServerStartUp_PlayerNetInfo netInfo[10];
				PX_GameStartUp_GameDesc gamedesc;
				PX_GameStartUp_PlayerDataDesc playerDesc[10];
			}serverstartup;
			serverstartup startup_param;
			startup_param.startupInfo.dataport=pGameInstance->dataport;
			startup_param.startupInfo.gameport=pGameInstance->gameport;
			startup_param.startupInfo.playerCount=PX_COUNTOF(startup_param.playerDesc);

			for (i=0;i<(px_int)startup_param.startupInfo.playerCount;i++)
			{
				px_int mapid=pGameInstance->PlayerStack[i].mapid;
				startup_param.netInfo[i].clientID=pGameInstance->clientIDArrary[i];
				startup_param.netInfo[i].serverID=pGameInstance->serverIDArrary[i];
			}

			startup_param.gamedesc.mode=GAME_PLAY_GAMETYPE_5V5;

			for (i=0;i<(px_int)startup_param.startupInfo.playerCount;i++)
			{
				px_int mapid=pGameInstance->PlayerStack[i].mapid;
				PX_LobbyServerGame_ClientData2PlayerDesc(pDesc,&pDesc->clients[mapid],&startup_param.playerDesc[i]);
				if (i<(px_int)startup_param.startupInfo.playerCount/2)
				{
					startup_param.playerDesc[i].group=0;
				}
				else
				{
					startup_param.playerDesc[i].group=1;
				}

			}

			do 
			{
				FILE *pf;
				px_char paramPath[MAX_PATH];
				PX_sprintf1(paramPath,sizeof(paramPath),"./bin/%1",PX_STRINGFORMAT_INT(pGameInstance->gameport));
				pf=fopen(paramPath,"wb");
				fwrite(&startup_param,1,sizeof(startup_param),pf);
				fclose(pf);
			} while (0);
		}
		break;
	}
	if (pGameInstance->hGameProcess==PX_NULL)
	{
		DWORD threadId;
		pGameInstance->hGameProcess=CreateThread(NULL, 0, PX_LobbyServerGameStartThread, pGameInstance, 0, &threadId);
	}

}

static px_int PX_StartupSever_GetGameTypePlayerCount(GAME_PLAY_GAMETYPE type)
{
	switch(type)
	{
	case GAME_PLAY_GAMETYPE_8P:
		{
			return 8;
		}
		break;
	case GAME_PLAY_GAMETYPE_6P:
		{
			return 6;
		}
		break;
	case  GAME_PLAY_GAMETYPE_4P:
		{
			return 4;
		}
		break;
	case GAME_PLAY_GAMETYPE_2P:
		{
			return 2;
		}
		break;
	case GAME_PLAY_GAMETYPE_1P:
		{
			return 1;
		}
		break;
	case GAME_PLAY_GAMETYPE_5V5:
		{
			return 10;
		}
		break;
	case GAME_PLAY_GAMETYPE_4V4:
		{
			return 8;
		}
		break;
	case GAME_PLAY_GAMETYPE_3V3:
		{
			return 6;
		}
		break;
	case GAME_PLAY_GAMETYPE_2V2:
		{
			return 2;
		}
		break;
	}
	return 0;
}

px_bool PX_StartupServer_LobbyInitialize(PX_StartupServer_Lobby *pDesc,PX_StartupServer_Database *pDataBase,px_int max_connections)
{
	PX_memset(pDesc,0,sizeof(PX_StartupServer_Lobby));
	pDesc->mp=MP_Create(malloc(1024*1024*8),1024*1024*8);
	pDesc->pDataBase=pDataBase;

	if (!PX_UDPInit(&pDesc->udp,PX_UDP_IP_TYPE_IPV4))
	{
		PX_Application_ConsolePrint(PX_STARTUPSERVER_LOBBY_TEXT_INITIALIZE_LOBBY_UDP_FAILED);
		goto _ERROR;
	}

	if (!PX_UDPListen(&pDesc->udp,PX_STARTUPSERVER_LOBBY_LISTEN_PORT))
	{
		PX_Application_ConsolePrint(PX_STARTUPSERVER_LOBBY_TEXT_LISTEN_LOBBY_UDP_FAILED);
		PX_UDPFree(&pDesc->udp);
		goto _ERROR;
	}

	if (!PX_UDPInit(&pDesc->udp_gameresult,PX_UDP_IP_TYPE_IPV4))
	{
		PX_Application_ConsolePrint(PX_STARTUPSERVER_LOBBY_TEXT_INITIALIZE_LOBBY_DATAUDP_FAILED);
		goto _ERROR;
	}

	if (!PX_UDPListen(&pDesc->udp_gameresult,PAINTERENGINE_GAMESERVER_RESULT_PORT))
	{
		PX_Application_ConsolePrint(PX_STARTUPSERVER_LOBBY_TEXT_LISTEN_LOBBY_DATAUDP_FAILED);
		PX_UDPFree(&pDesc->udp);
		goto _ERROR;
	}

	if (!PX_UDPInit(&pDesc->udp_largeData,PX_UDP_IP_TYPE_IPV4))
	{
		PX_Application_ConsolePrint(PX_STARTUPSERVER_LOBBY_TEXT_INITIALIZE_LOBBY_LARGEDATAUDP_FAILED);
		goto _ERROR;
	}

	if (!PX_UDPListen(&pDesc->udp_largeData,PX_STARTUPSERVER_LOBBY_LARGEDATA_LISTEN_PORT))
	{
		PX_Application_ConsolePrint(PX_STARTUPSERVER_LOBBY_TEXT_LISTEN_LOBBY__LARGEDATAUDP_FAILED);
		PX_UDPFree(&pDesc->udp);
		PX_UDPFree(&pDesc->udp_gameresult);
		goto _ERROR;
	}

	if(!PX_JsonInitialize(&pDesc->mp,&pDesc->json_config)) goto _ERROR;
	if(!PX_LoadJsonFromFile(&pDesc->json_config,"./database/config.json")) goto _ERROR;

	

	return PX_TRUE;
_ERROR:
	return PX_FALSE;
}

px_bool PX_StartupServer_LobbyLogin(PX_StartupServer_Lobby *pDesc,px_byte aes_code[32],px_char account[32],px_char pwd[32],_OUT px_byte cookie[32],_OUT px_dword *map_index,_OUT px_dword *id)
{
	PX_SHA256_HASH shahash;
	px_int i,j;

	PX_Sha256Calculate(pwd,32,&shahash);

	for (i=0;i<PX_COUNTOF(pDesc->clients);i++)
	{
		if (pDesc->clients[i].activated)
		{
			if (PX_strequ(pDesc->clients[i].account,account))
			{

				if (PX_memequ(shahash.bytes,pDesc->clients[i].pwd_sha256,sizeof(pDesc->clients[i].pwd_sha256)))
				{
					PX_memcpy(pDesc->clients[i].aes_code,aes_code,sizeof(pDesc->clients[i].aes_code));
					PX_AES_Initialize(&pDesc->clients[i].aes_encoder,PX_AES_KeySize_Bits256,aes_code);

					//lock id
					//pDesc->clients[i].id;
					PX_memcpy(cookie,pDesc->clients[i].cookie,sizeof(pDesc->clients[i].cookie));

					//return
					*id=pDesc->clients[i].id;
					*map_index=i;
					return PX_TRUE;
				}
				return PX_FALSE;
			}
		}
	}

	if (!PX_StartupServer_Database_AccountIsRegisted(pDesc->pDataBase,account))
	{
		return PX_FALSE;
	}

	for (i=0;i<PX_COUNTOF(pDesc->clients);i++)
	{
		if (!pDesc->clients[i].activated)
		{
			if (!PX_StartupServer_Database_AccountVerify(pDesc->pDataBase,account,pwd))
			{
				goto _ERROR;
			}
			if (!PX_StartupServer_Database_ReadUserData(pDesc->pDataBase,account,&pDesc->clients[i].userdata))
			{
				goto _ERROR;
			}
			if (!PX_StartupServer_Database_ReadPhoto(pDesc->pDataBase,account,pDesc->clients[i].photoData))
			{
				goto _ERROR;
			}

			PX_memcpy(pDesc->clients[i].account,account,sizeof(pDesc->clients[i].account));
			PX_memcpy(pDesc->clients[i].aes_code,aes_code,sizeof(pDesc->clients[i].aes_code));
			PX_AES_Initialize(&pDesc->clients[i].aes_encoder,PX_AES_KeySize_Bits256,pDesc->clients[i].aes_code);
			pDesc->clients[i].lastop_elpased=0;
			pDesc->clients[i].activated=PX_TRUE;
			pDesc->clients[i].hb_elpased=0;
			pDesc->clients[i].id=(pDesc->allocID+=PX_rand());
			pDesc->clients[i].userState.serverid=0;
			pDesc->clients[i].userState.clientid=0;

			for(j=0;j<sizeof(pDesc->clients[i].cookie)-1;j++)
			{
				pDesc->clients[i].cookie[j]=PX_rand()%0xff;
				if (pDesc->clients[i].cookie[j]==0)
				{
					pDesc->clients[i].cookie[j]=1;
				}
			}
			pDesc->clients[i].cookie[j]=0;
			PX_memcpy(cookie,pDesc->clients[i].cookie,sizeof(pDesc->clients[i].cookie));
			PX_memcpy(pDesc->clients[i].pwd_sha256,shahash.bytes,sizeof(pDesc->clients[i].pwd_sha256));

			*id=pDesc->clients[i].id;
			*map_index=i;

			do 
			{
				px_char message[64];
				PX_sprintf1(message,sizeof(message),"User login:%1",PX_STRINGFORMAT_STRING(account));
				PX_Application_ConsolePrint(message);
			} while (0);

			break;
		}
	}
	return PX_TRUE;
_ERROR:
	return PX_FALSE;
}

px_bool PX_StartupServer_LobbyUpdateClientUserInfoByAccount(PX_StartupServer_Lobby *pDesc,px_char account[])
{
	px_int i;
	for (i=0;i<PX_COUNTOF(pDesc->clients);i++)
	{
		if (pDesc->clients[i].activated)
		{
			if (PX_strequ(account,pDesc->clients[i].account))
			{
				PX_StartupServer_Database_ReadUserData(pDesc->pDataBase,account,&pDesc->clients[i].userdata);
				return PX_TRUE;
			}
		}
	}
		return PX_FALSE;
}

static px_void PX_StartupServer_LobbyUpdate_HeartBeat(PX_StartupServer_Lobby *pDesc,PX_UDP_ADDR addr,px_byte data[],px_int size)
{
	PX_Lobby_Packet_Header pack;
	PX_Lobby_Packet_Command *pPack=(PX_Lobby_Packet_Command *)data;
	pDesc->clients[pPack->header.mapid].hb_elpased=0;
	pack.magic=PX_NETWORK_LOBBY_MAGIC;
	pack.id=pDesc->clients[pPack->header.mapid].id;
	pack.size=0;
	pack.type=PX_NETWORK_LOBBY_TYPE_HEARTBEATACK;
	PX_UDPSend(&pDesc->udp,addr,(px_byte *)&pack,sizeof(pack));
}

static px_void PX_StartupServer_LobbyUpdate_QueryUserInfo(PX_StartupServer_Lobby *pDesc,PX_UDP_ADDR addr,px_byte data[],px_int size)
{
	px_byte Cache[2048];
	px_memorypool mp=MP_Create(Cache,sizeof(Cache));
	px_memory packBuildMemory;
	PX_Lobby_Packet_Header pack;
	PX_Lobby_Packet_Command *pPack=(PX_Lobby_Packet_Command *)data;

	PX_MemoryInit(&mp,&packBuildMemory);

	pack.magic=PX_NETWORK_LOBBY_MAGIC;
	pack.id=pDesc->clients[pPack->header.mapid].id;
	pack.size=sizeof(pDesc->clients[0].userdata);
	pack.type=PX_NETWORK_LOBBY_TYPE_QUERYUSERINFOACK;

	PX_MemoryCat(&packBuildMemory,&pack,sizeof(pack));
	PX_MemoryCat(&packBuildMemory,&pDesc->clients[pPack->header.mapid].userdata,sizeof(pDesc->clients[0].userdata));

	PX_UDPSend(&pDesc->udp,addr,packBuildMemory.buffer,packBuildMemory.usedsize);
}

static px_void PX_StartupServer_LobbyUpdate_QueryPhoto(PX_StartupServer_Lobby *pDesc,PX_UDP_ADDR addr,px_byte data[],px_int size)
{
	px_byte Cache[8192];
	px_memorypool mp=MP_Create(Cache,sizeof(Cache));
	px_memory packBuildMemory;
	PX_Lobby_Packet_Header pack;
	PX_Lobby_Packet_Command *pPack=(PX_Lobby_Packet_Command *)data;
	px_dword index;
	

	index=pPack->content.param[0];

	if (index>PX_STARTUP_USERDATA_PHOTOSIZE/1024)
	{
		return;
	}

	//size is not used
	size=PX_STARTUP_USERDATA_PHOTOSIZE-index*1024;

	if (size>1024)
	{
		size=1024;
	}

	PX_MemoryInit(&mp,&packBuildMemory);

	pack.magic=PX_NETWORK_LOBBY_MAGIC;
	pack.param=index;
	pack.size=size;
	pack.type=PX_NETWORK_LOBBY_TYPE_QUERYPHOTOACK;

	
	PX_MemoryCat(&packBuildMemory,&pack,sizeof(pack));
	PX_MemoryCat(&packBuildMemory,pDesc->clients[pPack->header.mapid].photoData+index*1024,size);

	PX_UDPSend(&pDesc->udp,addr,packBuildMemory.buffer,packBuildMemory.usedsize);
}

static px_void PX_StartupServer_LobbyUpdate_QueryState(PX_StartupServer_Lobby *pDesc,PX_UDP_ADDR addr,px_byte data[],px_int size)
{
	px_byte Cache[8192];
	px_memorypool mp=MP_Create(Cache,sizeof(Cache));
	px_memory packBuildMemory;
	PX_Lobby_Packet_Header pack;
	PX_Lobby_Packet_Command *pPack=(PX_Lobby_Packet_Command *)data;

	//size is not used
	size=sizeof(PX_Startup_UserState);


	PX_MemoryInit(&mp,&packBuildMemory);

	pack.magic=PX_NETWORK_LOBBY_MAGIC;
	pack.param=0;
	pack.size=size;
	pack.type=PX_NETWORK_LOBBY_TYPE_QUERYSTATEACK;


	PX_MemoryCat(&packBuildMemory,&pack,sizeof(pack));
	PX_MemoryCat(&packBuildMemory,&pDesc->clients[pPack->header.mapid].userState,size);

	PX_UDPSend(&pDesc->udp,addr,packBuildMemory.buffer,packBuildMemory.usedsize);
}


static px_void PX_StartupServer_LobbyUpdate_QueryNews(PX_StartupServer_Lobby *pDesc,PX_UDP_ADDR addr,px_byte data[],px_int size)
{
	typedef struct
	{
		PX_Lobby_Packet_Header header;
		px_char news[PX_LOBBY_NEWS_SIZE];
	}PX_Lobby_Packet_news;

	PX_Lobby_Packet_news pack;

	//size is not used
	size=sizeof(pack.news);
	pack.header.magic=PX_NETWORK_LOBBY_MAGIC;
	pack.header.param=0;
	pack.header.size=size;
	pack.header.type=PX_NETWORK_LOBBY_TYPE_QUERYNEWSACK;
	PX_memcpy(pack.news,pDesc->News,sizeof(pack.news));

	PX_UDPSend(&pDesc->udp,addr,&pack,sizeof(pack));
}


static px_bool PX_StartupServer_LobbyPlayerIsInGaming(PX_StartupServer_Lobby *pDesc,px_int id)
{
	px_int i,j;

	for (i=0;i<PX_COUNTOF(pDesc->GameInstance);i++)
	{
		if (pDesc->GameInstance[i].activated)
		{
			for (j=0;j<PX_COUNTOF(pDesc->GameInstance[i].PlayerStack);j++)
			{
				if (pDesc->GameInstance[i].PlayerStack[j].id==id)
				{
					return PX_TRUE;
				}
			}

		}
	}

	return PX_FALSE;
}

static px_bool PX_StartupServer_LobbyPlayerIsInRanking(PX_StartupServer_Lobby *pDesc,px_int id)
{
	px_int i,j;

	for (i=0;i<PX_COUNTOF(pDesc->GameRoom);i++)
	{
		if (pDesc->GameRoom[i].activated)
		{
			for (j=0;j<PX_COUNTOF(pDesc->GameRoom[i].PlayerStack);j++)
			{
				if (pDesc->GameRoom[i].PlayerStack[j].id==id)
				{
					return PX_TRUE;
				}
			}
			
		}
	}
	return PX_FALSE;
}

static px_bool PX_StartupServer_LobbyClientPayCoin(PX_StartupServer_Lobby *pDesc,PX_StartupServer_LobbyClient *pClient,px_int paycount)
{
	if ((px_int)pClient->userdata.coin>=paycount)
	{
		pClient->userdata.coin-=paycount;
		PX_StartupServer_Database_WriteUserData(pDesc->pDataBase,pClient->account,&pClient->userdata);
		return PX_TRUE;
	}
	return PX_FALSE;
}

static px_bool PX_StartupServer_LobbyClientPayTicket(PX_StartupServer_Lobby *pDesc,PX_StartupServer_LobbyClient *pClient,px_int paycount)
{
	if ((px_int)pClient->userdata.ticket>=paycount)
	{
		pClient->userdata.ticket-=paycount;
		PX_StartupServer_Database_WriteUserData(pDesc->pDataBase,pClient->account,&pClient->userdata);
		return PX_TRUE;
	}
	return PX_FALSE;
}

static px_void PX_StartupServer_LobbyUpdate_CancelMatch(PX_StartupServer_Lobby *pDesc,PX_UDP_ADDR addr,px_byte data[],px_int size)
{
	PX_AES aes;
	PX_Lobby_Packet_Command *pPack=(PX_Lobby_Packet_Command *)data;
	px_int i,j;

	//check client state
	if (!pDesc->clients[pPack->header.mapid].activated)
	{
		return;
	}

	PX_AES_Initialize(&aes,PX_AES_KeySize_Bits256,pDesc->clients[pPack->header.mapid].aes_code);
	PX_AES_InvCipherBuffer(&aes,(px_byte *)&pPack->content,sizeof(pPack->content),(px_byte *)&pPack->content);

	if (!PX_memequ(pDesc->clients[pPack->header.mapid].cookie,pPack->content.cookie,sizeof(pPack->content.cookie)))
	{
		return;
	}

	for (i=0;i<PX_COUNTOF(pDesc->GameRoom);i++)
	{
		if (pDesc->GameRoom[i].activated)
		{
			for(j=0;j<PX_COUNTOF(pDesc->GameRoom[i].PlayerStack);j++)
			{
				if (pDesc->GameRoom[i].PlayerStack[j].activated)
				{
					if (pDesc->GameRoom[i].PlayerStack[j].id==pDesc->clients[pPack->header.mapid].id)
					{
						pDesc->GameRoom[i].PlayerStack[j].activated=PX_FALSE;
						pDesc->GameRoom[i].PlayerStack[j].id=0;
						pDesc->GameRoom[i].PlayerStack[j].mapid=0;
						do 
						{
							px_char message[64];
							PX_sprintf2(message,sizeof(message),"User %1 leave the room %2 ",PX_STRINGFORMAT_STRING(pDesc->clients[pDesc->GameRoom[i].PlayerStack[j].mapid].account),PX_STRINGFORMAT_INT(i));
							PX_Application_ConsolePrint(message);
						} while (0);
					}
				}
			}
		}
	}

}

static px_void PX_StartupServer_LobbyUpdate_StartGame(PX_StartupServer_Lobby *pDesc,PX_UDP_ADDR addr,px_byte data[],px_int size)
{
	PX_AES aes;
	PX_Lobby_Packet_Command *pPack=(PX_Lobby_Packet_Command *)data;
	px_int i;
	px_int playerCount;

	//check client state
	if (!pDesc->clients[pPack->header.mapid].activated)
	{
		return;
	}

	if (PX_StartupServer_LobbyPlayerIsInRanking(pDesc,pDesc->clients[pPack->header.mapid].id))
	{
		return;
	}

	if (PX_StartupServer_LobbyPlayerIsInGaming(pDesc,pDesc->clients[pPack->header.mapid].id))
	{
		return;
	}

	PX_AES_Initialize(&aes,PX_AES_KeySize_Bits256,pDesc->clients[pPack->header.mapid].aes_code);
	PX_AES_InvCipherBuffer(&aes,(px_byte *)&pPack->content,sizeof(pPack->content),(px_byte *)&pPack->content);

	if (!PX_memequ(pDesc->clients[pPack->header.mapid].cookie,pPack->content.cookie,sizeof(pPack->content.cookie)))
	{
		return;
	}

	PX_StartupServer_LobbyUpdateClientUserInfo(pDesc,&pDesc->clients[pPack->header.mapid]);

	if (pPack->content.param[0]!=GAME_PLAY_GAMETYPE_1P)
	{
		px_int ticket=playerCount=PX_StartupSever_GetGameTypePlayerCount((GAME_PLAY_GAMETYPE)pPack->content.param[0]);
		if ((px_int)pDesc->clients[pPack->header.mapid].userdata.ticket<ticket/2)
		{
			return;
		}
	}
	

	//add
	for (i=0;i<PX_COUNTOF(pDesc->GameRoom);i++)
	{
		if (pDesc->GameRoom[i].activated)
		{
			if (pDesc->GameRoom[i].type==pPack->content.param[0]&&pPack->content.param[1]==pDesc->GameRoom[i].roomKey)
			{
				px_int j;

				playerCount=PX_StartupSever_GetGameTypePlayerCount(pDesc->GameRoom[i].type);
				
				for (j=0;j<playerCount;j++)
				{
					if (!pDesc->GameRoom[i].PlayerStack[j].activated)
					{
						pDesc->GameRoom[i].PlayerStack[j].activated=PX_TRUE;
						pDesc->GameRoom[i].PlayerStack[j].id=pDesc->clients[pPack->header.mapid].id;
						pDesc->GameRoom[i].PlayerStack[j].mapid=pPack->header.mapid;
						do 
						{
							px_char message[64];
							PX_sprintf2(message,sizeof(message),"User %1 join the room %2 ",PX_STRINGFORMAT_STRING(pDesc->clients[pDesc->GameRoom[i].PlayerStack[j].mapid].account),PX_STRINGFORMAT_INT(i));
							PX_Application_ConsolePrint(message);
						} while (0);
						return;
					}
				}
			}
		}
	}

	//new room
	for (i=0;i<PX_COUNTOF(pDesc->GameRoom);i++)
	{
		if (!pDesc->GameRoom[i].activated)
		{
			PX_memset(&pDesc->GameRoom[i],0,sizeof(pDesc->GameRoom[i]));
			//room
			pDesc->GameRoom[i].type=(GAME_PLAY_GAMETYPE)pPack->content.param[0];
			pDesc->GameRoom[i].roomKey=pDesc->GameRoom[i].roomKey;
			pDesc->GameRoom[i].activated=PX_TRUE;
			
			//user
				pDesc->GameRoom[i].PlayerStack[0].activated=PX_TRUE;
				pDesc->GameRoom[i].PlayerStack[0].id=pDesc->clients[pPack->header.mapid].id;
				pDesc->GameRoom[i].PlayerStack[0].mapid=pPack->header.mapid;
				do 
				{
					px_char message[64];
					PX_sprintf2(message,sizeof(message),"User %1 join the new room %2 ",PX_STRINGFORMAT_STRING(pDesc->clients[pDesc->GameRoom[i].PlayerStack[0].mapid].account),PX_STRINGFORMAT_INT(i));
					PX_Application_ConsolePrint(message);
				} while (0);
			break;
		}
	}

}

static px_void PX_StartupServer_LobbyUpdate_Upgrade(PX_StartupServer_Lobby *pDesc,PX_UDP_ADDR addr,px_byte data[],px_int size)
{
	PX_AES aes;
	PX_Lobby_Packet_Command *pPack=(PX_Lobby_Packet_Command *)data;
	PX_StartupServer_LobbyClient *pClient;
	px_int shipIndex;
	//check client state
	if (!pDesc->clients[pPack->header.mapid].activated)
	{
		return;
	}

	PX_AES_Initialize(&aes,PX_AES_KeySize_Bits256,pDesc->clients[pPack->header.mapid].aes_code);
	PX_AES_InvCipherBuffer(&aes,(px_byte *)&pPack->content,sizeof(pPack->content),(px_byte *)&pPack->content);

	if (!PX_memequ(pDesc->clients[pPack->header.mapid].cookie,pPack->content.cookie,sizeof(pPack->content.cookie)))
	{
		return;
	}

	shipIndex=pDesc->clients[pPack->header.mapid].userState.shipIndex;
	if (shipIndex>=PX_COUNTOF(pDesc->clients[pPack->header.mapid].userdata.ShipStack))
	{
		return;
	}
	if (pDesc->clients[pPack->header.mapid].userdata.ShipStack[shipIndex].unlock==PX_FALSE)
	{
		return;
	}

	pClient=&pDesc->clients[pPack->header.mapid];
	switch(pPack->header.type)
	{
	case PX_NETWORK_LOBBY_TYPE_UPGRADE_AMMOGEN:
		{
			px_int level=pClient->userdata.ShipStack[shipIndex].upgrade_ammo_generator;
			if (pClient->userdata.ShipStack[shipIndex].upgrade_ammo_generator<10)
			{
				if (PX_StartupServer_LobbyClientPayCoin(pDesc,pClient,100*(1<<level)))
				{
					pClient->userdata.ShipStack[shipIndex].upgrade_ammo_generator++;
					PX_StartupServer_Database_WriteUserData(pDesc->pDataBase,pClient->account,&pClient->userdata);
				}
			}
		}
		break;
	case PX_NETWORK_LOBBY_TYPE_UPGRADE_AMMOREC:
		{
			px_int level=pClient->userdata.ShipStack[shipIndex].upgrade_lastfirerecovery;
			if (pClient->userdata.ShipStack[shipIndex].upgrade_lastfirerecovery<10)
			{
				if (PX_StartupServer_LobbyClientPayCoin(pDesc,pClient,100*(1<<level)))
				{
					pClient->userdata.ShipStack[shipIndex].upgrade_lastfirerecovery++;
					PX_StartupServer_Database_WriteUserData(pDesc->pDataBase,pClient->account,&pClient->userdata);
				}
			}
		}
		break;
	case PX_NETWORK_LOBBY_TYPE_UPGRADE_MAXFORCE:
		{
			px_int level=pClient->userdata.ShipStack[shipIndex].upgrade_force;
			if (pClient->userdata.ShipStack[shipIndex].upgrade_force<10)
			{
				if (PX_StartupServer_LobbyClientPayCoin(pDesc,pClient,100*(1<<level)))
				{
					pClient->userdata.ShipStack[shipIndex].upgrade_force++;
					PX_StartupServer_Database_WriteUserData(pDesc->pDataBase,pClient->account,&pClient->userdata);
				}
			}
		}
		break;
	case PX_NETWORK_LOBBY_TYPE_UPGRADE_MAXLIFE:
		{
			px_int level=pClient->userdata.ShipStack[shipIndex].upgrade_life;
			if (pClient->userdata.ShipStack[shipIndex].upgrade_life<10)
			{
				if (PX_StartupServer_LobbyClientPayCoin(pDesc,pClient,100*(1<<level)))
				{
					pClient->userdata.ShipStack[shipIndex].upgrade_life++;
					PX_StartupServer_Database_WriteUserData(pDesc->pDataBase,pClient->account,&pClient->userdata);
				}
			}
		}
		break;
	case PX_NETWORK_LOBBY_TYPE_UPGRADE_MAXSHIELD:
		{
			px_int level=pClient->userdata.ShipStack[shipIndex].upgrade_shield;
			if (pClient->userdata.ShipStack[shipIndex].upgrade_shield<10)
			{
				if (PX_StartupServer_LobbyClientPayCoin(pDesc,pClient,100*(1<<level)))
				{
					pClient->userdata.ShipStack[shipIndex].upgrade_shield++;
					PX_StartupServer_Database_WriteUserData(pDesc->pDataBase,pClient->account,&pClient->userdata);
				}
			}
		}
		break;
	case PX_NETWORK_LOBBY_TYPE_UPGRADE_MAXSPEED:
		{
			px_int level=pClient->userdata.ShipStack[shipIndex].upgrade_speed;
			if (pClient->userdata.ShipStack[shipIndex].upgrade_speed<10)
			{
				if (PX_StartupServer_LobbyClientPayCoin(pDesc,pClient,100*(1<<level)))
				{
					pClient->userdata.ShipStack[shipIndex].upgrade_speed++;
					PX_StartupServer_Database_WriteUserData(pDesc->pDataBase,pClient->account,&pClient->userdata);
				}
			}
		}
		break;
	}
}


static px_void PX_StartupServer_LobbyUpdate_BuyTicket(PX_StartupServer_Lobby *pDesc,PX_UDP_ADDR addr,px_byte data[],px_int size)
{
	PX_AES aes;
	PX_Lobby_Packet_Command *pPack=(PX_Lobby_Packet_Command *)data;
	PX_StartupServer_LobbyClient *pClient;

	//check client state
	if (!pDesc->clients[pPack->header.mapid].activated)
	{
		return;
	}

	PX_AES_Initialize(&aes,PX_AES_KeySize_Bits256,pDesc->clients[pPack->header.mapid].aes_code);
	PX_AES_InvCipherBuffer(&aes,(px_byte *)&pPack->content,sizeof(pPack->content),(px_byte *)&pPack->content);

	if (!PX_memequ(pDesc->clients[pPack->header.mapid].cookie,pPack->content.cookie,sizeof(pPack->content.cookie)))
	{
		return;
	}


	pClient=&pDesc->clients[pPack->header.mapid];

	if (PX_StartupServer_LobbyClientPayCoin(pDesc,pClient,1000))
	{
		pClient->userdata.ticket+=10;
		PX_StartupServer_Database_WriteUserData(pDesc->pDataBase,pClient->account,&pClient->userdata);
	}

}

px_void PX_StartupServer_LobbyUpdate_Packets(PX_StartupServer_Lobby *pDesc,PX_UDP_ADDR addr,px_byte data[],px_int size)
{
	PX_Lobby_Packet_Command *pPack=(PX_Lobby_Packet_Command *)data;

	if (size!=sizeof(PX_Lobby_Packet_Command))
	{
		return;
	}

	if (pPack->header.magic!=PX_NETWORK_LOBBY_MAGIC)
	{
		return;
	}

	if (pPack->header.mapid>=PX_COUNTOF(pDesc->clients))
	{
		return;
	}

	if (pPack->header.id!=pDesc->clients[pPack->header.mapid].id)
	{
		return;
	}

	switch(pPack->header.type)
	{
	case PX_NETWORK_LOBBY_TYPE_HEARTBEAT:
		{
			PX_StartupServer_LobbyUpdate_HeartBeat(pDesc,addr,data,size);
		}
		break;
	case PX_NETWORK_LOBBY_TYPE_QUERYUSERINFO:
		{
			PX_StartupServer_LobbyUpdate_QueryUserInfo(pDesc,addr,data,size);
		}
		break;
	case PX_NETWORK_LOBBY_TYPE_QUERYPHOTO:
		{
			PX_StartupServer_LobbyUpdate_QueryPhoto(pDesc,addr,data,size);
		}
		break;
	case PX_NETWORK_LOBBY_TYPE_QUERYSTATE:
		{
			PX_StartupServer_LobbyUpdate_QueryState(pDesc,addr,data,size);
		}
		break;
	case PX_NETWORK_LOBBY_TYPE_QUERYNEWS:
		{
			PX_StartupServer_LobbyUpdate_QueryNews(pDesc,addr,data,size);
		}
		break;
	case PX_NETWORK_LOBBY_TYPE_STARTGAME:
		{
			PX_StartupServer_LobbyUpdate_StartGame(pDesc,addr,data,size);
		}
		break;
	case PX_NETWORK_LOBBY_TYPE_CANCELMATCH:
		{
			PX_StartupServer_LobbyUpdate_CancelMatch(pDesc,addr,data,size);
		}
		break;
	case PX_NETWORK_LOBBY_TYPE_UPGRADE_AMMOGEN:
	case PX_NETWORK_LOBBY_TYPE_UPGRADE_AMMOREC:
	case PX_NETWORK_LOBBY_TYPE_UPGRADE_MAXFORCE:
	case PX_NETWORK_LOBBY_TYPE_UPGRADE_MAXLIFE:
	case PX_NETWORK_LOBBY_TYPE_UPGRADE_MAXSHIELD:
	case PX_NETWORK_LOBBY_TYPE_UPGRADE_MAXSPEED:
		{
			PX_StartupServer_LobbyUpdate_Upgrade(pDesc,addr,data,size);
		}
		break;
	case PX_NETWORK_LOBBY_TYPE_BUY_TICKET:
		{
			PX_StartupServer_LobbyUpdate_BuyTicket(pDesc,addr,data,size);
		}
		break;
	}
}


px_void PX_StartupServer_LobbyUpdate_GameResultPackets(PX_StartupServer_Lobby *pDesc,PX_UDP_ADDR addr,px_byte data[],px_int size)
{
	PX_Startup_UserData userData;
	px_int playerCount;
	px_int i;
	px_char content[128];

	PX_GameServer_PlayerResult *pResult=(PX_GameServer_PlayerResult *)data;
	if (addr.ipv4!=0x0100007f)
	{
		return;
	}
	if (size!=sizeof(PX_GameServer_PlayerResult))
	{
		return;
	}
	playerCount=PX_StartupSever_GetGameTypePlayerCount((GAME_PLAY_GAMETYPE)pResult->gameType);
	
	if (playerCount<=0||(px_int)pResult->rank>playerCount)
	{
		return;
	}

	if(!PX_StartupServer_Database_ReadUserData(pDesc->pDataBase,pResult->account,&userData))
	{
		return;
	}

	
	switch(pResult->gameType)
	{
	case GAME_PLAY_GAMETYPE_2P:
	case GAME_PLAY_GAMETYPE_4P:
	case GAME_PLAY_GAMETYPE_6P:
	case GAME_PLAY_GAMETYPE_8P:
		{
			px_int pt;
			px_int coin;
			px_int ticket=playerCount/2;
			coin=(playerCount-pResult->rank)*100;
			if ((px_int)pResult->rank<=playerCount/2)
			{
				pt=playerCount/2*100-(pResult->rank-1)*100;
			}
			else
			{
				pt=-(((px_int)pResult->rank-(playerCount/2))*100);
			}


			PX_sprintf4(content,sizeof(content),"account:%1 rankpt:%2 coin:%3 ticket:%4",PX_STRINGFORMAT_STRING(pResult->account),PX_STRINGFORMAT_INT(pt),PX_STRINGFORMAT_INT(coin),PX_STRINGFORMAT_INT(ticket));
			PX_Application_ConsolePrint(content);
			
			userData.coin+=coin;

			if ((px_int)userData.ticket<ticket)
			{
				userData.ticket=0;
			}
			else
			{
				userData.ticket-=ticket;
			}

			if (pt<0)
			{
				if (-pt>(px_int)userData.rank_pt)
				{
					userData.rank_pt=0;
				}
				else
				{
					userData.rank_pt+=pt;
				}
			}
			else
			{
				userData.rank_pt+=pt;
			}
			

		}
		break;
	case GAME_PLAY_GAMETYPE_5V5:
	case GAME_PLAY_GAMETYPE_4V4:
	case GAME_PLAY_GAMETYPE_3V3:
	case GAME_PLAY_GAMETYPE_2V2:
		{
			px_int pt;
			px_int coin;
			px_int ticket=playerCount/2;
			coin=pResult->rank==1?playerCount/2*100:0;
			pt=pResult->rank==1?100:-100;

			PX_sprintf4(content,sizeof(content),"account:%1 rankpt:%2 coin:%3 ticket:%4",PX_STRINGFORMAT_STRING(pResult->account),PX_STRINGFORMAT_INT(pt),PX_STRINGFORMAT_INT(coin),PX_STRINGFORMAT_INT(ticket));
			PX_Application_ConsolePrint(content);

			if ((px_int)userData.ticket<ticket)
			{
				userData.ticket=0;
			}
			else
			{
				userData.ticket-=ticket;
			}

			if (pt<0)
			{
				if (-pt>(px_int)userData.rank_pt)
				{
					userData.rank_pt=0;
				}
				else
				{
					userData.rank_pt+=pt;
				}
			}
			else
			{
				userData.rank_pt+=pt;
			}

		}
		break;
	case GAME_PLAY_GAMETYPE_1P:
		{

		}
		break;
	}
	
	userData.match_count++;

	if(!PX_StartupServer_Database_WriteUserData(pDesc->pDataBase,pResult->account,&userData))
	{
		PX_StartupServer_Database_WriteBackupUserData(pDesc->pDataBase,pResult->account,&userData);
		return;
	}

	//update client
	for (i=0;i<PX_COUNTOF(pDesc->clients);i++)
	{
		if (pDesc->clients[i].activated)
		{
			if (PX_strequ(pDesc->clients[i].account,pResult->account))
			{
				PX_StartupServer_LobbyUpdateClientUserInfo(pDesc,&pDesc->clients[i]);

				PX_sprintf4(content,sizeof(content),"Update Account:%1 rankpt:%2 coin:%3 ticket:%4",\
					PX_STRINGFORMAT_STRING(pDesc->clients[i].account),\
					PX_STRINGFORMAT_INT(pDesc->clients[i].userdata.rank_pt),\
					PX_STRINGFORMAT_INT(pDesc->clients[i].userdata.coin),\
					PX_STRINGFORMAT_INT(pDesc->clients[i].userdata.ticket)\
					);
				PX_Application_ConsolePrint(content);
				
				
				break;
			}
		}
	}

}

static px_void PX_StartupServer_LobbyUpdate_UploadPhoto(PX_StartupServer_Lobby *pDesc,PX_UDP_ADDR addr,px_byte data[],px_int size)
{
	PX_TRaw_Header *pTrawHeader;
	PX_AES aes;
	PX_Lobby_Packet_LargeDataHeader *pPack=(PX_Lobby_Packet_LargeDataHeader *)data;
	px_byte *ptrawData;
	//check client state
	if (!pDesc->clients[pPack->mapid].activated)
	{
		return;
	}

	if (size-sizeof(PX_Lobby_Packet_LargeDataHeader)!=PX_STARTUP_USERDATA_PHOTOSIZE)
	{
		return;
	}

	PX_AES_Initialize(&aes,PX_AES_KeySize_Bits256,pDesc->clients[pPack->mapid].aes_code);
	PX_AES_InvCipherBuffer(&aes,(px_byte *)&pPack->cookie,sizeof(pPack->cookie),(px_byte *)&pPack->cookie);

	if (!PX_memequ(pDesc->clients[pPack->mapid].cookie,pPack->cookie,sizeof(pPack->cookie)))
	{
		return;
	}

	

	ptrawData=data+sizeof(PX_Lobby_Packet_LargeDataHeader);

	if (!PX_TRawVerify(ptrawData,PX_STARTUP_USERDATA_PHOTOSIZE))
	{
		return;
	}

	pTrawHeader=(PX_TRaw_Header *)ptrawData;
	if (pTrawHeader->Height!=64||pTrawHeader->Width!=64)
	{
		return;
	}

	if (!PX_StartupServer_LobbyClientPayCoin(pDesc,&pDesc->clients[pPack->mapid],12800))
	{
		return;
	}
	//copy data
	PX_memcpy(pDesc->clients[pPack->mapid].photoData,ptrawData,PX_STARTUP_USERDATA_PHOTOSIZE);
	PX_StartupServer_Database_WriteUserPhoto(pDesc->pDataBase,pDesc->clients[pPack->mapid].account,pDesc->clients[pPack->mapid].photoData,PX_STARTUP_USERDATA_PHOTOSIZE);



}
px_void PX_StartupServer_LobbyUpdate_LargeDataPackets(PX_StartupServer_Lobby *pDesc,PX_UDP_ADDR addr,px_byte data[],px_int size)
{
	PX_Lobby_Packet_LargeDataHeader *pPack=(PX_Lobby_Packet_LargeDataHeader *)data;

	if (size<sizeof(PX_Lobby_Packet_LargeDataHeader))
	{
		return;
	}

	if (pPack->magic!=PX_NETWORK_LOBBY_MAGIC)
	{
		return;
	}

	if (pPack->mapid>=PX_COUNTOF(pDesc->clients))
	{
		return;
	}

	if (pPack->id!=pDesc->clients[pPack->mapid].id)
	{
		return;
	}
	switch(pPack->type)
	{
	case PX_NETWORK_LOBBY_TYPE_UPLOADPHOTO:
		PX_StartupServer_LobbyUpdate_UploadPhoto(pDesc,addr,data,size);
		break;
	}

}
px_int PX_StartupServer_LobbyGetRoomReadyPlayers(PX_StartupServer_Lobby_GameRoom *room)
{
	px_int count=0;
	px_int i;
	for (i=0;i<PX_COUNTOF(room->PlayerStack);i++)
	{
		if (room->PlayerStack[i].activated)
		{
			count++;
		}
	}
	return count;
}


px_void PX_StartupServer_LobbyUpdateHeartBeat(PX_StartupServer_Lobby *pDesc,px_dword elpased)
{
	px_int i;

	for (i=0;i<PX_COUNTOF(pDesc->clients);i++)
	{
		if (pDesc->clients[i].activated)
		{
			//////////////////////////////////////////////////////////////////////////
			//HearBeat
			pDesc->clients[i].hb_elpased+=elpased;
			pDesc->clients[i].lastop_elpased+=elpased;
			if (pDesc->clients[i].hb_elpased>PX_STARTUPSERVER_LOBBY_CLIENT_TIMEOUT&&pDesc->clients[i].userState.userState!=PX_STARTUP_GAMESTATE_GAMING)
			{
				px_char message[64];
				pDesc->clients[i].activated=PX_FALSE;
				PX_sprintf1(message,sizeof(message),"user offline:%1",PX_STRINGFORMAT_STRING(pDesc->clients[i].account));
				PX_Application_ConsolePrint(message);
				PX_memset(&pDesc->clients[i],0,sizeof(pDesc->clients[i]));
			}
		}
	}
}

px_void PX_StartupServer_LobbyUpdateClientState(PX_StartupServer_Lobby *pDesc,px_dword elpased)
{
	px_int i;
	//////////////////////////////////////////////////////////////////////////
	//update clients state
	//clear state
	pDesc->OnlineCount=0;

	for (i=0;i<PX_COUNTOF(pDesc->clients);i++)
	{
		if (pDesc->clients[i].activated)
		{
			pDesc->OnlineCount++;
			pDesc->clients[i].userState.userState=PX_STARTUP_GAMESTATE_STANDBY;
		}
	}

	//online
	for (i=0;i<PX_COUNTOF(pDesc->clients);i++)
	{
		if (pDesc->clients[i].activated)
		{
			pDesc->clients[i].userState.game_onlineCount=pDesc->OnlineCount;
			PX_memcpy(pDesc->clients[i].userState.RoomPlayerCount,pDesc->roomPlayerCount,sizeof(pDesc->clients[i].userState.RoomPlayerCount));
		}
	}

	//room
	for (i=0;i<PX_COUNTOF(pDesc->GameRoom);i++)
	{
		if (pDesc->GameRoom[i].activated)
		{
			px_int j;
			for (j=0;j<PX_COUNTOF(pDesc->GameRoom[i].PlayerStack);j++)
			{
				if (pDesc->GameRoom[i].PlayerStack[j].activated)
				{
					px_int mapIndex=pDesc->GameRoom[i].PlayerStack[j].mapid;
					px_int id=pDesc->GameRoom[i].PlayerStack[j].id;

					if (pDesc->clients[mapIndex].activated)
					{
						if (pDesc->clients[mapIndex].id==id)
						{
							pDesc->clients[mapIndex].userState.userState=PX_STARTUP_GAMESTATE_RANKING;
							pDesc->clients[mapIndex].userState.game_port=0;
							pDesc->clients[mapIndex].userState.data_port=0;
							pDesc->clients[mapIndex].userState.clientid=0;
							pDesc->clients[mapIndex].userState.serverid=0;
							pDesc->clients[mapIndex].userState.game_readyPlayers=PX_StartupServer_LobbyGetRoomReadyPlayers(&pDesc->GameRoom[i]);
							
						}
					}
				}
			}
			
		}
	}


	for (i=0;i<PX_COUNTOF(pDesc->GameInstance);i++)
	{
		if (pDesc->GameInstance[i].activated)
		{
			px_int j;
			for (j=0;j<PX_COUNTOF(pDesc->GameInstance[i].PlayerStack);j++)
			{
				if (pDesc->GameInstance[i].PlayerStack[j].activated)
				{
					px_int mapIndex=pDesc->GameInstance[i].PlayerStack[j].mapid;
					px_int id=pDesc->GameInstance[i].PlayerStack[j].id;

					if (pDesc->clients[mapIndex].activated)
					{
						if (pDesc->clients[mapIndex].id==id)
						{
							pDesc->clients[mapIndex].userState.userState=PX_STARTUP_GAMESTATE_GAMING;
							pDesc->clients[mapIndex].userState.game_port=pDesc->GameInstance[i].gameport;
							pDesc->clients[mapIndex].userState.data_port=pDesc->GameInstance[i].dataport;
							pDesc->clients[mapIndex].userState.clientid=pDesc->GameInstance[i].clientIDArrary[j];
							pDesc->clients[mapIndex].userState.serverid=pDesc->GameInstance[i].serverIDArrary[j];
						}
					}
				}
			}
			
		}
	}

	//////////////////////////////////////////////////////////////////////////
}




px_void PX_StartupServer_LobbyUpdateRoomGameInstance_Room_StartInstance(PX_StartupServer_Lobby *pDesc,PX_StartupServer_Lobby_GameRoom *pRoom)
{
	px_int i;
	//GameServer Startup
	for (i=0;i<PX_COUNTOF(pDesc->GameInstance);i++)
	{
		if (!pDesc->GameInstance[i].activated)
		{
			px_int j;
			

			pDesc->GameInstance[i].activated=PX_TRUE;

			PX_memcpy(pDesc->GameInstance[i].PlayerStack,pRoom->PlayerStack,sizeof(pRoom->PlayerStack));
			//client server id alloc
			for (j=0;j<PX_COUNTOF(pDesc->GameInstance[i].clientIDArrary);j++)
			{
				pDesc->GameInstance[i].clientIDArrary[j]=((PX_rand()&0x7fff)<<16)+(PX_rand()&0xff00)+i*2;
				pDesc->GameInstance[i].serverIDArrary[j]=((PX_rand()&0x7fff)<<16)+(PX_rand()&0xff00)+i*2+1;
			}



			pDesc->GameInstance[i].gameport=PX_STARTUPSERVER_LOBBY_STARTPORT+i*8;
			pDesc->GameInstance[i].dataport=PX_STARTUPSERVER_LOBBY_STARTPORT+i*8+1;
			pDesc->GameInstance[i].type=pRoom->type;
			pDesc->GameInstance[i].hGameProcess=PX_NULL;//CreateProcess




			PX_LobbyServerGameStart(pDesc,&pDesc->GameInstance[i]);

			break;
		}
	}

	PX_memset(pRoom,0,sizeof(PX_StartupServer_Lobby_GameRoom));

}

px_void PX_StartupServer_LobbyUpdateRoomGameInstance_Room(PX_StartupServer_Lobby *pDesc,PX_StartupServer_Lobby_GameRoom *pRoom,px_int roomID)
{
	px_int i;

	px_int PlayerCount;
	px_bool StartGame;

	if (!pRoom->activated)
	{
		return;
	}

	if (PX_StartupServer_LobbyGetRoomReadyPlayers(pRoom)==0)
	{
		do 
		{
			px_char message[64];
			PX_sprintf2(message,sizeof(message),"Room Clear %1 Type %2",PX_STRINGFORMAT_INT(roomID),PX_STRINGFORMAT_INT(pRoom->type));
			PX_Application_ConsolePrint(message);
		} while (0);

		PX_memset(pRoom,0,sizeof(PX_StartupServer_Lobby_GameRoom));
		return;
	}

	//offline or error
	for (i=0;i<PX_COUNTOF(pRoom->PlayerStack);i++)
	{
		if (pRoom->PlayerStack[i].activated)
		{
			if (pRoom->PlayerStack[i].mapid>=PX_COUNTOF(pDesc->clients))
			{
				do 
				{
					px_char message[64];
					PX_sprintf2(message,sizeof(message),"Room %1 error:%2",PX_STRINGFORMAT_INT(roomID),PX_STRINGFORMAT_INT(i));
					PX_Application_ConsolePrint(message);
				} while (0);
				PX_memset(&pRoom->PlayerStack[i],0,sizeof(pRoom->PlayerStack[i]));
			}
			if (!pDesc->clients[pRoom->PlayerStack[i].mapid].activated||pDesc->clients[pRoom->PlayerStack[i].mapid].id!=pRoom->PlayerStack[i].id)
			{
				do 
				{
					px_char message[64];
					PX_sprintf2(message,sizeof(message),"Room %1 offline:%2",PX_STRINGFORMAT_INT(roomID),PX_STRINGFORMAT_INT(i));
					PX_Application_ConsolePrint(message);
				} while (0);
				PX_memset(&pRoom->PlayerStack[i],0,sizeof(pRoom->PlayerStack[i]));
			}
		}
	}

	StartGame=PX_FALSE;
	PlayerCount=PX_StartupServer_LobbyGetRoomReadyPlayers(pRoom);
	StartGame=(PX_StartupSever_GetGameTypePlayerCount(pRoom->type)==PlayerCount);

	if (StartGame)
	{
		do 
		{
			px_char message[64];
			PX_sprintf1(message,sizeof(message),"Room Completed:%1",PX_STRINGFORMAT_INT(roomID));
			PX_Application_ConsolePrint(message);
		} while (0);

		PX_StartupServer_LobbyUpdateRoomGameInstance_Room_StartInstance(pDesc,pRoom);
	}

}
px_void PX_StartupServer_LobbyUpdateRoomGameInstance(PX_StartupServer_Lobby *pDesc,px_dword elpased)
{
	px_int i;	
	PX_memset(pDesc->roomPlayerCount,0,sizeof(pDesc->roomPlayerCount));
	for (i=0;i<PX_COUNTOF(pDesc->GameRoom);i++)
	{
		if (pDesc->GameRoom[i].activated)
		{
			pDesc->roomPlayerCount[pDesc->GameRoom[i].type]+=PX_StartupServer_LobbyGetRoomReadyPlayers(&pDesc->GameRoom[i]);
			PX_StartupServer_LobbyUpdateRoomGameInstance_Room(pDesc,&pDesc->GameRoom[i],i);
		}
		
	}
}

px_void PX_StartupServer_LobbyUpdate(PX_StartupServer_Lobby *pDesc,px_dword elpased)
{
	px_int readsize;
	px_byte data[65536];
	PX_UDP_ADDR addr;
	
	PX_StartupServer_LobbyUpdateHeartBeat(pDesc,elpased);
	PX_StartupServer_LobbyUpdateClientState(pDesc,elpased);
	PX_StartupServer_LobbyUpdateRoomGameInstance(pDesc,elpased);

	while (PX_UDPReceived(&pDesc->udp,&addr,data,PX_NETWORK_PACKET_MAX_SIZE,&readsize))
	{
		PX_StartupServer_LobbyUpdate_Packets(pDesc,addr,data,readsize);
	}

	while (PX_UDPReceived(&pDesc->udp_gameresult,&addr,data,PX_NETWORK_PACKET_MAX_SIZE,&readsize))
	{
		PX_StartupServer_LobbyUpdate_GameResultPackets(pDesc,addr,data,readsize);
	}

	while (PX_UDPReceived(&pDesc->udp_largeData,&addr,data,sizeof(data),&readsize))
	{
		PX_StartupServer_LobbyUpdate_LargeDataPackets(pDesc,addr,data,readsize);
	}
}
