//====== Copyright (c) 1996-2005, Valve Corporation, All rights reserved. =======
//
// Purpose: Fortress Obscura hands - a beam weapon that locks on to an enemy in front of
//			the owner and holds on to them for as long as the attack is held.
//
//=============================================================================

#include "cbase.h"
#include "in_buttons.h"
#include "engine/IEngineSound.h"
#include "tf_gamerules.h"

#if defined( CLIENT_DLL )
#include <vgui_controls/Panel.h>
#include <vgui/ISurface.h>
#include "particles_simple.h"
#include "c_tf_player.h"
#include "soundenvelope.h"
#else
#include "tf_player.h"
#endif

#include "fo_weapon_hands.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

// Buff ranges
ConVar weapon_hands_damage_modifier( "weapon_hands_damage_modifier", "1.5", FCVAR_CHEAT | FCVAR_REPLICATED | FCVAR_DEVELOPMENTONLY, "Scales the damage a player does while being healed with the medigun." );
ConVar weapon_hands_construction_rate( "weapon_hands_construction_rate", "10", FCVAR_CHEAT | FCVAR_REPLICATED | FCVAR_DEVELOPMENTONLY, "Constructing object health healed per second by the medigun." );
ConVar weapon_hands_charge_rate( "weapon_hands_charge_rate", "40", FCVAR_CHEAT | FCVAR_REPLICATED | FCVAR_DEVELOPMENTONLY, "Amount of time healing it takes to fully charge the medigun." );
ConVar weapon_hands_chargerelease_rate( "weapon_hands_chargerelease_rate", "8", FCVAR_CHEAT | FCVAR_REPLICATED | FCVAR_DEVELOPMENTONLY, "Amount of time it takes the a full charge of the medigun to be released." );

#if defined (CLIENT_DLL)
ConVar tf_hands_autosteal( "tf_hands_autosteal", "0", FCVAR_CLIENTDLL | FCVAR_ARCHIVE | FCVAR_USERINFO, "Setting this to 1 will cause the Hands's primary attack to be a toggle instead of needing to be held down." );
#endif

#ifdef CLIENT_DLL
//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void RecvProxy_StealingTarget( const CRecvProxyData *pData, void *pStruct, void *pOut )
{
	CWeaponHands *pHands = ((CWeaponHands*)(pStruct));
	if ( pHands != NULL )
	{
		pHands->ForceStealingTargetUpdate();
	}

	RecvProxy_IntToEHandle( pData, pStruct, pOut );
}
#endif

LINK_ENTITY_TO_CLASS( fo_weapon_hands, CWeaponHands );
PRECACHE_WEAPON_REGISTER( fo_weapon_hands );

IMPLEMENT_NETWORKCLASS_ALIASED( WeaponHands, DT_WeaponHands )

BEGIN_NETWORK_TABLE( CWeaponHands, DT_WeaponHands )
#if !defined( CLIENT_DLL )
	SendPropFloat( SENDINFO(m_flChargeLevel), 0, SPROP_NOSCALE | SPROP_CHANGES_OFTEN ),
	SendPropEHandle( SENDINFO( m_hStealingTarget ) ),
	SendPropBool( SENDINFO( m_bStealing ) ),
	SendPropBool( SENDINFO( m_bAttacking ) ),
	SendPropBool( SENDINFO( m_bChargeRelease ) ),
	SendPropBool( SENDINFO( m_bHolstered ) ),
#else
	RecvPropFloat( RECVINFO(m_flChargeLevel) ),
	RecvPropEHandle( RECVINFO( m_hStealingTarget ), RecvProxy_StealingTarget ),
	RecvPropBool( RECVINFO( m_bStealing ) ),
	RecvPropBool( RECVINFO( m_bAttacking ) ),
	RecvPropBool( RECVINFO( m_bChargeRelease ) ),
	RecvPropBool( RECVINFO( m_bHolstered ) ),
#endif
END_NETWORK_TABLE()

#ifdef CLIENT_DLL
BEGIN_PREDICTION_DATA( CWeaponHands  )

	DEFINE_PRED_FIELD( m_bStealing, FIELD_BOOLEAN, FTYPEDESC_INSENDTABLE ),
	DEFINE_PRED_FIELD( m_bAttacking, FIELD_BOOLEAN, FTYPEDESC_INSENDTABLE ),
	DEFINE_PRED_FIELD( m_bHolstered, FIELD_BOOLEAN, FTYPEDESC_INSENDTABLE ),
	DEFINE_PRED_FIELD( m_hStealingTarget, FIELD_EHANDLE, FTYPEDESC_INSENDTABLE ),

	DEFINE_FIELD( m_flStealEffectLifetime, FIELD_FLOAT ),

	DEFINE_PRED_FIELD( m_flChargeLevel, FIELD_FLOAT, FTYPEDESC_INSENDTABLE ),
	DEFINE_PRED_FIELD( m_bChargeRelease, FIELD_BOOLEAN, FTYPEDESC_INSENDTABLE ),

