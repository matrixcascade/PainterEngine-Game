#include "PainterEngine_StartupServer_Login.h"

extern px_void PX_Application_ConsolePrint(const px_char *Text);

px_bool PX_StartupServer_LoginInitialize(PX_StartupServer_Login *pLogin,px_int max_connections)
{
	pLogin->connections=(PX_StartupServer_LoginConnection *)malloc(sizeof(PX_StartupServer_LoginConnection)*max_connections);
	PX_memset(pLogin->connections,0,sizeof(PX_StartupServer_LoginConnection)*max_connections);
	if(!pLogin)return PX_FALSE;
	pLogin->max_connections=max_connections;
	if (!PX_UDPInit(&pLogin->udp,PX_UDP_IP_TYPE_IPV4))
	{
		PX_Application_ConsolePrint(PX_STARTUPSERVER_LOGIN_TEXT_INITIALIZE_LOGIN_UDP_FAILED);
		goto _ERROR;
	}

	if (!PX_UDPListen(&pLogin->udp,PX_STARTUPSERVER_LOGIN_LISTEN_PORT))
	{
		PX_Application_ConsolePrint(PX_STARTUPSERVER_LOGIN_TEXT_LISTEN_LOGIN_UDP_FAILED);
		PX_UDPFree(&pLogin->udp);
		goto _ERROR;
	}
	return PX_TRUE;
_ERROR:
	if (pLogin->connections)
	{
		free(pLogin->connections);
		pLogin->connections=PX_NULL;
	}
	return PX_FALSE;
}

static const px_byte PX_StartupServer_Login_curve25519_basepoint[32] = {9};

static px_void PX_StartupServer_LoginSendAck(PX_StartupServer_Login *pLogin,px_int i)
{
	PX_Login_Packet_ConnectAck ack;
	ack.magic_numeric=PX_NETWORKLOGIN_MAGIC;
	ack.opcode=PX_NETWORKLOGIN_OPCODE_ACK;
	ack.v_code=pLogin->connections[i].v_code;
	PX_memcpy(ack.publickey,pLogin->connections[i].publickey,sizeof(pLogin->connections[i].publickey));
	PX_UDPSend(&pLogin->udp,pLogin->connections[i].Addr,(px_byte *)&ack,sizeof(ack));
}

