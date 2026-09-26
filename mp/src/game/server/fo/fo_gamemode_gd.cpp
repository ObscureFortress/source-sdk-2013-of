// TURNIP CODE 
// 8/16/2023

#include "cbase.h"
#include "tf_gamerules.h"

class CFOGamemodeGeneratorDefense : public CLogicalEntity
{
public:
	DECLARE_CLASS(CFOGamemodeGeneratorDefense, CLogicalEntity);
	DECLARE_DATADESC();

	// Constructor
	CFOGamemodeGeneratorDefense()
	{
		m_nRedGenHealth = 0;
		m_nBlueGenHealth = 0;
		m_nGreenGenHealth = 0;
		m_nYellowGenHealth = 0;
		m_nPurpleGenHealth = 0;
		m_nPinkGenHealth = 0;

		m_nPointTimer = 61;

		m_bRedGenDestroyed = false;
		m_bBlueGenDestroyed = false;
		m_bGreenGenDestroyed = false;
		m_bYellowGenDestroyed = false;
		m_bPurpleGenDestroyed = false;
		m_bPinkGenDestroyed = false;
		m_bHasWon = false;
	}

	// Input function
	void OpenRedGen(inputdata_t &inputData);
	void OpenBlueGen(inputdata_t &inputData);
	void OpenGreenGen(inputdata_t &inputData);
	void OpenYellowGen(inputdata_t &inputData);
	void OpenPurpleGen(inputdata_t &inputData);
	void OpenPinkGen(inputdata_t &inputData);

	void CloseRedGen(inputdata_t &inputData);
	void CloseBlueGen(inputdata_t &inputData);
	void CloseGreenGen(inputdata_t &inputData);
	void CloseYellowGen(inputdata_t &inputData);
	void ClosePurpleGen(inputdata_t &inputData);
	void ClosePinkGen(inputdata_t &inputData);

	void UpdatePointTimer(inputdata_t &inputData);
	void ResetPointTimer(inputdata_t &inputData);

	void Activate(void);
	void Think(void);

	int	m_nRedGenHealth;
	int	m_nBlueGenHealth;
	int	m_nGreenGenHealth;
	int	m_nYellowGenHealth;
	int	m_nPurpleGenHealth;
	int	m_nPinkGenHealth;

	int	m_nPointTimer;

private:

	bool m_bRedGenDestroyed;
	bool m_bBlueGenDestroyed;
	bool m_bGreenGenDestroyed;
	bool m_bYellowGenDestroyed;
	bool m_bPurpleGenDestroyed;
	bool m_bPinkGenDestroyed;
	bool m_bHasWon;

	CHandle<CBaseCombatCharacter>	m_hRedGen;
	string_t				m_iszRedGen;

	CHandle<CBaseCombatCharacter>	m_hBlueGen;
	string_t				m_iszBlueGen;

	CHandle<CBaseCombatCharacter>	m_hGreenGen;
	string_t				m_iszGreenGen;

	CHandle<CBaseCombatCharacter>	m_hYellowGen;
	string_t				m_iszYellowGen;

	CHandle<CBaseCombatCharacter>	m_hPurpleGen;
	string_t				m_iszPurpleGen;

	CHandle<CBaseCombatCharacter>	m_hPinkGen;
	string_t				m_iszPinkGen;

	COutputEvent	m_RedGenOpened;
	COutputEvent	m_BlueGenOpened;
	COutputEvent	m_GreenGenOpened;
	COutputEvent	m_YellowGenOpened;
	COutputEvent	m_PurpleGenOpened;
	COutputEvent	m_PinkGenOpened;

	COutputEvent	m_RedGenClosed;
	COutputEvent	m_BlueGenClosed;
	COutputEvent	m_GreenGenClosed;
	COutputEvent	m_YellowGenClosed;
	COutputEvent	m_PurpleGenClosed;
	COutputEvent	m_PinkGenClosed;

	COutputEvent	m_RedGenDestroyed;
	COutputEvent	m_BlueGenDestroyed;
	COutputEvent	m_GreenGenDestroyed;
	COutputEvent	m_YellowGenDestroyed;
	COutputEvent	m_PurpleGenDestroyed;
	COutputEvent	m_PinkGenDestroyed;

