#include "Game_UI_ScorePanel.h"

px_bool Game_UI_ScorePanelInitialize(Game_UI_ScorePanel *ui,PX_Instance *pIns,Game_Play *pPlay)
{
	ui->pIns=pIns;
	ui->pPlay=pPlay;
	ui->displayScore=0;
	return PX_TRUE;
}

px_void Game_UI_ScorePanelRender(px_surface *renderSurface,Game_UI_ScorePanel *ui,px_int monitorPlayer,px_dword elpased)
{
	px_float score;
	px_char ScoreContent[32];
	px_word ScoreContentw[32];

	if (monitorPlayer>PX_COUNTOF(ui->pPlay->Players))
	{
		PX_ASSERT();
		return;
	}
	score=ui->pPlay->Players[monitorPlayer].Score;
	if (ui->displayScore>score)
	{
		ui->displayScore-=elpased;
		if (ui->displayScore<score)
		{
			ui->displayScore=(px_int)score;
		}
	}
	else
	{
		ui->displayScore+=elpased;
		if (ui->displayScore>score)
		{
			ui->displayScore=(px_int)score;
		}
	}

	PX_FontModuleDrawText(renderSurface,ui->pIns->runtime.width/2+92,48,(px_word *)L"Score:",PX_COLOR(255,0,0,0),&ui->pIns->FontModule24,PX_FONT_ALIGN_XRIGHT);
	PX_itoa((px_int)ui->displayScore,ScoreContent,sizeof(ScoreContent),10);
	PX_FontModule_atow(ScoreContent,ScoreContentw);
	PX_FontModuleDrawText(renderSurface,ui->pIns->runtime.width/2+104,64,ScoreContentw,PX_COLOR(255,0,0,0),&ui->pIns->FontModuleScore,PX_FONT_ALIGN_XLEFT);
}

