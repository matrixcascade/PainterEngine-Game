#include "PainterEngine_Application.h"

PX_Application App;



px_void PX_ApplicationMessageBoxAlert(const px_char content[])
{
	PX_MessageBoxAlert(&App.messagebox,content);
}

px_void PX_ApplicationMessageBoxAlertOk(const px_char content[])
{
	PX_MessageBoxAlertOk(&App.messagebox,content);
}

px_void PX_ApplicationMessageBoxAlertYesNo(const px_char content[],PX_MessageBoxCallBack yescallback,px_void *yesptr,PX_MessageBoxCallBack nocallback,px_void *noptr)
{
	PX_MessageBoxAlertYesNoEx(&App.messagebox,content,yescallback,yesptr,nocallback,noptr);
}

px_void PX_ApplicationMessageBoxOkDisplayContent(const px_char content[])
{
	if (App.messagebox.show)
	{
		App.messagebox.Message=content;
	}
	else
	{
		PX_ApplicationMessageBoxAlertOk(content);
	}
}


px_void PX_ApplicationMessageBoxDisplayContent(const px_char content[])
{
	if (App.messagebox.show)
	{
		App.messagebox.Message=content;
	}
	else
	{
		PX_ApplicationMessageBoxAlert(content);
	}
}

px_void PX_ApplicationMessageBoxClose()
{
	PX_MessageBoxClose(&App.messagebox);
}

px_void PX_ApplicationSwitchLogin(px_char account[],px_char pwd[])
{
	PX_LoginReset(&App.login);
	PX_Object_EditSetText(App.login.edit_user,account);
	PX_Object_EditSetText(App.login.edit_pwd,pwd);
	App.status=GAMESTARTUP_STATUS_LOGIN;
}

px_void PX_ApplicationSwitchSignup()
{
	App.status=GAMESTARTUP_STATUS_SIGNUP;
}

px_void PX_ApplicationSwitchLobby()
{
	App.status=GAMESTARTUP_STATUS_LOBBY;
	PX_LobbySetUserContent(&App.lobby,App.login.get_cookie,App.login.sharekey,App.login.mapid,App.login.id);
	PX_LobbyReset(&App.lobby);
}


px_bool PX_ApplicationInitialize(PX_Application *pApp)
{
	
	PX_Json_Value *pValue;
	struct hostent *host;
	WSADATA wsaData;
	WSAStartup( MAKEWORD(2, 2), &wsaData);

	if (!PX_JsonInitialize(&pApp->Instance.runtime.mp_game,&pApp->ConfigJson))
	{
		MessageBox(NULL,"JSON配置失败","error",MB_OK);
		return PX_FALSE;
	}

	if (!PX_LoadJsonFromFile(&pApp->ConfigJson,"./resources/config.json"))
	{
		MessageBox(NULL,"配置文件缺失","error",MB_OK);
		return PX_FALSE;
	}

	pValue=PX_JsonGetValue(&pApp->ConfigJson,"LoginServer.IP");
	if (!pValue)
	{
		MessageBox(NULL,"无效的登录服务地址","错误",MB_OK);
		return PX_FALSE;
	}
	host=gethostbyname(pValue->_string.buffer);
	if (!host)
	{
		MessageBox(NULL,"无效的登录服务地址","错误",MB_OK);
		return PX_FALSE;
	}
	if(!PX_LoginInitialize(&pApp->Instance,&pApp->login,inet_ntoa( *(struct in_addr*)host->h_addr_list[0]),PX_APPLICATION_CLIENTLOGIN_PORT))
	{
		MessageBox(NULL,"登录服务启动失败","错误",MB_OK);
			return PX_FALSE;
	}

	pValue=PX_JsonGetValue(&pApp->ConfigJson,"SignupServer.IP");
	if (!pValue)
	{
		MessageBox(NULL,"无效的注册服务地址","错误",MB_OK);
		return PX_FALSE;
	}
	host=gethostbyname(pValue->_string.buffer);
	if (!host)
	{
		MessageBox(NULL,"无效的注册服务地址","错误",MB_OK);
		return PX_FALSE;
	}
	if(!PX_SignUpInitialize(&pApp->Instance,&pApp->signup,inet_ntoa( *(struct in_addr*)host->h_addr_list[0]),PX_SIGNUP_SERVER_PORT))
	{
		MessageBox(NULL,"注册服务启动失败","错误",MB_OK);
		return PX_FALSE;
	}

	pValue=PX_JsonGetValue(&pApp->ConfigJson,"LobbyServer.IP");
	if (!pValue)
	{
		MessageBox(NULL,"无效的大厅服务地址","错误",MB_OK);
		return PX_FALSE;
	}
	host=gethostbyname(pValue->_string.buffer);
	if (!host)
	{
		MessageBox(NULL,"无效的游戏大厅服务地址","错误",MB_OK);
		return PX_FALSE;
	}

	if(!PX_LobbyInitialize(&pApp->Instance,&pApp->lobby,inet_ntoa( *(struct in_addr*)host->h_addr_list[0]),PX_LOBBY_SERVER_PORT,PX_LOBBY_SERVER_LARGEDATAPORT))
	{
		MessageBox(NULL,"大厅服务启动失败","错误",MB_OK);
		return PX_FALSE;
	}

	if(!PX_MessageBoxInitialize(&pApp->Instance.runtime,&pApp->messagebox,&pApp->Instance.fontmodule32,pApp->Instance.runtime.width,pApp->Instance.runtime.height))
		{
			MessageBox(NULL,"MessageBox初始化失败","error",MB_OK);
			return PX_FALSE;
		}

	if (!PX_FontModuleInitialize(&pApp->Instance.runtime.mp_resources,&App.Instance.fontmodule32))
		{
			MessageBox(NULL,"字模初始化失败","error",MB_OK);
			return PX_FALSE;
		}
	if (!PX_LoadFontModuleFromFile(&pApp->Instance.fontmodule32,"./resources/chs32.pxf"))
		{
			MessageBox(NULL,"字模缺失","error",MB_OK);
				return PX_FALSE;
		}
	if (!PX_LoadFontModuleFromFile(&pApp->Instance.fontmodule32,"./resources/ss32.pxf"))
		{
			MessageBox(NULL,"字模缺失","error",MB_OK);
			return PX_FALSE;
		}
	

	return PX_TRUE;
}

