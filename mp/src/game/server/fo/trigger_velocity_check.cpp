// trigger_velocity_check
// A trigger that fires outputs depending on the speed of the entities touching it.
// Reconstructed from the Fortress Obscura release build.

#include "cbase.h"
#include "triggers.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

//-----------------------------------------------------------------------------
// Purpose: Fires OnVelocity when a touching entity meets the configured
//			velocity requirements, optionally requiring that the requirements
//			are met again within a time window ("velocity transition").
//-----------------------------------------------------------------------------
class CTriggerVelocityCheck : public CBaseTrigger
{
public:
	DECLARE_CLASS( CTriggerVelocityCheck, CBaseTrigger );
	DECLARE_DATADESC();

	virtual void Spawn( void );

	bool CheckVelocity( CBaseEntity *pOther, bool bNewVelocity );
	void BrushTouch( CBaseEntity *pOther );
	void BrushThink( void );

private:
	struct velentities_t
	{
		CHandle<CBaseEntity> hEntity;
		float flTime;
	};

	// Primary velocity requirements.
	float m_flLVel;			// overall speed
	float m_flXVel;
	float m_flYVel;
	float m_flZVel;
	bool m_bLMustThres;
	bool m_bXMustThres;
	bool m_bYMustThres;
	bool m_bZMustThres;
	float m_flLThres;
	float m_flXThres;
	float m_flYThres;
	float m_flZThres;

	// Velocity transition: after the first check passes, the "new" requirements
	// must be met within m_flVelTransMaxS seconds.
	bool m_bVelTransCheck;
	float m_flVelTransMaxS;
	float m_flNewLVel;
	float m_flNewXVel;
	float m_flNewYVel;
	float m_flNewZVel;
	bool m_bNewLMustThres;
	bool m_bNewXMustThres;
	bool m_bNewYMustThres;
	bool m_bNewZMustThres;
	float m_flNewLThres;
	float m_flNewXThres;
	float m_flNewYThres;
	float m_flNewZThres;

	CUtlVector<velentities_t> m_VelEntities;
	CUtlVector< CHandle<CBaseEntity> > m_FailEntities;

	COutputEvent m_OnVelocityTransitionFail;
	COutputEvent m_OnVelocityTransitionStart;
	COutputEvent m_OnVelocity;
};

LINK_ENTITY_TO_CLASS( trigger_velocity_check, CTriggerVelocityCheck );

