#include "Game_Object_Ship.h"
#include "Game_Play.h"

Game_Object_Ship *Game_Object_GetShip(PX_Object *pObject)
{
	return (Game_Object_Ship *)pObject->pObject;
}

px_void Game_Object_ShipOnSwitch(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	Game_Object_Ship *pShip=Game_Object_GetShip(pObject);
	px_int i;
	for (i=0;i<PX_COUNTOF(pShip->WeaponStack);i++)
	{
		pShip->WeaponIndex++;
		if (pShip->WeaponIndex>=PX_COUNTOF(pShip->WeaponStack))
		{
			pShip->WeaponIndex=0;
		}

		if (pShip->WeaponStack[pShip->WeaponIndex].type!=GAME_OBJECT_SHIP_WEAPON_TYPE_NONE)
		{
			pShip->WeaponStack[pShip->WeaponIndex].fire_elpased=0;
			break;
		}
	}
	
}

px_void Game_Object_ShipOnController(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	Game_Object_Ship *pShip=Game_Object_GetShip(pObject);
	pShip->controller.Cursor.x=(px_float)e.Param_int[0];
	pShip->controller.Cursor.y=(px_float)e.Param_int[1];
	pShip->controller.LeftButtonDown=e.Param_int[2];
}


px_void Game_Object_ShipFree( PX_Object *pObject )
{
}



px_void Game_Object_ShipRender(px_surface *psurface, PX_Object *pObject,px_uint elpased)
{

	px_point direction,force,power_direction,blend_point;
	px_float f_distance,f_force;
	Game_Object_Ship *pShip=Game_Object_GetShip(pObject);
	
	if (pShip->bLocalController)
	{
		direction=pShip->pIns->Controller.Cursor;
		direction.x=direction.x-pObject->x;
		direction.y=direction.y-pObject->y;
	}
	else
	{
		direction=pShip->controller.Cursor;
		direction.x=direction.x-(pObject->x+pShip->pWorld->offsetx);
		direction.y=direction.y-(pObject->y+pShip->pWorld->offsety);
	}


	
	f_distance=PX_PointMod(direction);
	if (f_distance>GAME_SHIP_FORCE_MAX_DISTANCE)
	{
		f_distance=GAME_SHIP_FORCE_MAX_DISTANCE;
	}

	force=PX_PointMul(PX_PointUnit(direction),f_distance/GAME_SHIP_FORCE_MAX_DISTANCE*pShip->max_force);
	f_force=PX_PointMod(force);
	pShip->force=PX_PointMod(force);

	power_direction=PX_PointInverse(direction);

	blend_point=PX_PointReflectX(direction,pShip->power_main_point);
	blend_point.x+=pObject->x;
	blend_point.y+=pObject->y;
	PX_Object_GetPartical(pShip->ship_powerpartical_main)->launcher.direction=power_direction;
	PX_Object_GetPartical(pShip->ship_powerpartical_main)->launcher.launcherPosition=blend_point;
	//PX_Object_GetPartical(pShip->ship_powerpartical_main)->launcher.launcherVelocity=pShip->velocity;
	PX_Object_GetPartical(pShip->ship_powerpartical_main)->launcher.generateDuration=(px_dword)(20.f-18.f*f_force/pShip->max_force);

	blend_point=PX_PointReflectX(direction,pShip->power_assist_L_point);
	blend_point.x+=pObject->x;
	blend_point.y+=pObject->y;
	PX_Object_GetPartical(pShip->ship_powerpartical_l1)->launcher.direction=power_direction;
	PX_Object_GetPartical(pShip->ship_powerpartical_l1)->launcher.launcherPosition=blend_point;
	//PX_Object_GetPartical(pShip->ship_powerpartical_l1)->launcher.launcherVelocity=pShip->velocity;
	PX_Object_GetPartical(pShip->ship_powerpartical_l1)->launcher.generateDuration=(px_dword)(30.f-26.f*pShip->force/pShip->max_force);


	blend_point=PX_PointReflectX(direction,pShip->power_assist_R_point);
	blend_point.x+=pObject->x;
	blend_point.y+=pObject->y;
	PX_Object_GetPartical(pShip->ship_powerpartical_l2)->launcher.direction=power_direction;
	PX_Object_GetPartical(pShip->ship_powerpartical_l2)->launcher.launcherPosition=blend_point;
	//PX_Object_GetPartical(pShip->ship_powerpartical_l2)->launcher.launcherVelocity=pShip->velocity;
	PX_Object_GetPartical(pShip->ship_powerpartical_l2)->launcher.generateDuration=(px_dword)(30.f-26.f*pShip->force/pShip->max_force);



	if (pShip->showHelpLine&&pShip->bLocalController)
	{
		px_float x0,y0,x1,y1;
		x0=pObject->x;
		y0=pObject->y;
		x1=pShip->pIns->Controller.Cursor.x;
		y1=pShip->pIns->Controller.Cursor.y;
		PX_GeoDrawLine(psurface,(px_int)x0,(px_int)y0,(px_int)x1,(px_int)y1,1,PX_COLOR(128,0,192,255));
	}

	if (PX_PointSquare(direction))
	{
		PX_TextureRenderRotation_vector(psurface,pShip->shipObject,(px_int)pObject->x,(px_int)pObject->y,PX_TEXTURERENDER_REFPOINT_CENTER,PX_NULL,direction);
	}
	else
	{
		PX_TextureRender(psurface,pShip->shipObject,(px_int)pObject->x,(px_int)pObject->y,PX_TEXTURERENDER_REFPOINT_CENTER,PX_NULL);
	}

	if (pShip->last_shield_damage_elpased<200)
	{
		PX_TEXTURERENDER_BLEND blend;
		pShip->last_shield_damage_elpased+=elpased;

		blend.alpha=pShip->last_shield_damage_elpased/200.0f;
		blend.hdr_R=1;
		blend.hdr_G=1;
		blend.hdr_B=1;
		PX_TextureRender(psurface,pShip->shieldtex,(px_int)pObject->x,(px_int)pObject->y,PX_TEXTURERENDER_REFPOINT_CENTER,&blend);
	}
	

	//////////////////////////////////////////////////////////////////////////
	//Name
	if (pShip->bLocalController)
	{
		PX_FontModuleDrawText(psurface,(px_int)pObject->x,(px_int)pObject->y-48,pShip->PlayerName,PX_COLOR(255,0,192,0),&pShip->pIns->FontModule18,PX_FONT_ALIGN_XCENTER);
	}
	else
	{
		PX_FontModuleDrawText(psurface,(px_int)pObject->x,(px_int)pObject->y-48,pShip->PlayerName,PX_COLOR(255,192,0,0),&pShip->pIns->FontModule18,PX_FONT_ALIGN_XCENTER);
	}
	//photo
	do 
	{
		px_int fwidth=PX_FontModuleGetTextPixelsWidth(&pShip->pIns->FontModule18,pShip->PlayerName);
		PX_TextureRender(psurface,&pShip->photo_mini,(px_int)pObject->x-fwidth/2-26,(px_int)pObject->y-66,PX_TEXTURERENDER_REFPOINT_LEFTTOP,PX_NULL);
	} while (0);
	
	
	//////////////////////////////////////////////////////////////////////////
	//silent
	if (pShip->slientTime)
	{
		px_char content[16]={0};
		px_word wconetnt[32]={0};
		PX_sprintf1(content,sizeof(content),"%1.2s",PX_STRINGFORMAT_FLOAT(pShip->slientTime/1000.0f));
		PX_wstrcat(wconetnt,(const px_word *)L"ÎäÆ÷½âËøÓÚ");
		PX_FontModule_wastrcat(wconetnt,content);
		PX_FontModuleDrawText(psurface,(px_int)pObject->x,(px_int)pObject->y+48,wconetnt,PX_COLOR(255,192,0,0),&pShip->pIns->FontModule18,PX_FONT_ALIGN_XCENTER);
	}
}

