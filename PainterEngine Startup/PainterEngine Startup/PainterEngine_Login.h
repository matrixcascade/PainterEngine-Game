#ifndef PX_LOGIN_H
#define PX_LOGIN_H

#include "PainterEngine_Startup.h"
#include "../../PainterEngine Game/PainterEngine Game/Game_Object_Base.h"

#define PX_LOGIN_EXCHAGE_TIME 300
#define PX_LOGIN_EXCHAGE_TIMES 10
#define PX_LOGIN_LOGIN_TIME 500
#define PX_LOGIN_LOGIN_TIMES 10

#define PX_LOGIN_CONNECTION_TIMEOUT 3000
#define PX_LOGIN_FAILED_TIMEOUT 3000

#define PX_LOGIN_TEXT_PATH_LOGO "./resources/login.traw"

#define PX_LOGIN_TEXT_SIGNUP "注册账户"
#define PX_LOGIN_TEXT_EXIT "退出游戏"
#define PX_LOGIN_TEXT_LOGIN "登录"
#define PX_LOGIN_TEXT_LOGINING_LOGIN_TEXT ((px_char *)L"以安全链接登录中")
#define PX_LOGIN_TEXT_LOGINING_EXCHANGE_TEXT ((px_char *)L"正在建立安全链接")
#define PX_LOGIN_TEXT_CONNECTION_FAILED ((px_char *)L"连接失败")

#define PX_LOGIN_TEXT_INVALID_ACCOUNT ((px_char *)L"无效的账户名")
#define PX_LOGIN_TEXT_INVALID_PASSWORD ((px_char *)L"无效的密码")
#define PX_LOGIN_TEXT_LOGINING ((px_char *)L"登陆中")
typedef enum
{
	PX_LOGIN_STATUS_STANDBY,
	PX_LOGIN_STATUS_LOGINING_EXCHAGE,
	PX_LOGIN_STATUS_LOGINING_LOGIN,
	PX_LOGIN_STATUS_LOGINING_DOWNLOAD,
	PX_LOGIN_STATUS_SUCCEEDED,
	PX_LOGIN_STATUS_FAILED,
}PX_LOGIN_STATUS;


typedef struct  
{
	PX_Instance *pIns;
	px_char userName[32];
	px_char password[32];
	px_byte privatekey[32];
	px_byte publickey[32];
	px_byte sharekey[32];
	px_byte get_cookie[32];
	px_dword id;
	px_dword mapid;
	px_dword v_code;
	px_word loginErrorMessage[64];
	px_char *loginText;
	px_texture LoginTexture;
	PX_LOGIN_STATUS status;
	PX_Object *login_root,*logining_root,*edit_user,*edit_pwd,*btn_login,*btn_exit,*btn_copyrights,*btn_signup,*label_loginstatus;
	PX_UDP udp;
	PX_UDP_ADDR serverAddr;
	px_dword elpased;
	px_int times;
}PX_Login;

px_bool PX_LoginInitialize(PX_Instance *pIns,PX_Login *pl,const px_char LoginServerIpAddr[],px_ushort port);
px_void PX_LoginPostEvent(PX_Login *pl,PX_Object_Event e);
px_void PX_LoginUpdate(PX_Login *pl,px_dword elpased);
px_void PX_LoginRender(PX_Login *pl,px_dword elpased);
px_void PX_LoginReset(PX_Login *pl);
#endif
