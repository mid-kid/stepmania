#include "stdafx.h"
/*
-----------------------------------------------------------------------------
 Class: GameManager

 Desc: See Header.

 Copyright (c) 2001-2002 by the person(s) listed below.  All rights reserved.
	Chris Danford
-----------------------------------------------------------------------------
*/

#include "GameManager.h"
#include "PrefsManager.h"
#include "GameConstantsAndTypes.h"
#include "GameState.h"
#include "GameInput.h"	// for GameButton constants
#define DIRECTINPUT_VERSION  0x0800
#include <dinput.h> 	// for DIK_* key codes

GameManager*	GAMEMAN = NULL;	// global and accessable from anywhere in our program


const int DANCE_COL_SPACING = 64;
const int DANCE_6PANEL_VERSUS_COL_SPACING = 54;
const int PUMP_COL_SPACING = 50;
const int EZ2_COL_SPACING = 46; 
const int EZ2_DOUBLE_ADJUST = 150;


struct {
	char *name;
	int NumTracks;
} const NotesTypes[NUM_NOTES_TYPES] = {
	{ "dance-single", 4 },
	{ "dance-double", 8 },
	{ "dance-couple", 8 },
	{ "dance-solo", 6 },
	{ "pump-single", 5 },
	{ "pump-double", 10 },
	{ "ez2-single", 5 },		// Single: TL,LHH,D,RHH,TR
	{ "ez2-single-hard", 5 },	// Single: TL,LHH,D,RHH,TR
	{ "ez2-double", 10 },		// Double: Single x2
	{ "ez2-real", 7 }			// Real: TL,LHH,LHL,D,RHL,RHH,TR
};

//
// Important:  Every game must define the buttons: "Start", "Back", "MenuLeft", and "MenuRight"
//
GameDef g_GameDefs[NUM_GAMES] = 
{
	{	// GAME_DANCE
		"dance",					// m_szName
		"Dance Dance Revolution",	// m_szDescription
		2,							// m_iNumControllers
		NUM_DANCE_BUTTONS,			// m_iButtonsPerController
		{	// m_szButtonNames
			"Left",
			"Right",
			"Up",
			"Down",
			"UpLeft",
			"UpRight",
			"Start",
			"Back",
			"MenuLeft",
			"MenuRight",
			"MenuUp",
			"MenuDown",
		},
		{	// m_szSecondaryFunction
			"(MenuLeft)",
			"(MenuRight)",
			"(MenuUp)",
			"(MenuDown)",
			"",
			"",
			"",
			"",
			"(dedicated)",
			"(dedicated)",
			"(dedicated)",
			"(dedicated)",
		},
		{	// m_DedicatedMenuButton
			DANCE_BUTTON_MENULEFT,	// MENU_BUTTON_LEFT
			DANCE_BUTTON_MENURIGHT,	// MENU_BUTTON_RIGHT
			DANCE_BUTTON_MENUUP,	// MENU_BUTTON_UP
			DANCE_BUTTON_MENUDOWN,	// MENU_BUTTON_DOWN
			DANCE_BUTTON_START,		// MENU_BUTTON_START
			DANCE_BUTTON_BACK,		// MENU_BUTTON_BACK
		},
		{	// m_SecondaryMenuButton
			DANCE_BUTTON_LEFT,		// MENU_BUTTON_LEFT
			DANCE_BUTTON_RIGHT,		// MENU_BUTTON_RIGHT
			DANCE_BUTTON_UP,		// MENU_BUTTON_UP
			DANCE_BUTTON_DOWN,		// MENU_BUTTON_DOWN
			DANCE_BUTTON_START,		// MENU_BUTTON_START
			DANCE_BUTTON_BACK,		// MENU_BUTTON_BACK
		},
		{	// m_iDefaultKeyboardKey
			{	// PLAYER_1
				DIK_LEFT,				// DANCE_BUTTON_LEFT,
				DIK_RIGHT,				// DANCE_BUTTON_RIGHT,
				DIK_UP,					// DANCE_BUTTON_UP,
				DIK_DOWN,				// DANCE_BUTTON_DOWN,
				-1, //no default key 	// DANCE_BUTTON_UPLEFT,
				-1, //no default key	// DANCE_BUTTON_UPRIGHT,
				DIK_RETURN,				// DANCE_BUTTON_START,
				DIK_ESCAPE,				// DANCE_BUTTON_BACK
				-1, //no default key	// DANCE_BUTTON_MENULEFT
				-1, //no default key	// DANCE_BUTTON_MENURIGHT
				DIK_UP,					// DANCE_BUTTON_MENUUP
				DIK_DOWN,				// DANCE_BUTTON_MENUDOWN
			},
			{	// PLAYER_2
				DIK_NUMPAD4,			// DANCE_BUTTON_LEFT,
				DIK_NUMPAD6,			// DANCE_BUTTON_RIGHT,
				DIK_NUMPAD8,			// DANCE_BUTTON_UP,
				DIK_NUMPAD2,			// DANCE_BUTTON_DOWN,
				DIK_NUMPAD7,			// DANCE_BUTTON_UPLEFT,
				DIK_NUMPAD9,			// DANCE_BUTTON_UPRIGHT,
				DIK_NUMPADENTER,		// DANCE_BUTTON_START,
				DIK_NUMPAD0,			// DANCE_BUTTON_BACK
				-1, //no default key	// DANCE_BUTTON_MENULEFT
				-1, //no default key	// DANCE_BUTTON_MENURIGHT
				-1, //no default key	// DANCE_BUTTON_MENUUP
				-1, //no default key	// DANCE_BUTTON_MENUDOWN
			},
		}
	},
	{	// GAME_PUMP
		"pump",				// m_szName
		"Pump It Up",		// m_szDescription
		2,					// m_iNumControllers
		NUM_PUMP_BUTTONS,	// m_iButtonsPerController
		{	// m_szButtonNames
			"UpLeft",
			"UpRight",
			"Center",
			"DownLeft",
			"DownRight",
			"Start",
			"Back",
			"MenuLeft",
			"MenuRight",
			"MenuUp",
			"MenuDown",
		},
		{	// m_szSecondaryFunction
			"(MenuLeft)",
			"(MenuRight)",
			"(Start)",
			"(MenuUp)",
			"(MenuDown)",
			"(dedicated)",
			"",
			"(dedicated)",
			"(dedicated)",
			"(dedicated)",
			"(dedicated)",
		},
		{	// m_DedicatedMenuButton
			PUMP_BUTTON_MENULEFT,	// MENU_BUTTON_LEFT
			PUMP_BUTTON_MENURIGHT,	// MENU_BUTTON_RIGHT
			PUMP_BUTTON_MENUUP,		// MENU_BUTTON_UP
			PUMP_BUTTON_MENUDOWN,	// MENU_BUTTON_DOWN
			PUMP_BUTTON_START,		// MENU_BUTTON_START
			PUMP_BUTTON_BACK,		// MENU_BUTTON_BACK
		},
		{	// m_SecondaryMenuButton
			PUMP_BUTTON_UPLEFT,		// MENU_BUTTON_LEFT
			PUMP_BUTTON_UPRIGHT,	// MENU_BUTTON_RIGHT
			PUMP_BUTTON_DOWNLEFT,	// MENU_BUTTON_UP
			PUMP_BUTTON_DOWNRIGHT,	// MENU_BUTTON_DOWN
			PUMP_BUTTON_CENTER,		// MENU_BUTTON_START
			PUMP_BUTTON_BACK,		// MENU_BUTTON_BACK
		},
		{	// m_iDefaultKeyboardKey
			{	// PLAYER_1
				DIK_Q,					// PUMP_BUTTON_UPLEFT,
				DIK_E,					// PUMP_BUTTON_UPRIGHT,
				DIK_S,					// PUMP_BUTTON_CENTER,
				DIK_Z,					// PUMP_BUTTON_DOWNLEFT,
				DIK_C,  				// PUMP_BUTTON_DOWNRIGHT,
				DIK_RETURN,				// PUMP_BUTTON_START,
				DIK_ESCAPE,				// PUMP_BUTTON_BACK,
				DIK_LEFT,				// PUMP_BUTTON_MENULEFT
				DIK_RIGHT,				// PUMP_BUTTON_MENURIGHT
				DIK_UP,					// PUMP_BUTTON_MENUUP
				DIK_DOWN,				// PUMP_BUTTON_MENUDOWN
			},
			{	// PLAYER_2
				DIK_NUMPAD7,			// PUMP_BUTTON_UPLEFT,
				DIK_NUMPAD9,			// PUMP_BUTTON_UPRIGHT,
				DIK_NUMPAD5,			// PUMP_BUTTON_CENTER,
				DIK_NUMPAD1,			// PUMP_BUTTON_DOWNLEFT,
				DIK_NUMPAD3,  			// PUMP_BUTTON_DOWNRIGHT,
				DIK_NUMPADENTER,		// PUMP_BUTTON_START,
				DIK_NUMPAD0,			// PUMP_BUTTON_BACK,
				-1,	//no default key	// PUMP_BUTTON_MENULEFT
				-1, //no default key	// PUMP_BUTTON_MENURIGHT
				-1, //no default key	// PUMP_BUTTON_MENUUP
				-1, //no default key	// PUMP_BUTTON_MENUDOWN
			},
		}
	},
	{
		"ez2",						// m_szName
		"Ez2dancer",				// m_szDescription
		2,							// m_iNumControllers
		NUM_EZ2_BUTTONS,			// m_iButtonsPerController
		{	// m_szButtonNames
			"FootUpLeft",
			"FootUpRight",
			"FootDown",
			"HandUpLeft",
			"HandUpRight",
			"HandLrLeft",
			"HandLrRight",
			"Start",
			"Back",
			"MenuLeft",
			"MenuRight",
			"MenuUp",
			"MenuDown",
		},
		{	// m_szSecondaryFunction
			"(MenuUp)",
			"(MenuDown)",
			"(Start)",
			"(MenuLeft)",
			"(MenuRight)",
			"",
			"",
			"(dedicated)",
			"",
			"(dedicated)",
			"(dedicated)",
			"(dedicated)",
			"(dedicated)",
		},
		{	// m_DedicatedMenuButton
			EZ2_BUTTON_MENULEFT,	// MENU_BUTTON_LEFT
			EZ2_BUTTON_MENURIGHT,	// MENU_BUTTON_RIGHT
			EZ2_BUTTON_MENUUP,		// MENU_BUTTON_UP
			EZ2_BUTTON_MENUDOWN,	// MENU_BUTTON_DOWN
			EZ2_BUTTON_START,		// MENU_BUTTON_START
			EZ2_BUTTON_BACK,		// MENU_BUTTON_BACK
		},
		{	// m_SecondaryMenuButton
			EZ2_BUTTON_HANDUPLEFT,	// MENU_BUTTON_LEFT
			EZ2_BUTTON_HANDUPRIGHT,	// MENU_BUTTON_RIGHT
			EZ2_BUTTON_FOOTUPLEFT,	// MENU_BUTTON_UP
			EZ2_BUTTON_FOOTUPRIGHT,	// MENU_BUTTON_DOWN
			EZ2_BUTTON_FOOTDOWN,	// MENU_BUTTON_START
			EZ2_BUTTON_BACK,		// MENU_BUTTON_BACK
		},
		{	// m_iDefaultKeyboardKey
			{	// PLAYER_1
				DIK_Z,					// EZ2_BUTTON_FOOTUPLEFT,
				DIK_C,					// EZ2_BUTTON_FOOTUPRIGHT,
				DIK_X,					// EZ2_BUTTON_FOOTDOWN,
				DIK_Q,					// EZ2_BUTTON_HANDUPLEFT,
				DIK_A,					// EZ2_BUTTON_HANDUPRIGHT,
				DIK_E,					// EZ2_BUTTON_HANDLRLEFT,
				DIK_D,  				// EZ2_BUTTON_HANDLRRIGHT,
				DIK_RETURN,				// EZ2_BUTTON_START,
				DIK_ESCAPE,				// EZ2_BUTTON_BACK,
				DIK_LEFT,				// EZ2_BUTTON_MENULEFT
				DIK_RIGHT,				// EZ2_BUTTON_MENURIGHT
				DIK_UP,					// EZ2_BUTTON_MENUUP
				DIK_DOWN,				// EZ2_BUTTON_MENUDOWN
			},
			{	// PLAYER_2
				DIK_NUMPAD1,			// EZ2_BUTTON_FOOTUPLEFT,
				DIK_NUMPAD3,			// EZ2_BUTTON_FOOTUPRIGHT,
				DIK_NUMPAD2,			// EZ2_BUTTON_FOOTDOWN,
				DIK_NUMPAD7,			// EZ2_BUTTON_HANDUPLEFT,
				DIK_NUMPAD9,			// EZ2_BUTTON_HANDUPRIGHT,
				DIK_NUMPAD4,			// EZ2_BUTTON_HANDLRLEFT,
				DIK_NUMPAD6,  			// EZ2_BUTTON_HANDLRRIGHT,
				DIK_NUMPADENTER,		// EZ2_BUTTON_START,
				DIK_NUMPAD0,			// EZ2_BUTTON_BACK,
				-1,	//no default key	// EZ2_BUTTON_MENULEFT
				-1, //no default key	// EZ2_BUTTON_MENURIGHT
				-1, //no default key	// EZ2_BUTTON_MENUUP
				-1, //no default key	// EZ2_BUTTON_MENUDOWN
			},
		},
	},

};