PX_Partical_Atom Game_ParticalLauncher_ShipAssistPower_CreateAtom(struct _PX_Partical_Launcher *launcher,px_int index)
{
	PX_Partical_Atom atom;
	Game_Object_Ship *pShip=(Game_Object_Ship *)launcher->user;
	atom.aliveTime=300;
	atom.alpha=1.0f;
	atom.alphaIncrement=-3.f;
	atom.elpasedTime=0;
	atom.hdrB=0.2f;
	atom.hdrG=0.2f;
	atom.hdrR=2;
	atom.mass=1.0f;
	atom.position=PX_POINT(0,0,0);
	atom.roatationSpeed=0;
	atom.rotation=0;
	atom.size=0.1f;
	atom.sizeIncrement=5.f;
	atom.velocity=PX_POINT(pShip->force*4+100,(px_float)(-10+PX_rand()%20),0);
	return atom;
}

px_void Game_Object_ShipDeath(PX_Object *pObject,px_int attackPlayer)
{
	PX_Object_Event e;
	Game_Object_Ship *pShip=Game_Object_GetShip(pObject);
	pShip->life=0;
	pShip->shield=0;

	pObject->Enabled=PX_FALSE;
	pObject->Visible=PX_FALSE;
	pObject->ReceiveEvents=PX_FALSE;
	pObject->impact_Object_type=0;
	pObject->impact_test_type=0;

	if (attackPlayer!=pShip->player)
	{
		px_word Message[GAME_MESSAGE_LEN]={0};

		e.Event=GAME_OBJECT_EVENT_ADDSCORE;
		e.Param_int[0]=attackPlayer;//player
		e.Param_float[1]=GAME_SHIP_NAME_DEFEAT_SCORE;
		PX_ObjectPostEvent(pShip->playerLinkerObject,e);


		e.Event=GAME_OBJECT_EVENT_DEFEAT;
		e.Param_int[0]=attackPlayer;//player
		e.Param_int[1]=pShip->player;
		PX_ObjectPostEvent(pShip->playerLinkerObject,e);
	}

	Game_Object_FireExplosionCreate(pShip->pIns,pShip->pWorld,PX_POINT(pObject->x,pObject->y,0),30,PX_TRUE);
	Game_Object_PickupRandCreate3(pShip->pIns,pShip->pWorld,PX_POINT(pObject->x,pObject->y,0));
	PX_PlayLocalSoundFromResource(pShip->pIns,pShip->pWorld,pObject->x,pObject->y,GAME_TEXT_KEY_SND_EXPLODE01,PX_FALSE);
}
px_void Game_Object_ShipOnName(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	Game_Object_Ship *pShip=Game_Object_GetShip(pObject);
	px_word *pName=(px_word *)e.Param_ptr[0];
	
	if (PX_wstrlen(pName)<PX_COUNTOF(pShip->PlayerName))
	{
		pShip->PlayerName[0]=0;
		PX_wstrcat(pShip->PlayerName,pName);
	}
	
}
px_void Game_Object_ShipOnImpact(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	Game_Object_Ship *pShip=Game_Object_GetShip(pObject);
	PX_Object *pImpactObject=(PX_Object *)e.Param_ptr[0];
	px_point dvector;
	px_float v=PX_PointMod(pShip->velocity);
	if (v<GAME_SHIP_IMPACT_MIN_VELOCITY)
	{
		v=GAME_SHIP_IMPACT_MIN_VELOCITY;
	}
	
	dvector=PX_POINT(pImpactObject->x-pObject->x,pImpactObject->y-pObject->y,0);
	if (dvector.x>0.1||dvector.y>0.1)
	{
		dvector=PX_PointInverse(dvector);
		pShip->velocity=PX_PointMul(PX_PointUnit(dvector),v);
	}
}

