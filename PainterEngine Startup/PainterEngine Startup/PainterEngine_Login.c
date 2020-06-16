#include "PainterEngine_Login.h"

static const px_byte GameStartup_ClientLogin_curve25519_basepoint[32] = {9};//curve25519 base point

static const px_char PX_Login_EditAccountLimit[]="1234567890zxcvbnmasdfghjklqwertyuiopQWERTYUIOPLKJHGFDSAZXCVBNM_.";
static const px_char PX_Login_EditPasswordLimit[]="1234567890zxcvbnmasdfghjklqwertyuiopQWERTYUIOPLKJHGFDSAZXCVBNM_~!@#$%^*().";

px_void GameStartup_LoginButtonOnSignUp(PX_Object * pObject,PX_Object_Event e,px_void *user_ptr)
{
	extern px_void PX_ApplicationSwitchSignup();
	PX_ApplicationSwitchSignup();
}


px_void GameStartup_LoginButtonOnExit(PX_Object * pObject,PX_Object_Event e,px_void *user_ptr)
{
	exit(0);
}

px_void GameStartup_LoginButtonOnLogin(PX_Object * pObject,PX_Object_Event e,px_void *user_ptr)
{
	px_int i;
	PX_Login *pl=(PX_Login *)user_ptr;

	if(PX_Object_EditGetText(pl->edit_user)[0]=='\0')
	{
		PX_ApplicationMessageBoxAlertOk(PX_LOGIN_TEXT_INVALID_ACCOUNT);
		return;
	}

	if(PX_Object_EditGetText(pl->edit_pwd)[0]=='\0')
	{
		PX_ApplicationMessageBoxAlertOk(PX_LOGIN_TEXT_INVALID_PASSWORD);
		return;
	}

	pl->status=PX_LOGIN_STATUS_LOGINING_EXCHAGE;
	
	PX_ApplicationMessageBoxDisplayContent(PX_LOGIN_TEXT_LOGINING);
	//generate private key
	for(i=0;i<sizeof(pl->privatekey);i++)
	{
		pl->privatekey[i]=PX_rand()%0x100;
	}
	pl->privatekey[0] &= 248;
	pl->privatekey[31] &= 127;
	pl->privatekey[31] |= 64;
	PX_Curve25519(pl->publickey,pl->privatekey,GameStartup_ClientLogin_curve25519_basepoint);
}

