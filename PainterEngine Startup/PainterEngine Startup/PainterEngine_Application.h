#ifndef PAINTERENGINE_APPLICATION_H
#define PAINTERENGINE_APPLICATION_H
#include "PainterEngine_Startup.h"

#include "PainterEngine_Login.h"
#include "PainterEngine_SignUp.h"
#include "PainterEngine_Lobby.h"

//////////////////////////////////////////////////////////////////////////
//Configures
#define  PX_WINDOW_NAME     ""

#define  PX_WINDOW_WIDTH    800
#define  PX_WINDOW_HEIGHT   600

//memorypool for runtime(bytes)
#define PX_MEMORY_UI_SIZE (1024*1024*8)
#define PX_MEMORY_RESOURCES_SIZE (1024*1024*32)
#define PX_MEMORY_GAME_SIZE (1024*1024)


//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//Configurations

#define PX_APPLICATION_CLIENTLOGIN_PORT 31415
#define PX_SIGNUP_SERVER_PORT 31416
#define PX_LOBBY_SERVER_PORT 31417
#define PX_LOBBY_SERVER_LARGEDATAPORT 31418

typedef enum
{
	GAMESTARTUP_STATUS_LOGIN,
	GAMESTARTUP_STATUS_SIGNUP,
	GAMESTARTUP_STATUS_LOBBY,
}GAMESTARTUP_STATUS;


typedef struct
{
	PX_Instance Instance;
	PX_MessageBox messagebox;
	PX_SignUp signup;
	PX_Login login;
	PX_Lobby lobby;
	GAMESTARTUP_STATUS status;
	PX_Json ConfigJson;
}PX_Application;

extern PX_Application App;

px_bool PX_ApplicationInitialize(PX_Application *pApp);
px_void PX_ApplicationUpdate(PX_Application *App,px_dword elpased);
px_void PX_ApplicationRender(PX_Application *App,px_dword elpased);
px_void PX_ApplicationPostEvent(PX_Application *App,PX_Object_Event e);

#endif
