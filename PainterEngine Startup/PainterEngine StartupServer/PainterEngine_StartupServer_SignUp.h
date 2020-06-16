#ifndef PAINTERENGINE_STARTUPSERVER_SIGNUP_H
#define PAINTERENGINE_STARTUPSERVER_SIGNUP_H

#include "PainterEngine_Startup.h"
#include "PainterEngine_StartupServer_Database.h"
#include "../PainterEngine Network/GameServer_Network.h"

#define PX_STARTUPSERVER_SIGNUP_MAXCONNECTION 256
#define PX_STARTUPSERVER_SIGNUP_CONNECTION_TIMEOUT 3000
#define PX_STARTUPSERVER_SIGNUP_LISTEN_PORT 31416


#define PX_STARTUP_SIGNUP_IP_STACK 128
#define PX_STARTUP_SIGNUP_IP_TIMEOUT 600000
#define PX_STARTUP_SIGNUP_IP_LEVEL 9

#define PX_STARTUPSERVER_SIGNUP_TEXT_INITIALIZE_SIGNUP_UDP_FAILED "初始化注册服务器UDP初始化失败."
#define PX_STARTUPSERVER_SIGNUP_TEXT_LISTEN_SIGNUP_UDP_FAILED "监听注册服务端口失败."

typedef struct
{
	px_bool activate;
	PX_UDP_ADDR Addr;
	px_byte  privatekey[32];
	px_byte  publickey[32];
	px_byte  theirkey[32];
	px_byte  sharekey[32];
	px_dword elpased;
}PX_StartupServer_SignUpConnection;

typedef struct
{
	px_char  regCode[32];
	px_char  sign_admin[16];
	px_dword active_count;
	px_dword authority_level;
}PX_StartupServer_SignUp_RegCode;

typedef struct  
{
	px_bool activated;
	px_dword ipv4;
	px_dword elpased;
}PX_StartupServer_SignUp_IP_Delay;



typedef struct  
{
	PX_StartupServer_SignUpConnection *connections;
	px_vector regcode;
	px_int max_connections;
	px_memorypool mp;
	PX_UDP udp;
	PX_StartupServer_SignUp_IP_Delay regIp[PX_STARTUP_SIGNUP_IP_STACK];
	PX_StartupServer_Database *link_database;
}PX_StartupServer_SignUp;

px_bool PX_StartupServer_SignUpInitialize(PX_StartupServer_SignUp *pSp,PX_StartupServer_Database *link_database,px_int max_connections);
px_void PX_StartupServer_SignUpUpdate(PX_StartupServer_SignUp *pSp,px_dword elpased);
px_void PX_StartupServer_SignUpFree(PX_StartupServer_SignUp *pSp);
px_bool PX_StartupServer_SignAddRegCode(PX_StartupServer_SignUp *pSp,px_char regCode[32],px_char signAdmin[16],px_dword activeCount,px_dword authority_level);

#endif
