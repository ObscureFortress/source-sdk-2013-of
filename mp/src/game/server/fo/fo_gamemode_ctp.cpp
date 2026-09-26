// TURNIP CODE 
// 8/31/2022

#include "cbase.h"
#include "tf/tf_shareddefs.h"
#include "entity_forcerespawn.h"
#include "tf_player.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

class CFOGamemodeCaptureThePoint : public CLogicalEntity
{
public:
	DECLARE_CLASS(CFOGamemodeCaptureThePoint, CLogicalEntity);
	DECLARE_DATADESC();

	// Constructor
	CFOGamemodeCaptureThePoint()
	{
		m_nRedCaptures = 0;
		m_nBlueCaptures = 0;
		m_nGreenCaptures = 0;
		m_nYellowCaptures = 0;
		m_nPurpleCaptures = 0;
		m_nPinkCaptures = 0;

		m_nRedPointTimer = 21;
		m_nBluePointTimer = 21;
		m_nGreenPointTimer = 21;
		m_nYellowPointTimer = 21;
		m_nPurplePointTimer = 21;
		m_nPinkPointTimer = 21;
	}

	// Input function
	void AddRedCapture(inputdata_t &inputData);
	void AddBlueCapture(inputdata_t &inputData);
	void AddGreenCapture(inputdata_t &inputData);
	void AddYellowCapture(inputdata_t &inputData);
	void AddPurpleCapture(inputdata_t &inputData);
	void AddPinkCapture(inputdata_t &inputData);

	void ActivateRedBounceBack(inputdata_t &inputData);
	void ActivateBlueBounceBack(inputdata_t &inputData);
	void ActivateGreenBounceBack(inputdata_t &inputData);
	void ActivateYellowBounceBack(inputdata_t &inputData);
	void ActivatePurpleBounceBack(inputdata_t &inputData);
	void ActivatePinkBounceBack(inputdata_t &inputData);

	void DisableRedBounceBack(inputdata_t &inputData);
	void DisableBlueBounceBack(inputdata_t &inputData);
	void DisableGreenBounceBack(inputdata_t &inputData);
	void DisableYellowBounceBack(inputdata_t &inputData);
	void DisablePurpleBounceBack(inputdata_t &inputData);
	void DisablePinkBounceBack(inputdata_t &inputData);

	void UpdateRedPointTimer(inputdata_t &inputData);
	void UpdateBluePointTimer(inputdata_t &inputData);
	void UpdateGreenPointTimer(inputdata_t &inputData);
	void UpdateYellowPointTimer(inputdata_t &inputData);
	void UpdatePurplePointTimer(inputdata_t &inputData);
	void UpdatePinkPointTimer(inputdata_t &inputData);

	void CheckPoints(inputdata_t &inputData);

	void BounceBackThink(inputdata_t &inputData);

	int	m_nPointLimit;
	int	m_nRedCaptures;
	int	m_nBlueCaptures;
	int	m_nGreenCaptures;
	int	m_nYellowCaptures;
	int	m_nPurpleCaptures;
	int	m_nPinkCaptures;

	int	m_nRedPointTimer;
	int	m_nBluePointTimer;
	int	m_nGreenPointTimer;
	int	m_nYellowPointTimer;
	int	m_nPurplePointTimer;
	int	m_nPinkPointTimer;

	bool m_bRedBounceBack;
	bool m_bBlueBounceBack;
	bool m_bGreenBounceBack;
	bool m_bYellowBounceBack;
	bool m_bPurpleBounceBack;
	bool m_bPinkBounceBack;

private:

	COutputEvent	m_OnLimit;

	COutputEvent	m_OnLimitRed;
	COutputEvent	m_OnLimitBlue;
	COutputEvent	m_OnLimitGreen;
	COutputEvent	m_OnLimitYellow;
	COutputEvent	m_OnLimitPurple;
	COutputEvent	m_OnLimitPink;

	COutputEvent	m_OnPointTimer;

