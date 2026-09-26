//====== Copyright © 1996-2005, Valve Corporation, All rights reserved. =======
//
// Purpose: 
//
//=============================================================================
#include "cbase.h"
#include "fo_weapon_grenade.h"
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
// Weapon Grenade Launcher tables.
//
IMPLEMENT_NETWORKCLASS_ALIASED( FOGrenade, DT_WeaponGrenade )

BEGIN_NETWORK_TABLE( CFOGrenade, DT_WeaponGrenade )
END_NETWORK_TABLE()

BEGIN_PREDICTION_DATA( CFOGrenade )
END_PREDICTION_DATA()

LINK_ENTITY_TO_CLASS( fo_weapon_grenade, CFOGrenade );
PRECACHE_WEAPON_REGISTER( fo_weapon_grenade );

// Server specific.
#ifndef CLIENT_DLL
BEGIN_DATADESC( CFOGrenade )
END_DATADESC()
#endif

#define FO_GRENADE_MIN_VEL 1200

//=============================================================================
//
// Weapon Grenade functions.
//

//-----------------------------------------------------------------------------
// Purpose: 
// Input  :  - 
//-----------------------------------------------------------------------------
CFOGrenade::CFOGrenade()
{
	m_bReloadsSingly = true;
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  :  - 
//-----------------------------------------------------------------------------
CFOGrenade::~CFOGrenade()
{
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CFOGrenade::Spawn( void )
{
	m_iAltFireHint = HINT_ALTFIRE_GRENADE;
	BaseClass::Spawn();
}

//-----------------------------------------------------------------------------
// Purpose: Reset the charge when we holster
//-----------------------------------------------------------------------------
bool CFOGrenade::Holster( CBaseCombatWeapon *pSwitchingTo )
{
	return BaseClass::Holster( pSwitchingTo );
}

//-----------------------------------------------------------------------------
// Purpose: Reset the charge when we deploy
//-----------------------------------------------------------------------------
bool CFOGrenade::Deploy( void )
{
	return BaseClass::Deploy();

	// Check for ammunition.
	if (m_iClip1 <= 0 && m_iClip1 != -1)
		return;

	// Are we capable of firing again?
	if (m_flNextPrimaryAttack > gpGlobals->curtime)
		return;

	if (!CanAttack())
	{
		return;
	}

	m_iWeaponMode = TF_WEAPON_PRIMARY_MODE;

	LaunchGrenade();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
int CFOGrenade::GetMaxClip1( void ) const
{
/*#ifdef _X360 
	return TF_GRENADE_LAUNCHER_XBOX_CLIP;
#endif
*/

	return BaseClass::GetMaxClip1();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
int CFOGrenade::GetDefaultClip1( void ) const
{
/*
#ifdef _X360
	return TF_GRENADE_LAUNCHER_XBOX_CLIP;
#endif
*/

	return BaseClass::GetDefaultClip1();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CFOGrenade::PrimaryAttack( void )
{

}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CFOGrenade::WeaponIdle( void )
{
	BaseClass::WeaponIdle();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CFOGrenade::LaunchGrenade( void )
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

float CFOGrenade::GetProjectileSpeed( void )
{
	return FO_GRENADE_MIN_VEL;
}

//-----------------------------------------------------------------------------
// Purpose: Detonate this demoman's pipebombs
//-----------------------------------------------------------------------------
void CFOGrenade::SecondaryAttack( void )
{
#ifdef GAME_DLL

	if ( !CanAttack() )
		return;

	CTFPlayer *pOwner = ToTFPlayer( GetOwner() );
	pOwner->DoClassSpecialSkill();

#endif
}

bool CFOGrenade::Reload( void )
{
	return BaseClass::Reload();
}
