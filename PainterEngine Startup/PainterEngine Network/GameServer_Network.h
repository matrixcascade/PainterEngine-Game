#ifndef PAINTERENGINE_STARTUP_NETWORK_H
#define PAINTERENGINE_STARTUP_NETWORK_H

#define PX_STARTUP_USERDATA_PHOTOSIZE (64*64*4+sizeof(PX_TRaw_Header))


#define PX_STARTUP_ACCOUNT_MINLEN 6
#define PX_STARTUP_ACCOUNT_MAXLEN 16

#define PX_STARTUP_PASSWORD_MINLEN 6
#define PX_STARTUP_PASSWORD_MAXLEN 16

#define PX_STARTUP_NICKNAME_MINLEN 2
#define PX_STARTUP_NICKNAME_MAXLEN 16


typedef enum
{
	PX_STARTUP_GAMESTATE_STANDBY,
	PX_STARTUP_GAMESTATE_RANKING,
	PX_STARTUP_GAMESTATE_GAMING,
}PX_STARTUP_GAMESTATE;

typedef enum
{
	PX_STARTUP_SHIP_TYPE_ALPHA =0,
	PX_STARTUP_SHIP_TYPE_PROPHET,
	PX_STARTUP_SHIP_TYPE_UFO,
	PX_STARTUP_SHIP_TYPE_ILLUSION,
	PX_STARTUP_SHIP_TYPE_GHOST,
	PX_STARTUP_SHIP_TYPE_TITAN,
}PX_STARTUP_SHIP_TYPE;

typedef struct
{
	px_uchar unlock;
	px_uchar type;
	px_uchar upgrade_shield;
	px_uchar upgrade_life;
	px_uchar upgrade_force;
	px_uchar upgrade_speed;
	px_uchar upgrade_lastfirerecovery;
	px_uchar upgrade_ammo_generator;
}PX_Startup_ShipInfo;

typedef struct
{
	px_char account[32];
	px_word nickname[32];
	px_dword rank_pt;
	px_dword rank;
	px_dword match_count;
	px_dword ticket;
	px_dword coin;
	PX_Startup_ShipInfo ShipStack[16];
}PX_Startup_UserData;

typedef struct
{
	px_dword  userState;
	px_dword  serverid;
	px_dword  clientid;
	px_ushort game_port;
	px_ushort data_port;
	px_dword  game_readyPlayers;
	px_dword  game_onlineCount;
	px_dword  shipIndex;
	px_uchar  RoomPlayerCount[16];
}PX_Startup_UserState;


//////////////////////////////////////////////////////////////////////////
//netpackets
#define PX_NETWORK_PACKET_MAX_SIZE 1420



//sign up
#define PX_NETWORKSIGNUP_MAGIC 0x31415926


#define PX_NETWORKSIGNUP_SIGNACK_SUCCEED 0
#define PX_NETWORKSIGNUP_SIGNACK_INVALID_REGCODE 1
#define PX_NETWORKSIGNUP_SIGNACK_INVALID_ACCOUNT 2
#define PX_NETWORKSIGNUP_SIGNACK_REPEAT_ACCOUNT 3
#define PX_NETWORKSIGNUP_SIGNACK_INVALID_NICKNAME 4
#define PX_NETWORKSIGNUP_SIGNACK_INVALID_PASSWORD 5
#define PX_NETWORKSIGNUP_SIGNACK_REPEAT_IP 6


#define PX_NETWORKSIGNUP_OPCODE_CONNECT 0
#define PX_NETWORKSIGNUP_OPCODE_ACK 1
#define PX_NETWORKSIGNUP_OPCODE_DATA 2
#define PX_NETWORKSIGNUP_OPCODE_SIGN 3
#define PX_NETWORKSIGNUP_OPCODE_SIGNACK 4

typedef struct
{
	px_dword magic_numeric;
	px_dword opcode;
}PX_SignUp_Packet_Header;

typedef struct
{
	px_dword magic_numeric;
	px_dword opcode;
	px_byte  publickey[32];
}PX_SignUp_Packet_Connect;


typedef struct
{
	px_dword magic_numeric;
	px_dword opcode;
	px_byte  publickey[32];
}PX_SignUp_Packet_ConnectAck;


typedef struct
{
	px_dword magic_numeric;
	px_dword opcode;
	px_byte  userName[32];
	px_word  nickName[32];
	px_byte  password[32];
	px_byte  regcode[32];
}PX_Signup_Packet_Sign;