	COutputEvent	m_OnPointTimerRed;
	COutputEvent	m_OnPointTimerBlue;
	COutputEvent	m_OnPointTimerGreen;
	COutputEvent	m_OnPointTimerYellow;
	COutputEvent	m_OnPointTimerPurple;
	COutputEvent	m_OnPointTimerPink;
};

LINK_ENTITY_TO_CLASS(fo_gamemode_ctp, CFOGamemodeCaptureThePoint);


BEGIN_DATADESC(CFOGamemodeCaptureThePoint)

DEFINE_KEYFIELD(m_nRedCaptures, FIELD_INTEGER, "redpoints"),
DEFINE_KEYFIELD(m_nBlueCaptures, FIELD_INTEGER, "bluepoints"),
DEFINE_KEYFIELD(m_nGreenCaptures, FIELD_INTEGER, "greenpoints"),
DEFINE_KEYFIELD(m_nYellowCaptures, FIELD_INTEGER, "yellowpoints"),
DEFINE_KEYFIELD(m_nPurpleCaptures, FIELD_INTEGER, "purplepoints"),
DEFINE_KEYFIELD(m_nPinkCaptures, FIELD_INTEGER, "pinkpoints"),

DEFINE_KEYFIELD(m_nPointLimit, FIELD_INTEGER, "pointlimit"),

DEFINE_INPUTFUNC(FIELD_INTEGER, "AddRedCapture", AddRedCapture),
DEFINE_INPUTFUNC(FIELD_INTEGER, "AddBlueCapture", AddBlueCapture),
DEFINE_INPUTFUNC(FIELD_INTEGER, "AddGreenCapture", AddGreenCapture),
DEFINE_INPUTFUNC(FIELD_INTEGER, "AddYellowCapture", AddYellowCapture),
DEFINE_INPUTFUNC(FIELD_INTEGER, "AddPurpleCapture", AddPurpleCapture),
DEFINE_INPUTFUNC(FIELD_INTEGER, "AddPinkCapture", AddPinkCapture),

DEFINE_INPUTFUNC(FIELD_VOID, "ActivateRedBounceBack", ActivateRedBounceBack),
DEFINE_INPUTFUNC(FIELD_VOID, "ActivateBlueBounceBack", ActivateBlueBounceBack),
DEFINE_INPUTFUNC(FIELD_VOID, "ActivateGreenBounceBack", ActivateGreenBounceBack),
DEFINE_INPUTFUNC(FIELD_VOID, "ActivateYellowBounceBack", ActivateYellowBounceBack),
DEFINE_INPUTFUNC(FIELD_VOID, "ActivatePurpleBounceBack", ActivatePurpleBounceBack),
DEFINE_INPUTFUNC(FIELD_VOID, "ActivatePinkBounceBack", ActivatePinkBounceBack),

DEFINE_INPUTFUNC(FIELD_VOID, "DisableRedBounceBack", DisableRedBounceBack),
DEFINE_INPUTFUNC(FIELD_VOID, "DisableBlueBounceBack", DisableBlueBounceBack),
DEFINE_INPUTFUNC(FIELD_VOID, "DisableGreenBounceBack", DisableGreenBounceBack),
DEFINE_INPUTFUNC(FIELD_VOID, "DisableYellowBounceBack", DisableYellowBounceBack),
DEFINE_INPUTFUNC(FIELD_VOID, "DisablePurpleBounceBack", DisablePurpleBounceBack),
DEFINE_INPUTFUNC(FIELD_VOID, "DisablePinkBounceBack", DisablePinkBounceBack),

DEFINE_INPUTFUNC(FIELD_VOID, "UpdateRedPointTimer", UpdateRedPointTimer),
DEFINE_INPUTFUNC(FIELD_VOID, "UpdateBluePointTimer", UpdateBluePointTimer),
DEFINE_INPUTFUNC(FIELD_VOID, "UpdateGreenPointTimer", UpdateGreenPointTimer),
DEFINE_INPUTFUNC(FIELD_VOID, "UpdateYellowPointTimer", UpdateYellowPointTimer),
DEFINE_INPUTFUNC(FIELD_VOID, "UpdatePurplePointTimer", UpdatePurplePointTimer),
DEFINE_INPUTFUNC(FIELD_VOID, "UpdatePinkPointTimer", UpdatePinkPointTimer),

