/*
 *  sscanf 2.11.2
 *
 *  Version: MPL 1.1
 *
 *  The contents of this file are subject to the Mozilla Public License Version
 *  1.1 (the "License"); you may not use this file except in compliance with
 *  the License. You may obtain a copy of the License at
 *  http://www.mozilla.org/MPL/
 *
 *  Software distributed under the License is distributed on an "AS IS" basis,
 *  WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 *  for the specific language governing rights and limitations under the
 *  License.
 *
 *  The Original Code is the sscanf 2.0 SA:MP plugin.
 *
 *  The Initial Developer of the Original Code is Alex "Y_Less" Cole.
 *  Portions created by the Initial Developer are Copyright (C) 2020
 *  the Initial Developer. All Rights Reserved.
 *
 *  Contributor(s):
 *
 *      Cheaterman
 *      DEntisT
 *      Emmet_
 *      karimcambridge
 *      leHeix
 *      maddinat0r
 *      Southclaws
 *      Y_Less
 *      ziggi
 *
 *  Special Thanks to:
 *
 *      SA:MP Team past, present, and future.
 *      maddinat0r, for hosting the repo for a very long time.
 *      Emmet_, for his efforts in maintaining it for almost a year.
 */

#if !defined _INC_SSCANF
	#define _INC_SSCANF
#else
	#endinput
#endif

#if !defined _samp_included
	#error Please include <a_npc> or <a_samp> first.
#endif

#if defined GetDistanceFromMeToPoint
	// NPC script.
	#define SSCANF_NPC (1)
	#pragma library sscanf
#else
	#define SSCANF_NPC (0)
#endif

#if !defined __PawnBuild && !defined SSCANF_NO_NICE_FEATURES
	#error sscanf likes the community compiler.  Use `#define SSCANF_NO_NICE_FEATURES` to ignore this.
#endif

#define SSCANF_VERSION_MAJOR  2
#define SSCANF_VERSION_MINOR 11
#define SSCANF_VERSION_BUILD  2

#define SSCANF_VERSION_STRING #SSCANF_VERSION_MAJOR "." #SSCANF_VERSION_MINOR "." #SSCANF_VERSION_BUILD

// Example:
//
//         2.10.3
//     => 02 10 03
//     => 0x021003
//
const SSCANF_VERSION =
	((SSCANF_VERSION_MAJOR / 10) << 20) |
	((SSCANF_VERSION_MAJOR % 10) << 16) |
	((SSCANF_VERSION_MINOR / 10) << 12) |
	((SSCANF_VERSION_MINOR % 10) <<  8) |
	((SSCANF_VERSION_BUILD / 10) <<  4) |
	((SSCANF_VERSION_BUILD % 10) <<  0) ;

stock const SSCANF_VERSION_BCD = SSCANF_VERSION;

#define SSCANF:%0(%1) forward sscanf_%0(%1);public sscanf_%0(%1)

#if defined sscanf
	#error sscanf (possibly the PAWN version) already defined.
#endif

native SSCANF__(const file[], const line, const data[], const format[], {Float, _}:...);
native UNFORMAT__(const file[], const line, const data[], const format[], {Float, _}:...) = SSCANF__;
native SSCANF_Init(players, invalid, len);
native SSCANF_Join(playerid, const name[], npc);
native SSCANF_Leave(playerid);
native SSCANF_IsConnected(playerid);

// Get a global SSCANF setting/option.
native SSCANF_GetOption(const name[]) = SSCANF_Option;
native SSCANF_GetOption__(const name[]) = SSCANF_Option;

// Set a global SSCANF setting/option.
native SSCANF_SetOption(const name[], value) = SSCANF_Option;
native SSCANF_SetOption__(const name[], value) = SSCANF_Option;

// Backwards-compatibility with `SSCANF_Option`.  Usage:
//
//     SSCANF_Option(SSCANF_QUIET, 1);
//     new quiet = SSCANF_Option(SSCANF_QUIET);
//
// This can't just use a default parameter because a user may want to set the
// option to whatever that parameter is.
#define SSCANF_Option SSCANF_GetOption__
#define SSCANF_GetOption__(%0,%1) SSCANF_SetOption__(%0,%1)
#define _ALS_SSCANF_Option
#define _ALS_SSCANF_GetOption__

// Get the sscanf plugin version.
native SSCANF_VersionString(version[], size = sizeof (version)) = SSCANF_Version;
native SSCANF_VersionString__(version[], size = sizeof (version)) = SSCANF_Version;

native SSCANF_VersionBCD() = SSCANF_Version;
native SSCANF_VersionBCD__() = SSCANF_Version;

// The `SSCANF_Version` function is overloaded to take 0 or 2 parameters (or 1 plus a default).
#define SSCANF_Version SSCANF_VersionString__
#define SSCANF_VersionString__() SSCANF_VersionBCD__()
#define _ALS_SSCANF_Version
#define _ALS_SSCANF_VersionString__

#if defined __PawnBuild
	#define sscanf( SSCANF__(__file,__line,
	#define unformat( SSCANF__(__file,__line,
