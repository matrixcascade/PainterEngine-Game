#ifndef GAME_UI_MAP_H
#define GAME_UI_MAP_H
#include "../PainterEngine Game/Game_Play.h"
#include "../PainterEngine Game/Game_ObjectTypes.h"

#define GAME_UI_MAP_SCALE 100

typedef struct  
{
	PX_Instance *pIns;
	Game_Play *pPlay;
	px_dword elpased;
	px_float scale;
	px_float rotation;
	px_shape map_star;
	px_shape map_treasure;
	px_texture backgroundTexture;
	px_texture renderTarget;
}Game_UI_Map;

px_bool Game_UI_MapInitialize(PX_Instance *pIns,Game_UI_Map *map,Game_Play *pPlay,px_int size);
px_void Game_UI_MapRender(px_surface *renderSurface,Game_UI_Map *map,px_dword elpased);
px_void Game_UI_MapPostEvent(Game_UI_Map *map,PX_Object_Event e);

#endif
