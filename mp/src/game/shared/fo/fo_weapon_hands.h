//====== Copyright (c) 1996-2005, Valve Corporation, All rights reserved. =======
//
// Purpose: Fortress Obscura hands - a beam weapon that locks on to an enemy in front of
//			the owner and holds on to them for as long as the attack is held.
//
//=============================================================================

#ifndef FO_WEAPON_HANDS_H
#define FO_WEAPON_HANDS_H
#ifdef _WIN32
#pragma once
#endif

#include "tf_weaponbase_gun.h"

#if defined( CLIENT_DLL )
#define CWeaponHands C_WeaponHands
#endif

//=========================================================
// Beam stealing gun
//=========================================================
class CWeaponHands : public CTFWeaponBaseGun
{
	DECLARE_CLASS( CWeaponHands, CTFWeaponBaseGun );
public:
	DECLARE_NETWORKCLASS();
	DECLARE_PREDICTABLE();

	CWeaponHands( void );
	~CWeaponHands( void );

	virtual void	Precache();

	virtual bool	Deploy( void );
	virtual bool	Holster( CBaseCombatWeapon *pSwitchingTo );
	virtual void	UpdateOnRemove( void );
	virtual void	ItemHolsterFrame( void );
	virtual void	ItemPostFrame( void );
	virtual bool	Lower( void );
	virtual void	PrimaryAttack( void );
	virtual void	WeaponIdle( void );
	void			DrainCharge( void );
	virtual void	WeaponReset( void );

	virtual float	GetTargetRange( void );
	virtual float	GetStickRange( void );
	virtual float	GetStealRate( void );
	virtual bool	AppliesModifier( void ) { return true; }

	virtual int		GetWeaponID( void ) const			{ return FO_WEAPON_HANDS; }

	bool			IsReleasingCharge( void ) { return (m_bChargeRelease && !m_bHolstered); }

	CBaseEntity		*GetStealTarget( void ) { return m_hStealingTarget.Get(); }

#if defined( CLIENT_DLL )
	// Stop all sounds being output.
	void			StopStealSound( bool bStopStealingSound = true, bool bStopNoTargetSound = true );

	virtual void	OnDataChanged( DataUpdateType_t updateType );
	virtual void	ClientThink();
	void			UpdateEffects( void );
	void			ForceStealingTargetUpdate( void ) { m_bUpdateStealingTargets = true; }

	void			ManageChargeEffect( void );
#endif

	float			GetChargeLevel( void ) { return m_flChargeLevel; }

private:
	bool					FindAndStealTargets( void );
	void					MaintainTargetInSlot();
	void					FindNewTargetForSlot();
	void					RemoveStealingTarget( bool bStopStealingSelf = false );
	bool					StealingTarget( CBaseEntity *pTarget );
	bool					CouldStealTarget( CBaseEntity *pTarget );
	bool					AllowedToStealTarget( CBaseEntity *pTarget );

public:

#ifdef GAME_DLL
	CNetworkHandle( CBaseEntity, m_hStealingTarget );
#else
	CNetworkHandle( C_BaseEntity, m_hStealingTarget );
#endif

protected:
	// Networked data.
	CNetworkVar( bool,		m_bStealing );
	CNetworkVar( bool,		m_bAttacking );

	double					m_flNextBuzzTime;
	float					m_flStealEffectLifetime;	// Count down until the stealing effect goes off.
	float					m_flReleaseStartedAt;

	CNetworkVar( bool,		m_bHolstered );
	CNetworkVar( bool,		m_bChargeRelease );
	CNetworkVar( float,		m_flChargeLevel );

	float					m_flNextTargetCheckTime;
	bool					m_bCanChangeTarget; // used to track the PrimaryAttack key being released for autosteal mode

#ifdef CLIENT_DLL
	bool					m_bPlayingSound;
	bool					m_bUpdateStealingTargets;
	struct stealingtargeteffects_t
	{
		C_BaseEntity		*pTarget;
		CNewParticleEffect	*pEffect;
	};
	stealingtargeteffects_t m_hStealingTargetEffect;

	float					m_flFlashCharge;
	bool					m_bOldChargeRelease;

	CNewParticleEffect	*m_pChargeEffect;
	CSoundPatch			*m_pChargedSound;
#endif

private:														
	CWeaponHands( const CWeaponHands & );
};

#endif // FO_WEAPON_HANDS_H
