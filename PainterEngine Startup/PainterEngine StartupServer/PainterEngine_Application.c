#include "PainterEngine_Application.h"

PX_Application App;

static char scriptHeader[]=\
	"\
	host int AddRegistrationCode(string code,string admin,int count,int authority_level);\r\n\
	host int AddCoin(string account,int coin);\r\n\
	host int AddTicket(string account,int ticket);\r\n\
	host int RemoveRegistrationCode(string code);\r\n\
	host int SetNews(string news);\r\n\
	host int ListRegistryCode();\r\n\
	";


px_bool PX_Application_LobbyLogin(px_byte aes_code[32],px_char account[32],px_char pwd[32],_OUT px_byte cookie[32],_OUT px_dword *map_index,_OUT px_dword *id)
{
	return PX_StartupServer_LobbyLogin(&App.lobby,aes_code,account,pwd,cookie,map_index,id);
}

px_void PX_Application_ConsolePrint(const px_char *Text)
{
	PX_ConsolePrintText(&App.Instance.console,Text);
}

px_bool PX_Application_ConsoleVM_AddSignCode(PX_ScriptVM_Instance *Ins)
{
	PX_Console *pc=(PX_Console *)Ins->pThread[Ins->T].user_runtime_data;

	if (PX_ScriptVM_STACK(Ins,0).type!=PX_SCRIPTVM_VARIABLE_TYPE_STRING)
	{
		PX_ScriptVM_RET(Ins,PX_ScriptVM_Variable_int(0));
		return PX_TRUE;
	}

	if (PX_ScriptVM_STACK(Ins,1).type!=PX_SCRIPTVM_VARIABLE_TYPE_STRING)
	{
		PX_ScriptVM_RET(Ins,PX_ScriptVM_Variable_int(1));
		return PX_TRUE;
	}

	if (PX_ScriptVM_STACK(Ins,2).type!=PX_SCRIPTVM_VARIABLE_TYPE_INT)
	{
		PX_ScriptVM_RET(Ins,PX_ScriptVM_Variable_int(2));
		return PX_TRUE;
	}

	if (PX_ScriptVM_STACK(Ins,3).type!=PX_SCRIPTVM_VARIABLE_TYPE_INT)
	{
		PX_ScriptVM_RET(Ins,PX_ScriptVM_Variable_int(3));
		return PX_TRUE;
	}
	if (PX_StartupServer_SignAddRegCode(
		&App.signup,\
		PX_ScriptVM_STACK(Ins,0)._string.buffer,\
		PX_ScriptVM_STACK(Ins,1)._string.buffer,\
		PX_ScriptVM_STACK(Ins,2)._int,\
		PX_ScriptVM_STACK(Ins,3)._int
		))
	{
		PX_ConsolePrintText(&App.Instance.console,"Added registration code succeeded.");
	}
	else
	{
		PX_ConsolePrintText(&App.Instance.console,"Added registration code Failed.");
	}
	return PX_TRUE;
}

px_bool PX_Application_ConsoleVM_SetNews(PX_ScriptVM_Instance *Ins)
{
	px_char account[32]={0};
	PX_Console *pc=(PX_Console *)Ins->pThread[Ins->T].user_runtime_data;

	if (PX_ScriptVM_STACK(Ins,0).type!=PX_SCRIPTVM_VARIABLE_TYPE_STRING)
	{
		PX_ScriptVM_RET(Ins,PX_ScriptVM_Variable_int(0));
		return PX_TRUE;
	}

	PX_strset(App.lobby.News,PX_ScriptVM_STACK(Ins,0)._string.buffer);
	PX_ConsolePrintText(&App.Instance.console,"SetNews succeeded.");
	
	return PX_TRUE;
}

px_bool PX_Application_ConsoleVM_AddCoin(PX_ScriptVM_Instance *Ins)
{
	px_char account[32]={0};
	PX_Console *pc=(PX_Console *)Ins->pThread[Ins->T].user_runtime_data;

	if (PX_ScriptVM_STACK(Ins,0).type!=PX_SCRIPTVM_VARIABLE_TYPE_STRING)
	{
		PX_ScriptVM_RET(Ins,PX_ScriptVM_Variable_int(0));
		return PX_TRUE;
	}

	if (PX_ScriptVM_STACK(Ins,1).type!=PX_SCRIPTVM_VARIABLE_TYPE_INT)
	{
		PX_ScriptVM_RET(Ins,PX_ScriptVM_Variable_int(1));
		return PX_TRUE;
	}
	PX_strset(account,PX_ScriptVM_STACK(Ins,0)._string.buffer);
	if (PX_StartupServer_Database_AddCoin(&App.database,account,PX_ScriptVM_STACK(Ins,1)._int))
	{
		PX_StartupServer_LobbyUpdateClientUserInfoByAccount(&App.lobby,account);
		PX_ConsolePrintText(&App.Instance.console,"Added coins succeeded.");
	}
	else
	{
		PX_ConsolePrintText(&App.Instance.console,"Added coins Failed.");
	}
	return PX_TRUE;
}

