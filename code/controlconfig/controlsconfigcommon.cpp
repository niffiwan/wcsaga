/*
 * Copyright (C) Volition, Inc. 1999.  All rights reserved.
 *
 * All source code herein is the property of Volition, Inc. You may not sell 
 * or otherwise commercially exploit the source or things you created based on the 
 * source.
 *
*/



#include "controlconfig/controlsconfig.h"
#include "io/key.h"
#include "io/joy.h"
#include "localization/localize.h"
#include "osapi/osapi.h"


#define TARGET_TAB			0
#define SHIP_TAB				1
#define WEAPON_TAB			2
#define COMPUTER_TAB			3

int Failed_key_index;

// assume control keys are used as modifiers until we find out 
int Shift_is_modifier;
int Ctrl_is_modifier;
int Alt_is_modifier;

int Axis_enabled[JOY_NUM_AXES] =
{
	1,
	1,
	1,
	0,
	0,
	0
};
int Axis_enabled_defaults[JOY_NUM_AXES] =
{
	1,
	1,
	1,
	0,
	0,
	0
};
int Invert_axis[JOY_NUM_AXES] =
{
	0,
	0,
	0,
	0,
	0,
	0
};
int Invert_axis_defaults[JOY_NUM_AXES] =
{
	0,
	0,
	0,
	0,
	0,
	0
};