DEFINE_INPUTFUNC(FIELD_VOID, "CheckPoints", CheckPoints),

DEFINE_INPUTFUNC(FIELD_VOID, "BounceBackThink", BounceBackThink),

DEFINE_OUTPUT(m_OnLimit, "OnLimit"),

DEFINE_OUTPUT(m_OnLimitRed, "OnLimitRed"),
DEFINE_OUTPUT(m_OnLimitBlue, "OnLimitBlue"),
DEFINE_OUTPUT(m_OnLimitGreen, "OnLimitGreen"),
DEFINE_OUTPUT(m_OnLimitYellow, "OnLimitYellow"),
DEFINE_OUTPUT(m_OnLimitPurple, "OnLimitPurple"),
DEFINE_OUTPUT(m_OnLimitPink, "OnLimitPink"),

DEFINE_OUTPUT(m_OnPointTimer, "OnPointTimer"),

DEFINE_OUTPUT(m_OnPointTimerRed, "OnPointTimerRed"),
DEFINE_OUTPUT(m_OnPointTimerBlue, "OnPointTimerBlue"),
DEFINE_OUTPUT(m_OnPointTimerGreen, "OnPointTimerGreen"),
DEFINE_OUTPUT(m_OnPointTimerYellow, "OnPointTimerYellow"),
DEFINE_OUTPUT(m_OnPointTimerPurple, "OnPointTimerPurple"),
DEFINE_OUTPUT(m_OnPointTimerPink, "OnPointTimerPink"),

END_DATADESC()

extern ConVar fo_ctp_red_score;
extern ConVar fo_ctp_blue_score;
extern ConVar fo_ctp_green_score;
extern ConVar fo_ctp_yellow_score;
extern ConVar fo_ctp_purple_score;
extern ConVar fo_ctp_pink_score;
extern ConVar fo_ctp_scorelimit;

extern ConVar fo_ctp_red_timer;
extern ConVar fo_ctp_blue_timer;
extern ConVar fo_ctp_green_timer;
extern ConVar fo_ctp_yellow_timer;
extern ConVar fo_ctp_purple_timer;
extern ConVar fo_ctp_pink_timer;

void CFOGamemodeCaptureThePoint::CheckPoints(inputdata_t &inputData)
{
	if (m_nRedCaptures >= m_nPointLimit)
	{
		m_OnLimit.FireOutput(this, this);
		m_OnLimitRed.FireOutput(this, this);
	}
	else if (m_nBlueCaptures >= m_nPointLimit)
	{
		m_OnLimit.FireOutput(this, this);
		m_OnLimitBlue.FireOutput(this, this);
	}
	else if (m_nGreenCaptures >= m_nPointLimit)
	{
		m_OnLimit.FireOutput(this, this);
		m_OnLimitGreen.FireOutput(this, this);
	}
	else if (m_nYellowCaptures >= m_nPointLimit)
	{
		m_OnLimit.FireOutput(this, this);
		m_OnLimitYellow.FireOutput(this, this);
	}
	else if (m_nPurpleCaptures >= m_nPointLimit)
	{
		m_OnLimit.FireOutput(this, this);
		m_OnLimitPurple.FireOutput(this, this);
	}
	else if (m_nPinkCaptures >= m_nPointLimit)
	{
		m_OnLimit.FireOutput(this, this);
		m_OnLimitPink.FireOutput(this, this);
	}

	fo_ctp_scorelimit.SetValue(m_nPointLimit);
	fo_ctp_red_score.SetValue(m_nRedCaptures);
	fo_ctp_blue_score.SetValue(m_nBlueCaptures);
	fo_ctp_green_score.SetValue(m_nGreenCaptures);
	fo_ctp_yellow_score.SetValue(m_nYellowCaptures);
	fo_ctp_purple_score.SetValue(m_nPurpleCaptures);
	fo_ctp_pink_score.SetValue(m_nPinkCaptures);

	fo_ctp_red_timer.SetValue(m_nRedPointTimer);
	fo_ctp_blue_timer.SetValue(m_nBluePointTimer);
	fo_ctp_green_timer.SetValue(m_nGreenPointTimer);
	fo_ctp_yellow_timer.SetValue(m_nYellowPointTimer);
	fo_ctp_purple_timer.SetValue(m_nPurplePointTimer);
	fo_ctp_pink_timer.SetValue(m_nPinkPointTimer);
}

