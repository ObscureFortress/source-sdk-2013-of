// TURNIP CODE 
// 1/20/2023

// based off of tf_logic_arena
// made for extrateam cp maps that play like cp_patchwork

// its called "logic" instead of "gamemode" because it isnt an entire gamemode, and rather just something that adds onto cp

#include "cbase.h"
#include "teamplay_gamerules.h"
#include "teamplayroundbased_gamerules.h"
#include "tf_gamerules.h"

class CFOLogicPatchwork : public CLogicalEntity
{
public:
	DECLARE_CLASS(CFOLogicPatchwork, CLogicalEntity);
	DECLARE_DATADESC();

	// Constructor
	CFOLogicPatchwork() 
	{
		m_bRedAlive = true;
		m_bBlueAlive = true;
		m_bGreenAlive = true;
		m_bYellowAlive = true;
		m_bPurpleAlive = true;
		m_bPinkAlive = true;
	}

	void Think(void);
	void TeamThink(void);
	void Activate(void);

	void SetRedEliminated(inputdata_t &inputData);
	void SetBlueEliminated(inputdata_t &inputData);
	void SetGreenEliminated(inputdata_t &inputData);
	void SetYellowEliminated(inputdata_t &inputData);
	void SetPurpleEliminated(inputdata_t &inputData);
	void SetPinkEliminated(inputdata_t &inputData);

private:

	bool m_bRoundRunning;

	bool m_bRoundOver;

	bool m_bRedAlive;
	bool m_bBlueAlive;
	bool m_bGreenAlive;
	bool m_bYellowAlive;
	bool m_bPurpleAlive;
	bool m_bPinkAlive;

	COutputEvent	m_OnPatchworkRoundStart;
};

LINK_ENTITY_TO_CLASS(fo_logic_patchwork, CFOLogicPatchwork);


BEGIN_DATADESC(CFOLogicPatchwork)

	DEFINE_THINKFUNC(Think),
	DEFINE_THINKFUNC(TeamThink),

	DEFINE_INPUTFUNC(FIELD_VOID, "SetRedEliminated", SetRedEliminated),
	DEFINE_INPUTFUNC(FIELD_VOID, "SetBlueEliminated", SetBlueEliminated),
	DEFINE_INPUTFUNC(FIELD_VOID, "SetGreenEliminated", SetGreenEliminated),
	DEFINE_INPUTFUNC(FIELD_VOID, "SetYellowEliminated", SetYellowEliminated),
	DEFINE_INPUTFUNC(FIELD_VOID, "SetPurpleEliminated", SetPurpleEliminated),
	DEFINE_INPUTFUNC(FIELD_VOID, "SetPinkEliminated", SetPinkEliminated),

	DEFINE_OUTPUT(m_OnPatchworkRoundStart, "OnPatchworkRoundStart"),

END_DATADESC()

void CFOLogicPatchwork::SetRedEliminated(inputdata_t &inputData)
{
	m_bRedAlive = false;
}

void CFOLogicPatchwork::SetBlueEliminated(inputdata_t &inputData)
{
	m_bBlueAlive = false;
}

void CFOLogicPatchwork::SetGreenEliminated(inputdata_t &inputData)
{
	m_bGreenAlive = false;
}

void CFOLogicPatchwork::SetYellowEliminated(inputdata_t &inputData)
{
	m_bYellowAlive = false;
}

void CFOLogicPatchwork::SetPurpleEliminated(inputdata_t &inputData)
{
	m_bPurpleAlive = false;
}

void CFOLogicPatchwork::SetPinkEliminated(inputdata_t &inputData)
{
	m_bPinkAlive = false;
}


//-----------------------------------------------------------------------------
// Purpose: Precache function for the entity
//-----------------------------------------------------------------------------
void CFOLogicPatchwork::Activate(void)
{
	m_bRoundRunning = false;
	m_bRoundOver = false;

	m_bRedAlive = true;
	m_bBlueAlive = true;
	m_bGreenAlive = true;
	m_bYellowAlive = true;
	m_bPurpleAlive = true;
	m_bPinkAlive = true;

	TeamplayRoundBasedRules()->m_TeamRespawnWaveTimes.Set(TF_TEAM_RED, 10);
	TeamplayRoundBasedRules()->m_TeamRespawnWaveTimes.Set(TF_TEAM_BLUE, 10);
	TeamplayRoundBasedRules()->m_TeamRespawnWaveTimes.Set(FO_TEAM_GREEN, 10);
	TeamplayRoundBasedRules()->m_TeamRespawnWaveTimes.Set(FO_TEAM_YELLOW, 10);
	TeamplayRoundBasedRules()->m_TeamRespawnWaveTimes.Set(FO_TEAM_PURPLE, 10);
	TeamplayRoundBasedRules()->m_TeamRespawnWaveTimes.Set(FO_TEAM_PINK, 10);

	SetContextThink(&CFOLogicPatchwork::Think, gpGlobals->curtime + 5, "patchworkthink");
	SetContextThink(&CFOLogicPatchwork::TeamThink, gpGlobals->curtime + 0.1f, "patchworkteamthink");
}

