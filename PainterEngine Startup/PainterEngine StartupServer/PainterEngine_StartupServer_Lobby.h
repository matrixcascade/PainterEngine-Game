#ifndef DEMO_SERVERLOBBY_H
#define DEMO_SERVERLOBBY_H

#include "PainterEngine_Startup.h"
#include "../PainterEngine Network/GameServer_Network.h"
#include "PainterEngine_StartupServer_Database.h"
#include "../../PainterEngine Game/PainterEngine Game Server/PainterEngine_GameServerStartup.h"
#include "../../PainterEngine Game/PainterEngine Game/Game_Object_Base.h"

#define PX_STARTUPSERVER_LOBBY_MAXCLIENT 128
#define PX_STARTUPSERVER_LOBBY_CLIENT_TIMEOUT 16000
#define PX_STARTUPSERVER_LOBBY_LISTEN_PORT 31417
#define PX_STARTUPSERVER_LOBBY_LARGEDATA_LISTEN_PORT 31418

#define PX_STARTUP_ROOM_MAX_SUPPORT_PLAYER 16
#define PX_STARTUPSERVER_LOBBY_MAX_SUPPORTINSTANCE 64
#define PX_STARTUPSERVER_LOBBY_MAX_SUPPORTROOM 64
#define PX_STARTUPSERVER_LOBBY_STARTPORT 2048
#define PX_STARTUPSERVER_LOBBY_ENDPORT 8192

#define PX_STARTUPSERVER_LOBBY_TEXT_INITIALIZE_LOBBY_UDP_FAILED "初始化大厅服务器UDP初始化失败."
#define PX_STARTUPSERVER_LOBBY_TEXT_LISTEN_LOBBY_UDP_FAILED "监听大厅服务端口失败."

#define PX_STARTUPSERVER_LOBBY_TEXT_INITIALIZE_LOBBY_DATAUDP_FAILED "初始化大厅服务器数据UDP初始化失败."
#define PX_STARTUPSERVER_LOBBY_TEXT_LISTEN_LOBBY_DATAUDP_FAILED "监听大厅服务数据端口失败."

#define PX_STARTUPSERVER_LOBBY_TEXT_INITIALIZE_LOBBY_LARGEDATAUDP_FAILED "初始化用户数据服务器数据UDP初始化失败."
#define PX_STARTUPSERVER_LOBBY_TEXT_LISTEN_LOBBY__LARGEDATAUDP_FAILED "监听用户数据服务数据端口失败."
typedef struct
{
	px_bool activated;
	px_int mapid;
	px_int id;
}PX_StartupServer_Lobby_GameInstance_Player;




typedef struct
{
	volatile px_bool activated;
	volatile HANDLE hGameProcess;
	px_dword gameport;
	px_dword dataport;
	GAME_PLAY_GAMETYPE type;
	px_dword serverIDArrary[PX_STARTUP_ROOM_MAX_SUPPORT_PLAYER];
	px_dword clientIDArrary[PX_STARTUP_ROOM_MAX_SUPPORT_PLAYER];
	PX_StartupServer_Lobby_GameInstance_Player PlayerStack[PX_STARTUP_ROOM_MAX_SUPPORT_PLAYER];
}PX_StartupServer_Lobby_GameInstance;

typedef struct
{
	px_bool activated;
	px_dword roomKey;
	GAME_PLAY_GAMETYPE type;
	PX_StartupServer_Lobby_GameInstance_Player PlayerStack[PX_STARTUP_ROOM_MAX_SUPPORT_PLAYER];
}PX_StartupServer_Lobby_GameRoom;

typedef struct
{
	px_bool activated;


	//authority
	px_dword id;
	px_char account[32];
	px_uchar pwd_sha256[SHA256_HASH_SIZE];
	px_byte cookie[32];
	PX_AES aes_encoder;

	//timer
	px_byte aes_code[32];
	px_dword hb_elpased;
	px_dword lastop_elpased;
	px_dword photo_elpased;

	//data
	PX_Startup_UserData userdata;
	px_byte photoData[PX_STARTUP_USERDATA_PHOTOSIZE];

	//Game Instance
	PX_Startup_UserState userState;
	px_int GameInstanceIndex;

}PX_StartupServer_LobbyClient;


typedef struct  
{
	px_memorypool mp;
	px_int allocID;

	PX_StartupServer_LobbyClient clients[PX_STARTUPSERVER_LOBBY_MAXCLIENT];
	PX_StartupServer_Database *pDataBase;
	PX_UDP udp;
	PX_UDP udp_gameresult;
	PX_UDP udp_largeData;
	px_dword OnlineCount;
	px_uchar roomPlayerCount[16];
 	px_char News[PX_LOBBY_NEWS_SIZE];
	PX_StartupServer_Lobby_GameRoom GameRoom[PX_STARTUPSERVER_LOBBY_MAX_SUPPORTROOM];
	PX_StartupServer_Lobby_GameInstance GameInstance[PX_STARTUPSERVER_LOBBY_MAX_SUPPORTINSTANCE];

	//config
	PX_Json json_config;
}PX_StartupServer_Lobby;


px_bool PX_StartupServer_LobbyInitialize(PX_StartupServer_Lobby *pDesc,PX_StartupServer_Database *pDataBase,px_int max_connections);
px_bool PX_StartupServer_LobbyLogin(PX_StartupServer_Lobby *pDesc,px_byte aes_code[32],px_char account[32],px_char pwd[32],_OUT px_byte cookie[32],_OUT px_dword *map_index,_OUT px_dword *id);
px_bool PX_StartupServer_LobbyUpdateClientUserInfoByAccount(PX_StartupServer_Lobby *pDesc,px_char account[]);
px_void PX_StartupServer_LobbyUpdate(PX_StartupServer_Lobby *pDesc,px_dword elpased);
px_void PX_StartupServer_LobbyFree(PX_StartupServer_Lobby *pDesc);

#endif
