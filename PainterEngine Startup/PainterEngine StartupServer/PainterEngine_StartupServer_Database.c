#include "PainterEngine_StartupServer_Database.h"
#include "windows.h"
#include "direct.h"
#include "time.h"

px_bool PX_StartupServer_Database_Initialize(PX_StartupServer_Database *database)
{
	if(GetFileAttributes(GAMESERVER_TEXT_DATABASE_FILE_DATABASE)!=FILE_ATTRIBUTE_DIRECTORY) return PX_FALSE;
	database->path=GAMESERVER_TEXT_DATABASE_FILE_DATABASE;
	return PX_TRUE;
}

px_bool PX_StartupServer_Database_AccountIsRegisted(PX_StartupServer_Database *database,px_char account[32])
{
	px_int i;
	px_char path[MAX_PATH];
	px_char hash_char[65]={0};
	PX_SHA256_HASH sha256;
	PX_Sha256Calculate(account,32,&sha256);
	for (i=0;i<64;i+=2)
	{
		PX_itoa(sha256.bytes[i/2],hash_char+i,3,16);
		if (hash_char[i+1]==0)
		{
			hash_char[i+1]=hash_char[i];
			hash_char[i]='0';
		}
	}
	sprintf_s(path,sizeof(path),"%s/%s",database->path,hash_char);
	if (GetFileAttributesA(path)!=INVALID_FILE_ATTRIBUTES)
	{
		return PX_TRUE;
	}
	return PX_FALSE;
}



//////////////////////////////////////////////////////////////////////////
//dir-----sha256 account
//userinfo
//password
//data(2dx data)
//regcode
//log
//authority(authority level)
//regadmin

px_bool PX_StartupServer_Database_RegistryAccount(PX_StartupServer_Database *database,px_char account[32],px_word nickname[32],px_char pwd[32],px_char regcode[32],px_dword authority_level,px_char admin_name[16])
{
	px_int i;
	px_char hash_char[65]={0};
	px_char path[MAX_PATH];
	PX_SHA256_HASH sha256;
	PX_Startup_UserData userinfo;
	time_t t;
	struct tm * lt;
	FILE *pf;
	//check data
	if (account[31]!=0||nickname[31]!=0||regcode[31]!=0)
	{
		return PX_FALSE;
	}


	PX_Sha256Calculate(account,32,&sha256);
	for (i=0;i<64;i+=2)
	{
		PX_itoa(sha256.bytes[i/2],hash_char+i,3,16);
		if (hash_char[i+1]==0)
		{
			hash_char[i+1]=hash_char[i];
			hash_char[i]='0';
		} 
	}
	sprintf_s(path,sizeof(path),"%s/%s",database->path,hash_char);
	if (GetFileAttributesA(path)!=INVALID_FILE_ATTRIBUTES)
	{
		px_byte pwd_hash[32];
		PX_Sha256Calculate(pwd,32,&sha256);
		sprintf_s(path,sizeof(path),"%s/%s/%s",database->path,hash_char,GAMESERVER_TEXT_DATABASE_FILE_PASSWORD);
		pf=fopen(path,"rb");
		if (!pf)
		{
			return PX_FALSE;
		}
		if (!fread(pwd_hash,sizeof(pwd_hash),1,pf))
		{
			fclose(pf);
			return PX_FALSE;
		}
		fclose(pf);
		return PX_memequ(pwd_hash,sha256.bytes,sizeof(pwd_hash));
	}
	_mkdir(path);
	//password
	PX_Sha256Calculate(pwd,32,&sha256);
	sprintf_s(path,sizeof(path),"%s/%s/%s",database->path,hash_char,GAMESERVER_TEXT_DATABASE_FILE_PASSWORD);
	pf=fopen(path,"wb");
	if (!pf)
	{
		return PX_FALSE;
	}
	if (!fwrite(sha256.bytes,sizeof(sha256.bytes),1,pf))
	{
		fclose(pf);
		return PX_FALSE;
	}
	fclose(pf);

	///
	//default photo
	sprintf_s(path,sizeof(path),"%s/%s/%s",database->path,hash_char,GAMESERVER_TEXT_DATABASE_FILE_PROFILE_PHOTO);
	if(!PX_CopyFile("./resources/default_photo.traw",path))
	{
		return PX_FALSE;
	}

	//user info
	PX_memset(&userinfo,0,sizeof(userinfo));
	PX_strcpy(userinfo.account,account,sizeof(userinfo.account));
	PX_wstrcpy(userinfo.nickname,nickname,sizeof(userinfo.nickname));
	userinfo.rank=0;
	userinfo.coin=0;
	userinfo.ticket=100;
	userinfo.rank_pt=10000;
	userinfo.ShipStack[0].type=0;
	userinfo.ShipStack[0].unlock=PX_TRUE;

	sprintf_s(path,sizeof(path),"%s/%s/%s",database->path,hash_char,GAMESERVER_TEXT_DATABASE_FILE_USERINFO);
	pf=fopen(path,"wb");
	if (!pf)
	{
		return PX_FALSE;
	}
	if (!fwrite(&userinfo,sizeof(userinfo),1,pf))
	{
		fclose(pf);
		return PX_FALSE;
	}
	fclose(pf);
	
	//regcode
	sprintf_s(path,sizeof(path),"%s/%s/%s",database->path,hash_char,GAMESERVER_TEXT_DATABASE_FILE_REGCODE);
	pf=fopen(path,"wb");
	if (!pf)
	{
		return PX_FALSE;
	}
	fprintf(pf,regcode);
	fclose(pf);

	//log
	sprintf_s(path,sizeof(path),"%s/%s/%s",database->path,hash_char,GAMESERVER_TEXT_DATABASE_FILE_LOG);
	pf=fopen(path,"wb");
	if (!pf)
	{
		return PX_FALSE;
	}
	fputs("sign up in ",pf);
	time (&t);
	lt = localtime (&t);
	fprintf (pf,"%d/%d/%d %d:%d:%d\r\n",lt->tm_year+1900, lt->tm_mon, lt->tm_mday, lt->tm_hour, lt->tm_min, lt->tm_sec);

	fclose(pf);

	//auth_level
	sprintf_s(path,sizeof(path),"%s/%s/%s",database->path,hash_char,GAMESERVER_TEXT_DATABASE_FILE_AUTHORITY);
	pf=fopen(path,"wb");
	if (!pf)
	{
		return PX_FALSE;
	}
	if (!fwrite(&authority_level,sizeof(authority_level),1,pf))
	{
		fclose(pf);
		return PX_FALSE;
	}
	fclose(pf);


	//reg admin
	sprintf_s(path,sizeof(path),"%s/%s/%s",database->path,hash_char,GAMESERVER_TEXT_DATABASE_FILE_REGADMIN);
	pf=fopen(path,"wb");
	if (!pf)
	{
		return PX_FALSE;
	}
	fprintf(pf,admin_name);
	fclose(pf);

	return PX_TRUE;
}

