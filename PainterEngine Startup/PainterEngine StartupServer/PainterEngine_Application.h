#ifndef PAINTERENGINE_APPLICATION_H
#define PAINTERENGINE_APPLICATION_H

#include "PainterEngine_StartupServer_SignUp.h"
#include "PainterEngine_StartupServer_Database.h"
#include "PainterEngine_StartupServer_Login.h"
#include "PainterEngine_StartupServer_Lobby.h"

//////////////////////////////////////////////////////////////////////////
//Configures
#define  PX_WINDOW_NAME     ""

#define  PX_WINDOW_WIDTH    800
#define  PX_WINDOW_HEIGHT   600

//memorypool for runtime(bytes)
#define PX_MEMORY_UI_SIZE (1024*1024*8)
#define PX_MEMORY_RESOURCES_SIZE (1024*1024*8)
#define PX_MEMORY_GAME_SIZE (2*1024*1024)
//////////////////////////////////////////////////////////////////////////

#define PX_APPLICATION_TEXT_LOGIN_SERVER_COULD_NOT_STARTUP "无法开启登录服务器"
#define PX_APPLICATION_TEXT_LOBBY_SERVER_COULD_NOT_STARTUP "无法开启游戏大厅服务器"
#define PX_APPLICATION_TEXT_SIGNUP_SERVER_COULD_NOT_STARTUP "无法开启注册服务器"

typedef struct
{
	PX_Instance Instance;
	PX_StartupServer_Database database;
	PX_StartupServer_Login login;
	PX_StartupServer_SignUp signup;
	PX_StartupServer_Lobby lobby;

}PX_Application;

extern PX_Application App;

px_bool PX_ApplicationInitialize(PX_Application *App);
px_void PX_ApplicationUpdate(PX_Application *App,px_dword elpased);
px_void PX_ApplicationRender(PX_Application *App,px_dword elpased);
px_void PX_ApplicationPostEvent(PX_Application *App,PX_Object_Event e);

#endif