px_bool PX_LoginInitialize(PX_Instance *pIns,PX_Login *pl,const px_char LoginServerIpAddr[],px_ushort port)
{
	PX_memset(pl,0,sizeof(PX_Login));

	pl->pIns=pIns;
	PX_memset(&pl->serverAddr,0,sizeof(pl->serverAddr));
	PX_memset(pl->userName,0,sizeof(pl->userName));
	PX_memset(pl->password,0,sizeof(pl->password));
	pl->login_root=PX_NULL;
	pl->edit_pwd=PX_NULL;
	pl->edit_user=PX_NULL;
	pl->btn_exit=PX_NULL;
	
	pl->label_loginstatus=PX_NULL;
	pl->logining_root=PX_ObjectCreate(&pl->pIns->runtime.mp_ui,0,0,0,0,0,0,0);

	pl->login_root=PX_ObjectCreate(&pl->pIns->runtime.mp_ui,PX_NULL,0,0,0,0,0,0);
	if (pl->login_root)
	{
		if(!(pl->edit_user=PX_Object_EditCreate(&pl->pIns->runtime.mp_ui,pl->login_root,300,235,238,22,PX_COLOR(255,0,0,0)))) goto _ERROR;
		PX_Object_EditSetBorder(pl->edit_user,PX_FALSE);
		PX_Object_EditSetMaxTextLength(pl->edit_user,16);
		PX_Object_EditSetLimit(pl->edit_user,PX_Login_EditAccountLimit);

		if(!(pl->edit_pwd=PX_Object_EditCreate(&pl->pIns->runtime.mp_ui,pl->login_root,300,298,238,22,PX_COLOR(255,0,0,0)))) goto _ERROR;
		PX_Object_EditSetBorder(pl->edit_pwd,PX_FALSE);
		PX_Object_EditSetPasswordStyle(pl->edit_pwd,PX_TRUE);
		PX_Object_EditSetMaxTextLength(pl->edit_pwd,16);
		PX_Object_EditSetLimit(pl->edit_pwd,PX_Login_EditPasswordLimit);

		if(!(pl->btn_login=PX_Object_PushButtonCreate(&pl->pIns->runtime.mp_ui,pl->login_root,255,370,128,32,PX_LOGIN_TEXT_LOGIN,PX_COLOR(255,0,0,0)))) goto _ERROR;
		PX_Object_PushButtonSetBackgroundColor(pl->btn_login,PX_COLOR(255,255,255,255));
		PX_Object_PushButtonSetBorder(pl->btn_login,PX_TRUE);
		PX_Object_PushButtonSetBorderColor(pl->btn_login,PX_COLOR(255,0,0,0));
		PX_Object_PushButtonSetCursorColor(pl->btn_login,PX_COLOR(255,192,192,192));
		PX_Object_PushButtonSetPushColor(pl->btn_login,PX_COLOR(255,128,128,128));
		if(!(pl->btn_exit=PX_Object_PushButtonCreate(&pl->pIns->runtime.mp_ui,pl->login_root,425,370,128,32,PX_LOGIN_TEXT_EXIT,PX_COLOR(255,0,0,0)))) goto _ERROR;
		PX_Object_PushButtonSetBackgroundColor(pl->btn_exit,PX_COLOR(255,255,255,255));
		PX_Object_PushButtonSetBorder(pl->btn_exit,PX_TRUE);
		PX_Object_PushButtonSetBorderColor(pl->btn_exit,PX_COLOR(255,0,0,0));
		PX_Object_PushButtonSetCursorColor(pl->btn_exit,PX_COLOR(255,192,192,192));
		PX_Object_PushButtonSetPushColor(pl->btn_exit,PX_COLOR(255,128,128,128));
		if(!(pl->btn_signup=PX_Object_PushButtonCreate(&pl->pIns->runtime.mp_ui,pl->login_root,718,2,80,24,PX_LOGIN_TEXT_SIGNUP,PX_COLOR(255,25,25,255)))) goto _ERROR;
		PX_Object_PushButtonSetBackgroundColor(pl->btn_signup,PX_COLOR(255,255,255,255));
		PX_Object_PushButtonSetBorder(pl->btn_signup,PX_TRUE);
		PX_Object_PushButtonSetBorderColor(pl->btn_signup,PX_COLOR(255,25,25,255));
		PX_Object_PushButtonSetCursorColor(pl->btn_signup,PX_COLOR(255,192,192,192));
		PX_Object_PushButtonSetPushColor(pl->btn_signup,PX_COLOR(255,128,128,128));

		PX_ObjectRegisterEvent(pl->btn_exit,PX_OBJECT_EVENT_EXECUTE,GameStartup_LoginButtonOnExit,PX_NULL);
		PX_ObjectRegisterEvent(pl->btn_login,PX_OBJECT_EVENT_EXECUTE,GameStartup_LoginButtonOnLogin,pl);
		PX_ObjectRegisterEvent(pl->btn_signup,PX_OBJECT_EVENT_EXECUTE,GameStartup_LoginButtonOnSignUp,PX_NULL);
	}
	else
	{
		return PX_FALSE;
	}


	if(!PX_LoadTextureFromFile(&pl->pIns->runtime.mp_resources,&pl->LoginTexture,PX_LOGIN_TEXT_PATH_LOGO)) goto _ERROR;

	pl->status=PX_LOGIN_STATUS_STANDBY;

	if (!PX_UDPInit(&pl->udp,PX_UDP_IP_TYPE_IPV4))
	{
		goto _ERROR;
	}
	pl->serverAddr.ipv4=PX_inet_addr(LoginServerIpAddr);
	pl->serverAddr.port=port;
	pl->elpased=0;
	pl->times=0;
	return PX_TRUE;
_ERROR:
	PX_ObjectDelete(pl->logining_root);
	PX_ObjectDelete(pl->login_root);
	return PX_FALSE;
}

