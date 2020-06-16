#include "Game_UI_Message.h"

px_bool Game_UI_MessageInitialize(Game_UI_Message *ui,PX_Instance *pIns,Game_Play *pPlay)
{
	ui->currentElpased=0;
	ui->currentIndex=-1;
	ui->pIns=pIns;
	ui->pPlay=pPlay;
	return PX_TRUE;
}

px_void Game_UI_MessageRender(px_surface *renderSurface,Game_UI_Message *ui,px_dword elpased)
{
	ui->currentElpased+=elpased;
	
	if (ui->currentElpased>8000)
	{
		if (ui->currentIndex<ui->pPlay->Messages.size-1)
		{
			ui->currentIndex++;
			ui->currentElpased=0;
		}
	}

	if (ui->currentIndex<ui->pPlay->Messages.size)
	{
		if (ui->currentElpased<=8000)
		{
			Game_Play_Message *pMessage=PX_VECTORAT(Game_Play_Message,&ui->pPlay->Messages,ui->currentIndex);

			if (ui->currentElpased>=3000)
			{
				px_uchar a=(px_uchar)((8000-ui->currentElpased)/5000.0f*255);
				PX_FontModuleDrawText(renderSurface,ui->pIns->runtime.width/2,128,pMessage->Message,PX_COLOR(a,255,0,0),&ui->pIns->FontModule32,PX_FONT_ALIGN_XCENTER);
			}
			else
			{
				PX_FontModuleDrawText(renderSurface,ui->pIns->runtime.width/2,128,pMessage->Message,PX_COLOR(255,255,0,0),&ui->pIns->FontModule32,PX_FONT_ALIGN_XCENTER);
			}
		}
	}

}

