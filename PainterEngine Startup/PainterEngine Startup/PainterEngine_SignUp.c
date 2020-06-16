#include "PainterEngine_SignUp.h"

static const px_byte PX_SignUp_curve25519_basepoint[32] = {9};//curve25519 base point
static const px_char PX_SignUp_EditAccountLimit[]="1234567890zxcvbnmasdfghjklqwertyuiopQWERTYUIOPLKJHGFDSAZXCVBNM_.";
static const px_char PX_SignUp_EditPasswordLimit[]="1234567890zxcvbnmasdfghjklqwertyuiopQWERTYUIOPLKJHGFDSAZXCVBNM_~!@#$%^*().";
extern px_void PX_ApplicationSwitchLogin(px_char account[],px_char pwd[]);

px_void PX_SignUpRegistryButtonOnLogin(PX_Object * pObject,PX_Object_Event e,px_void *user_ptr)
{
	PX_ApplicationSwitchLogin("","");
}


px_void PX_SignUpRegistryButtonOnExit(PX_Object * pObject,PX_Object_Event e,px_void *user_ptr)
{
	exit(0);
}

px_void PX_SignUpRegistryPwdOnChange(PX_Object * pObject,PX_Object_Event e,px_void *user_ptr)
{
	PX_SignUp *pl=(PX_SignUp *)user_ptr;
	PX_Object_LabelSetText(pl->label_pwdInfo,"");
}

px_void PX_SignUpRegistryAccountOnChange(PX_Object * pObject,PX_Object_Event e,px_void *user_ptr)
{
	PX_SignUp *pl=(PX_SignUp *)user_ptr;
	PX_Object_LabelSetText(pl->label_accountInfo,"");
}

px_void PX_SignUpRegistryNickNameOnChange(PX_Object * pObject,PX_Object_Event e,px_void *user_ptr)
{
	PX_SignUp *pl=(PX_SignUp *)user_ptr;
	PX_Object_LabelSetText(pl->label_nickInfo,"");
}

px_void PX_SignUpRegistryRegCodeOnChange(PX_Object * pObject,PX_Object_Event e,px_void *user_ptr)
{
	PX_SignUp *pl=(PX_SignUp *)user_ptr;
	PX_Object_LabelSetText(pl->label_regcodeInfo,"");
}


px_void PX_SignUpRegistryButtonOnRegistry(PX_Object * pObject,PX_Object_Event e,px_void *user_ptr)
{
	px_int i;
	PX_SignUp *pl=(PX_SignUp *)user_ptr;

	if (PX_strlen(PX_Object_EditGetText(pl->edit_user))==0)
	{
		PX_Object_LabelSetText(pl->label_accountInfo,PX_SIGNUP_TEXT_ACCOUNT_COULD_NOT_EMPTY);
		return;
	}

	if (PX_strlen(PX_Object_EditGetText(pl->edit_nickname))==0)
	{
		PX_Object_LabelSetText(pl->label_nickInfo,PX_SIGNUP_TEXT_NICKNAME_COULD_NOT_EMPTY);
		return;
	}

	if (PX_strlen(PX_Object_EditGetText(pl->edit_regcode))==0)
	{
		PX_Object_LabelSetText(pl->label_regcodeInfo,PX_SIGNUP_TEXT_REGCODE_COULD_NOT_EMPTY);
		return;
	}

	if (PX_strlen(PX_Object_EditGetText(pl->edit_pwd))<8)
	{
		PX_Object_LabelSetText(pl->label_pwdInfo,PX_SIGNUP_TEXT_PASSWORD_TOO_SHORT);
		return;
	}

	if (!(PX_strequ(PX_Object_EditGetText(pl->edit_pwd),PX_Object_EditGetText(pl->edit_pwdrepeat))))
	{
		PX_Object_LabelSetText(pl->label_pwdInfo,PX_SIGNUP_TEXT_PASSWORD_NOT_MATCH);
		return;
	}

	pl->status=PX_SIGNUP_STATUS_REGISTING_EXCHAGE;
	//generate private key
	for(i=0;i<sizeof(pl->privatekey);i++)
	{
		pl->privatekey[i]=PX_rand()%0x100;
	}
	pl->privatekey[0]&= 248;
	pl->privatekey[31]&= 127;
	pl->privatekey[31]|= 64;

	PX_Curve25519(pl->publickey,pl->privatekey,PX_SignUp_curve25519_basepoint);

	PX_ObjectSetVisible(pl->btn_login,PX_FALSE);
	PX_ObjectSetVisible(pl->btn_reg,PX_FALSE);
	PX_ObjectSetVisible(pl->btn_exit,PX_FALSE);
	PX_ObjectSetVisible(pl->radius_signing,PX_TRUE);

	PX_Object_EditSetBackgroundColor(pl->edit_nickname,PX_COLOR(255,128,128,128));
	PX_Object_EditSetBackgroundColor(pl->edit_pwd,PX_COLOR(255,128,128,128));
	PX_Object_EditSetBackgroundColor(pl->edit_pwdrepeat,PX_COLOR(255,128,128,128));
	PX_Object_EditSetBackgroundColor(pl->edit_regcode,PX_COLOR(255,128,128,128));
	PX_Object_EditSetBackgroundColor(pl->edit_user,PX_COLOR(255,128,128,128));

}