px_void Game_Object_ShipAddWeapon(PX_Object *pObject,GAME_OBJECT_SHIP_WEAPON_TYPE type,px_int atomCount,px_dword fireDuration,px_int maxCount,px_dword turretType)
{
	px_int i;
	Game_Object_Ship *pShip=(Game_Object_Ship *)pObject->pObject;
	for (i=0;i<PX_COUNTOF(pShip->WeaponStack);i++)
	{
		//Search REPEAT
		if (pShip->WeaponStack[i].type==type)
		{
			pShip->WeaponStack[i].AtomCount+=atomCount;
			if (pShip->WeaponStack[i].AtomCount>pShip->WeaponStack[i].MaxAtomCount)
			{
				pShip->WeaponStack[i].AtomCount=pShip->WeaponStack[i].MaxAtomCount;
			}
			return;
		}
	}

	for (i=0;i<PX_COUNTOF(pShip->WeaponStack);i++)
	{
		//Search REPEAT
		if (pShip->WeaponStack[i].type==GAME_OBJECT_SHIP_WEAPON_TYPE_NONE)
		{
			pShip->WeaponStack[i].AtomCount=atomCount;
			pShip->WeaponStack[i].fire_duration=fireDuration;
			pShip->WeaponStack[i].fire_elpased=0;
			pShip->WeaponStack[i].MaxAtomCount=maxCount;
			pShip->WeaponStack[i].type=type;
			pShip->WeaponStack[i].turretType=turretType;
			return;
		}
	}
}

px_void Game_Object_ShipAddWeaponStack(PX_Object *pObject,GAME_OBJECT_SHIP_WEAPON_TYPE type,px_int AddMaxAtomCount)
{
	px_int i;
	Game_Object_Ship *pShip=(Game_Object_Ship *)pObject->pObject;
	for (i=0;i<PX_COUNTOF(pShip->WeaponStack);i++)
	{
		//Search REPEAT
		if (pShip->WeaponStack[i].type==type)
		{
			pShip->WeaponStack[i].MaxAtomCount+=AddMaxAtomCount;
			return;
		}
	}
}