// arrays which hold the key mappings.  The array index represents a key-independent action.
//
//XSTR:OFF
config_item Control_config[CCFG_MAX + 1] =
{
	// targeting a ship
	{
		KEY_T,
		-1,
		TARGET_TAB,
		true,
		"Target Next Ship"
	},
	{
		KEY_SHIFTED | KEY_T,
		-1,
		TARGET_TAB,
		true,
		"Target Previous Ship"
	},
	{
		KEY_H,
		2,
		TARGET_TAB,
		true,
		"Target Next Closest Hostile Ship"
	},
	{
		KEY_SHIFTED | KEY_H,
		-1,
		TARGET_TAB,
		true,
		"Target Previous Closest Hostile Ship"
	},
	{
		KEY_ALTED | KEY_H,
		-1,
		TARGET_TAB,
		true,
		"Toggle Auto Targeting"
	},
	{
		KEY_F,
		-1,
		TARGET_TAB,
		true,
		"Target Next Closest Friendly Ship"
	},
	{
		KEY_SHIFTED | KEY_F,
		-1,
		TARGET_TAB,
		true,
		"Target Previous Closest Friendly Ship"
	},
	{
		KEY_Y,
		4,
		TARGET_TAB,
		true,
		"Target Ship in Reticle"
	},
	{
		KEY_G,
		-1,
		TARGET_TAB,
		true,
		"Target Target's Nearest Attacker"
	},
	{
		KEY_ALTED | KEY_Y,
		-1,
		TARGET_TAB,
		true,
		"Target Last Ship to Send Transmission"
	},
	{
		KEY_ALTED | KEY_T,
		-1,
		TARGET_TAB,
		true,
		"Turn Off Targeting"
	},

	// targeting a ship's subsystem
	{
		KEY_V,
		-1,
		TARGET_TAB,
		true,
		"Target Subsystem in Reticle"
	},
	{
		KEY_S,
		-1,
		TARGET_TAB,
		true,
		"Target Next Subsystem"
	},
	{
		KEY_SHIFTED | KEY_S,
		-1,
		TARGET_TAB,
		true,
		"Target Previous Subsystem"
	},
	{
		KEY_ALTED | KEY_S,
		-1,
		TARGET_TAB,
		true,
		"Turn Off Targeting of Subsystems"
	},

	// matching speed
	{
		KEY_M,
		-1,
		COMPUTER_TAB,
		true,
		"Match Target Speed"
	},
	{
		KEY_ALTED | KEY_M,
		-1,
		COMPUTER_TAB,
		true,
		"Toggle Auto Speed Matching"
	},

	// weapons
	{
		KEY_LCTRL,
		0,
		WEAPON_TAB,
		true,
		"Fire Primary Weapon",
		CC_TYPE_CONTINUOUS
	},
	{
		KEY_SPACEBAR,
		1,
		WEAPON_TAB,
		true,
		"Fire Secondary Weapon",
		CC_TYPE_CONTINUOUS
	},
	{
		KEY_PERIOD,
		-1,
		WEAPON_TAB,
		true,
		"Cycle Forward Primary Weapon"
	},
	{
		KEY_COMMA,
		-1,
		WEAPON_TAB,
		true,
		"Cycle Backward Primary Weapon"
	},
	{
		KEY_DIVIDE,
		-1,
		WEAPON_TAB,
		true,
		"Cycle Secondary Weapon Bank"
	},
	{
		KEY_SHIFTED | KEY_DIVIDE,
		-1,
		WEAPON_TAB,
		true,
		"Cycle Secondary Weapon Firing Rate"
	},
	{
		KEY_X,
		3,
		WEAPON_TAB,
		true,
		"Launch Countermeasure"
	},

	// controls
	{
		KEY_A,
		-1,
		SHIP_TAB,
		true,
		"Forward Thrust",
		CC_TYPE_CONTINUOUS
	},
	{
		KEY_Z,
		-1,
		SHIP_TAB,
		true,
		"Reverse Thrust",
		CC_TYPE_CONTINUOUS
	},
	{
		KEY_PAD7,
		-1,
		SHIP_TAB,
		true,
		"Bank Left",
		CC_TYPE_CONTINUOUS
	},
	{
		KEY_PAD9,
		-1,
		SHIP_TAB,
		true,
		"Bank Right",
		CC_TYPE_CONTINUOUS
	},
	{
		KEY_PAD8,
		-1,
		SHIP_TAB,
		true,
		"Pitch Forward",
		CC_TYPE_CONTINUOUS
	},
	{
		KEY_PAD2,
		-1,
		SHIP_TAB,
		true,
		"Pitch Backward",
		CC_TYPE_CONTINUOUS
	},
	{
		KEY_PAD4,
		-1,
		SHIP_TAB,
		true,
		"Turn Left",
		CC_TYPE_CONTINUOUS
	},
	{
		KEY_PAD6,
		-1,
		SHIP_TAB,
		true,
		"Turn Right",
		CC_TYPE_CONTINUOUS
	},

	// throttle controls
	{
		KEY_BACKSP,
		-1,
		SHIP_TAB,
		true,
		"Set Throttle to Zero"
	},
	{
		KEY_SLASH,
		-1,
		SHIP_TAB,
		true,
		"Set Throttle to Max"
	},
	{
		KEY_LBRACKET,
		-1,
		SHIP_TAB,
		true,
		"Set Throttle to One-Third"
	},
	{
		KEY_RBRACKET,
		-1,
		SHIP_TAB,
		true,
		"Set Throttle to Two-Thirds"
	},
	{
		KEY_EQUAL,
		-1,
		SHIP_TAB,
		true,
		"Increase Throttle 5 Percent"
	},
	{
		KEY_MINUS,
		-1,
		SHIP_TAB,
		true,
		"Decrease Throttle 5 Percent"
	},

	// squadmate messaging
	{
		KEY_SHIFTED | KEY_A,
		-1,
		COMPUTER_TAB,
		true,
		"Attack My Target"
	},
	{
		KEY_SHIFTED | KEY_Z,
		-1,
		COMPUTER_TAB,
		true,
		"Disarm My Target"
	},
	{
		KEY_SHIFTED | KEY_D,
		-1,
		COMPUTER_TAB,
		true,
		"Disable My Target"
	},
	{
		KEY_SHIFTED | KEY_V,
		-1,
		COMPUTER_TAB,
		true,
		"Attack My Subsystem"
	},
	{
		KEY_SHIFTED | KEY_X,
		-1,
		COMPUTER_TAB,
		true,
		"Capture My Target"
	},
	{
		KEY_SHIFTED | KEY_E,
		-1,
		COMPUTER_TAB,
		true,
		"Engage Enemy"
	},
	{
		KEY_SHIFTED | KEY_W,
		-1,
		COMPUTER_TAB,
		true,
		"Form on My Wing"
	},
	{
		KEY_SHIFTED | KEY_I,
		-1,
		COMPUTER_TAB,
		true,
		"Ignore My Target"
	},
	{
		KEY_SHIFTED | KEY_P,
		-1,
		COMPUTER_TAB,
		true,
		"Protect My Target"
	},
	{
		KEY_SHIFTED | KEY_C,
		-1,
		COMPUTER_TAB,
		true,
		"Cover Me"
	},
	{
		KEY_SHIFTED | KEY_J,
		-1,
		COMPUTER_TAB,
		true,
		"Return to Base"
	},
	{
		KEY_SHIFTED | KEY_R,
		-1,
		COMPUTER_TAB,
		true,
		"Rearm Me"
	},

	{
		KEY_R,
		6,
		TARGET_TAB,
		true,
		"Target Closest Attacking Ship"
	},

	// Views
	{
		KEY_PADMULTIPLY,
		-1,
		COMPUTER_TAB,
		true,
		"Chase View"
	},
	{
		KEY_PADPERIOD,
		-1,
		COMPUTER_TAB,
		true,
		"External View"
	},
	{
		KEY_PADENTER,
		-1,
		COMPUTER_TAB,
		true,
		"Toggle External Camera Lock"
	},
	{
		KEY_PAD0,
		-1,
		COMPUTER_TAB,
		true,
		"Free Look View",
		CC_TYPE_CONTINUOUS
	}, // Not in use anymore (Swifty)
	{
		KEY_PADDIVIDE,
		-1,
		COMPUTER_TAB,
		true,
		"Current Target View"
	},
	{
		KEY_PADPLUS,
		-1,
		COMPUTER_TAB,
		true,
		"Increase View Distance",
		CC_TYPE_CONTINUOUS
	},
	{
		KEY_PADMINUS,
		-1,
		COMPUTER_TAB,
		true,
		"Decrease View Distance",
		CC_TYPE_CONTINUOUS
	},
	{
		KEY_PAD5,
		-1,
		COMPUTER_TAB,
		true,
		"Center View",
		CC_TYPE_CONTINUOUS
	},
	{
		-1,
		33,
		COMPUTER_TAB,
		true,
		"View Up",
		CC_TYPE_CONTINUOUS
	},
	{
		-1,
		32,
		COMPUTER_TAB,
		true,
		"View Rear",
		CC_TYPE_CONTINUOUS
	},
	{
		-1,
		34,
		COMPUTER_TAB,
		true,
		"View Left",
		CC_TYPE_CONTINUOUS
	},
	{
		-1,
		35,
		COMPUTER_TAB,
		true,
		"View Right",
		CC_TYPE_CONTINUOUS
	},

	{
		KEY_RAPOSTRO,
		-1,
		COMPUTER_TAB,
		true,
		"Cycle Radar Range"
	},
	{
		KEY_C,
		-1,
		COMPUTER_TAB,
		true,
		"Communications Menu"
	},
	{
		-1,
		-1,
		-1,
		true,
		"Show Objectives"
	},
	{
		KEY_ALTED | KEY_J,
		-1,
		COMPUTER_TAB,
		true,
		"Enter Subspace (End Mission)"
	},
	{
		KEY_J,
		-1,
		TARGET_TAB,
		true,
		"Target Target's Target"
	},
	{
		KEY_TAB,
		5,
		SHIP_TAB,
		true,
		"Afterburner",
		CC_TYPE_CONTINUOUS
	},

	{
		KEY_INSERT,
		-1,
		COMPUTER_TAB,
		true,
		"Increase Weapon Energy"
	},
	{
		KEY_DELETE,
		-1,
		COMPUTER_TAB,
		true,
		"Decrease Weapon Energy"
	},
	{
		KEY_HOME,
		-1,
		COMPUTER_TAB,
		true,
		"Increase Shield Energy"
	},
	{
		KEY_END,
		-1,
		COMPUTER_TAB,
		true,
		"Decrease Shield Energy"
	},
	{
		KEY_PAGEUP,
		-1,
		COMPUTER_TAB,
		true,
		"Increase Engine Energy"
	},
	{
		KEY_PAGEDOWN,
		-1,
		COMPUTER_TAB,
		true,
		"Decrease Engine Energy"
	},
	{
		KEY_ALTED | KEY_D,
		-1,
		COMPUTER_TAB,
		true,
		"Equalize Energy Settings"
	},

	{
		KEY_Q,
		7,
		COMPUTER_TAB,
		true,
		"Equalize Shields"
	},
	{
		KEY_UP,
		-1,
		COMPUTER_TAB,
		true,
		"Augment Forward Shield"
	},
	{
		KEY_DOWN,
		-1,
		COMPUTER_TAB,
		true,
		"Augment Rear Shield"
	},
	{
		KEY_LEFT,
		-1,
		COMPUTER_TAB,
		true,
		"Augment Left Shield"
	},
	{
		KEY_RIGHT,
		-1,
		COMPUTER_TAB,
		true,
		"Augment Right Shield"
	},
	{
		KEY_SCROLLOCK,
		-1,
		COMPUTER_TAB,
		true,
		"Transfer Energy Laser->Shield"
	},
	{
		KEY_SHIFTED | KEY_SCROLLOCK,
		-1,
		COMPUTER_TAB,
		true,
		"Transfer Energy Shield->Laser"
	},
	//	{                           -1,					-1, -1,				true, "Show Damage Popup Window" },	

	{
		-1,
		-1,
		SHIP_TAB,
		false,
		"Glide When Pressed",
		CC_TYPE_CONTINUOUS
	},
	//Backslash -- this was a convenient place for Glide When Pressed, as Show Damage Popup isn't used
	{
		-1,
		-1,
		SHIP_TAB,
		true,
		"Bank When Pressed",
		CC_TYPE_CONTINUOUS
	},
	{
		-1,
		-1,
		-1,
		true,
		"Show Nav Map"
	},
	{
		KEY_ALTED | KEY_E,
		-1,
		COMPUTER_TAB,
		true,
		"Add or Remove Escort"
	},
	{
		KEY_ALTED | KEY_SHIFTED | KEY_E,
		-1,
		COMPUTER_TAB,
		true,
		"Clear Escort List"
	},
	{
		KEY_E,
		-1,
		TARGET_TAB,
		true,
		"Target Next Escort Ship"
	},
	{
		KEY_ALTED | KEY_R,
		-1,
		TARGET_TAB,
		true,
		"Target Closest Repair Ship"
	},

	{
		KEY_U,
		-1,
		TARGET_TAB,
		true,
		"Target Next Uninspected Cargo"
	},
	{
		KEY_SHIFTED | KEY_U,
		-1,
		TARGET_TAB,
		true,
		"Target Previous Uninspected Cargo"
	},
	{
		KEY_N,
		-1,
		TARGET_TAB,
		true,
		"Target Newest Ship in Area"
	},
	{
		KEY_K,
		-1,
		TARGET_TAB,
		true,
		"Target Next Live Turret"
	},
	{
		KEY_SHIFTED | KEY_K,
		-1,
		TARGET_TAB,
		true,
		"Target Previous Live Turret"
	},

	{
		KEY_B,
		-1,
		TARGET_TAB,
		true,
		"Target Next Hostile Bomb or Bomber"
	},
	{
		KEY_SHIFTED | KEY_B,
		-1,
		TARGET_TAB,
		true,
		"Target Previous Hostile Bomb or Bomber"
	},

	// multiplayer messaging keys
	{
		KEY_1,
		-1,
		COMPUTER_TAB,
		true,
		"(Multiplayer) Message All",
		CC_TYPE_CONTINUOUS
	},
	{
		KEY_2,
		-1,
		COMPUTER_TAB,
		true,
		"(Multiplayer) Message Friendly",
		CC_TYPE_CONTINUOUS
	},
	{
		KEY_3,
		-1,
		COMPUTER_TAB,
		true,
		"(Multiplayer) Message Hostile",
		CC_TYPE_CONTINUOUS
	},
	{
		KEY_4,
		-1,
		COMPUTER_TAB,
		true,
		"(Multiplayer) Message Target",
		CC_TYPE_CONTINUOUS
	},
	{
		KEY_ALTED | KEY_X,
		-1,
		COMPUTER_TAB,
		true,
		"(Multiplayer) Observer Zoom to Target"
	},
	{
		KEY_SHIFTED | KEY_PERIOD,
		-1,
		COMPUTER_TAB,
		true,
		"Increase Time Compression"
	},
	{
		KEY_SHIFTED | KEY_COMMA,
		-1,
		COMPUTER_TAB,
		true,
		"Decrease Time Compression"
	},
	{
		KEY_L,
		-1,
		COMPUTER_TAB,
		true,
		"Toggle High HUD Contrast"
	},
	{
		KEY_SHIFTED | KEY_N,
		-1,
		COMPUTER_TAB,
		true,
		"(Multiplayer) Toggle Network Info"
	},
	{
		KEY_SHIFTED | KEY_END,
		-1,
		COMPUTER_TAB,
		true,
		"(Multiplayer) Self Destruct"
	},

	// Misc
	{
		KEY_SHIFTED | KEY_O,
		-1,
		COMPUTER_TAB,
		true,
		"Toggle HUD"
	},
	{
		KEY_SHIFTED | KEY_3,
		-1,
		SHIP_TAB,
		true,
		"Right Thrust",
		CC_TYPE_CONTINUOUS
	},
	{
		KEY_SHIFTED | KEY_1,
		-1,
		SHIP_TAB,
		true,
		"Left Thrust",
		CC_TYPE_CONTINUOUS
	},
	{
		KEY_SHIFTED | KEY_PADPLUS,
		-1,
		SHIP_TAB,
		true,
		"Up Thrust",
		CC_TYPE_CONTINUOUS
	},
	{
		KEY_SHIFTED | KEY_PADENTER,
		-1,
		SHIP_TAB,
		true,
		"Down Thrust",
		CC_TYPE_CONTINUOUS
	},
	{
		KEY_ALTED | KEY_SHIFTED | KEY_Q,
		-1,
		COMPUTER_TAB,
		true,
		"Toggle HUD Wireframe Target View"
	},
	{
		-1,
		-1,
		COMPUTER_TAB,
		false,
		"Top-Down View"
	},
	{
		-1,
		-1,
		COMPUTER_TAB,
		false,
		"Target Padlock View"
	}, // (Swifty) Toggle for VM_TRACK
	// Auto Navigation Systen
	{
		KEY_ALTED | KEY_A,
		-1,
		COMPUTER_TAB,
		false,
		"Toggle Auto Pilot"
	},
	{
		KEY_ALTED | KEY_N,
		-1,
		COMPUTER_TAB,
		false,
		"Cycle Nav Points"
	},
	{
		KEY_ALTED | KEY_G,
		-1,
		SHIP_TAB,
		false,
		"Toggle Gliding"
	},
	{
		-1,
		-1,
		-1,
		false,
		""
	}
};