px_bool PX_SignUpInitialize(PX_Instance *pIns,PX_SignUp *pR,const px_char SignupServerIpAddr[],px_ushort port)
{
	
	pR->pIns=pIns;
	PX_memset(&pR->ServerAddr,0,sizeof(pR->ServerAddr));
	PX_memset(pR->userName,0,sizeof(pR->userName));
	PX_memset(pR->password,0,sizeof(pR->password));
	PX_memset(pR->password,0,sizeof(pR->regcode));

	pR->signup_root=PX_NULL;
	pR->edit_pwd=PX_NULL;
	pR->edit_user=PX_NULL;
	pR->btn_exit=PX_NULL;
	pR->btn_reg=PX_NULL;

	pR->signup_root=PX_ObjectCreate(&pR->pIns->runtime.mp_ui,PX_NULL,0,0,0,0,0,0);
	if (pR->signup_root)
	{
		if(!(pR->edit_user=PX_Object_EditCreate(&pR->pIns->runtime.mp_ui,pR->signup_root,298,178,238,22,PX_COLOR(255,0,0,0)))) goto _ERROR;
		PX_Object_EditSetBorder(pR->edit_user,PX_FALSE);
		PX_Object_EditSetLimit(pR->edit_user,PX_SignUp_EditAccountLimit);

		if(!(pR->label_accountInfo=PX_Object_LabelCreate(&pR->pIns->runtime.mp_ui,pR->signup_root,545,178,128,22,"*ÕË»§",PX_COLOR(255,255,0,0)))) goto _ERROR;
		PX_Object_LabelSetBackgroundColor(pR->label_accountInfo,PX_COLOR(255,255,255,255));
		PX_Object_LabelSetAlign(pR->label_accountInfo,PX_OBJECT_ALIGN_LEFT|PX_OBJECT_ALIGN_VCENTER);

		if(!(pR->edit_nickname=PX_Object_EditCreate(&pR->pIns->runtime.mp_ui,pR->signup_root,298,231,238,22,PX_COLOR(255,0,0,0)))) goto _ERROR;
		PX_Object_EditSetBorder(pR->edit_nickname,PX_FALSE);

		if(!(pR->label_nickInfo=PX_Object_LabelCreate(&pR->pIns->runtime.mp_ui,pR->signup_root,545,231,128,22,"*êÇ³Æ",PX_COLOR(255,255,0,0)))) goto _ERROR;
		PX_Object_LabelSetBackgroundColor(pR->label_nickInfo,PX_COLOR(255,255,255,255));
		PX_Object_LabelSetAlign(pR->label_nickInfo,PX_OBJECT_ALIGN_LEFT|PX_OBJECT_ALIGN_VCENTER);


		if(!(pR->edit_pwd=PX_Object_EditCreate(&pR->pIns->runtime.mp_ui,pR->signup_root,298,284,238,22,PX_COLOR(255,0,0,0)))) goto _ERROR;
		PX_Object_EditSetBorder(pR->edit_pwd,PX_FALSE);
		PX_Object_EditSetPasswordStyle(pR->edit_pwd,PX_TRUE);
		PX_Object_EditSetLimit(pR->edit_pwd,PX_SignUp_EditPasswordLimit);

		if(!(pR->label_pwdInfo=PX_Object_LabelCreate(&pR->pIns->runtime.mp_ui,pR->signup_root,555,284,128,22,"*ÃÜÂë",PX_COLOR(255,255,0,0)))) goto _ERROR;
		PX_Object_LabelSetBackgroundColor(pR->label_pwdInfo,PX_COLOR(255,255,255,255));
		PX_Object_LabelSetAlign(pR->label_pwdInfo,PX_OBJECT_ALIGN_LEFT|PX_OBJECT_ALIGN_VCENTER);


		if(!(pR->edit_pwdrepeat=PX_Object_EditCreate(&pR->pIns->runtime.mp_ui,pR->signup_root,298,326,238,22,PX_COLOR(255,0,0,0)))) goto _ERROR;
		PX_Object_EditSetBorder(pR->edit_pwdrepeat,PX_FALSE);
		PX_Object_EditSetPasswordStyle(pR->edit_pwdrepeat,PX_TRUE);
		PX_Object_EditSetLimit(pR->edit_pwdrepeat,PX_SignUp_EditPasswordLimit);


		if(!(pR->edit_regcode=PX_Object_EditCreate(&pR->pIns->runtime.mp_ui,pR->signup_root,298,384,238,22,PX_COLOR(255,0,0,0)))) goto _ERROR;
		PX_Object_EditSetBorder(pR->edit_regcode,PX_FALSE);
		PX_Object_EditSetLimit(pR->edit_regcode,PX_SignUp_EditAccountLimit);

		if(!(pR->label_regcodeInfo=PX_Object_LabelCreate(&pR->pIns->runtime.mp_ui,pR->signup_root,545,384,128,22,"*×¢²áÂë",PX_COLOR(255,255,0,0)))) goto _ERROR;
		PX_Object_LabelSetBackgroundColor(pR->label_regcodeInfo,PX_COLOR(255,255,255,255));
		PX_Object_LabelSetAlign(pR->label_regcodeInfo,PX_OBJECT_ALIGN_LEFT|PX_OBJECT_ALIGN_VCENTER);


		if(!(pR->btn_reg=PX_Object_PushButtonCreate(&pR->pIns->runtime.mp_ui,pR->signup_root,255,450,128,32,PX_SIGNUP_TEXT_SIGNUP,PX_COLOR(255,0,0,0)))) goto _ERROR;
		PX_Object_PushButtonSetBackgroundColor(pR->btn_reg,PX_COLOR(255,255,255,255));
		PX_Object_PushButtonSetBorder(pR->btn_reg,PX_TRUE);
		PX_Object_PushButtonSetPushColor(pR->btn_reg,PX_COLOR(255,128,128,128));
		PX_Object_PushButtonSetBorderColor(pR->btn_reg,PX_COLOR(255,0,0,0));
		PX_Object_PushButtonSetCursorColor(pR->btn_reg,PX_COLOR(255,192,192,192));

		if(!(pR->btn_exit=PX_Object_PushButtonCreate(&pR->pIns->runtime.mp_ui,pR->signup_root,425,450,128,32,PX_SIGNUP_TEXT_EXIT,PX_COLOR(255,0,0,0)))) goto _ERROR;
		PX_Object_PushButtonSetBackgroundColor(pR->btn_exit,PX_COLOR(255,255,255,255));
		PX_Object_PushButtonSetPushColor(pR->btn_exit,PX_COLOR(255,128,128,128));
		PX_Object_PushButtonSetBorder(pR->btn_exit,PX_TRUE);
		PX_Object_PushButtonSetBorderColor(pR->btn_exit,PX_COLOR(255,0,0,0));
		PX_Object_PushButtonSetCursorColor(pR->btn_exit,PX_COLOR(255,192,192,192));

		if(!(pR->btn_login=PX_Object_PushButtonCreate(&pR->pIns->runtime.mp_ui,pR->signup_root,718,2,80,24,PX_SIGNUP_TEXT_LOGIN,PX_COLOR(255,25,25,255)))) goto _ERROR;
		PX_Object_PushButtonSetBackgroundColor(pR->btn_login,PX_COLOR(255,255,255,255));
		PX_Object_PushButtonSetBorder(pR->btn_login,PX_TRUE);
		PX_Object_PushButtonSetBorderColor(pR->btn_login,PX_COLOR(255,25,25,255));
		PX_Object_PushButtonSetCursorColor(pR->btn_login,PX_COLOR(255,192,192,192));
		PX_Object_PushButtonSetPushColor(pR->btn_login,PX_COLOR(255,128,128,128));

		
		PX_Object_EditSetMaxTextLength(pR->edit_pwd,16);
		PX_Object_EditSetMaxTextLength(pR->edit_pwdrepeat,16);
		PX_Object_EditSetMaxTextLength(pR->edit_user,16);
		PX_Object_EditSetMaxTextLength(pR->edit_nickname,16);
		PX_Object_EditSetMaxTextLength(pR->edit_regcode,16);

		PX_ObjectRegisterEvent(pR->btn_exit,PX_OBJECT_EVENT_EXECUTE,PX_SignUpRegistryButtonOnExit,PX_NULL);
		PX_ObjectRegisterEvent(pR->btn_reg,PX_OBJECT_EVENT_EXECUTE,PX_SignUpRegistryButtonOnRegistry,pR);
		PX_ObjectRegisterEvent(pR->btn_login,PX_OBJECT_EVENT_EXECUTE,PX_SignUpRegistryButtonOnLogin,PX_NULL);

		PX_ObjectRegisterEvent(pR->edit_pwd,PX_OBJECT_EVENT_VALUECHAGE,PX_SignUpRegistryPwdOnChange,pR);
		PX_ObjectRegisterEvent(pR->edit_pwdrepeat,PX_OBJECT_EVENT_VALUECHAGE,PX_SignUpRegistryPwdOnChange,pR);
		PX_ObjectRegisterEvent(pR->edit_user,PX_OBJECT_EVENT_VALUECHAGE,PX_SignUpRegistryAccountOnChange,pR);
		PX_ObjectRegisterEvent(pR->edit_nickname,PX_OBJECT_EVENT_VALUECHAGE,PX_SignUpRegistryNickNameOnChange,pR);
		PX_ObjectRegisterEvent(pR->edit_regcode,PX_OBJECT_EVENT_VALUECHAGE,PX_SignUpRegistryRegCodeOnChange,pR);
	}
	else
	{
		return PX_FALSE;
	}
	

	if(!PX_LoadTextureFromFile(&pR->pIns->runtime.mp_resources,&pR->SignUpTexture,"./resources/signup.traw")) goto _ERROR;
	if(!PX_LoadTextureFromFile(&pR->pIns->runtime.mp_resources,&pR->SigningTexture,"./resources/signing.traw")) goto _ERROR;

	if(!(pR->radius_signing=PX_Object_RotationCreate(&pR->pIns->runtime.mp_ui,pR->signup_root,-360,400,480,&pR->SigningTexture))) goto _ERROR;
	PX_ObjectSetVisible(pR->radius_signing,PX_FALSE);

	pR->status=PX_SIGNUP_STATUS_STANDBY;

	if (!PX_UDPInit(&pR->udp,PX_UDP_IP_TYPE_IPV4))
	{
		goto _ERROR;
	}
	pR->ServerAddr.ipv4=PX_inet_addr(SignupServerIpAddr);
	pR->ServerAddr.port=port;
	pR->elpased=0;
	pR->times=0;
	return PX_TRUE;
_ERROR:
	PX_ObjectDelete(pR->signup_root);
	return PX_FALSE;
}