	COutputEvent	m_OnPointTimer;

	COutputEvent	m_OnRedWin;
	COutputEvent	m_OnBlueWin;
	COutputEvent	m_OnGreenWin;
	COutputEvent	m_OnYellowWin;
	COutputEvent	m_OnPurpleWin;
	COutputEvent	m_OnPinkWin;
	COutputEvent	m_OnWin;
};

LINK_ENTITY_TO_CLASS(fo_gamemode_gd, CFOGamemodeGeneratorDefense);


BEGIN_DATADESC(CFOGamemodeGeneratorDefense)

	DEFINE_INPUTFUNC(FIELD_VOID, "OpenRedGen", OpenRedGen),
	DEFINE_INPUTFUNC(FIELD_VOID, "OpenBlueGen", OpenBlueGen),
	DEFINE_INPUTFUNC(FIELD_VOID, "OpenGreenGen", OpenGreenGen),
	DEFINE_INPUTFUNC(FIELD_VOID, "OpenYellowGen", OpenYellowGen),
	DEFINE_INPUTFUNC(FIELD_VOID, "OpenPurpleGen", OpenPurpleGen),
	DEFINE_INPUTFUNC(FIELD_VOID, "OpenPinkGen", OpenPinkGen),

	DEFINE_INPUTFUNC(FIELD_VOID, "CloseRedGen", CloseRedGen),
	DEFINE_INPUTFUNC(FIELD_VOID, "CloseBlueGen", CloseBlueGen),
	DEFINE_INPUTFUNC(FIELD_VOID, "CloseGreenGen", CloseGreenGen),
	DEFINE_INPUTFUNC(FIELD_VOID, "CloseYellowGen", CloseYellowGen),
	DEFINE_INPUTFUNC(FIELD_VOID, "ClosePurpleGen", ClosePurpleGen),
	DEFINE_INPUTFUNC(FIELD_VOID, "ClosePinkGen", ClosePinkGen),

	DEFINE_INPUTFUNC(FIELD_VOID, "UpdatePointTimer", UpdatePointTimer),
	DEFINE_INPUTFUNC(FIELD_VOID, "ResetPointTimer", ResetPointTimer),

	// this is stupid, but i dont know what else to do
	DEFINE_FIELD(m_hRedGen, FIELD_EHANDLE),
	DEFINE_KEYFIELD(m_iszRedGen, FIELD_STRING, "RedGen"),
	DEFINE_FIELD(m_hBlueGen, FIELD_EHANDLE),
	DEFINE_KEYFIELD(m_iszBlueGen, FIELD_STRING, "BlueGen"),
	DEFINE_FIELD(m_hGreenGen, FIELD_EHANDLE),
	DEFINE_KEYFIELD(m_iszGreenGen, FIELD_STRING, "GreenGen"),
	DEFINE_FIELD(m_hYellowGen, FIELD_EHANDLE),
	DEFINE_KEYFIELD(m_iszYellowGen, FIELD_STRING, "YellowGen"),
	DEFINE_FIELD(m_hPurpleGen, FIELD_EHANDLE),
	DEFINE_KEYFIELD(m_iszPurpleGen, FIELD_STRING, "PurpleGen"),
	DEFINE_FIELD(m_hPinkGen, FIELD_EHANDLE),
	DEFINE_KEYFIELD(m_iszPinkGen, FIELD_STRING, "PinkGen"),

	DEFINE_THINKFUNC(Think),

	DEFINE_OUTPUT(m_RedGenOpened, "RedGenOpened"),
	DEFINE_OUTPUT(m_BlueGenOpened, "BlueGenOpened"),
	DEFINE_OUTPUT(m_GreenGenOpened, "GreenGenOpened"),
	DEFINE_OUTPUT(m_YellowGenOpened, "YellowGenOpened"),
	DEFINE_OUTPUT(m_PurpleGenOpened, "PurpleGenOpened"),
	DEFINE_OUTPUT(m_PinkGenOpened, "PinkGenOpened"),

	DEFINE_OUTPUT(m_RedGenClosed, "RedGenClosed"),
	DEFINE_OUTPUT(m_BlueGenClosed, "BlueGenClosed"),
	DEFINE_OUTPUT(m_GreenGenClosed, "GreenGenClosed"),
	DEFINE_OUTPUT(m_YellowGenClosed, "YellowGenClosed"),
	DEFINE_OUTPUT(m_PurpleGenClosed, "PurpleGenClosed"),
	DEFINE_OUTPUT(m_PinkGenClosed, "PinkGenClosed"),

	DEFINE_OUTPUT(m_RedGenDestroyed, "RedGenDestroyed"),
	DEFINE_OUTPUT(m_BlueGenDestroyed, "BlueGenDestroyed"),
	DEFINE_OUTPUT(m_GreenGenDestroyed, "GreenGenDestroyed"),
	DEFINE_OUTPUT(m_YellowGenDestroyed, "YellowGenDestroyed"),
	DEFINE_OUTPUT(m_PurpleGenDestroyed, "PurpleGenDestroyed"),
	DEFINE_OUTPUT(m_PinkGenDestroyed, "PinkGenDestroyed"),

	DEFINE_OUTPUT(m_OnPointTimer, "OnPointTimer"),

	DEFINE_OUTPUT(m_OnRedWin, "OnRedWin"),
	DEFINE_OUTPUT(m_OnBlueWin, "OnBlueWin"),
	DEFINE_OUTPUT(m_OnGreenWin, "OnGreenWin"),
	DEFINE_OUTPUT(m_OnYellowWin, "OnYellowWin"),
	DEFINE_OUTPUT(m_OnPurpleWin, "OnPurpleWin"),
	DEFINE_OUTPUT(m_OnPinkWin, "OnPinkWin"),
	DEFINE_OUTPUT(m_OnWin, "OnWin"),