px_bool PX_Application_ConsoleVM_AddTicket(PX_ScriptVM_Instance *Ins)
{
	px_char account[32]={0};
	PX_Console *pc=(PX_Console *)Ins->pThread[Ins->T].user_runtime_data;

	if (PX_ScriptVM_STACK(Ins,0).type!=PX_SCRIPTVM_VARIABLE_TYPE_STRING)
	{
		PX_ScriptVM_RET(Ins,PX_ScriptVM_Variable_int(0));
		return PX_TRUE;
	}

	if (PX_ScriptVM_STACK(Ins,1).type!=PX_SCRIPTVM_VARIABLE_TYPE_INT)
	{
		PX_ScriptVM_RET(Ins,PX_ScriptVM_Variable_int(1));
		return PX_TRUE;
	}
	PX_strset(account,PX_ScriptVM_STACK(Ins,0)._string.buffer);
	if (PX_StartupServer_Database_AddTicket(&App.database,account,PX_ScriptVM_STACK(Ins,1)._int))
	{
		PX_StartupServer_LobbyUpdateClientUserInfoByAccount(&App.lobby,account);
		PX_ConsolePrintText(&App.Instance.console,"Added coins succeeded.");
	}
	else
	{
		PX_ConsolePrintText(&App.Instance.console,"Added coins Failed.");
	}
	return PX_TRUE;
}

px_bool PX_Application_ConsoleVM_RemoveSignCode(PX_ScriptVM_Instance *Ins)
{
	PX_Console *pc=(PX_Console *)Ins->pThread[Ins->T].user_runtime_data;
	px_int i;
	if (PX_ScriptVM_STACK(Ins,0).type!=PX_SCRIPTVM_VARIABLE_TYPE_STRING)
	{
		PX_ScriptVM_RET(Ins,PX_ScriptVM_Variable_int(0));
		return PX_TRUE;
	}
	for (i=0;i<App.signup.regcode.size;i++)
	{
		PX_StartupServer_SignUp_RegCode *preg=PX_VECTORAT(PX_StartupServer_SignUp_RegCode,&App.signup.regcode,i);
		if (PX_strequ(preg->regCode,PX_ScriptVM_STACK(Ins,0)._string.buffer))
		{
			PX_VectorErase(&App.signup.regcode,i);
			break;
		}
	}
	return PX_TRUE;
}

px_bool PX_Application_ConsoleVM_ListSignCode(PX_ScriptVM_Instance *Ins)
{
	PX_Console *pc=(PX_Console *)Ins->pThread[Ins->T].user_runtime_data;
	px_int i;
	for (i=0;i<App.signup.regcode.size;i++)
	{
		//GameLoginServer_SignUp_RegCode *preg=PX_VECTORAT(GameLoginServer_SignUp_RegCode,&serverSignUp.regcode,i);
		//DEMO_Printf(DEMO_TEXT_LIST_REGISTRY_CODE,preg->regCode,preg->regCode,preg->sign_admin,preg->authority_level);
	}
	return PX_TRUE;
}

px_bool PC_ConsoleVM_registryHostCall(PX_ScriptVM_Instance *Ins)
{
	PX_ScriptVM_RegistryHostFunction(Ins,"ADDREGISTRATIONCODE",PX_Application_ConsoleVM_AddSignCode);//AddRegistryCode
	PX_ScriptVM_RegistryHostFunction(Ins,"REMOVEREGISTRATIONCODE",PX_Application_ConsoleVM_RemoveSignCode);//RemoveRegistryCode
	PX_ScriptVM_RegistryHostFunction(Ins,"LISTREGISTRYCODE",PX_Application_ConsoleVM_ListSignCode);//ListRegistryCode
	PX_ScriptVM_RegistryHostFunction(Ins,"ADDCOIN",PX_Application_ConsoleVM_AddCoin);//ListRegistryCode
	PX_ScriptVM_RegistryHostFunction(Ins,"ADDTICKET",PX_Application_ConsoleVM_AddTicket);//ListRegistryCode
	PX_ScriptVM_RegistryHostFunction(Ins,"SETNEWS",PX_Application_ConsoleVM_SetNews);//ListRegistryCode
	return PX_TRUE;
}

