#ifndef GAME_UI_MESSAGE_H
#define GAME_UI_MESSAGE_H

#include "../PainterEngine Game/Game_Play.h"
#include "../PainterEngine Game/Game_ObjectTypes.h"

typedef struct
{
	PX_Instance *pIns;
	Game_Play *pPlay;
	px_int currentIndex;
	px_dword currentElpased;
}Game_UI_Message;

px_bool Game_UI_MessageInitialize(Game_UI_Message *ui,PX_Instance *pIns,Game_Play *pPlay);
px_void Game_UI_MessageRender(px_surface *renderSurface,Game_UI_Message *ui,px_dword elpased);

#endif