static px_void PX_SignUpUpdate_Exchange(PX_SignUp *pR,px_dword elpased)
{
	PX_UDP_ADDR addr;
	px_int readsize;
	px_byte data[PX_NETWORK_PACKET_MAX_SIZE];
	PX_SignUp_Packet_Header *pHeader=(PX_SignUp_Packet_Header *)data;
	PX_SignUp_Packet_ConnectAck *pAck=(PX_SignUp_Packet_ConnectAck *)data;

	pR->elpased+=elpased;

	if (PX_UDPReceived(&pR->udp,&addr,data,sizeof(data),&readsize))
	{
		if (pHeader->magic_numeric!=PX_NETWORKSIGNUP_MAGIC||pHeader->opcode!=PX_NETWORKSIGNUP_OPCODE_ACK)
		{
			return;
		}
		PX_Curve25519(pR->sharekey,pR->privatekey,pAck->publickey);
		pR->times=0;
		pR->elpased=0;
		pR->status=PX_SIGNUP_STATUS_REGISTING_REG;

		PX_memset(pR->userName,0,sizeof(pR->userName));
		PX_memset(pR->nickName,0,sizeof(pR->nickName));
		PX_memset(pR->password,0,sizeof(pR->password));
		PX_memset(pR->regcode,0,sizeof(pR->regcode));

		PX_strcpy(pR->userName,PX_Object_EditGetText(pR->edit_user),sizeof(pR->userName));
		MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED,PX_Object_EditGetText(pR->edit_nickname),sizeof(pR->nickName),(LPWSTR)pR->nickName,PX_COUNTOF(pR->nickName));
		pR->nickName[PX_COUNTOF(pR->nickName)-1]=0;
		PX_strcpy(pR->password,PX_Object_EditGetText(pR->edit_pwd),sizeof(pR->password));
		PX_strcpy(pR->regcode,PX_Object_EditGetText(pR->edit_regcode),sizeof(pR->regcode));
		return;
	}


	if (pR->elpased>=PX_SIGNUP_EXCHAGE_TIME)
	{
		PX_SignUp_Packet_Connect connect;
		connect.magic_numeric=PX_NETWORKSIGNUP_MAGIC;
		connect.opcode=PX_NETWORKSIGNUP_OPCODE_CONNECT;
		PX_memcpy(connect.publickey,pR->publickey,sizeof(connect.publickey));
		PX_UDPSend(&pR->udp,pR->ServerAddr,(px_byte *)&connect,sizeof(connect));
		pR->times++;
		pR->elpased=0;
	}

	if (pR->times>PX_SIGNUP_EXCHAGE_TIMES)
	{
		PX_ApplicationMessageBoxAlertOk(PX_SIGNUP_TEXT_CONNECTION_FAILED);
		pR->status=PX_SIGNUP_STATUS_FAILED;
		pR->times=0;
		pR->elpased=0;
	}
}