extern px_bool PX_Application_LobbyLogin(px_byte aes_code[32],px_char account[32],px_char pwd[32],_OUT px_byte cookie[32],_OUT px_dword *map_index,_OUT px_dword *id);
px_void PX_StartupServer_LoginUpdate(PX_StartupServer_Login *pLogin,px_dword elpased)
{
	px_int readsize,i,j;
	px_byte data[PX_NETWORK_PACKET_MAX_SIZE];
	PX_UDP_ADDR addr;
	PX_Login_Packet_Header *pHeader=(PX_Login_Packet_Header *)data;
	PX_Login_Packet_Connect *pConn=(PX_Login_Packet_Connect *)data;
	PX_Login_Packet_login *pLoginPack=(PX_Login_Packet_login *)data;
	PX_memset(&addr,0,sizeof(addr));

	for (i=0;i<pLogin->max_connections;i++)
	{
		if (pLogin->connections[i].activate)
		{
			pLogin->connections[i].elpased+=elpased;
			if (pLogin->connections[i].elpased>PX_STARTUPSERVER_LOGIN_CONNECTION_TIMEOUT)
			{
				pLogin->connections[i].activate=PX_FALSE;
			}
		}
	}

	while (PX_UDPReceived(&pLogin->udp,&addr,data,sizeof(data),&readsize))
	{
		if (pHeader->magic_numeric!=PX_NETWORKLOGIN_MAGIC)
		{
			return;
		}
		switch(pHeader->opcode)
		{
		case PX_NETWORKLOGIN_OPCODE_CONNECT:
			if (readsize!=sizeof(PX_Login_Packet_Connect))
			{
				break;
			}
			for (i=0;i<pLogin->max_connections;i++)
			{
				if(pLogin->connections[i].activate)
				{
					if (PX_memequ(&pLogin->connections[i].Addr,&addr,sizeof(addr)))
					{
						pLogin->connections[i].elpased=0;
						if (PX_memequ(pLogin->connections[i].theirkey,pConn->publickey,sizeof(pConn->publickey)))
						{
							PX_StartupServer_LoginSendAck(pLogin,i);

						}
						else
						{
							PX_memcpy(pLogin->connections[i].theirkey,pConn->publickey,sizeof(pConn->publickey));
							for(j=0;j<sizeof(pLogin->connections[i].privatekey);j++)
							{
								pLogin->connections[i].privatekey[j]=PX_rand()%0x100;
							}
							pLogin->connections[i].privatekey[0] &= 248;
							pLogin->connections[i].privatekey[31] &= 127;
							pLogin->connections[i].privatekey[31] |= 64;

							PX_Curve25519(pLogin->connections[i].publickey,pLogin->connections[i].privatekey,PX_StartupServer_Login_curve25519_basepoint);
							PX_Curve25519(pLogin->connections[i].sharekey,pLogin->connections[i].privatekey,pLogin->connections[i].theirkey);
							PX_StartupServer_LoginSendAck(pLogin,i);
						}
						break;
					}
					
				}
			}
			if (i==pLogin->max_connections)
			{
				
				//new connection
				for (i=0;i<pLogin->max_connections;i++)
				{
					if(!pLogin->connections[i].activate)
					{
						pLogin->connections[i].Addr=addr;
						pLogin->connections[i].activate=PX_TRUE;
						pLogin->connections[i].v_code=PX_rand();
						pLogin->connections[i].elpased=0;

						PX_memcpy(pLogin->connections[i].theirkey,pConn->publickey,sizeof(pConn->publickey));
						for(j=0;j<sizeof(pLogin->connections[i].privatekey);j++)
						{
							pLogin->connections[i].privatekey[j]=PX_rand()%0x100;
						}
						PX_Curve25519(pLogin->connections[i].publickey,pLogin->connections[i].privatekey,PX_StartupServer_Login_curve25519_basepoint);
						PX_Curve25519(pLogin->connections[i].sharekey,pLogin->connections[i].privatekey,pLogin->connections[i].theirkey);
						PX_StartupServer_LoginSendAck(pLogin,i);
						break;
					}
				}
			}
			break;
		case PX_NETWORKLOGIN_OPCODE_LOGIN:
			{
				
				if (readsize!=sizeof(PX_Login_Packet_login))
				{
					return;
				}
				for (i=0;i<pLogin->max_connections;i++)
				{
					if(pLogin->connections[i].activate)
					{
						if (PX_memequ(&pLogin->connections[i].Addr,&addr,sizeof(addr)))
						{
							PX_AES aes;
							PX_Login_Packet_loginAck loginAck;
							px_byte cookie[32];
							px_char pwd[32],user[32];
							if (pLoginPack->v_code!=pLogin->connections[i].v_code)
							{
								break;
							}

							if (pLoginPack->version!=GAME_VERSION)
							{
								loginAck.magic_numeric=PX_NETWORKLOGIN_MAGIC;
								loginAck.opcode=PX_NETWORKLOGIN_OPCODE_LOGINACK;

								loginAck.Message[0]=0;
								PX_wstrcat(loginAck.Message,(px_word *)PX_STARTUPSERVER_LOGIN_INVALID_VERSION);
								loginAck.result=PX_NETWORKLOGIN_OPCODE_LOGINRESULT_FAILED;

								PX_UDPSend(&pLogin->udp,pLogin->connections[i].Addr,(px_byte *)&loginAck,sizeof(loginAck));
							}
							else
							{
								PX_AES_Initialize(&aes,PX_AES_KeySize_Bits256,pLogin->connections[i].sharekey);
								PX_AES_InvCipher(&aes,pLoginPack->userName,(px_byte *)user);
								PX_AES_InvCipher(&aes,pLoginPack->userName+PX_AES_BLOCK_BYTES_SIZE,(px_byte *)user+PX_AES_BLOCK_BYTES_SIZE);
								PX_AES_InvCipher(&aes,pLoginPack->password,(px_byte *)pwd);
								PX_AES_InvCipher(&aes,pLoginPack->password+PX_AES_BLOCK_BYTES_SIZE,(px_byte *)pwd+PX_AES_BLOCK_BYTES_SIZE);

								loginAck.magic_numeric=PX_NETWORKLOGIN_MAGIC;
								loginAck.opcode=PX_NETWORKLOGIN_OPCODE_LOGINACK;

								if (PX_Application_LobbyLogin(pLogin->connections[i].sharekey,user,pwd,cookie,&loginAck.map_index,&loginAck.id))
								{
									loginAck.result=PX_NETWORKLOGIN_OPCODE_LOGINRESULT_SUCCEEDED;
									PX_AES_Cipher(&aes,cookie,loginAck.cookie);
									PX_AES_Cipher(&aes,cookie+PX_AES_BLOCK_BYTES_SIZE,loginAck.cookie+PX_AES_BLOCK_BYTES_SIZE);
								}
								else
								{
									PX_memset(loginAck.Message,0,sizeof(loginAck.Message));
									PX_wstrcat(loginAck.Message,(px_word *)PX_STARTUPSERVER_LOGIN_INVALID_LOGIN_DATA);
									loginAck.result=PX_NETWORKLOGIN_OPCODE_LOGINRESULT_FAILED;
								}

								PX_UDPSend(&pLogin->udp,pLogin->connections[i].Addr,(px_byte *)&loginAck,sizeof(loginAck));
							}
						}
					}
				}
			}
			break;
		case PX_NETWORKLOGIN_OPCODE_DATA:
			break;
		default:
			return;
		}
	}
}
