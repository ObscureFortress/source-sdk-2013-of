//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: Worker Node's Fort
//
// $NoKeywords: $
//=============================================================================//
#include "cbase.h"

#include "fo_obj_fort.h"
#include "engine/IEngineSound.h"
#include "tf_player.h"
#include "tf_team.h"
#include "vguiscreen.h"
#include "world.h"
#include "explode.h"
#include "triggers.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

// Ground placed version
#define FORT_MODEL_PLACEMENT			"models/buildables/fort_blueprint.mdl"
#define FORT_MODEL_BUILDING			"models/buildables/fort_heavy.mdl"
#define FORT_MODEL					"models/buildables/fort_light.mdl"
#define FORT_MODEL_LEVEL_2_UPGRADE	"models/buildables/fort_level2_heavy.mdl"
#define FORT_MODEL_LEVEL_2			"models/buildables/fort_level2_light.mdl"

#define FORT_MINS				Vector( -60, -60, 0 )
#define FORT_MAXS				Vector( 60, 60, 110 )

#define FORT_MAX_HEALTH			450
#define FORT_UPGRADE_METAL		100
#define FORT_UPGRADE_DURATION		2.5f
#define FORT_MINIGUN_RESIST_LVL_1	0.2f
#define FORT_MINIGUN_RESIST_LVL_2	0.33f
#define FORT_THINK_DELAY			0.05
#define FORT_CONTEXT				"FortContext"

IMPLEMENT_SERVERCLASS_ST( CObjectWorkerFort, DT_ObjectWorkerFort )
	SendPropInt( SENDINFO( m_iUpgradeLevel ), 2 ),
	SendPropInt( SENDINFO( m_iState ), Q_log2( WALL_NUM_STATES ) + 1, SPROP_UNSIGNED ),
	SendPropInt( SENDINFO( m_iUpgradeMetal ), 10 ),
END_SEND_TABLE()

BEGIN_DATADESC( CObjectWorkerFort )
END_DATADESC()

LINK_ENTITY_TO_CLASS( obj_fort, CObjectWorkerFort );
PRECACHE_REGISTER( obj_fort );

