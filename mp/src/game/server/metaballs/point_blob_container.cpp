//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Defines the volume in which metaballs (point_blob_element) are rendered.
//
//=============================================================================//

#include "cbase.h"
#include "point_blob_container.h"
#include "sendproxy.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

LINK_ENTITY_TO_CLASS( point_blob_container, CPointBlobContainer );

BEGIN_DATADESC( CPointBlobContainer )
	DEFINE_KEYFIELD( GridSize, FIELD_INTEGER, "resolution" ),
	DEFINE_KEYFIELD( GridBounds, FIELD_VECTOR, "bounds" ),
	DEFINE_KEYFIELD( color, FIELD_COLOR32, "color" ),
	DEFINE_KEYFIELD( Ambcolor, FIELD_COLOR32, "ambientcolor" ),
	DEFINE_KEYFIELD( colorBoost, FIELD_FLOAT, "brightness" ),
	DEFINE_KEYFIELD( BlobMaterialName, FIELD_STRING, "materialpath" ),
	DEFINE_KEYFIELD( attraction, FIELD_FLOAT, "attraction" ),
END_DATADESC()

IMPLEMENT_SERVERCLASS_ST( CPointBlobContainer, DT_PointBlobContainer )
	SendPropInt( SENDINFO( GridSize ) ),
	SendPropVector( SENDINFO( GridBounds ), -1, SPROP_COORD_MP ),
	SendPropInt( SENDINFO( color ), 32, SPROP_UNSIGNED, SendProxy_Color32ToInt ),
	SendPropInt( SENDINFO( Ambcolor ), 32, SPROP_UNSIGNED, SendProxy_Color32ToInt ),
	SendPropFloat( SENDINFO( colorBoost ) ),
	SendPropStringT( SENDINFO( BlobMaterialName ) ),
	SendPropFloat( SENDINFO( attraction ) ),
END_SEND_TABLE()

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
CPointBlobContainer::CPointBlobContainer()
{
	GridSize = 10;
	GridBounds = Vector( 50, 50, 50 );

	color32 ambientColor = { 0x50, 0x50, 0x50, 0 };
	Ambcolor = ambientColor;
	colorBoost = 1.0f;

	color32 blobColor = { 0x64, 0x64, 0x64, 0 };
	color = blobColor;
	attraction = 1.0f;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CPointBlobContainer::Spawn( void )
{
	SetTransmitState( FL_EDICT_PVSCHECK );

	m_nRenderMode = kRenderNormal;
	m_nRenderFX = 2;

	SetLightingOrigin( this );
	SetMoveType( MOVETYPE_NONE );
	m_takedamage = DAMAGE_NO;
	SetNextThink( TICK_NEVER_THINK );

	m_flAnimTime = gpGlobals->curtime;
	m_flPlaybackRate = 0.0f;
	SetCycle( 0 );
}
