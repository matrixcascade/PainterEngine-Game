#include "Game_ResoucesLoader.h"
px_bool Game_ResourcesLoaderInitialize(PX_Instance *Ins,GAME_RESOURCESLOADER_TYPE type)
{

	PX_Runtime *runtime=&Ins->runtime;

	//client
	switch (type)
	{
	case GAME_RESOURCESLOADER_TYPE_CLIENT:
		{
			//FontModule
			if(!PX_FontModuleInitialize(&Ins->runtime.mp_resources,&Ins->FontModule32))
			{
				return PX_FALSE;
			}

			if(!PX_FontModuleInitialize(&Ins->runtime.mp_resources,&Ins->FontModule24))
			{
				return PX_FALSE;
			}

			if(!PX_FontModuleInitialize(&Ins->runtime.mp_resources,&Ins->FontModule18))
			{
				return PX_FALSE;
			}

			if(!PX_FontModuleInitialize(&Ins->runtime.mp_resources,&Ins->FontModuleScore))
			{
				return PX_FALSE;
			}

			if (!PX_LoadFontModuleFromFile(&Ins->FontModule32,GAME_TEXT_PATH_FONTMODULE32_CHS))
			{
				return PX_FALSE;
			}

			if (!PX_LoadFontModuleFromFile(&Ins->FontModule32,GAME_TEXT_PATH_FONTMODULE32_EN))
			{
				return PX_FALSE;
			}

			if (!PX_LoadFontModuleFromFile(&Ins->FontModule24,GAME_TEXT_PATH_FONTMODULE24_CHS))
			{
				return PX_FALSE;
			}

			if (!PX_LoadFontModuleFromFile(&Ins->FontModule24,GAME_TEXT_PATH_FONTMODULE24_EN))
			{
				return PX_FALSE;
			}

			if (!PX_LoadFontModuleFromFile(&Ins->FontModule18,GAME_TEXT_PATH_FONTMODULE18_CHS))
			{
				return PX_FALSE;
			}

			if (!PX_LoadFontModuleFromFile(&Ins->FontModule18,GAME_TEXT_PATH_FONTMODULE18_EN))
			{
				return PX_FALSE;
			}

			if (!PX_LoadFontModuleFromFile(&Ins->FontModuleScore,GAME_TEXT_PATH_FONTMODULE_SCOREPANEL))
			{
				return PX_FALSE;
			}
			
			//shield
			if (!PX_LoadTextureToResource(runtime,GAME_TEXT_PATH_SHIELD,GAME_TEXT_KEY_TEX_SHIELD))
			{
				return PX_FALSE;
			}

			//BGM
			if (!PX_LoadSoundToResource(runtime,GAME_TEXT_PATH_BGM,GAME_TEXT_KEY_SND_BGM))
			{
				return PX_FALSE;
			}

			//Explosion
			if (!PX_LoadTextureToResource(runtime,GAME_TEXT_PATH_EXPLODE_PARTICAL,GAME_TEXT_KEY_TEX_EXPLODE_PARTICAL))
			{
				return PX_FALSE;
			}

			//logos
			if (!PX_LoadShapeToResource(runtime,GAME_TEXT_PATH_MISSILE_LOGO,GAME_TEXT_KEY_MISSILE_LOGO))
			{
				return PX_FALSE;
			}
			if (!PX_LoadShapeToResource(runtime,GAME_TEXT_PATH_MACHINEGUN_LOGO,GAME_TEXT_KEY_MACHINEGUN_LOGO))
			{
				return PX_FALSE;
			}
			if (!PX_LoadShapeToResource(runtime,GAME_TEXT_PATH_RAILGUN_LOGO,GAME_TEXT_KEY_RAILGUN_LOGO))
			{
				return PX_FALSE;
			}
			if (!PX_LoadShapeToResource(runtime,GAME_TEXT_PATH_NUCLEAR_LOGO,GAME_TEXT_KEY_NUCLEAR_LOGO))
			{
				return PX_FALSE;
			}
			if (!PX_LoadShapeToResource(runtime,GAME_TEXT_PATH_BLACKHOLE_LOGO,GAME_TEXT_KEY_BLACKHOLE_LOGO))
			{
				return PX_FALSE;
			}
			if (!PX_LoadShapeToResource(runtime,GAME_TEXT_PATH_MINE_LOGO,GAME_TEXT_KEY_MINE_LOGO))
			{
				return PX_FALSE;
			}
			if (!PX_LoadShapeToResource(runtime,GAME_TEXT_PATH_SHIELD_LOGO,GAME_TEXT_KEY_SHIELD_LOGO))
			{
				return PX_FALSE;
			}
			if (!PX_LoadShapeToResource(runtime,GAME_TEXT_PATH_POWERUP_LOGO,GAME_TEXT_KEY_POWERUP_LOGO))
			{
				return PX_FALSE;
			}
			if (!PX_LoadShapeToResource(runtime,GAME_TEXT_PATH_SPEEDUP_LOGO,GAME_TEXT_KEY_SPEEDUP_LOGO))
			{
				return PX_FALSE;
			}
			if (!PX_LoadShapeToResource(runtime,GAME_TEXT_PATH_REPAIR_LOGO,GAME_TEXT_KEY_REPAIR_LOGO))
			{
				return PX_FALSE;
			}

			//Explosion sound
			if (!PX_LoadSoundToResource(runtime,GAME_TEXT_PATH_SND_EXPLODE01,GAME_TEXT_KEY_SND_EXPLODE01))
			{
				return PX_FALSE;
			}

			if (!PX_LoadSoundToResource(runtime,GAME_TEXT_PATH_SND_EXPLODE02,GAME_TEXT_KEY_SND_EXPLODE02))
			{
				return PX_FALSE;
			}
			if (!PX_LoadSoundToResource(runtime,GAME_TEXT_PATH_SND_EXPLODE03,GAME_TEXT_KEY_SND_EXPLODE03))
			{
				return PX_FALSE;
			}

			if (!PX_LoadTextureToResource(runtime,GAME_TEXT_PATH_STONE1,GAME_TEXT_KEY_TEX_STONE1))
			{
				return PX_FALSE;
			}

			if (!PX_LoadTextureToResource(runtime,GAME_TEXT_PATH_STONE2,GAME_TEXT_KEY_TEX_STONE2))
			{
				return PX_FALSE;
			}

			if (!PX_LoadTextureToResource(runtime,GAME_TEXT_PATH_STONE3,GAME_TEXT_KEY_TEX_STONE3))
			{
				return PX_FALSE;
			}
			//////////////////////////////////////////////////////////////////////////
			//rail gun
			if (!PX_LoadAnimationToResource(runtime,GAME_TEXT_PATH_RAILGUN_AMMO,GAME_TEXT_KEY_RAILGUN_AMMO))
			{
				return PX_FALSE;
			}

			if (!PX_LoadAnimationToResource(runtime,GAME_TEXT_PATH_RAILGUN_EXPLOSION,GAME_TEXT_KEY_RAILGUN_EXPLOSION))
			{
				return PX_FALSE;
			}

			if (!PX_LoadAnimationToResource(runtime,GAME_TEXT_PATH_RAILGUN_FIRE,GAME_TEXT_KEY_RAILGUN_FIRE))
			{
				return PX_FALSE;
			}

			if (!PX_LoadSoundToResource(runtime,GAME_TEXT_PATH_RAILGUN_FIRESOUND,GAME_TEXT_KEY_RAILGUN_FIRESOUND))
			{
				return PX_FALSE;
			}

			if (!PX_LoadSoundToResource(runtime,GAME_TEXT_PATH_RAILGUN_EXPOLSIONSOUND,GAME_TEXT_KEY_RAILGUN_EXPOLSIONSOUND))
			{
				return PX_FALSE;
			}

			//////////////////////////////////////////////////////////////////////////
			//machine gun
			if (!PX_LoadTextureToResource(runtime,GAME_TEXT_PATH_MACHINEGUN_AMMO,GAME_TEXT_KEY_TEX_MACHINEGUN_AMMO))
			{
				return PX_FALSE;
			}

			if (!PX_LoadSoundToResource(runtime,GAME_TEXT_PATH_MACHINEGUN_FIRESOUND,GAME_TEXT_KEY_MACHINEGUN_FIRESOUND))
			{
				return PX_FALSE;
			}

			if (!PX_LoadTextureToResource(runtime,GAME_TEXT_PATH_PARTICAL_FIRE_LARGE,GAME_TEXT_KEY_PARTICAL_FIRE_LARGE))
			{
				return PX_FALSE;
			}

			if (!PX_LoadTextureToResource(runtime,GAME_TEXT_PATH_MACHINE_FIRE,GAME_TEXT_KEY_PARTICAL_SHOTGUNASH))
			{
				return PX_FALSE;
			}
			//////////////////////////////////////////////////////////////////////////////

			//////////////////////////////////////////////////////////////////////////
			//missile
			if (!PX_LoadAnimationToResource(runtime,GAME_TEXT_PATH_MISSILE_AMMO,GAME_TEXT_KEY_MISSILE_AMMO))
			{
				return PX_FALSE;
			}

			if (!PX_LoadSoundToResource(runtime,GAME_TEXT_PATH_MISSILE_LAUNCH,GAME_TEXT_KEY_MISSILE_LAUNCH))
			{
				return PX_FALSE;
			}

			//////////////////////////////////////////////////////////////////////////
			//////////////////////////////////////////////////////////////////////////
			//nuclear
			if (!PX_LoadAnimationToResource(runtime,GAME_TEXT_PATH_NUCLEAR_AMMO,GAME_TEXT_KEY_NUCLEAR_AMMO))
			{
				return PX_FALSE;
			}

			if (!PX_LoadSoundToResource(runtime,GAME_TEXT_PATH_NUCLEAR_LAUNCHSOUND,GAME_TEXT_KEY_NUCLEAR_LAUNCHSOUND))
			{
				return PX_FALSE;
			}
 
			if (!PX_LoadSoundToResource(runtime,GAME_TEXT_PATH_NUCLEAR_EXPLOSIONSOUND,GAME_TEXT_KEY_NUCLEAR_EXPLOSIONSOUND))
			{
				return PX_FALSE;
			}
			//////////////////////////////////////////////////////////////////////////

			//////////////////////////////////////////////////////////////////////////
			//mine
			if (!PX_LoadAnimationToResource(runtime,GAME_TEXT_PATH_MINE_AMMO,GAME_TEXT_KEY_MINE_AMMO))
			{
				return PX_FALSE;
			}
			//////////////////////////////////////////////////////////////////////////

			//////////////////////////////////////////////////////////////////////////
			//BlackHole
			if (!PX_LoadAnimationToResource(runtime,GAME_TEXT_PATH_BLACKHOLE_AMMO,GAME_TEXT_KEY_BLACKHOLE_AMMO))
			{
				return PX_FALSE;
			}

			if (!PX_LoadSoundToResource(runtime,GAME_TEXT_PATH_BLACKHOLE_EXPLOSIONSOUND,GAME_TEXT_KEY_BLACKHOLE_EXPLOSIONSOUND))
			{
				return PX_FALSE;
			}
			//////////////////////////////////////////////////////////////////////////

			//ship

			if(!PX_LoadTextureToResource(runtime,GAME_TEXT_PATH_SHIP,GAME_TEXT_KEY_TEX_SHIP_ALPHA))
			{
				return PX_FALSE;
			}

			if (!PX_LoadTextureToResource(runtime,GAME_TEXT_PATH_CURSOR,GAME_TEXT_KEY_TEX_CURSOR))
			{
				return PX_FALSE;
			}
			//////////////////////////////////////////////////////////////////////////
			//pickup explosion
			if(!PX_LoadSoundToResource(runtime,GAME_TEXT_PATH_PICKUPEXPLOSION_SOUND,GAME_TEXT_KEY_PICKUPEXPLOSION_SOUND))
			{
				return PX_FALSE;
			}
			//////////////////////////////////////////////////////////////////////////

			//////////////////////////////////////////////////////////////////////////
			//Treasure
			if(!PX_LoadTextureToResource(runtime,GAME_TEXT_PATH_TEX_TREASURE0,GAME_TEXT_KEY_TEX_TREASURE0))
			{
				return PX_FALSE;
			}

			if(!PX_LoadTextureToResource(runtime,GAME_TEXT_PATH_TEX_TREASURE1,GAME_TEXT_KEY_TEX_TREASURE1))
			{
				return PX_FALSE;
			}

			//////////////////////////////////////////////////////////////////////////
			//TreasureTag
			if(!PX_LoadShapeToResource(runtime,GAME_TEXT_PATH_TREASURETAG,GAME_TEXT_KEY_TREASURETAG))
			{
				return PX_FALSE;
			}

			//////////////////////////////////////////////////////////////////////////
			//ProtectShield
			if(!PX_LoadShapeToResource(runtime,GAME_TEXT_PATH_PROTECTSHIELD_LOGO,GAME_TEXT_KEY_PROTECTSHIELD_LOGO))
			{
				return PX_FALSE;
			}

			if(!PX_LoadSoundToResource(runtime,GAME_TEXT_PATH_PROTECTSHIELDSOUND,GAME_TEXT_KEY_PROTECTSHIELDSOUND))
			{
				return PX_FALSE;
			}

			//////////////////////////////////////////////////////////////////////////
			//Alien
			if (!PX_LoadAnimationToResource(runtime,GAME_TEXT_PATH_ALIEN,GAME_TEXT_KEY_ALIEN))
			{
				return PX_FALSE;
			}

			//////////////////////////////////////////////////////////////////////////
			//
			if(!PX_LoadShapeToResource(runtime,GAME_TEXT_PATH_PIERCER_LOGO,GAME_TEXT_KEY_PIERCER_LOGO))
			{
				return PX_FALSE;
			}
			if (!PX_LoadAnimationToResource(runtime,GAME_TEXT_PATH_PIERCER,GAME_TEXT_KEY_PIERCER))
			{
				return PX_FALSE;
			}

			//////////////////////////////////////////////////////////////////////////
			//
			if (!PX_LoadSoundToResource(runtime,GAME_TEXT_PATH_PIERCER_FIRESOUND,GAME_TEXT_KEY_PIERCER_FIRESOUND))
			{
				return PX_FALSE;
			}


		}
		break;
	case GAME_RESOURCESLOADER_TYPE_SERVER:
		{

		}
		break;
	}
	return PX_TRUE;
}

