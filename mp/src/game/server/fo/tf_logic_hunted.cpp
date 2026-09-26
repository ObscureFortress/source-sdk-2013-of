// TURNIP CODE 
// 1/15/2023

// written from scratch
// designed to (sort of (not really)) work with the leaked hunted map

#include "cbase.h"
#include "teamplay_gamerules.h"
#include "teamplayroundbased_gamerules.h"
#include "tf_gamerules.h"

class CTFGamemodeHunted : public CLogicalEntity
{
public:
	DECLARE_CLASS(CTFGamemodeHunted, CLogicalEntity);
	DECLARE_DATADESC();

	// Constructor
	CTFGamemodeHunted() {}

	void Think(void);
	void CourierThink(void);
	void Activate(void);
	bool CheckForCourier(int iTeam, bool bReset);
	void AddCourier(int iTeam);
	void AwardDefendingTeam();
	void TeamWin(int iTeam, bool bPlaying);

	void ResetAllCouriers(inputdata_t &inputData);
	void ResetRedCourier(inputdata_t &inputData);
	void ResetBlueCourier(inputdata_t &inputData);
	void ResetGreenCourier(inputdata_t &inputData);
	void ResetYellowCourier(inputdata_t &inputData);
	void ResetPurpleCourier(inputdata_t &inputData);
	void ResetPinkCourier(inputdata_t &inputData);

	void AddRedPoint(inputdata_t &inputData);
	void AddBluePoint(inputdata_t &inputData);
	void AddGreenPoint(inputdata_t &inputData);
	void AddYellowPoint(inputdata_t &inputData);
	void AddPurplePoint(inputdata_t &inputData);
	void AddPinkPoint(inputdata_t &inputData);

private:

	bool m_bResetRedCourier;
	bool m_bResetBlueCourier;
	bool m_bResetGreenCourier;
	bool m_bResetYellowCourier;
	bool m_bResetPurpleCourier;
	bool m_bResetPinkCourier;

	bool m_bRedPlaying;
	bool m_bBluePlaying;
	bool m_bGreenPlaying;
	bool m_bYellowPlaying;
	bool m_bPurplePlaying;
	bool m_bPinkPlaying;

	int m_nDefendingTeam;

	int m_nScoreLimitRed;
	int m_nScoreLimitBlue;
	int m_nScoreLimitGreen;
	int m_nScoreLimitYellow;
	int m_nScoreLimitPurple;
	int m_nScoreLimitPink;

	int m_nRedScore;
	int m_nBlueScore;
	int m_nGreenScore;
	int m_nYellowScore;
	int m_nPurpleScore;
	int m_nPinkScore;

	bool m_bRoundRunning;

	bool m_bRoundOver;

	COutputEvent	m_OnHuntedRoundStart;

	COutputEvent	m_OnCourierKilled;

	COutputEvent	m_OnRedCourierKilled;
	COutputEvent	m_OnBlueCourierKilled;
	COutputEvent	m_OnGreenCourierKilled;
	COutputEvent	m_OnYellowCourierKilled;
	COutputEvent	m_OnPurpleCourierKilled;
	COutputEvent	m_OnPinkCourierKilled;

	COutputEvent	m_OnRedWin;
	COutputEvent	m_OnBlueWin;
	COutputEvent	m_OnGreenWin;
	COutputEvent	m_OnYellowWin;
	COutputEvent	m_OnPurpleWin;
	COutputEvent	m_OnPinkWin;
};

LINK_ENTITY_TO_CLASS(tf_logic_hunted, CTFGamemodeHunted);