typedef struct
{
	px_dword magic_numeric;
	px_dword opcode;
	px_dword result;
}PX_SignUp_Packet_SignAck;

//login
#define PX_NETWORKLOGIN_MAGIC 0x31415926

#define PX_NETWORKLOGIN_OPCODE_CONNECT 0
#define PX_NETWORKLOGIN_OPCODE_ACK 1
#define PX_NETWORKLOGIN_OPCODE_DATA 2
#define PX_NETWORKLOGIN_OPCODE_LOGIN 3
#define PX_NETWORKLOGIN_OPCODE_LOGINACK 4


#define PX_NETWORKLOGIN_OPCODE_LOGINRESULT_SUCCEEDED 0
#define PX_NETWORKLOGIN_OPCODE_LOGINRESULT_FAILED    1

typedef struct
{
	px_dword magic_numeric;
	px_dword opcode;
}PX_Login_Packet_Header;

typedef struct
{
	px_dword magic_numeric;
	px_dword opcode;
	px_byte  publickey[32];
}PX_Login_Packet_Connect;

typedef struct
{
	px_dword magic_numeric;
	px_dword opcode;
	px_byte  publickey[32];
	px_dword v_code;
}PX_Login_Packet_ConnectAck;

typedef struct
{
	px_dword magic_numeric;
	px_dword opcode;
	px_dword version;
	px_byte  userName[32];
	px_byte  password[32];
	px_dword v_code;
}PX_Login_Packet_login;

typedef struct
{
	px_dword magic_numeric;
	px_dword opcode;
	px_dword result;
	union
	{
		struct  
		{
			px_byte  cookie[32];
			px_dword id;
			px_dword map_index;
		};
		px_word Message[64];
	};
	
}PX_Login_Packet_loginAck;

//lobby

//heart beat
#define PX_NETWORK_LOBBY_MAGIC 1990012

#define PX_NETWORK_LOBBY_TYPE_HEARTBEAT 0
#define PX_NETWORK_LOBBY_TYPE_HEARTBEATACK 1
#define PX_NETWORK_LOBBY_TYPE_QUERYUSERINFO 2
#define PX_NETWORK_LOBBY_TYPE_QUERYUSERINFOACK 3
#define PX_NETWORK_LOBBY_TYPE_QUERYPHOTO 4
#define PX_NETWORK_LOBBY_TYPE_QUERYPHOTOACK 5
#define PX_NETWORK_LOBBY_TYPE_QUERYSTATE 6
#define PX_NETWORK_LOBBY_TYPE_QUERYSTATEACK 7
#define PX_NETWORK_LOBBY_TYPE_STARTGAME 8
#define PX_NETWORK_LOBBY_TYPE_CANCELMATCH  9
#define PX_NETWORK_LOBBY_TYPE_UPGRADE_AMMOGEN 10
#define PX_NETWORK_LOBBY_TYPE_UPGRADE_AMMOREC 11
#define PX_NETWORK_LOBBY_TYPE_UPGRADE_MAXFORCE 12
#define PX_NETWORK_LOBBY_TYPE_UPGRADE_MAXLIFE 13
#define PX_NETWORK_LOBBY_TYPE_UPGRADE_MAXSHIELD 14
#define PX_NETWORK_LOBBY_TYPE_UPGRADE_MAXSPEED 15
#define PX_NETWORK_LOBBY_TYPE_UPLOADPHOTO 16
#define PX_NETWORK_LOBBY_TYPE_QUERYNEWS 17
#define PX_NETWORK_LOBBY_TYPE_QUERYNEWSACK 18
#define PX_NETWORK_LOBBY_TYPE_BUY_TICKET 19
typedef struct  
{
	px_dword magic;
	px_dword type;
	union
	{
		px_dword id;
		px_dword param;
	};
	
	union
	{
		px_dword size;
		px_dword mapid;
	};
}PX_Lobby_Packet_Header;

typedef struct  
{
	PX_Lobby_Packet_Header header;
	struct  
	{
		px_byte cookie[32];
		union
		{
			struct  
			{
				px_dword param[8];
				px_byte  param_byte[32];
			};
			px_byte data[32];
		};
	}content;
}PX_Lobby_Packet_Command;

typedef struct  
{
	px_dword magic;
	px_dword type;
	px_dword id;
	px_dword mapid;
	px_byte cookie[32];
}PX_Lobby_Packet_LargeDataHeader;

#define PX_LOBBY_NEWS_SIZE 256

#endif