char* Scan_code_text_german[] =
{
	"",
	"Esc",
	"1",
	"2",
	"3",
	"4",
	"5",
	"6",
	"7",
	"8",
	"9",
	"0",
	"Sharp S",
	"Accute Accent",
	"Backspace",
	"Tab",
	"Q",
	"W",
	"E",
	"R",
	"T",
	"Z",
	"U",
	"I",
	"O",
	"P",
	"Umlaut U",
	"+",
	"Enter",
	"Left Ctrl",
	"A",
	"S",

	"D",
	"F",
	"G",
	"H",
	"J",
	"K",
	"L",
	"Umlaut O",
	"Umlaut A",
	"Grave Accent",
	"Shift",
	"#",
	"Y",
	"X",
	"C",
	"V",
	"B",
	"N",
	"M",
	",",
	".",
	"-",
	"Shift",
	"Pad *",
	"Alt",
	"Spacebar",
	"Caps Lock",
	"F1",
	"F2",
	"F3",
	"F4",
	"F5",

	"F6",
	"F7",
	"F8",
	"F9",
	"F10",
	"Pause",
	"Scroll Lock",
	"Pad 7",
	"Pad 8",
	"Pad 9",
	"Pad -",
	"Pad 4",
	"Pad 5",
	"Pad 6",
	"Pad +",
	"Pad 1",
	"Pad 2",
	"Pad 3",
	"Pad 0",
	"Pad ,",
	"",
	"",
	"<",
	"F11",
	"F12",
	"",
	"",
	"",
	"",
	"",
	"",
	"",

	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",

	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"Pad Enter",
	"Right Ctrl",
	"",
	"",

	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"Pad /",
	"",
	"Print Scrn",
	"Alt",
	"",
	"",
	"",
	"",
	"",
	"",
	"",

	"",
	"",
	"",
	"",
	"",
	"Num Lock",
	"",
	"Home",
	"Up Arrow",
	"Page Up",
	"",
	"Left Arrow",
	"",
	"Right Arrow",
	"",
	"End",
	"Down Arrow",
	"Page Down",
	"Insert",
	"Delete",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",

	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
};