END_DATADESC()

extern ConVar fo_gd_red_health;
extern ConVar fo_gd_blue_health;
extern ConVar fo_gd_green_health;
extern ConVar fo_gd_yellow_health;
extern ConVar fo_gd_purple_health;
extern ConVar fo_gd_pink_health;

extern ConVar fo_gd_point_timer;

//-----------------------------------------------------------------------------
// Purpose: Precache function for the entity
//-----------------------------------------------------------------------------
void CFOGamemodeGeneratorDefense::Activate(void)
{
	BaseClass::Activate();

	m_bHasWon = false;

	fo_gd_point_timer.SetValue(m_nPointTimer);

	if (m_iszRedGen != NULL_STRING)
	{
		CBaseEntity *pEnt = gEntList.FindEntityByName(NULL, STRING(m_iszRedGen));
		if (!pEnt)
		{
			Warning("%s(%s) unable to find associated entity named '%s'.\n", GetClassname(), GetDebugName(), STRING(m_iszRedGen));
		}
		else
		{
			m_hRedGen = dynamic_cast<CBaseCombatCharacter*>(pEnt);
			if (!m_hRedGen)
			{
				Warning("%s(%s) tried to use associated entity named '%s', but it isn't a team_generator entity. \n", GetClassname(), GetDebugName(), STRING(m_iszRedGen));
			}
		}
	}
	else
	{
		Warning("%s(%s) has no associated RED generator.\n", GetClassname(), GetDebugName());
	}

	if (m_iszBlueGen != NULL_STRING)
	{
		CBaseEntity *pEnt = gEntList.FindEntityByName(NULL, STRING(m_iszBlueGen));
		if (!pEnt)
		{
			Warning("%s(%s) unable to find associated entity named '%s'.\n", GetClassname(), GetDebugName(), STRING(m_iszBlueGen));
		}
		else
		{
			m_hBlueGen = dynamic_cast<CBaseCombatCharacter*>(pEnt);
			if (!m_hBlueGen)
			{
				Warning("%s(%s) tried to use associated entity named '%s', but it isn't a team_generator entity. \n", GetClassname(), GetDebugName(), STRING(m_iszBlueGen));
			}
		}
	}
	else
	{
		Warning("%s(%s) has no associated BLU generator.\n", GetClassname(), GetDebugName());
	}
	if (TFGameRules() && TFGameRules()->m_nExtraTeamMode == 1)
	{
		if (m_iszGreenGen != NULL_STRING)
		{
			CBaseEntity *pEnt = gEntList.FindEntityByName(NULL, STRING(m_iszGreenGen));
			if (!pEnt)
			{
				Warning("%s(%s) unable to find associated entity named '%s'.\n", GetClassname(), GetDebugName(), STRING(m_iszGreenGen));
			}
			else
			{
				m_hGreenGen = dynamic_cast<CBaseCombatCharacter*>(pEnt);
				if (!m_hGreenGen)
				{
					Warning("%s(%s) tried to use associated entity named '%s', but it isn't a team_generator entity. \n", GetClassname(), GetDebugName(), STRING(m_iszGreenGen));
				}
			}
		}
		else
		{
			Warning("%s(%s) has no associated GRN generator.\n", GetClassname(), GetDebugName());
		}
	}
	if (TFGameRules() && TFGameRules()->m_nExtraTeamMode == 2)
	{
		if (m_iszYellowGen != NULL_STRING)
		{
			CBaseEntity *pEnt = gEntList.FindEntityByName(NULL, STRING(m_iszYellowGen));
			if (!pEnt)
			{
				Warning("%s(%s) unable to find associated entity named '%s'.\n", GetClassname(), GetDebugName(), STRING(m_iszYellowGen));
			}
			else
			{
				m_hYellowGen = dynamic_cast<CBaseCombatCharacter*>(pEnt);
				if (!m_hYellowGen)
				{
					Warning("%s(%s) tried to use associated entity named '%s', but it isn't a team_generator entity. \n", GetClassname(), GetDebugName(), STRING(m_iszYellowGen));
				}
			}
		}
		else
		{
			Warning("%s(%s) has no associated YLW generator.\n", GetClassname(), GetDebugName());
		}

		if (m_iszPurpleGen != NULL_STRING)
		{
			CBaseEntity *pEnt = gEntList.FindEntityByName(NULL, STRING(m_iszPurpleGen));
			if (!pEnt)
			{
				Warning("%s(%s) unable to find associated entity named '%s'.\n", GetClassname(), GetDebugName(), STRING(m_iszPurpleGen));
			}
			else
			{
				m_hPurpleGen = dynamic_cast<CBaseCombatCharacter*>(pEnt);
				if (!m_hPurpleGen)
				{
					Warning("%s(%s) tried to use associated entity named '%s', but it isn't a team_generator entity. \n", GetClassname(), GetDebugName(), STRING(m_iszPurpleGen));
				}
			}
		}
		else
		{
			Warning("%s(%s) has no associated PRP generator.\n", GetClassname(), GetDebugName());
		}

		if (m_iszPinkGen != NULL_STRING)
		{
			CBaseEntity *pEnt = gEntList.FindEntityByName(NULL, STRING(m_iszPinkGen));
			if (!pEnt)
			{
				Warning("%s(%s) unable to find associated entity named '%s'.\n", GetClassname(), GetDebugName(), STRING(m_iszPinkGen));
			}
			else
			{
				m_hPinkGen = dynamic_cast<CBaseCombatCharacter*>(pEnt);
				if (!m_hPinkGen)
				{
					Warning("%s(%s) tried to use associated entity named '%s', but it isn't a team_generator entity. \n", GetClassname(), GetDebugName(), STRING(m_iszPinkGen));
				}
			}
		}
		else
		{
			Warning("%s(%s) has no associated PNK generator.\n", GetClassname(), GetDebugName());
		}
	}

	SetContextThink(&CFOGamemodeGeneratorDefense::Think, gpGlobals->curtime + 0.1f, "gdthink");
}

