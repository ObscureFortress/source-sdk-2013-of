//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: Worker Node's Wall
//
// $NoKeywords: $
//=============================================================================//
#include "cbase.h"

#include "fo_obj_wall.h"
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
#define WALL_MODEL_PLACEMENT			"models/buildables/wall_blueprint.mdl"
#define WALL_MODEL_BUILDING			"models/buildables/wall_heavy.mdl"
#define WALL_MODEL					"models/buildables/wall_light.mdl"
#define WALL_MODEL_LEVEL_2_UPGRADE	"models/buildables/wall_level2_heavy.mdl"
#define WALL_MODEL_LEVEL_2			"models/buildables/wall_level2_light.mdl"

#define WALL_MINS				Vector( -20, -20, 0 )
#define WALL_MAXS				Vector( 20, 20, 110 )

#define WALL_MAX_HEALTH			200
#define WALL_UPGRADE_METAL		50
#define WALL_UPGRADE_DURATION		1.5f
#define WALL_MINIGUN_RESIST_LVL_1	0.2f
#define WALL_MINIGUN_RESIST_LVL_2	0.33f
#define WALL_THINK_DELAY			0.05
#define WALL_CONTEXT				"WallContext"

IMPLEMENT_SERVERCLASS_ST( CObjectWorkerWall, DT_ObjectWorkerWall )
	SendPropInt( SENDINFO( m_iUpgradeLevel ), 2 ),
	SendPropInt( SENDINFO( m_iState ), Q_log2( WALL_NUM_STATES ) + 1, SPROP_UNSIGNED ),
	SendPropInt( SENDINFO( m_iUpgradeMetal ), 10 ),
END_SEND_TABLE()

BEGIN_DATADESC( CObjectWorkerWall )
	DEFINE_THINKFUNC( WallThink ),
END_DATADESC()

LINK_ENTITY_TO_CLASS( obj_wall, CObjectWorkerWall );
PRECACHE_REGISTER( obj_wall );

extern ConVar tf_cheapobjects;
ConVar tf_wall_upgrade_per_hit( "tf_wall_upgrade_per_hit", "25", FCVAR_CHEAT | FCVAR_DEVELOPMENTONLY );

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
CObjectWorkerWall::CObjectWorkerWall()
{
	SetMaxHealth( WALL_MAX_HEALTH );
	m_iHealth = WALL_MAX_HEALTH;

	SetType( OBJ_WALL );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CObjectWorkerWall::Spawn()
{
	SetModel( WALL_MODEL_PLACEMENT );

	m_takedamage = DAMAGE_YES;

	m_iUpgradeLevel = 1;
	m_iUpgradeMetal = 0;
	m_iUpgradeMetalRequired = WALL_UPGRADE_METAL;

	SetMaxHealth( WALL_MAX_HEALTH );
	SetHealth( WALL_MAX_HEALTH );

	// Pipes explode when they hit this
	m_takedamage = DAMAGE_AIM;

	m_flHeavyBulletResist = WALL_MINIGUN_RESIST_LVL_1;

	BaseClass::Spawn();

	UTIL_SetSize( this, WALL_MINS, WALL_MAXS );

	m_iState.Set( WALL_STATE_INACTIVE );

	SetContextThink( &CObjectWorkerWall::WallThink, gpGlobals->curtime + WALL_THINK_DELAY, WALL_CONTEXT );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CObjectWorkerWall::WallThink( void )
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

	SetContextThink( &CObjectWorkerWall::WallThink, gpGlobals->curtime + WALL_THINK_DELAY, WALL_CONTEXT );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CObjectWorkerWall::StartPlacement( CTFPlayer *pPlayer )
{
	BaseClass::StartPlacement( pPlayer );

	// Set my build size
	m_vecBuildMins = WALL_MINS;
	m_vecBuildMaxs = WALL_MAXS;
	m_vecBuildMins -= Vector( 4,4,0 );
	m_vecBuildMaxs += Vector( 4,4,0 );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool CObjectWorkerWall::StartBuilding( CBaseEntity *pBuilder )
{
	SetModel( WALL_MODEL_BUILDING );

	return BaseClass::StartBuilding( pBuilder );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CObjectWorkerWall::OnGoActive( void )
{
	CTFPlayer *pBuilder = GetBuilder();

	Assert( pBuilder );

	if ( !pBuilder )
		return;

	SetModel( WALL_MODEL );

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
void CObjectWorkerWall::Precache()
{
	BaseClass::Precache();

	int iModelIndex;

	PrecacheModel( WALL_MODEL_PLACEMENT );

	iModelIndex = PrecacheModel( WALL_MODEL_BUILDING );
	PrecacheGibsForModel( iModelIndex );

	iModelIndex = PrecacheModel( WALL_MODEL );
	PrecacheGibsForModel( iModelIndex );

	iModelIndex = PrecacheModel( WALL_MODEL_LEVEL_2_UPGRADE );
	PrecacheGibsForModel( iModelIndex );

	iModelIndex = PrecacheModel( WALL_MODEL_LEVEL_2 );
	PrecacheGibsForModel( iModelIndex );

	PrecacheParticleSystem( "sentrydamage_1" );
	PrecacheParticleSystem( "sentrydamage_2" );
	PrecacheParticleSystem( "sentrydamage_3" );
	PrecacheParticleSystem( "sentrydamage_4" );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool CObjectWorkerWall::CanBeUpgraded( CTFPlayer *pPlayer )
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
void CObjectWorkerWall::StartUpgrading( void )
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
		SetModel( WALL_MODEL_LEVEL_2_UPGRADE );
		m_flHeavyBulletResist = WALL_MINIGUN_RESIST_LVL_2;
		break;
	default:
		Assert(0);
		break;
	}

	m_iState.Set( WALL_STATE_UPGRADING );

	SetActivity( ACT_OBJ_UPGRADING );

	m_flUpgradeCompleteTime = gpGlobals->curtime + WALL_UPGRADE_DURATION;

	RemoveAllGestures();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CObjectWorkerWall::FinishUpgrading( void )
{
	m_iState.Set( WALL_STATE_ACTIVE );

	switch( m_iUpgradeLevel )
	{
	case 2:
		SetModel( WALL_MODEL_LEVEL_2 );
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
void CObjectWorkerWall::UpgradeThink( void )
{
	if ( gpGlobals->curtime > m_flUpgradeCompleteTime )
	{
		FinishUpgrading();
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool CObjectWorkerWall::IsUpgrading( void ) const
{
	return ( m_iState == WALL_STATE_UPGRADING );
}

//-----------------------------------------------------------------------------
// Purpose: Hit by a friendly engineer's wrench
//-----------------------------------------------------------------------------
bool CObjectWorkerWall::OnWrenchHit( CTFPlayer *pPlayer )
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
		int iAmountToAdd = min( tf_wall_upgrade_per_hit.GetInt(), iPlayerMetal );

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
int CObjectWorkerWall::DrawDebugTextOverlays( void ) 
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
int CObjectWorkerWall::OnTakeDamage( const CTakeDamageInfo &info )
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
void CObjectWorkerWall::Killed( const CTakeDamageInfo &info )
{
	// do normal handling
	BaseClass::Killed( info );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CObjectWorkerWall::SetModel( const char *pModel )
{
	BaseClass::SetModel( pModel );

	// Reset this after model change
	SetSolid( SOLID_VPHYSICS );
	VPhysicsInitStatic();
	CollisionProp()->SetSurroundingBoundsType( USE_HITBOXES );

	ResetSequenceInfo();
}
