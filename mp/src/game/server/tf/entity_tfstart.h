//====== Copyright © 1996-2005, Valve Corporation, All rights reserved. =======//
//
// Purpose: CTF Spawn Point.
//
//=============================================================================//
#ifndef ENTITY_TFSTART_H
#define ENTITY_TFSTART_H

#ifdef _WIN32
#pragma once
#endif

class CTeamControlPoint;
class CTeamControlPointRound;

//=============================================================================
//
// TF team spawning entity.
//

class CTFTeamSpawn : public CPointEntity
{
public:
	DECLARE_CLASS( CTFTeamSpawn, CPointEntity );

	CTFTeamSpawn();

	void Activate( void );

	bool IsDisabled( void ) { return m_bDisabled; }
	void SetDisabled( bool bDisabled ) { m_bDisabled = bDisabled; }

	// Inputs/Outputs.
	void InputEnable( inputdata_t &inputdata );
	void InputDisable( inputdata_t &inputdata );
	void InputRoundSpawn( inputdata_t &inputdata );

	int DrawDebugTextOverlays(void);

	CHandle<CTeamControlPoint> GetControlPoint( void ) { return m_hControlPoint; }
	CHandle<CTeamControlPointRound> GetRoundBlueSpawn( void ) { return m_hRoundBlueSpawn; }
	CHandle<CTeamControlPointRound> GetRoundRedSpawn( void ) { return m_hRoundRedSpawn; }
	CHandle<CTeamControlPointRound> GetRoundGreenSpawn(void) { return m_hRoundGreenSpawn; }
	CHandle<CTeamControlPointRound> GetRoundYellowSpawn(void) { return m_hRoundYellowSpawn; }
	CHandle<CTeamControlPointRound> GetRoundPurpleSpawn(void) { return m_hRoundPurpleSpawn; }
	CHandle<CTeamControlPointRound> GetRoundPinkSpawn(void) { return m_hRoundPinkSpawn; }

private:
	bool	m_bDisabled;		// Enabled/Disabled?

	string_t						m_iszControlPointName;
	string_t						m_iszRoundBlueSpawn;
	string_t						m_iszRoundRedSpawn;
	string_t						m_iszRoundGreenSpawn;
	string_t						m_iszRoundYellowSpawn;
	string_t						m_iszRoundPurpleSpawn;
	string_t						m_iszRoundPinkSpawn;

	CHandle<CTeamControlPoint>		m_hControlPoint;
	CHandle<CTeamControlPointRound>	m_hRoundBlueSpawn;
	CHandle<CTeamControlPointRound>	m_hRoundRedSpawn;
	CHandle<CTeamControlPointRound>	m_hRoundGreenSpawn;
	CHandle<CTeamControlPointRound>	m_hRoundYellowSpawn;
	CHandle<CTeamControlPointRound>	m_hRoundPurpleSpawn;
	CHandle<CTeamControlPointRound>	m_hRoundPinkSpawn;

	DECLARE_DATADESC();
};

#endif // ENTITY_TFSTART_H


