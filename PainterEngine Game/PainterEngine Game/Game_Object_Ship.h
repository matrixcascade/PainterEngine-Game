#ifndef GAME_OBJECT_SHIP
#define GAME_OBJECT_SHIP


#include "Game_Object_Base.h"

#include "Game_Object_Shotgun.h"
#include "Game_Object_LaunchFire.h"
#include "Game_Object_Railgun.h"
#include "Game_Object_RailgunFire.h"
#include "Game_Object_Missile.h"
#include "Game_Object_Nuclear.h"
#include "Game_Object_Mine.h"
#include "Game_Object_Blackhole.h"
#include "Game_Object_ProtectShield.h"
#include "Game_Object_Piercer.h"

#define GAME_SHIP_FORCE_MAX_DISTANCE 300
#define GAME_SHIP_SIZE 64
#define GAME_SHIP_MAX_TURRUT 8
#define GAME_SHIP_NAME_MAX_LEN 32
#define GAME_SHIP_IMPACT_MIN_VELOCITY 20
#define GAME_SHIP_NAME_DEFEAT_SCORE 8000


typedef enum
{
	GAME_OBJECT_SHIP_TYPE_ALPHA,
	GAME_OBJECT_SHIP_TYPE_PROPHET,
	GAME_OBJECT_SHIP_TYPE_UFO,
	GAME_OBJECT_SHIP_TYPE_ILLUSION,
	GAME_OBJECT_SHIP_TYPE_GHOST,
	GAME_OBJECT_SHIP_TYPE_TITAN,
}GAME_OBJECT_SHIP_TYPE;

typedef struct
{
	GAME_OBJECT_SHIP_WEAPON_TYPE type;
	px_dword turretType;
	px_int AtomCount;
	px_int MaxAtomCount;
	px_dword fire_duration;
	px_dword fire_elpased;
}Game_Weapon_Info;




typedef struct
{
	//Player ID
	px_int player;
	px_int group;

	//Controller
	PX_Controller controller;
	px_bool  bLocalController;

	//Interface
	PX_Instance *pIns;
	PX_World *pWorld;
	PX_Object *playerLinkerObject;

	//Base Info
	GAME_OBJECT_SHIP_TYPE type;

	px_float life;	
	px_float shield;

	px_float max_force;
	px_float max_speed;
	px_float max_life;
	px_float max_shield;
	px_float forceLimit;
	px_float speedLimit;
	px_float Armor;

	px_point direction;
	px_point velocity;
	px_float force;

	px_dword base_weapon_generator_time;
	px_dword base_weapon_generator_elpased;

	px_dword last_fire_recovery;
	px_dword last_fire_elpased;

	px_dword last_shield_damage_elpased;
	px_dword slientTime;

	px_point power_main_point,power_assist_L_point,power_assist_R_point;

	Game_Object_Plugin_Turret Turret[GAME_SHIP_MAX_TURRUT];
	px_point cannon_point0,cannon_point1,cannon_point2;

	px_int WeaponIndex;
	Game_Weapon_Info WeaponStack[8];

	//Render Data
	PX_Object *ship_powerpartical_main,*ship_powerpartical_l1,*ship_powerpartical_l2;
	px_word PlayerName[GAME_SHIP_NAME_MAX_LEN];
	px_bool showHelpLine;
	px_texture photo,photo_mini;
	px_texture *shipObject;
	px_texture *shieldtex;
}Game_Object_Ship;

typedef struct  
{
	px_int player;
	px_int group;
	px_float maxshield;
	px_float maxlife;
	px_float maxforce;
	px_float maxspeed;
	px_float Armor;
	px_dword base_weapon_generator_time;
	px_dword last_fire_recovery;
	px_dword slientTime;
	PX_Object *playerLinkerObject;

	GAME_OBJECT_SHIP_TYPE type;
	px_word PlayerName[GAME_SHIP_NAME_MAX_LEN];
}Game_Object_ShipDescription;

Game_Object_Ship *Game_Object_GetShip(PX_Object *pObject);
PX_Object* Game_Object_ShipCreate(PX_Instance *pIns,PX_World *pWorld,Game_Object_ShipDescription desc);


#endif