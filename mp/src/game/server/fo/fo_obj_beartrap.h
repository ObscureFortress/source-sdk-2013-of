//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: Saptrap's Beartrap
//
// $NoKeywords: $
//=============================================================================//

#ifndef FO_OBJ_BEARTRAP_H
#define FO_OBJ_BEARTRAP_H
#ifdef _WIN32
#pragma once
#endif

#include "tf_obj.h"

class CTFPlayer;

// ------------------------------------------------------------------------ //
// Trap that's built by the Saptrap. Detonates on the first enemy to touch it.
// ------------------------------------------------------------------------ //
class CObjectBeartrap : public CBaseObject
{
	DECLARE_CLASS( CObjectBeartrap, CBaseObject );
	DECLARE_DATADESC();

public:
	DECLARE_SERVERCLASS();

	CObjectBeartrap();

	virtual void	Spawn();
	virtual void	Precache();
	virtual bool	StartBuilding( CBaseEntity *pBuilder );
	virtual void	OnGoActive( void );
	virtual int		DrawDebugTextOverlays( void );
	virtual bool	IsPlacementPosValid( void );
	virtual void	SetModel( const char *pModel );
	virtual int		OnTakeDamage( const CTakeDamageInfo &info );
	virtual void	FinishedBuilding( void );

	void			SetState( int state );
	void			BeartrapTouch( CBaseEntity *pOther );

	int				GetState( void ) { return m_iState; }

protected:
	CNetworkVar( int, m_iState );

	float			m_flLastStateChangeTime;
	float			m_flMyNextThink;
	float			m_flNextEnemyTouchHint;
};

#endif // FO_OBJ_BEARTRAP_H
