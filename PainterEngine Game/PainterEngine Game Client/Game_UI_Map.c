#include "Game_UI_Map.h"

px_bool Game_UI_MapInitialize(PX_Instance *pIns,Game_UI_Map *map,Game_Play *pPlay,px_int size)
{
	px_int largeSize;
	px_int i;
	map->elpased=0;
	map->pIns=pIns;
	map->pPlay=pPlay;
	map->scale=10;
	map->rotation=0;

	if(!PX_LoadShapeFromFile(&pIns->runtime.mp_resources,&map->map_star,GAME_TEXT_PATH_MAP_STAR))return PX_FALSE;
	if(!PX_LoadShapeFromFile(&pIns->runtime.mp_resources,&map->map_treasure,GAME_TEXT_PATH_TREASURETAGMINI))return PX_FALSE;

	if(!PX_TextureCreate(&pIns->runtime.mp_ui,&map->renderTarget,size,size)) return PX_FALSE;
	if(!PX_TextureCreate(&pIns->runtime.mp_ui,&map->backgroundTexture,size,size)) return PX_FALSE;
	largeSize=map->renderTarget.height>map->renderTarget.width?map->renderTarget.height:map->renderTarget.width;

	PX_GeoDrawLine(&map->backgroundTexture,0,size/2,size-1,size/2,1,PX_COLOR(255,32,255,32));
	PX_GeoDrawLine(&map->backgroundTexture,size/2,0,size/2,size-1,1,PX_COLOR(255,32,255,32));
	for (i=0;i<=largeSize/2;i+=32)
	{
		PX_GeoDrawCircle(&map->backgroundTexture,size/2,size/2,i,1,PX_COLOR((px_uchar)(255-i/1.1f),0,0,0));
	}
	return PX_TRUE;
}

px_void Game_UI_MapRender(px_surface *renderSurface,Game_UI_Map *map,px_dword elpased)
{
	px_int i;
	PX_World *pWorld=&map->pPlay->world;
	map->rotation+=elpased/1000.0f*60;

	PX_TextureCover(&map->renderTarget,&map->backgroundTexture,0,0,PX_TEXTURERENDER_REFPOINT_LEFTTOP);
	
	for (i=0;i<pWorld->pObjects.size;i++)
	{
		PX_WorldObject *pwo=PX_VECTORAT(PX_WorldObject,&pWorld->pObjects,i);
		if (pwo->pObject&&(!pwo->DeleteMark))
		{
			PX_Object *pObject=pwo->pObject;
			switch(pObject->Type)
			{
			case GAME_OBJECT_TYPE_SHIP:
			case GAME_OBJECT_TYPE_STONE:
			case GAME_OBJECT_TYPE_PICKUP:
			case GAME_OBJECT_TYPE_TREASURE:
			case GAME_OBJECT_TYPE_TREASURETAG:
			case GAME_OBJECT_TYPE_ALIEN:
			case GAME_OBJECT_TYPE_MISSILE:
				{
					//global x,y->local x,y
					px_int localX,localY;
					localX=(px_int)((pObject->x-pWorld->offsetx-pWorld->surface_width/2)/map->scale);
					localY=(px_int)((pObject->y-pWorld->offsety-pWorld->surface_height/2)/map->scale);
					
					if (localX*localX+localY*localY>map->renderTarget.width/2*map->renderTarget.width/2)
					{
						continue;
					}

					localX+=map->renderTarget.width/2;
					localY+=map->renderTarget.height/2;
					
					

					switch(pObject->Type)
					{
						case GAME_OBJECT_TYPE_SHIP:
							{
								if (pObject->Enabled)
								{
									if (Game_Object_GetShip(pObject)->bLocalController)
									{
										PX_GeoDrawSolidCircle(&map->renderTarget,localX,localY,5,PX_COLOR(255,0,255,0));
										PX_GeoDrawCircle(&map->renderTarget,localX,localY,5,1,PX_COLOR(255,0,0,0));
									}
									else
									{
										PX_GeoDrawSolidCircle(&map->renderTarget,localX,localY,5,PX_COLOR(255,255,0,0));
										PX_GeoDrawCircle(&map->renderTarget,localX,localY,5,1,PX_COLOR(255,0,0,0));
									}
								}
								
							}
							break;
						case GAME_OBJECT_TYPE_STONE:
							{
								PX_GeoDrawSolidCircle(&map->renderTarget,localX,localY,5,PX_COLOR(255,128,128,128));
								PX_GeoDrawCircle(&map->renderTarget,localX,localY,5,1,PX_COLOR(255,0,0,0));
							}
							break;
						case GAME_OBJECT_TYPE_ALIEN:
							{
								PX_GeoDrawSolidCircle(&map->renderTarget,localX,localY,5,PX_COLOR(255,192,192,32));
								PX_GeoDrawCircle(&map->renderTarget,localX,localY,5,1,PX_COLOR(255,0,0,0));
							}
							break;
						case GAME_OBJECT_TYPE_MISSILE:
							{
								PX_GeoDrawSolidCircle(&map->renderTarget,localX,localY,2,PX_COLOR(255,255,0,0));
								PX_GeoDrawCircle(&map->renderTarget,localX,localY,5,1,PX_COLOR(255,255,0,0));
							}
							break;
						case GAME_OBJECT_TYPE_PICKUP:
							{
								Game_Object_Pickup *pPickUp=(Game_Object_Pickup *)pObject->pObject;
								if (pPickUp->quality==GAME_OBJECT_PICKUP_QUILITY_NORMAL)
								{
									PX_ShapeRender(&map->renderTarget,&map->map_star,localX,localY,PX_TEXTURERENDER_REFPOINT_CENTER,PX_COLOR(255,0,0,0));
								}
								else
								{
									PX_ShapeRender(&map->renderTarget,&map->map_star,localX,localY,PX_TEXTURERENDER_REFPOINT_CENTER,PX_COLOR(255,255,0,0));
								}
							}
							break;
						case GAME_OBJECT_TYPE_TREASURE:
						case GAME_OBJECT_TYPE_TREASURETAG:
							{
								PX_ShapeRenderEx(&map->renderTarget,&map->map_treasure,localX,localY,PX_TEXTURERENDER_REFPOINT_CENTER,PX_COLOR(255,255,0,0),1.0f,map->rotation);
							}
							break;
					}
				}
				break;
			}
		}
	}

	PX_TextureRender(renderSurface,&map->renderTarget,renderSurface->width-map->renderTarget.width-1,renderSurface->height-map->renderTarget.height-1,PX_TEXTURERENDER_REFPOINT_LEFTTOP,PX_NULL);
}

px_void Game_UI_MapPostEvent(Game_UI_Map *map,PX_Object_Event e)
{

}

