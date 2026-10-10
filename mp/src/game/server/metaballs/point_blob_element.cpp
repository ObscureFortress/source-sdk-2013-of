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
	DEFINE_THINKFUNC( Think ),
END_DATADESC()

// NOTE: collide and destroy are ints but the shipped build networks them as floats.
IMPLEMENT_SERVERCLASS_ST( CPointBlobElement, DT_PointBlobElement )
	SendPropFloat( SENDINFO( radius ) ),
	SendPropFloat( SENDINFO( radiusSquared ) ),
	SendPropFloat( SENDINFO( collide ) ),
	SendPropFloat( SENDINFO( destroy ) ),
END_SEND_TABLE()

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
CPointBlobElement::CPointBlobElement()
{
	radius = 10.0f;
	radiusSquared = radius * radius;
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

	SetTransmitState( FL_EDICT_PVSCHECK );

	m_nRenderMode = kRenderNone;
	m_nRenderFX = 2;
}

//-----------------------------------------------------------------------------
// Purpose: The blob slowly shrinks and is removed once it is too small to see.
//-----------------------------------------------------------------------------
void CPointBlobElement::Think( void )
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

	SetNextThink( gpGlobals->curtime + 0.1f, "blobthink" );
}

//-----------------------------------------------------------------------------
// Purpose: collide 1/2 = static 32/64 unit collision model, 64 = physics sphere
//-----------------------------------------------------------------------------
void CPointBlobElement::Activate( void )
{
	m_takedamage = DAMAGE_YES;
	m_iHealth = health;

	if ( collide > 0 && collide != 64 )
	{
		switch ( collide )
		{
		case 1:
			PrecacheModel( "models/blob_phys_32.mdl" );
			SetModel( "models/blob_phys_32.mdl" );
			break;
		case 2:
			PrecacheModel( "models/blob_phys_64.mdl" );
			SetModel( "models/blob_phys_64.mdl" );
			break;
		}
		SetSolid( SOLID_VPHYSICS );
		VPhysicsInitStatic();
		CollisionProp()->SetSurroundingBoundsType( USE_HITBOXES );
	}
	else if ( collide == 64 )
	{
		SetSolid( SOLID_BBOX );

		Vector vecMaxs( radius, radius, radius );
		SetCollisionBounds( -vecMaxs, vecMaxs );

		objectparams_t params = g_PhysDefaultObjectParams;
		params.pGameData = static_cast<void *>( this );
		int nMaterialIndex = physprops->GetSurfaceIndex( "default" );
		IPhysicsObject *pPhysicsObject = physenv->CreateSphereObject( radius, nMaterialIndex, GetAbsOrigin(), GetAbsAngles(), &params, false );
		if ( pPhysicsObject )
		{
			VPhysicsSetObject( pPhysicsObject );
			SetMoveType( MOVETYPE_VPHYSICS );

			Vector vecForce( RandomFloat( -10.0f, 10.0f ), RandomFloat( -10.0f, 10.0f ), -30.0f );
			pPhysicsObject->ApplyForceCenter( vecForce );
			pPhysicsObject->SetMass( 750.0f );
			pPhysicsObject->EnableGravity( true );
			pPhysicsObject->EnableDrag( true );
			pPhysicsObject->EnableCollisions( false );
			pPhysicsObject->Wake();
		}
	}

	SetContextThink( &CPointBlobElement::Think, gpGlobals->curtime + 0.1f, "blobthink" );
}

//-----------------------------------------------------------------------------
// Purpose: destroy 1 = any damage pops the blob, 2 = only melee (slash/club) damage
//-----------------------------------------------------------------------------
int CPointBlobElement::OnTakeDamage( const CTakeDamageInfo &info )
{
	if ( destroy > 0 )
	{
		if ( m_takedamage == DAMAGE_NO )
			m_takedamage = DAMAGE_YES;

		if ( destroy == 1 )
		{
			int iDamage = info.GetDamage();
			m_iHealth -= iDamage;
			if ( m_iHealth <= 0 )
			{
				AddEffects( EF_NODRAW );
				SetSolid( SOLID_NONE );
				UTIL_Remove( this );
				return 0;
			}
			return iDamage;
		}
		else if ( destroy == 2 )
		{
			if ( !( info.GetDamageType() & ( DMG_SLASH | DMG_CLUB ) ) )
				return 0;

			float flDamage = info.GetDamage();
			m_iHealth -= flDamage;
			if ( m_iHealth <= 0 )
			{
				AddEffects( EF_NODRAW );
				SetSolid( SOLID_NONE );
				UTIL_Remove( this );
				return 0;
			}
			return flDamage;
		}
	}

	return 0;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
bool CPointBlobElement::ShouldCollide( void ) const
{
	if ( collide == 1 )
		return true;

	return false;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
bool CPointBlobElement::KeyValue( const char *szKeyName, const char *szValue )
{
	if ( FStrEq( szKeyName, "radius" ) )
	{
		// The shipped build overrides the mapper's value and picks a random size.
		radius = atof( szValue );
		radius = -1.0f;
		if ( radius != 0.0f )
			radius = RandomFloat( 32.0f, 64.0f );
		radiusSquared = radius * radius;
	}
	if ( FStrEq( szKeyName, "collide" ) )
	{
		collide = atof( szValue );
	}
	if ( FStrEq( szKeyName, "destroy" ) )
	{
		destroy = atof( szValue );
	}
	if ( FStrEq( szKeyName, "health" ) )
	{
		health = atof( szValue );
	}

	return BaseClass::KeyValue( szKeyName, szValue );
}