#else
	#define sscanf( SSCANF__(SSCANF_UNK_,-1,
	#define unformat( SSCANF__(SSCANF_FOM_,-1,
#endif

stock const
	SSCANF_UNK_[] = "sscanf",   // Append "unknown file" in the plugin when line number < 0.
	SSCANF_FOM_[] = "unformat", // Append "unknown file" in the plugin when line number < 0.
	SSCANF_EXT_[] = "extract",  // Append "unknown file" in the plugin when line number < 0.
	SSCANF_QUIET[] = "SSCANF_QUIET",
	OLD_DEFAULT_NAME[] = "OLD_DEFAULT_NAME",
	MATCH_NAME_PARTIAL[] = "MATCH_NAME_PARTIAL",
	CELLMIN_ON_MATCHES[] = "CELLMIN_ON_MATCHES",
	OLD_DEFAULT_KUSTOM[] = "OLD_DEFAULT_KUSTOM",
	OLD_DEFAULT_CUSTOM[] = "OLD_DEFAULT_CUSTOM",
	SSCANF_COLOUR_FORMS[] = "SSCANF_COLOUR_FORMS",
	SSCANF_ALPHA[] = "SSCANF_ALPHA",
	SSCANF_ARGB[] = "SSCANF_ARGB";

static stock
	bool:SSCANF_gInit = false;

// Pre-hook data.
#if !defined CHAIN_ORDER
	#define CHAIN_ORDER() 0
#endif

#define CHAIN_HOOK(%0) forward @CO_%0();public @CO_%0(){return CHAIN_ORDER()+1;}
#define CHAIN_NEXT(%0) @CO_%0

#define CHAIN_FORWARD:%0_%2(%1)=%3; \
	forward %0_%2(%1); \
	public %0_%2(%1) <_ALS : _ALS_x0, _ALS : _ALS_x1> { return (%3); } \
	public %0_%2(%1) <> { return (%3); }

#define CHAIN_PUBLIC:%0(%1) %0(%1) <_ALS : _ALS_go>

CHAIN_HOOK(SSCANF)
#undef CHAIN_ORDER
#define CHAIN_ORDER CHAIN_NEXT(SSCANF)

static stock _SSCANF_IncludeStates() <_ALS : _ALS_x0, _ALS : _ALS_x1, _ALS : _ALS_x2, _ALS : _ALS_x3> {}
static stock _SSCANF_IncludeStates() <_ALS : _ALS_go> {}

#if SSCANF_NPC
	forward SSCANF_PlayerCheck();

	/*
	  OnNPCModeInit

	  Called when the script starts if it is a NPC mode, sets up the system,
	  then calls the "real" OnNPCModeInit (using the new ALS 2 hook method).
	*/

	native SSCANF_SetTimer(const funcname[], interval, bool:repeating) = SetTimer;

	public OnNPCModeInit()
	{
		state _ALS : _ALS_go;
		SSCANF_Init(MAX_PLAYERS, INVALID_PLAYER_ID, MAX_PLAYER_NAME);
		#if !defined SSCANF_NO_PLAYERS
			// Initialise the system.
			SSCANF_PlayerCheck();
			// Will be run once per frame.
			SSCANF_SetTimer("SSCANF_PlayerCheck", 0, true);
		#endif
		SSCANF_OnNPCModeInit();
		return 1;
	}

	#if defined _ALS_OnNPCModeInit
		#undef OnNPCModeInit
	#else
		#define _ALS_OnNPCModeInit
	#endif
	#define OnNPCModeInit(%0) CHAIN_PUBLIC:SSCANF_OnNPCModeInit(%0)
	CHAIN_FORWARD:SSCANF_OnNPCModeInit() = 1;

	/*
	  SSCANF_PlayerCheck

	  NPC modes have no "OnPlayerConnect callback, so we need to simulate one.
	*/

	#if !defined SSCANF_NO_PLAYERS
		public SSCANF_PlayerCheck()
		{
			for (new i = 0; i != MAX_PLAYERS; ++i)
			{
				if (IsPlayerConnected(i))
				{
					if (!SSCANF_IsConnected(i))
					{
						new
							name[MAX_PLAYER_NAME + 1];
						GetPlayerName(i, name, sizeof (name));
						// We have no way to know if they are an NPC or not!
						SSCANF_Join(i, name, 0);
					}
				}
				else
				{
					if (SSCANF_IsConnected(i))
					{
						SSCANF_Leave(i);
					}
				}
			}
		}
	#endif
