#ifndef PAINTERENGINE_APPLICATION_H
#define PAINTERENGINE_APPLICATION_H
#include "../PainterEngine Game/PainterEngine_Startup.h"
#include "../PainterEngine Game/Game_Play.h"
#include "PainterEngine_GameServerStartup.h"

//////////////////////////////////////////////////////////////////////////
//Configures
#define  PX_WINDOW_NAME     ""

#define  PX_WINDOW_WIDTH    0
#define  PX_WINDOW_HEIGHT   0

//memorypool for runtime(bytes)
#define PX_MEMORY_UI_SIZE (1024*1024)
#define PX_MEMORY_RESOURCES_SIZE (1024*1024*2)
#define PX_MEMORY_GAME_SIZE (1024*1024*32)

#define PX_APPLICATION_GAMEOVER_DELAY 6000
#define PX_APPLICATION_CONNECTIONTIMEOUT_DELAY 30000
//////////////////////////////////////////////////////////////////////////

typedef enum
{
	PX_APPLICATION_STATUS_CONNECTING,
	PX_APPLICATION_STATUS_GAMMING,
}PX_APPLICATION_STATUS;

typedef struct
{
	PX_Instance Instance;
	Game_Play play;
	PX_UDP GameUDP,DataUDP;
	px_dword syncTime;
	px_dword gameovrtelpased;
	PX_APPLICATION_STATUS status;
	px_dword connectionTimeOutElpased;
	PX_SyncFrame_Server SyncFrameServer;
	PX_SyncData_Server SyncDataServer;
}PX_Application;

extern PX_Application App;

px_bool PX_ApplicationInitialize(PX_Application *App,px_ushort dataport,px_ushort gameport,PX_GameServerStartUp_PlayerNetInfo PlayerNet[],PX_GameStartUp_GameDesc *gameDesc,PX_GameStartUp_PlayerDataDesc PlayerDataDesc[],px_int PlayerCount);
px_void PX_ApplicationUpdate(PX_Application *App,px_dword elpased);
px_void PX_ApplicationRender(PX_Application *App,px_dword elpased);
px_void PX_ApplicationPostEvent(PX_Application *App,PX_Object_Event e);

#endif