char* Joy_button_text_german[] =
{
	"Button 1",
	"Button 2",
	"Button 3",
	"Button 4",
	"Button 5",
	"Button 6",
	"Button 7",
	"Button 8",
	"Button 9",
	"Button 10",
	"Button 11",
	"Button 12",
	"Button 13",
	"Button 14",
	"Button 15",
	"Button 16",
	"Button 17",
	"Button 18",
	"Button 19",
	"Button 20",
	"Button 21",
	"Button 22",
	"Button 23",
	"Button 24",
	"Button 25",
	"Button 26",
	"Button 27",
	"Button 28",
	"Button 29",
	"Button 30",
	"Button 31",
	"Button 32",
	"Hat Back",
	"Hat Forward",
	"Hat Left",
	"Hat Right"
};

char* Scan_code_text_french[] =
{
	"",
	"Esc",
	"1",
	"2",
	"3",
	"4",
	"5",
	"6",
	"7",
	"8",
	"9",
	"0",
	"-",
	"=",
	"Backspace",
	"Tab",
	"Q",
	"W",
	"E",
	"R",
	"T",
	"Y",
	"U",
	"I",
	"O",
	"P",
	"[",
	"]",
	"Enter",
	"Left Ctrl",
	"A",
	"S",

	"D",
	"F",
	"G",
	"H",
	"J",
	"K",
	"L",
	";",
	"'",
	"Grave Accent",
	"Shift",
	"\\",
	"Z",
	"X",
	"C",
	"V",
	"B",
	"N",
	"M",
	",",
	".",
	"/",
	"Shift",
	"Pad *",
	"Alt",
	"Spacebar",
	"Caps Lock",
	"F1",
	"F2",
	"F3",
	"F4",
	"F5",

	"F6",
	"F7",
	"F8",
	"F9",
	"F10",
	"Pause",
	"Scroll Lock",
	"Pad 7",
	"Pad 8",
	"Pad 9",
	"Pad -",
	"Pad 4",
	"Pad 5",
	"Pad 6",
	"Pad +",
	"Pad 1",
	"Pad 2",
	"Pad 3",
	"Pad 0",
	"Pad .",
	"",
	"",
	"<",
	"F11",
	"F12",
	"",
	"",
	"",
	"",
	"",
	"",
	"",

	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",

	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"Pad Enter",
	"Right Ctrl",
	"",
	"",

	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"Pad /",
	"",
	"Print Scrn",
	"Alt",
	"",
	"",
	"",
	"",
	"",
	"",
	"",

	"",
	"",
	"",
	"",
	"",
	"Num Lock",
	"",
	"Home",
	"Up Arrow",
	"Page Up",
	"",
	"Left Arrow",
	"",
	"Right Arrow",
	"",
	"End",
	"Down Arrow",
	"Page Down",
	"Insert",
	"Delete",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",

	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
};

