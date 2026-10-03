//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: A single metaball. Rendered by the client as part of a point_blob_container.
//
//=============================================================================//

#ifndef POINT_BLOB_ELEMENT_H
#define POINT_BLOB_ELEMENT_H
#ifdef _WIN32
#pragma once
#endif

#include "basecombatcharacter.h"

class CPointBlobElement : public CBaseCombatCharacter
{
public:
	DECLARE_CLASS( CPointBlobElement, CBaseCombatCharacter );
	DECLARE_SERVERCLASS();
	DECLARE_DATADESC();

	CPointBlobElement();

	virtual void Spawn( void );
	virtual void Activate( void );
	virtual bool KeyValue( const char *szKeyName, const char *szValue );
	virtual int OnTakeDamage( const CTakeDamageInfo &info );
	virtual bool ShouldCollide( int collisionGroup, int contentsMask ) const;
	virtual int UpdateTransmitState( void ) { return SetTransmitState( FL_EDICT_ALWAYS ); }

	void BlobThink( void );

protected:
	CNetworkVar( float, radius );
	CNetworkVar( float, radiusSquared );
	CNetworkVar( int, collide );
	CNetworkVar( int, destroy );
	int health;
};

#endif // POINT_BLOB_ELEMENT_H