BEGIN_DATADESC( CTriggerVelocityCheck )
	DEFINE_FUNCTION( BrushTouch ),
	DEFINE_THINKFUNC( BrushThink ),

	DEFINE_KEYFIELD( m_flLVel, FIELD_FLOAT, "lvel" ),
	DEFINE_KEYFIELD( m_flXVel, FIELD_FLOAT, "xvel" ),
	DEFINE_KEYFIELD( m_flYVel, FIELD_FLOAT, "yvel" ),
	DEFINE_KEYFIELD( m_flZVel, FIELD_FLOAT, "zvel" ),
	DEFINE_KEYFIELD( m_bLMustThres, FIELD_BOOLEAN, "lmustthres" ),
	DEFINE_KEYFIELD( m_bXMustThres, FIELD_BOOLEAN, "xmustthres" ),
	DEFINE_KEYFIELD( m_bYMustThres, FIELD_BOOLEAN, "ymustthres" ),
	DEFINE_KEYFIELD( m_bZMustThres, FIELD_BOOLEAN, "zmustthres" ),
	DEFINE_KEYFIELD( m_flLThres, FIELD_FLOAT, "lthres" ),
	DEFINE_KEYFIELD( m_flXThres, FIELD_FLOAT, "xthres" ),
	DEFINE_KEYFIELD( m_flYThres, FIELD_FLOAT, "ythres" ),
	DEFINE_KEYFIELD( m_flZThres, FIELD_FLOAT, "zthres" ),

	DEFINE_KEYFIELD( m_bVelTransCheck, FIELD_BOOLEAN, "veltransc" ),
	DEFINE_KEYFIELD( m_flVelTransMaxS, FIELD_FLOAT, "veltransmaxs" ),
	DEFINE_KEYFIELD( m_flNewLVel, FIELD_FLOAT, "newlvel" ),
	DEFINE_KEYFIELD( m_flNewXVel, FIELD_FLOAT, "newxvel" ),
	DEFINE_KEYFIELD( m_flNewYVel, FIELD_FLOAT, "newyvel" ),
	DEFINE_KEYFIELD( m_flNewZVel, FIELD_FLOAT, "newzvel" ),
	DEFINE_KEYFIELD( m_bNewLMustThres, FIELD_BOOLEAN, "newlmustthres" ),
	DEFINE_KEYFIELD( m_bNewXMustThres, FIELD_BOOLEAN, "newxmustthres" ),
	DEFINE_KEYFIELD( m_bNewYMustThres, FIELD_BOOLEAN, "newymustthres" ),
	DEFINE_KEYFIELD( m_bNewZMustThres, FIELD_BOOLEAN, "newzmustthres" ),
	DEFINE_KEYFIELD( m_flNewLThres, FIELD_FLOAT, "newlthres" ),
	DEFINE_KEYFIELD( m_flNewXThres, FIELD_FLOAT, "newxthres" ),
	DEFINE_KEYFIELD( m_flNewYThres, FIELD_FLOAT, "newythres" ),
	DEFINE_KEYFIELD( m_flNewZThres, FIELD_FLOAT, "newzthres" ),

	DEFINE_OUTPUT( m_OnVelocityTransitionFail, "OnVelocityTransitionFail" ),
	DEFINE_OUTPUT( m_OnVelocityTransitionStart, "OnVelocityTransitionStart" ),
	DEFINE_OUTPUT( m_OnVelocity, "OnVelocity" ),
END_DATADESC()

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTriggerVelocityCheck::Spawn( void )
{
	BaseClass::Spawn();

	AddSpawnFlags( SF_TRIGGER_ALLOW_CLIENTS );

	InitTrigger();

	SetTouch( &CTriggerVelocityCheck::BrushTouch );
	SetThink( &CTriggerVelocityCheck::BrushThink );
}