StyleDef g_StyleDefs[NUM_STYLES] = 
{
	{	// STYLE_DANCE_SINGLE
		GAME_DANCE,								// m_Game
		true,									// m_bUsedForGameplay
		true,									// m_bUsedForEdit
		"DDR Single",								// m_szName
		{ NOTES_TYPE_DANCE_SINGLE,NOTES_TYPE_DANCE_SINGLE },	// m_NotesTypes
		NOTES_TYPE_INVALID,						// m_FallbackNotesType
		StyleDef::ONE_PLAYER_ONE_CREDIT,		// m_StyleType
		{ 160, 480 },							// m_iCenterX
		4,										// m_iColsPerPlayer
		{	// m_ColumnInfo[NUM_PLAYERS][MAX_COLS_PER_PLAYER];
			{	// PLAYER_1
				{ TRACK_1,	GAME_CONTROLLER_1,	DANCE_BUTTON_LEFT,	-DANCE_COL_SPACING*1.5f },
				{ TRACK_2,	GAME_CONTROLLER_1,	DANCE_BUTTON_DOWN,	-DANCE_COL_SPACING*0.5f },
				{ TRACK_3,	GAME_CONTROLLER_1,	DANCE_BUTTON_UP,	+DANCE_COL_SPACING*0.5f },
				{ TRACK_4,	GAME_CONTROLLER_1,	DANCE_BUTTON_RIGHT,	+DANCE_COL_SPACING*1.5f },
			},
			{	// PLAYER_2
				{ TRACK_1,	GAME_CONTROLLER_2,	DANCE_BUTTON_LEFT,	-DANCE_COL_SPACING*1.5f },
				{ TRACK_2,	GAME_CONTROLLER_2,	DANCE_BUTTON_DOWN,	-DANCE_COL_SPACING*0.5f },
				{ TRACK_3,	GAME_CONTROLLER_2,	DANCE_BUTTON_UP,	+DANCE_COL_SPACING*0.5f },
				{ TRACK_4,	GAME_CONTROLLER_2,	DANCE_BUTTON_RIGHT,	+DANCE_COL_SPACING*1.5f },
			},
		},
		{	// m_iColumnDrawOrder[MAX_COLS_PER_PLAYER];
			0, 1, 2, 3
		},
	},
	{	// STYLE_DANCE_VERSUS
		GAME_DANCE,								// m_Game
		true,									// m_bUsedForGameplay
		false,									// m_bUsedForEdit
		"DDR Versus",							// m_szName
		{ NOTES_TYPE_DANCE_SINGLE,NOTES_TYPE_DANCE_SINGLE }, // m_NotesTypes
		NOTES_TYPE_INVALID,						// m_FallbackNotesType
		StyleDef::TWO_PLAYERS_TWO_CREDITS,		// m_StyleType
		{ 160, 480 },							// m_iCenterX
		4,										// m_iColsPerPlayer
		{	// m_ColumnInfo[NUM_PLAYERS][MAX_COLS_PER_PLAYER];
			{	// PLAYER_1
				{ TRACK_1,	GAME_CONTROLLER_1,	DANCE_BUTTON_LEFT,	-DANCE_COL_SPACING*1.5f },
				{ TRACK_2,	GAME_CONTROLLER_1,	DANCE_BUTTON_DOWN,	-DANCE_COL_SPACING*0.5f },
				{ TRACK_3,	GAME_CONTROLLER_1,	DANCE_BUTTON_UP,	+DANCE_COL_SPACING*0.5f },
				{ TRACK_4,	GAME_CONTROLLER_1,	DANCE_BUTTON_RIGHT,	+DANCE_COL_SPACING*1.5f },
			},
			{	// PLAYER_2
				{ TRACK_1,	GAME_CONTROLLER_2,	DANCE_BUTTON_LEFT,	-DANCE_COL_SPACING*1.5f },
				{ TRACK_2,	GAME_CONTROLLER_2,	DANCE_BUTTON_DOWN,	-DANCE_COL_SPACING*0.5f },
				{ TRACK_3,	GAME_CONTROLLER_2,	DANCE_BUTTON_UP,	+DANCE_COL_SPACING*0.5f },
				{ TRACK_4,	GAME_CONTROLLER_2,	DANCE_BUTTON_RIGHT,	+DANCE_COL_SPACING*1.5f },
			},
		},
		{	// m_iColumnDrawOrder[MAX_COLS_PER_PLAYER];
			0, 1, 2, 3
		},
	},
	{	// STYLE_DANCE_DOUBLE
		GAME_DANCE,								// m_Game
		true,									// m_bUsedForGameplay
		true,									// m_bUsedForEdit
		"DDR Double",							// m_szName
		{ NOTES_TYPE_DANCE_DOUBLE,NOTES_TYPE_DANCE_DOUBLE },	// m_NotesTypes
		NOTES_TYPE_INVALID,
		StyleDef::ONE_PLAYER_TWO_CREDITS,		// m_StyleType
		{ 320, 320 },							// m_iCenterX
		8,										// m_iColsPerPlayer
		{	// m_ColumnInfo[NUM_PLAYERS][MAX_COLS_PER_PLAYER];
			{	// PLAYER_1
				{ TRACK_1,	GAME_CONTROLLER_1,	DANCE_BUTTON_LEFT,	-DANCE_COL_SPACING*3.5f },
				{ TRACK_2,	GAME_CONTROLLER_1,	DANCE_BUTTON_DOWN,	-DANCE_COL_SPACING*2.5f },
				{ TRACK_3,	GAME_CONTROLLER_1,	DANCE_BUTTON_UP,	-DANCE_COL_SPACING*1.5f },
				{ TRACK_4,	GAME_CONTROLLER_1,	DANCE_BUTTON_RIGHT,	-DANCE_COL_SPACING*0.5f },
				{ TRACK_5,	GAME_CONTROLLER_2,	DANCE_BUTTON_LEFT,	+DANCE_COL_SPACING*0.5f },
				{ TRACK_6,	GAME_CONTROLLER_2,	DANCE_BUTTON_DOWN,	+DANCE_COL_SPACING*1.5f },
				{ TRACK_7,	GAME_CONTROLLER_2,	DANCE_BUTTON_UP,	+DANCE_COL_SPACING*2.5f },
				{ TRACK_8,	GAME_CONTROLLER_2,	DANCE_BUTTON_RIGHT,	+DANCE_COL_SPACING*3.5f },
			},
			{	// PLAYER_2
				{ TRACK_1,	GAME_CONTROLLER_1,	DANCE_BUTTON_LEFT,	-DANCE_COL_SPACING*3.5f },
				{ TRACK_2,	GAME_CONTROLLER_1,	DANCE_BUTTON_DOWN,	-DANCE_COL_SPACING*2.5f },
				{ TRACK_3,	GAME_CONTROLLER_1,	DANCE_BUTTON_UP,	-DANCE_COL_SPACING*1.5f },
				{ TRACK_4,	GAME_CONTROLLER_1,	DANCE_BUTTON_RIGHT,	-DANCE_COL_SPACING*0.5f },
				{ TRACK_5,	GAME_CONTROLLER_2,	DANCE_BUTTON_LEFT,	+DANCE_COL_SPACING*0.5f },
				{ TRACK_6,	GAME_CONTROLLER_2,	DANCE_BUTTON_DOWN,	+DANCE_COL_SPACING*1.5f },
				{ TRACK_7,	GAME_CONTROLLER_2,	DANCE_BUTTON_UP,	+DANCE_COL_SPACING*2.5f },
				{ TRACK_8,	GAME_CONTROLLER_2,	DANCE_BUTTON_RIGHT,	+DANCE_COL_SPACING*3.5f },
			},
		},
		{	// m_iColumnDrawOrder[MAX_COLS_PER_PLAYER];
			0,1,2,3,4,5,6,7
		},
	},
	{	// StyleDef
		GAME_DANCE,							// m_Game
		true,								// m_bUsedForGameplay
		false,								// m_bUsedForEdit
		"DDR Couple",						// m_szName
		{ NOTES_TYPE_DANCE_COUPLE, NOTES_TYPE_DANCE_COUPLE },	// m_NotesTypes
		NOTES_TYPE_INVALID,						// m_FallbackNotesType
		StyleDef::TWO_PLAYERS_TWO_CREDITS,	// m_StyleType
		{ 160, 480 },						// m_iCenterX
		4,									// m_iColsPerPlayer
		{	// m_ColumnInfo[NUM_PLAYERS][MAX_COLS_PER_PLAYER];
			{	// PLAYER_1
				{ TRACK_1,	GAME_CONTROLLER_1,	DANCE_BUTTON_LEFT,	-DANCE_COL_SPACING*1.5f },
				{ TRACK_2,	GAME_CONTROLLER_1,	DANCE_BUTTON_DOWN,	-DANCE_COL_SPACING*0.5f },
				{ TRACK_3,	GAME_CONTROLLER_1,	DANCE_BUTTON_UP,	+DANCE_COL_SPACING*0.5f },
				{ TRACK_4,	GAME_CONTROLLER_1,	DANCE_BUTTON_RIGHT,	+DANCE_COL_SPACING*1.5f },
			},
			{	// PLAYER_2
				{ TRACK_5,	GAME_CONTROLLER_2,	DANCE_BUTTON_LEFT,	-DANCE_COL_SPACING*1.5f },
				{ TRACK_6,	GAME_CONTROLLER_2,	DANCE_BUTTON_DOWN,	-DANCE_COL_SPACING*0.5f },
				{ TRACK_7,	GAME_CONTROLLER_2,	DANCE_BUTTON_UP,	+DANCE_COL_SPACING*0.5f },
				{ TRACK_8,	GAME_CONTROLLER_2,	DANCE_BUTTON_RIGHT,	+DANCE_COL_SPACING*1.5f },
			},
		},
		{	// m_iColumnDrawOrder[MAX_COLS_PER_PLAYER];
			0,1,2,3
		},
	},
	{	// STYLE_DANCE_SOLO
		GAME_DANCE,							// m_Game
		true,								// m_bUsedForGameplay
		true,								// m_bUsedForEdit
		"DDR Solo",							// m_szName
		{ NOTES_TYPE_DANCE_SOLO,NOTES_TYPE_DANCE_SOLO },	// m_NotesTypes
		NOTES_TYPE_INVALID,						// m_FallbackNotesType
		StyleDef::ONE_PLAYER_ONE_CREDIT,	// m_StyleType
		{ 320, 320 },						// m_iCenterX
		6,									// m_iColsPerPlayer
		{	// m_ColumnInfo[NUM_PLAYERS][MAX_COLS_PER_PLAYER];
			{	// PLAYER_1
				{ TRACK_1,	GAME_CONTROLLER_1,	DANCE_BUTTON_LEFT,		-DANCE_COL_SPACING*2.5f },
				{ TRACK_2,	GAME_CONTROLLER_1,	DANCE_BUTTON_UPLEFT,	-DANCE_COL_SPACING*1.5f },
				{ TRACK_3,	GAME_CONTROLLER_1,	DANCE_BUTTON_DOWN,		-DANCE_COL_SPACING*0.5f },
				{ TRACK_4,	GAME_CONTROLLER_1,	DANCE_BUTTON_UP,		+DANCE_COL_SPACING*0.5f },
				{ TRACK_5,	GAME_CONTROLLER_1,	DANCE_BUTTON_UPRIGHT,	+DANCE_COL_SPACING*1.5f },
				{ TRACK_6,	GAME_CONTROLLER_1,	DANCE_BUTTON_RIGHT,		+DANCE_COL_SPACING*2.5f },
			},
			{	// PLAYER_2
				{ TRACK_1,	GAME_CONTROLLER_2,	DANCE_BUTTON_LEFT,		-DANCE_COL_SPACING*2.5f },
				{ TRACK_2,	GAME_CONTROLLER_2,	DANCE_BUTTON_UPLEFT,	-DANCE_COL_SPACING*1.5f },
				{ TRACK_3,	GAME_CONTROLLER_2,	DANCE_BUTTON_DOWN,		-DANCE_COL_SPACING*0.5f },
				{ TRACK_4,	GAME_CONTROLLER_2,	DANCE_BUTTON_UP,		+DANCE_COL_SPACING*0.5f },
				{ TRACK_5,	GAME_CONTROLLER_2,	DANCE_BUTTON_UPRIGHT,	+DANCE_COL_SPACING*1.5f },
				{ TRACK_6,	GAME_CONTROLLER_2,	DANCE_BUTTON_RIGHT,		+DANCE_COL_SPACING*2.5f },
			},
		},
		{	// m_iColumnDrawOrder[MAX_COLS_PER_PLAYER];
			0,1,2,3,4,5
		},
	},
	{	// STYLE_DANCE_COUPLE
		GAME_DANCE,								// m_Game
		false,									// m_bUsedForGameplay
		true,									// m_bUsedForEdit
		"DDR Couple",							// m_szName
		{ NOTES_TYPE_DANCE_COUPLE, NOTES_TYPE_DANCE_COUPLE }, // m_NotesTypes
		NOTES_TYPE_INVALID,						// m_FallbackNotesType
		StyleDef::ONE_PLAYER_TWO_CREDITS,		// m_StyleType
		{ 320, 320 },							// m_iCenterX
		8,										// m_iColsPerPlayer
		{	// m_ColumnInfo[NUM_PLAYERS][MAX_COLS_PER_PLAYER];
			{	// PLAYER_1
				{ TRACK_1,	GAME_CONTROLLER_1,	DANCE_BUTTON_LEFT,	-DANCE_COL_SPACING*3.5f },
				{ TRACK_2,	GAME_CONTROLLER_1,	DANCE_BUTTON_DOWN,	-DANCE_COL_SPACING*2.5f },
				{ TRACK_3,	GAME_CONTROLLER_1,	DANCE_BUTTON_UP,	-DANCE_COL_SPACING*1.5f },
				{ TRACK_4,	GAME_CONTROLLER_1,	DANCE_BUTTON_RIGHT,	-DANCE_COL_SPACING*0.5f },
				{ TRACK_5,	GAME_CONTROLLER_2,	DANCE_BUTTON_LEFT,	+DANCE_COL_SPACING*0.5f },
				{ TRACK_6,	GAME_CONTROLLER_2,	DANCE_BUTTON_DOWN,	+DANCE_COL_SPACING*1.5f },
				{ TRACK_7,	GAME_CONTROLLER_2,	DANCE_BUTTON_UP,	+DANCE_COL_SPACING*2.5f },
				{ TRACK_8,	GAME_CONTROLLER_2,	DANCE_BUTTON_RIGHT,	+DANCE_COL_SPACING*3.5f },
			},
			{	// PLAYER_2
				{ TRACK_1,	GAME_CONTROLLER_1,	DANCE_BUTTON_LEFT,	-DANCE_COL_SPACING*3.5f },
				{ TRACK_2,	GAME_CONTROLLER_1,	DANCE_BUTTON_DOWN,	-DANCE_COL_SPACING*2.5f },
				{ TRACK_3,	GAME_CONTROLLER_1,	DANCE_BUTTON_UP,	-DANCE_COL_SPACING*1.5f },
				{ TRACK_4,	GAME_CONTROLLER_1,	DANCE_BUTTON_RIGHT,	-DANCE_COL_SPACING*0.5f },
				{ TRACK_5,	GAME_CONTROLLER_2,	DANCE_BUTTON_LEFT,	+DANCE_COL_SPACING*0.5f },
				{ TRACK_6,	GAME_CONTROLLER_2,	DANCE_BUTTON_DOWN,	+DANCE_COL_SPACING*1.5f },
				{ TRACK_7,	GAME_CONTROLLER_2,	DANCE_BUTTON_UP,	+DANCE_COL_SPACING*2.5f },
				{ TRACK_8,	GAME_CONTROLLER_2,	DANCE_BUTTON_RIGHT,	+DANCE_COL_SPACING*3.5f },
			},
		},
		{	// m_iColumnDrawOrder[MAX_COLS_PER_PLAYER];
			0,1,2,3,4,5,6,7
		},
	},
/*	{	// STYLE_DANCE_SOLO_VERSUS 
		"dance-solo-versus",				// m_szName
		NOTES_TYPE_DANCE_SOLO,				// m_NotesType
		StyleDef::ONE_PLAYER_ONE_CREDIT,	// m_StyleType
		{ 160, 480 },						// m_iCenterX
		6,									// m_iColsPerPlayer
		{	// m_ColumnInfo[NUM_PLAYERS][MAX_COLS_PER_PLAYER];
			{	// PLAYER_1
				{ TRACK_1,	GAME_CONTROLLER_1,	DANCE_BUTTON_LEFT,		-DANCE_6PANEL_VERSUS_COL_SPACING*2.5f },
				{ TRACK_2,	GAME_CONTROLLER_1,	DANCE_BUTTON_UPLEFT,	-DANCE_6PANEL_VERSUS_COL_SPACING*1.5f },
				{ TRACK_3,	GAME_CONTROLLER_1,	DANCE_BUTTON_DOWN,		-DANCE_6PANEL_VERSUS_COL_SPACING*0.5f },
				{ TRACK_4,	GAME_CONTROLLER_1,	DANCE_BUTTON_UP,		+DANCE_6PANEL_VERSUS_COL_SPACING*0.5f },
				{ TRACK_5,	GAME_CONTROLLER_1,	DANCE_BUTTON_UPRIGHT,	+DANCE_6PANEL_VERSUS_COL_SPACING*1.5f },
				{ TRACK_6,	GAME_CONTROLLER_1,	DANCE_BUTTON_RIGHT,		+DANCE_6PANEL_VERSUS_COL_SPACING*2.5f },
			},
			{	// PLAYER_2
				{ TRACK_1,	GAME_CONTROLLER_2,	DANCE_BUTTON_LEFT,		-DANCE_6PANEL_VERSUS_COL_SPACING*2.5f },
				{ TRACK_2,	GAME_CONTROLLER_2,	DANCE_BUTTON_UPLEFT,	-DANCE_6PANEL_VERSUS_COL_SPACING*1.5f },
				{ TRACK_3,	GAME_CONTROLLER_2,	DANCE_BUTTON_DOWN,		-DANCE_6PANEL_VERSUS_COL_SPACING*0.5f },
				{ TRACK_4,	GAME_CONTROLLER_2,	DANCE_BUTTON_UP,		+DANCE_6PANEL_VERSUS_COL_SPACING*0.5f },
				{ TRACK_5,	GAME_CONTROLLER_2,	DANCE_BUTTON_UPRIGHT,	+DANCE_6PANEL_VERSUS_COL_SPACING*1.5f },
				{ TRACK_6,	GAME_CONTROLLER_2,	DANCE_BUTTON_RIGHT,		+DANCE_6PANEL_VERSUS_COL_SPACING*2.5f },
			},
		},
		{	// m_iColumnDrawOrder[MAX_COLS_PER_PLAYER];
			0,5,1,4,2,3		// outside in
		},
	},	*/
	{	// PUMP_STYLE_SINGLE
		GAME_PUMP,								// m_Game
		true,									// m_bUsedForGameplay
		true,									// m_bUsedForEdit
		"Pump Single",							// m_szName
		{ NOTES_TYPE_PUMP_SINGLE,NOTES_TYPE_PUMP_SINGLE },	// m_NotesTypes
		NOTES_TYPE_INVALID,						// m_FallbackNotesType
		StyleDef::ONE_PLAYER_ONE_CREDIT,		// m_StyleType
		{ 160, 480 },							// m_iCenterX
		5,										// m_iColsPerPlayer
		{	// m_ColumnInfo[NUM_PLAYERS][MAX_COLS_PER_PLAYER];
			{	// PLAYER_1
				{ TRACK_1,	GAME_CONTROLLER_1,	PUMP_BUTTON_DOWNLEFT,	-PUMP_COL_SPACING*2 },
				{ TRACK_2,	GAME_CONTROLLER_1,	PUMP_BUTTON_UPLEFT,		-PUMP_COL_SPACING*1 },
				{ TRACK_3,	GAME_CONTROLLER_1,	PUMP_BUTTON_CENTER,		+PUMP_COL_SPACING*0 },
				{ TRACK_4,	GAME_CONTROLLER_1,	PUMP_BUTTON_UPRIGHT,	+PUMP_COL_SPACING*1 },
				{ TRACK_5,	GAME_CONTROLLER_1,	PUMP_BUTTON_DOWNRIGHT,	+PUMP_COL_SPACING*2 },
			},
			{	// PLAYER_2
				{ TRACK_1,	GAME_CONTROLLER_2,	PUMP_BUTTON_DOWNLEFT,	-PUMP_COL_SPACING*2 },
				{ TRACK_2,	GAME_CONTROLLER_2,	PUMP_BUTTON_UPLEFT,		-PUMP_COL_SPACING*1 },
				{ TRACK_3,	GAME_CONTROLLER_2,	PUMP_BUTTON_CENTER,		+PUMP_COL_SPACING*0 },
				{ TRACK_4,	GAME_CONTROLLER_2,	PUMP_BUTTON_UPRIGHT,	+PUMP_COL_SPACING*1 },
				{ TRACK_5,	GAME_CONTROLLER_2,	PUMP_BUTTON_DOWNRIGHT,	+PUMP_COL_SPACING*2 },
			},
		},
		{	// m_iColumnDrawOrder[MAX_COLS_PER_PLAYER];
			0,2,4,1,3
		},
	},
	{	// PUMP_STYLE_VERSUS
		GAME_PUMP,								// m_Game
		true,									// m_bUsedForGameplay
		false,									// m_bUsedForEdit
		"Pump Versus",							// m_szName
		{ NOTES_TYPE_PUMP_SINGLE,NOTES_TYPE_PUMP_SINGLE },	// m_NotesTypes
		NOTES_TYPE_INVALID,						// m_FallbackNotesType
		StyleDef::TWO_PLAYERS_TWO_CREDITS,		// m_StyleType
		{ 160, 480 },							// m_iCenterX
		5,										// m_iColsPerPlayer
		{	// m_ColumnInfo[NUM_PLAYERS][MAX_COLS_PER_PLAYER];
			{	// PLAYER_1
				{ TRACK_1,	GAME_CONTROLLER_1,	PUMP_BUTTON_DOWNLEFT,	-PUMP_COL_SPACING*2 },
				{ TRACK_2,	GAME_CONTROLLER_1,	PUMP_BUTTON_UPLEFT,		-PUMP_COL_SPACING*1 },
				{ TRACK_3,	GAME_CONTROLLER_1,	PUMP_BUTTON_CENTER,		+PUMP_COL_SPACING*0 },
				{ TRACK_4,	GAME_CONTROLLER_1,	PUMP_BUTTON_UPRIGHT,	+PUMP_COL_SPACING*1 },
				{ TRACK_5,	GAME_CONTROLLER_1,	PUMP_BUTTON_DOWNRIGHT,	+PUMP_COL_SPACING*2 },
			},
			{	// PLAYER_2
				{ TRACK_1,	GAME_CONTROLLER_2,	PUMP_BUTTON_DOWNLEFT,	-PUMP_COL_SPACING*2 },
				{ TRACK_2,	GAME_CONTROLLER_2,	PUMP_BUTTON_UPLEFT,		-PUMP_COL_SPACING*1 },
				{ TRACK_3,	GAME_CONTROLLER_2,	PUMP_BUTTON_CENTER,		+PUMP_COL_SPACING*0 },
				{ TRACK_4,	GAME_CONTROLLER_2,	PUMP_BUTTON_UPRIGHT,	+PUMP_COL_SPACING*1 },
				{ TRACK_5,	GAME_CONTROLLER_2,	PUMP_BUTTON_DOWNRIGHT,	+PUMP_COL_SPACING*2 },
			},
		},
		{	// m_iColumnDrawOrder[MAX_COLS_PER_PLAYER];
			0,2,4,1,3
		},
	},
	{	// PUMP_STYLE_DOUBLE
		GAME_PUMP,								// m_Game
		true,									// m_bUsedForGameplay
		true,									// m_bUsedForEdit
		"Pump Double",							// m_szName
		{ NOTES_TYPE_PUMP_DOUBLE,NOTES_TYPE_PUMP_DOUBLE }, // m_NotesTypes
		NOTES_TYPE_INVALID,						// m_FallbackNotesType
		StyleDef::ONE_PLAYER_TWO_CREDITS,		// m_StyleType
		{ 320, 480 },							// m_iCenterX
		10,										// m_iColsPerPlayer
		{	// m_ColumnInfo[NUM_PLAYERS][MAX_COLS_PER_PLAYER];
			{	// PLAYER_1
				{ TRACK_1,	GAME_CONTROLLER_1,	PUMP_BUTTON_DOWNLEFT,	-PUMP_COL_SPACING*4.5f },
				{ TRACK_2,	GAME_CONTROLLER_1,	PUMP_BUTTON_UPLEFT,		-PUMP_COL_SPACING*3.5f },
				{ TRACK_3,	GAME_CONTROLLER_1,	PUMP_BUTTON_CENTER,		-PUMP_COL_SPACING*2.5f },
				{ TRACK_4,	GAME_CONTROLLER_1,	PUMP_BUTTON_UPRIGHT,	-PUMP_COL_SPACING*1.5f },
				{ TRACK_5,	GAME_CONTROLLER_1,	PUMP_BUTTON_DOWNRIGHT,	-PUMP_COL_SPACING*0.5f },
				{ TRACK_6,	GAME_CONTROLLER_2,	PUMP_BUTTON_DOWNLEFT,	+PUMP_COL_SPACING*0.5f },
				{ TRACK_7,	GAME_CONTROLLER_2,	PUMP_BUTTON_UPLEFT,		+PUMP_COL_SPACING*1.5f },
				{ TRACK_8,	GAME_CONTROLLER_2,	PUMP_BUTTON_CENTER,		+PUMP_COL_SPACING*2.5f },
				{ TRACK_9,	GAME_CONTROLLER_2,	PUMP_BUTTON_UPRIGHT,	+PUMP_COL_SPACING*3.5f },
				{ TRACK_10,	GAME_CONTROLLER_2,	PUMP_BUTTON_DOWNRIGHT,	+PUMP_COL_SPACING*4.5f },
			},
			{	// PLAYER_2
				{ TRACK_1,	GAME_CONTROLLER_1,	PUMP_BUTTON_DOWNLEFT,	-PUMP_COL_SPACING*4.5f },
				{ TRACK_2,	GAME_CONTROLLER_1,	PUMP_BUTTON_UPLEFT,		-PUMP_COL_SPACING*3.5f },
				{ TRACK_3,	GAME_CONTROLLER_1,	PUMP_BUTTON_CENTER,		-PUMP_COL_SPACING*2.5f },
				{ TRACK_4,	GAME_CONTROLLER_1,	PUMP_BUTTON_UPRIGHT,	-PUMP_COL_SPACING*1.5f },
				{ TRACK_5,	GAME_CONTROLLER_1,	PUMP_BUTTON_DOWNRIGHT,	-PUMP_COL_SPACING*0.5f },
				{ TRACK_6,	GAME_CONTROLLER_2,	PUMP_BUTTON_DOWNLEFT,	+PUMP_COL_SPACING*0.5f },
				{ TRACK_7,	GAME_CONTROLLER_2,	PUMP_BUTTON_UPLEFT,		+PUMP_COL_SPACING*1.5f },
				{ TRACK_8,	GAME_CONTROLLER_2,	PUMP_BUTTON_CENTER,		+PUMP_COL_SPACING*2.5f },
				{ TRACK_9,	GAME_CONTROLLER_2,	PUMP_BUTTON_UPRIGHT,	+PUMP_COL_SPACING*3.5f },
				{ TRACK_10,	GAME_CONTROLLER_2,	PUMP_BUTTON_DOWNRIGHT,	+PUMP_COL_SPACING*4.5f },
			},
		},
		{	// m_iColumnDrawOrder[MAX_COLS_PER_PLAYER];
			0,2,4,1,3,5,7,9,6,8
		},
	},
/*	{	// PUMP_STYLE_COUPLE
		GAME_PUMP,								// m_Game
		true,									// m_bUsedForGameplay
		false,									// m_bUsedForEdit
		"pump-couple",							// m_szName
		NOTES_TYPE_PUMP_COUPLE,					// m_NotesTypes
		StyleDef::TWO_PLAYERS_TWO_CREDITS,		// m_StyleType
		{ 160, 480 },							// m_iCenterX
		5,										// m_iColsPerPlayer
		{	// m_ColumnInfo[NUM_PLAYERS][MAX_COLS_PER_PLAYER];
			{	// PLAYER_1
				{ TRACK_1,	GAME_CONTROLLER_1,	PUMP_BUTTON_DOWNLEFT,	-PUMP_COL_SPACING*2 },
				{ TRACK_2,	GAME_CONTROLLER_1,	PUMP_BUTTON_UPLEFT,		-PUMP_COL_SPACING*1 },
				{ TRACK_3,	GAME_CONTROLLER_1,	PUMP_BUTTON_CENTER,		+PUMP_COL_SPACING*0 },
				{ TRACK_4,	GAME_CONTROLLER_1,	PUMP_BUTTON_UPRIGHT,	+PUMP_COL_SPACING*1 },
				{ TRACK_5,	GAME_CONTROLLER_1,	PUMP_BUTTON_DOWNRIGHT,	+PUMP_COL_SPACING*2 },
			},
			{	// PLAYER_2
				{ TRACK_1,	GAME_CONTROLLER_2,	PUMP_BUTTON_DOWNLEFT,	-PUMP_COL_SPACING*2 },
				{ TRACK_2,	GAME_CONTROLLER_2,	PUMP_BUTTON_UPLEFT,		-PUMP_COL_SPACING*1 },
				{ TRACK_3,	GAME_CONTROLLER_2,	PUMP_BUTTON_CENTER,		+PUMP_COL_SPACING*0 },
				{ TRACK_4,	GAME_CONTROLLER_2,	PUMP_BUTTON_UPRIGHT,	+PUMP_COL_SPACING*1 },
				{ TRACK_5,	GAME_CONTROLLER_2,	PUMP_BUTTON_DOWNRIGHT,	+PUMP_COL_SPACING*2 },
			},
		},
		{	// m_iColumnDrawOrder[MAX_COLS_PER_PLAYER];
			0,2,4,1,3
		},
	},
*/

	{	// EZ2_STYLE_SINGLE
		GAME_EZ2,								// m_Game
		true,									// m_bUsedForGameplay
		true,									// m_bUsedForEdit
		"EZ2 Single",							// m_szName
		{ NOTES_TYPE_EZ2_SINGLE,NOTES_TYPE_EZ2_SINGLE  },	 // m_NotesTypes
		NOTES_TYPE_INVALID,						// m_FallbackNotesType
		StyleDef::ONE_PLAYER_ONE_CREDIT,		// m_StyleType
		{ 160, 480 },							// m_iCenterX
		5,										// m_iColsPerPlayer
		{	// m_ColumnInfo[NUM_PLAYERS][MAX_COLS_PER_PLAYER];
			{	// PLAYER_1
				{ TRACK_1,	GAME_CONTROLLER_1,	EZ2_BUTTON_FOOTUPLEFT,	-EZ2_COL_SPACING*2 },
				{ TRACK_2,	GAME_CONTROLLER_1,	EZ2_BUTTON_HANDUPLEFT,	-EZ2_COL_SPACING*1 },
				{ TRACK_3,	GAME_CONTROLLER_1,	EZ2_BUTTON_FOOTDOWN,	+EZ2_COL_SPACING*0 },
				{ TRACK_4,	GAME_CONTROLLER_1,	EZ2_BUTTON_HANDUPRIGHT,	+EZ2_COL_SPACING*1 },
				{ TRACK_5,	GAME_CONTROLLER_1,	EZ2_BUTTON_FOOTUPRIGHT,	+EZ2_COL_SPACING*2 },
			},
			{	// PLAYER_2
				{ TRACK_1,	GAME_CONTROLLER_2,	EZ2_BUTTON_FOOTUPLEFT,	-EZ2_COL_SPACING*2 },
				{ TRACK_2,	GAME_CONTROLLER_2,	EZ2_BUTTON_HANDUPLEFT,	-EZ2_COL_SPACING*1 },
				{ TRACK_3,	GAME_CONTROLLER_2,	EZ2_BUTTON_FOOTDOWN,	+EZ2_COL_SPACING*0 },
				{ TRACK_4,	GAME_CONTROLLER_2,	EZ2_BUTTON_HANDUPRIGHT,	+EZ2_COL_SPACING*1 },
				{ TRACK_5,	GAME_CONTROLLER_2,	EZ2_BUTTON_FOOTUPRIGHT,	+EZ2_COL_SPACING*2 },
			},
		},
		{	// m_iColumnDrawOrder[MAX_COLS_PER_PLAYER];
			2,0,4,1,3 // This should be from back to front: Down, UpLeft, UpRight, Upper Left Hand, Upper Right Hand 
		},
	},
	{	// EZ2_STYLE_SINGLE_HARD
		GAME_EZ2,								// m_Game
		true,									// m_bUsedForGameplay
		true,									// m_bUsedForEdit
		"EZ2 SingleHard",							// m_szName
		{ NOTES_TYPE_EZ2_SINGLE_HARD,NOTES_TYPE_EZ2_SINGLE_HARD}, // m_NotesTypes
		NOTES_TYPE_INVALID,						// m_FallbackNotesType
		StyleDef::ONE_PLAYER_ONE_CREDIT,		// m_StyleType
		{ 160, 480 },							// m_iCenterX
		5,										// m_iColsPerPlayer
		{	// m_ColumnInfo[NUM_PLAYERS][MAX_COLS_PER_PLAYER];
			{	// PLAYER_1
				{ TRACK_1,	GAME_CONTROLLER_1,	EZ2_BUTTON_FOOTUPLEFT,	-EZ2_COL_SPACING*2 },
				{ TRACK_2,	GAME_CONTROLLER_1,	EZ2_BUTTON_HANDUPLEFT,	-EZ2_COL_SPACING*1 },
				{ TRACK_3,	GAME_CONTROLLER_1,	EZ2_BUTTON_FOOTDOWN,	+EZ2_COL_SPACING*0 },
				{ TRACK_4,	GAME_CONTROLLER_1,	EZ2_BUTTON_HANDUPRIGHT,	+EZ2_COL_SPACING*1 },
				{ TRACK_5,	GAME_CONTROLLER_1,	EZ2_BUTTON_FOOTUPRIGHT,	+EZ2_COL_SPACING*2 },
			},
			{	// PLAYER_2
				{ TRACK_1,	GAME_CONTROLLER_2,	EZ2_BUTTON_FOOTUPLEFT,	-EZ2_COL_SPACING*2 },
				{ TRACK_2,	GAME_CONTROLLER_2,	EZ2_BUTTON_HANDUPLEFT,	-EZ2_COL_SPACING*1 },
				{ TRACK_3,	GAME_CONTROLLER_2,	EZ2_BUTTON_FOOTDOWN,	+EZ2_COL_SPACING*0 },
				{ TRACK_4,	GAME_CONTROLLER_2,	EZ2_BUTTON_HANDUPRIGHT,	+EZ2_COL_SPACING*1 },
				{ TRACK_5,	GAME_CONTROLLER_2,	EZ2_BUTTON_FOOTUPRIGHT,	+EZ2_COL_SPACING*2 },
			},
		},
		{	// m_iColumnDrawOrder[MAX_COLS_PER_PLAYER];
			2,0,4,1,3 // This should be from back to front: Down, UpLeft, UpRight, Upper Left Hand, Upper Right Hand 
		},
	},
	{	// EZ2_STYLE_DOUBLE
		GAME_EZ2,								// m_Game
		true,									// m_bUsedForGameplay
		true,									// m_bUsedForEdit
		"EZ2Double",							// m_szName
		{ NOTES_TYPE_EZ2_DOUBLE,NOTES_TYPE_EZ2_DOUBLE },	// m_NotesTypes
		NOTES_TYPE_INVALID,						// m_FallbackNotesType
		StyleDef::ONE_PLAYER_TWO_CREDITS,		// m_StyleType
		{ 160, 480 },							// m_iCenterX
		10,										// m_iColsPerPlayer
		{	// m_ColumnInfo[NUM_PLAYERS][MAX_COLS_PER_PLAYER];
			{	// PLAYER_1
				{ TRACK_1,	GAME_CONTROLLER_1,	EZ2_BUTTON_FOOTUPLEFT,	-EZ2_COL_SPACING*4.5f+EZ2_DOUBLE_ADJUST },
				{ TRACK_2,	GAME_CONTROLLER_1,	EZ2_BUTTON_HANDUPLEFT,	-EZ2_COL_SPACING*3.5f+EZ2_DOUBLE_ADJUST },
				{ TRACK_3,	GAME_CONTROLLER_1,	EZ2_BUTTON_FOOTDOWN,	-EZ2_COL_SPACING*2.5f+EZ2_DOUBLE_ADJUST },
				{ TRACK_4,	GAME_CONTROLLER_1,	EZ2_BUTTON_HANDUPRIGHT,	-EZ2_COL_SPACING*1.5f+EZ2_DOUBLE_ADJUST },
				{ TRACK_5,	GAME_CONTROLLER_1,	EZ2_BUTTON_FOOTUPRIGHT,	-EZ2_COL_SPACING*0.5f+EZ2_DOUBLE_ADJUST },
				{ TRACK_6,	GAME_CONTROLLER_2,	EZ2_BUTTON_FOOTUPLEFT,	+EZ2_COL_SPACING*0.5f+EZ2_DOUBLE_ADJUST }, 
				{ TRACK_7,	GAME_CONTROLLER_2,	EZ2_BUTTON_HANDUPLEFT,	+EZ2_COL_SPACING*1.5f+EZ2_DOUBLE_ADJUST },  
				{ TRACK_8,	GAME_CONTROLLER_2,	EZ2_BUTTON_FOOTDOWN,	+EZ2_COL_SPACING*2.5f+EZ2_DOUBLE_ADJUST },
				{ TRACK_9,	GAME_CONTROLLER_2,	EZ2_BUTTON_HANDUPRIGHT,	+EZ2_COL_SPACING*3.5f+EZ2_DOUBLE_ADJUST },
				{ TRACK_10,	GAME_CONTROLLER_2,	EZ2_BUTTON_FOOTUPRIGHT,	+EZ2_COL_SPACING*4.5f+EZ2_DOUBLE_ADJUST },
			},
			{	// PLAYER_2
				{ TRACK_1,	GAME_CONTROLLER_1,	EZ2_BUTTON_FOOTUPLEFT,	-EZ2_COL_SPACING*4.5f+EZ2_DOUBLE_ADJUST },
				{ TRACK_2,	GAME_CONTROLLER_1,	EZ2_BUTTON_HANDUPLEFT,	-EZ2_COL_SPACING*3.5f+EZ2_DOUBLE_ADJUST },
				{ TRACK_3,	GAME_CONTROLLER_1,	EZ2_BUTTON_FOOTDOWN,	-EZ2_COL_SPACING*2.5f+EZ2_DOUBLE_ADJUST },
				{ TRACK_4,	GAME_CONTROLLER_1,	EZ2_BUTTON_HANDUPRIGHT,	-EZ2_COL_SPACING*1.5f+EZ2_DOUBLE_ADJUST },
				{ TRACK_5,	GAME_CONTROLLER_1,	EZ2_BUTTON_FOOTUPRIGHT,	-EZ2_COL_SPACING*0.5f+EZ2_DOUBLE_ADJUST },
				{ TRACK_6,	GAME_CONTROLLER_2,	EZ2_BUTTON_FOOTUPLEFT,	+EZ2_COL_SPACING*0.5f+EZ2_DOUBLE_ADJUST }, 
				{ TRACK_7,	GAME_CONTROLLER_2,	EZ2_BUTTON_HANDUPLEFT,	+EZ2_COL_SPACING*1.5f+EZ2_DOUBLE_ADJUST },  
				{ TRACK_8,	GAME_CONTROLLER_2,	EZ2_BUTTON_FOOTDOWN,	+EZ2_COL_SPACING*2.5f+EZ2_DOUBLE_ADJUST },
				{ TRACK_9,	GAME_CONTROLLER_2,	EZ2_BUTTON_HANDUPRIGHT,	+EZ2_COL_SPACING*3.5f+EZ2_DOUBLE_ADJUST },
				{ TRACK_10,	GAME_CONTROLLER_2,	EZ2_BUTTON_FOOTUPRIGHT,	+EZ2_COL_SPACING*4.5f+EZ2_DOUBLE_ADJUST },
			},
		},
		{	// m_iColumnDrawOrder[MAX_COLS_PER_PLAYER];
			2,0,4,1,3,7,5,9,6,8 // This should be from back to front: Down, UpLeft, UpRight, Upper Left Hand, Upper Right Hand 
		},
	},
	{	// EZ2_STYLE_REAL
		GAME_EZ2,								// m_Game
		true,									// m_bUsedForGameplay
		true,									// m_bUsedForEdit
		"EZ2 Real",								// m_szName
		{ NOTES_TYPE_EZ2_REAL,NOTES_TYPE_EZ2_REAL },	// m_NotesTypes
		NOTES_TYPE_INVALID,						// m_FallbackNotesType
		StyleDef::ONE_PLAYER_ONE_CREDIT,		// m_StyleType
		{ 160, 480 },							// m_iCenterX
		7,										// m_iColsPerPlayer
		{	// m_ColumnInfo[NUM_PLAYERS][MAX_COLS_PER_PLAYER];
			{	// PLAYER_1
				{ TRACK_1,	GAME_CONTROLLER_1,	EZ2_BUTTON_FOOTUPLEFT,	-EZ2_COL_SPACING*3 },
				{ TRACK_2,	GAME_CONTROLLER_1,	EZ2_BUTTON_HANDLRLEFT,	-EZ2_COL_SPACING*2 },
				{ TRACK_3,	GAME_CONTROLLER_1,	EZ2_BUTTON_HANDUPLEFT,	-EZ2_COL_SPACING*1 }, 
				{ TRACK_4,	GAME_CONTROLLER_1,	EZ2_BUTTON_FOOTDOWN,	+EZ2_COL_SPACING*0 },
				{ TRACK_5,	GAME_CONTROLLER_1,	EZ2_BUTTON_HANDUPRIGHT,	+EZ2_COL_SPACING*1 }, 
				{ TRACK_6,	GAME_CONTROLLER_1,	EZ2_BUTTON_HANDLRRIGHT,	+EZ2_COL_SPACING*2 },
				{ TRACK_7,	GAME_CONTROLLER_1,	EZ2_BUTTON_FOOTUPRIGHT,	+EZ2_COL_SPACING*3 },
			},
			{	// PLAYER_2
				{ TRACK_1,	GAME_CONTROLLER_2,	EZ2_BUTTON_FOOTUPLEFT,	-EZ2_COL_SPACING*3 },
				{ TRACK_2,	GAME_CONTROLLER_2,	EZ2_BUTTON_HANDLRLEFT,	-EZ2_COL_SPACING*2 },
				{ TRACK_3,	GAME_CONTROLLER_2,	EZ2_BUTTON_HANDUPLEFT,	-EZ2_COL_SPACING*1 }, 
				{ TRACK_4,	GAME_CONTROLLER_2,	EZ2_BUTTON_FOOTDOWN,	+EZ2_COL_SPACING*0 },
				{ TRACK_5,	GAME_CONTROLLER_2,	EZ2_BUTTON_HANDUPRIGHT,	+EZ2_COL_SPACING*1 }, 
				{ TRACK_6,	GAME_CONTROLLER_2,	EZ2_BUTTON_HANDLRRIGHT,	+EZ2_COL_SPACING*2 },
				{ TRACK_7,	GAME_CONTROLLER_2,	EZ2_BUTTON_FOOTUPRIGHT,	+EZ2_COL_SPACING*3 },
			},
		},
		{	// m_iColumnDrawOrder[MAX_COLS_PER_PLAYER];
			3,0,6,2,4,1,5 // This should be from back to front: Down, UpLeft, UpRight, Lower Left Hand, Lower Right Hand, Upper Left Hand, Upper Right Hand 
		},
	},
	{	// EZ2_STYLE_SINGLE_VERSUS
		GAME_EZ2,								// m_Game
		true,									// m_bUsedForGameplay
		true,									// m_bUsedForEdit
		"EZ2 Versus",							// m_szName
		{ NOTES_TYPE_EZ2_SINGLE,NOTES_TYPE_EZ2_SINGLE },	// m_NotesTypes
		NOTES_TYPE_INVALID,						// m_FallbackNotesType
		StyleDef::TWO_PLAYERS_TWO_CREDITS,		// m_StyleType
		{ 160, 480 },							// m_iCenterX
		5,										// m_iColsPerPlayer
		{	// m_ColumnInfo[NUM_PLAYERS][MAX_COLS_PER_PLAYER];
			{	// PLAYER_1
				{ TRACK_1,	GAME_CONTROLLER_1,	EZ2_BUTTON_FOOTUPLEFT,	-EZ2_COL_SPACING*2 },
				{ TRACK_2,	GAME_CONTROLLER_1,	EZ2_BUTTON_HANDUPLEFT,	-EZ2_COL_SPACING*1 },
				{ TRACK_3,	GAME_CONTROLLER_1,	EZ2_BUTTON_FOOTDOWN,	+EZ2_COL_SPACING*0 },
				{ TRACK_4,	GAME_CONTROLLER_1,	EZ2_BUTTON_HANDUPRIGHT,	+EZ2_COL_SPACING*1 },
				{ TRACK_5,	GAME_CONTROLLER_1,	EZ2_BUTTON_FOOTUPRIGHT,	+EZ2_COL_SPACING*2 },
			},
			{	// PLAYER_2
				{ TRACK_1,	GAME_CONTROLLER_2,	EZ2_BUTTON_FOOTUPLEFT,	-EZ2_COL_SPACING*2 },
				{ TRACK_2,	GAME_CONTROLLER_2,	EZ2_BUTTON_HANDUPLEFT,	-EZ2_COL_SPACING*1 },
				{ TRACK_3,	GAME_CONTROLLER_2,	EZ2_BUTTON_FOOTDOWN,	+EZ2_COL_SPACING*0 },
				{ TRACK_4,	GAME_CONTROLLER_2,	EZ2_BUTTON_HANDUPRIGHT,	+EZ2_COL_SPACING*1 },
				{ TRACK_5,	GAME_CONTROLLER_2,	EZ2_BUTTON_FOOTUPRIGHT,	+EZ2_COL_SPACING*2 },
			},
		},
		{	// m_iColumnDrawOrder[MAX_COLS_PER_PLAYER];
			2,0,4,1,3 // This should be from back to front: Down, UpLeft, UpRight, Upper Left Hand, Upper Right Hand 
		},
	},
	{	// EZ2_STYLE_SINGLE_VERSUS
		GAME_EZ2,								// m_Game
		true,									// m_bUsedForGameplay
		true,									// m_bUsedForEdit
		"EZ2 VersusHard",						// m_szName
		{ NOTES_TYPE_EZ2_SINGLE_HARD,NOTES_TYPE_EZ2_SINGLE_HARD },	// m_NotesTypes
		NOTES_TYPE_INVALID,						// m_FallbackNotesType
		StyleDef::TWO_PLAYERS_TWO_CREDITS,		// m_StyleType
		{ 160, 480 },							// m_iCenterX
		5,										// m_iColsPerPlayer
		{	// m_ColumnInfo[NUM_PLAYERS][MAX_COLS_PER_PLAYER];
			{	// PLAYER_1
				{ TRACK_1,	GAME_CONTROLLER_1,	EZ2_BUTTON_FOOTUPLEFT,	-EZ2_COL_SPACING*2 },
				{ TRACK_2,	GAME_CONTROLLER_1,	EZ2_BUTTON_HANDUPLEFT,	-EZ2_COL_SPACING*1 },
				{ TRACK_3,	GAME_CONTROLLER_1,	EZ2_BUTTON_FOOTDOWN,	+EZ2_COL_SPACING*0 },
				{ TRACK_4,	GAME_CONTROLLER_1,	EZ2_BUTTON_HANDUPRIGHT,	+EZ2_COL_SPACING*1 },
				{ TRACK_5,	GAME_CONTROLLER_1,	EZ2_BUTTON_FOOTUPRIGHT,	+EZ2_COL_SPACING*2 },
			},
			{	// PLAYER_2
				{ TRACK_1,	GAME_CONTROLLER_2,	EZ2_BUTTON_FOOTUPLEFT,	-EZ2_COL_SPACING*2 },
				{ TRACK_2,	GAME_CONTROLLER_2,	EZ2_BUTTON_HANDUPLEFT,	-EZ2_COL_SPACING*1 },
				{ TRACK_3,	GAME_CONTROLLER_2,	EZ2_BUTTON_FOOTDOWN,	+EZ2_COL_SPACING*0 },
				{ TRACK_4,	GAME_CONTROLLER_2,	EZ2_BUTTON_HANDUPRIGHT,	+EZ2_COL_SPACING*1 },
				{ TRACK_5,	GAME_CONTROLLER_2,	EZ2_BUTTON_FOOTUPRIGHT,	+EZ2_COL_SPACING*2 },
			},
		},
		{	// m_iColumnDrawOrder[MAX_COLS_PER_PLAYER];
			2,0,4,1,3 // This should be from back to front: Down, UpLeft, UpRight, Upper Left Hand, Upper Right Hand 
		},
	},
	{	// EZ2_STYLE_REAL_VERSUS
		GAME_EZ2,								// m_Game
		true,									// m_bUsedForGameplay
		true,									// m_bUsedForEdit
		"EZ2 VersusReal",						// m_szName
		{ NOTES_TYPE_EZ2_REAL,NOTES_TYPE_EZ2_REAL },	// m_NotesTypes
		NOTES_TYPE_INVALID,						// m_FallbackNotesType
		StyleDef::TWO_PLAYERS_TWO_CREDITS,		// m_StyleType
		{ 160, 480 },							// m_iCenterX
		7,										// m_iColsPerPlayer
		{	// m_ColumnInfo[NUM_PLAYERS][MAX_COLS_PER_PLAYER];
			{	// PLAYER_1
				{ TRACK_1,	GAME_CONTROLLER_1,	EZ2_BUTTON_FOOTUPLEFT,	-EZ2_COL_SPACING*3 },
				{ TRACK_2,	GAME_CONTROLLER_1,	EZ2_BUTTON_HANDLRLEFT,	-EZ2_COL_SPACING*2 },
				{ TRACK_3,	GAME_CONTROLLER_1,	EZ2_BUTTON_HANDUPLEFT,	-EZ2_COL_SPACING*1 }, 
				{ TRACK_4,	GAME_CONTROLLER_1,	EZ2_BUTTON_FOOTDOWN,	+EZ2_COL_SPACING*0 },
				{ TRACK_5,	GAME_CONTROLLER_1,	EZ2_BUTTON_HANDUPRIGHT,	+EZ2_COL_SPACING*1 }, 
				{ TRACK_6,	GAME_CONTROLLER_1,	EZ2_BUTTON_HANDLRRIGHT,	+EZ2_COL_SPACING*2 },
				{ TRACK_7,	GAME_CONTROLLER_1,	EZ2_BUTTON_FOOTUPRIGHT,	+EZ2_COL_SPACING*3 },
			},
			{	// PLAYER_2
				{ TRACK_1,	GAME_CONTROLLER_2,	EZ2_BUTTON_FOOTUPLEFT,	-EZ2_COL_SPACING*3 },
				{ TRACK_2,	GAME_CONTROLLER_2,	EZ2_BUTTON_HANDLRLEFT,	-EZ2_COL_SPACING*2 },
				{ TRACK_3,	GAME_CONTROLLER_2,	EZ2_BUTTON_HANDUPLEFT,	-EZ2_COL_SPACING*1 }, 
				{ TRACK_4,	GAME_CONTROLLER_2,	EZ2_BUTTON_FOOTDOWN,	+EZ2_COL_SPACING*0 },
				{ TRACK_5,	GAME_CONTROLLER_2,	EZ2_BUTTON_HANDUPRIGHT,	+EZ2_COL_SPACING*1 }, 
				{ TRACK_6,	GAME_CONTROLLER_2,	EZ2_BUTTON_HANDLRRIGHT,	+EZ2_COL_SPACING*2 },
				{ TRACK_7,	GAME_CONTROLLER_2,	EZ2_BUTTON_FOOTUPRIGHT,	+EZ2_COL_SPACING*3 },
			},
		},
		{	// m_iColumnDrawOrder[MAX_COLS_PER_PLAYER];
			3,0,6,2,4,1,5 // This should be from back to front: Down, UpLeft, UpRight, Lower Left Hand, Lower Right Hand, Upper Left Hand, Upper Right Hand 
		},
	},
};


