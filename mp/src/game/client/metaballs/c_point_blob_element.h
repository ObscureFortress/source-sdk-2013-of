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

class C_PointBlobElement : public C_BaseCombatCharacter
{
public:
	DECLARE_CLASS( C_PointBlobElement, C_BaseCombatCharacter );
	DECLARE_CLIENTCLASS();

	C_PointBlobElement();

	virtual void Spawn( void );
	virtual void ClientThink( void ) {}
	virtual void OnDataChanged( DataUpdateType_t updateType ) {}
	virtual bool ShouldCollide( int collisionGroup, int contentsMask ) const;

	float GetRadius() const { return radius; }
	float GetRadiusSquared() const { return radiusSquared; }

private:
	float radius;
	float radiusSquared;
	float collide;
	float destroy;
	float health;
};

#endif // C_POINT_BLOB_ELEMENT_H