#else
	static stock SSCANF_RunInit()
	{
		state _ALS : _ALS_go;
		new
			name[MAX_PLAYER_NAME + 1];

		SSCANF_Init(GetMaxPlayers(), INVALID_PLAYER_ID, MAX_PLAYER_NAME);
		SSCANF_gInit = true;

		// Check if there are any players that aren't initialized.
		for (new i = 0; i < MAX_PLAYERS; i ++)
		{
			if (IsPlayerConnected(i) && !SSCANF_IsConnected(i))
			{
				GetPlayerName(i, name, sizeof (name));
				SSCANF_Join(i, name, IsPlayerNPC(i));
			}
		}
	}

	/*
	  OnScriptInit

	  Called for earlier initialisation by YSI.
	*/

	#define SSCANF_BRACKETS ()
	forward OnScriptInit SSCANF_BRACKETS;
	#undef SSCANF_BRACKETS

	public OnScriptInit()
	{
		if (!SSCANF_gInit)
		{
			SSCANF_RunInit();
		}
		SSCANF_OnScriptInit();
		return 1;
	}

	#if defined _ALS_OnScriptInit
		#undef OnScriptInit
	#else
		#define _ALS_OnScriptInit
	#endif
	#define OnScriptInit(%0) CHAIN_PUBLIC:SSCANF_OnScriptInit(%0)
	CHAIN_FORWARD:SSCANF_OnScriptInit() = 1;

	/*
	  OnFilterScriptInit

	  Called when the script starts if it is a filterscript, sets up the system,
	  then calls the "real" OnFilterScriptInit (using the new ALS 2 hook
	  method).
	*/

	public OnFilterScriptInit()
	{
		if (!SSCANF_gInit)
		{
			SSCANF_RunInit();
		}
		SSCANF_OnFilterScriptInit();
		return 1;
	}

	#if defined _ALS_OnFilterScriptInit
		#undef OnFilterScriptInit
	#else
		#define _ALS_OnFilterScriptInit
	#endif
	#define OnFilterScriptInit(%0) CHAIN_PUBLIC:SSCANF_OnFilterScriptInit(%0)
	CHAIN_FORWARD:SSCANF_OnFilterScriptInit() = 1;

	/*
	  OnGameModeInit

	  Called when the script starts if it is a gamemode.  This callback is also
	  called in filterscripts so we don't want to reinitialise the system in
	  that case.
	*/

	public OnGameModeInit()
	{
		if (!SSCANF_gInit)
		{
			SSCANF_RunInit();
		}
		SSCANF_OnGameModeInit();
		return 1;
	}

	#if defined _ALS_OnGameModeInit
		#undef OnGameModeInit
	#else
		#define _ALS_OnGameModeInit
	#endif
	#define OnGameModeInit(%0) CHAIN_PUBLIC:SSCANF_OnGameModeInit(%0)
	CHAIN_FORWARD:SSCANF_OnGameModeInit() = 1;

	/*
	  OnCachedInit

	  Called when the script starts if it is YSI cached mode.
	*/

	forward OnCachedInit();
	public OnCachedInit()
	{
		SSCANF_RunInit();
		SSCANF_OnCachedInit();
		return 1;
	}

	#if defined _ALS_OnCachedInit
		#undef OnCachedInit
	#else
		#define _ALS_OnCachedInit
	#endif
	#define OnCachedInit(%0) CHAIN_PUBLIC:SSCANF_OnCachedInit(%0)
	CHAIN_FORWARD:SSCANF_OnCachedInit() = 1;

	/*
	  OnPlayerConnect

	  Called when a player connects.  Actually increments an internal count so
	  that if a script ends and "OnPlayerDisconnect" is called then "sscanf"
	  still knows that the player is really connected.  Also stores their name
	  internally.
	*/

	public OnPlayerConnect(playerid)
	{
		new
			name[MAX_PLAYER_NAME + 1];
		GetPlayerName(playerid, name, sizeof(name));
		SSCANF_Join(playerid, name, IsPlayerNPC(playerid));
		SSCANF_OnPlayerConnect(playerid);
		return 1;
	}

	#if defined _ALS_OnPlayerConnect
		#undef OnPlayerConnect
	#else
		#define _ALS_OnPlayerConnect
	#endif
	#define OnPlayerConnect(%0) CHAIN_PUBLIC:SSCANF_OnPlayerConnect(%0)
	CHAIN_FORWARD:SSCANF_OnPlayerConnect(playerid) = 1;

	/*
	  OnPlayerDisconnect

	  Called when a player disconnects, or when a script is ended.
	*/

	public OnPlayerDisconnect(playerid, reason)
	{
		SSCANF_OnPlayerDisconnect(playerid, reason);
		SSCANF_Leave(playerid);
		return 1;
	}

	#if defined _ALS_OnPlayerDisconnect
		#undef OnPlayerDisconnect
	#else
		#define _ALS_OnPlayerDisconnect
	#endif
	#define OnPlayerDisconnect(%0) CHAIN_PUBLIC:SSCANF_OnPlayerDisconnect(%0)
	CHAIN_FORWARD:SSCANF_OnPlayerDisconnect(playerid, reason) = 1;
#endif

#define SSCANF_Init
#define SSCANF_Join
#define SSCANF_Leave
#define SSCANF_IsConnected

#if defined __PawnBuild
	#define extract%0->%1; EXTRN%1;UNFORMAT__(__file,__line,_:EXTRV:EXTRX:%0,""#,%1,,);
