#include "GameLoginServer_Lobby.h"
#include "time.h"

extern px_void GameLoginServer_ConsolePrint(const px_char *Text);

px_bool GameLoginServer_LobbyInitialize(GameLoginServer_Lobby *plb,GameServer_Database *linker_Database,px_int max_client)
{
	px_int i;
	plb->alloc_id=0;
	plb->linker_Database=linker_Database;
	plb->mp=MP_Create(malloc(max_client*256+128*1024),max_client*256+128*1024);
	if (plb->mp.StartAddr=PX_NULL)
	{
		goto _ERROR;
	}
	plb->max_client=max_client;
	if(!PX_VectorInit(&plb->mp,&plb->clients,sizeof(GameLoginServer_Lobby_Client),max_client))
	{
		return PX_FALSE;
	}
	PX_VectorAllocSize(&plb->clients,max_client);
	for (i=0;i<plb->clients.size;i++)
	{
		PX_memset(PX_VECTORAT(GameLoginServer_Lobby_Client,&plb->clients,i),0,sizeof(GameLoginServer_Lobby_Client));
		PX_VECTORAT(GameLoginServer_Lobby_Client,&plb->clients,i)->last_chat_packetid=-1;
	}
	
	if (!PX_UDPInit(&plb->udp,PX_UDP_IP_TYPE_IPV4))
	{
		GameLoginServer_ConsolePrint(GAMELOGINSERVER_TEXT_INITIALIZE_LOBBY_UDP_FAILED);
		goto _ERROR;
	}

	if (!PX_UDPListen(&plb->udp,DEMO_SERVERLOBBY_LISTEN_PORT))
	{
		GameLoginServer_ConsolePrint(GAMELOGINSERVER_TEXT_LISTEN_LOBBY_UDP_FAILED);
		PX_UDPFree(&plb->udp);
		goto _ERROR;
	}


	plb->chatid=1;
	plb->wchat_index=0;
	PX_memset(plb->chatData,0,sizeof(plb->chatData));
	return PX_TRUE;
_ERROR:
	if (plb->mp.StartAddr!=PX_NULL)
	{
		free(plb->mp.StartAddr);
	}
	return PX_FALSE;
}
static px_void DEMO_ServerLobbyOnRecvHandle_HeartBeat(GameLoginServer_Lobby *plb,PX_UDP_ADDR faddr,px_byte *data,px_int size)
{
	DEMO_ClientLobby_Packet_HeartBeat *phb=(DEMO_ClientLobby_Packet_HeartBeat *)data;
	DEMO_ServerLobby_Packet_HeartBeatAck hbAck;
	GameLoginServer_Lobby_Client *pClient;

	if (size!=sizeof(DEMO_ClientLobby_Packet_HeartBeat))
	{
		return;
	}
	if (phb->mymapid>=GAMELOGINSERVER_LOBBY_MAXONLINE)
	{
		return;
	}
	pClient=PX_VECTORAT(GameLoginServer_Lobby_Client,&plb->clients,phb->mymapid);
	if (!pClient->activated||pClient->id!=phb->myid)
	{
		return;
	}
	pClient->last_hb_elpased=0;
	hbAck.magic=PX_NETWORK_HEARTBEATACK_MAGIC;

	PX_UDPSend(&plb->udp,faddr,(px_byte *)&hbAck,sizeof(hbAck));
}

