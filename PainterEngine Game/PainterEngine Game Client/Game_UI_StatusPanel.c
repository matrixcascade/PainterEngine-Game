#include "Game_UI_StatusPanel.h"

px_bool Game_UI_StatusPanelInitialize(PX_Instance *pIns,Game_UI_StatusPanel *ui,Game_Play *pPlay)
{
	ui->pIns=pIns;
	ui->pPlay=pPlay;
	ui->lastAtom=0;
	ui->flashelpased=0;
	ui->lastWeaponIndex=0;
	ui->lastWeaponCount=0;

	ui->weaponlogo[GAME_OBJECT_SHIP_WEAPON_TYPE_NONE]=PX_NULL;
	ui->weaponlogo[GAME_OBJECT_SHIP_WEAPON_TYPE_MACHINEGUN]=PX_ResourceLibraryGetShape(&pIns->runtime.ResourceLibrary,GAME_TEXT_KEY_MACHINEGUN_LOGO);
	ui->weaponlogo[GAME_OBJECT_SHIP_WEAPON_TYPE_NUCLEAR]=PX_ResourceLibraryGetShape(&pIns->runtime.ResourceLibrary,GAME_TEXT_KEY_NUCLEAR_LOGO);
	ui->weaponlogo[GAME_OBJECT_SHIP_WEAPON_TYPE_BLACKHOLE]=PX_ResourceLibraryGetShape(&pIns->runtime.ResourceLibrary,GAME_TEXT_KEY_BLACKHOLE_LOGO);
	ui->weaponlogo[GAME_OBJECT_SHIP_WEAPON_TYPE_MISSILE]=PX_ResourceLibraryGetShape(&pIns->runtime.ResourceLibrary,GAME_TEXT_KEY_MISSILE_LOGO);
	ui->weaponlogo[GAME_OBJECT_SHIP_WEAPON_TYPE_MINE]=PX_ResourceLibraryGetShape(&pIns->runtime.ResourceLibrary,GAME_TEXT_KEY_MINE_LOGO);
	ui->weaponlogo[GAME_OBJECT_SHIP_WEAPON_TYPE_RAILGUN]=PX_ResourceLibraryGetShape(&pIns->runtime.ResourceLibrary,GAME_TEXT_KEY_RAILGUN_LOGO);
	ui->weaponlogo[GAME_OBJECT_SHIP_WEAPON_TYPE_PROTECTSHIELD]=PX_ResourceLibraryGetShape(&pIns->runtime.ResourceLibrary,GAME_TEXT_KEY_PROTECTSHIELD_LOGO);
	ui->weaponlogo[GAME_OBJECT_SHIP_WEAPON_TYPE_PIERCER]=PX_ResourceLibraryGetShape(&pIns->runtime.ResourceLibrary,GAME_TEXT_KEY_PIERCER_LOGO);

	return PX_TRUE;
}

