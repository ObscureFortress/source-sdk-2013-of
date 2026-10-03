//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Client side of a single metaball.
//
//=============================================================================//

#include "cbase.h"
#include "c_point_blob_element.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

IMPLEMENT_CLIENTCLASS_DT( C_PointBlobElement, DT_PointBlobElement, CPointBlobElement )
	RecvPropFloat( RECVINFO( radius ) ),
	RecvPropFloat( RECVINFO( radiusSquared ) ),
	RecvPropFloat( RECVINFO( collide ) ),
	RecvPropFloat( RECVINFO( destroy ) ),
	RecvPropFloat( RECVINFO( health ) ),
END_RECV_TABLE()

C_PointBlobElement::C_PointBlobElement()
{
	radius = 10.0f;
	radiusSquared = 100.0f;
	collide = 0.0f;
	destroy = 0.0f;
	health = 10.0f;
}

void C_PointBlobElement::Spawn( void )
{
	C_BaseFlex::Spawn();
}

bool C_PointBlobElement::ShouldCollide( int collisionGroup, int contentsMask ) const
{
	return collide == 1.0f;
}