px_bool PX_StartupServer_Database_AccountVerify(PX_StartupServer_Database *database,px_char account[32],px_char pwd[32])
{
	px_int i;
	px_char hash_char[65]={0};
	px_byte pwd_hash[32];
	px_char path[MAX_PATH];
	time_t t;
	struct tm * lt;
	PX_SHA256_HASH sha256;
	FILE *pf;
	//check data
	if (account[31]!=0)
	{
		return PX_FALSE;
	}


	PX_Sha256Calculate(account,32,&sha256);
	for (i=0;i<64;i+=2)
	{
		PX_itoa(sha256.bytes[i/2],hash_char+i,3,16);
		if (hash_char[i+1]==0)
		{
			hash_char[i+1]=hash_char[i];
			hash_char[i]='0';
		}
	}
	sprintf_s(path,sizeof(path),"%s/%s",database->path,hash_char);
	if (GetFileAttributesA(path)==INVALID_FILE_ATTRIBUTES) return PX_FALSE;

	
	PX_Sha256Calculate(pwd,32,&sha256);
	sprintf_s(path,sizeof(path),"%s/%s/%s",database->path,hash_char,GAMESERVER_TEXT_DATABASE_FILE_PASSWORD);
	pf=fopen(path,"rb");
	if (!pf)
	{
		return PX_FALSE;
	}
	if (!fread(pwd_hash,sizeof(pwd_hash),1,pf))
	{
		fclose(pf);
		return PX_FALSE;
	}
	fclose(pf);
	if(!PX_memequ(pwd_hash,sha256.bytes,sizeof(pwd_hash))) return PX_FALSE;

	

	//log
	sprintf_s(path,sizeof(path),"%s/%s/%s",database->path,hash_char,GAMESERVER_TEXT_DATABASE_FILE_LOG);
	pf=fopen(path,"a+");
	if (!pf)
	{
		return PX_FALSE;
	}
	fputs("Login at ",pf);
	time (&t);
	lt = localtime (&t);
	fprintf (pf,"%d/%d/%d %d:%d:%d\r\n",lt->tm_year+1900, lt->tm_mon, lt->tm_mday, lt->tm_hour, lt->tm_min, lt->tm_sec);

	fclose(pf);
	return PX_TRUE;
}