px_void Game_UI_StatusPanelRender(px_surface *renderSurface,Game_UI_StatusPanel *ui,px_int monitorPlayer,px_int fps,px_dword elpased)
{
	px_int i;
	px_color color;
	px_int weaponCount=0;
	px_float shield_pc,life_pc;
	px_float atom_pc;
	px_char  text[32];
	PX_Object *pShipObject;
	Game_Object_Ship *pShip; 
	px_int x,y;
	px_int coolx=0,cooly=0;
	px_float force=0,maxForce=1,shield=0,maxshield=1,velocity=0,maxvelocity=1,life=0,maxLife=1;
	px_int atomCount=0,maxAtomCount=1;

	pShipObject=ui->pPlay->Players[monitorPlayer].pShipObject;
	if (!pShipObject)
	{
		return;
	}

	pShip=Game_Object_GetShip(ui->pPlay->Players[monitorPlayer].pShipObject);

	if (pShipObject->Enabled)
	{
		force=pShip->force;
		maxForce=pShip->max_force;
		shield=pShip->shield;
		maxshield=pShip->max_shield;
		maxvelocity=pShip->max_speed;
		velocity=PX_PointMod(pShip->velocity);
		life=pShip->life;
		maxLife=pShip->max_life;
		if (pShip->WeaponStack[pShip->WeaponIndex].type!=GAME_OBJECT_SHIP_WEAPON_TYPE_NONE)
		{
			atomCount=pShip->WeaponStack[pShip->WeaponIndex].AtomCount;
			maxAtomCount=pShip->WeaponStack[pShip->WeaponIndex].MaxAtomCount;
		}
		for (i=0;i<PX_COUNTOF(pShip->WeaponStack);i++)
		{
			if (pShip->WeaponStack[i].type!=GAME_OBJECT_SHIP_WEAPON_TYPE_NONE)
			{
				weaponCount++;
			}

		}
	}

	x=32;
	y=32;

	color._argb.a=255;
	color._argb.r=(px_uchar)(force/maxForce*255);
	color._argb.g=255-color._argb.r;
	color._argb.b=96;

	PX_GeoDrawCircle(renderSurface,x,y,24,8,PX_COLOR(255,0,0,0));
	PX_GeoDrawRing(renderSurface,x,y,24,6,color,90,90+(px_int)(force/maxForce*360));
	PX_itoa((px_int)force,text,sizeof(text),10);
	PX_FontDrawText(renderSurface,x-3-__PX_FONT_ASCSIZE*(PX_strlen(text)-1)/2-1,26,text,color,PX_FONT_ALIGN_XLEFT);

	x+=64;

	color._argb.a=255;
	color._argb.r=(px_uchar)(velocity/maxvelocity*255);
	color._argb.g=255-color._argb.r;
	color._argb.b=96;

	PX_GeoDrawCircle(renderSurface,x,y,24,8,PX_COLOR(255,0,0,0));
	PX_GeoDrawRing(renderSurface,x,y,24,6,color,90,90+(px_int)(velocity/maxvelocity*360));
	PX_itoa((px_int)velocity,text,sizeof(text),10);
	PX_FontDrawText(renderSurface,x-3-__PX_FONT_ASCSIZE*(PX_strlen(text)-1)/2-1,26,text,color,PX_FONT_ALIGN_XLEFT);


	x+=36;
	y=8;
	shield_pc=shield*1.0f/maxshield;
	color._argb.a=255;
	color._argb.r=64;
	color._argb.g=64;
	color._argb.b=255;
	PX_GeoDrawBorder(renderSurface,x,y,x+104,y+16,2,PX_COLOR(255,0,0,0));
	PX_GeoDrawRect(renderSurface,x+2,y+2,(px_int)(shield_pc*100)+x+2,y+14,color);

	y+=19;

	life_pc=life*1.0f/maxLife;
	color._argb.a=255;
	color._argb.r=255-(px_uchar)(life_pc*255);
	color._argb.g=255-color._argb.r;
	color._argb.b=0;
	PX_GeoDrawBorder(renderSurface,x,y,x+104,y+16,2,PX_COLOR(255,0,0,0));
	PX_GeoDrawRect(renderSurface,x+2,y+2,(px_int)(life_pc*100)+x+2,y+14,color);


	y+=19;
	atom_pc=atomCount*1.0f/maxAtomCount;
	PX_GeoDrawBorder(renderSurface,x,y,x+104,y+16,2,PX_COLOR(255,0,0,0));

	

	if (ui->lastAtom!=atomCount)
	{
		ui->lastAtom=atomCount;
		ui->flashelpased=0;
	}

	ui->flashelpased+=elpased;

	if (ui->flashelpased<50)
	{
		color=PX_COLOR(255,255,0,0);
	}
	else
	{
		color=PX_COLOR(255,128,32,255);
	}
	PX_GeoDrawRect(renderSurface,x+2,y+2,(px_int)(atom_pc*100)+x+2,y+14,color);


	for (i=x;i<x+104;i+=3)
	{
		PX_GeoDrawLine(renderSurface,i,y,i,y+16,1,PX_COLOR(255,0,0,0));
	}


	//////////////////////////////////////////////////////////////////////////
	//fps
	x=139;
	y+=18;

	PX_sprintf1(text,sizeof(text),"fps:%1",PX_STRINGFORMAT_INT(fps));
	if (fps<25)
	{
		PX_FontDrawText(renderSurface,x,y,text,PX_COLOR(255,255,0,0),PX_FONT_ALIGN_XLEFT);
	}
	else if(fps<35)
	{
		PX_FontDrawText(renderSurface,x,y,text,PX_COLOR(255,255,255,0),PX_FONT_ALIGN_XLEFT);
	}
	else
	{
		PX_FontDrawText(renderSurface,x,y,text,PX_COLOR(255,37,119,0),PX_FONT_ALIGN_XLEFT);
	}

	//////////////////////////////////////////////////////////////////////////
	//Weapon Panel
	x+=118;
	y=8;

	coolx=x;
	cooly=y;

	if (pShipObject->Enabled)
	{
		px_int wi=0,ri=0;
		ui->weaponSwitchElpased+=elpased;

		wi=pShip->WeaponIndex;

		if (ui->lastWeaponCount!=weaponCount)
		{
			ui->lastWeaponCount=weaponCount;
			ui->weaponSwitchElpased=0;
		}

		if (ui->lastWeaponIndex!=wi)
		{
			ui->lastWeaponIndex=wi;
			ui->weaponSwitchElpased=0;
		}


		if (ui->weaponSwitchElpased<2000)
		{
			//current weapon
			if (pShip->WeaponStack[wi].type!=GAME_OBJECT_SHIP_WEAPON_TYPE_NONE)
			{
				ri=pShip->WeaponStack[wi].type;
				PX_ShapeRender(renderSurface,ui->weaponlogo[ri],x,y,PX_TEXTURERENDER_REFPOINT_LEFTTOP,PX_COLOR(255,(px_uchar)((1-ui->weaponSwitchElpased/2000.f)*255),0,0));
			}
			wi++;

			for (i=0;i<PX_COUNTOF(pShip->WeaponStack)-1;i++)
			{
				if (wi>=PX_COUNTOF(pShip->WeaponStack))
				{
					wi=0;
				}

				if (pShip->WeaponStack[wi].type!=GAME_OBJECT_SHIP_WEAPON_TYPE_NONE)
				{
					ri=pShip->WeaponStack[wi].type;

					x+=ui->weaponlogo[ri]->width+6;
					PX_ShapeRender(renderSurface,ui->weaponlogo[ri],x,y,PX_TEXTURERENDER_REFPOINT_LEFTTOP,PX_COLOR((px_uchar)((1-ui->weaponSwitchElpased/2000.f)*255),0,0,0));
				}
				wi++;
			}
		}
		else
		{
			//current weapon
			if (pShip->WeaponStack[wi].type!=GAME_OBJECT_SHIP_WEAPON_TYPE_NONE)
			{
				ri=pShip->WeaponStack[wi].type;
				PX_ShapeRender(renderSurface,ui->weaponlogo[ri],x,y,PX_TEXTURERENDER_REFPOINT_LEFTTOP,PX_COLOR(255,0,0,0));
			}
			wi++;
		}
		//weapon cool-down
		if (pShip->WeaponStack[pShip->WeaponIndex].type!=GAME_OBJECT_SHIP_WEAPON_TYPE_NONE)
		{
			if (pShip->WeaponStack[pShip->WeaponIndex].fire_elpased<pShip->WeaponStack[pShip->WeaponIndex].fire_duration)
			{
				px_float schedule=pShip->WeaponStack[pShip->WeaponIndex].fire_elpased*1.0f/pShip->WeaponStack[pShip->WeaponIndex].fire_duration;
				PX_GeoDrawRect(renderSurface,coolx,cooly+(px_int)(48*schedule),coolx+48-1,cooly+48-1,PX_COLOR(64,255,0,0));
			}
		}
		
		//weapon Count
		if (pShip->WeaponStack[pShip->WeaponIndex].type!=GAME_OBJECT_SHIP_WEAPON_TYPE_NONE)
		{
			px_char countText[16];
			PX_itoa(pShip->WeaponStack[pShip->WeaponIndex].AtomCount,countText,sizeof(countText),10);
			PX_FontDrawText(renderSurface,coolx+24,cooly+52,countText,PX_COLOR(255,255,0,0),PX_FONT_ALIGN_XCENTER);
		}

	}
}

px_void Game_UI_StatusPanelPostEvent(Game_UI_StatusPanel *ui,PX_Object_Event e)
{

}