//-----------------------------------------------------------------------------
// Purpose: Test an entity's velocity against either the primary or the "new"
//			set of requirements.
//-----------------------------------------------------------------------------
bool CTriggerVelocityCheck::CheckVelocity( CBaseEntity *pOther, bool bNewVelocity )
{
	float flLVel, flXVel, flYVel, flZVel;
	float flLThres, flXThres, flYThres, flZThres;
	bool bLMust, bXMust, bYMust, bZMust;

	if ( bNewVelocity )
	{
		flLVel = m_flNewLVel;	flXVel = m_flNewXVel;	flYVel = m_flNewYVel;	flZVel = m_flNewZVel;
		bLMust = m_bNewLMustThres;	bXMust = m_bNewXMustThres;	bYMust = m_bNewYMustThres;	bZMust = m_bNewZMustThres;
		flLThres = m_flNewLThres;	flXThres = m_flNewXThres;	flYThres = m_flNewYThres;	flZThres = m_flNewZThres;
	}
	else
	{
		flLVel = m_flLVel;	flXVel = m_flXVel;	flYVel = m_flYVel;	flZVel = m_flZVel;
		bLMust = m_bLMustThres;	bXMust = m_bXMustThres;	bYMust = m_bYMustThres;	bZMust = m_bZMustThres;
		flLThres = m_flLThres;	flXThres = m_flXThres;	flYThres = m_flYThres;	flZThres = m_flZThres;
	}

	bool bResult = false;

	// Overall speed check takes priority when it is set.
	if ( flLVel > 0.0f )
	{
		float flSpeed = pOther->GetAbsVelocity().Length();
		if ( flSpeed < flLVel )
			return false;

		flSpeed = pOther->GetAbsVelocity().Length();

		bool bInBand = ( flSpeed >= flLVel - flLThres ) || ( flLVel + flLThres >= flSpeed );
		if ( bInBand && bLMust )
			return true;

		if ( flSpeed < flLVel )
			return false;

		return !bLMust;
	}

	// Per-axis checks.
	bool bX = false;
	bool bY = false;
	bool bZ = false;

	float flVelX = pOther->GetAbsVelocity().x;
	if ( flVelX >= flXVel )
	{
		bool bInBand = ( flVelX >= flXVel - flXThres ) || ( flXThres + flXVel >= flVelX );
		if ( bInBand && bXMust )
			bX = true;
		else
			bX = ( flVelX >= flXVel ) && !bXMust;
	}

	float flVelY = pOther->GetAbsVelocity().y;
	if ( flVelY >= flYVel )
	{
		bool bInBand = ( flVelY >= flYVel - flYThres ) || ( flYThres + flYVel >= flVelY );
		if ( bInBand && bYMust )
			bY = true;
		else
			bY = ( flVelY >= flYVel ) && !bYMust;
	}

	float flVelZ = pOther->GetAbsVelocity().z;
	if ( flVelZ >= flZVel )
	{
		bool bInBand = ( flVelZ >= flZVel - flZThres ) || ( flZThres + flZVel >= flVelZ );
		if ( bInBand && bZMust )
			bZ = true;
		else
			bZ = ( flVelZ >= flZVel ) && !bZMust;
	}

	if ( bX || bY || bZ )
	{
		bResult = true;
	}

	// Axes flagged as required must all have passed.
	if ( m_bXMustThres )
		bResult = bX ? bResult : false;
	if ( m_bYMustThres )
		bResult = bY ? bResult : false;
	if ( m_bZMustThres )
		bResult = bZ ? bResult : false;

	return bResult;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTriggerVelocityCheck::BrushTouch( CBaseEntity *pOther )
{
	// Ignore entities that have already failed the transition check and are still inside.
	for ( int i = 0; i < m_FailEntities.Count(); i++ )
	{
		if ( m_FailEntities[i].Get() == pOther )
			return;
	}

	bool bPassed = CheckVelocity( pOther, false );

	if ( !m_bVelTransCheck )
	{
		m_OnVelocity.FireOutput( this, this );
		return;
	}

	if ( !bPassed )
		return;

	// Already waiting on this entity?
	for ( int i = 0; i < m_VelEntities.Count(); i++ )
	{
		if ( m_VelEntities[i].hEntity.Get() == pOther )
			return;
	}

	int iIndex = m_VelEntities.AddToTail();
	m_VelEntities[iIndex].hEntity = pOther;
	m_VelEntities[iIndex].flTime = gpGlobals->curtime;

	m_OnVelocityTransitionStart.FireOutput( this, this );
}

//-----------------------------------------------------------------------------
// Purpose: Resolve entities that have run out of time to meet the "new" velocity.
//-----------------------------------------------------------------------------
void CTriggerVelocityCheck::BrushThink( void )
{
	if ( !m_bVelTransCheck )
		return;

	for ( int i = 0; i < m_VelEntities.Count(); i++ )
	{
		CBaseEntity *pEntity = m_VelEntities[i].hEntity.Get();

		if ( gpGlobals->curtime > m_VelEntities[i].flTime + m_flVelTransMaxS )
		{
			CHandle<CBaseEntity> hEntity;
			hEntity = pEntity;

			if ( m_hTouchingEntities.Find( hEntity ) >= 0 )
			{
				if ( CheckVelocity( pEntity, true ) )
				{
					m_OnVelocity.FireOutput( this, this );
				}
				else
				{
					m_OnVelocityTransitionFail.FireOutput( this, this );
					m_FailEntities.AddToTail( hEntity );
				}
			}

			m_VelEntities.Remove( i );
		}
	}

	// Forget failed entities once they have left the trigger.
	for ( int i = 0; i < m_FailEntities.Count(); i++ )
	{
		if ( m_hTouchingEntities.Find( m_FailEntities[i] ) < 0 )
		{
			m_FailEntities.Remove( i );
		}
	}

	SetNextThink( gpGlobals->curtime + 0.1f, "triggervelocitycheckthink" );
}