static px_void DEMO_ServerLobbyOnRecvHandle_onChatMessage(GameLoginServer_Lobby *plb,PX_UDP_ADDR faddr,px_byte *data,px_int mapIndex)
{
	GameLoginServer_Lobby_Client *pClient;
	DEMO_ServerChat_MessagePacketAck messageOkPack;
	DEMO_ClientChat_MessagePacket *pMessagePacket=(DEMO_ClientChat_MessagePacket *)data;
	
	pClient=PX_VECTORAT(GameLoginServer_Lobby_Client,&plb->clients,mapIndex);
	if (pClient->last_chat_time<GAMELOGINSERVER_LOBBY_MINUTE_HCAT_TIME)
	{
		return;
	}
	pClient->last_chat_time=0;

	//relpy ok
	messageOkPack.magic=DEMO_NETWORK_CHATACK_MAGIC;
	messageOkPack.packetid=pMessagePacket->packetid;
	if (pMessagePacket->Message[sizeof(pMessagePacket->Message)-1]!=0||pMessagePacket->Message[0]==0)
	{
		return;
	}
	if (pClient->last_chat_packetid==pMessagePacket->packetid)
	{
		PX_UDPSend(&plb->udp,faddr,(px_byte *)&messageOkPack,sizeof(messageOkPack));
		return;
	}

	pClient->last_chat_packetid=pMessagePacket->packetid;

	PX_memcpy(plb->chatData[plb->wchat_index].account,pClient->userInfo.account,sizeof(plb->chatData->account));
	PX_memcpy(plb->chatData[plb->wchat_index].cookie,pClient->cookie,sizeof(plb->chatData->cookie));
	PX_memcpy(plb->chatData[plb->wchat_index].Name,pClient->userInfo.nickname,sizeof(plb->chatData->Name));
	PX_strcpy(plb->chatData[plb->wchat_index].Message,pMessagePacket->Message,sizeof(plb->chatData[plb->wchat_index].Message));
	PX_memcpy(plb->chatData[plb->wchat_index].profilePhoto_data_sha256,pClient->profilePhoto_data_sha256,sizeof(pClient->profilePhoto_data_sha256));
	plb->chatData[plb->wchat_index].level=pClient->userInfo.level;
	plb->chatData[plb->wchat_index].namecolor=pClient->userInfo.namecolor;
	plb->chatData[plb->wchat_index].nid=plb->chatid;
	plb->chatData[plb->wchat_index].userid=pClient->id;
	plb->chatData[plb->wchat_index].usermapid=mapIndex;
	plb->chatid++;
	plb->wchat_index++;
	if (plb->wchat_index>=PX_COUNTOF(plb->chatData))
	{
		plb->wchat_index=0;
	}
	PX_UDPSend(&plb->udp,faddr,(px_byte *)&messageOkPack,sizeof(messageOkPack));
		
}


static px_void DEMO_ServerLobbyOnRecvHandle_EncryptCode(GameLoginServer_Lobby *plb,PX_UDP_ADDR faddr,px_byte *data,px_int size)
{
	DEMO_NetWork_Encrypted_PacketHeader *phb=(DEMO_NetWork_Encrypted_PacketHeader *)data;
	DEMO_NetWork_Encrypted_Datainfo *pcryptInfo;
	px_byte *pRawData;
	DEMO_NetWork_Encrypted_Data *pDataHeader;
	GameLoginServer_Lobby_Client *pClient;
	px_int mapIndex;
	px_int encode_data_size;
	px_uint32 crc32,sum32;
	if (size<=sizeof(DEMO_NetWork_Encrypted_PacketHeader))
	{
		return;
	}
	if(phb->cookie[31]!=0) return;

	if (phb->mapid<(px_dword)plb->clients.size)
	{
		mapIndex=phb->mapid;
	}

	pClient=PX_VECTORAT(GameLoginServer_Lobby_Client,&plb->clients,mapIndex);
	if (!pClient->activated)
	{
		return;
	}

	if (pClient->id!=phb->myid)
	{
		return;
	}

	if (!PX_memequ(phb->cookie,pClient->cookie,sizeof(phb->cookie)))
	{
		return;
	}

	encode_data_size=size-(sizeof(DEMO_NetWork_Encrypted_PacketHeader));//calc header size
	if (encode_data_size%16)
	{
		// not 16 bytes aligned
		return;
	}

	if(!PX_AES_InvCipherBuffer(&pClient->aes_encoder,(px_byte *)data+sizeof(DEMO_NetWork_Encrypted_PacketHeader),encode_data_size,(px_byte *)data+sizeof(DEMO_NetWork_Encrypted_PacketHeader)))
		return;

	pcryptInfo=(DEMO_NetWork_Encrypted_Datainfo *)((px_byte *)data+sizeof(DEMO_NetWork_Encrypted_PacketHeader));
	//check size
	if (encode_data_size-sizeof(DEMO_NetWork_Encrypted_Datainfo)!=pcryptInfo->size)
	{
		return;
	}
	pRawData=((px_byte *)data+sizeof(DEMO_NetWork_Encrypted_PacketHeader)+sizeof(DEMO_NetWork_Encrypted_Datainfo));
	//verify crc32 &sum
	crc32=PX_crc32(pRawData,pcryptInfo->size);
	sum32=PX_sum32(pRawData,pcryptInfo->size);
	if (pcryptInfo->crc32!=crc32||pcryptInfo->sum!=sum32)
	{
		return;
	}

	//OK,go head
	pDataHeader=(DEMO_NetWork_Encrypted_Data *)pRawData;

	switch(pDataHeader->opcode)
	{
	case DEMO_NETWORK_LOBBY_OPCODE_CHAT:
		{
			DEMO_ServerLobbyOnRecvHandle_onChatMessage(plb,faddr,pRawData,mapIndex);
		}
		break;
	default:
		break;
	}

}


