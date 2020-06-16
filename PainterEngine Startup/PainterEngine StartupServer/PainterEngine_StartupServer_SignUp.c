#include "PainterEngine_StartupServer_SignUp.h"

extern px_void PX_Application_ConsolePrint(const px_char *Text);

px_bool PX_StartupServer_SignUpInitialize(PX_StartupServer_SignUp *pSp,PX_StartupServer_Database *link_database,px_int max_connections)
{
	PX_memset(pSp,0,sizeof(PX_StartupServer_SignUp));

	pSp->mp=MP_Create(malloc(1024*1024),1024*1024);
	pSp->connections=(PX_StartupServer_SignUpConnection *)malloc(sizeof(PX_StartupServer_SignUpConnection)*max_connections);
	PX_memset(pSp->connections,0,sizeof(PX_StartupServer_SignUpConnection)*max_connections);
	if(!pSp)return PX_FALSE;
	pSp->max_connections=max_connections;
	
	pSp->link_database=link_database;
	if (!PX_UDPInit(&pSp->udp,PX_UDP_IP_TYPE_IPV4))
	{
		PX_Application_ConsolePrint(PX_STARTUPSERVER_SIGNUP_TEXT_INITIALIZE_SIGNUP_UDP_FAILED);
		goto _ERROR;
	}
	
	if (!PX_UDPListen(&pSp->udp,PX_STARTUPSERVER_SIGNUP_LISTEN_PORT))
	{
		PX_Application_ConsolePrint(PX_STARTUPSERVER_SIGNUP_TEXT_LISTEN_SIGNUP_UDP_FAILED);
		PX_UDPFree(&pSp->udp);
		goto _ERROR;
	}
	if(!PX_VectorInit(&pSp->mp,&pSp->regcode,sizeof(PX_StartupServer_SignUp_RegCode),16));
	return PX_TRUE;
_ERROR:
	if (pSp->connections)
	{
		free(pSp->connections);
		pSp->connections=PX_NULL;
	}
	return PX_FALSE;
}

static const px_byte PX_StartupServer_SignUp_curve25519_basepoint[32] = {9};

static px_void PX_StartupServer_SignUpAddRegIP(PX_StartupServer_SignUp *pDesc,px_dword ip)
{
	px_int i;
	for (i=0;i<PX_COUNTOF(pDesc->regIp);i++)
	{
		if (!pDesc->regIp[i].activated)
		{
			pDesc->regIp[i].activated=PX_TRUE;
			pDesc->regIp[i].ipv4=ip;
			pDesc->regIp[i].elpased=0;
			return;
		}
	}
}

static px_void PX_StartupServer_SignUpregIPUpdate(PX_StartupServer_SignUp *pDesc,px_dword elpased)
{
	px_int i;
	for (i=0;i<PX_COUNTOF(pDesc->regIp);i++)
	{
		if (pDesc->regIp[i].activated)
		{
			pDesc->regIp[i].elpased+=elpased;
			if (pDesc->regIp[i].elpased>PX_STARTUP_SIGNUP_IP_TIMEOUT)
			{
				pDesc->regIp[i].activated=0;
				pDesc->regIp[i].ipv4=0;
				pDesc->regIp[i].elpased=0;
			}
		}
	}
}

static px_bool PX_StartupServer_SignUpIsRegIPExisted(PX_StartupServer_SignUp *pDesc,px_dword ip)
{
	px_int i;
	for (i=0;i<PX_COUNTOF(pDesc->regIp);i++)
	{
		if (pDesc->regIp[i].activated)
		{
			if (pDesc->regIp[i].ipv4==ip)
			{
				return PX_TRUE;
			}
		}
	}
	return PX_FALSE;
}

static px_void PX_StartupServer_SignUpSendAck(PX_StartupServer_SignUp *pLogin,px_int i)
{
	PX_SignUp_Packet_ConnectAck ack;
	ack.magic_numeric=PX_NETWORKSIGNUP_MAGIC;
	ack.opcode=PX_NETWORKSIGNUP_OPCODE_ACK;
	PX_memcpy(ack.publickey,pLogin->connections[i].publickey,sizeof(pLogin->connections[i].publickey));
	PX_UDPSend(&pLogin->udp,pLogin->connections[i].Addr,(px_byte *)&ack,sizeof(ack));
}