px_void PX_ApplicationUpdate(PX_Application *pApp,px_dword elpased)
{
	if (pApp->messagebox.show)
	{
		PX_MessageBoxUpdate(&pApp->messagebox,elpased);
	}

	switch(pApp->status)
	{
	case GAMESTARTUP_STATUS_LOGIN:
		{
			PX_LoginUpdate(&pApp->login,elpased);
		}
		break;
	case GAMESTARTUP_STATUS_SIGNUP:
		{
			PX_SignUpUpdate(&pApp->signup,elpased);
		}
		break;
	case GAMESTARTUP_STATUS_LOBBY:
		{
			PX_LobbyUpdate(&pApp->lobby,elpased);
		}
		break;
	}
}

px_void PX_ApplicationRender(PX_Application *pApp,px_dword elpased)
{
	PX_RuntimeRenderClear(&pApp->Instance.runtime,PX_COLOR(255,255,255,255));

	if (pApp->messagebox.show)
	{
		PX_MessageBoxRender(&pApp->Instance.runtime.RenderSurface,&pApp->messagebox,elpased);
		return;
	}

	switch(pApp->status)
	{
	case GAMESTARTUP_STATUS_LOGIN:
		{
			PX_LoginRender(&pApp->login,elpased);
		}
		break;
	case GAMESTARTUP_STATUS_SIGNUP:
		{
			PX_SignUpRender(&pApp->signup,elpased);
		}
		break;
	case GAMESTARTUP_STATUS_LOBBY:
		{
			PX_LobbyRender(&pApp->lobby,elpased);
		}
		break;
	}
}

px_void PX_ApplicationPostEvent(PX_Application *pApp,PX_Object_Event e)
{
	if (pApp->messagebox.show)
	{
		PX_MessageBoxPostEvent(&pApp->messagebox,e);
		return;
	}

	switch(pApp->status)
	{
	case GAMESTARTUP_STATUS_LOGIN:
		{
			PX_LoginPostEvent(&pApp->login,e);
		}
		break;
	case GAMESTARTUP_STATUS_SIGNUP:
		{
			PX_SignUpPostEvent(&pApp->signup,e);
		}
		break;
	case GAMESTARTUP_STATUS_LOBBY:
		{
			PX_LobbyPostEvent(&pApp->lobby,e);
		}
		break;
	}
}