GameManager::GameManager()
{
}


GameDef* GameManager::GetGameDefForGame( Game g )
{
	ASSERT( g != GAME_INVALID );
	return &g_GameDefs[ g ];
}

const StyleDef* GameManager::GetStyleDefForStyle( Style s )
{
	ASSERT( s != STYLE_NONE );
	return &g_StyleDefs[ s ];
}

void GameManager::GetGameplayStylesForGame( Game game, CArray<Style,Style>& aStylesAddTo, bool editor )
{
	for( int s=0; s<NUM_STYLES; s++ ) {
		if( g_StyleDefs[s].m_Game != game)
			continue;
		if( !editor && g_StyleDefs[s].m_bUsedForGameplay )	
			continue;
		if( editor && !g_StyleDefs[s].m_bUsedForEdit )	
			continue;

		aStylesAddTo.Add( (Style)s );
	}
}

void GameManager::GetNotesTypesForGame( Game game, CArray<NotesType,NotesType>& aNotesTypeAddTo )
{
	for( int nt=0; nt<NUM_NOTES_TYPES; nt++ )
	{
		bool found = false;
		for( int s=0; !found && s<NUM_STYLES; s++ )
		{
			if( g_StyleDefs[s].m_Game != game )
				continue;
			for( int pl = 0; !found && pl < NUM_PLAYERS; ++pl)
			{
				if( g_StyleDefs[s].m_NotesTypes[pl] != nt )	
					continue;

				found=true;
			}
		}
		if(found)
			aNotesTypeAddTo.Add( (NotesType)nt );
	}
}