void CFOLogicPatchwork::Think( void )
{
	if (TFGameRules() && TeamplayRoundBasedRules() && TFGameRules()->m_bRoundRunning && !TeamplayRoundBasedRules()->IsInWaitingForPlayers())
	{
		if (!m_bRoundRunning)
		{
			m_bRedAlive = true;
			m_bBlueAlive = true;
			m_bGreenAlive = true;
			m_bYellowAlive = true;
			m_bPurpleAlive = true;
			m_bPinkAlive = true;

			TeamplayRoundBasedRules()->m_TeamRespawnWaveTimes.Set(TF_TEAM_RED, 10);
			TeamplayRoundBasedRules()->m_TeamRespawnWaveTimes.Set(TF_TEAM_BLUE, 10);
			TeamplayRoundBasedRules()->m_TeamRespawnWaveTimes.Set(FO_TEAM_GREEN, 10);
			TeamplayRoundBasedRules()->m_TeamRespawnWaveTimes.Set(FO_TEAM_YELLOW, 10);
			TeamplayRoundBasedRules()->m_TeamRespawnWaveTimes.Set(FO_TEAM_PURPLE, 10);
			TeamplayRoundBasedRules()->m_TeamRespawnWaveTimes.Set(FO_TEAM_PINK, 10);

			m_OnPatchworkRoundStart.FireOutput(this, this);
			m_bRoundRunning = true;
		}
		if (TFGameRules() && TFGameRules()->m_nExtraTeamMode == 1)
		{
			m_bYellowAlive = false;
			m_bPurpleAlive = false;
			m_bPinkAlive = false;
		}
	}
	
	if (TeamplayRoundBasedRules() && m_bRoundRunning)
	{
		if (!m_bRedAlive)
		{
			TeamplayRoundBasedRules()->m_TeamRespawnWaveTimes.Set(TF_TEAM_RED, 999999999999);
		}
		if (!m_bBlueAlive)
		{
			TeamplayRoundBasedRules()->m_TeamRespawnWaveTimes.Set(TF_TEAM_BLUE, 999999999999);
		}
		if (!m_bGreenAlive)
		{
			TeamplayRoundBasedRules()->m_TeamRespawnWaveTimes.Set(FO_TEAM_GREEN, 999999999999);
		}
		if (!m_bYellowAlive)
		{
			TeamplayRoundBasedRules()->m_TeamRespawnWaveTimes.Set(FO_TEAM_YELLOW, 999999999999);
		}
		if (!m_bPurpleAlive)
		{
			TeamplayRoundBasedRules()->m_TeamRespawnWaveTimes.Set(FO_TEAM_PURPLE, 999999999999);
		}
		if (!m_bPinkAlive)
		{
			TeamplayRoundBasedRules()->m_TeamRespawnWaveTimes.Set(FO_TEAM_PINK, 999999999999);
		}
	}
	// eat my farts

	SetNextThink(gpGlobals->curtime + 1.0f, "patchworkthink"); // think every second
}

void CFOLogicPatchwork::TeamThink(void)
{
	CTeamplayRoundBasedRules *pGameRules = dynamic_cast<CTeamplayRoundBasedRules *>(GameRules());

	if (pGameRules)
	{
		if (TeamplayRoundBasedRules())
		{
			if (!(TeamplayRoundBasedRules()->IsInWaitingForPlayers()) && m_bRoundRunning && !m_bRoundOver)
			{
				if ((!m_bBlueAlive) && (!m_bGreenAlive) && (!m_bYellowAlive) && (!m_bPurpleAlive) && (!m_bPinkAlive))
				{
					pGameRules->SetWinningTeam(TF_TEAM_RED, WINREASON_ARENA, true);
					m_bRoundOver = true;
				}
				if ((!m_bRedAlive) && (!m_bGreenAlive) && (!m_bYellowAlive) && (!m_bPurpleAlive) && (!m_bPinkAlive))
				{
					pGameRules->SetWinningTeam(TF_TEAM_BLUE, WINREASON_ARENA, true);
					m_bRoundOver = true;
				}
				if ((!m_bBlueAlive) && (!m_bRedAlive) && (!m_bYellowAlive) && (!m_bPurpleAlive) && (!m_bPinkAlive))
				{
					pGameRules->SetWinningTeam(FO_TEAM_GREEN, WINREASON_ARENA, true);
					m_bRoundOver = true;
				}
				if ((!m_bBlueAlive) && (!m_bGreenAlive) && (!m_bRedAlive) && (!m_bPurpleAlive) && (!m_bPinkAlive))
				{
					pGameRules->SetWinningTeam(FO_TEAM_YELLOW, WINREASON_ARENA, true);
					m_bRoundOver = true;
				}
				if ((!m_bBlueAlive) && (!m_bGreenAlive) && (!m_bYellowAlive) && (!m_bRedAlive) && (!m_bPinkAlive))
				{
					pGameRules->SetWinningTeam(FO_TEAM_PURPLE, WINREASON_ARENA, true);
					m_bRoundOver = true;
				}
				if ((!m_bBlueAlive) && (!m_bGreenAlive) && (!m_bYellowAlive) && (!m_bPurpleAlive) && (!m_bRedAlive))
				{
					pGameRules->SetWinningTeam(FO_TEAM_PINK, WINREASON_ARENA, true);
					m_bRoundOver = true;
				}
			}
		}
	}

	SetNextThink(gpGlobals->curtime + 0.05f, "patchworkteamthink");
}