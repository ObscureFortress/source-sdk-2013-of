// TURNIP CODE 
// 10/19/2022

// written from scratch
// designed to work with already existing arena maps

#include "cbase.h"
#include "teamplay_gamerules.h"
#include "teamplayroundbased_gamerules.h"
#include "tf_gamerules.h"

class CTFGamemodeArena : public CLogicalEntity
{
public:
	DECLARE_CLASS(CTFGamemodeArena, CLogicalEntity);
	DECLARE_DATADESC();

	// Constructor
	CTFGamemodeArena() {}

	void Think(void);
	void TeamThink(void);
	void Activate(void);

private:

	float m_nCapEnableDelay;

	bool m_bCapEnabled;
	bool m_bRoundRunning;

	bool m_bRoundOver;

	int m_nRedPlayers;
	int m_nBluePlayers;
	int m_nGreenPlayers;
	int m_nYellowPlayers;
	int m_nPurplePlayers;
	int m_nPinkPlayers;

	COutputEvent	m_OnArenaRoundStart;

	COutputEvent	m_OnCapEnabled;
};

LINK_ENTITY_TO_CLASS(tf_logic_arena, CTFGamemodeArena);


BEGIN_DATADESC(CTFGamemodeArena)

	DEFINE_KEYFIELD(m_nCapEnableDelay, FIELD_FLOAT, "CapEnableDelay"),

	DEFINE_THINKFUNC(Think),
	DEFINE_THINKFUNC(TeamThink),

	DEFINE_OUTPUT(m_OnArenaRoundStart, "OnArenaRoundStart"),

	DEFINE_OUTPUT(m_OnCapEnabled, "OnCapEnabled"),

END_DATADESC()

//-----------------------------------------------------------------------------
// Purpose: Precache function for the entity
//-----------------------------------------------------------------------------
void CTFGamemodeArena::Activate(void)
{
	Floor2Int(m_nCapEnableDelay); // round this because i dont know how to bother with floats
	m_bCapEnabled = false;
	m_bRoundRunning = false;
	m_bRoundOver = false;

	SetContextThink(&CTFGamemodeArena::Think, gpGlobals->curtime + 5, "arenathink");
	SetContextThink(&CTFGamemodeArena::TeamThink, gpGlobals->curtime + 0.1f, "arenateamthink");
}

void CTFGamemodeArena::Think( void )
{
	if (TFGameRules() && TeamplayRoundBasedRules() && TFGameRules()->m_bRoundRunning && !TeamplayRoundBasedRules()->IsInWaitingForPlayers())
	{
		if (!m_bRoundRunning)
		{
			m_OnArenaRoundStart.FireOutput(this, this);
			m_bRoundRunning = true;
		}

		if (m_nCapEnableDelay >= 0)
		{
			m_nCapEnableDelay -= 1;
			TeamplayRoundBasedRules()->m_bArenaLock = true;
		}
		else
		{
			TeamplayRoundBasedRules()->m_bArenaLock = false;
			if (!m_bCapEnabled)
			{
				m_OnCapEnabled.FireOutput(this, this);
				m_bCapEnabled = true;
			}
		}
	}

	
	if (TeamplayRoundBasedRules())
	{
		TeamplayRoundBasedRules()->m_TeamRespawnWaveTimes.Set(TF_TEAM_RED, 999999999999);
		TeamplayRoundBasedRules()->m_TeamRespawnWaveTimes.Set(TF_TEAM_BLUE, 999999999999);
		TeamplayRoundBasedRules()->m_TeamRespawnWaveTimes.Set(FO_TEAM_GREEN, 999999999999);
		TeamplayRoundBasedRules()->m_TeamRespawnWaveTimes.Set(FO_TEAM_YELLOW, 999999999999);
		TeamplayRoundBasedRules()->m_TeamRespawnWaveTimes.Set(FO_TEAM_PURPLE, 999999999999);
		TeamplayRoundBasedRules()->m_TeamRespawnWaveTimes.Set(FO_TEAM_PINK, 999999999999);
	}
	// eat my farts

	SetNextThink(gpGlobals->curtime + 1.0f, "arenathink"); // think every second
}