void CFOGamemodeCaptureThePoint::AddRedCapture(inputdata_t &inputData)
{
	m_nRedCaptures += inputData.value.Int();

	CheckPoints(inputData);
}

void CFOGamemodeCaptureThePoint::AddBlueCapture(inputdata_t &inputData)
{
	m_nBlueCaptures += inputData.value.Int();

	CheckPoints(inputData);
}

void CFOGamemodeCaptureThePoint::AddGreenCapture(inputdata_t &inputData)
{
	m_nGreenCaptures += inputData.value.Int();

	CheckPoints(inputData);
}

void CFOGamemodeCaptureThePoint::AddYellowCapture(inputdata_t &inputData)
{
	m_nYellowCaptures += inputData.value.Int();

	CheckPoints(inputData);
}

void CFOGamemodeCaptureThePoint::AddPurpleCapture(inputdata_t &inputData)
{
	m_nPurpleCaptures += inputData.value.Int();

	CheckPoints(inputData);
}

void CFOGamemodeCaptureThePoint::AddPinkCapture(inputdata_t &inputData)
{
	m_nPinkCaptures += inputData.value.Int();

	CheckPoints(inputData);
}

void CFOGamemodeCaptureThePoint::BounceBackThink(inputdata_t &inputData)
{
	if (m_bRedBounceBack)
	{
		int i = 0;

		// respawn the players
		for (i = 1; i <= gpGlobals->maxClients; i++)
		{
			CTFPlayer *pPlayer = ToTFPlayer(UTIL_PlayerByIndex(i));
			if (pPlayer)
			{
				if (pPlayer->GetTeamNumber() == TF_TEAM_RED && pPlayer->IsDead())
				{
					if (pPlayer->GetPlayerClass() || pPlayer->GetPlayerClass()->GetClassIndex() != TF_CLASS_UNDEFINED)
					{
						// Allow them to spawn instantly when they do choose
						pPlayer->ForceRespawn();
						continue;
					}
				}
			}
		}
	}

	if (m_bBlueBounceBack)
	{
		int i = 0;

		// respawn the players
		for (i = 1; i <= gpGlobals->maxClients; i++)
		{
			CTFPlayer *pPlayer = ToTFPlayer(UTIL_PlayerByIndex(i));
			if (pPlayer)
			{
				if (pPlayer->GetTeamNumber() == TF_TEAM_BLUE && pPlayer->IsDead())
				{
					if (pPlayer->GetPlayerClass() || pPlayer->GetPlayerClass()->GetClassIndex() != TF_CLASS_UNDEFINED)
					{
						// Allow them to spawn instantly when they do choose
						pPlayer->ForceRespawn();
						continue;
					}
				}
			}
		}
	}

	if (m_bGreenBounceBack)
	{
		int i = 0;

		// respawn the players
		for (i = 1; i <= gpGlobals->maxClients; i++)
		{
			CTFPlayer *pPlayer = ToTFPlayer(UTIL_PlayerByIndex(i));
			if (pPlayer)
			{
				if (pPlayer->GetTeamNumber() == FO_TEAM_GREEN && pPlayer->IsDead())
				{
					if (pPlayer->GetPlayerClass() || pPlayer->GetPlayerClass()->GetClassIndex() != TF_CLASS_UNDEFINED)
					{
						// Allow them to spawn instantly when they do choose
						pPlayer->ForceRespawn();
						continue;
					}
				}
			}
		}
	}

	if (m_bYellowBounceBack)
	{
		int i = 0;

		// respawn the players
		for (i = 1; i <= gpGlobals->maxClients; i++)
		{
			CTFPlayer *pPlayer = ToTFPlayer(UTIL_PlayerByIndex(i));
			if (pPlayer)
			{
				if (pPlayer->GetTeamNumber() == FO_TEAM_YELLOW && pPlayer->IsDead())
				{
					if (pPlayer->GetPlayerClass() || pPlayer->GetPlayerClass()->GetClassIndex() != TF_CLASS_UNDEFINED)
					{
						// Allow them to spawn instantly when they do choose
						pPlayer->ForceRespawn();
						continue;
					}
				}
			}
		}
	}

	if (m_bPurpleBounceBack)
	{
		int i = 0;

		// respawn the players
		for (i = 1; i <= gpGlobals->maxClients; i++)
		{
			CTFPlayer *pPlayer = ToTFPlayer(UTIL_PlayerByIndex(i));
			if (pPlayer)
			{
				if (pPlayer->GetTeamNumber() == FO_TEAM_PURPLE && pPlayer->IsDead())
				{
					if (pPlayer->GetPlayerClass() || pPlayer->GetPlayerClass()->GetClassIndex() != TF_CLASS_UNDEFINED)
					{
						// Allow them to spawn instantly when they do choose
						pPlayer->ForceRespawn();
						continue;
					}
				}
			}
		}
	}

	if (m_bPinkBounceBack)
	{
		int i = 0;

		// respawn the players
		for (i = 1; i <= gpGlobals->maxClients; i++)
		{
			CTFPlayer *pPlayer = ToTFPlayer(UTIL_PlayerByIndex(i));
			if (pPlayer)
			{
				if (pPlayer->GetTeamNumber() == FO_TEAM_PINK && pPlayer->IsDead())
				{
					if (pPlayer->GetPlayerClass() || pPlayer->GetPlayerClass()->GetClassIndex() != TF_CLASS_UNDEFINED)
					{
						// Allow them to spawn instantly when they do choose
						pPlayer->ForceRespawn();
						continue;
					}
				}
			}
		}
	}
}

