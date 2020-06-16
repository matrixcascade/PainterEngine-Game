#ifndef GAME_UI_SCOREPANEL_H
#define GAME_UI_SCOREPANEL_H

#include "../PainterEngine Game/Game_Play.h"
#include "../PainterEngine Game/Game_ObjectTypes.h"

typedef struct  
{
	PX_Instance *pIns;
	Game_Play *pPlay;
	px_int displayScore;
}Game_UI_ScorePanel;

px_bool Game_UI_ScorePanelInitialize(Game_UI_ScorePanel *ui,PX_Instance *pIns,Game_Play *pPlay);
px_void Game_UI_ScorePanelRender(px_surface *renderSurface,Game_UI_ScorePanel *ui,px_int monitorPlayer,px_dword elpased);

#endif