px_void Game_Object_ShipOnAddWeapon(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	Game_Object_Ship *pShip=Game_Object_GetShip(pObject);
	px_int type=e.Param_int[0];
	px_int count=e.Param_int[1];
	if (e.Event==GAME_OBJECT_EVENT_ADDWEAPON)
	{
		switch (type)
		{
		case GAME_OBJECT_SHIP_WEAPON_TYPE_BLACKHOLE:
			Game_Object_ShipAddWeapon(pObject,GAME_OBJECT_SHIP_WEAPON_TYPE_BLACKHOLE,count,GAME_OBJECT_SHIP_WEAPON_BLACKHOLE_DURATION,GAME_OBJECT_SHIP_WEAPON_BLACKHOLE_MAX_CARGO,GAME_OBJECT_PLUGIN_TURRET_TYPE_LAUNCHER);
			break;
		case GAME_OBJECT_SHIP_WEAPON_TYPE_MACHINEGUN:
			Game_Object_ShipAddWeaponStack(pObject,GAME_OBJECT_SHIP_WEAPON_TYPE_MACHINEGUN,count);
			Game_Object_ShipAddWeapon(pObject,GAME_OBJECT_SHIP_WEAPON_TYPE_MACHINEGUN,1000,GAME_OBJECT_SHIP_WEAPON_MACHINEGUN_DURATION,GAME_OBJECT_SHIP_WEAPON_MACHINEGUN_MAX_CARGO,GAME_OBJECT_PLUGIN_TURRET_TYPE_GUN);
			break;
		case GAME_OBJECT_SHIP_WEAPON_TYPE_MINE:
			Game_Object_ShipAddWeapon(pObject,GAME_OBJECT_SHIP_WEAPON_TYPE_MINE,count,GAME_OBJECT_SHIP_WEAPON_MINE_DURATION,GAME_OBJECT_SHIP_WEAPON_MINE_MAX_CARGO,GAME_OBJECT_PLUGIN_TURRET_TYPE_LAUNCHER);
			break;
		case GAME_OBJECT_SHIP_WEAPON_TYPE_MISSILE:
			Game_Object_ShipAddWeapon(pObject,GAME_OBJECT_SHIP_WEAPON_TYPE_MISSILE,count,GAME_OBJECT_SHIP_WEAPON_MISSILE_DURATION,GAME_OBJECT_SHIP_WEAPON_MISSILE_MAX_CARGO,GAME_OBJECT_PLUGIN_TURRET_TYPE_MISSILE);
			break;
		case GAME_OBJECT_SHIP_WEAPON_TYPE_NUCLEAR:
			Game_Object_ShipAddWeapon(pObject,GAME_OBJECT_SHIP_WEAPON_TYPE_NUCLEAR,count,GAME_OBJECT_SHIP_WEAPON_NUCLEAR_DURATION,GAME_OBJECT_SHIP_WEAPON_NUCLEAR_MAX_CARGO,GAME_OBJECT_PLUGIN_TURRET_TYPE_LAUNCHER);
			break;
		case GAME_OBJECT_SHIP_WEAPON_TYPE_RAILGUN:
			Game_Object_ShipAddWeapon(pObject,GAME_OBJECT_SHIP_WEAPON_TYPE_RAILGUN,count,GAME_OBJECT_SHIP_WEAPON_RAILGUN_DURATION,GAME_OBJECT_SHIP_WEAPON_RAILGUN_MAX_CARGO,GAME_OBJECT_PLUGIN_TURRET_TYPE_CHARGER);
			break;
		case GAME_OBJECT_SHIP_WEAPON_TYPE_PROTECTSHIELD:
			Game_Object_ShipAddWeapon(pObject,GAME_OBJECT_SHIP_WEAPON_TYPE_PROTECTSHIELD,count,GAME_OBJECT_SHIP_WEAPON_PROTECTSHIELD_DURATION,GAME_OBJECT_SHIP_WEAPON_PROTECTSHIELD_MAX_CARGO,GAME_OBJECT_PLUGIN_TURRET_TYPE_CHARGER);
			break;
		case GAME_OBJECT_SHIP_WEAPON_TYPE_PIERCER:
			Game_Object_ShipAddWeapon(pObject,GAME_OBJECT_SHIP_WEAPON_TYPE_PIERCER,count,GAME_OBJECT_SHIP_WEAPON_PIERCER_DURATION,GAME_OBJECT_SHIP_WEAPON_PIERCER_MAX_CARGO,GAME_OBJECT_PLUGIN_TURRET_TYPE_GUN);
			break;
		}
	}
}

px_void Game_Object_ShipOnAddShield(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	Game_Object_Ship *pShip=Game_Object_GetShip(pObject);
	pShip->shield+=e.Param_float[0];
	if (pShip->shield>pShip->max_shield)
	{
		pShip->shield=pShip->max_shield;
	}
}

px_void Game_Object_ShipOnAddHealth(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	Game_Object_Ship *pShip=Game_Object_GetShip(pObject);
	pShip->life+=e.Param_float[0];
	if (pShip->life>pShip->max_life)
	{
		pShip->life=pShip->max_life;
	}
}

px_void Game_Object_ShipOnPowerUp(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	Game_Object_Ship *pShip=Game_Object_GetShip(pObject);
	pShip->max_force+=e.Param_float[0];
	if (pShip->max_force>pShip->forceLimit)
	{
		pShip->max_force=pShip->forceLimit;
	}
}

px_void Game_Object_ShipOnSpeedUp(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	Game_Object_Ship *pShip=Game_Object_GetShip(pObject);
	pShip->max_speed+=e.Param_float[0];
	if (pShip->max_speed>pShip->speedLimit)
	{
		pShip->max_speed=pShip->speedLimit;
	}
}

px_void Game_Object_ShipOnChangeVelocity(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	Game_Object_Ship *pShip=Game_Object_GetShip(pObject);
	pShip->velocity=PX_PointAdd(pShip->velocity,PX_POINT(e.Param_float[0],e.Param_float[1],0));
}

px_void Game_Object_ShipOnDamage(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	Game_Object_Ship *pShip=Game_Object_GetShip(pObject);
	PX_Object_Event ne;
	px_float Damage;
	px_float score;
	px_int AttackPlayer;
	AttackPlayer=e.Param_int[1];
	Damage=e.Param_float[0];
	score=Damage*10;
	Damage=Damage/(1+pShip->Armor);

	if (AttackPlayer!=pShip->player)
	{
		ne.Event=GAME_OBJECT_EVENT_ADDSCORE;
		ne.Param_int[0]=e.Param_int[1];//player
		ne.Param_float[1]=score;
		PX_ObjectPostEvent(pShip->playerLinkerObject,ne);
	}
	

	if (pShip->shield)
	{
		pShip->last_shield_damage_elpased=0;
		if (pShip->shield>Damage)
		{
			pShip->shield-=Damage;
			return;
		}
		else
		{
			Damage-=pShip->shield;
			pShip->shield=0;
		}
	}

	if (pShip->life>Damage)
	{
		pShip->life-=Damage;
	}
	else
	{
		pShip->life=0;
		Game_Object_ShipDeath(pObject,AttackPlayer);
	}
}