static px_void DEMO_ServerLobbyOnRecvHandle_Sync(GameLoginServer_Lobby *plb,PX_UDP_ADDR faddr,px_byte *data,px_int size)
{
	DEMO_ClientSync_Query_Packet *pQueryPacket=(DEMO_ClientSync_Query_Packet *)data;
	
	if (pQueryPacket->magic!=DEMO_NETWORKSYNC_MAGIC)
	{
		return;
	}
	switch(pQueryPacket->opcode)
	{
	case DEMO_SERVERSYNC_OPCODE_GETSIZE:
		{
			px_int index;
			DEMO_ServerSync_QuerySizeAck_Packet queryAck;
			GameLoginServer_Lobby_Client *pTargetClient,*pMyClient;

			if (pQueryPacket->mymapid<(px_dword)plb->clients.size)
			{
				index=pQueryPacket->mymapid;
			}
			else
			{
				return;
			}
			pMyClient=PX_VECTORAT(GameLoginServer_Lobby_Client,&plb->clients,index);
			if (!pMyClient->activated||pMyClient->id!=pQueryPacket->myid||!PX_memequ(pQueryPacket->cookie,pMyClient->cookie,sizeof(pQueryPacket->cookie)))
			{
				return;
			}

			if (pQueryPacket->target_mapid>=GAMELOGINSERVER_LOBBY_MAXONLINE)
			{
				return;
			}

			pTargetClient=PX_VECTORAT(GameLoginServer_Lobby_Client,&plb->clients,pQueryPacket->target_mapid);

			queryAck.magic=DEMO_NETWORKSYNC_MAGIC;
			queryAck.opcode=DEMO_SERVERSYNC_OPCODE_ACKGETSIZE;
			queryAck.queryid=pQueryPacket->queryId;
			queryAck.size=0;

			if (pTargetClient->activated)
			{
				if (pTargetClient->id==pQueryPacket->target_id)
				{
					switch(pQueryPacket->QueryType)
					{
					case DEMO_SERVERSYNC_QUERY_TYPE_USERINFO:
						{
							queryAck.size=sizeof(DEMO_ServerDatabase_userInfo);
						}
						break;
					case DEMO_SERVERSYNC_QUERY_TYPE_PROFILE_PHOTO:
						{
							queryAck.size=pTargetClient->profilePhoto_datasize;
						}
						break;
					case DEMO_SERVERSYNC_QUERY_TYPE_USERGAME_DATA:
						{
							queryAck.size=pTargetClient->UserGame_datasize;
						}
						break;
					}
					
				}
			}
			PX_UDPSend(&plb->udp,faddr,(px_byte *)&queryAck,sizeof(queryAck));
		}
		break;
	case DEMO_SERVERSYNC_OPCODE_GETDATA:
		{
			px_byte *data_ptr=PX_NULL;
			px_int datasize=0;
			px_int index;
			DEMO_ServerSync_QueryDataAck_Packet queryDataAck;
			GameLoginServer_Lobby_Client *pTargetClient,*pMyClient;

			if (pQueryPacket->mymapid<(px_dword)plb->clients.size)
			{
				index=pQueryPacket->mymapid;
			}
			else
			{
				return;
			}
			pMyClient=PX_VECTORAT(GameLoginServer_Lobby_Client,&plb->clients,index);
			if (!pMyClient->activated||pMyClient->id!=pQueryPacket->myid||!PX_memequ(pQueryPacket->cookie,pMyClient->cookie,sizeof(pQueryPacket->cookie)))
			{
				return;
			}

			if (pQueryPacket->target_mapid>=GAMELOGINSERVER_LOBBY_MAXONLINE)
			{
				return;
			}

			pTargetClient=PX_VECTORAT(GameLoginServer_Lobby_Client,&plb->clients,pQueryPacket->target_mapid);

			queryDataAck.magic=DEMO_NETWORKSYNC_MAGIC;
			queryDataAck.opcode=DEMO_SERVERSYNC_OPCODE_ACKGETDATA;
			queryDataAck.queryid=pQueryPacket->queryId;
			queryDataAck.size=0;


			if (pTargetClient->activated)
			{
				if (pTargetClient->id==pQueryPacket->target_id)
				{
					switch(pQueryPacket->QueryType)
					{
					case DEMO_SERVERSYNC_QUERY_TYPE_USERINFO:
						{
							data_ptr=(px_byte *)&pTargetClient->userInfo;
							datasize=sizeof(DEMO_ServerDatabase_userInfo);
						}
						break;
					case DEMO_SERVERSYNC_QUERY_TYPE_PROFILE_PHOTO:
						{
							data_ptr=pTargetClient->profilePhoto_data;
							datasize=pTargetClient->profilePhoto_datasize;
						}
						break;
					case DEMO_SERVERSYNC_QUERY_TYPE_USERGAME_DATA:
						{
							data_ptr=pTargetClient->UserGame_data;
							datasize=pTargetClient->UserGame_datasize;
						}
						break;
					default:
						datasize=0;
						break;
					}
					queryDataAck.block=pQueryPacket->queryBlock;
					if (pQueryPacket->queryBlock<=(px_dword)(datasize/DEMO_NETWORKSYNC_ACK_PACKET_BLOCK_SIZE))
					{
						px_int reqBlock=pQueryPacket->queryBlock;
						
						if((reqBlock+1)*DEMO_NETWORKSYNC_ACK_PACKET_BLOCK_SIZE<datasize)
						{
							queryDataAck.size=DEMO_NETWORKSYNC_ACK_PACKET_BLOCK_SIZE;
							PX_memset(queryDataAck.data,0,sizeof(queryDataAck.data));
							PX_memcpy(queryDataAck.data,(data_ptr)+reqBlock*DEMO_NETWORKSYNC_ACK_PACKET_BLOCK_SIZE,DEMO_NETWORKSYNC_ACK_PACKET_BLOCK_SIZE);
						}
						else
						{
							queryDataAck.size=datasize-reqBlock*DEMO_NETWORKSYNC_ACK_PACKET_BLOCK_SIZE;
							PX_memset(queryDataAck.data,0,sizeof(queryDataAck.data));
							PX_memcpy(queryDataAck.data,(data_ptr)+reqBlock*DEMO_NETWORKSYNC_ACK_PACKET_BLOCK_SIZE,datasize-reqBlock*DEMO_NETWORKSYNC_ACK_PACKET_BLOCK_SIZE);
						}	
					}
				}
			}
			if (queryDataAck.size==0)
			{
				PX_UDPSend(&plb->udp,faddr,(px_byte *)&queryDataAck,sizeof(queryDataAck));
			}
			else
			PX_UDPSend(&plb->udp,faddr,(px_byte *)&queryDataAck,sizeof(queryDataAck));
		}
		break;
	}

}