char* Joy_button_text_french[] =
{
	"Button 1",
	"Button 2",
	"Button 3",
	"Button 4",
	"Button 5",
	"Button 6",
	"Button 7",
	"Button 8",
	"Button 9",
	"Button 10",
	"Button 11",
	"Button 12",
	"Button 13",
	"Button 14",
	"Button 15",
	"Button 16",
	"Button 17",
	"Button 18",
	"Button 19",
	"Button 20",
	"Button 21",
	"Button 22",
	"Button 23",
	"Button 24",
	"Button 25",
	"Button 26",
	"Button 27",
	"Button 28",
	"Button 29",
	"Button 30",
	"Button 31",
	"Button 32",
	"Hat Back",
	"Hat Forward",
	"Hat Left",
	"Hat Right"
};

//	This is the text that is displayed on the screen for the keys a player selects
char* Scan_code_text_english[] =
{
	"",
	"Esc",
	"1",
	"2",
	"3",
	"4",
	"5",
	"6",
	"7",
	"8",
	"9",
	"0",
	"-",
	"=",
	"Backspace",
	"Tab",
	"Q",
	"W",
	"E",
	"R",
	"T",
	"Y",
	"U",
	"I",
	"O",
	"P",
	"[",
	"]",
	"Enter",
	"Left Ctrl",
	"A",
	"S",

	"D",
	"F",
	"G",
	"H",
	"J",
	"K",
	"L",
	";",
	"'",
	"`",
	"Shift",
	"\\",
	"Z",
	"X",
	"C",
	"V",
	"B",
	"N",
	"M",
	",",
	".",
	"/",
	"Shift",
	"Pad *",
	"Alt",
	"Spacebar",
	"Caps Lock",
	"F1",
	"F2",
	"F3",
	"F4",
	"F5",

	"F6",
	"F7",
	"F8",
	"F9",
	"F10",
	"Pause",
	"Scroll Lock",
	"Pad 7",
	"Pad 8",
	"Pad 9",
	"Pad -",
	"Pad 4",
	"Pad 5",
	"Pad 6",
	"Pad +",
	"Pad 1",
	"Pad 2",
	"Pad 3",
	"Pad 0",
	"Pad .",
	"",
	"",
	"",
	"F11",
	"F12",
	"",
	"",
	"",
	"",
	"",
	"",
	"",

	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",

	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"Pad Enter",
	"Right Ctrl",
	"",
	"",

	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"Pad /",
	"",
	"Print Scrn",
	"Alt",
	"",
	"",
	"",
	"",
	"",
	"",
	"",

	"",
	"",
	"",
	"",
	"",
	"Num Lock",
	"",
	"Home",
	"Up Arrow",
	"Page Up",
	"",
	"Left Arrow",
	"",
	"Right Arrow",
	"",
	"End",
	"Down Arrow",
	"Page Down",
	"Insert",
	"Delete",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",

	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
};

