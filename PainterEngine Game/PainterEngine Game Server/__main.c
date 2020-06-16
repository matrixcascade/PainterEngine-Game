#include "PainterEngine_ApplicationServer.h"

//mouse informations
POINT main_zoomPoint;
px_int main_ZoomRegion;
//////////////////////////////////////////////////////////////////////////
DWORD WINAPI DEMO_RenderThreadFunc(LPVOID p)
{   
	
	return 0;
}

//////////////////////////////////////////////////////////////////////////
//


int main(int argc,const px_char *argv[])
{
	DWORD time,elpased;
	const px_char *StartupFilePath=PX_NULL;
	PX_GameServerStartUp_Info networkStartup;
	PX_GameServerStartUp_PlayerNetInfo *pPlayerNetDesc;
	PX_GameStartUp_GameDesc *pGameDesc;
	PX_GameStartUp_PlayerDataDesc *pPlayerDataDesc;
	if (argc!=2)
	{
		StartupFilePath=PX_OpenFileDialog("");
	}
	else
	{
		StartupFilePath=argv[1];
	}

	if (StartupFilePath!=PX_NULL&&StartupFilePath[0]!=0)
	{
		FILE *pf=fopen(StartupFilePath,"rb");
		if (!pf)
		{
			return 0;
		}
		if (fread(&networkStartup,1,sizeof(PX_GameServerStartUp_Info),pf)!=sizeof(PX_GameServerStartUp_Info))
		{
			return 0;
		}
		pPlayerNetDesc=(PX_GameServerStartUp_PlayerNetInfo *)malloc(sizeof(PX_GameServerStartUp_PlayerNetInfo)*networkStartup.playerCount);
		if (fread(pPlayerNetDesc,1,sizeof(PX_GameServerStartUp_PlayerNetInfo)*networkStartup.playerCount,pf)!=sizeof(PX_GameServerStartUp_PlayerNetInfo)*networkStartup.playerCount)
		{
			return 0;
		}

		pGameDesc=(PX_GameStartUp_GameDesc *)malloc(sizeof(PX_GameStartUp_GameDesc));
		if (fread(pGameDesc,1,sizeof(PX_GameStartUp_GameDesc),pf)!=sizeof(PX_GameStartUp_GameDesc))
		{
			return 0;
		}

		pPlayerDataDesc=(PX_GameStartUp_PlayerDataDesc *)malloc(sizeof(PX_GameStartUp_PlayerDataDesc)*networkStartup.playerCount);
		if (fread(pPlayerDataDesc,1,sizeof(PX_GameStartUp_PlayerDataDesc)*networkStartup.playerCount,pf)!=sizeof(PX_GameStartUp_PlayerDataDesc)*networkStartup.playerCount)
		{
			return 0;
		}

		fclose(pf);
	}
	else
	{
		return 0;
	}

	PX_srand(314159);
	

	if(!PX_InstanceInitialize(&App.Instance,PX_WINDOW_NAME,PX_WINDOW_WIDTH,PX_WINDOW_HEIGHT,PX_MEMORY_UI_SIZE,PX_MEMORY_RESOURCES_SIZE,PX_MEMORY_GAME_SIZE))
	{
		return 0;
	}
	if(!PX_ApplicationInitialize(&App,networkStartup.dataport,networkStartup.gameport,pPlayerNetDesc,pGameDesc,pPlayerDataDesc,networkStartup.playerCount))
	{
		return 0;
	}
	
	time=timeGetTime();

	while(1)
	{
		elpased=timeGetTime()-time;
		if (elpased<2)
		{
			Sleep(2);
		}
		else
		{
			time=timeGetTime();
			PX_ApplicationUpdate(&App,elpased);
			Sleep(0);
		}
	}


	return 0;
}