#else
	#define extract%0->%1; EXTRN%1;UNFORMAT__(SSCANF_EXT_,-1,_:EXTRV:EXTRX:%0,""#,%1,,);
#endif

#define UNFORMAT__(%7,%8,_:EXTRV:EXTRX:%0,""#,%1);%2else if (UNFORMAT__(%7,%8,_:EXTRV:EXTRX:%0,""#,%1))

#define EXTRV:EXTRX:%0<%3>%4#,%9new%1,%2) EXTRZ:EXTRY:%0%4#P<%3>,|||%1|||%2)
#define EXTRZ:EXTRY:%0#P<,> EXTRY:%0"P<,>"#
#define EXTRX:%0#,%9new%1,%2) EXTRY:%0#,|||%1|||%2)
#define EXTRY: EXTR8:EXTR9:EXTR0:EXTR1:EXTR2:EXTR3:EXTR4:

#define EXTR8:EXTR9:EXTR0:EXTR1:EXTR2:EXTR3:EXTR4:%0#%1,%2|||%6:%3=%9|||%4) %6_EXTRO:%0#%1,%2|||%3=%9|||%4)
#define EXTR9:EXTR0:EXTR1:EXTR2:EXTR3:EXTR4:%0#%1,%2|||%3=%9|||%4) __EXTRO:%0#%1,%2|||%3=%9|||%4)
#define EXTR0:EXTR1:EXTR2:EXTR3:EXTR4:%0#%1,%2|||%6:%3[%7]|||%4) %6_EXTRW:%0#%1,%2|||%3[%7]|||%4)
#define EXTR1:EXTR2:EXTR3:EXTR4:%0#%1,%2|||%3[%7]|||%4) __EXTRW:%0#%1,%2|||%3[%7]|||%4)
#define EXTR2:EXTR3:EXTR4:%0#%1,%2|||%6:%3|||%4) %6_EXTRN:%0#%1,%2|||%3|||%4)
#define EXTR3:EXTR4:%0#%1,,%2||||||%4) %0#%1,%2)
#define EXTR4:%0#%1,%2|||%3|||%4) __EXTRN:%0#%1,%2|||%3|||%4)

// Optional specifiers.
#define __EXTRO:%0#%1,%2|||%3=%9|||%4,%5) EXTRY:%0#%1I"("#%9")"#,%2,%3|||%4|||%5)
#define Float_EXTRO:%0#%1,%2|||%3=%9|||%4,%5) EXTRY:%0#%1F"("#%9")"#,%2,%3|||%4|||%5)
#define player_EXTRO:%0#%1,%2|||%3=%9|||%4,%5) EXTRY:%0#%1U"("#%9")"#,%2,%3|||%4|||%5)
#define string_EXTRO:%0#%1,%2|||%3[%7]=%9|||%4,%5) EXTRY:%0#%1S"("#%9")[*]",%2,(%7),%3|||%4|||%5)

// Normal specifiers (the double underscore is to work for "_:".
#define __EXTRN:%0#%1,%2|||%3|||%4,%5) EXTRY:%0#%1i,%2,%3|||%4|||%5)
#define Float_EXTRN:%0#%1,%2|||%3|||%4,%5) EXTRY:%0#%1f,%2,%3|||%4|||%5)
#define player_EXTRN:%0#%1,%2|||%3|||%4,%5) EXTRY:%0#%1u,%2,%3|||%4|||%5)
//#define string_EXTRW:%0#%1,%2|||%3[%7]|||%4,%5) EXTRY:%0#%1s[%7],%2,%3|||%4|||%5)

// Array versions of normal specifiers.
#define __EXTRW:%0#%1,%2|||%3[%7]|||%4,%5) EXTRY:%0#%1a<i>[*],%2,(%7),%3|||%4|||%5)
#define Float_EXTRW:%0#%1,%2|||%3[%7]|||%4,%5) EXTRY:%0#%1a<f>[*],%2,(%7),%3|||%4|||%5)
#define player_EXTRW:%0#%1,%2|||%3[%7]|||%4,%5) EXTRY:%0#%1a<u>[*],%2,(%7),%3|||%4|||%5)
#define string_EXTRW:%0#%1,%2|||%3[%7]|||%4,%5) EXTRY:%0#%1s[*],%2,(%7),%3|||%4|||%5)

// Get rid of excess leading space which causes warnings.
#define EXTRN%0new%1; new%1;

#if !defined string
    #define string:
#endif

#define player:%0;UNFORMAT__(%1) %0;UNFORMAT__(%1)

#define hex:%0;UNFORMAT__(%1) %0;UNFORMAT__(%1)
#define hex_EXTRO:%0#%1,%2|||%3=%9|||%4,%5) EXTRY:%0#%1H"("#%9")"#,%2,%3|||%4|||%5)
#define hex_EXTRN:%0#%1,%2|||%3|||%4,%5) EXTRY:%0#%1h,%2,%3|||%4|||%5)
#define hex_EXTRW:%0#%1,%2|||%3[%7]|||%4,%5) EXTRY:%0#%1a<h>[*],%2,(%7),%3|||%4|||%5)