BEGIN_DATADESC(CTFGamemodeHunted)

	DEFINE_KEYFIELD(m_bRedPlaying, FIELD_BOOLEAN, "RedPlaying"),
	DEFINE_KEYFIELD(m_bBluePlaying, FIELD_BOOLEAN, "BluePlaying"),
	DEFINE_KEYFIELD(m_bGreenPlaying, FIELD_BOOLEAN, "GreenPlaying"),
	DEFINE_KEYFIELD(m_bYellowPlaying, FIELD_BOOLEAN, "YellowPlaying"),
	DEFINE_KEYFIELD(m_bPurplePlaying, FIELD_BOOLEAN, "PurplePlaying"),
	DEFINE_KEYFIELD(m_bPinkPlaying, FIELD_BOOLEAN, "PinkPlaying"),

	DEFINE_KEYFIELD(m_nScoreLimitRed, FIELD_INTEGER, "ScoreLimitRed"),
	DEFINE_KEYFIELD(m_nScoreLimitBlue, FIELD_INTEGER, "ScoreLimitBlue"),
	DEFINE_KEYFIELD(m_nScoreLimitGreen, FIELD_INTEGER, "ScoreLimitGreen"),
	DEFINE_KEYFIELD(m_nScoreLimitYellow, FIELD_INTEGER, "ScoreLimitYellow"),
	DEFINE_KEYFIELD(m_nScoreLimitPurple, FIELD_INTEGER, "ScoreLimitPurple"),
	DEFINE_KEYFIELD(m_nScoreLimitPink, FIELD_INTEGER, "ScoreLimitPink"),

	DEFINE_KEYFIELD(m_nDefendingTeam, FIELD_INTEGER, "DefendingTeam"),

	DEFINE_INPUTFUNC(FIELD_INTEGER, "AddRedPoint", AddRedPoint),
	DEFINE_INPUTFUNC(FIELD_INTEGER, "AddBluePoint", AddBluePoint),
	DEFINE_INPUTFUNC(FIELD_INTEGER, "AddGreenPoint", AddGreenPoint),
	DEFINE_INPUTFUNC(FIELD_INTEGER, "AddYellowPoint", AddYellowPoint),
	DEFINE_INPUTFUNC(FIELD_INTEGER, "AddPurplePoint", AddPurplePoint),
	DEFINE_INPUTFUNC(FIELD_INTEGER, "AddPinkPoint", AddPinkPoint),

	DEFINE_INPUTFUNC(FIELD_VOID, "ResetAllCouriers", ResetAllCouriers),

	DEFINE_INPUTFUNC(FIELD_VOID, "ResetRedCourier", ResetRedCourier),
	DEFINE_INPUTFUNC(FIELD_VOID, "ResetBlueCourier", ResetBlueCourier),
	DEFINE_INPUTFUNC(FIELD_VOID, "ResetGreenCourier", ResetGreenCourier),
	DEFINE_INPUTFUNC(FIELD_VOID, "ResetYellowCourier", ResetYellowCourier),
	DEFINE_INPUTFUNC(FIELD_VOID, "ResetPurpleCourier", ResetPurpleCourier),
	DEFINE_INPUTFUNC(FIELD_VOID, "ResetPinkCourier", ResetPinkCourier),

	DEFINE_THINKFUNC(Think),

	DEFINE_OUTPUT(m_OnHuntedRoundStart, "OnHuntedRoundStart"),

	DEFINE_OUTPUT(m_OnCourierKilled, "OnCourierKilled"),

	DEFINE_OUTPUT(m_OnRedCourierKilled, "OnRedCourierKilled"),
	DEFINE_OUTPUT(m_OnBlueCourierKilled, "OnBlueCourierKilled"),
	DEFINE_OUTPUT(m_OnGreenCourierKilled, "OnGreenCourierKilled"),
	DEFINE_OUTPUT(m_OnYellowCourierKilled, "OnYellowCourierKilled"),
	DEFINE_OUTPUT(m_OnPurpleCourierKilled, "OnPurpleCourierKilled"),

	DEFINE_OUTPUT(m_OnRedWin, "OnRedWin"),
	DEFINE_OUTPUT(m_OnBlueWin, "OnBlueWin"),
	DEFINE_OUTPUT(m_OnGreenWin, "OnGreenWin"),
	DEFINE_OUTPUT(m_OnPurpleWin, "OnPurpleWin"),
	DEFINE_OUTPUT(m_OnPinkWin, "OnPinkWin"),

