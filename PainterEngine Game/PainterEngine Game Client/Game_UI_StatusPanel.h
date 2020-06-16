#ifndef GAME_UI_PANEL_H
#define GAME_UI_PANEL_H

#include "../PainterEngine Game/Game_Play.h"
#include "../PainterEngine Game/Game_ObjectTypes.h"

#define GAME_UI_MAX_SUPPORT_WEAPON 16

typedef struct  
{
	PX_Instance *pIns;
	Game_Play *pPlay;
	px_int lastAtom;
	px_dword weaponSwitchElpased;
	px_dword lastWeaponIndex;
	px_dword flashelpased;
	px_dword lastWeaponCount;
	px_shape *weaponlogo[GAME_UI_MAX_SUPPORT_WEAPON];
}Game_UI_StatusPanel;

px_bool Game_UI_StatusPanelInitialize(PX_Instance *pIns,Game_UI_StatusPanel *ui,Game_Play *pPlay);
px_void Game_UI_StatusPanelRender(px_surface *renderSurface,Game_UI_StatusPanel *ui,px_int monitorPlayer,px_int fps,px_dword elpased);
px_void Game_UI_StatusPanelPostEvent(Game_UI_StatusPanel *ui,PX_Object_Event e);

#endif