void CFOGamemodeGeneratorDefense::Think(void)
{
	if (m_hRedGen != NULL && !m_bRedGenDestroyed)
	{
		m_nRedGenHealth = m_hRedGen->GetHealth();

		if (m_nRedGenHealth <= 0)
		{
			m_nRedGenHealth = 0;
			m_RedGenDestroyed.FireOutput(this, this);
			m_bRedGenDestroyed = true;
		}
	}
	if (m_hBlueGen != NULL && !m_bBlueGenDestroyed)
	{
		m_nBlueGenHealth = m_hBlueGen->GetHealth();

		if (m_nBlueGenHealth <= 0)
		{
			m_nBlueGenHealth = 0;
			m_BlueGenDestroyed.FireOutput(this, this);
			m_bBlueGenDestroyed = true;
		}
	}
	if (m_hGreenGen != NULL && !m_bGreenGenDestroyed)
	{
		m_nGreenGenHealth = m_hGreenGen->GetHealth();

		if (m_nGreenGenHealth <= 0)
		{
			m_nGreenGenHealth = 0;
			m_GreenGenDestroyed.FireOutput(this, this);
			m_bGreenGenDestroyed = true;
		}
	}
	if (m_hYellowGen != NULL && !m_bYellowGenDestroyed)
	{
		m_nYellowGenHealth = m_hYellowGen->GetHealth();

		if (m_nYellowGenHealth <= 0)
		{
			m_nYellowGenHealth = 0;
			m_YellowGenDestroyed.FireOutput(this, this);
			m_bYellowGenDestroyed = true;
		}
	}
	if (m_hPurpleGen != NULL && !m_bPurpleGenDestroyed)
	{
		m_nPurpleGenHealth = m_hPurpleGen->GetHealth();

		if (m_nPurpleGenHealth <= 0)
		{
			m_nPurpleGenHealth = 0;
			m_PurpleGenDestroyed.FireOutput(this, this);
			m_bPurpleGenDestroyed = true;
		}
	}
	if (m_hPinkGen != NULL && !m_bPinkGenDestroyed)
	{
		m_nPinkGenHealth = m_hPinkGen->GetHealth();

		if (m_nPinkGenHealth <= 0)
		{
			m_nPinkGenHealth = 0;
			m_PinkGenDestroyed.FireOutput(this, this);
			m_bPinkGenDestroyed = true;
		}
	}

	if (!m_bHasWon)
	{
		if (TFGameRules() && TFGameRules()->m_nExtraTeamMode == 1)
		{
			if (m_nBlueGenHealth == 0 && m_nGreenGenHealth == 0)
			{
				m_OnRedWin.FireOutput(this, this);
				m_OnWin.FireOutput(this, this);
				m_bHasWon = true;
			}
			if (m_nRedGenHealth == 0 && m_nGreenGenHealth == 0)
			{
				m_OnBlueWin.FireOutput(this, this);
				m_OnWin.FireOutput(this, this);
				m_bHasWon = true;
			}
			if (m_nRedGenHealth == 0 && m_nBlueGenHealth == 0)
			{
				m_OnGreenWin.FireOutput(this, this);
				m_OnWin.FireOutput(this, this);
				m_bHasWon = true;
			}
		}
		if (TFGameRules() && TFGameRules()->m_nExtraTeamMode == 2)
		{
			if (m_nBlueGenHealth == 0 && m_nGreenGenHealth == 0 && m_nYellowGenHealth == 0 && m_nPurpleGenHealth == 0 && m_nPinkGenHealth == 0)
			{
				m_OnRedWin.FireOutput(this, this);
				m_OnWin.FireOutput(this, this);
				m_bHasWon = true;
			}
			if (m_nRedGenHealth == 0 && m_nGreenGenHealth == 0 && m_nYellowGenHealth == 0 && m_nPurpleGenHealth == 0 && m_nPinkGenHealth == 0)
			{
				m_OnBlueWin.FireOutput(this, this);
				m_OnWin.FireOutput(this, this);
				m_bHasWon = true;
			}
			if (m_nRedGenHealth == 0 && m_nBlueGenHealth == 0 && m_nYellowGenHealth == 0 && m_nPurpleGenHealth == 0 && m_nPinkGenHealth == 0)
			{
				m_OnGreenWin.FireOutput(this, this);
				m_OnWin.FireOutput(this, this);
				m_bHasWon = true;
			}
			if (m_nRedGenHealth == 0 && m_nBlueGenHealth == 0 && m_nGreenGenHealth == 0 && m_nPurpleGenHealth == 0 && m_nPinkGenHealth == 0)
			{
				m_OnYellowWin.FireOutput(this, this);
				m_OnWin.FireOutput(this, this);
				m_bHasWon = true;
			}
			if (m_nRedGenHealth == 0 && m_nBlueGenHealth == 0 && m_nGreenGenHealth == 0 && m_nYellowGenHealth == 0 && m_nPinkGenHealth == 0)
			{
				m_OnPurpleWin.FireOutput(this, this);
				m_OnWin.FireOutput(this, this);
				m_bHasWon = true;
			}
			if (m_nRedGenHealth == 0 && m_nBlueGenHealth == 0 && m_nGreenGenHealth == 0 && m_nYellowGenHealth == 0 && m_nPurpleGenHealth == 0)
			{
				m_OnPinkWin.FireOutput(this, this);
				m_OnWin.FireOutput(this, this);
				m_bHasWon = true;
			}
		}
	}

	fo_gd_red_health.SetValue(m_nRedGenHealth);
	fo_gd_blue_health.SetValue(m_nBlueGenHealth);
	fo_gd_green_health.SetValue(m_nGreenGenHealth);
	fo_gd_yellow_health.SetValue(m_nYellowGenHealth);
	fo_gd_purple_health.SetValue(m_nPurpleGenHealth);
	fo_gd_pink_health.SetValue(m_nPinkGenHealth);

	SetNextThink(gpGlobals->curtime + 0.1f, "gdthink");
}

