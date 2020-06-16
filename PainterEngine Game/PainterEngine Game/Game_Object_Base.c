#include "Game_Object_Base.h"



px_dword Game_Object_GroupImpactTestType(px_int player)
{
	switch(player)
	{
	case 0:
		return GAME_IMPACT_TYPE_ALL_OBJECTGROUP^(GAME_IMPACT_TYPE_GROUP0);
		break;
	case 1:
		return GAME_IMPACT_TYPE_ALL_OBJECTGROUP^(GAME_IMPACT_TYPE_GROUP1);
		break;
	case 2:
		return GAME_IMPACT_TYPE_ALL_OBJECTGROUP^(GAME_IMPACT_TYPE_GROUP2);
		break;
	case 3:
		return GAME_IMPACT_TYPE_ALL_OBJECTGROUP^(GAME_IMPACT_TYPE_GROUP3);
		break;
	case 4:
		return GAME_IMPACT_TYPE_ALL_OBJECTGROUP^(GAME_IMPACT_TYPE_GROUP4);
		break;
	case 5:
		return GAME_IMPACT_TYPE_ALL_OBJECTGROUP^(GAME_IMPACT_TYPE_GROUP5);
		break;
	case 6:
		return GAME_IMPACT_TYPE_ALL_OBJECTGROUP^(GAME_IMPACT_TYPE_GROUP6);
		break;
	case 7:
		return GAME_IMPACT_TYPE_ALL_OBJECTGROUP^(GAME_IMPACT_TYPE_GROUP7);
		break;
	case 8:
		return GAME_IMPACT_TYPE_ALL_OBJECTGROUP^(GAME_IMPACT_TYPE_GROUP8);
		break;
	case 9:
		return GAME_IMPACT_TYPE_ALL_OBJECTGROUP^(GAME_IMPACT_TYPE_GROUP9);
		break;
	case 10:
		return GAME_IMPACT_TYPE_ALL_OBJECTGROUP^(GAME_IMPACT_TYPE_GROUP10);
		break;
	case 11:
		return GAME_IMPACT_TYPE_ALL_OBJECTGROUP^(GAME_IMPACT_TYPE_GROUP11);
		break;
	case 12:
		return GAME_IMPACT_TYPE_ALL_OBJECTGROUP^(GAME_IMPACT_TYPE_NATURE);
		break;
	default:
		return 0;
	}
}

px_dword Game_Object_GroupImpactType(px_int player)
{
	switch(player)
	{
	case 0:
		return GAME_IMPACT_TYPE_GROUP0;
		break;
	case 1:
		return GAME_IMPACT_TYPE_GROUP1;
		break;
	case 2:
		return GAME_IMPACT_TYPE_GROUP2;
		break;
	case 3:
		return GAME_IMPACT_TYPE_GROUP3;
		break;
	case 4:
		return GAME_IMPACT_TYPE_GROUP4;
		break;
	case 5:
		return GAME_IMPACT_TYPE_GROUP5;
		break;
	case 6:
		return GAME_IMPACT_TYPE_GROUP6;
		break;
	case 7:
		return GAME_IMPACT_TYPE_GROUP7;
		break;
	case 8:
		return GAME_IMPACT_TYPE_GROUP8;
		break;
	case 9:
		return GAME_IMPACT_TYPE_GROUP9;
		break;
	case 10:
		return GAME_IMPACT_TYPE_GROUP10;
		break;
	case 11:
		return GAME_IMPACT_TYPE_GROUP11;
		break;
	default:
		return 0;
	}
}