static px_void PX_ClientLoginUpdate_Exchange(PX_Login *pl,px_dword elpased)
{
	PX_UDP_ADDR addr;
	px_int readsize;
	px_byte data[PX_NETWORK_PACKET_MAX_SIZE];
	PX_Login_Packet_Header *pHeader=(PX_Login_Packet_Header *)data;
	PX_Login_Packet_ConnectAck *pAck=(PX_Login_Packet_ConnectAck *)data;
	PX_memset(&addr,0,sizeof(addr));
	PX_ObjectUpdate(pl->logining_root,elpased);
	PX_ApplicationMessageBoxDisplayContent(PX_LOGIN_TEXT_LOGINING_EXCHANGE_TEXT);
	pl->elpased+=elpased;

	if (PX_UDPReceived(&pl->udp,&addr,data,sizeof(data),&readsize))
	{
		if (pHeader->magic_numeric!=PX_NETWORKLOGIN_MAGIC||pHeader->opcode!=PX_NETWORKLOGIN_OPCODE_ACK)
		{
			return;
		}
		PX_Curve25519(pl->sharekey,pl->privatekey,pAck->publickey);
		pl->v_code=pAck->v_code;
		pl->times=0;
		pl->elpased=0;
		pl->status=PX_LOGIN_STATUS_LOGINING_LOGIN;

		PX_memset(pl->userName,0,sizeof(pl->userName));
		PX_memset(pl->password,0,sizeof(pl->userName));

		PX_strcpy(pl->userName,PX_Object_EditGetText(pl->edit_user),sizeof(pl->userName));
		PX_strcpy(pl->password,PX_Object_EditGetText(pl->edit_pwd),sizeof(pl->password));
		return;
	}


	if (pl->elpased>=PX_LOGIN_EXCHAGE_TIME)
	{
		PX_Login_Packet_Connect connect;
		connect.magic_numeric=PX_NETWORKLOGIN_MAGIC;
		connect.opcode=PX_NETWORKLOGIN_OPCODE_CONNECT;
		PX_memcpy(connect.publickey,pl->publickey,sizeof(connect.publickey));
		PX_UDPSend(&pl->udp,pl->serverAddr,(px_byte *)&connect,sizeof(connect));
		pl->times++;
		pl->elpased=0;
	}
	if (pl->times>PX_LOGIN_EXCHAGE_TIMES)
	{
		pl->status=PX_LOGIN_STATUS_FAILED;
		PX_ApplicationMessageBoxAlertOk("");
		pl->loginErrorMessage[0]=0;
		PX_wstrcat(pl->loginErrorMessage,(px_word *)PX_LOGIN_TEXT_CONNECTION_FAILED);
		pl->times=0;
		pl->elpased=0;
	}
}

static px_void DEMO_ClientLoginUpdate_Login(PX_Login *pl,px_dword elpased)
{
	extern px_void PX_ApplicationSwitchLobby();

	PX_UDP_ADDR addr;
	px_int readsize;

	px_byte data[PX_NETWORK_PACKET_MAX_SIZE];
	PX_Login_Packet_Header *pHeader=(PX_Login_Packet_Header *)data;
	PX_Login_Packet_loginAck *pAck=(PX_Login_Packet_loginAck *)data;

	PX_ObjectUpdate(pl->logining_root,elpased);

	PX_ApplicationMessageBoxDisplayContent(PX_LOGIN_TEXT_LOGINING_LOGIN_TEXT);
	pl->elpased+=elpased;


	if (PX_UDPReceived(&pl->udp,&addr,data,sizeof(data),&readsize))
	{
		if (pHeader->magic_numeric!=PX_NETWORKLOGIN_MAGIC||pHeader->opcode!=PX_NETWORKLOGIN_OPCODE_LOGINACK)
		{
			return;
		}
		switch(pAck->result)
		{
		case PX_NETWORKLOGIN_OPCODE_LOGINRESULT_SUCCEEDED:
			{
			PX_AES aes;
			PX_AES_Initialize(&aes,PX_AES_KeySize_Bits256,pl->sharekey);
			pl->status=PX_LOGIN_STATUS_SUCCEEDED;
			PX_AES_InvCipher(&aes,pAck->cookie,pl->get_cookie);
			PX_AES_InvCipher(&aes,pAck->cookie+PX_AES_BLOCK_BYTES_SIZE,pl->get_cookie+PX_AES_BLOCK_BYTES_SIZE);
			pl->id=pAck->id;
			pl->mapid=pAck->map_index;
			pl->elpased=0;
			pl->times=0;

			PX_ApplicationMessageBoxClose();
			PX_ApplicationSwitchLobby();
			
			}
			return;
		case PX_NETWORKLOGIN_OPCODE_LOGINRESULT_FAILED:
		default:
			{
				pl->loginErrorMessage[0]=0;
				PX_wstrcat(pl->loginErrorMessage,pAck->Message);
				PX_ApplicationMessageBoxAlertOk("");
				pl->status=PX_LOGIN_STATUS_FAILED;
			}
			
			return;
		}
	}

	if (pl->elpased>=PX_LOGIN_LOGIN_TIME)
	{
		PX_AES aes;
		PX_Login_Packet_login packLogin;
		packLogin.magic_numeric=PX_NETWORKLOGIN_MAGIC;
		packLogin.opcode=PX_NETWORKLOGIN_OPCODE_LOGIN;
		packLogin.version=GAME_VERSION;
		PX_AES_Initialize(&aes,PX_AES_KeySize_Bits256,pl->sharekey);
		PX_AES_Cipher(&aes,(px_byte *)pl->userName,packLogin.userName);
		PX_AES_Cipher(&aes,(px_byte *)pl->userName+PX_AES_BLOCK_BYTES_SIZE,packLogin.userName+PX_AES_BLOCK_BYTES_SIZE);
		PX_AES_Cipher(&aes,(px_byte *)pl->password,packLogin.password);
		PX_AES_Cipher(&aes,(px_byte *)pl->password+PX_AES_BLOCK_BYTES_SIZE,packLogin.password+PX_AES_BLOCK_BYTES_SIZE);
		packLogin.v_code=pl->v_code;
		PX_UDPSend(&pl->udp,pl->serverAddr,(px_byte *)&packLogin,sizeof(packLogin));
		pl->times++;
		pl->elpased=0;
	}
	if (pl->times>PX_LOGIN_LOGIN_TIMES)
	{
		pl->status=PX_LOGIN_STATUS_FAILED;
		pl->times=0;
		pl->elpased=0;
		pl->loginErrorMessage[0]=0;
		PX_ApplicationMessageBoxAlertOk("");
		PX_wstrcat(pl->loginErrorMessage,(px_word *)PX_LOGIN_TEXT_CONNECTION_FAILED);
	}
}