static px_void PX_SignUpUpdate_SingUp(PX_SignUp *pR,px_dword elpased)
{
	px_int readsize;
	PX_UDP_ADDR addr;
	px_byte data[PX_NETWORK_PACKET_MAX_SIZE];
	PX_SignUp_Packet_SignAck *signAck=(PX_SignUp_Packet_SignAck *)data;

	if (PX_UDPReceived(&pR->udp,&addr,data,sizeof(data),&readsize))
	{
		if (signAck->magic_numeric!=PX_NETWORKSIGNUP_MAGIC||signAck->opcode!=PX_NETWORKSIGNUP_OPCODE_SIGNACK)
		{
			return;
		}
		switch(signAck->result)
		{
		case PX_NETWORKSIGNUP_SIGNACK_SUCCEED:
			pR->status=PX_SIGNUP_STATUS_SUCCEEDED;
			pR->times=0;
			pR->elpased=0;
			break;
		case PX_NETWORKSIGNUP_SIGNACK_INVALID_ACCOUNT:
			PX_Object_LabelSetText(pR->label_accountInfo,PX_SIGNUP_TEXT_INVALID_ACCOUNT);
			pR->status=PX_SIGNUP_STATUS_FAILED;
			pR->times=0;
			pR->elpased=0;
			break;
		case PX_NETWORKSIGNUP_SIGNACK_REPEAT_ACCOUNT:
			PX_Object_LabelSetText(pR->label_accountInfo,PX_SIGNUP_TEXT_REPEAT_ACCOUNT);
			pR->status=PX_SIGNUP_STATUS_FAILED;
			pR->times=0;
			pR->elpased=0;
			break;
		case PX_NETWORKSIGNUP_SIGNACK_INVALID_NICKNAME:
			PX_Object_LabelSetText(pR->label_nickInfo,PX_SIGNUP_TEXT_INVALID_NICKNAME);
			pR->status=PX_SIGNUP_STATUS_FAILED;
			pR->times=0;
			pR->elpased=0;
			break;
		case PX_NETWORKSIGNUP_SIGNACK_INVALID_PASSWORD:
			PX_Object_LabelSetText(pR->label_pwdInfo,PX_SIGNUP_TEXT_INVALID_PASSWORD);
			pR->status=PX_SIGNUP_STATUS_FAILED;
			pR->times=0;
			pR->elpased=0;
			break;
		case PX_NETWORKSIGNUP_SIGNACK_INVALID_REGCODE:
			PX_Object_LabelSetText(pR->label_regcodeInfo,PX_SIGNUP_TEXT_INVALID_SIGNUP_CODE);
			pR->status=PX_SIGNUP_STATUS_FAILED;
			pR->times=0;
			pR->elpased=0;
			break;
		case PX_NETWORKSIGNUP_SIGNACK_REPEAT_IP:
			PX_Object_LabelSetText(pR->label_regcodeInfo,PX_SIGNUP_TEXT_INVALID_REPEAT_IP);
			pR->status=PX_SIGNUP_STATUS_FAILED;
			pR->times=0;
			pR->elpased=0;
			break;
		}
		return;
	}

	pR->elpased+=elpased;
	if (pR->elpased>=PX_SIGNUP_REG_TIME)
	{
		PX_AES aes;
		PX_Signup_Packet_Sign packSignUp;
		PX_memset(&packSignUp,0,sizeof(packSignUp));

		packSignUp.magic_numeric=PX_NETWORKSIGNUP_MAGIC;
		packSignUp.opcode=PX_NETWORKSIGNUP_OPCODE_SIGN;
		
		PX_AES_Initialize(&aes,PX_AES_KeySize_Bits256,pR->sharekey);
		PX_AES_Cipher(&aes,(px_byte *)pR->userName,packSignUp.userName);
		PX_AES_Cipher(&aes,(px_byte *)pR->userName+PX_AES_BLOCK_BYTES_SIZE,packSignUp.userName+PX_AES_BLOCK_BYTES_SIZE);
		PX_AES_CipherBuffer(&aes,(px_byte *)pR->nickName,sizeof(pR->nickName),(px_byte *)packSignUp.nickName);
		PX_AES_Cipher(&aes,(px_byte *)pR->password,packSignUp.password);
		PX_AES_Cipher(&aes,(px_byte *)pR->password+PX_AES_BLOCK_BYTES_SIZE,packSignUp.password+PX_AES_BLOCK_BYTES_SIZE);
		PX_AES_Cipher(&aes,(px_byte *)pR->regcode,packSignUp.regcode);
		PX_AES_Cipher(&aes,(px_byte *)pR->regcode+PX_AES_BLOCK_BYTES_SIZE,packSignUp.regcode+PX_AES_BLOCK_BYTES_SIZE);

		PX_UDPSend(&pR->udp,pR->ServerAddr,(px_byte *)&packSignUp,sizeof(packSignUp));
		pR->times++;
		pR->elpased=0;
	}
	if (pR->times>PX_SIGNUP_REG_TIMES)
	{
		PX_ApplicationMessageBoxAlertOk(PX_SIGNUP_TEXT_CONNECTION_FAILED);
		pR->status=PX_SIGNUP_STATUS_FAILED;
		pR->times=0;
		pR->elpased=0;
	}
}