void GameManager::GetNoteSkinNames( CStringArray &AddTo )
{
	GAMESTATE->GetCurrentGameDef()->GetSkinNames( AddTo );
}

void GameManager::GetNoteSkinNames( Game game, CStringArray &AddTo )
{
	GetGameDefForGame(game)->GetSkinNames( AddTo );
}

bool GameManager::DoesNoteSkinExist( CString sSkinName )
{
	CStringArray asSkinNames;	
	GetNoteSkinNames( asSkinNames );
	for( int i=0; i<asSkinNames.GetSize(); i++ )
		if( 0==stricmp(sSkinName, asSkinNames[i]) )
			return true;
	return false;
}

void GameManager::SwitchNoteSkin( CString sNewNoteSkin )
{
	if( sNewNoteSkin == ""  ||  !DoesNoteSkinExist(sNewNoteSkin) )
	{
		CStringArray as;
		GetNoteSkinNames( as );
		m_sCurNoteSkin = as[0];
	}
	else
		m_sCurNoteSkin = sNewNoteSkin;
}

CString GameManager::GetPathTo( const int col, const SkinElement gbg )	// looks in GAMESTATE for the current Style
{
	const StyleDef* pStyleDef = GAMESTATE->GetCurrentStyleDef();
	const GameDef* pGameDef = GAMESTATE->GetCurrentGameDef();

	StyleInput SI( PLAYER_1, col );
	GameInput GI = pStyleDef->StyleInputToGameInput( SI );
	CString sButtonName = pGameDef->m_szButtonNames[GI.button];
	return pGameDef->GetPathToGraphic( m_sCurNoteSkin, sButtonName, gbg );
}