void CTFGamemodeArena::TeamThink(void)
{
	CTeamplayRoundBasedRules *pGameRules = dynamic_cast<CTeamplayRoundBasedRules *>(GameRules());

	if (pGameRules)
	{
		if (TeamplayRoundBasedRules())
		{
			m_nRedPlayers = TeamplayRoundBasedRules()->CountLivingPlayers(TF_TEAM_RED);
			m_nBluePlayers = TeamplayRoundBasedRules()->CountLivingPlayers(TF_TEAM_BLUE);
			m_nGreenPlayers = TeamplayRoundBasedRules()->CountLivingPlayers(FO_TEAM_GREEN);
			m_nYellowPlayers = TeamplayRoundBasedRules()->CountLivingPlayers(FO_TEAM_YELLOW);
			m_nPurplePlayers = TeamplayRoundBasedRules()->CountLivingPlayers(FO_TEAM_PURPLE);
			m_nPinkPlayers = TeamplayRoundBasedRules()->CountLivingPlayers(FO_TEAM_PINK);

			if (!(TeamplayRoundBasedRules()->IsInWaitingForPlayers()) && m_bRoundRunning && !m_bRoundOver)
			{
				if (m_nRedPlayers > 0 && m_nBluePlayers == 0 && m_nGreenPlayers == 0 && m_nYellowPlayers == 0 && m_nPurplePlayers == 0 && m_nPinkPlayers == 0)
				{
					pGameRules->SetWinningTeam(TF_TEAM_RED, WINREASON_ARENA, true);
					m_bRoundOver = true;
				}
				if (m_nBluePlayers > 0 && m_nRedPlayers == 0 && m_nGreenPlayers == 0 && m_nYellowPlayers == 0 && m_nPurplePlayers == 0 && m_nPinkPlayers == 0)
				{
					pGameRules->SetWinningTeam(TF_TEAM_BLUE, WINREASON_ARENA, true);
					m_bRoundOver = true;
				}
				if (m_nGreenPlayers > 0 && m_nRedPlayers == 0 && m_nBluePlayers == 0 && m_nYellowPlayers == 0 && m_nPurplePlayers == 0 && m_nPinkPlayers == 0)
				{
					pGameRules->SetWinningTeam(FO_TEAM_GREEN, WINREASON_ARENA, true);
					m_bRoundOver = true;
				}
				if (m_nYellowPlayers > 0 && m_nRedPlayers == 0 && m_nBluePlayers == 0 && m_nGreenPlayers == 0 && m_nPurplePlayers == 0 && m_nPinkPlayers == 0)
				{
					pGameRules->SetWinningTeam(FO_TEAM_YELLOW, WINREASON_ARENA, true);
					m_bRoundOver = true;
				}
				if (m_nPurplePlayers > 0 && m_nRedPlayers == 0 && m_nBluePlayers == 0 && m_nGreenPlayers == 0 && m_nYellowPlayers == 0 && m_nPinkPlayers == 0)
				{
					pGameRules->SetWinningTeam(FO_TEAM_PURPLE, WINREASON_ARENA, true);
					m_bRoundOver = true;
				}
				if (m_nPinkPlayers > 0 && m_nRedPlayers == 0 && m_nBluePlayers == 0 && m_nGreenPlayers == 0 && m_nYellowPlayers == 0 && m_nPurplePlayers == 0)
				{
					pGameRules->SetWinningTeam(FO_TEAM_PINK, WINREASON_ARENA, true);
					m_bRoundOver = true;
				}
			}
		}
	}

	SetNextThink(gpGlobals->curtime + 0.05f, "arenateamthink");
}