#define bin:%0;UNFORMAT__(%1) %0;UNFORMAT__(%1)
#define bin_EXTRO:%0#%1,%2|||%3=%9|||%4,%5) EXTRY:%0#%1B"("#%9")"#,%2,%3|||%4|||%5)
#define bin_EXTRN:%0#%1,%2|||%3|||%4,%5) EXTRY:%0#%1b,%2,%3|||%4|||%5)
#define bin_EXTRW:%0#%1,%2|||%3[%7]|||%4,%5) EXTRY:%0#%1a<b>[*],%2,(%7),%3|||%4|||%5)

#define colour:%0;UNFORMAT__(%1) %0;UNFORMAT__(%1)
#define colour_EXTRO:%0#%1,%2|||%3=%9|||%4,%5) EXTRY:%0#%1M"("#%9")"#,%2,%3|||%4|||%5)
#define colour_EXTRN:%0#%1,%2|||%3|||%4,%5) EXTRY:%0#%1m,%2,%3|||%4|||%5)
#define colour_EXTRW:%0#%1,%2|||%3[%7]|||%4,%5) EXTRY:%0#%1a<m>[*],%2,(%7),%3|||%4|||%5)

#define kustom:%0<%1> %0
#define kustom_EXTRO:%0#%1,%2|||%3<%8>=%9|||%4,%5) EXTRY:%0#%1K<%8>"("#%9")"#,%2,%3|||%4|||%5)
#define kustom_EXTRN:%0#%1,%2|||%3<%8>|||%4,%5) EXTRY:%0#%1k<%8>,%2,%3|||%4|||%5)
//#define bin_EXTRW:%0#%1,%2|||%3[%7]|||%4,%5) EXTRY:%0#%1a<b>[*],%2,(%7),%3|||%4|||%5)

SSCANF:weapon(string[])
{
	// This function is VERY basic, needs VASTLY improving to detect variations.
	if ('0' <= string[0] <= '9')
	{
		new
			ret = strval(string);
		if (0 <= ret <= 18 || 22 <= ret <= 46)
		{
			return ret;
		}
	}
	else if (!strcmp(string, "Unarmed", true)) return 0;
	else if (!strcmp(string, "Brass Knuckles", true)) return 1;
	else if (!strcmp(string, "Golf Club", true)) return 2;
	else if (!strcmp(string, "Night Stick", true)) return 3;
	else if (!strcmp(string, "Knife", true)) return 4;
	else if (!strcmp(string, "Baseball Bat", true)) return 5;
	else if (!strcmp(string, "Shovel", true)) return 6;
	else if (!strcmp(string, "Pool cue", true)) return 7;
	else if (!strcmp(string, "Katana", true)) return 8;
	else if (!strcmp(string, "Chainsaw", true)) return 9;
	else if (!strcmp(string, "Purple Dildo", true)) return 10;
	else if (!strcmp(string, "White Dildo", true)) return 11;
	else if (!strcmp(string, "Long White Dildo", true)) return 12;
	else if (!strcmp(string, "White Dildo 2", true)) return 13;
	else if (!strcmp(string, "Flowers", true)) return 14;
	else if (!strcmp(string, "Cane", true)) return 15;
	else if (!strcmp(string, "Grenades", true)) return 16;
	else if (!strcmp(string, "Tear Gas", true)) return 17;
	else if (!strcmp(string, "Molotovs", true)) return 18;
	else if (!strcmp(string, "Pistol", true)) return 22;
	else if (!strcmp(string, "Silenced Pistol", true)) return 23;
	else if (!strcmp(string, "Desert Eagle", true)) return 24;
	else if (!strcmp(string, "Shotgun", true)) return 25;
	else if (!strcmp(string, "Sawn Off Shotgun", true)) return 26;
	else if (!strcmp(string, "Combat Shotgun", true)) return 27;
	else if (!strcmp(string, "Micro Uzi", true)) return 28;
	else if (!strcmp(string, "Mac 10", true)) return 28;
	else if (!strcmp(string, "MP5", true)) return 29;
	else if (!strcmp(string, "AK47", true)) return 30;
	else if (!strcmp(string, "M4", true)) return 31;
	else if (!strcmp(string, "Tec9", true)) return 32;
	else if (!strcmp(string, "Rifle", true)) return 33;
	else if (!strcmp(string, "Sniper Rifle", true)) return 34;
	else if (!strcmp(string, "RPG", true)) return 35;
	else if (!strcmp(string, "Missile Launcher", true)) return 36;
	else if (!strcmp(string, "Flame Thrower", true)) return 37;
	else if (!strcmp(string, "Minigun", true)) return 38;
	else if (!strcmp(string, "Sachel Charges", true)) return 39;
	else if (!strcmp(string, "Detonator", true)) return 40;
	else if (!strcmp(string, "Spray Paint", true)) return 41;
	else if (!strcmp(string, "Fire Extinguisher", true)) return 42;
	else if (!strcmp(string, "Camera", true)) return 43;
	else if (!strcmp(string, "Nightvision Goggles", true)) return 44;
	else if (!strcmp(string, "Thermal Goggles", true)) return 45;
	else if (!strcmp(string, "Parachute", true)) return 46;
	return -1;
}