char* Joy_button_text_english[] =
{
	"Button 1",
	"Button 2",
	"Button 3",
	"Button 4",
	"Button 5",
	"Button 6",
	"Button 7",
	"Button 8",
	"Button 9",
	"Button 10",
	"Button 11",
	"Button 12",
	"Button 13",
	"Button 14",
	"Button 15",
	"Button 16",
	"Button 17",
	"Button 18",
	"Button 19",
	"Button 20",
	"Button 21",
	"Button 22",
	"Button 23",
	"Button 24",
	"Button 25",
	"Button 26",
	"Button 27",
	"Button 28",
	"Button 29",
	"Button 30",
	"Button 31",
	"Button 32",
	"Hat Back",
	"Hat Forward",
	"Hat Left",
	"Hat Right"
};

char** Scan_code_text = Scan_code_text_english;
char** Joy_button_text = Joy_button_text_english;

void set_modifier_status()
{
	int i;

	Alt_is_modifier = 0;
	Shift_is_modifier = 0;
	Ctrl_is_modifier = 0;

	for (i = 0; i < CCFG_MAX; i++)
	{
		if (Control_config[i].key_id < 0)
			continue;

		if (Control_config[i].key_id & KEY_ALTED)
			Alt_is_modifier = 1;

		if (Control_config[i].key_id & KEY_SHIFTED)
			Shift_is_modifier = 1;

		if (Control_config[i].key_id & KEY_CTRLED)
		{
			Assert(0);  // get Alan
			Ctrl_is_modifier = 1;
		}
	}
}

