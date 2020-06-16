#include "Game_UI_RankPanel.h"

px_bool Game_UI_RankPanelInitialize(Game_UI_RankPanel *ui,PX_Instance *pIns,Game_Play *pPlay)
{
	ui->pIns=pIns;
	ui->pPlay=pPlay;
	return PX_TRUE;
}

px_void Game_UI_RankPanelRender(px_surface *renderSurface,Game_UI_RankPanel *ui,px_dword elpased)
{
	typedef struct
	{
		px_int player;
		px_bool alive;
		px_int score;
	}Game_UI_RankPanelData;
	px_int sortcount;
	px_int i,yoffset=0;
	PX_Object *pShipObject;
	Game_Object_Ship *pShip;
	Game_UI_RankPanelData Data[32];
	PX_QuickSortAtom SortData[32];
	px_char Content[64];
	px_word contentw[64];

	sortcount=0;
	
	for (i=0;i<ui->pPlay->PlayerCount;i++)
	{
		if (ui->pPlay->Players[i].pShipObject)
		{
			pShipObject=ui->pPlay->Players[i].pShipObject;
			pShip=Game_Object_GetShip(ui->pPlay->Players[i].pShipObject);
			if (pShipObject->Enabled)
			{
				Data[i].player=i;
				Data[i].alive=pShipObject->Enabled;
				Data[i].score=(px_int)ui->pPlay->Players[i].Score;

				SortData[sortcount].pData=&Data[i];
				SortData[sortcount].power=(px_float)Data[i].score;
				sortcount++;
			}
		}
	}

	if (sortcount)
	{
		PX_Quicksort_MaxToMin(SortData,0,sortcount-1);
		for (i=0;i<sortcount;i++)
		{
			Game_UI_RankPanelData *pData=(Game_UI_RankPanelData *)SortData[i].pData;
			pShip=Game_Object_GetShip(ui->pPlay->Players[pData->player].pShipObject);
			PX_sprintf1(Content,sizeof(Content),":%1",PX_STRINGFORMAT_INT(pData->score));
			contentw[0]=0;
			PX_wstrcat(contentw,pShip->PlayerName);
			PX_FontModule_wastrcat(contentw,Content);
			
			switch(i)
			{
			case 0:
				PX_GeoDrawRect(renderSurface,ui->pIns->runtime.width-194,32*i,ui->pIns->runtime.width-30,23+32*i,PX_COLOR(96,255,0,0));
				break;
			case 1:
				PX_GeoDrawRect(renderSurface,ui->pIns->runtime.width-194,32*i,ui->pIns->runtime.width-30,23+32*i,PX_COLOR(96,0,255,0));
				break;
			case 2:
				PX_GeoDrawRect(renderSurface,ui->pIns->runtime.width-194,32*i,ui->pIns->runtime.width-30,23+32*i,PX_COLOR(96,0,0,255));
				break;
			default:
				PX_GeoDrawRect(renderSurface,ui->pIns->runtime.width-194,32*i,ui->pIns->runtime.width-30,23+32*i,PX_COLOR(255,192,192,192));
			}
			
			PX_TextureRender(renderSurface,&pShip->photo_mini,ui->pIns->runtime.width-30,32*i,PX_TEXTURERENDER_REFPOINT_LEFTTOP,PX_NULL);

			PX_FontModuleDrawText(renderSurface,ui->pIns->runtime.width-32,20+32*i,contentw,PX_COLOR(255,0,128,0),&ui->pIns->FontModule24,PX_FONT_ALIGN_XRIGHT);

		}
		yoffset=32*i;
	}
	

	sortcount=0;

	for (i=0;i<ui->pPlay->PlayerCount;i++)
	{
		pShipObject=ui->pPlay->Players[i].pShipObject;
		pShip=Game_Object_GetShip(ui->pPlay->Players[i].pShipObject);
		if (!pShipObject->Enabled)
		{
			Data[i].player=i;
			Data[i].alive=pShipObject->Enabled;
			Data[i].score=(px_int)ui->pPlay->Players[i].Score;

			SortData[sortcount].pData=&Data[i];
			SortData[sortcount].power=(px_float)ui->pPlay->Players[i].aliveTime;
			sortcount++;
		}
	}
	if (sortcount)
	{
		PX_Quicksort_MaxToMin(SortData,0,sortcount-1);

		for (i=0;i<sortcount;i++)
		{
			Game_UI_RankPanelData *pData=(Game_UI_RankPanelData *)SortData[i].pData;
			pShip=Game_Object_GetShip(ui->pPlay->Players[pData->player].pShipObject);
			
			PX_FontModuleDrawText(renderSurface,ui->pIns->runtime.width-32,20+32*i+yoffset,pShip->PlayerName,PX_COLOR(255,168,168,168),&ui->pIns->FontModule24,PX_FONT_ALIGN_XRIGHT);

		}
	}
}

