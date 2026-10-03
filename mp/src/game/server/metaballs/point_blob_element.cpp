//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: A single metaball. Rendered by the client as part of a point_blob_container.
//
//=============================================================================//

#include "cbase.h"
#include "point_blob_element.h"
#include "props.h"
#include "vphysics/object_hash.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

LINK_ENTITY_TO_CLASS( point_blob_element, CPointBlobElement );

BEGIN_DATADESC( CPointBlobElement )
	DEFINE_FIELD( radius, FIELD_FLOAT ),
	DEFINE_FIELD( radiusSquared, FIELD_FLOAT ),
	DEFINE_KEYFIELD( collide, FIELD_INTEGER, "collide" ),
	DEFINE_KEYFIELD( destroy, FIELD_INTEGER, "destroy" ),
	DEFINE_KEYFIELD( health, FIELD_INTEGER, "health" ),
	DEFINE_THINKFUNC( BlobThink ),
END_DATADESC()

// NOTE: collide and destroy are ints but the shipped build networks them as floats.
IMPLEMENT_SERVERCLASS_ST( CPointBlobElement, DT_PointBlobElement )
	SendPropFloat( SENDINFO( radius ) ),
	SendPropFloat( SENDINFO( radiusSquared ) ),
	SendPropFloat( SENDINFO( collide ) ),
	SendPropFloat( SENDINFO( destroy ) ),
END_SEND_TABLE()

static const char *s_pBlobThinkContext = "blobthink";

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
CPointBlobElement::CPointBlobElement()
{
	radius = 10.0f;
	radiusSquared = 100.0f;
	collide = 0;
	destroy = 0;
	health = 10;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CPointBlobElement::Spawn( void )
{
	BaseClass::Spawn();

	SetTransmitState( FL_EDICT_ALWAYS );

	m_nRenderMode = kRenderNone;
	m_nRenderFX = 2;
}

//-----------------------------------------------------------------------------
// Purpose: The blob slowly shrinks and is removed once it is too small to see.
//-----------------------------------------------------------------------------
void CPointBlobElement::BlobThink( void )
{
	radius = radius * 0.985f;
	radiusSquared = radius * radius;

	if ( radius < 16.0f )
	{
		AddEffects( EF_NODRAW );
		SetSolid( SOLID_NONE );
		UTIL_Remove( this );
		return;
	}

	SetContextThink( &CPointBlobElement::BlobThink, gpGlobals->curtime + 0.1f, s_pBlobThinkContext );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CPointBlobElement::Activate( void )
{
	BaseClass::Activate();

	m_takedamage = DAMAGE_YES;
	m_iHealth = health;

	if ( collide == 1 || collide == 2 )
	{
		SetModel( collide == 1 ? "models/blob_phys_32.mdl" : "models/blob_phys_64.mdl" );
		SetSolid( SOLID_VPHYSICS );
		VPhysicsInitStatic();
		CollisionProp()->SetSurroundingBoundsType( USE_HITBOXES );
	}
	else if ( collide == 64 )
	{
		SetModel( "models/blob_phys_32.mdl" );
		SetSolid( SOLID_VPHYSICS );

		IPhysicsObject *pPhysObj = VPhysicsInitNormal( SOLID_VPHYSICS, 0, false );
		if ( pPhysObj )
		{
			pPhysObj->SetMass( 748.0f );
			pPhysObj->EnableGravity( true );
			pPhysObj->EnableDrag( true );
			pPhysObj->EnableCollisions( false );
			pPhysObj->ApplyForceCenter( RandomVector( -1.0f, 1.0f ) * 748.0f );
		}
		SetMoveType( MOVETYPE_VPHYSICS );
	}

	SetContextThink( &CPointBlobElement::BlobThink, gpGlobals->curtime + 0.1f, s_pBlobThinkContext );
}

//-----------------------------------------------------------------------------
// Purpose: destroy 1 = any damage pops the blob, 2 = only melee (slash/club) damage
//-----------------------------------------------------------------------------
int CPointBlobElement::OnTakeDamage( const CTakeDamageInfo &info )
{
	if ( destroy == 1 || ( destroy == 2 && ( info.GetDamageType() & ( DMG_SLASH | DMG_CLUB ) ) ) )
	{
		return BaseClass::OnTakeDamage( info );
	}

	return 0;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool CPointBlobElement::ShouldCollide( int collisionGroup, int contentsMask ) const
{
	return collide == 1;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool CPointBlobElement::KeyValue( const char *szKeyName, const char *szValue )
{
	if ( FStrEq( szKeyName, "radius" ) )
	{
		// The shipped build ignores the mapper's value and picks a random size.
		radius = RandomFloat( 32.0f, 64.0f );
		radiusSquared = radius * radius;
		return true;
	}
	if ( FStrEq( szKeyName, "collide" ) )
	{
		collide = atoi( szValue );
		return true;
	}
	if ( FStrEq( szKeyName, "destroy" ) )
	{
		destroy = atoi( szValue );
		return true;
	}
	if ( FStrEq( szKeyName, "health" ) )
	{
		health = atoi( szValue );
		return true;
	}

	return BaseClass::KeyValue( szKeyName, szValue );
}