SSCANF:vehicle(string[])
{
	// This function is VERY basic, needs VASTLY improving to detect variations.
	if ('0' <= string[0] <= '9')
	{
		new
			ret = strval(string);
		if (400 <= ret <= 611)
		{
			return ret;
		}
	}
	else if (!strcmp(string, "Landstalker", true)) return 400;
	else if (!strcmp(string, "Bravura", true)) return 401;
	else if (!strcmp(string, "Buffalo", true)) return 402;
	else if (!strcmp(string, "Linerunner", true)) return 403;
	else if (!strcmp(string, "Perenniel", true)) return 404;
	else if (!strcmp(string, "Sentinel", true)) return 405;
	else if (!strcmp(string, "Dumper", true)) return 406;
	else if (!strcmp(string, "Firetruck", true)) return 407;
	else if (!strcmp(string, "Trashmaster", true)) return 408;
	else if (!strcmp(string, "Stretch", true)) return 409;
	else if (!strcmp(string, "Manana", true)) return 410;
	else if (!strcmp(string, "Infernus", true)) return 411;
	else if (!strcmp(string, "Voodoo", true)) return 412;
	else if (!strcmp(string, "Pony", true)) return 413;
	else if (!strcmp(string, "Mule", true)) return 414;
	else if (!strcmp(string, "Cheetah", true)) return 415;
	else if (!strcmp(string, "Ambulance", true)) return 416;
	else if (!strcmp(string, "Leviathan", true)) return 417;
	else if (!strcmp(string, "Moonbeam", true)) return 418;
	else if (!strcmp(string, "Esperanto", true)) return 419;
	else if (!strcmp(string, "Taxi", true)) return 420;
	else if (!strcmp(string, "Washington", true)) return 421;
	else if (!strcmp(string, "Bobcat", true)) return 422;
	else if (!strcmp(string, "Mr Whoopee", true)) return 423;
	else if (!strcmp(string, "BF Injection", true)) return 424;
	else if (!strcmp(string, "Hunter", true)) return 425;
	else if (!strcmp(string, "Premier", true)) return 426;
	else if (!strcmp(string, "Enforcer", true)) return 427;
	else if (!strcmp(string, "Securicar", true)) return 428;
	else if (!strcmp(string, "Banshee", true)) return 429;
	else if (!strcmp(string, "Predator", true)) return 430;
	else if (!strcmp(string, "Bus", true)) return 431;
	else if (!strcmp(string, "Rhino", true)) return 432;
	else if (!strcmp(string, "Barracks", true)) return 433;
	else if (!strcmp(string, "Hotknife", true)) return 434;
	else if (!strcmp(string, "Article Trailer", true)) return 435;
	else if (!strcmp(string, "Previon", true)) return 436;
	else if (!strcmp(string, "Coach", true)) return 437;
	else if (!strcmp(string, "Cabbie", true)) return 438;
	else if (!strcmp(string, "Stallion", true)) return 439;
	else if (!strcmp(string, "Rumpo", true)) return 440;
	else if (!strcmp(string, "RC Bandit", true)) return 441;
	else if (!strcmp(string, "Romero", true)) return 442;
	else if (!strcmp(string, "Packer", true)) return 443;
	else if (!strcmp(string, "Monster", true)) return 444;
	else if (!strcmp(string, "Admiral", true)) return 445;
	else if (!strcmp(string, "Squallo", true)) return 446;
	else if (!strcmp(string, "Seasparrow", true)) return 447;
	else if (!strcmp(string, "Pizzaboy", true)) return 448;
	else if (!strcmp(string, "Tram", true)) return 449;
	else if (!strcmp(string, "Article Trailer 2", true)) return 450;
	else if (!strcmp(string, "Turismo", true)) return 451;
	else if (!strcmp(string, "Speeder", true)) return 452;
	else if (!strcmp(string, "Reefer", true)) return 453;
	else if (!strcmp(string, "Tropic", true)) return 454;
	else if (!strcmp(string, "Flatbed", true)) return 455;
	else if (!strcmp(string, "Yankee", true)) return 456;
	else if (!strcmp(string, "Caddy", true)) return 457;
	else if (!strcmp(string, "Solair", true)) return 458;
	else if (!strcmp(string, "Berkley's RC Van", true)) return 459;
	else if (!strcmp(string, "Skimmer", true)) return 460;
	else if (!strcmp(string, "PCJ-600", true)) return 461;
	else if (!strcmp(string, "Faggio", true)) return 462;
	else if (!strcmp(string, "Freeway", true)) return 463;
	else if (!strcmp(string, "RC Baron", true)) return 464;
	else if (!strcmp(string, "RC Raider", true)) return 465;
	else if (!strcmp(string, "Glendale", true)) return 466;
	else if (!strcmp(string, "Oceanic", true)) return 467;
	else if (!strcmp(string, "Sanchez", true)) return 468;
	else if (!strcmp(string, "Sparrow", true)) return 469;
	else if (!strcmp(string, "Patriot", true)) return 470;
	else if (!strcmp(string, "Quad", true)) return 471;
	else if (!strcmp(string, "Coastguard", true)) return 472;
	else if (!strcmp(string, "Dinghy", true)) return 473;
	else if (!strcmp(string, "Hermes", true)) return 474;
	else if (!strcmp(string, "Sabre", true)) return 475;
	else if (!strcmp(string, "Rustler", true)) return 476;
	else if (!strcmp(string, "ZR-350", true)) return 477;
	else if (!strcmp(string, "Walton", true)) return 478;
	else if (!strcmp(string, "Regina", true)) return 479;
	else if (!strcmp(string, "Comet", true)) return 480;
	else if (!strcmp(string, "BMX", true)) return 481;
	else if (!strcmp(string, "Burrito", true)) return 482;
	else if (!strcmp(string, "Camper", true)) return 483;
	else if (!strcmp(string, "Marquis", true)) return 484;
	else if (!strcmp(string, "Baggage", true)) return 485;
	else if (!strcmp(string, "Dozer", true)) return 486;
	else if (!strcmp(string, "Maverick", true)) return 487;
	else if (!strcmp(string, "SAN News Maverick", true)) return 488;
	else if (!strcmp(string, "Rancher", true)) return 489;
	else if (!strcmp(string, "FBI Rancher", true)) return 490;
	else if (!strcmp(string, "Virgo", true)) return 491;
	else if (!strcmp(string, "Greenwood", true)) return 492;
	else if (!strcmp(string, "Jetmax", true)) return 493;
	else if (!strcmp(string, "Hotring Racer", true)) return 494;
	else if (!strcmp(string, "Sandking", true)) return 495;
	else if (!strcmp(string, "Blista Compact", true)) return 496;
	else if (!strcmp(string, "Police Maverick", true)) return 497;
	else if (!strcmp(string, "Boxville", true)) return 498;
	else if (!strcmp(string, "Benson", true)) return 499;
	else if (!strcmp(string, "Mesa", true)) return 500;
	else if (!strcmp(string, "RC Goblin", true)) return 501;
	else if (!strcmp(string, "Hotring Racer", true)) return 502;
	else if (!strcmp(string, "Hotring Racer", true)) return 503;
	else if (!strcmp(string, "Bloodring Banger", true)) return 504;
	else if (!strcmp(string, "Rancher", true)) return 505;
	else if (!strcmp(string, "Super GT", true)) return 506;
	else if (!strcmp(string, "Elegant", true)) return 507;
	else if (!strcmp(string, "Journey", true)) return 508;
	else if (!strcmp(string, "Bike", true)) return 509;
	else if (!strcmp(string, "Mountain Bike", true)) return 510;
	else if (!strcmp(string, "Beagle", true)) return 511;
	else if (!strcmp(string, "Cropduster", true)) return 512;
	else if (!strcmp(string, "Stuntplane", true)) return 513;
	else if (!strcmp(string, "Tanker", true)) return 514;
	else if (!strcmp(string, "Roadtrain", true)) return 515;
	else if (!strcmp(string, "Nebula", true)) return 516;
	else if (!strcmp(string, "Majestic", true)) return 517;
	else if (!strcmp(string, "Buccaneer", true)) return 518;
	else if (!strcmp(string, "Shamal", true)) return 519;
	else if (!strcmp(string, "Hydra", true)) return 520;
	else if (!strcmp(string, "FCR-900", true)) return 521;
	else if (!strcmp(string, "NRG-500", true)) return 522;
	else if (!strcmp(string, "HPV1000", true)) return 523;
	else if (!strcmp(string, "Cement Truck", true)) return 524;
	else if (!strcmp(string, "Towtruck", true)) return 525;
	else if (!strcmp(string, "Fortune", true)) return 526;
	else if (!strcmp(string, "Cadrona", true)) return 527;
	else if (!strcmp(string, "FBI Truck", true)) return 528;
	else if (!strcmp(string, "Willard", true)) return 529;
	else if (!strcmp(string, "Forklift", true)) return 530;
	else if (!strcmp(string, "Tractor", true)) return 531;
	else if (!strcmp(string, "Combine Harvester", true)) return 532;
	else if (!strcmp(string, "Feltzer", true)) return 533;
	else if (!strcmp(string, "Remington", true)) return 534;
	else if (!strcmp(string, "Slamvan", true)) return 535;
	else if (!strcmp(string, "Blade", true)) return 536;
	else if (!strcmp(string, "Freight (Train)", true)) return 537;
	else if (!strcmp(string, "Brownstreak (Train)", true)) return 538;
	else if (!strcmp(string, "Vortex", true)) return 539;
	else if (!strcmp(string, "Vincent", true)) return 540;
	else if (!strcmp(string, "Bullet", true)) return 541;
	else if (!strcmp(string, "Clover", true)) return 542;
	else if (!strcmp(string, "Sadler", true)) return 543;
	else if (!strcmp(string, "Firetruck LA", true)) return 544;
	else if (!strcmp(string, "Hustler", true)) return 545;
	else if (!strcmp(string, "Intruder", true)) return 546;
	else if (!strcmp(string, "Primo", true)) return 547;
	else if (!strcmp(string, "Cargobob", true)) return 548;
	else if (!strcmp(string, "Tampa", true)) return 549;
	else if (!strcmp(string, "Sunrise", true)) return 550;
	else if (!strcmp(string, "Merit", true)) return 551;
	else if (!strcmp(string, "Utility Van", true)) return 552;
	else if (!strcmp(string, "Nevada", true)) return 553;
	else if (!strcmp(string, "Yosemite", true)) return 554;
	else if (!strcmp(string, "Windsor", true)) return 555;
	else if (!strcmp(string, "Monster \"A\"", true)) return 556;
	else if (!strcmp(string, "Monster \"B\"", true)) return 557;
	else if (!strcmp(string, "Uranus", true)) return 558;
	else if (!strcmp(string, "Jester", true)) return 559;
	else if (!strcmp(string, "Sultan", true)) return 560;
	else if (!strcmp(string, "Stratum", true)) return 561;
	else if (!strcmp(string, "Elegy", true)) return 562;
	else if (!strcmp(string, "Raindance", true)) return 563;
	else if (!strcmp(string, "RC Tiger", true)) return 564;
	else if (!strcmp(string, "Flash", true)) return 565;
	else if (!strcmp(string, "Tahoma", true)) return 566;
	else if (!strcmp(string, "Savanna", true)) return 567;
	else if (!strcmp(string, "Bandito", true)) return 568;
	else if (!strcmp(string, "Freight Flat Trailer (Train)", true)) return 569;
	else if (!strcmp(string, "Streak Trailer (Train)", true)) return 570;
	else if (!strcmp(string, "Kart", true)) return 571;
	else if (!strcmp(string, "Mower", true)) return 572;
	else if (!strcmp(string, "Dune", true)) return 573;
	else if (!strcmp(string, "Sweeper", true)) return 574;
	else if (!strcmp(string, "Broadway", true)) return 575;
	else if (!strcmp(string, "Tornado", true)) return 576;
	else if (!strcmp(string, "AT400", true)) return 577;
	else if (!strcmp(string, "DFT-30", true)) return 578;
	else if (!strcmp(string, "Huntley", true)) return 579;
	else if (!strcmp(string, "Stafford", true)) return 580;
	else if (!strcmp(string, "BF-400", true)) return 581;
	else if (!strcmp(string, "Newsvan", true)) return 582;
	else if (!strcmp(string, "Tug", true)) return 583;
	else if (!strcmp(string, "Petrol Trailer", true)) return 584;
	else if (!strcmp(string, "Emperor", true)) return 585;
	else if (!strcmp(string, "Wayfarer", true)) return 586;
	else if (!strcmp(string, "Euros", true)) return 587;
	else if (!strcmp(string, "Hotdog", true)) return 588;
	else if (!strcmp(string, "Club", true)) return 589;
	else if (!strcmp(string, "Freight Box Trailer (Train)", true)) return 590;
	else if (!strcmp(string, "Article Trailer 3", true)) return 591;
	else if (!strcmp(string, "Andromada", true)) return 592;
	else if (!strcmp(string, "Dodo", true)) return 593;
	else if (!strcmp(string, "RC Cam", true)) return 594;
	else if (!strcmp(string, "Launch", true)) return 595;
	else if (!strcmp(string, "Police Car (LSPD)", true)) return 596;
	else if (!strcmp(string, "Police Car (SFPD)", true)) return 597;
	else if (!strcmp(string, "Police Car (LVPD)", true)) return 598;
	else if (!strcmp(string, "Police Ranger", true)) return 599;
	else if (!strcmp(string, "Picador", true)) return 600;
	else if (!strcmp(string, "S.W.A.T.", true)) return 601;
	else if (!strcmp(string, "Alpha", true)) return 602;
	else if (!strcmp(string, "Phoenix", true)) return 603;
	else if (!strcmp(string, "Glendale Shit", true)) return 604;
	else if (!strcmp(string, "Sadler Shit", true)) return 605;
	else if (!strcmp(string, "Baggage Trailer \"A\"", true)) return 606;
	else if (!strcmp(string, "Baggage Trailer \"B\"", true)) return 607;
	else if (!strcmp(string, "Tug Stairs Trailer", true)) return 608;
	else if (!strcmp(string, "Boxville", true)) return 609;
	else if (!strcmp(string, "Farm Trailer", true)) return 610;
	else if (!strcmp(string, "Utility Trailer", true)) return 611;
	return -1;
}

// Fix the compiler crash when both the PAWN and Plugin versions of sscanf are
// found by renaming the old version at declaration.  (fixes.inc compatible
// naming scheme: "BAD_Function()").
#define SSCANF__(%0:...) BAD_sscanf(%0:...)