void CFOGamemodeCaptureThePoint::ActivateRedBounceBack(inputdata_t &inputData)
{
	m_bRedBounceBack = true;
	m_nRedPointTimer = 20;
	fo_ctp_red_timer.SetValue(m_nRedPointTimer);
}

void CFOGamemodeCaptureThePoint::ActivateBlueBounceBack(inputdata_t &inputData)
{
	m_bBlueBounceBack = true;
	m_nBluePointTimer = 20;
	fo_ctp_blue_timer.SetValue(m_nBluePointTimer);
}

void CFOGamemodeCaptureThePoint::ActivateGreenBounceBack(inputdata_t &inputData)
{
	m_bGreenBounceBack = true;
	m_nGreenPointTimer = 20;
	fo_ctp_green_timer.SetValue(m_nGreenPointTimer);
}

void CFOGamemodeCaptureThePoint::ActivateYellowBounceBack(inputdata_t &inputData)
{
	m_bYellowBounceBack = true;
	m_nYellowPointTimer = 20;
	fo_ctp_yellow_timer.SetValue(m_nYellowPointTimer);
}

void CFOGamemodeCaptureThePoint::ActivatePurpleBounceBack(inputdata_t &inputData)
{
	m_bPurpleBounceBack = true;
	m_nPurplePointTimer = 20;
	fo_ctp_purple_timer.SetValue(m_nPurplePointTimer);
}

void CFOGamemodeCaptureThePoint::ActivatePinkBounceBack(inputdata_t &inputData)
{
	m_bPinkBounceBack = true;
	m_nPinkPointTimer = 20;
	fo_ctp_pink_timer.SetValue(m_nPinkPointTimer);
}

void CFOGamemodeCaptureThePoint::DisableRedBounceBack(inputdata_t &inputData)
{
	m_bRedBounceBack = false;
}