static px_void PX_SignUpReset(PX_SignUp *pR)
{
	PX_ObjectSetVisible(pR->btn_login,PX_TRUE);
	PX_ObjectSetVisible(pR->btn_reg,PX_TRUE);
	PX_ObjectSetVisible(pR->btn_exit,PX_TRUE);
	PX_ObjectSetVisible(pR->radius_signing,PX_FALSE);

	PX_Object_EditSetBackgroundColor(pR->edit_nickname,PX_COLOR(255,255,255,255));
	PX_Object_EditSetBackgroundColor(pR->edit_pwd,PX_COLOR(255,255,255,255));
	PX_Object_EditSetBackgroundColor(pR->edit_pwdrepeat,PX_COLOR(255,255,255,255));
	PX_Object_EditSetBackgroundColor(pR->edit_regcode,PX_COLOR(255,255,255,255));
	PX_Object_EditSetBackgroundColor(pR->edit_user,PX_COLOR(255,255,255,255));

	pR->status=PX_SIGNUP_STATUS_STANDBY;
}

static px_void PX_SignUpUpdate_Failed(PX_SignUp *pR,px_dword elpased)
{
	PX_SignUpReset(pR);
}


static px_void PX_SignUpSucceeded(PX_SignUp *pR,px_dword elpased)
{
	extern px_void PX_ApplicationSwitchLogin(px_char account[],px_char pwd[]);

	PX_ApplicationSwitchLogin(pR->userName,pR->password);
	PX_SignUpReset(pR);
	PX_ApplicationMessageBoxAlertOk(PX_SIGNUP_TEXT_REGISTRY_SUCCEEDED);
}