void GameManager::GetTapTweenColors( const int col, CArray<D3DXCOLOR,D3DXCOLOR> &aTapTweenColorsAddTo )	// looks in GAMESTATE for the current Style
{
	ASSERT( m_sCurNoteSkin != "" );	// if this == NULL, SwitchGame() was never called

	const StyleDef* pStyleDef = GAMESTATE->GetCurrentStyleDef();
	const GameDef* pGameDef = GAMESTATE->GetCurrentGameDef();

	StyleInput SI( PLAYER_1, col );
	GameInput GI = pStyleDef->StyleInputToGameInput( SI );
	CString sButtonName = pGameDef->m_szButtonNames[GI.button];
	pGameDef->GetTapTweenColors( m_sCurNoteSkin, sButtonName, aTapTweenColorsAddTo );
}

void GameManager::GetHoldTweenColors( const int col, CArray<D3DXCOLOR,D3DXCOLOR> &aHoldTweenColorsAddTo )	// looks in GAMESTATE for the current Style
{
	ASSERT( m_sCurNoteSkin != "" );	// if this == NULL, SwitchGame() was never called

	const StyleDef* pStyleDef = GAMESTATE->GetCurrentStyleDef();
	const GameDef* pGameDef = GAMESTATE->GetCurrentGameDef();

	StyleInput SI( PLAYER_1, col );
	GameInput GI = pStyleDef->StyleInputToGameInput( SI );
	CString sButtonName = pGameDef->m_szButtonNames[GI.button];
	pGameDef->GetHoldTweenColors( m_sCurNoteSkin, sButtonName, aHoldTweenColorsAddTo );
}