END_PREDICTION_DATA()
#endif

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
CWeaponHands::CWeaponHands( void )
{
	WeaponReset();

	SetPredictionEligible( true );
}

CWeaponHands::~CWeaponHands()
{
#ifdef CLIENT_DLL
	if ( m_pChargedSound )
	{
		CSoundEnvelopeController::GetController().SoundDestroy( m_pChargedSound );
	}
#endif
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CWeaponHands::WeaponReset( void )
{
	BaseClass::WeaponReset();

	m_flStealEffectLifetime = 0;

	m_bStealing = false;
	m_bAttacking = false;
	m_bHolstered = true;
	m_bChargeRelease = false;

	m_bCanChangeTarget = true;

	m_flNextBuzzTime = 0;
	m_flReleaseStartedAt = 0;
	m_flChargeLevel = 0.0f;

	RemoveStealingTarget( true );

#if defined( CLIENT_DLL )
	m_bPlayingSound = false;
	m_bUpdateStealingTargets = false;
	m_bOldChargeRelease = false;

	UpdateEffects();
	ManageChargeEffect();

	m_pChargeEffect = NULL;
	m_pChargedSound = NULL;
#endif

}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CWeaponHands::Precache()
{
	BaseClass::Precache();
	PrecacheScriptSound( "WeaponMedigun.NoTarget" );
	PrecacheScriptSound( "WeaponMedigun.Healing" );
	PrecacheScriptSound( "WeaponMedigun.Charged" );
	PrecacheParticleSystem( "medicgun_invulnstatus_fullcharge_blue" );
	PrecacheParticleSystem( "medicgun_invulnstatus_fullcharge_red" );
	PrecacheParticleSystem( "medicgun_beam_red_invun" );
	PrecacheParticleSystem( "medicgun_beam_red" );
	PrecacheParticleSystem( "medicgun_beam_blue_invun" );
	PrecacheParticleSystem( "medicgun_beam_blue" );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool CWeaponHands::Deploy( void )
{
	if ( BaseClass::Deploy() )
	{
		m_bHolstered = false;


#ifdef CLIENT_DLL
		ManageChargeEffect();
#endif

		m_flNextTargetCheckTime = gpGlobals->curtime;

		return true;
	}

	return false;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool CWeaponHands::Holster( CBaseCombatWeapon *pSwitchingTo )
{
	RemoveStealingTarget( true );
	m_bAttacking = false;
	m_bHolstered = true;


#ifdef CLIENT_DLL
	UpdateEffects();
	ManageChargeEffect();
#endif

	return BaseClass::Holster( pSwitchingTo );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CWeaponHands::UpdateOnRemove( void )
{
	RemoveStealingTarget( true );
	m_bAttacking = false;

#ifdef CLIENT_DLL
	if ( m_bPlayingSound )
	{
		m_bPlayingSound = false;
		StopStealSound();
	}

	UpdateEffects();
#endif

	BaseClass::UpdateOnRemove();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
float CWeaponHands::GetTargetRange( void )
{
	return (float)m_pWeaponInfo->GetWeaponData( m_iWeaponMode ).m_flRange;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
float CWeaponHands::GetStickRange( void )
{
	return (GetTargetRange() * 1.2);
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
float CWeaponHands::GetStealRate( void )
{
	return (float)m_pWeaponInfo->GetWeaponData( m_iWeaponMode ).m_nDamage;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool CWeaponHands::StealingTarget( CBaseEntity *pTarget )
{
	if ( pTarget == m_hStealingTarget )
		return true;

	return false;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool CWeaponHands::AllowedToStealTarget( CBaseEntity *pTarget )
{
	CTFPlayer *pOwner = ToTFPlayer( GetOwnerEntity() );
	if ( !pOwner )
		return false;

	CTFPlayer *pTFPlayer = ToTFPlayer( pTarget );
	if ( !pTFPlayer )
		return false;

	bool bStealthed = pTFPlayer->m_Shared.InCond( TF_COND_STEALTHED );
	bool bDisguised = pTFPlayer->m_Shared.InCond( TF_COND_DISGUISED );

	// We can steal from enemies and from teammates that are disguised as the enemy
	if ( !bStealthed &&
		( !pTFPlayer->InSameTeam( pOwner ) ||
		( bDisguised && pTFPlayer->m_Shared.GetDisguiseTeam() != pOwner->GetTeamNumber() ) ) )
	{
		return true;
	}

	return false;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool CWeaponHands::CouldStealTarget( CBaseEntity *pTarget )
{
	CTFPlayer *pOwner = ToTFPlayer( GetOwnerEntity() );
	if ( !pOwner )
		return false;

	if ( pTarget->IsPlayer() && pTarget->IsAlive() && !StealingTarget(pTarget) )
		return AllowedToStealTarget( pTarget );

	return false;
}

// Now make sure there isn't something other than team players in the way.
class CHandsFilter : public CTraceFilterSimple
{
public:
	CHandsFilter( CBaseEntity *pShooter ) : CTraceFilterSimple( pShooter, COLLISION_GROUP_WEAPON )
	{
		m_pShooter = pShooter;
	}

	virtual bool ShouldHitEntity( IHandleEntity *pHandleEntity, int contentsMask )
	{
		// If it hit an edict the isn't the target and is on our team, then the ray is blocked.
		CBaseEntity *pEnt = static_cast<CBaseEntity*>(pHandleEntity);

		// Ignore collisions with the shooter
		if ( pEnt == m_pShooter )
			return false;
		
		if ( pEnt->GetTeam() == m_pShooter->GetTeam() )
			return false;

		return CTraceFilterSimple::ShouldHitEntity( pHandleEntity, contentsMask );
	}

	CBaseEntity	*m_pShooter;
};

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CWeaponHands::MaintainTargetInSlot()
{
	CTFPlayer *pOwner = ToTFPlayer( GetOwnerEntity() );
	if ( !pOwner )
		return;

	CBaseEntity *pTarget = m_hStealingTarget;
	Assert( pTarget );

	// Make sure the guy didn't go out of range.
	bool bLostTarget = true;
	Vector vecSrc = pOwner->Weapon_ShootPosition( );
	Vector vecTargetPoint = pTarget->WorldSpaceCenter();
	Vector vecPoint;

	// If it's brush built, use absmins/absmaxs
	pTarget->CollisionProp()->CalcNearestPoint( vecSrc, &vecPoint );

	float flDistance = (vecPoint - vecSrc).Length();
	if ( flDistance < GetStickRange() )
	{
		if ( m_flNextTargetCheckTime > gpGlobals->curtime )
			return;

		m_flNextTargetCheckTime = gpGlobals->curtime + 1.0f;

		trace_t tr;
		CHandsFilter drainFilter( pOwner );

		Vector vecAiming;
		pOwner->EyeVectors( &vecAiming );

		Vector vecEnd = vecSrc + vecAiming * GetTargetRange();
		UTIL_TraceLine( vecSrc, vecEnd, (MASK_SHOT & ~CONTENTS_HITBOX), pOwner, DMG_GENERIC, &tr );

		// Still visible?
		if ( tr.m_pEnt == pTarget )
			return;

		UTIL_TraceLine( vecSrc, vecTargetPoint, MASK_SHOT, &drainFilter, &tr );

		// Still visible?
		if (( tr.fraction == 1.0f) || (tr.m_pEnt == pTarget))
			return;

		// If we failed, try the target's eye point as well
		UTIL_TraceLine( vecSrc, pTarget->EyePosition(), MASK_SHOT, &drainFilter, &tr );
		if (( tr.fraction == 1.0f) || (tr.m_pEnt == pTarget))
			return;
	}

	// We've lost this guy
	if ( bLostTarget )
	{
		RemoveStealingTarget();
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CWeaponHands::FindNewTargetForSlot()
{
	CTFPlayer *pOwner = ToTFPlayer( GetOwnerEntity() );
	if ( !pOwner )
		return;

	Vector vecSrc = pOwner->Weapon_ShootPosition( );
	if ( m_hStealingTarget )
	{
		RemoveStealingTarget();
	}

	// In Normal mode, we steal from players under our crosshair
	Vector vecAiming;
	pOwner->EyeVectors( &vecAiming );

	// Find a player in range of this player, and make sure they're stealable.
	Vector vecEnd = vecSrc + vecAiming * GetTargetRange();
	trace_t tr;

	UTIL_TraceLine( vecSrc, vecEnd, (MASK_SHOT & ~CONTENTS_HITBOX), pOwner, DMG_GENERIC, &tr );
	if ( tr.fraction != 1.0 && tr.m_pEnt )
	{
		if ( CouldStealTarget( tr.m_pEnt ) )
		{

			m_hStealingTarget.Set( tr.m_pEnt );
			m_flNextTargetCheckTime = gpGlobals->curtime + 1.0f;
		}			
	}
}


//-----------------------------------------------------------------------------
// Purpose: Returns a pointer to a stealable target
//-----------------------------------------------------------------------------
bool CWeaponHands::FindAndStealTargets( void )
{
	CTFPlayer *pOwner = ToTFPlayer( GetOwnerEntity() );
	if ( !pOwner )
		return false;

	bool bFound = false;

	// Maintaining beam to existing target?
	CBaseEntity *pTarget = m_hStealingTarget;
	if ( pTarget && pTarget->IsAlive() )
	{
		MaintainTargetInSlot();
	}
	else
	{
		FindNewTargetForSlot();
	}

	CBaseEntity *pNewTarget = m_hStealingTarget;
	if ( pNewTarget && pNewTarget->IsAlive() )
	{
		bFound = true;
	}

	return bFound;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CWeaponHands::ItemHolsterFrame( void )
{
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CWeaponHands::DrainCharge( void )
{
	// If we're in charge release mode, drain our charge
	if ( m_bChargeRelease )
	{
		CTFPlayer *pOwner = ToTFPlayer( GetOwnerEntity() );
		if ( !pOwner )
			return;

		float flChargeAmount = gpGlobals->frametime / weapon_hands_chargerelease_rate.GetFloat();
		m_flChargeLevel = max( m_flChargeLevel - flChargeAmount, 0.0 );
		if ( !m_flChargeLevel )
		{
			m_bChargeRelease = false;
			m_flReleaseStartedAt = 0;

		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: Overloaded to handle the hold-down stealing
//-----------------------------------------------------------------------------
void CWeaponHands::ItemPostFrame( void )
{
	CTFPlayer *pOwner = ToTFPlayer( GetOwnerEntity() );
	if ( !pOwner )
		return;

	// If we're lowered, we're not allowed to fire
	if ( CanAttack() == false )
	{
		RemoveStealingTarget( true );
		return;
	}


	// Try to start stealing
	m_bAttacking = false;
	if ( pOwner->GetHandsAutoSteal() )
	{
		if ( pOwner->m_nButtons & IN_ATTACK )
		{
			if ( m_bCanChangeTarget )
			{
				RemoveStealingTarget();
#if defined( CLIENT_DLL )
				m_bPlayingSound = false;
				StopStealSound();
#endif
				// can't change again until we release the attack button
				m_bCanChangeTarget = false;
			}
		}
		else
		{
			m_bCanChangeTarget = true;
		}

		if ( m_bStealing || ( pOwner->m_nButtons & IN_ATTACK ) )
		{
			PrimaryAttack();
			m_bAttacking = true;
		}
	}
	else
	{
		if ( /*m_bChargeRelease || */ pOwner->m_nButtons & IN_ATTACK )
		{
			PrimaryAttack();
			m_bAttacking = true;
		}
 		else if ( m_bStealing )
 		{
 			// Detach from the player if they release the attack button.
 			RemoveStealingTarget();
 		}
	}

	if ( pOwner->m_nButtons & IN_ATTACK2 )
	{
		SecondaryAttack();
	}

	WeaponIdle();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool CWeaponHands::Lower( void )
{
	// Stop stealing if we are
	if ( m_bStealing )
	{
		RemoveStealingTarget( true );
		m_bAttacking = false;

#ifdef CLIENT_DLL
		UpdateEffects();
#endif
	}

	return BaseClass::Lower();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CWeaponHands::RemoveStealingTarget( bool bStopStealingSelf )
{
	CTFPlayer *pOwner = ToTFPlayer( GetOwnerEntity() );
	if ( !pOwner )
		return;


	m_hStealingTarget.Set( NULL );

	// Stop the welding animation
	if ( m_bStealing )
	{
		SendWeaponAnim( ACT_MP_ATTACK_STAND_POSTFIRE );
		pOwner->DoAnimationEvent( PLAYERANIMEVENT_ATTACK_POST );
	}

	m_bStealing = false;

}


//-----------------------------------------------------------------------------
// Purpose: Attempt to steal from any player within range of the hands
//-----------------------------------------------------------------------------
void CWeaponHands::PrimaryAttack( void )
{
	CTFPlayer *pOwner = ToTFPlayer( GetOwnerEntity() );
	if ( !pOwner )
		return;


	if ( !CanAttack() )
		return;

	if ( FindAndStealTargets() )
	{
		// Start the animation
		if ( !m_bStealing )
		{

			SendWeaponAnim( ACT_MP_ATTACK_STAND_PREFIRE );
			pOwner->DoAnimationEvent( PLAYERANIMEVENT_ATTACK_PRE );
		}

		m_bStealing = true;
	}
	else
	{
		RemoveStealingTarget();
	}
	
}

//-----------------------------------------------------------------------------
// Purpose: Idle tests to see if we're facing a valid target for the hands
//			If so, move into the "steal-able" animation. 
//			Otherwise, move into the "not-steal-able" animation.
//-----------------------------------------------------------------------------
void CWeaponHands::WeaponIdle( void )
{
	if ( HasWeaponIdleTimeElapsed() )
	{
		// Loop the welding animation
		if ( m_bStealing )
		{
			SendWeaponAnim( ACT_VM_PRIMARYATTACK );
			return;
		}

		return BaseClass::WeaponIdle();
	}
}

#if defined( CLIENT_DLL )
//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CWeaponHands::StopStealSound( bool bStopStealingSound, bool bStopNoTargetSound )
{
	if ( bStopStealingSound )
	{
		StopSound( "WeaponMedigun.Healing" );
	}

	if ( bStopNoTargetSound )
	{
		StopSound( "WeaponMedigun.NoTarget" );
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CWeaponHands::ManageChargeEffect( void )
{
	C_TFPlayer *pLocalPlayer = C_TFPlayer::GetLocalTFPlayer();
	C_BaseEntity *pEffectOwner = this;

	if ( pLocalPlayer == NULL )
		return;

	if ( pLocalPlayer == GetTFPlayerOwner() )
	{
		pEffectOwner = pLocalPlayer->GetViewModel();
		if ( !pEffectOwner )
			return;
	}

	bool bOwnerTaunting = false;

	if ( GetTFPlayerOwner() && GetTFPlayerOwner()->m_Shared.InCond( TF_COND_TAUNTING ) == true )
	{
		bOwnerTaunting = true;
	}

	if ( GetTFPlayerOwner() && bOwnerTaunting == false && m_bHolstered == false && ( m_flChargeLevel >= 1.0f || m_bChargeRelease == true ) )
	{
		if ( m_pChargeEffect == NULL )
		{
			char *pszEffectName = NULL;

			switch( GetTFPlayerOwner()->GetTeamNumber() )
			{
			case TF_TEAM_BLUE:
				pszEffectName = "medicgun_invulnstatus_fullcharge_blue";
				break;
			case TF_TEAM_RED:
				pszEffectName = "medicgun_invulnstatus_fullcharge_red";
				break;
			default:
				pszEffectName = "";
				break;
			}

			m_pChargeEffect = pEffectOwner->ParticleProp()->Create( pszEffectName, PATTACH_POINT_FOLLOW, "muzzle" );
		}

		if ( m_pChargedSound == NULL )
		{
			CLocalPlayerFilter filter;

			CSoundEnvelopeController &controller = CSoundEnvelopeController::GetController();

			m_pChargedSound = controller.SoundCreate( filter, entindex(), "WeaponMedigun.Charged" );
			controller.Play( m_pChargedSound, 1.0, 100 );
		}
	}
	else
	{
		if ( m_pChargeEffect != NULL )
		{
			pEffectOwner->ParticleProp()->StopEmission( m_pChargeEffect );
			m_pChargeEffect = NULL;
		}

		if ( m_pChargedSound != NULL )
		{
			CSoundEnvelopeController::GetController().SoundDestroy( m_pChargedSound );
			m_pChargedSound = NULL;
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : updateType - 
//-----------------------------------------------------------------------------
void CWeaponHands::OnDataChanged( DataUpdateType_t updateType )
{
	BaseClass::OnDataChanged( updateType );

	if ( m_bUpdateStealingTargets )
	{
		UpdateEffects();
		m_bUpdateStealingTargets = false;
	}

	// Think?
	if ( m_bStealing )
	{
		ClientThinkList()->SetNextClientThink( GetClientHandle(), CLIENT_THINK_ALWAYS );
	}
	else
	{
		ClientThinkList()->SetNextClientThink( GetClientHandle(), CLIENT_THINK_NEVER );
		m_bPlayingSound = false;
		StopStealSound( true, false );

		// Are they holding the attack button but not stealing from anyone? Give feedback.
		if ( IsActiveByLocalPlayer() && GetOwner() && GetOwner()->IsAlive() && m_bAttacking && GetOwner() == C_BasePlayer::GetLocalPlayer() && CanAttack() == true )
		{
			if ( gpGlobals->curtime >= m_flNextBuzzTime )
			{
				CLocalPlayerFilter filter;
				EmitSound( filter, entindex(), "WeaponMedigun.NoTarget" );
				m_flNextBuzzTime = gpGlobals->curtime + 0.5f;	// only buzz every so often.
			}
		}
		else
		{
			StopStealSound( false, true );	// Stop the "no target" sound.
		}
	}

	ManageChargeEffect();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CWeaponHands::ClientThink()
{
	C_TFPlayer *pLocalPlayer = C_TFPlayer::GetLocalTFPlayer();
	if ( !pLocalPlayer )
		return;

	// Don't show it while the player is dead. Ideally, we'd respond to m_bStealing in OnDataChanged,
	// but it stops sending the weapon when it's holstered, and it gets holstered when the player dies.
	CTFPlayer *pFiringPlayer = ToTFPlayer( GetOwnerEntity() );
	if ( !pFiringPlayer || pFiringPlayer->IsPlayerDead() || pFiringPlayer->IsDormant() )
	{
		ClientThinkList()->SetNextClientThink( GetClientHandle(), CLIENT_THINK_NEVER );
		m_bPlayingSound = false;
		StopStealSound();
		return;
	}

	if( m_hStealingTarget != NULL )
	{
		if ( !m_bPlayingSound )
		{
			m_bPlayingSound = true;
			CLocalPlayerFilter filter;
			EmitSound( filter, entindex(), "WeaponMedigun.Healing" );
		}
	}

	if ( m_bOldChargeRelease != m_bChargeRelease )
	{
		m_bOldChargeRelease = m_bChargeRelease;
		ForceStealingTargetUpdate();
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CWeaponHands::UpdateEffects( void )
{
	CTFPlayer *pFiringPlayer = ToTFPlayer( GetOwnerEntity() );
	if ( !pFiringPlayer )
		return;

	C_TFPlayer *pLocalPlayer = C_TFPlayer::GetLocalTFPlayer();
	C_BaseEntity *pEffectOwner = this;
	if ( pLocalPlayer == pFiringPlayer )
	{
		pEffectOwner = pLocalPlayer->GetViewModel();
		if ( !pEffectOwner )
			return;
	}

	// Remove all the effects
	if ( pEffectOwner )
	{
		pEffectOwner->ParticleProp()->StopEmission( m_hStealingTargetEffect.pEffect );
	}
	else
	{
		m_hStealingTargetEffect.pEffect->StopEmission();
	}
	m_hStealingTargetEffect.pTarget = NULL;
	m_hStealingTargetEffect.pEffect = NULL;

	// Don't add targets if the owner is dead or isn't a Saptrap
	if ( !pEffectOwner || pFiringPlayer->IsPlayerDead() || !pFiringPlayer->IsPlayerClass( FO_CLASS_SAPTRAP + 1 ) )
		return;

	// Add our targets
	// Loops through the stealing targets, and make sure we have an effect for each of them
	if ( m_hStealingTarget )
	{
		if ( m_hStealingTargetEffect.pTarget == m_hStealingTarget )
			return;

		const char *pszEffectName;
		if ( pFiringPlayer->GetTeamNumber() == TF_TEAM_RED )
		{
			if ( m_bChargeRelease )
			{
				pszEffectName = "medicgun_beam_red_invun";
			}
			else
			{
				pszEffectName = "medicgun_beam_red";
			}
		}
		else
		{
			if ( m_bChargeRelease )
			{
				pszEffectName = "medicgun_beam_blue_invun";
			}
			else
			{
				pszEffectName = "medicgun_beam_blue";
			}
		}

		CNewParticleEffect *pEffect = pEffectOwner->ParticleProp()->Create( pszEffectName, PATTACH_POINT_FOLLOW, "muzzle" );
		pEffectOwner->ParticleProp()->AddControlPoint( pEffect, 1, m_hStealingTarget, PATTACH_ABSORIGIN_FOLLOW, NULL, Vector(0,0,50) );

		m_hStealingTargetEffect.pTarget = m_hStealingTarget;
		m_hStealingTargetEffect.pEffect = pEffect;
	}
}
#endif