int translate_key_to_index(char* key)
{
	int i, index = -1, alt = 0, shift = 0, max_scan_codes;

	if (Lcl_gr)
	{
		max_scan_codes = sizeof(Scan_code_text_german) / sizeof(char*);
	}
	else if (Lcl_fr)
	{
		max_scan_codes = sizeof(Scan_code_text_french) / sizeof(char*);
	}
	else
	{
		max_scan_codes = sizeof(Scan_code_text_english) / sizeof(char*);
	}

	// look for modifiers
	Assert(key);
	if (!strnicmp(key, "Alt", 3))
	{
		alt = 1;
		key += 3;
		if (*key)
			key++;
	}

	char* translated_shift;

	if (Lcl_gr)
	{
		translated_shift = "Shift";
	}
	else if (Lcl_fr)
	{
		translated_shift = "Maj.";
	}
	else
	{
		translated_shift = "Shift";
	}

	if (!strnicmp(key, translated_shift, 5))
	{
		shift = 1;
		key += 5;
		if (*key)
			key++;
	}

	// look up index for default key
	if (*key)
	{
		for (i = 0; i < max_scan_codes; i++)
			if (!stricmp(key, Scan_code_text_english[i]))
			{
				index = i;
				break;
			}

		if (i == max_scan_codes)
			return -1;

		if (shift)
			index |= KEY_SHIFTED;
		if (alt)
			index |= KEY_ALTED;

		// convert scancode to Control_config index
		for (i = 0; i < CCFG_MAX; i++)
		{
			if (Control_config[i].key_default == index)
			{
				index = i;
				break;
			}
		}

		if (i == CCFG_MAX)
			return -1;

		return index;
	}

	return -1;
}