void CFOGamemodeGeneratorDefense::UpdatePointTimer(inputdata_t &inputData)
{
	if (m_nPointTimer == -1)
	{
		m_nPointTimer = 61;
	}
	else
	{
		m_nPointTimer--;
	}

	if (m_nPointTimer < 0)
	{
		m_OnPointTimer.FireOutput(this, this);
		m_RedGenClosed.FireOutput(this, this);
		m_BlueGenClosed.FireOutput(this, this);
		m_GreenGenClosed.FireOutput(this, this);
		m_YellowGenClosed.FireOutput(this, this);
		m_PurpleGenClosed.FireOutput(this, this);
		m_PinkGenClosed.FireOutput(this, this);
	}

	fo_gd_point_timer.SetValue(m_nPointTimer);
}

void CFOGamemodeGeneratorDefense::ResetPointTimer(inputdata_t &inputData)
{
	m_nPointTimer = 61;

	fo_gd_point_timer.SetValue(m_nPointTimer);
}

void CFOGamemodeGeneratorDefense::OpenRedGen(inputdata_t &inputData)
{
	m_RedGenOpened.FireOutput(this, this);
}

void CFOGamemodeGeneratorDefense::OpenBlueGen(inputdata_t &inputData)
{
	m_BlueGenOpened.FireOutput(this, this);
}

