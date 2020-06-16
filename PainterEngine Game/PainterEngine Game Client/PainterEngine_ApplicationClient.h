#ifndef PAINTERENGINE_APPLICATION_H
#define PAINTERENGINE_APPLICATION_H
#include "../PainterEngine Game/PainterEngine_Startup.h"
#include "../PainterEngine Game/Game_Play.h"
#include "PainterEngine_GameClientStartup.h"
#include "Game_UI.h"

//////////////////////////////////////////////////////////////////////////
//Configures
#define  PX_WINDOW_NAME     ""

#define  PX_WINDOW_WIDTH    1200
#define  PX_WINDOW_HEIGHT   800

//memorypool for runtime(bytes)
#define PX_MEMORY_UI_SIZE (1024*1024*32)
#define PX_MEMORY_RESOURCES_SIZE (1024*1024*96)
#define PX_MEMORY_GAME_SIZE (1024*1024*32)
//////////////////////////////////////////////////////////////////////////

typedef enum
{
	PX_APPLICATION_STATUS_DOWNLOADING,
	PX_APPLICATION_STATUS_CONNECTING,
	PX_APPLICATION_STATUS_WAITING,
	PX_APPLICATION_STATUS_GAMING,
	PX_APPLICATION_STATUS_GAMEOVER,
}PX_APPLICATION_STATUS;

#define PX_APPLICATION_CONNECTIONTIMEOUT_DELAY 45000


typedef struct
{
	PX_Instance Instance;
	
	PX_UDP udp;
	PX_SyncFrame_Client SyncFrameClient;
	PX_SyncData_Client  SyncDataClient;

	PX_MessageBox messagebox,SyncMessageBox;
	Game_Play play;
	Game_UI ui;

	px_int avg_fps;
	px_int sum_fps;
	px_dword fps_elpased;

	px_dword gameTime;
	px_dword syncTime;
	px_dword controllerSyncTime;
	px_dword remianUpdateTime;
	px_dword connectionTimeOutElpased;

	px_int   ObMonitorPlayer;
	px_dword enterGameOverElpased;
	px_dword gameOverElpased;

	PX_APPLICATION_STATUS status;
	Game_Play_Instr LastCursorInstr;
}PX_Application;

extern PX_Application App;

px_bool PX_ApplicationInitialize(PX_Application *App,const px_char ServerIpAddr[],px_ushort dataPort,px_ushort syncPort,px_dword client_id,px_dword server_id);
px_void PX_ApplicationUpdate(PX_Application *App,px_dword elpased);
px_void PX_ApplicationRender(PX_Application *App,px_dword elpased);
px_void PX_ApplicationPostEvent(PX_Application *App,PX_Object_Event e);
px_void PX_ApplicationSaveReplay(PX_Application *App);

#endif