px_point Game_Object_ShipGetTurretPoint(PX_Object *pObject,px_int index)
{
	Game_Object_Ship *pShip=(Game_Object_Ship *)pObject->pObject;
	px_point pt=PX_PointReflectX(pShip->direction,pShip->Turret[index].offsetPoint);
	pt.x+=pObject->x;
	pt.y+=pObject->y;
	return pt;
}


px_void Game_Object_ShipFire(PX_Object *pObject)
{
	Game_Object_Ship *pShip=(Game_Object_Ship *)pObject->pObject;

	if(pShip->WeaponIndex>=PX_COUNTOF(pShip->WeaponStack))
	{
		pShip->WeaponIndex=0;
	}
	if (pShip->slientTime)
	{
		return;
	}
	
	if (pShip->WeaponStack[pShip->WeaponIndex].AtomCount>0)
	{
		if(pShip->WeaponStack[pShip->WeaponIndex].fire_elpased>=pShip->WeaponStack[pShip->WeaponIndex].fire_duration)
		{
			px_int turrent_i;
			pShip->WeaponStack[pShip->WeaponIndex].fire_elpased-=pShip->WeaponStack[pShip->WeaponIndex].fire_duration;
			for (turrent_i=0;turrent_i<PX_COUNTOF(pShip->Turret);turrent_i++)
			{
				if ((pShip->WeaponStack[pShip->WeaponIndex].turretType&pShip->Turret[turrent_i].type)&&pShip->Turret[turrent_i].bActivate)
				{
					px_point pt;			
					if (!pShip->WeaponStack[pShip->WeaponIndex].AtomCount)
					{
						break;
					}
					pShip->WeaponStack[pShip->WeaponIndex].AtomCount--;
					pt=Game_Object_ShipGetTurretPoint(pObject,turrent_i);
					switch(pShip->WeaponStack[pShip->WeaponIndex].type)
					{
					case  GAME_OBJECT_SHIP_WEAPON_TYPE_NONE:
						{

						}
						break;
					case GAME_OBJECT_SHIP_WEAPON_TYPE_MACHINEGUN:
						{
							Game_Object_LaunchFireCreate(pShip->pIns,pShip->pWorld,pt,pShip->direction,pShip->velocity,GAME_TEXT_KEY_MACHINEGUN_FIRESOUND);
							Game_Object_ShotgunCreate(pShip->pIns,pShip->pWorld,pt,pShip->direction,pShip->velocity,pShip->group,pShip->player);
						}
						break;
					case GAME_OBJECT_SHIP_WEAPON_TYPE_PIERCER:
						{
							Game_Object_LaunchFireCreate(pShip->pIns,pShip->pWorld,pt,pShip->direction,pShip->velocity,GAME_TEXT_KEY_PIERCER_FIRESOUND);
							Game_Object_PiercerCreate(pShip->pIns,pShip->pWorld,pt,pShip->direction,pShip->velocity,pShip->group,pShip->player);
						}
						break;
					case GAME_OBJECT_SHIP_WEAPON_TYPE_RAILGUN:
						{
							Game_Object_RailgunCreate(pShip->pIns,pShip->pWorld,pt,pShip->direction,PX_POINT(0,0,0),pShip->group,pShip->player);
							Game_Object_RailgunFireCreate(pShip->pIns,pShip->pWorld,pt);
						}
						break;
					case GAME_OBJECT_SHIP_WEAPON_TYPE_MISSILE:
						{
							Game_Object_LaunchFireCreate(pShip->pIns,pShip->pWorld,pt,pShip->direction,pShip->velocity,GAME_TEXT_KEY_MACHINEGUN_FIRESOUND);
							Game_Object_MissileCreate(pShip->pIns,pShip->pWorld,pt,pShip->direction,pShip->group,pShip->player);
						}
						break;
					case GAME_OBJECT_SHIP_WEAPON_TYPE_NUCLEAR:
						{
							Game_Object_NuclearCreate(pShip->pIns,pShip->pWorld,pt,pShip->direction,pShip->group,pShip->player);
						}
						break;
					case GAME_OBJECT_SHIP_WEAPON_TYPE_MINE:
						{
							Game_Object_LaunchFireCreate(pShip->pIns,pShip->pWorld,pt,pShip->direction,pShip->velocity,GAME_TEXT_KEY_MACHINEGUN_FIRESOUND);
							Game_Object_MineCreate(pShip->pIns,pShip->pWorld,pt,pShip->direction,pShip->group,pShip->player);
						}
						break;
					case GAME_OBJECT_SHIP_WEAPON_TYPE_BLACKHOLE:
						{
							Game_Object_LaunchFireCreate(pShip->pIns,pShip->pWorld,pt,pShip->direction,pShip->velocity,GAME_TEXT_KEY_MACHINEGUN_FIRESOUND);
							Game_Object_BlackHoleCreate(pShip->pIns,pShip->pWorld,pt,pShip->direction,pShip->group,pShip->player);
						}
						break;
					case GAME_OBJECT_SHIP_WEAPON_TYPE_PROTECTSHIELD:
						{
							Game_Object_ProtectShieldCreate(pShip->pIns,pShip->pWorld,pObject,pShip->player);
						}
						break;
					}
				}
			}
			pShip->last_fire_elpased=0;
		}
	}
}

