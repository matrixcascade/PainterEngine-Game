#ifndef GAMESERVER_DATABASE_H
#define GAMESERVER_DATABASE_H

#include "../../../PainterEngine/kernel/PX_Kernel.h"
#include "../PainterEngine Network/GameServer_Network.h"
#include "PainterEngine_Startup.h"
#include "stdio.h"

#define GAMESERVER_TEXT_DATABASE_FILE_DATABASE "./database"
#define GAMESERVER_TEXT_DATABASE_FILE_PASSWORD "password"
#define GAMESERVER_TEXT_DATABASE_FILE_USERINFO "userinfo"
#define GAMESERVER_TEXT_DATABASE_FILE_USERINFO_BAK "userinfo_backup"
#define GAMESERVER_TEXT_DATABASE_FILE_PROFILE_PHOTO "photo"
#define GAMESERVER_TEXT_DATABASE_FILE_USERGAME_DATA "usergamedata"
#define GAMESERVER_TEXT_DATABASE_FILE_REGCODE "regcode"
#define GAMESERVER_TEXT_DATABASE_FILE_LOG "log"
#define GAMESERVER_TEXT_DATABASE_FILE_AUTHORITY "authority"
#define GAMESERVER_TEXT_DATABASE_FILE_REGADMIN "regadmin"


typedef struct
{
	px_void *path;
}PX_StartupServer_Database;



px_bool PX_StartupServer_Database_Initialize(PX_StartupServer_Database *database);
px_bool PX_StartupServer_Database_AccountIsRegisted(PX_StartupServer_Database *database,px_char account[32]);
px_bool PX_StartupServer_Database_RegistryAccount(PX_StartupServer_Database *database,px_char account[32],px_word nickname[32],px_char pwd[32],px_char regcode[32],px_dword authority_level,px_char admin_name[16]);
px_bool PX_StartupServer_Database_AccountVerify(PX_StartupServer_Database *database,px_char account[32],px_char pwd[32]);
px_bool PX_StartupServer_Database_ReadUserData(PX_StartupServer_Database *database,px_char account[32],PX_Startup_UserData *pUserData);
px_bool PX_StartupServer_Database_AddCoin(PX_StartupServer_Database *database,px_char account[32],px_int count);
px_bool PX_StartupServer_Database_AddTicket(PX_StartupServer_Database *database,px_char account[32],px_int ticket);
px_bool PX_StartupServer_Database_WriteUserData(PX_StartupServer_Database *database,px_char account[32],PX_Startup_UserData *pUserData);
px_bool PX_StartupServer_Database_WriteUserPhoto(PX_StartupServer_Database *database,px_char account[32],px_void *data,px_int size);
px_bool PX_StartupServer_Database_WriteBackupUserData(PX_StartupServer_Database *database,px_char account[32],PX_Startup_UserData *pUserData);
px_bool PX_StartupServer_Database_ReadPhoto(PX_StartupServer_Database *database,px_char account[32],px_byte data[]);
#endif