static px_void DEMO_ClientLoginUpdate_Failed(PX_Login *pl,px_dword elpased)
{
	extern px_void PX_ApplicationMessageBoxOkDisplayContent(const px_char content[]);
	PX_ObjectUpdate(pl->logining_root,elpased);
	PX_ApplicationMessageBoxOkDisplayContent((px_char *)pl->loginErrorMessage);
	pl->status=PX_LOGIN_STATUS_STANDBY;
}


px_void PX_LoginUpdate(PX_Login *pl,px_dword elpased)
{
	switch(pl->status)
	{
	case PX_LOGIN_STATUS_STANDBY:
		PX_ObjectUpdate(pl->login_root,elpased);
		break;
	case PX_LOGIN_STATUS_LOGINING_EXCHAGE:
		PX_ClientLoginUpdate_Exchange(pl,elpased);
		break;
	case PX_LOGIN_STATUS_LOGINING_LOGIN:
		DEMO_ClientLoginUpdate_Login(pl,elpased);
		break;
	case PX_LOGIN_STATUS_FAILED:
		DEMO_ClientLoginUpdate_Failed(pl,elpased);
		break;
	case PX_LOGIN_STATUS_SUCCEEDED:
		break;
	}
	
}

px_void PX_LoginRender(PX_Login *pl,px_dword elpased)
{
	switch(pl->status)
	{
	case PX_LOGIN_STATUS_STANDBY:
		PX_TextureRender(&pl->pIns->runtime.RenderSurface,&pl->LoginTexture,pl->pIns->runtime.width/2,pl->pIns->runtime.height/2-75,PX_TEXTURERENDER_REFPOINT_CENTER,PX_NULL);
		PX_ObjectRender(&pl->pIns->runtime.RenderSurface,pl->login_root,elpased);
		break;
	case PX_LOGIN_STATUS_LOGINING_EXCHAGE:
	case PX_LOGIN_STATUS_LOGINING_LOGIN:
	case PX_LOGIN_STATUS_LOGINING_DOWNLOAD:
	case PX_LOGIN_STATUS_FAILED:
		PX_ObjectRender(&pl->pIns->runtime.RenderSurface,pl->logining_root,elpased);
		break;
	case PX_LOGIN_STATUS_SUCCEEDED:
		break;
	}

	
}

px_void PX_LoginPostEvent(PX_Login *pl,PX_Object_Event e)
{
	PX_ObjectPostEvent(pl->login_root,e);
}

px_void PX_LoginReset(PX_Login *pl)
{
	pl->status=PX_LOGIN_STATUS_STANDBY;
	pl->elpased=0;
	pl->times=0;
}