px_void Game_Object_ShipOnFire(PX_Object *pObject,PX_Object_Event e,px_void *ptr)
{
	Game_Object_ShipFire(pObject);
}

#include "stdio.h"

px_void Game_Object_ShipUpdate(PX_Object *pObject,px_uint elpased)
{
	px_float f_distance;
	px_int i;
	px_point force;
	
	//px_point direction;
	Game_Object_Ship *pShip;
	if (pObject==PX_NULL)
	{
		PX_ASSERT();
	}

	pShip=Game_Object_GetShip(pObject);

	if (!pShip)
	{
		PX_ASSERT();
		return;
	}



	pShip->direction=pShip->controller.Cursor;
	pShip->direction.x=pShip->direction.x-pObject->x;
	pShip->direction.y=pShip->direction.y-pObject->y;

	//sync direction
	f_distance=PX_PointMod(pShip->direction);
	if (f_distance>GAME_SHIP_FORCE_MAX_DISTANCE)
	{
		f_distance=GAME_SHIP_FORCE_MAX_DISTANCE;
	}

	force=PX_PointMul(PX_PointUnit(pShip->direction),f_distance/GAME_SHIP_FORCE_MAX_DISTANCE*pShip->max_force);


	//Update velocity
	pShip->velocity.x+=force.x*elpased/1000;
	pShip->velocity.y+=force.y*elpased/1000;
	pShip->velocity.z=0;

	if (PX_PointMod(pShip->velocity)>pShip->max_speed)
	{
		pShip->velocity=PX_PointMul(PX_PointUnit(pShip->velocity),pShip->max_speed);
	}


	if (pObject->x+pShip->velocity.x*elpased/1000<0)
	{
		pShip->velocity.x/=-2;
	}
	else if (pObject->x+pShip->velocity.x*elpased/1000>pShip->pWorld->world_width)
	{
		pShip->velocity.x/=-2;
	}
	else
	{
		pObject->x+=pShip->velocity.x*elpased/1000;
	}

	if (pObject->y+pShip->velocity.y*elpased/1000<0)
	{
		pShip->velocity.y/=-2;
	}
	else if (pObject->y+pShip->velocity.y*elpased/1000>pShip->pWorld->world_height)
	{
		pShip->velocity.y/=-2;
	}
	else
		pObject->y+=pShip->velocity.y*elpased/1000;

	pObject->z=GAME_SHIP_Z;



	
	//weapon
	//generate
	if (pShip->last_fire_elpased>pShip->last_fire_recovery)
	{
		pShip->base_weapon_generator_elpased+=elpased;
		while (pShip->base_weapon_generator_elpased>=pShip->base_weapon_generator_time)
		{
			pShip->base_weapon_generator_elpased-=pShip->base_weapon_generator_time;
			if (pShip->WeaponStack[0].AtomCount<pShip->WeaponStack[0].MaxAtomCount)
			{
				pShip->WeaponStack[0].AtomCount++;
			}
		}
	}
	else
	{
		pShip->last_fire_elpased+=elpased;
	}
	

	//fire
	if (pShip->WeaponStack[pShip->WeaponIndex].type!=GAME_OBJECT_SHIP_WEAPON_TYPE_NONE)
	{
		pShip->WeaponStack[pShip->WeaponIndex].fire_elpased+=elpased;
		if (pShip->WeaponStack[pShip->WeaponIndex].fire_elpased>pShip->WeaponStack[pShip->WeaponIndex].fire_duration)
		{
			pShip->WeaponStack[pShip->WeaponIndex].fire_elpased=pShip->WeaponStack[pShip->WeaponIndex].fire_duration;
		}
	}

	//slient
	if (pShip->slientTime)
	{
		if (elpased>pShip->slientTime)
		{
			pShip->slientTime=0;
		}
		else
		{
			pShip->slientTime-=elpased;
		}
	}

	if (pShip->controller.LeftButtonDown)
	{
		Game_Object_ShipFire(pObject);
	}

	//Weapon Clear
	for (i=1;i<PX_COUNTOF(pShip->WeaponStack);i++)
	{
		if (pShip->WeaponStack[i].type!=GAME_OBJECT_SHIP_WEAPON_TYPE_NONE)
		{
			if (pShip->WeaponStack[i].AtomCount==0)
			{
				pShip->WeaponStack[i].type=GAME_OBJECT_SHIP_WEAPON_TYPE_NONE;
				pShip->WeaponIndex=0;
				pShip->WeaponStack[i].fire_elpased=0;
				pShip->WeaponStack[0].fire_elpased=0;
			}
		}
	}
}


px_texture *Game_Object_ShipLoadTexture(PX_Instance *pIns,GAME_OBJECT_SHIP_TYPE type)
{
	switch(type)
	{
	case GAME_OBJECT_SHIP_TYPE_ALPHA:
		{
			return PX_ResourceLibraryGetTexture(&pIns->runtime.ResourceLibrary,GAME_TEXT_KEY_TEX_SHIP_ALPHA);
		}
		break;
	case GAME_OBJECT_SHIP_TYPE_GHOST:
		{

		}
		break;
	case GAME_OBJECT_SHIP_TYPE_ILLUSION:
		{

		}
		break;
	case GAME_OBJECT_SHIP_TYPE_PROPHET:
		{

		}
		break;
	case GAME_OBJECT_SHIP_TYPE_TITAN:
		{

		}
		break;
	case GAME_OBJECT_SHIP_TYPE_UFO:
		{

		}
		break;
	default:
		PX_ASSERT();
	}
	return PX_NULL;
}

