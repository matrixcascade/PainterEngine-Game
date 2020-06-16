#ifndef PX_LOBBY_H
#define PX_LOBBY_H
#include "PainterEngine_Startup.h"
#include "../../PainterEngine Game/PainterEngine Game Client/PainterEngine_GameClientStartup.h"
#include "../../PainterEngine Game/PainterEngine Game/Game_Object_Base.h"

#define PX_LOBBY_PANELUSERINFO_X 0
#define PX_LOBBY_PANELUSERINFO_Y 0
#define PX_LOBBY_PANELUSERINFO_HEIGHT (80)

#define PX_LOBBY_HEARTBEAT_TIME 2000
#define PX_LOBBY_CONNECTION_TIME 16000
#define PX_LOBBY_UPDATE_USERINFO_TIME 3000
#define PX_LOBBY_UPDATE_USERSTATE_TIME 1000
#define PX_LOBBY_UPDATE_PHOTO_TIME 3000
#define PX_LOBBY_UPDATE_NEWS_TIME 10000
typedef struct  
{
	PX_Object   *root;
	px_shape	shape_coin,shape_ticket;
	PX_Object	*coin,*ticket,*uploadPhoto,*btn_buyTicket;
}PX_Lobby_PanelUserInfo;


typedef struct
{
	PX_Instance *pIns;
	//authority
	px_byte aes_code[32];
	px_byte cookie[32];
	px_int  map_id;
	px_int  id;
	PX_UDP udp;
	PX_UDP_ADDR serverAddr;
	PX_UDP_ADDR serverLargeDataAddr;
	//ui
	px_float state_angle;
	px_float photo_loading_angle;
	PX_Lobby_PanelUserInfo panel_userinfo;
	px_int displayRankPt;
	PX_Object *ui_oproot;
	PX_Object *btn_matchRanking8p,*btn_matchRanking6p,*btn_matchRanking4p,*btn_match2p,*btn_match1p,*btn_matchCancel,*btn_EnterGame,*autoText_Content;
	PX_Object *btn_upgrade_ammorec,*btn_upgrade_ammogen,*btn_upgrade_force,*btn_upgrade_life,*btn_upgrade_shield,*btn_upgrade_speed;
	//infos
	px_int photo_block;
	px_byte photo_cache[PX_STARTUP_USERDATA_PHOTOSIZE];
	px_char news[PX_LOBBY_NEWS_SIZE];
	px_texture photo;
	PX_Startup_UserData userInfo;
	PX_Startup_UserState userState;
	//timer
	px_dword Operator_elpased;
	px_dword hb_elpased;
	px_dword connection_elpased;
	px_dword userinfo_elpased;
	px_dword userstate_elpased;
	px_dword photo_elpased;
	px_dword news_elpased;

	//Game
	px_char serverIpAddr[24];
	HANDLE gameHandle;

}PX_Lobby;


px_bool PX_LobbyInitialize(PX_Instance *pIns,PX_Lobby *pDesc,const px_char LobbyServerIpAddr[],px_ushort port,px_ushort largedataport);
px_void PX_LobbySetUserContent(PX_Lobby *pl,px_byte cookie[32],px_byte aes_code[32],px_int map_id,px_int id);
px_void PX_LobbyPostEvent(PX_Lobby *pl,PX_Object_Event e);
px_void PX_LobbyUpdate(PX_Lobby *pl,px_dword elpased);
px_void PX_LobbyRender(PX_Lobby *pl,px_dword elpased);
px_void PX_LobbyReset(PX_Lobby *pl);
px_void PX_LobbyDisconnect(PX_Lobby *pl);
#endif