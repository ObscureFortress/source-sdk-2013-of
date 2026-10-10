//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Client side of a single metaball.
//
//=============================================================================//

#include "cbase.h"
#include "c_point_blob_element.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

LINK_ENTITY_TO_CLASS( point_blob_element, C_PointBlobElement );

IMPLEMENT_CLIENTCLASS_DT( C_PointBlobElement, DT_PointBlobElement, CPointBlobElement )
	RecvPropFloat( RECVINFO( radius ) ),
	RecvPropFloat( RECVINFO( radiusSquared ) ),
	RecvPropFloat( RECVINFO( collide ) ),
	RecvPropFloat( RECVINFO( destroy ) ),
	RecvPropFloat( RECVINFO( health ) ),
END_RECV_TABLE()

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
C_PointBlobElement::C_PointBlobElement() : SpawnTime( 0 ), First( false )
{
	radius = 10.0f;
	radiusSquared = 100.0f;
	collide = 0;
	destroy = 0;
	health = 10;

	metaball = METABALL();
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void C_PointBlobElement::Spawn( void )
{
	C_BaseFlex::Spawn();
}

void C_PointBlobElement::Activate( void )
{
}

void C_PointBlobElement::ClientThink( void )
{
}

bool C_PointBlobElement::ShouldCollide( void ) const
{
	if ( collide == 1 )
		return true;

	return false;
}

void C_PointBlobElement::OnDataChanged( DataUpdateType_t updateType )
{
}

void C_PointBlobElement::Simulate( void )
{
	BaseClass::Simulate();
}
