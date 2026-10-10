//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Client side of a single metaball.
//
//=============================================================================//

#ifndef C_POINT_BLOB_ELEMENT_H
#define C_POINT_BLOB_ELEMENT_H
#ifdef _WIN32
#pragma once
#endif

#include "c_basecombatcharacter.h"
#include "metaball.h"

class C_PointBlobElement : public C_BaseCombatCharacter
{
public:
	DECLARE_CLASS( C_PointBlobElement, C_BaseCombatCharacter );
	DECLARE_CLIENTCLASS();

	C_PointBlobElement();
	virtual ~C_PointBlobElement() {}

	virtual void Spawn( void );
	virtual void Activate( void );
	virtual void ClientThink( void );
	virtual bool ShouldCollide( void ) const;
	virtual void OnDataChanged( DataUpdateType_t updateType );
	virtual void Simulate( void );

	float radius;
	float radiusSquared;
	int collide;
	int destroy;
	int health;

	METABALL metaball;
	Vector pos;
	float SpawnTime;
	bool First;
};

#endif // C_POINT_BLOB_ELEMENT_H