// Given the system default key 'key', return the current key that is bound to the function
// Both are 'key' and the return value are descriptive strings that can be displayed
// directly to the user.  If 'key' isn't a real key or not normally bound to anything,
// or there is no key current bound to the function, NULL is returned.
char* translate_key(char* key)
{
	int index = -1, code = -1;

	index = translate_key_to_index(key);
	if (index < 0)
		return NULL;

	code = Control_config[index].key_id;
	Failed_key_index = index;
	if (code < 0)
	{
		code = Control_config[index].joy_id;
		if (code >= 0)
			return Joy_button_text[code];
	}

	return textify_scancode(code);
}

char* textify_scancode(int code)
{
	static char text[40];

	if (code < 0)
		return "None";

	int keycode = code & KEY_MASK;

	*text = 0;
	if (code & KEY_ALTED && !(keycode == KEY_LALT || keycode == KEY_RALT))
	{
		if (Lcl_gr)
		{
			strcat_s(text, "Alt-");
		}
		else if (Lcl_fr)
		{
			strcat_s(text, "Alt-");
		}
		else
		{
			strcat_s(text, "Alt-");
		}
	}

	if (code & KEY_SHIFTED && !(keycode == KEY_LSHIFT || keycode == KEY_RSHIFT))
	{
		if (Lcl_gr)
		{
			strcat_s(text, "Shift-");
		}
		else if (Lcl_fr)
		{
			strcat_s(text, "Maj.-");
		}
		else
		{
			strcat_s(text, "Shift-");
		}
	}

	strcat_s(text, Scan_code_text[keycode]);
	return text;
}
//XSTR:ON

// initialize common control config stuff - call at game startup after localization has been initialized
void control_config_common_init()
{
	int layout = get_keyboad_layout();

	if (layout == LCL_GERMAN)
	{
		Scan_code_text = Scan_code_text_german;
		Joy_button_text = Joy_button_text_german;

		// swap init bindings for y and z keys
		Control_config[TARGET_SHIP_IN_RETICLE].key_default = KEY_Z;
		Control_config[TARGET_LAST_TRANMISSION_SENDER].key_default = KEY_ALTED | KEY_Z;
		Control_config[REVERSE_THRUST].key_default = KEY_Y;
		Control_config[DISARM_MESSAGE].key_default = KEY_SHIFTED | KEY_Y;
	}
	else if (LCL_ENGLISH == LCL_FRENCH)
	{
		Scan_code_text = Scan_code_text_french;
		Joy_button_text = Joy_button_text_french;
	}
	else
	{
		Scan_code_text = Scan_code_text_english;
		Joy_button_text = Joy_button_text_english;
	}
}