END_DATADESC()

void CTFGamemodeHunted::ResetRedCourier(inputdata_t &inputData)
{
	m_bResetRedCourier = true;
}

void CTFGamemodeHunted::ResetBlueCourier(inputdata_t &inputData)
{
	m_bResetBlueCourier = true;
}

void CTFGamemodeHunted::ResetGreenCourier(inputdata_t &inputData)
{
	m_bResetGreenCourier = true;
}

void CTFGamemodeHunted::ResetYellowCourier(inputdata_t &inputData)
{
	m_bResetYellowCourier = true;
}

void CTFGamemodeHunted::ResetPurpleCourier(inputdata_t &inputData)
{
	m_bResetPurpleCourier = true;
}

void CTFGamemodeHunted::ResetPinkCourier(inputdata_t &inputData)
{
	m_bResetPinkCourier = true;
}

void CTFGamemodeHunted::ResetAllCouriers(inputdata_t &inputData)
{
	m_bResetRedCourier = true;
	m_bResetBlueCourier = true;
	m_bResetGreenCourier = true;
	m_bResetYellowCourier = true;
	m_bResetPurpleCourier = true;
	m_bResetPinkCourier = true;
}

void CTFGamemodeHunted::AddRedPoint(inputdata_t &inputData)
{
	m_nRedScore += inputData.value.Int();
}

void CTFGamemodeHunted::AddBluePoint(inputdata_t &inputData)
{
	m_nBlueScore += inputData.value.Int();
}

void CTFGamemodeHunted::AddGreenPoint(inputdata_t &inputData)
{
	m_nGreenScore += inputData.value.Int();
}

void CTFGamemodeHunted::AddYellowPoint(inputdata_t &inputData)
{
	m_nYellowScore += inputData.value.Int();
}

void CTFGamemodeHunted::AddPurplePoint(inputdata_t &inputData)
{
	m_nPurpleScore += inputData.value.Int();
}

void CTFGamemodeHunted::AddPinkPoint(inputdata_t &inputData)
{
	m_nPinkScore += inputData.value.Int();
}

void CTFGamemodeHunted::AwardDefendingTeam(void)
{
	switch (m_nDefendingTeam)
	{
	case 1:
		m_nRedScore += 1;
		break;
	case 2:
		m_nBlueScore += 1;
		break;
	case 3:
		m_nGreenScore += 1;
		break;
	case 4:
		m_nYellowScore += 1;
		break;
	case 5:
		m_nPurpleScore += 1;
		break;
	case 6:
		m_nPinkScore += 1;
		break;
	default:
		break;
	}
}

//-----------------------------------------------------------------------------
// Purpose: Precache function for the entity
//-----------------------------------------------------------------------------
void CTFGamemodeHunted::Activate(void)
{
	m_bRoundRunning = false;
	m_bRoundOver = false;

	m_nRedScore = 0;
	m_nBlueScore = 0;
	m_nGreenScore = 0;
	m_nYellowScore = 0;
	m_nPurpleScore = 0;
	m_nPinkScore = 0;

	//Msg("activate");


	SetContextThink(&CTFGamemodeHunted::Think, gpGlobals->curtime + 3.0f, "huntedthink");
	SetContextThink(&CTFGamemodeHunted::CourierThink, gpGlobals->curtime + 3.0f, "courierthink");
}