px_void PX_SignUpUpdate(PX_SignUp *pl,px_dword elpased)
{
	switch(pl->status)
	{
	case PX_SIGNUP_STATUS_STANDBY:
		PX_ObjectUpdate(pl->signup_root,elpased);
		break;
	case PX_SIGNUP_STATUS_REGISTING_EXCHAGE:
		PX_SignUpUpdate_Exchange(pl,elpased);
		break;
	case PX_SIGNUP_STATUS_REGISTING_REG:
		PX_SignUpUpdate_SingUp(pl,elpased);
		break;
	case PX_SIGNUP_STATUS_FAILED:
		PX_SignUpUpdate_Failed(pl,elpased);
		break;
	case PX_SIGNUP_STATUS_SUCCEEDED:
		PX_SignUpSucceeded(pl,elpased);
		break;
	}
	
}

px_void PX_SignUpRender(PX_SignUp *pl,px_dword elpased)
{
	switch(pl->status)
	{
	case PX_SIGNUP_STATUS_STANDBY:
	case PX_SIGNUP_STATUS_REGISTING_EXCHAGE:
	case PX_SIGNUP_STATUS_REGISTING_REG:
	case PX_SIGNUP_STATUS_FAILED:
		PX_TextureRender(&pl->pIns->runtime.RenderSurface,&pl->SignUpTexture,pl->pIns->runtime.width/2,pl->pIns->runtime.height/2-75,PX_TEXTURERENDER_REFPOINT_CENTER,PX_NULL);
		PX_ObjectRender(&pl->pIns->runtime.RenderSurface,pl->signup_root,elpased);
		break;
	case PX_SIGNUP_STATUS_SUCCEEDED:
		break;
	}

	
}

px_void PX_SignUpPostEvent(PX_SignUp *pl,PX_Object_Event e)
{

	switch(pl->status)
	{
	case PX_SIGNUP_STATUS_STANDBY:
		PX_ObjectPostEvent(pl->signup_root,e);
		break;
	case PX_SIGNUP_STATUS_REGISTING_EXCHAGE:
		break;
	case PX_SIGNUP_STATUS_REGISTING_REG:
		break;
	case PX_SIGNUP_STATUS_FAILED:
		break;
	case PX_SIGNUP_STATUS_SUCCEEDED:
		break;
	}

	
}
