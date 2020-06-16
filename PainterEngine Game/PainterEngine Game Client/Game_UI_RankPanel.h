#ifndef GAME_UI_RANKPANEL_H
#define GAME_UI_RANKPANEL_H

#include "../PainterEngine Game/Game_Play.h"
#include "../PainterEngine Game/Game_ObjectTypes.h"

typedef struct  
{
	PX_Instance *pIns;
	Game_Play *pPlay;
}Game_UI_RankPanel;

px_bool Game_UI_RankPanelInitialize(Game_UI_RankPanel *ui,PX_Instance *pIns,Game_Play *pPlay);
px_void Game_UI_RankPanelRender(px_surface *renderSurface,Game_UI_RankPanel *ui,px_dword elpased);

#endif