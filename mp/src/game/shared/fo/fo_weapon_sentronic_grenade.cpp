//====== Copyright © 1996-2005, Valve Corporation, All rights reserved. =======
//
// Purpose: 
//
//=============================================================================
#include "cbase.h"
#include "fo_weapon_sentronic_grenade.h"
#include "tf_fx_shared.h"

// Client specific.
#ifdef CLIENT_DLL
#include "c_tf_player.h"
// Server specific.
#else
#include "tf_player.h"
#include "tf_gamestats.h"
#endif

//=============================================================================
//
// Weapon Sentronic Grenade tables.
//
IMPLEMENT_NETWORKCLASS_ALIASED( FOSentronicGrenade, DT_WeaponSentronicGrenade )

BEGIN_NETWORK_TABLE( CFOSentronicGrenade, DT_WeaponSentronicGrenade)
END_NETWORK_TABLE()

BEGIN_PREDICTION_DATA(CFOSentronicGrenade)
END_PREDICTION_DATA()

LINK_ENTITY_TO_CLASS( fo_weapon_sentronic_grenade, CFOSentronicGrenade);
PRECACHE_WEAPON_REGISTER( fo_weapon_sentronic_grenade );

// Server specific.
#ifndef CLIENT_DLL
BEGIN_DATADESC(CFOSentronicGrenade)
END_DATADESC()
#endif

#define TF_GRENADE_LAUNCER_MIN_VEL 1200

//=============================================================================
//
// Weapon Grenade Launcher functions.
//

//-----------------------------------------------------------------------------
// Purpose: 
// Input  :  - 
//-----------------------------------------------------------------------------
CFOSentronicGrenade::CFOSentronicGrenade()
{
	m_bReloadsSingly = true;
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  :  - 
//-----------------------------------------------------------------------------
CFOSentronicGrenade::~CFOSentronicGrenade()
{
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CFOSentronicGrenade::Spawn( void )
{
	m_iAltFireHint = HINT_ALTFIRE_GRENADELAUNCHER;
	BaseClass::Spawn();
}

//-----------------------------------------------------------------------------
// Purpose: Reset the charge when we holster
//-----------------------------------------------------------------------------
bool CFOSentronicGrenade::Holster( CBaseCombatWeapon *pSwitchingTo )
{
	return BaseClass::Holster( pSwitchingTo );
}

//-----------------------------------------------------------------------------
// Purpose: Reset the charge when we deploy
//-----------------------------------------------------------------------------
bool CFOSentronicGrenade::Deploy( void )
{
	return BaseClass::Deploy();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
int CFOSentronicGrenade::GetMaxClip1( void ) const
{
#ifdef _X360 
	return TF_GRENADE_LAUNCHER_XBOX_CLIP;
#endif

	return BaseClass::GetMaxClip1();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
int CFOSentronicGrenade::GetDefaultClip1( void ) const
{
#ifdef _X360
	return TF_GRENADE_LAUNCHER_XBOX_CLIP;
#endif

	return BaseClass::GetDefaultClip1();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CFOSentronicGrenade::PrimaryAttack( void )
{
	// Check for ammunition.
	if ( m_iClip1 <= 0 && m_iClip1 != -1 )
		return;

	// Are we capable of firing again?
	if ( m_flNextPrimaryAttack > gpGlobals->curtime )
		return;

	if ( !CanAttack() )
	{
		return;
	}

	m_iWeaponMode = TF_WEAPON_PRIMARY_MODE;
	
	LaunchGrenade();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CFOSentronicGrenade::WeaponIdle( void )
{
	BaseClass::WeaponIdle();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CFOSentronicGrenade::LaunchGrenade( void )
{
	// Get the player owning the weapon.
	CTFPlayer *pPlayer = ToTFPlayer( GetPlayerOwner() );
	if ( !pPlayer )
		return;

	CalcIsAttackCritical();

	SendWeaponAnim( ACT_VM_PRIMARYATTACK );

	pPlayer->SetAnimation( PLAYER_ATTACK1 );
	pPlayer->DoAnimationEvent( PLAYERANIMEVENT_ATTACK_PRIMARY );

	FireProjectile( pPlayer );

#if !defined( CLIENT_DLL ) 
	pPlayer->SpeakWeaponFire();
	CTF_GameStats.Event_PlayerFiredWeapon( pPlayer, IsCurrentAttackACrit() );
#endif

	// Set next attack times.
	m_flNextPrimaryAttack = gpGlobals->curtime + m_pWeaponInfo->GetWeaponData( m_iWeaponMode ).m_flTimeFireDelay;

	SetWeaponIdleTime( gpGlobals->curtime + SequenceDuration() );

	// Check the reload mode and behave appropriately.
	if ( m_bReloadsSingly )
	{
		m_iReloadMode.Set( TF_RELOAD_START );
	}
}

float CFOSentronicGrenade::GetProjectileSpeed( void )
{
	return TF_GRENADE_LAUNCER_MIN_VEL;
}

//-----------------------------------------------------------------------------
// Purpose: Detonate this demoman's pipebombs
//-----------------------------------------------------------------------------
void CFOSentronicGrenade::SecondaryAttack( void )
{
#ifdef GAME_DLL

	if ( !CanAttack() )
		return;

	CTFPlayer *pOwner = ToTFPlayer( GetOwner() );
	pOwner->DoClassSpecialSkill();

#endif
}

bool CFOSentronicGrenade::Reload( void )
{
	return BaseClass::Reload();
}
