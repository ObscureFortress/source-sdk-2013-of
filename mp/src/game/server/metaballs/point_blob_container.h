//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Defines the volume in which metaballs (point_blob_element) are rendered.
//
//=============================================================================//

#ifndef POINT_BLOB_CONTAINER_H
#define POINT_BLOB_CONTAINER_H
#ifdef _WIN32
#pragma once
#endif

#include "baseanimating.h"

class CPointBlobContainer : public CBaseAnimating
{
public:
	DECLARE_CLASS( CPointBlobContainer, CBaseAnimating );
	DECLARE_DATADESC();
	DECLARE_SERVERCLASS();

	CPointBlobContainer();

	virtual void Spawn( void );

protected:
	CNetworkVar( int, GridSize );
	CNetworkVar( float, colorBoost );
	CNetworkVar( Vector, GridBounds );
	CNetworkVar( string_t, BlobMaterialName );
	CNetworkColor32( color );
	CNetworkColor32( Ambcolor );
	CNetworkVar( float, attraction );
};

#endif // POINT_BLOB_CONTAINER_H