px_bool PX_StartupServer_Database_ReadUserData(PX_StartupServer_Database *database,px_char account[32],PX_Startup_UserData *pUserData)
{
	px_int i;
	px_char hash_char[65]={0};
	px_char path[MAX_PATH];
	PX_SHA256_HASH sha256;
	FILE *pf;
	//check data
	if (account[31]!=0)
	{
		return PX_FALSE;
	}


	PX_Sha256Calculate(account,32,&sha256);
	for (i=0;i<64;i+=2)
	{
		PX_itoa(sha256.bytes[i/2],hash_char+i,3,16);
		if (hash_char[i+1]==0)
		{
			hash_char[i+1]=hash_char[i];
			hash_char[i]='0';
		}
	}
	sprintf_s(path,sizeof(path),"%s/%s",database->path,hash_char);
	if (GetFileAttributesA(path)==INVALID_FILE_ATTRIBUTES) return PX_FALSE;

	sprintf_s(path,sizeof(path),"%s/%s/%s",database->path,hash_char,GAMESERVER_TEXT_DATABASE_FILE_USERINFO);
	pf=fopen(path,"rb");
	if (!pf)
	{
		return PX_FALSE;
	}
	if (!fread(pUserData,sizeof(PX_Startup_UserData),1,pf))
	{
		fclose(pf);
		return PX_FALSE;
	}
	fclose(pf);
	return PX_TRUE;
}

px_bool PX_StartupServer_Database_AddCoin(PX_StartupServer_Database *database,px_char account[32],px_int count)
{
	PX_Startup_UserData userData;
	if(PX_StartupServer_Database_ReadUserData(database,account,&userData))
	{
		if (count>0)
		{
			userData.coin+=count;
		}
		else
		{
			if ((px_int)userData.coin<PX_ABS(count))
			{
				userData.coin=0;
			}
			else
			{
				userData.coin+=count;
			}
		}
		PX_StartupServer_Database_WriteUserData(database,account,&userData);
		return PX_TRUE;
	}
	return PX_FALSE;
}

px_bool PX_StartupServer_Database_AddTicket(PX_StartupServer_Database *database,px_char account[32],px_int count)
{
	PX_Startup_UserData userData;
	if(PX_StartupServer_Database_ReadUserData(database,account,&userData))
	{
		if (count>0)
		{
			userData.ticket+=count;
		}
		else
		{
			if ((px_int)userData.ticket<PX_ABS(count))
			{
				userData.ticket=0;
			}
			else
			{
				userData.ticket+=count;
			}
		}
		PX_StartupServer_Database_WriteUserData(database,account,&userData);
		return PX_TRUE;
	}
	return PX_FALSE;
}

px_bool PX_StartupServer_Database_WriteUserData(PX_StartupServer_Database *database,px_char account[32],PX_Startup_UserData *pUserData)
{
	px_int i;
	px_char hash_char[65]={0};
	px_char path[MAX_PATH];
	PX_SHA256_HASH sha256;
	FILE *pf;
	//check data
	if (account[31]!=0)
	{
		return PX_FALSE;
	}


	PX_Sha256Calculate(account,32,&sha256);
	for (i=0;i<64;i+=2)
	{
		PX_itoa(sha256.bytes[i/2],hash_char+i,3,16);
		if (hash_char[i+1]==0)
		{
			hash_char[i+1]=hash_char[i];
			hash_char[i]='0';
		}
	}
	sprintf_s(path,sizeof(path),"%s/%s",database->path,hash_char);
	if (GetFileAttributesA(path)==INVALID_FILE_ATTRIBUTES) return PX_FALSE;

	sprintf_s(path,sizeof(path),"%s/%s/%s",database->path,hash_char,GAMESERVER_TEXT_DATABASE_FILE_USERINFO);
	pf=fopen(path,"wb");
	if (!pf)
	{
		return PX_FALSE;
	}
	if (!fwrite(pUserData,1,sizeof(PX_Startup_UserData),pf))
	{
		fclose(pf);
		return PX_FALSE;
	}
	fclose(pf);
	return PX_TRUE;
}

