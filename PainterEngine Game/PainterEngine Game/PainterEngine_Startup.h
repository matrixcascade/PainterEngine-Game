#ifndef PAINTERENGINE_STARTUP_H
#define PAINTERENGINE_STARTUP_H

#include "../../../PainterEngine/platform/windows/Platform_Windows_D2D.h"
#include "../../../PainterEngine/platform/windows/Platform_Windows_Audio.h"
#include "../../../PainterEngine/Architecture/PainterEngine_Console.h"
#include "../../../PainterEngine/Architecture/PainterEngine_MessageBox.h"
#include "../../../PainterEngine/platform/windows/px_udp.h"


extern px_uint64 game_rand_seed;


typedef struct 
{
	px_point Cursor;
	px_bool  LeftButtonDown;
	px_bool	 RightButtonDown;
}PX_Controller;


typedef struct
{
	PX_Runtime runtime;
	PX_SoundPlay soundplay;
	PX_Controller Controller;

	PX_FontModule FontModule32;
	PX_FontModule FontModule24;
	PX_FontModule FontModule18;
	PX_FontModule FontModuleScore;
}PX_Instance;


//////////////////////////////////////////////////////////////////////////

typedef struct
{
	px_byte *buffer;
	px_uint size;
}PX_IO_Data;

px_bool PX_InstanceInitialize(PX_Instance *Instance,const px_char name[],px_int width,px_int height,px_uint ui_size,px_uint resource_size,px_uint game_size);
px_uint32 Game_rand();
PX_IO_Data PX_LoadFileToIOData(const px_char path[]);
px_void PX_FreeIOData(PX_IO_Data *io);
px_bool PX_LoadTextureFromFile(px_memorypool *mp,px_texture *tex,const px_char path[]);
px_bool PX_LoadShapeFromFile(px_memorypool *mp,px_shape *shape,const px_char path[]);
px_bool PX_LoadSoundFromFile(px_memorypool *mp,px_shape *shape,const px_char path[]);
px_bool PX_LoadAnimationLibraryFromFile(px_memorypool *mp,PX_Animationlibrary *lib,const px_char path[]);
px_bool PX_LoadScriptFromFile(px_memory *code,const px_char path[]);
px_bool PX_LoadScriptInstanceFromFile(px_memorypool *mp,PX_ScriptVM_Instance *ins,const px_char path[]);
px_bool PX_LoadTextureToResource(PX_Runtime *runtime,const px_char Path[],const px_char key[]);
px_bool PX_LoadShapeToResource(PX_Runtime *runtime,const px_char Path[],const px_char key[]);
px_bool PX_LoadAnimationToResource(PX_Runtime *runtime,const px_char Path[],const px_char key[]);
px_bool PX_LoadScriptToResource(PX_Runtime *runtime,const px_char Path[],const px_char key[]);
px_bool PX_LoadSoundToResource(PX_Runtime *runtime,const px_char Path[],const px_char key[]);
px_bool PX_LoadFontModuleFromFile(PX_FontModule *fm,const px_char Path[]);
px_void PX_PlayLocalSoundFromResource(PX_Instance *pIns,PX_World *pWorld,px_float x,px_float y,const px_char key[],px_bool loop);
px_void PX_PlayGlobalSoundFromResource(PX_Instance *pIns,const px_char key[],px_bool loop);
#endif