void CFOGamemodeCaptureThePoint::DisableBlueBounceBack(inputdata_t &inputData)
{
	m_bBlueBounceBack = false;
}

void CFOGamemodeCaptureThePoint::DisableGreenBounceBack(inputdata_t &inputData)
{
	m_bGreenBounceBack = false;
}

void CFOGamemodeCaptureThePoint::DisableYellowBounceBack(inputdata_t &inputData)
{
	m_bYellowBounceBack = false;
}

void CFOGamemodeCaptureThePoint::DisablePurpleBounceBack(inputdata_t &inputData)
{
	m_bPurpleBounceBack = false;
}

void CFOGamemodeCaptureThePoint::DisablePinkBounceBack(inputdata_t &inputData)
{
	m_bPinkBounceBack = false;
}

void CFOGamemodeCaptureThePoint::UpdateRedPointTimer(inputdata_t &inputData)
{
	if (m_nRedPointTimer == -1)
	{
		m_nRedPointTimer = 21;
	}
	else
	{
		m_nRedPointTimer--;
	}

	if (m_nRedPointTimer < 0)
	{
		m_OnPointTimer.FireOutput(this, this);
		m_OnPointTimerRed.FireOutput(this, this);
	}

	fo_ctp_red_timer.SetValue(m_nRedPointTimer);
}

void CFOGamemodeCaptureThePoint::UpdateBluePointTimer(inputdata_t &inputData)
{
	if (m_nBluePointTimer == -1)
	{
		m_nBluePointTimer = 21;
	}
	else
	{
		m_nBluePointTimer--;
	}

	if (m_nBluePointTimer < 0)
	{
		m_OnPointTimer.FireOutput(this, this);
		m_OnPointTimerBlue.FireOutput(this, this);
	}

	fo_ctp_blue_timer.SetValue(m_nBluePointTimer);
}

void CFOGamemodeCaptureThePoint::UpdateGreenPointTimer(inputdata_t &inputData)
{
	if (m_nGreenPointTimer == -1)
	{
		m_nGreenPointTimer = 21;
	}
	else
	{
		m_nGreenPointTimer--;
	}

	if (m_nGreenPointTimer < 0)
	{
		m_OnPointTimer.FireOutput(this, this);
		m_OnPointTimerGreen.FireOutput(this, this);
	}

	fo_ctp_green_timer.SetValue(m_nGreenPointTimer);
}

void CFOGamemodeCaptureThePoint::UpdateYellowPointTimer(inputdata_t &inputData)
{
	if (m_nYellowPointTimer == -1)
	{
		m_nYellowPointTimer = 21;
	}
	else
	{
		m_nYellowPointTimer--;
	}

	if (m_nYellowPointTimer < 0)
	{
		m_OnPointTimer.FireOutput(this, this);
		m_OnPointTimerYellow.FireOutput(this, this);
	}

	fo_ctp_yellow_timer.SetValue(m_nYellowPointTimer);
}

void CFOGamemodeCaptureThePoint::UpdatePurplePointTimer(inputdata_t &inputData)
{
	if (m_nPurplePointTimer == -1)
	{
		m_nPurplePointTimer = 21;
	}
	else
	{
		m_nPurplePointTimer--;
	}

	if (m_nPurplePointTimer < 0)
	{
		m_OnPointTimer.FireOutput(this, this);
		m_OnPointTimerPurple.FireOutput(this, this);
	}

	fo_ctp_purple_timer.SetValue(m_nPurplePointTimer);
}

void CFOGamemodeCaptureThePoint::UpdatePinkPointTimer(inputdata_t &inputData)
{
	if (m_nPinkPointTimer == -1)
	{
		m_nPinkPointTimer = 21;
	}
	else
	{
		m_nPinkPointTimer--;
	}

	if (m_nPinkPointTimer < 0)
	{
		m_OnPointTimer.FireOutput(this, this);
		m_OnPointTimerPink.FireOutput(this, this);
	}

	fo_ctp_pink_timer.SetValue(m_nPinkPointTimer);
}
