#ifndef PAINTERENGINE_SIGNUP_H
#define PAINTERENGINE_SIGNUP_H

#include "PainterEngine_Startup.h"

#define PX_SIGNUP_EXCHAGE_TIME 300
#define PX_SIGNUP_EXCHAGE_TIMES 10
#define PX_SIGNUP_REG_TIME 500
#define PX_SIGNUP_REG_TIMES 10

#define PX_SIGNUP_CONNECTION_TIMEOUT 3000
#define PX_SIGNUP_FAILED_TIMEOUT 3000

#define PX_SIGNUP_TEXT_ACCOUNT_COULD_NOT_EMPTY "账户不能为空"
#define PX_SIGNUP_TEXT_NICKNAME_COULD_NOT_EMPTY "昵称不能为空"
#define PX_SIGNUP_TEXT_REGCODE_COULD_NOT_EMPTY "注册码不能为空"
#define PX_SIGNUP_TEXT_PASSWORD_TOO_SHORT "密码长度太短"
#define PX_SIGNUP_TEXT_PASSWORD_NOT_MATCH "两次密码不一致"
#define PX_SIGNUP_TEXT_SIGNUP "注册"
#define PX_SIGNUP_TEXT_EXIT "退出"

#define PX_SIGNUP_TEXT_INVALID_ACCOUNT "无效的账户信息"
#define PX_SIGNUP_TEXT_REPEAT_ACCOUNT "账户名重复"
#define PX_SIGNUP_TEXT_INVALID_NICKNAME "无效的昵称"
#define PX_SIGNUP_TEXT_INVALID_PASSWORD "无效的密码"
#define PX_SIGNUP_TEXT_INVALID_SIGNUP_CODE "无效的注册码"
#define PX_SIGNUP_TEXT_INVALID_REPEAT_IP "注册码实效错误"
#define PX_SIGNUP_TEXT_CONNECTION_FAILED ((px_char *)L"网络连接失败")
#define PX_SIGNUP_TEXT_REGISTRY_SUCCEEDED ((px_char *)L"注册成功")
#define PX_SIGNUP_TEXT_LOGIN "登录"


typedef enum
{
	PX_SIGNUP_STATUS_STANDBY,
	PX_SIGNUP_STATUS_REGISTING_EXCHAGE,
	PX_SIGNUP_STATUS_REGISTING_REG,
	PX_SIGNUP_STATUS_REGISTING_DOWNLOAD,
	PX_SIGNUP_STATUS_SUCCEEDED,
	PX_SIGNUP_STATUS_FAILED,
}PX_SIGNUP_STATUS;



typedef struct  
{
	PX_Instance *pIns;
	px_char userName[32];
	px_word nickName[32];
	px_char password[32];
	px_char regcode[32];
	px_byte privatekey[32];
	px_byte publickey[32];
	px_byte sharekey[32];
	px_char *loginText;
	px_texture SignUpTexture,SigningTexture;
	PX_SIGNUP_STATUS status;
	PX_Object *signup_root;
	PX_Object *edit_user,*edit_nickname,*edit_pwd,*edit_pwdrepeat,*edit_regcode,*btn_login;
	PX_Object *btn_reg,*btn_exit;
	PX_Object *label_accountInfo,*label_nickInfo,*label_pwdInfo,*label_regcodeInfo;
	PX_Object *radius_signing;
	PX_UDP udp;
	PX_UDP_ADDR ServerAddr;
	px_dword elpased;
	px_int times;
}PX_SignUp;


px_bool PX_SignUpInitialize(PX_Instance *pIns,PX_SignUp *pR,const px_char SignupServerIpAddr[],px_ushort port);
px_void PX_SignUpPostEvent(PX_SignUp *pl,PX_Object_Event e);
px_void PX_SignUpUpdate(PX_SignUp *pl,px_dword elpased);
px_void PX_SignUpRender(PX_SignUp *pl,px_dword elpased);
#endif
