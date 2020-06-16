#ifndef GAME_UI_H
#define GAME_UI_H
#include "../PainterEngine Game/Game_Play.h"
#include "../PainterEngine Game/Game_ObjectTypes.h"
#include "Game_UI_StatusPanel.h"
#include "Game_UI_Map.h"
#include "Game_UI_ScorePanel.h"
#include "Game_UI_RankPanel.h"
#include "Game_UI_Message.h"



typedef struct
{
	px_texture *pTexture;
	px_float rotation;
}Game_Object_Cursor;

typedef struct  
{
	PX_Instance *pIns;
	Game_Play *pPlay;
	Game_Object_Cursor cursor;
	Game_UI_StatusPanel uiPanel;
	Game_UI_Map uiMap;
	Game_UI_ScorePanel uiScorePanel;
	Game_UI_RankPanel uiRankPanel;
	Game_UI_Message uiMessage;
}Game_UI;

px_bool Game_UIInitialize(PX_Instance *pIns,Game_UI *ui,Game_Play *pPlay);
px_void Game_UIRender(px_surface *renderSurface,Game_UI *ui,px_int monitorPlayer,px_int fps,px_dword elpased);
px_void Game_UIPostEvent(Game_UI *ui,PX_Object_Event e);
#endif