static px_void DEMO_ServerLobbyOnRecvHandle_ChatQueryInfo(GameLoginServer_Lobby *plb,PX_UDP_ADDR faddr,px_byte *data,px_int size)
{
	DEMO_ServerChat_QueryInfoAck ack;
	ack.magic=DEMO_NETWORK_CHAT_QUERYINFOACK_MAGIC;

	if(plb->wchat_index==0)
	ack.server_chatid=plb->chatData[PX_COUNTOF(plb->chatData)-1].nid;
	else
	ack.server_chatid=plb->chatData[plb->wchat_index-1].nid;
	PX_UDPSend(&plb->udp,faddr,(px_byte *)&ack,sizeof(ack));
}

static px_void DEMO_ServerLobbyOnRecvHandle_ChatQueryMessage(GameLoginServer_Lobby *plb,PX_UDP_ADDR faddr,px_byte *data,px_int size)
{
	DEMO_ClientChat_QueryMessage *pQm;
	DEMO_ServerChat_QueryMessageAck ack;
	px_int sid,dist;
	pQm=(DEMO_ClientChat_QueryMessage *)data;
	ack.header.magic=DEMO_NETWORK_CHAT_QUERYMESSAGEACK_MAGIC;


	if(plb->wchat_index==0)
		sid=PX_COUNTOF(plb->chatData)-1;
	else
		sid=plb->wchat_index-1;

	dist=plb->chatData[sid].nid-pQm->queryid;
	if (dist>=PX_COUNTOF(plb->chatData)||dist<0)
	{
		ack.header.nid=plb->chatData[sid].nid;
		ack.header.result=DEMO_SERVERCHAT_QUERYMESSAGEACK_RESULT_RESET;
		PX_UDPSend(&plb->udp,faddr,(px_byte *)&ack.header,sizeof(ack.header));
		return;
	}
	else
	{
		if (sid<dist)
		{
			sid+=PX_COUNTOF(plb->chatData);
		}
		sid-=dist;
		ack.header.nid=plb->chatData[sid].nid;
		ack.header.result=DEMO_SERVERCHAT_QUERYMESSAGEACK_RESULT_SUCCEEDED;
		PX_memcpy(ack.account,plb->chatData[sid].account,sizeof(ack.account));
		PX_memcpy(ack.Message,plb->chatData[sid].Message,sizeof(ack.Message));
		PX_memcpy(ack.Name,plb->chatData[sid].Name,sizeof(ack.Name));
		PX_memcpy(ack.profilePhoto_data_sha256,plb->chatData[sid].profilePhoto_data_sha256,sizeof(ack.profilePhoto_data_sha256));
		ack.level=plb->chatData[sid].level;
		ack.id=plb->chatData[sid].userid;
		ack.mapid=plb->chatData[sid].usermapid;
		ack.namecolor=plb->chatData[sid].namecolor;
		PX_UDPSend(&plb->udp,faddr,(px_byte *)&ack,sizeof(ack));
	}
	
}
static px_void DEMO_ServerLobbyOnRecvHandle(GameLoginServer_Lobby *plb,PX_UDP_ADDR faddr,px_byte *data,px_int size)
{
	GameLoginServer_Lobby_Packet_Header *pHeader=(GameLoginServer_Lobby_Packet_Header *)data;
	switch(pHeader->magic)
	{
	case DEMO_NETWORKSYNC_MAGIC:
		DEMO_ServerLobbyOnRecvHandle_Sync(plb,faddr,data,size);
		break;
	case PX_NETWORK_HEARTBEAT_MAGIC:
		DEMO_ServerLobbyOnRecvHandle_HeartBeat(plb,faddr,data,size);
		break;
	case DEMO_NETWORK_ENCRYPT_PACKET_MAGIC:
		DEMO_ServerLobbyOnRecvHandle_EncryptCode(plb,faddr,data,size);
		break;
	case DEMO_NETWORK_CHAT_QUERYINFO_MAGIC:
		DEMO_ServerLobbyOnRecvHandle_ChatQueryInfo(plb,faddr,data,size);
		break;
	case DEMO_NETWORK_CHAT_QUERYMESSAGE_MAGIC:
		DEMO_ServerLobbyOnRecvHandle_ChatQueryMessage(plb,faddr,data,size);
		break;
	default:
		break;
	}
}