void CTFGamemodeHunted::Think( void )
{
	if (TFGameRules() && TeamplayRoundBasedRules() && TFGameRules()->m_bRoundRunning && !TeamplayRoundBasedRules()->IsInWaitingForPlayers())
	{
		if (!m_bRoundRunning)
		{
			 // find any existing couriers and delete them so that a new person gets to be the courier
			CheckForCourier(TF_TEAM_RED, true);
			CheckForCourier(TF_TEAM_BLUE, true);
			CheckForCourier(FO_TEAM_GREEN, true);
			CheckForCourier(FO_TEAM_YELLOW, true);
			CheckForCourier(FO_TEAM_PURPLE, true);
			CheckForCourier(FO_TEAM_PINK, true);

			m_OnHuntedRoundStart.FireOutput(this, this);
			m_bRoundRunning = true;
		}

		if (m_bRoundRunning)
		{
			if (m_bRedPlaying && !CheckForCourier(TF_TEAM_RED, false))
			{
				AddCourier(TF_TEAM_RED);
			}
			if (m_bBluePlaying && !CheckForCourier(TF_TEAM_BLUE, false))
			{
				AddCourier(TF_TEAM_BLUE);
			}
			if (m_bGreenPlaying && !CheckForCourier(FO_TEAM_GREEN, false))
			{
				AddCourier(FO_TEAM_GREEN);
			}
			if (m_bYellowPlaying && !CheckForCourier(FO_TEAM_YELLOW, false))
			{
				AddCourier(FO_TEAM_YELLOW);
			}
			if (m_bPurplePlaying && !CheckForCourier(FO_TEAM_PURPLE, false))
			{
				AddCourier(FO_TEAM_PURPLE);
			}
			if (m_bPinkPlaying && !CheckForCourier(FO_TEAM_PINK, false))
			{
				AddCourier(FO_TEAM_PINK);
			}

			if (m_nRedScore >= m_nScoreLimitRed)
			{
				TeamWin(TF_TEAM_RED, m_bRedPlaying);
				m_OnRedWin.FireOutput(this, this);
			}
			if (m_nBlueScore >= m_nScoreLimitBlue)
			{
				TeamWin(TF_TEAM_BLUE, m_bBluePlaying);
				m_OnBlueWin.FireOutput(this, this);
			}
			if (m_nGreenScore >= m_nScoreLimitGreen)
			{
				TeamWin(FO_TEAM_GREEN, m_bGreenPlaying);
				m_OnGreenWin.FireOutput(this, this);
			}
			if (m_nYellowScore >= m_nScoreLimitYellow)
			{
				TeamWin(FO_TEAM_YELLOW, m_bYellowPlaying);
				m_OnYellowWin.FireOutput(this, this);
			}
			if (m_nPurpleScore >= m_nScoreLimitPurple)
			{
				TeamWin(FO_TEAM_PURPLE, m_bPurplePlaying);
				m_OnPurpleWin.FireOutput(this, this);
			}
		}
	}

	SetNextThink(gpGlobals->curtime + 1.0f, "huntedthink"); // think every second
}

