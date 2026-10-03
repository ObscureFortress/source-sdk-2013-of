//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Mounts the content of other installed Steam games and SourceMods
//			(listed in gamemounting.txt / sourcemounting.txt) as extra search paths.
//
//=============================================================================//

#ifndef GAMEMOUNTER_H
#define GAMEMOUNTER_H
#ifdef _WIN32
#pragma once
#endif

// Reads gamemounting.txt and sourcemounting.txt from the mod and adds a "GAME"
// search path for each listed game that is installed. Call once at client startup,
// after the filesystem and Steam are available.
void AddRequiredSearchPaths();

#endif // GAMEMOUNTER_H