px_void GameLoginServer_LobbyUpdate(GameLoginServer_Lobby *plb,px_dword elpased)
{
	px_int i,readsize;
	GameLoginServer_Lobby_Client *pClient;
	PX_UDP_ADDR faddr;
	time_t t;
	struct tm * lt;
	px_byte packet_data[PX_NETWORK_PACKET_MAX_SIZE];



	for (i=0;i<plb->clients.size;i++)
	{
		pClient=PX_VECTORAT(GameLoginServer_Lobby_Client,&plb->clients,i);
		if (pClient->activated)
		{
			pClient->last_op_time+=elpased;
			pClient->last_hb_elpased+=elpased;
			pClient->online_time+=elpased;
			pClient->last_chat_time+=elpased;

			if (pClient->last_op_time>=GAMELOGINSERVER_LOBBY_OP_OFFLINETIME)
			{
				GameLoginServer_LobbyClientRelease(plb,i);
				time (&t);
				lt = localtime (&t);
				//DEMO_Printf (DEMO_TEXT_USER_OFFLINE,pClient->userInfo.account,lt->tm_year+1900, lt->tm_mon, lt->tm_mday, lt->tm_hour, lt->tm_min, lt->tm_sec);
				continue;
			}

			if (pClient->last_hb_elpased>=GAMELOGINSERVER_LOBBY_HB_OFFLINETIME)
			{
				GameLoginServer_LobbyClientRelease(plb,i);
				time (&t);
				lt = localtime (&t);
				//DEMO_Printf (DEMO_TEXT_USER_OFFLINE,pClient->userInfo.account,lt->tm_year+1900, lt->tm_mon, lt->tm_mday, lt->tm_hour, lt->tm_min, lt->tm_sec);
				continue;
			}
		}
	}

	//recv
	while (PX_UDPReceived(&plb->udp,&faddr,packet_data,sizeof(packet_data),&readsize))
	{
		DEMO_ServerLobbyOnRecvHandle(plb,faddr,packet_data,readsize);
	}

}

