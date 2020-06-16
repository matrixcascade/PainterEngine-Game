#ifndef GAME_PLAY_H
#define GAME_PLAY_H

#define GAME_PLAY_MAX_SUPPORT_PLAYERS 16

#include "PainterEngine_Startup.h"
#include "Game_ObjectTypes.h"
#include "Game_ResoucesLoader.h"
#include "Game_Text.h"

#define GAME_PARTICAL_MP_FIRE 1024*1024 //1M for fire
#define GAME_PARTICAL_MP_EXPLODE 1024*1024 //1M for partical
#define GAME_PARTICAL_MP_CALCULATE 1024*1024 //1M for calculate

#define GAME_WORLD_WIDTH 3200 //world description
#define GAME_WORLD_HEIGHT 2400 //world description

#define GAME_UPDATE_TIME 20
#define GAME_SYNC_DURATION 100
#define GAME_SYNC_DELAY 100
#define GAME_SYNC_RECONNECT_XSPEED 20
#define GAME_PLAY_INSTR_OPCODE_MOVE    0
#define GAME_PLAY_INSTR_OPCODE_SWITCH  1
#define GAME_PLAY_INSTR_OPCODE_NAME    2

#define GAME_PLAY_DEFAULT_TIME  360000

#define GAME_PLAY_STONE_GEN_DURATION 500
#define GAME_PLAY_ALIEN_GEN_DURATION 5000
#define GAME_PLAY_TREASURE_GEN_DURATION 45000
#define GAME_PLAY_TREASURE_GEN_COUNT 3
#define GAME_PLAY_GAME_WAIT_ELPASED 1600

typedef struct  
{
	px_short opcode;
	px_short param0;
	px_short param1;
	union
	{
		struct
		{
			px_short lButtonDown:4;
			px_short rButtonDown:4;
		};
		px_short param2;
	};
}Game_Play_Instr;

typedef struct  
{
	px_short opcode;
	px_word Name[16];
}Game_Play_NameInstr;


typedef struct  
{
	px_word Message[GAME_MESSAGE_LEN];
}Game_Play_Message;


typedef struct
{
	PX_Instance *pIns;
	PX_World world;
	px_int PlayerCount;
	px_dword elpased;
	px_dword GameTime;
	px_dword stoneGenElpased;
	px_dword AlienGenElpased;
	px_dword treasureGenElpased;
	px_dword GameOverWaitElpased;
	GAME_PLAY_GAMETYPE GameType;
	PX_Object *PlayerLinker;
	px_vector Messages;//Game_Play_Message
	Game_Play_Player Players[GAME_PLAY_MAX_SUPPORT_PLAYERS];
}Game_Play;


px_bool Game_PlayInitialize(PX_Instance *Ins,Game_Play *pPlay);
px_void Game_PlayReset(Game_Play *pPlay);
px_void Game_PlaySetGameTime(Game_Play *pPlay,px_dword gameTime);
px_void Game_PlayStart(Game_Play *pPlay,GAME_PLAY_GAMETYPE gameType,PX_GameStartUp_PlayerDataDesc PlayerDataDesc[],px_int playerCount);
px_void Game_PlayUpdate(Game_Play *pPlay,px_dword elpased);
px_void Game_PlayRender(px_surface *psurface,Game_Play *pPlay,px_dword elpased);
px_void Game_PlayPostEvent(Game_Play *pPlay,PX_Object_Event e);
px_int  Game_PlayGetPlayerRank(Game_Play *pPlay,px_int Player);
px_int  Game_PlayIsPlayerAlive(Game_Play *pPlay,px_int Player);
px_bool Game_PlayIsGameOver(Game_Play *pPlay);
#endif