px_bool PX_StartupServer_Database_WriteUserPhoto(PX_StartupServer_Database *database,px_char account[32],px_void *data,px_int size)
{
	px_int i;
	px_char hash_char[65]={0};
	px_char path[MAX_PATH];
	PX_SHA256_HASH sha256;
	FILE *pf;
	//check data
	if (account[31]!=0)
	{
		return PX_FALSE;
	}


	PX_Sha256Calculate(account,32,&sha256);
	for (i=0;i<64;i+=2)
	{
		PX_itoa(sha256.bytes[i/2],hash_char+i,3,16);
		if (hash_char[i+1]==0)
		{
			hash_char[i+1]=hash_char[i];
			hash_char[i]='0';
		}
	}
	sprintf_s(path,sizeof(path),"%s/%s",database->path,hash_char);
	if (GetFileAttributesA(path)==INVALID_FILE_ATTRIBUTES) return PX_FALSE;

	sprintf_s(path,sizeof(path),"%s/%s/%s",database->path,hash_char,GAMESERVER_TEXT_DATABASE_FILE_PROFILE_PHOTO);
	pf=fopen(path,"wb");
	if (!pf)
	{
		return PX_FALSE;
	}
	if (!fwrite(data,1,size,pf))
	{
		fclose(pf);
		return PX_FALSE;
	}
	fclose(pf);
	return PX_TRUE;
}

px_bool PX_StartupServer_Database_WriteBackupUserData(PX_StartupServer_Database *database,px_char account[32],PX_Startup_UserData *pUserData)
{
	px_int i;
	px_char hash_char[65]={0};
	px_char path[MAX_PATH];
	PX_SHA256_HASH sha256;
	FILE *pf;
	//check data
	if (account[31]!=0)
	{
		return PX_FALSE;
	}


	PX_Sha256Calculate(account,32,&sha256);
	for (i=0;i<64;i+=2)
	{
		PX_itoa(sha256.bytes[i/2],hash_char+i,3,16);
		if (hash_char[i+1]==0)
		{
			hash_char[i+1]=hash_char[i];
			hash_char[i]='0';
		}
	}
	sprintf_s(path,sizeof(path),"%s/%s",database->path,hash_char);
	if (GetFileAttributesA(path)==INVALID_FILE_ATTRIBUTES) return PX_FALSE;

	sprintf_s(path,sizeof(path),"%s/%s/%s",database->path,hash_char,GAMESERVER_TEXT_DATABASE_FILE_USERINFO_BAK);
	pf=fopen(path,"rb");
	if (pf==PX_NULL)
	{
		pf=fopen(path,"wb");
		if (!pf)
		{
			return PX_FALSE;
		}
		if (!fwrite(pUserData,1,sizeof(PX_Startup_UserData),pf))
		{
			fclose(pf);
			return PX_FALSE;
		}
		fclose(pf);
	}
	else
	{
		fclose(pf);
	}
	
	return PX_TRUE;
}

px_bool PX_StartupServer_Database_ReadPhoto(PX_StartupServer_Database *database,px_char account[32],px_byte data[])
{
	px_int i;
	px_char hash_char[65]={0};
	px_char path[MAX_PATH];
	PX_SHA256_HASH sha256;
	FILE *pf;
	//check data
	if (account[31]!=0)
	{
		return PX_FALSE;
	}


	PX_Sha256Calculate(account,32,&sha256);
	for (i=0;i<64;i+=2)
	{
		PX_itoa(sha256.bytes[i/2],hash_char+i,3,16);
		if (hash_char[i+1]==0)
		{
			hash_char[i+1]=hash_char[i];
			hash_char[i]='0';
		}
	}
	sprintf_s(path,sizeof(path),"%s/%s",database->path,hash_char);
	if (GetFileAttributesA(path)==INVALID_FILE_ATTRIBUTES) return PX_FALSE;

	sprintf_s(path,sizeof(path),"%s/%s/%s",database->path,hash_char,GAMESERVER_TEXT_DATABASE_FILE_PROFILE_PHOTO);
	pf=fopen(path,"rb");
	if (!pf)
	{
		return PX_FALSE;
	}
	if (fread(data,1,PX_STARTUP_USERDATA_PHOTOSIZE,pf)!=PX_STARTUP_USERDATA_PHOTOSIZE)
	{
		fclose(pf);
		return PX_FALSE;
	}
	fclose(pf);
	return PX_TRUE;
}