px_void PX_StartupServer_SignUpUpdate(PX_StartupServer_SignUp *pR,px_dword elpased)
{
	px_int readsize,i,j;
	px_byte data[PX_NETWORK_PACKET_MAX_SIZE];
	PX_UDP_ADDR Addr;
	PX_SignUp_Packet_Header *pHeader=(PX_SignUp_Packet_Header *)data;
	PX_SignUp_Packet_Connect *pConn=(PX_SignUp_Packet_Connect *)data;
	PX_Signup_Packet_Sign *pSignPack=(PX_Signup_Packet_Sign *)data;

	PX_memset(&Addr,0,sizeof(Addr));

	for (i=0;i<pR->max_connections;i++)
	{
		if (pR->connections[i].activate)
		{
			pR->connections[i].elpased+=elpased;
			if (pR->connections[i].elpased>PX_STARTUPSERVER_SIGNUP_CONNECTION_TIMEOUT)
			{
				pR->connections[i].elpased=0;
				pR->connections[i].activate=PX_FALSE;
			}
		}
	}

	PX_StartupServer_SignUpregIPUpdate(pR,elpased);

	while (PX_UDPReceived(&pR->udp,&Addr,data,sizeof(data),&readsize))
	{
		if (pHeader->magic_numeric!=PX_NETWORKSIGNUP_MAGIC)
		{
			continue;
		}
		switch(pHeader->opcode)
		{
		case PX_NETWORKSIGNUP_OPCODE_CONNECT:
			if (readsize!=sizeof(PX_SignUp_Packet_Connect))
			{
				break;
			}
			for (i=0;i<pR->max_connections;i++)
			{
				if(pR->connections[i].activate)
				{
					if (PX_memequ(&pR->connections[i].Addr,&Addr,sizeof(Addr)))
					{
						pR->connections[i].elpased=0;
						if (PX_memequ(pR->connections[i].theirkey,pConn->publickey,sizeof(pConn->publickey)))
						{
							PX_StartupServer_SignUpSendAck(pR,i);

						}
						else
						{
							PX_memcpy(pR->connections[i].theirkey,pConn->publickey,sizeof(pConn->publickey));
							for(j=0;j<sizeof(pR->connections[i].privatekey);j++)
							{
								pR->connections[i].privatekey[j]=PX_rand()%0x100;
							}
							pR->connections[i].privatekey[0] &= 248;
							pR->connections[i].privatekey[31] &= 127;
							pR->connections[i].privatekey[31] |= 64;

							PX_Curve25519(pR->connections[i].publickey,pR->connections[i].privatekey,PX_StartupServer_SignUp_curve25519_basepoint);
							PX_Curve25519(pR->connections[i].sharekey,pR->connections[i].privatekey,pR->connections[i].theirkey);
							PX_StartupServer_SignUpSendAck(pR,i);
						}
						break;
					}
					
				}
			}
			if (i==pR->max_connections)
			{
				
				//new connection
				for (i=0;i<pR->max_connections;i++)
				{
					if(!pR->connections[i].activate)
					{
						pR->connections[i].Addr=Addr;
						pR->connections[i].activate=PX_TRUE;
						pR->connections[i].elpased=0;

						PX_memcpy(pR->connections[i].theirkey,pConn->publickey,sizeof(pConn->publickey));
						for(j=0;j<sizeof(pR->connections[i].privatekey);j++)
						{
							pR->connections[i].privatekey[j]=PX_rand()%0x100;
						}
						PX_Curve25519(pR->connections[i].publickey,pR->connections[i].privatekey,PX_StartupServer_SignUp_curve25519_basepoint);
						PX_Curve25519(pR->connections[i].sharekey,pR->connections[i].privatekey,pR->connections[i].theirkey);
						PX_StartupServer_SignUpSendAck(pR,i);
						break;
					}
				}
			}
			break;
		case PX_NETWORKSIGNUP_OPCODE_SIGN:
			{
				
				if (readsize!=sizeof(PX_Signup_Packet_Sign))
				{
					break;
				}
				for (i=0;i<pR->max_connections;i++)
				{
					if(pR->connections[i].activate)
					{
						if (PX_memequ(&pR->connections[i].Addr,&Addr,sizeof(Addr)))
						{
							PX_AES aes;
							px_int j;
							px_char pwd[32],user[32],regcode[32];
							px_word nickname[32];
							PX_StartupServer_SignUp_RegCode *preg;
							PX_AES_Initialize(&aes,PX_AES_KeySize_Bits256,pR->connections[i].sharekey);
							PX_AES_InvCipher(&aes,pSignPack->userName,(px_byte *)user);
							PX_AES_InvCipher(&aes,pSignPack->userName+PX_AES_BLOCK_BYTES_SIZE,(px_byte *)user+PX_AES_BLOCK_BYTES_SIZE);
							PX_AES_InvCipher(&aes,pSignPack->password,(px_byte *)pwd);
							PX_AES_InvCipher(&aes,pSignPack->password+PX_AES_BLOCK_BYTES_SIZE,(px_byte *)pwd+PX_AES_BLOCK_BYTES_SIZE);
							PX_AES_InvCipherBuffer(&aes,(px_byte *)pSignPack->nickName,sizeof(pSignPack->nickName),(px_byte *)nickname);
							PX_AES_InvCipher(&aes,pSignPack->regcode,(px_byte *)regcode);
							PX_AES_InvCipher(&aes,pSignPack->regcode+PX_AES_BLOCK_BYTES_SIZE,(px_byte *)regcode+PX_AES_BLOCK_BYTES_SIZE);
							//disable this connection
							pR->connections[i].activate=PX_FALSE;

							for (j=0;j<pR->regcode.size;j++)
							{
								preg=PX_VECTORAT(PX_StartupServer_SignUp_RegCode,&pR->regcode,j);
								if (!preg->active_count)
								{
									PX_VectorErase(&pR->regcode,j);
									j--;
									continue;
								}
								if (PX_strequ(regcode,preg->regCode))
								{
									break;
								}
							}
							if (j>=pR->regcode.size)
							{
								//error;
								PX_SignUp_Packet_SignAck ack;
								ack.magic_numeric=PX_NETWORKSIGNUP_MAGIC;
								ack.opcode=PX_NETWORKSIGNUP_OPCODE_SIGNACK;
								ack.result=PX_NETWORKSIGNUP_SIGNACK_INVALID_REGCODE;
								PX_UDPSend(&pR->udp,pR->connections[i].Addr,(px_byte *)&ack,sizeof(ack));
								break;
							}

							if (PX_strlen(pwd)<PX_STARTUP_PASSWORD_MINLEN||PX_strlen(pwd)>PX_STARTUP_PASSWORD_MAXLEN)
							{
								PX_SignUp_Packet_SignAck ack;
								ack.magic_numeric=PX_NETWORKSIGNUP_MAGIC;
								ack.opcode=PX_NETWORKSIGNUP_OPCODE_SIGNACK;
								ack.result=PX_NETWORKSIGNUP_SIGNACK_INVALID_PASSWORD;
								PX_UDPSend(&pR->udp,pR->connections[i].Addr,(px_byte *)&ack,sizeof(ack));
								break;
							}
							if (PX_wstrlen(nickname)<PX_STARTUP_NICKNAME_MINLEN||PX_wstrlen(nickname)>PX_STARTUP_NICKNAME_MAXLEN)
							{
								PX_SignUp_Packet_SignAck ack;
								ack.magic_numeric=PX_NETWORKSIGNUP_MAGIC;
								ack.opcode=PX_NETWORKSIGNUP_OPCODE_SIGNACK;
								ack.result=PX_NETWORKSIGNUP_SIGNACK_INVALID_NICKNAME;
								PX_UDPSend(&pR->udp,pR->connections[i].Addr,(px_byte *)&ack,sizeof(ack));
								break;
							}
							if (PX_strlen(user)<PX_STARTUP_ACCOUNT_MINLEN||PX_strlen(user)>PX_STARTUP_ACCOUNT_MAXLEN)
							{
								PX_SignUp_Packet_SignAck ack;
								ack.magic_numeric=PX_NETWORKSIGNUP_MAGIC;
								ack.opcode=PX_NETWORKSIGNUP_OPCODE_SIGNACK;
								ack.result=PX_NETWORKSIGNUP_SIGNACK_INVALID_ACCOUNT;
								PX_UDPSend(&pR->udp,pR->connections[i].Addr,(px_byte *)&ack,sizeof(ack));
								break;
							}

							if (preg->active_count)
							{
								if(PX_StartupServer_Database_AccountIsRegisted(pR->link_database,user))
								{
									PX_SignUp_Packet_SignAck ack;
									ack.magic_numeric=PX_NETWORKSIGNUP_MAGIC;
									ack.opcode=PX_NETWORKSIGNUP_OPCODE_SIGNACK;
									ack.result=PX_NETWORKSIGNUP_SIGNACK_REPEAT_ACCOUNT;
									PX_UDPSend(&pR->udp,pR->connections[i].Addr,(px_byte *)&ack,sizeof(ack));
									break;
								}
								else
								{
									if (preg->authority_level>PX_STARTUP_SIGNUP_IP_LEVEL&&PX_StartupServer_SignUpIsRegIPExisted(pR,Addr.ipv4))
									{
										PX_SignUp_Packet_SignAck ack;
										ack.magic_numeric=PX_NETWORKSIGNUP_MAGIC;
										ack.opcode=PX_NETWORKSIGNUP_OPCODE_SIGNACK;
										ack.result=PX_NETWORKSIGNUP_SIGNACK_REPEAT_IP;
										PX_UDPSend(&pR->udp,pR->connections[i].Addr,(px_byte *)&ack,sizeof(ack));
										break;
									}

									if (PX_StartupServer_Database_RegistryAccount(pR->link_database,user,nickname,pwd,regcode,preg->authority_level,preg->sign_admin))
									{
										PX_SignUp_Packet_SignAck ack;
										ack.magic_numeric=PX_NETWORKSIGNUP_MAGIC;
										ack.opcode=PX_NETWORKSIGNUP_OPCODE_SIGNACK;
										ack.result=PX_NETWORKSIGNUP_SIGNACK_SUCCEED;
										PX_UDPSend(&pR->udp,pR->connections[i].Addr,(px_byte *)&ack,sizeof(ack));
										preg->active_count--;
										if (!preg->active_count)
										{
											PX_VectorErase(&pR->regcode,j);
										}
										do 
										{
											px_char message[64];
											PX_sprintf1(message,sizeof(message),"User %1 registered ",PX_STRINGFORMAT_STRING(user));
											PX_Application_ConsolePrint(message);
										} while (0);

										if (preg->authority_level>PX_STARTUP_SIGNUP_IP_LEVEL)
											PX_StartupServer_SignUpAddRegIP(pR,Addr.ipv4);

										break;
									}
									else
									{
										PX_SignUp_Packet_SignAck ack;
										ack.magic_numeric=PX_NETWORKSIGNUP_MAGIC;
										ack.opcode=PX_NETWORKSIGNUP_OPCODE_SIGNACK;
										ack.result=PX_NETWORKSIGNUP_SIGNACK_INVALID_ACCOUNT;
										PX_UDPSend(&pR->udp,pR->connections[i].Addr,(px_byte *)&ack,sizeof(ack));
										break;
									}
								}
								
								
							}
							else
							{				
								//error;
								PX_SignUp_Packet_SignAck ack;
								ack.magic_numeric=PX_NETWORKSIGNUP_MAGIC;
								ack.opcode=PX_NETWORKSIGNUP_OPCODE_SIGNACK;
								ack.result=PX_NETWORKSIGNUP_SIGNACK_INVALID_REGCODE;
								PX_UDPSend(&pR->udp,pR->connections[i].Addr,(px_byte *)&ack,sizeof(ack));
								break;
							}
						}
					}
				}
			}
			break;
		case PX_NETWORKSIGNUP_OPCODE_DATA:
			break;
		default:
			break;
		}
	}
}

px_void PX_StartupServer_SignUpFree(PX_StartupServer_SignUp *pSp)
{
	if (pSp->connections)
	{
		free(pSp->connections);
		pSp->connections=PX_NULL;
	}
	
}

px_bool PX_StartupServer_SignAddRegCode(PX_StartupServer_SignUp *pSp,px_char regCode[32],px_char signAdmin[16],px_dword activeCount,px_dword authority_level)
{
	PX_StartupServer_SignUp_RegCode reg;
	if (activeCount==0)
	{
		return PX_TRUE;
	}
	PX_memset(&reg,0,sizeof(reg));
	PX_strcat(reg.regCode,regCode);
	PX_strcat(reg.sign_admin,signAdmin);
	reg.active_count=activeCount;
	reg.authority_level=authority_level;
	return PX_VectorPushback(&pSp->regcode,&reg);
}