px_void GameLoginServer_LobbyFree(GameLoginServer_Lobby *plb)
{
	if (plb->mp.StartAddr!=PX_NULL)
	{
		free(plb->mp.StartAddr);
	}
}

px_bool GameLoginServer_LobbyLogin(GameLoginServer_Lobby *plb,px_byte aes_code[32],px_char account[32],px_char pwd[32],_OUT px_byte cookie[32],_OUT px_dword *map_index,_OUT px_dword *id)
{
	GameLoginServer_Lobby_Client client,*pClient;
	PX_SHA256_HASH shahash;
	px_int i,j;
	time_t t;
	struct tm * lt;
	px_char text[128];
	client.UserGame_data=PX_NULL;
	client.profilePhoto_data=PX_NULL;
	PX_Sha256Calculate(pwd,sizeof(pClient->pwd),&shahash);

	for (i=0;i<plb->clients.size;i++)
	{
		pClient=PX_VECTORAT(GameLoginServer_Lobby_Client,&plb->clients,i);
		if (pClient->activated)
		{
			if (PX_strequ(pClient->userInfo.account,account))
			{

				if (PX_memequ(shahash.bytes,pClient->pwd,sizeof(pClient->pwd)))
				{
					PX_memcpy(pClient->connect_aes_code,aes_code,sizeof(pClient->connect_aes_code));
					PX_AES_Initialize(&pClient->aes_encoder,PX_AES_KeySize_Bits256,aes_code);

					//regenerate id
					pClient->id=(plb->alloc_id+=(PX_rand()%0x1fff));
					PX_memcpy(cookie,pClient->cookie,sizeof(pClient->cookie));

					//return
					*id=pClient->id;
					*map_index=i;
					return PX_TRUE;
				}
				return PX_FALSE;
			}
		}
	}

	if (!GameServer_Database_AccountIsRegisted(plb->linker_Database,account))
	{
		return PX_FALSE;
	}

	for (i=0;i<plb->clients.size;i++)
	{
		pClient=PX_VECTORAT(GameLoginServer_Lobby_Client,&plb->clients,i);
		if (!pClient->activated)
		{
			px_int msize;
			if (!GameServer_Database_AccountVerify(plb->linker_Database,account,pwd))
			{
				goto _ERROR;
			}
			if (!GameServer_Database_ReadUserInfo(plb->linker_Database,account,&client.userInfo))
			{
				goto _ERROR;
			}

			msize=GameServer_Database_GetUserGameDataSize(plb->linker_Database,account);
			if (!msize)
			{
				GameLoginServer_ConsolePrint(GAMELOGINSERVER_TEXT_USER_GAMEDATA_MISSING);
				GameLoginServer_ConsolePrint(account);
				goto _ERROR;
			}
			client.UserGame_data=(px_byte *)malloc(msize);
			if (!client.UserGame_data)
			{
				goto _ERROR;
			}
			client.UserGame_datasize=msize;
			if (!GameServer_Database_ReadUserGameData(plb->linker_Database,account,client.UserGame_data))
			{
				goto _ERROR;
			}

			msize=GameServer_Database_GetProfilePhotoDataSize(plb->linker_Database,account);
			if (!msize)
			{
				GameLoginServer_ConsolePrint(GAMELOGINSERVER_TEXT_USER_PROFILE_PHOTO_MISSING);
				GameLoginServer_ConsolePrint(account);
				goto _ERROR;
			}
			client.profilePhoto_data=(px_byte *)malloc(msize);
			if (!client.profilePhoto_data)
			{
				goto _ERROR;
			}
			client.profilePhoto_datasize=msize;
			if (!GameServer_Database_ReadProfilePhotoData(plb->linker_Database,account,client.profilePhoto_data))
			{
				goto _ERROR;
			}
			PX_Sha256CalculateHashString(client.profilePhoto_data,msize,(px_char *)client.profilePhoto_data_sha256);


			PX_memcpy(client.connect_aes_code,aes_code,sizeof(client.connect_aes_code));
			PX_AES_Initialize(&client.aes_encoder,PX_AES_KeySize_Bits256,client.connect_aes_code);
			client.last_op_time=0;
			client.last_hb_elpased=0;
			client.last_chat_time=0;
			client.last_chat_packetid=0;
			client.online_time=0;
			client.activated=PX_TRUE;
			client.id=(plb->alloc_id+=(PX_rand()%0x1fff));
		
			for(j=0;j<sizeof(client.cookie)-1;j++)
			{
				client.cookie[j]=PX_rand()%0xff;
				if (client.cookie[j]==0)
				{
					client.cookie[j]=1;
				}
			}
			client.cookie[j]=0;
			PX_memcpy(cookie,client.cookie,sizeof(client.cookie));

			PX_memcpy(client.pwd,shahash.bytes,sizeof(client.pwd));

			//calculate sha256 for profile photo
			PX_Sha256CalculateHashString(client.profilePhoto_data,client.profilePhoto_datasize,(px_char *)client.profilePhoto_data_sha256);

			*pClient=client;
			*id=pClient->id;
			*map_index=i;
			break;
		}
	}
	if (i==plb->clients.size)
	{
		goto _ERROR;
	}


	time (&t);
	lt = localtime(&t);
	sprintf_s(text,sizeof(text),GAMELOGINSERVER_TEXT_USER_LOGIN,account,lt->tm_year+1900, lt->tm_mon, lt->tm_mday, lt->tm_hour, lt->tm_min, lt->tm_sec);
	GameLoginServer_ConsolePrint(text);

	return PX_TRUE;
_ERROR:
	if (client.UserGame_data)
	{
		free(client.UserGame_data);
	}
	if (client.profilePhoto_data)
	{
		free(client.profilePhoto_data);
	}
	client.UserGame_data=PX_NULL;
	client.profilePhoto_data=PX_NULL;
	return PX_FALSE;
}

px_void GameLoginServer_LobbyClientRelease(GameLoginServer_Lobby *plb,px_int index)
{
	GameLoginServer_Lobby_Client *pClient;
	if (index>=plb->clients.size)
	{
		return;
	}
	pClient=PX_VECTORAT(GameLoginServer_Lobby_Client,&plb->clients,index);
	if (!pClient->activated)
	{
		return;
	}
	if(pClient->UserGame_data)
		free(pClient->UserGame_data);
	if(pClient->profilePhoto_data)
		free(pClient->profilePhoto_data);
	pClient->UserGame_data=PX_NULL;
	pClient->profilePhoto_data=PX_NULL;
	pClient->UserGame_datasize=0;
	pClient->profilePhoto_data=0;
	pClient->activated=PX_FALSE;
}