px_void Game_Object_ShipSetTurret(GAME_OBJECT_SHIP_TYPE type,Game_Object_Plugin_Turret *pTurret)
{
	switch(type)
	{
	case GAME_OBJECT_SHIP_TYPE_ALPHA:
		{
			pTurret[0].offsetPoint=PX_POINT(30,0,0);
			pTurret[0].bActivate=PX_TRUE;
			pTurret[0].type=GAME_OBJECT_PLUGIN_TURRET_TYPE_CHARGER|GAME_OBJECT_PLUGIN_TURRET_TYPE_LAUNCHER;

			pTurret[1].offsetPoint=PX_POINT(2,-12,0);
			pTurret[1].bActivate=PX_TRUE;
			pTurret[1].type=GAME_OBJECT_PLUGIN_TURRET_TYPE_GUN|GAME_OBJECT_PLUGIN_TURRET_TYPE_MISSILE;

			pTurret[2].offsetPoint=PX_POINT(2,12,0);
			pTurret[2].bActivate=PX_TRUE;
			pTurret[2].type=GAME_OBJECT_PLUGIN_TURRET_TYPE_GUN|GAME_OBJECT_PLUGIN_TURRET_TYPE_MISSILE;
		}
		break;
	case GAME_OBJECT_SHIP_TYPE_GHOST:
		{

		}
		break;
	case GAME_OBJECT_SHIP_TYPE_ILLUSION:
		{

		}
		break;
	case GAME_OBJECT_SHIP_TYPE_PROPHET:
		{

		}
		break;
	case GAME_OBJECT_SHIP_TYPE_TITAN:
		{

		}
		break;
	case GAME_OBJECT_SHIP_TYPE_UFO:
		{

		}
		break;
	default:
		PX_ASSERT();
	}
}