DWORD WINAPI PX_Application_Login_ThreadFunc(LPVOID p)
{
	DWORD time,elpased;
	time=timeGetTime();
	while(1)
	{
		elpased=timeGetTime()-time;
		time=timeGetTime();
		PX_StartupServer_LoginUpdate(&App.login,elpased);
		
		Sleep(1);
	}

}
DWORD WINAPI ServerSignUp_ThreadFunc(LPVOID p)
{
	DWORD time,elpased;
	time=timeGetTime();
	while(1)
	{
		elpased=timeGetTime()-time;
		time=timeGetTime();
		PX_StartupServer_SignUpUpdate(&App.signup,elpased);
		Sleep(1);
	}
}

DWORD WINAPI ServerLobby_ThreadFunc(LPVOID p)
{
	DWORD time,elpased;
	time=timeGetTime();
	while(1)
	{
		elpased=timeGetTime()-time;
		time=timeGetTime();
		PX_StartupServer_LobbyUpdate(&App.lobby,elpased);
		Sleep(1);
	}

}

px_bool PX_ApplicationInitialize(PX_Application *pApp)
{
	DWORD threadId;
	PX_ConsoleClear(&pApp->Instance.console);
	if (!PX_LoadTextureToResource(&pApp->Instance.runtime,"./resources/logo.traw","logo"))
	{
		return PX_FALSE;
	}
	PX_ConsolePrintImage(&pApp->Instance.console,"logo");

	PX_ConsoleShow(&pApp->Instance.console,PX_TRUE);
	
	if (!PX_StartupServer_Database_Initialize(&App.database))
	{
		PX_ConsolePrintText(&pApp->Instance.console,"Database connection failed");
		return PX_FALSE;
	}

	//////////////////////////////////////////////////////////////////////////
	//loginServer

	if(!PX_StartupServer_LoginInitialize(&App.login,PX_STARTUPSERVER_LOGIN_MAXCONNECTION))
	{
		PX_ConsolePrintText(&pApp->Instance.console,PX_APPLICATION_TEXT_LOGIN_SERVER_COULD_NOT_STARTUP);
	}
	else
	{
		CreateThread(NULL, 0, PX_Application_Login_ThreadFunc, 0, 0, &threadId);
	}

	if(!PX_StartupServer_SignUpInitialize(&App.signup,&App.database,PX_STARTUPSERVER_SIGNUP_MAXCONNECTION))
	{
		PX_ConsolePrintText(&pApp->Instance.console,PX_APPLICATION_TEXT_SIGNUP_SERVER_COULD_NOT_STARTUP);
	}
	else
	{
		CreateThread(NULL, 0, ServerSignUp_ThreadFunc, 0, 0, &threadId);
	}


	if(!PX_StartupServer_LobbyInitialize(&App.lobby,&App.database,PX_STARTUPSERVER_SIGNUP_MAXCONNECTION))
	{
		PX_ConsolePrintText(&pApp->Instance.console,PX_APPLICATION_TEXT_LOBBY_SERVER_COULD_NOT_STARTUP);
	}
	else
	{
		CreateThread(NULL, 0, ServerLobby_ThreadFunc, 0, 0, &threadId);
	}

	PX_ConsoleRegistryScriptHeader(&pApp->Instance.console,scriptHeader);
	PX_ConsoleRegistryHostCall(&pApp->Instance.console,PC_ConsoleVM_registryHostCall);

	return PX_TRUE;
}

px_void PX_ApplicationUpdate(PX_Application *App,px_dword elpased)
{
	PX_ConsoleUpdate(&App->Instance.console,elpased);
}

px_void PX_ApplicationRender(PX_Application *App,px_dword elpased)
{
	PX_ConsoleRender(&App->Instance.console,elpased);
	Sleep(10);
}

px_void PX_ApplicationPostEvent(PX_Application *App,PX_Object_Event e)
{
	if (e.Param_uint[0]==36)
	{
		return;
	}

	PX_ConsolePostEvent(&App->Instance.console,e);
}