void CTFGamemodeHunted::CourierThink(void)
{
	int i;
	CTFPlayer *pPlayer;
	CTFPlayer *redCourier;
	CTFPlayer *blueCourier;
	CTFPlayer *greenCourier;
	CTFPlayer *yellowCourier;
	CTFPlayer *purpleCourier;
	CTFPlayer *pinkCourier;

	redCourier = nullptr; // just so the compiler doesnt yell at me
	blueCourier = nullptr;
	greenCourier = nullptr;
	yellowCourier = nullptr;
	purpleCourier = nullptr;
	pinkCourier = nullptr;

	if (m_bRedPlaying)
	{
		for (i = 1; i <= gpGlobals->maxClients; i++)
		{
			pPlayer = ToTFPlayer(UTIL_PlayerByIndex(i));

			if (pPlayer)
			{
				if (pPlayer->GetTeamNumber() == TF_TEAM_RED)
				{
					if (pPlayer->GetPlayerClass()->GetClassIndex() == FO_CLASS_COURIER + 1)
					{
						redCourier = pPlayer;
						break;
					}
				}
			}
		}

		if (redCourier)
		{
			if (!redCourier->IsAlive())
			{
				m_OnCourierKilled.FireOutput(this, this);
				m_OnRedCourierKilled.FireOutput(this, this);
				AwardDefendingTeam();
				redCourier->ForceRespawn();
			}

			if (m_bResetRedCourier)
			{
				redCourier->ForceRespawn();
				m_bResetRedCourier = false;
			}
		}
	}
	if (m_bBluePlaying)
	{
		for (i = 1; i <= gpGlobals->maxClients; i++)
		{
			pPlayer = ToTFPlayer(UTIL_PlayerByIndex(i));

			if (pPlayer)
			{
				if (pPlayer->GetTeamNumber() == TF_TEAM_BLUE)
				{
					if (pPlayer->GetPlayerClass()->GetClassIndex() == FO_CLASS_COURIER + 1)
					{
						blueCourier = pPlayer;
						break;
					}
				}
			}
		}

		if (blueCourier)
		{
			if (!blueCourier->IsAlive())
			{
				m_OnCourierKilled.FireOutput(this, this);
				m_OnBlueCourierKilled.FireOutput(this, this);
				AwardDefendingTeam();
				blueCourier->ForceRespawn();
			}

			if (m_bResetBlueCourier)
			{
				blueCourier->ForceRespawn();
				m_bResetBlueCourier = false;
			}
		}
	}
	if (m_bGreenPlaying)
	{
		for (i = 1; i <= gpGlobals->maxClients; i++)
		{
			pPlayer = ToTFPlayer(UTIL_PlayerByIndex(i));

			if (pPlayer)
			{
				if (pPlayer->GetTeamNumber() == FO_TEAM_GREEN)
				{
					if (pPlayer->GetPlayerClass()->GetClassIndex() == FO_CLASS_COURIER + 1)
					{
						greenCourier = pPlayer;
						break;
					}
				}
			}
		}

		if (greenCourier)
		{
			if (!greenCourier->IsAlive())
			{
				m_OnCourierKilled.FireOutput(this, this);
				m_OnGreenCourierKilled.FireOutput(this, this);
				AwardDefendingTeam();
				greenCourier->ForceRespawn();
			}

			if (m_bResetGreenCourier)
			{
				greenCourier->ForceRespawn();
				m_bResetGreenCourier = false;
			}
		}
	}
	if (m_bYellowPlaying)
	{
		for (i = 1; i <= gpGlobals->maxClients; i++)
		{
			pPlayer = ToTFPlayer(UTIL_PlayerByIndex(i));

			if (pPlayer)
			{
				if (pPlayer->GetTeamNumber() == FO_TEAM_YELLOW)
				{
					if (pPlayer->GetPlayerClass()->GetClassIndex() == FO_CLASS_COURIER + 1)
					{
						yellowCourier = pPlayer;
						break;
					}
				}
			}
		}

		if (yellowCourier)
		{
			if (!yellowCourier->IsAlive())
			{
				m_OnCourierKilled.FireOutput(this, this);
				m_OnYellowCourierKilled.FireOutput(this, this);
				AwardDefendingTeam();
				yellowCourier->ForceRespawn();
			}

			if (m_bResetYellowCourier)
			{
				yellowCourier->ForceRespawn();
				m_bResetYellowCourier = false;
			}
		}
	}
	if (m_bPurplePlaying)
	{
		for (i = 1; i <= gpGlobals->maxClients; i++)
		{
			pPlayer = ToTFPlayer(UTIL_PlayerByIndex(i));

			if (pPlayer)
			{
				if (pPlayer->GetTeamNumber() == FO_TEAM_PURPLE)
				{
					if (pPlayer->GetPlayerClass()->GetClassIndex() == FO_CLASS_COURIER + 1)
					{
						purpleCourier = pPlayer;
						break;
					}
				}
			}
		}

		if (purpleCourier)
		{
			if (!purpleCourier->IsAlive())
			{
				m_OnCourierKilled.FireOutput(this, this);
				m_OnPurpleCourierKilled.FireOutput(this, this);
				AwardDefendingTeam();
				purpleCourier->ForceRespawn();
			}

			if (m_bResetPurpleCourier)
			{
				purpleCourier->ForceRespawn();
				m_bResetPurpleCourier = false;
			}
		}
	}
	if (m_bPinkPlaying)
	{
		for (i = 1; i <= gpGlobals->maxClients; i++)
		{
			pPlayer = ToTFPlayer(UTIL_PlayerByIndex(i));

			if (pPlayer)
			{
				if (pPlayer->GetTeamNumber() == FO_TEAM_PINK)
				{
					if (pPlayer->GetPlayerClass()->GetClassIndex() == FO_CLASS_COURIER + 1)
					{
						pinkCourier = pPlayer;
						break;
					}
				}
			}
		}

		if (pinkCourier)
		{
			if (!pinkCourier->IsAlive())
			{
				m_OnCourierKilled.FireOutput(this, this);
				m_OnPinkCourierKilled.FireOutput(this, this);
				AwardDefendingTeam();
				pinkCourier->ForceRespawn();
			}

			if (m_bResetPinkCourier)
			{
				pinkCourier->ForceRespawn();
				m_bResetPinkCourier = false;
			}
		}
	}

	SetNextThink(gpGlobals->curtime + 0.5f, "courierthink"); // think every half second
}