void CFOGamemodeGeneratorDefense::OpenGreenGen(inputdata_t &inputData)
{
	m_GreenGenOpened.FireOutput(this, this);
}

void CFOGamemodeGeneratorDefense::OpenYellowGen(inputdata_t &inputData)
{
	m_YellowGenOpened.FireOutput(this, this);
}

void CFOGamemodeGeneratorDefense::OpenPurpleGen(inputdata_t &inputData)
{
	m_PurpleGenOpened.FireOutput(this, this);
}

void CFOGamemodeGeneratorDefense::OpenPinkGen(inputdata_t &inputData)
{
	m_PinkGenOpened.FireOutput(this, this);
}

void CFOGamemodeGeneratorDefense::CloseRedGen(inputdata_t &inputData)
{
	m_RedGenClosed.FireOutput(this, this);
}

void CFOGamemodeGeneratorDefense::CloseBlueGen(inputdata_t &inputData)
{
	m_BlueGenClosed.FireOutput(this, this);
}

void CFOGamemodeGeneratorDefense::CloseGreenGen(inputdata_t &inputData)
{
	m_GreenGenClosed.FireOutput(this, this);
}

void CFOGamemodeGeneratorDefense::CloseYellowGen(inputdata_t &inputData)
{
	m_YellowGenClosed.FireOutput(this, this);
}

void CFOGamemodeGeneratorDefense::ClosePurpleGen(inputdata_t &inputData)
{
	m_PurpleGenClosed.FireOutput(this, this);
}

void CFOGamemodeGeneratorDefense::ClosePinkGen(inputdata_t &inputData)
{
	m_PinkGenClosed.FireOutput(this, this);
}