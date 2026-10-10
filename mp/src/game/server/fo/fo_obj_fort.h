//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: Worker Node's Fort
//
// $NoKeywords: $
//=============================================================================//

#ifndef FO_OBJ_FORT_H
#define FO_OBJ_FORT_H
#ifdef _WIN32
#pragma once
#endif

#include "tf_obj.h"

class CTFPlayer;

// ------------------------------------------------------------------------ //
// Worker Node's Fort
// ------------------------------------------------------------------------ //
class CObjectWorkerFort : public CBaseObject
{
	DECLARE_CLASS( CObjectWorkerFort, CBaseObject );
	DECLARE_DATADESC();

public:
	DECLARE_SERVERCLASS();

	CObjectWorkerFort();

	static CObjectWorkerFort* Create( const Vector &vOrigin, const QAngle &vAngles );

	virtual void	Spawn();
	virtual void	Precache();
	virtual void	OnGoActive( void );
	virtual int		DrawDebugTextOverlays( void );
	virtual int		OnTakeDamage( const CTakeDamageInfo &info );
	virtual void	Killed( const CTakeDamageInfo &info );
	virtual void	SetModel( const char *pModel );
	virtual bool	StartBuilding( CBaseEntity *pBuilder );
	virtual void	StartPlacement( CTFPlayer *pPlayer );
	virtual bool	OnWrenchHit( CTFPlayer *pPlayer );
	virtual bool	IsUpgrading( void ) const;

	void			UpgradeThink( void );
	int				GetUpgradeLevel( void ) { return m_iUpgradeLevel; }

private:
	void			FortThink( void );
	bool			CanBeUpgraded( CTFPlayer *pPlayer );
	void			StartUpgrading( void );
	void			FinishUpgrading( void );

	CNetworkVar( int, m_iState );
	CNetworkVar( int, m_iUpgradeLevel );
	float m_flUpgradeCompleteTime;
	CNetworkVar( int, m_iUpgradeMetal );
	CNetworkVar( int, m_iUpgradeMetalRequired );
	float m_flLastAttackedTime;
	float m_flHeavyBulletResist;
	int m_iPlacementBodygroup;
};

#endif // FO_OBJ_FORT_H
