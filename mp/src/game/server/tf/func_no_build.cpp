//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//
//=============================================================================//
#include "cbase.h"
#include "func_no_build.h"
#include "tf_team.h"
#include "ndebugoverlay.h"
#include "tf_obj.h"
#include "triggers.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

//-----------------------------------------------------------------------------
// Purpose: Defines an area where objects cannot be built
//-----------------------------------------------------------------------------
class CFuncNoBuild : public CBaseTrigger
{
	DECLARE_CLASS( CFuncNoBuild, CBaseTrigger );
public:
	CFuncNoBuild();

	DECLARE_DATADESC();

	virtual void Spawn( void );
	virtual void Precache( void );
	virtual void Activate( void );

	// Inputs
	void	InputSetActive( inputdata_t &inputdata );
	void	InputSetInactive( inputdata_t &inputdata );
	void	InputToggleActive( inputdata_t &inputdata );

	void	SetActive( bool bActive );
	bool	GetActive() const;

	//bool	IsEmpty( void );
	bool	PointIsWithin( const Vector &vecPoint );
	bool	PreventsBuildOf( int iObjectType );

private:
	bool	m_bActive;

	// Object types that are allowed to be built inside this zone
	bool	m_bAllowSentry;
	bool	m_bAllowDispenser;
	bool	m_bAllowTeleporters;
	bool	m_bAllowTeleporterEntrances;
	bool	m_bAllowTeleporterExits;
	bool	m_bAllowForts;
	bool	m_bAllowWalls;
	bool	m_bAllowStairs;
	bool	m_bAllowRepairNodes;
};

LINK_ENTITY_TO_CLASS( func_nobuild, CFuncNoBuild);

BEGIN_DATADESC( CFuncNoBuild )

	// inputs
	DEFINE_INPUTFUNC( FIELD_VOID, "SetActive", InputSetActive ),
	DEFINE_INPUTFUNC( FIELD_VOID, "SetInactive", InputSetInactive ),
	DEFINE_INPUTFUNC( FIELD_VOID, "ToggleActive", InputToggleActive ),

	// keys
	DEFINE_KEYFIELD( m_bAllowSentry, FIELD_BOOLEAN, "AllowSentry" ),
	DEFINE_KEYFIELD( m_bAllowDispenser, FIELD_BOOLEAN, "AllowDispenser" ),
	DEFINE_KEYFIELD( m_bAllowTeleporters, FIELD_BOOLEAN, "AllowTeleporters" ),
	DEFINE_KEYFIELD( m_bAllowTeleporterEntrances, FIELD_BOOLEAN, "AllowTeleporterEntrances" ),
	DEFINE_KEYFIELD( m_bAllowTeleporterExits, FIELD_BOOLEAN, "AllowTeleporterExits" ),
	DEFINE_KEYFIELD( m_bAllowForts, FIELD_BOOLEAN, "AllowForts" ),
	DEFINE_KEYFIELD( m_bAllowWalls, FIELD_BOOLEAN, "AllowWalls" ),
	DEFINE_KEYFIELD( m_bAllowStairs, FIELD_BOOLEAN, "AllowStairs" ),
	DEFINE_KEYFIELD( m_bAllowRepairNodes, FIELD_BOOLEAN, "AllowRepairNodes" ),

END_DATADESC()


PRECACHE_REGISTER( func_nobuild );

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
CFuncNoBuild::CFuncNoBuild()
{
	m_bAllowSentry = false;
	m_bAllowDispenser = false;
	m_bAllowTeleporters = false;
	m_bAllowTeleporterEntrances = false;
	m_bAllowTeleporterExits = false;
	m_bAllowForts = false;
	m_bAllowWalls = false;
	m_bAllowStairs = false;
	m_bAllowRepairNodes = false;
}

//-----------------------------------------------------------------------------
// Purpose: Initializes the resource zone
//-----------------------------------------------------------------------------
void CFuncNoBuild::Spawn( void )
{
	BaseClass::Spawn();
	InitTrigger();

	m_bActive = true;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CFuncNoBuild::Precache( void )
{
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CFuncNoBuild::Activate( void )
{
	BaseClass::Activate();
	SetActive( true );
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CFuncNoBuild::InputSetActive( inputdata_t &inputdata )
{
	SetActive( true );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CFuncNoBuild::InputSetInactive( inputdata_t &inputdata )
{
	SetActive( false );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CFuncNoBuild::InputToggleActive( inputdata_t &inputdata )
{
	if ( m_bActive )
	{
		SetActive( false );
	}
	else
	{
		SetActive( true );
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CFuncNoBuild::SetActive( bool bActive )
{
	m_bActive = bActive;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool CFuncNoBuild::GetActive() const
{
	return m_bActive;
}

//-----------------------------------------------------------------------------
// Purpose: Return true if the specified point is within this zone
//-----------------------------------------------------------------------------
bool CFuncNoBuild::PointIsWithin( const Vector &vecPoint )
{
	Ray_t ray;
	trace_t tr;
	ICollideable *pCollide = CollisionProp();
	ray.Init( vecPoint, vecPoint );
	enginetrace->ClipRayToCollideable( ray, MASK_ALL, pCollide, &tr );
	return ( tr.startsolid );
}

//-----------------------------------------------------------------------------
// Purpose: Is this type of object allowed inside this zone?
//-----------------------------------------------------------------------------
bool CFuncNoBuild::PreventsBuildOf( int iObjectType )
{
	switch ( iObjectType )
	{
	case OBJ_DISPENSER:
		return m_bAllowDispenser;

	case OBJ_TELEPORTER_ENTRANCE:
		if ( m_bAllowTeleporters )
			return m_bAllowTeleporterEntrances;
		break;

	case OBJ_TELEPORTER_EXIT:
		if ( m_bAllowTeleporters )
			return m_bAllowTeleporterExits;
		break;

	case OBJ_SENTRYGUN:
		return m_bAllowSentry;

	case OBJ_FORT:
		return m_bAllowForts;

	case OBJ_WALL:
		return m_bAllowWalls;

	case OBJ_STAIRS:
		return m_bAllowStairs;

	case OBJ_REPAIRNODE:
		return m_bAllowRepairNodes;
	}

	return false;
}

//-----------------------------------------------------------------------------
// Purpose: Does a nobuild zone prevent us from building?
//-----------------------------------------------------------------------------
bool PointInNoBuild( const Vector &vecBuildOrigin, CBaseObject *pObject )
{
	// Find out whether we're in a resource zone or not
	CBaseEntity *pEntity = NULL;
	while ((pEntity = gEntList.FindEntityByClassname( pEntity, "func_nobuild" )) != NULL)
	{
		CFuncNoBuild *pNoBuild = (CFuncNoBuild *)pEntity;

		// Are we within this no build?
		if ( pNoBuild->GetActive() && pNoBuild->PointIsWithin( vecBuildOrigin ) )
		{
			// The zone may explicitly allow this type of object
			return !pNoBuild->PreventsBuildOf( pObject->ObjectType() );
		}
	}

	return false; // Building should be ok.
}