extern ConVar tf_cheapobjects;
ConVar tf_fort_upgrade_per_hit( "tf_fort_upgrade_per_hit", "25", FCVAR_CHEAT | FCVAR_DEVELOPMENTONLY );

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
CObjectWorkerFort::CObjectWorkerFort()
{
	SetMaxHealth( FORT_MAX_HEALTH );
	m_iHealth = FORT_MAX_HEALTH;

	SetType( OBJ_FORT );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CObjectWorkerFort::Spawn()
{
	SetModel( FORT_MODEL_PLACEMENT );

	m_takedamage = DAMAGE_YES;

	m_iUpgradeLevel = 1;
	m_iUpgradeMetal = 0;
	m_iUpgradeMetalRequired = FORT_UPGRADE_METAL;

	SetMaxHealth( FORT_MAX_HEALTH );
	SetHealth( FORT_MAX_HEALTH );

	// Pipes explode when they hit this
	m_takedamage = DAMAGE_AIM;

	m_flHeavyBulletResist = FORT_MINIGUN_RESIST_LVL_1;

	BaseClass::Spawn();

	UTIL_SetSize( this, FORT_MINS, FORT_MAXS );

	m_iState.Set( WALL_STATE_INACTIVE );

	SetContextThink( &CObjectWorkerFort::FortThink, gpGlobals->curtime + FORT_THINK_DELAY, FORT_CONTEXT );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CObjectWorkerFort::FortThink( void )
{
	switch( m_iState )
	{
	case WALL_STATE_INACTIVE:
		break;

	case WALL_STATE_ACTIVE:
		break;

	case WALL_STATE_ATTACKING:
		break;

	case WALL_STATE_UPGRADING:
		UpgradeThink();
		break;

	default:
		Assert( 0 );
		break;
	}

	SetContextThink( &CObjectWorkerFort::FortThink, gpGlobals->curtime + FORT_THINK_DELAY, FORT_CONTEXT );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CObjectWorkerFort::StartPlacement( CTFPlayer *pPlayer )
{
	BaseClass::StartPlacement( pPlayer );

	// Set my build size
	m_vecBuildMins = FORT_MINS;
	m_vecBuildMaxs = FORT_MAXS;
	m_vecBuildMins -= Vector( 4,4,0 );
	m_vecBuildMaxs += Vector( 4,4,0 );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool CObjectWorkerFort::StartBuilding( CBaseEntity *pBuilder )
{
	SetModel( FORT_MODEL_BUILDING );

	return BaseClass::StartBuilding( pBuilder );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CObjectWorkerFort::OnGoActive( void )
{
	CTFPlayer *pBuilder = GetBuilder();

	Assert( pBuilder );

	if ( !pBuilder )
		return;

	SetModel( FORT_MODEL );

	m_iState.Set( WALL_STATE_ACTIVE );

	SetSolid( SOLID_VPHYSICS );
	VPhysicsInitStatic();
	CollisionProp()->SetSurroundingBoundsType( USE_HITBOXES );

	EmitSound( "Building_Sentrygun.Built" );

	BaseClass::OnGoActive();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CObjectWorkerFort::Precache()
{
	BaseClass::Precache();

	int iModelIndex;

	PrecacheModel( FORT_MODEL_PLACEMENT );

	iModelIndex = PrecacheModel( FORT_MODEL_BUILDING );
	PrecacheGibsForModel( iModelIndex );

	iModelIndex = PrecacheModel( FORT_MODEL );
	PrecacheGibsForModel( iModelIndex );

	iModelIndex = PrecacheModel( FORT_MODEL_LEVEL_2_UPGRADE );
	PrecacheGibsForModel( iModelIndex );

	iModelIndex = PrecacheModel( FORT_MODEL_LEVEL_2 );
	PrecacheGibsForModel( iModelIndex );

	PrecacheParticleSystem( "sentrydamage_1" );
	PrecacheParticleSystem( "sentrydamage_2" );
	PrecacheParticleSystem( "sentrydamage_3" );
	PrecacheParticleSystem( "sentrydamage_4" );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool CObjectWorkerFort::CanBeUpgraded( CTFPlayer *pPlayer )
{
	// Already upgrading
	if ( m_iState == WALL_STATE_UPGRADING )
	{
		return false;
	}

	// only engineers
	if ( !ClassCanBuild( pPlayer->GetPlayerClass()->GetClassIndex(), GetType() ) )
	{
		return false;
	}

	// max upgraded
	if ( m_iUpgradeLevel >= 2 )
	{
		return false;
	}

	return true;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CObjectWorkerFort::StartUpgrading( void )
{
	// Increase level
	m_iUpgradeLevel++;

	// more health
	int iMaxHealth = GetMaxHealth();
	SetMaxHealth( iMaxHealth * 1.5 );
	SetHealth( iMaxHealth * 1.5 );

	EmitSound( "Building_Sentrygun.Built" );

	switch( m_iUpgradeLevel )
	{
	case 2:
		SetModel( FORT_MODEL_LEVEL_2_UPGRADE );
		m_flHeavyBulletResist = FORT_MINIGUN_RESIST_LVL_2;
		break;
	default:
		Assert(0);
		break;
	}

	m_iState.Set( WALL_STATE_UPGRADING );

	SetActivity( ACT_OBJ_UPGRADING );

	m_flUpgradeCompleteTime = gpGlobals->curtime + FORT_UPGRADE_DURATION;

	RemoveAllGestures();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CObjectWorkerFort::FinishUpgrading( void )
{
	m_iState.Set( WALL_STATE_ACTIVE );

	switch( m_iUpgradeLevel )
	{
	case 2:
		SetModel( FORT_MODEL_LEVEL_2 );
		break;
	default:
		Assert(0);
		break;
	}

	EmitSound( "Building_Sentrygun.Built" );
}

//-----------------------------------------------------------------------------
// Purpose: Playing the upgrade animation
//-----------------------------------------------------------------------------
void CObjectWorkerFort::UpgradeThink( void )
{
	if ( gpGlobals->curtime > m_flUpgradeCompleteTime )
	{
		FinishUpgrading();
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool CObjectWorkerFort::IsUpgrading( void ) const
{
	return ( m_iState == WALL_STATE_UPGRADING );
}

//-----------------------------------------------------------------------------
// Purpose: Hit by a friendly engineer's wrench
//-----------------------------------------------------------------------------
bool CObjectWorkerFort::OnWrenchHit( CTFPlayer *pPlayer )
{
	bool bDidWork = false;

	// If the player repairs it at all, we're done
	if ( GetHealth() < GetMaxHealth() )
	{
		if ( Command_Repair( pPlayer ) )
		{
			bDidWork = true;
		}
	}

	// Don't put in upgrade metal until it is fully healed
	if ( !bDidWork && CanBeUpgraded( pPlayer ) )
	{
		int iPlayerMetal = pPlayer->GetAmmoCount( TF_AMMO_METAL );
		int iAmountToAdd = min( tf_fort_upgrade_per_hit.GetInt(), iPlayerMetal );

		if ( iAmountToAdd > ( m_iUpgradeMetalRequired - m_iUpgradeMetal ) )
			iAmountToAdd = ( m_iUpgradeMetalRequired - m_iUpgradeMetal );

		if ( tf_cheapobjects.GetBool() == false )
		{
			pPlayer->RemoveAmmo( iAmountToAdd, TF_AMMO_METAL );
		}
		m_iUpgradeMetal += iAmountToAdd;

		if ( iAmountToAdd > 0 )
		{
			bDidWork = true;
		}

		if ( m_iUpgradeMetal >= m_iUpgradeMetalRequired )
		{
			StartUpgrading();
			m_iUpgradeMetal = 0;
		}
	}

	return bDidWork;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
int CObjectWorkerFort::DrawDebugTextOverlays( void ) 
{
	int text_offset = BaseClass::DrawDebugTextOverlays();

	if (m_debugOverlays & OVERLAY_TEXT_BIT) 
	{
		char tempstr[512];

		Q_snprintf( tempstr, sizeof( tempstr ), "Level: %d", m_iUpgradeLevel.Get() );
		EntityText( text_offset, tempstr, 0 );
		text_offset++;

		Q_snprintf( tempstr, sizeof( tempstr ), "Upgrade metal %d", m_iUpgradeMetal.Get() );
		EntityText( text_offset, tempstr, 0 );
		text_offset++;
	}

	return text_offset;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
int CObjectWorkerFort::OnTakeDamage( const CTakeDamageInfo &info )
{
	CTakeDamageInfo newInfo = info;

	// As we increase in level, we get more resistant to minigun bullets, to compensate for
	// our increased surface area taking more minigun hits.
	if ( ( info.GetDamageType() & DMG_BULLET ) && ( info.GetDamageCustom() == TF_DMG_CUSTOM_MINIGUN ) )
	{
		float flDamage = newInfo.GetDamage();

		flDamage *= ( 1.0 - m_flHeavyBulletResist );

		newInfo.SetDamage( flDamage );
	}

	int iDamageTaken = BaseClass::OnTakeDamage( newInfo );

	if ( iDamageTaken > 0 )
	{
		m_flLastAttackedTime = gpGlobals->curtime;
	}

	return iDamageTaken;
}

//-----------------------------------------------------------------------------
// Purpose: Called when this object is destroyed
//-----------------------------------------------------------------------------
void CObjectWorkerFort::Killed( const CTakeDamageInfo &info )
{
	// do normal handling
	BaseClass::Killed( info );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CObjectWorkerFort::SetModel( const char *pModel )
{
	BaseClass::SetModel( pModel );

	// Reset this after model change
	SetSolid( SOLID_VPHYSICS );
	VPhysicsInitStatic();
	CollisionProp()->SetSurroundingBoundsType( USE_HITBOXES );

	ResetSequenceInfo();
}