PX_Object* Game_Object_ShipCreate(PX_Instance *pIns,PX_World *pWorld,Game_Object_ShipDescription desc)
{
	PX_Object *pObject;
	px_float x,y,width,height;
	px_texture *pTexture;
	Game_Object_Ship *pShip;
	PX_ParticalLauncher_InitializeInfo partical_info;

	x=(px_float)pWorld->world_width/2;
	y=(px_float)pWorld->world_height/2;

	pTexture=Game_Object_ShipLoadTexture(pIns,desc.type);
	
	
	width=GAME_SHIP_SIZE;
	height=GAME_SHIP_SIZE;
	if (!pTexture)
	{
		//server
	}


	pShip=(Game_Object_Ship *)MP_Malloc(&pIns->runtime.mp_game,sizeof(Game_Object_Ship));
	if (pShip==PX_NULL)
	{
		PX_ASSERT();
		return PX_NULL;
	}
	PX_memset(pShip,0,sizeof(Game_Object_Ship));

	pObject=(PX_Object *)PX_ObjectCreate(&pIns->runtime.mp_game,PX_NULL,0,0,0,0,0,0);

	if (!pObject)
	{
		PX_ASSERT();
		MP_Free(&pIns->runtime.mp_game,pShip);
		return PX_NULL;
	}


	partical_info.Create_func=Game_ParticalLauncher_ShipAssistPower_CreateAtom;
	partical_info.force=PX_POINT(0,0,0);
	partical_info.generateDuration=1000;
	partical_info.launchCount=-1;
	partical_info.maxCount=200;
	partical_info.resistanceK=0;
	partical_info.tex=PX_ResourceLibraryGetTexture(&pIns->runtime.ResourceLibrary,GAME_TEXT_KEY_PARTICAL_FIRE_LARGE);
	partical_info.Update_func=PX_NULL;
	partical_info.user=pShip;

	pShip->ship_powerpartical_main=PX_Object_ParticalCreateEx(&pIns->runtime.mp_game,pObject,0,0,0,partical_info);

	partical_info.Create_func=Game_ParticalLauncher_ShipAssistPower_CreateAtom;
	partical_info.force=PX_POINT(0,0,0);
	partical_info.generateDuration=1000;
	partical_info.launchCount=-1;
	partical_info.maxCount=100;
	partical_info.resistanceK=0;
	partical_info.tex=PX_ResourceLibraryGetTexture(&pIns->runtime.ResourceLibrary,GAME_TEXT_KEY_PARTICAL_FIRE_LARGE);
	partical_info.Update_func=PX_NULL;
	partical_info.user=pShip;
	pShip->ship_powerpartical_l1=PX_Object_ParticalCreateEx(&pIns->runtime.mp_game,pObject,0,0,0,partical_info);
	pShip->ship_powerpartical_l2=PX_Object_ParticalCreateEx(&pIns->runtime.mp_game,pObject,0,0,0,partical_info);

	pObject->x=x;
	pObject->y=y;
	pObject->z=GAME_SHIP_Z;
	pObject->Width=width;
	pObject->Height=height;

	pObject->pObject=pShip;
	pObject->Type=GAME_OBJECT_TYPE_SHIP;
	pObject->ReceiveEvents=PX_TRUE;
	pObject->Func_ObjectFree=Game_Object_ShipFree;
	pObject->Func_ObjectUpdate=Game_Object_ShipUpdate;
	pObject->Func_ObjectRender=Game_Object_ShipRender;
	pObject->diameter=48;
	pObject->User_ptr=PX_NULL;
	pObject->impact_Object_type=Game_Object_GroupImpactType(desc.group);
	pObject->impact_test_type=Game_Object_GroupImpactTestType(desc.group)|GAME_IMPACT_TYPE_NATURE;
	pShip->shield=0;
	pShip->max_shield=desc.maxshield;
	pShip->max_life=desc.maxlife;
	pShip->life=desc.maxlife;
	pShip->force=0;
	pShip->max_force=desc.maxforce;
	pShip->forceLimit=desc.maxforce*6;
	pShip->speedLimit=desc.maxspeed*2;
	pShip->max_speed=desc.maxspeed;
	pShip->shipObject=pTexture;
	pShip->velocity=PX_POINT(0,0,0);
	pShip->showHelpLine=PX_TRUE;
	pShip->controller.Cursor.x=x;
	pShip->controller.Cursor.y=y;
	pShip->controller.Cursor.z=0;
	pShip->controller.LeftButtonDown=PX_FALSE;
	pShip->controller.RightButtonDown=PX_FALSE;
	

	pShip->shieldtex=PX_ResourceLibraryGetTexture(&pIns->runtime.ResourceLibrary,GAME_TEXT_KEY_TEX_SHIELD);
	pShip->power_main_point=PX_POINT(-30,0,0);
	pShip->power_assist_L_point=PX_POINT(-28,-12,0);
	pShip->power_assist_R_point=PX_POINT(-28,12,0);

	PX_memset(pShip->Turret,0,sizeof(pShip->Turret));
	Game_Object_ShipSetTurret(desc.type,pShip->Turret);

	pShip->bLocalController=PX_FALSE;
	pShip->player=desc.player;
	pShip->group=desc.group;
	pShip->last_fire_elpased=0;
	pShip->base_weapon_generator_time=desc.base_weapon_generator_time;
	pShip->base_weapon_generator_elpased=0;
	pShip->last_fire_elpased=0;
	pShip->last_fire_recovery=desc.last_fire_recovery;
	pShip->last_shield_damage_elpased=0;
	pShip->slientTime=desc.slientTime;
	pShip->Armor=desc.Armor;
	pShip->playerLinkerObject=desc.playerLinkerObject;
	PX_memcpy(pShip->PlayerName,desc.PlayerName,sizeof(pShip->PlayerName));
	pShip->pWorld=pWorld;
	pShip->pIns=pIns;

	
	PX_ObjectRegisterEvent(pObject,GAME_OBJECT_EVENT_CHANGEVELOCITY,Game_Object_ShipOnChangeVelocity,PX_NULL);
	PX_ObjectRegisterEvent(pObject,GAME_OBJECT_EVENT_SWITCH,Game_Object_ShipOnSwitch,PX_NULL);
	PX_ObjectRegisterEvent(pObject,GAME_OBJECT_EVENT_CONTROLLER,Game_Object_ShipOnController,PX_NULL);
	PX_ObjectRegisterEvent(pObject,GAME_OBJECT_EVENT_DAMAGE,Game_Object_ShipOnDamage,PX_NULL);
	PX_ObjectRegisterEvent(pObject,GAME_OBJECT_EVENT_FIRE,Game_Object_ShipOnFire,PX_NULL);
	PX_ObjectRegisterEvent(pObject,PX_OBJECT_EVENT_IMPACT,Game_Object_ShipOnImpact,PX_NULL);
	PX_ObjectRegisterEvent(pObject,GAME_OBJECT_EVENT_ADDWEAPON,Game_Object_ShipOnAddWeapon,PX_NULL);
	PX_ObjectRegisterEvent(pObject,GAME_OBJECT_EVENT_ADDSHIELD,Game_Object_ShipOnAddShield,PX_NULL);
	PX_ObjectRegisterEvent(pObject,GAME_OBJECT_EVENT_HEALTH,Game_Object_ShipOnAddHealth,PX_NULL);
	PX_ObjectRegisterEvent(pObject,GAME_OBJECT_EVENT_SPEEDRUP,Game_Object_ShipOnSpeedUp,PX_NULL);
	PX_ObjectRegisterEvent(pObject,GAME_OBJECT_EVENT_POWERUP,Game_Object_ShipOnPowerUp,PX_NULL);
	PX_ObjectRegisterEvent(pObject,GAME_OBJECT_EVENT_SETNAME,Game_Object_ShipOnName,PX_NULL);

	Game_Object_ShipAddWeapon(pObject,GAME_OBJECT_SHIP_WEAPON_TYPE_MACHINEGUN,0,GAME_OBJECT_SHIP_WEAPON_MACHINEGUN_DURATION,GAME_OBJECT_SHIP_WEAPON_MACHINEGUN_MAX_CARGO,GAME_OBJECT_PLUGIN_TURRET_TYPE_GUN);
	PX_WorldAddObject(pWorld,pObject);

	return pObject;
}