void CTFGamemodeHunted::TeamWin(int iTeam, bool bPlaying)
{
	CTeamplayRoundBasedRules *pGameRules = dynamic_cast<CTeamplayRoundBasedRules *>(GameRules());

	if (pGameRules)
	{
		if (TeamplayRoundBasedRules())
		{
			if (!m_bRoundOver)
			{
				if (bPlaying)
				{
					pGameRules->SetWinningTeam(iTeam, WINREASON_HUNTED_ATTACK, true);
				}
				else
				{
					pGameRules->SetWinningTeam(iTeam, WINREASON_HUNTED_DEFENSE, true);
				}
				m_bRoundOver = true;
			}
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool CTFGamemodeHunted::CheckForCourier(int iTeam, bool bReset)
{
	int i;
	bool found = false;
	CTFPlayer *pPlayer;

	for (i = 1; i <= gpGlobals->maxClients; i++)
	{
		pPlayer = ToTFPlayer(UTIL_PlayerByIndex(i));

		if (pPlayer)
		{
			if (pPlayer->GetTeamNumber() == iTeam)
			{
				if (pPlayer->GetPlayerClass()->GetClassIndex() == FO_CLASS_COURIER + 1)
				{
					if (!bReset)
					{
						found = true;
					}
					else
					{
						pPlayer->SetDesiredPlayerClassIndex(FO_CLASS_SENTRONIC + 1);
						pPlayer->ForceRespawn();
					}
					break;
				}
			}
		}
	}

	return found;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFGamemodeHunted::AddCourier(int iTeam)
{
	CTFPlayer *pPlayer;
	int playerNum;
	int chosenPlayer;
	bool found = false;

	while (!found) // dont wait for the next think check DO IT NOW
	{
		playerNum = gpGlobals->maxClients;
		chosenPlayer = random->RandomInt(1, playerNum);

		pPlayer = ToTFPlayer(UTIL_PlayerByIndex(chosenPlayer)); // his purpose

		if (pPlayer && pPlayer->IsReadyToPlay() && pPlayer->IsAlive() && pPlayer->GetTeamNumber() == iTeam && pPlayer->GetPlayerClass()->GetClassIndex() != TF_CLASS_UNDEFINED)
		{
			pPlayer->SetDesiredPlayerClassIndex(FO_CLASS_COURIER + 1);
			pPlayer->ForceRespawn();
			found = true;
		}

		if (TeamplayRoundBasedRules()->CountActivePlayersOnTeam(iTeam) == 0) // dont freeze 
			break;
	}
}