//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Mounts the content of other installed Steam games and SourceMods
//			(listed in gamemounting.txt / sourcemounting.txt) as extra search paths.
//
//=============================================================================//

#include "cbase.h"

#ifdef CLIENT_DLL

#include "gamemounter.h"
#include "filesystem.h"
#include "tier1/KeyValues.h"
#include "cdll_client_int.h"
#include "steam/steam_api.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

// Lets KeyValues ask us about conditionals it doesn't know ( e.g. "[!$DEDICATED]" ).
// Defined in tier1/KeyValues.cpp.
void SetExtraConditionalFunc( bool (*pfnEvaluate)( const char * ) );

//-----------------------------------------------------------------------------
// Purpose: Extra KeyValues conditional: this is a game client, so it is never
//			"$DEDICATED" - only the negated form evaluates to true.
//-----------------------------------------------------------------------------
static bool EvaluateExtraConditionals( const char *pszConditional )
{
	bool bNot = ( pszConditional[0] == '!' );

	return bNot && V_stristr( pszConditional, "$DEDICATED" );
}

//-----------------------------------------------------------------------------
// Purpose: Mount one entry from the mounting files.
//			Entry layout:
//				"<name>"
//				{
//					"appid"		"<steam app id>"
//					"required"	"0|1"
//					"paths"
//					{
//						"local"		"<folder inside the game's install directory>"
//					}
//				}
//-----------------------------------------------------------------------------
static void MountPathLocal( KeyValues *pGame )
{
	const char *pszName = pGame->GetName();
	bool bRequired = pGame->GetBool( "required", false );

	if ( steamapicontext && steamapicontext->SteamApps() )
	{
		char szInstallPath[ 520 ];
		int iLength = steamapicontext->SteamApps()->GetAppInstallDir( (AppId_t)pGame->GetUint64( "appid", 0 ), szInstallPath, sizeof( szInstallPath ) );

		if ( iLength > 0 )
		{
			ConColorMsg( Color( 90, 240, 90, 255 ), "Mounting %s (local)\n", pszName );

			KeyValues *pPaths = pGame->FindKey( "paths", false );
			if ( pPaths )
			{
				for ( KeyValues *pPath = pPaths->GetFirstSubKey(); pPath; pPath = pPath->GetNextKey() )
				{
					if ( V_stricmp( pPath->GetName(), "local" ) != 0 )
						continue;

					char szPath[ 520 ];
					V_strncpy( szPath, szInstallPath, sizeof( szPath ) );
					V_AppendSlash( szPath, sizeof( szPath ) );
					V_strncat( szPath, pPath->GetString( NULL, "" ), sizeof( szPath ) );

					g_pFullFileSystem->AddSearchPath( szPath, "GAME", PATH_ADD_TO_TAIL );

					ConColorMsg( Color( 144, 238, 144, 255 ), "\tAdding path: %s\n", pPath->GetString( NULL, "" ) );
				}
			}
		}
		else
		{
			if ( bRequired )
			{
				Error( "Failed to mount required game: %s\n", pszName );
			}

			Warning( "%s not found on system. Skipping.\n", pszName );
		}
	}
	else
	{
		if ( bRequired )
		{
			Error( "Failed to mount required game: %s, unable to determine app install path.\nPlease make sure Steam is running, and the game is installed properly.\n", pszName );
		}

		Msg( "Skipping %s, unable to get app install path.\n", pszName );
	}
}

//-----------------------------------------------------------------------------
// Purpose: Mount everything listed in gamemounting.txt and sourcemounting.txt.
//-----------------------------------------------------------------------------
void AddRequiredSearchPaths()
{
	SetExtraConditionalFunc( EvaluateExtraConditionals );

	KeyValues *pGameMounting = new KeyValues( "gamemounting.txt" );
	pGameMounting->LoadFromFile( g_pFullFileSystem, "gamemounting.txt", "MOD" );

	for ( KeyValues *pGame = pGameMounting->GetFirstTrueSubKey(); pGame; pGame = pGame->GetNextTrueSubKey() )
	{
		MountPathLocal( pGame );
	}

	pGameMounting->deleteThis();

	KeyValues *pSourceMods = new KeyValues( "SourceMods" );
	pSourceMods->LoadFromFile( g_pFullFileSystem, "sourcemounting.txt", "MOD" );

	for ( KeyValues *pMod = pSourceMods->GetFirstTrueSubKey(); pMod; pMod = pMod->GetNextTrueSubKey() )
	{
		MountPathLocal( pMod );
	}

	pSourceMods->deleteThis();
}

void SetExtraConditionalFunc(bool (*func)(const char*))
{
	// empty stub - satisfies the linker
}

#endif // CLIENT_DLL
