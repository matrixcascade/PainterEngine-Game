#include "Game_UI.h"

px_bool Game_UIInitialize(PX_Instance *pIns,Game_UI *ui,Game_Play *pPlay)
{
	ui->pIns=pIns;
	ui->pPlay=pPlay;

	ui->cursor.rotation=0;
	ui->cursor.pTexture=PX_ResourceLibraryGetTexture(&pIns->runtime.ResourceLibrary,GAME_TEXT_KEY_TEX_CURSOR);
	if(!Game_UI_StatusPanelInitialize(pIns,&ui->uiPanel,pPlay))
		return PX_FALSE;

	if (!Game_UI_MapInitialize(pIns,&ui->uiMap,pPlay,256))
		return PX_FALSE;

	if (!Game_UI_ScorePanelInitialize(&ui->uiScorePanel,pIns,pPlay))
		return PX_FALSE;

	if (!Game_UI_RankPanelInitialize(&ui->uiRankPanel,pIns,pPlay))
		return PX_FALSE;

	if (!Game_UI_MessageInitialize(&ui->uiMessage,pIns,pPlay))
		return PX_FALSE;


	return PX_TRUE;
}

px_void Game_UI_TreasureAlertRender(px_surface *renderSurface,Game_UI *ui,px_dword elpased)
{
	if (GAME_PLAY_TREASURE_GEN_DURATION-ui->pPlay->treasureGenElpased<10000&&GAME_PLAY_TREASURE_GEN_DURATION-ui->pPlay->treasureGenElpased>=0)
	{
		px_float schedule=(GAME_PLAY_TREASURE_GEN_DURATION-ui->pPlay->treasureGenElpased)/10000.0f;
		PX_GeoDrawRing(renderSurface,ui->pIns->runtime.width/2-160,ui->pIns->runtime.height-32,24,8,PX_COLOR(255,255,0,0),270,(px_uint)(270+360*schedule));
		PX_FontModuleDrawText(renderSurface,ui->pIns->runtime.width/2,ui->pIns->runtime.height-24,(px_word *)L"强化道具即将投放",PX_COLOR(192,255,0,0),&ui->pIns->FontModule32,PX_FONT_ALIGN_XCENTER);
	}

}

px_void Game_UI_GameCountDownRender(px_surface *renderSurface,Game_UI *ui,px_dword elpased)
{
	if (ui->pPlay->GameTime>=ui->pPlay->elpased)
	{
		px_word timeContent[32]={0};
		px_char NumericContent[8];
		px_dword reserveTime=ui->pPlay->GameTime-ui->pPlay->elpased;
		px_int min=reserveTime/1000/60;
		px_int second=(reserveTime/1000)%60;
		px_int millisecond=(reserveTime%1000)/10;

		PX_itoa(min,NumericContent,sizeof(NumericContent),10);
		PX_FontModule_wastrcat(timeContent,NumericContent);
		PX_FontModule_wastrcat(timeContent,":");

		PX_itoa(second,NumericContent,sizeof(NumericContent),10);
		PX_FontModule_wastrcat(timeContent,NumericContent);
		PX_FontModule_wastrcat(timeContent,".");

		PX_itoa(millisecond,NumericContent,sizeof(NumericContent),10);
		PX_FontModule_wastrcat(timeContent,NumericContent);
		if (reserveTime<=10000)
		{
			if ((reserveTime/500)&1)
			{
				PX_FontModuleDrawText(renderSurface,ui->pIns->runtime.width/2+256+64,64,timeContent,PX_COLOR(255,255,0,0),&ui->pIns->FontModule24,PX_FONT_ALIGN_XLEFT);
			}
			else
			{
				PX_FontModuleDrawText(renderSurface,ui->pIns->runtime.width/2+256+64,64,timeContent,PX_COLOR(128,64,64,64),&ui->pIns->FontModule24,PX_FONT_ALIGN_XLEFT);
			}
			
		}
		else
		{
			PX_FontModuleDrawText(renderSurface,ui->pIns->runtime.width/2+256+64,64,timeContent,PX_COLOR(255,0,168,0),&ui->pIns->FontModule24,PX_FONT_ALIGN_XLEFT);
		}
		
	}
	

}

px_void Game_UIRender(px_surface *renderSurface,Game_UI *ui,px_int monitorPlayer,px_int fps,px_dword elpased)
{
	ui->cursor.rotation+=elpased*0.36f;
	PX_TextureRenderRotation(renderSurface,ui->cursor.pTexture,(px_int)ui->pIns->Controller.Cursor.x,(px_int)ui->pIns->Controller.Cursor.y,PX_TEXTURERENDER_REFPOINT_CENTER,PX_NULL,(px_int)ui->cursor.rotation);
	Game_UI_StatusPanelRender(renderSurface,&ui->uiPanel,monitorPlayer,fps,elpased);
	Game_UI_MapRender(renderSurface,&ui->uiMap,elpased);
	Game_UI_ScorePanelRender(renderSurface,&ui->uiScorePanel,monitorPlayer,elpased);
	Game_UI_RankPanelRender(renderSurface,&ui->uiRankPanel,elpased);
	Game_UI_MessageRender(renderSurface,&ui->uiMessage,elpased);
	Game_UI_TreasureAlertRender(renderSurface,ui,elpased);
	Game_UI_GameCountDownRender(renderSurface,ui,elpased);
}



px_void Game_UIPostEvent(Game_UI *ui,PX_Object_Event e)
{
	Game_UI_StatusPanelPostEvent(&ui->uiPanel,e);
	Game_UI_MapPostEvent(&ui->uiMap,e);
}