void GameManager::GetEnabledGames( CArray<Game,Game>& aGamesOut )
{
	for( int g=0; g<NUM_GAMES; g++ )
	{
		Game game = (Game)g;
		CStringArray asNoteSkins;
		GetNoteSkinNames( game, asNoteSkins );
		if( asNoteSkins.GetSize() > 0 )
			aGamesOut.Add( game );
	}
}

int GameManager::NotesTypeToNumTracks( NotesType nt )
{
	if(nt >= NUM_NOTES_TYPES)
	{
		// invalid NotesType
		ASSERT(0);
		return -1;
	}

	return NotesTypes[nt].NumTracks;
}

NotesType GameManager::StringToNotesType( CString sNotesType )
{
	sNotesType.MakeLower();
	for( int i=0; i<NUM_NOTES_TYPES; i++ )
		if( NotesTypes[i].name == sNotesType )
			return NotesType(i);
	
	// invalid NotesType
	ASSERT(0);
	return NOTES_TYPE_DANCE_SINGLE;
}

CString GameManager::NotesTypeToString( NotesType nt )
{
	if(nt >= NUM_NOTES_TYPES)
	{
		// invalid NotesType
		ASSERT(0);
		return "";
	}

	return NotesTypes[nt].name;
}

/* once we're sure the above lookups works, nuke this */
/*
int NotesTypeToNumTracks( NotesType nt )
{
	switch( nt )
	{
	case NOTES_TYPE_DANCE_SINGLE:		return 4;
	case NOTES_TYPE_DANCE_DOUBLE:		return 8;
	case NOTES_TYPE_DANCE_COUPLE:		return 8;
	case NOTES_TYPE_DANCE_SOLO:			return 6;
	case NOTES_TYPE_PUMP_SINGLE:		return 5;
	case NOTES_TYPE_PUMP_DOUBLE:		return 10;
	case NOTES_TYPE_PUMP_COUPLE:		return 10;
	case NOTES_TYPE_EZ2_SINGLE:			return 5; // Single: TL,LHH,D,RHH,TR
	case NOTES_TYPE_EZ2_SINGLE_HARD:	return 5; // Single: TL,LHH,D,RHH,TR
	case NOTES_TYPE_EZ2_DOUBLE:			return 10; // Double: Single x2
	case NOTES_TYPE_EZ2_REAL:			return 7; // Real: TL,LHH,LHL,D,RHL,RHH,TR
//	case NOTES_TYPE_EZ2_SINGLE_VERSUS:	return 10;	
//	case NOTES_TYPE_EZ2_SINGLE_HARD_VERSUS:		return 10; 
//	case NOTES_TYPE_EZ2_REAL_VERSUS:	return 10;
	default:	ASSERT(0);		return -1;	// invalid NotesType
	}
}

NotesType StringToNotesType( CString sNotesType )
{
	sNotesType.MakeLower();
	if     ( sNotesType == "dance-single" )	return NOTES_TYPE_DANCE_SINGLE;
	else if( sNotesType == "dance-double" )	return NOTES_TYPE_DANCE_DOUBLE;
	else if( sNotesType == "dance-couple" )	return NOTES_TYPE_DANCE_COUPLE;
	else if( sNotesType == "dance-solo" )	return NOTES_TYPE_DANCE_SOLO;
	else if( sNotesType == "pump-single" )	return NOTES_TYPE_PUMP_SINGLE;
	else if( sNotesType == "pump-double" )	return NOTES_TYPE_PUMP_DOUBLE;
	else if( sNotesType == "pump-couple" )	return NOTES_TYPE_PUMP_COUPLE;
	else if( sNotesType == "ez2-single" )	return NOTES_TYPE_EZ2_SINGLE;
	else if( sNotesType == "ez2-single-hard" )	return NOTES_TYPE_EZ2_SINGLE_HARD;
	else if( sNotesType == "ez2-double" )	return NOTES_TYPE_EZ2_DOUBLE;
	else if( sNotesType == "ez2-real" )		return NOTES_TYPE_EZ2_REAL;
// 	else if( sNotesType == "ez2-real-versus" )		return NOTES_TYPE_EZ2_REAL_VERSUS;
//	else if( sNotesType == "ez2-single-versus" )		return NOTES_TYPE_EZ2_SINGLE_VERSUS;
	else	ASSERT(0);	return NOTES_TYPE_DANCE_SINGLE;	// invalid NotesType
}

CString NotesTypeToString( NotesType nt )
{
	switch( nt )
	{
	case NOTES_TYPE_DANCE_SINGLE:	return "dance-single";
	case NOTES_TYPE_DANCE_DOUBLE:	return "dance-double";
	case NOTES_TYPE_DANCE_COUPLE:	return "dance-couple";
	case NOTES_TYPE_DANCE_SOLO:		return "dance-solo";
	case NOTES_TYPE_PUMP_SINGLE:	return "pump-single";
	case NOTES_TYPE_PUMP_DOUBLE:	return "pump-double";
	case NOTES_TYPE_PUMP_COUPLE:	return "pump-couple";
	case NOTES_TYPE_EZ2_SINGLE:		return "ez2-single";
	case NOTES_TYPE_EZ2_SINGLE_HARD:		return "ez2-single-hard";
	case NOTES_TYPE_EZ2_DOUBLE:		return "ez2-double";
	case NOTES_TYPE_EZ2_REAL:		return "ez2-real";
//	case NOTES_TYPE_EZ2_REAL_VERSUS:		return "ez2-real-versus";
//	case NOTES_TYPE_EZ2_SINGLE_VERSUS:		return "ez2-single-versus";
//	case NOTES_TYPE_EZ2_SINGLE_HARD_VERSUS:		return "ez2-single-hard-versus";
	default:	ASSERT(0);		return "";	// invalid NotesType
	}
}
*/
