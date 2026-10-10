// TURNIP CODE 
// 10/3/2022

#include "cbase.h"
#include "entity_capture_flag.h"

extern ConVar fo_ditr_diamond_progress;
extern ConVar fo_ditr_diamond_digging;

class CFODiamondHole : public CBaseCombatCharacter
{
public:
	DECLARE_CLASS(CFODiamondHole, CBaseCombatCharacter);
	DECLARE_DATADESC();

	// Constructor
	CFODiamondHole()
	{
		m_flDig = 0;
		m_bFirstTouch = true;
		m_bReachedMax = false;
	}

	// Input function
	virtual int		OnTakeDamage(const CTakeDamageInfo &info);

	void Activate(void);

	void ResetAll(inputdata_t &inputData);
	void HoleChosen(inputdata_t &inputData);

private:

	int m_flDigMax;

	float	m_flDig;
	bool	m_bFirstTouch; // :flushed:
	bool	m_bReachedMax;

	// Which percentage outputs have fired
	bool	m_b90Done;
	bool	m_b80Done;
	bool	m_b70Done;
	bool	m_b60Done;
	bool	m_b50Done;
	bool	m_b40Done;
	bool	m_b30Done;
	bool	m_b20Done;
	bool	m_b10Done;

	string_t				m_iszModel;

	CHandle<CCaptureFlag>	m_hAssociatedFlag;
	string_t				m_iszAssociatedFlag;

	string_t				m_iszFlagModel;

	COutputEvent	m_OnFirstDig;
	COutputEvent	m_OnDig;
	COutputEvent	m_OnReachDigMax;

	COutputEvent	m_On10PercentDig;
	COutputEvent	m_On20PercentDig;
	COutputEvent	m_On30PercentDig;
	COutputEvent	m_On40PercentDig;
	COutputEvent	m_On50PercentDig;
	COutputEvent	m_On60PercentDig;
	COutputEvent	m_On70PercentDig;
	COutputEvent	m_On80PercentDig;
	COutputEvent	m_On90PercentDig;
};

LINK_ENTITY_TO_CLASS(fo_diamond_hole, CFODiamondHole);


BEGIN_DATADESC(CFODiamondHole)

	DEFINE_INPUTFUNC(FIELD_VOID, "ResetAll", ResetAll),
	DEFINE_INPUTFUNC(FIELD_VOID, "HoleChosen", HoleChosen),

	DEFINE_KEYFIELD(m_iszModel, FIELD_STRING, "Model"),

	DEFINE_KEYFIELD(m_flDigMax, FIELD_INTEGER, "DigMax"),

	DEFINE_FIELD(m_hAssociatedFlag, FIELD_EHANDLE),
	DEFINE_KEYFIELD(m_iszAssociatedFlag, FIELD_STRING, "AssociatedFlag"),

	DEFINE_KEYFIELD(m_iszFlagModel, FIELD_STRING, "FlagModel"),

	DEFINE_OUTPUT(m_OnFirstDig, "OnFirstDig"),
	DEFINE_OUTPUT(m_OnDig, "OnDig"),
	DEFINE_OUTPUT(m_OnReachDigMax, "OnReachDigMax"),

	DEFINE_OUTPUT(m_On10PercentDig, "On10PercentDig"),
	DEFINE_OUTPUT(m_On20PercentDig, "On20PercentDig"),
	DEFINE_OUTPUT(m_On30PercentDig, "On30PercentDig"),
	DEFINE_OUTPUT(m_On40PercentDig, "On40PercentDig"),
	DEFINE_OUTPUT(m_On50PercentDig, "On50PercentDig"),
	DEFINE_OUTPUT(m_On60PercentDig, "On60PercentDig"),
	DEFINE_OUTPUT(m_On70PercentDig, "On70PercentDig"),
	DEFINE_OUTPUT(m_On80PercentDig, "On80PercentDig"),
	DEFINE_OUTPUT(m_On90PercentDig, "On90PercentDig"),

END_DATADESC()

//-----------------------------------------------------------------------------
// Purpose: Precache function for the entity
//-----------------------------------------------------------------------------
void CFODiamondHole::Activate(void)
{
	BaseClass::Activate();

	fo_ditr_diamond_progress.SetValue(0);
	fo_ditr_diamond_digging.SetValue(0);

	m_takedamage = 2;
	m_iHealth = 100000000;

	PrecacheModel(STRING(m_iszModel));
	PrecacheModel(STRING(m_iszFlagModel));

	SetModel(STRING(m_iszModel));

	AddEffects(EF_NODRAW);
	SetBodygroup(0, 0);

	if (m_iszAssociatedFlag != NULL_STRING)
	{
		CBaseEntity *pEnt = gEntList.FindEntityByName(NULL, STRING(m_iszAssociatedFlag));
		if (!pEnt)
		{
			Warning("%s(%s) unable to find associated flag named '%s'.\n", GetClassname(), GetDebugName(), STRING(m_iszAssociatedFlag));
		}
		else
		{
			m_hAssociatedFlag = dynamic_cast<CCaptureFlag*>(pEnt);
			if (!m_hAssociatedFlag)
			{
				Warning("%s(%s) tried to use associated flag named '%s', but it isn't a flag entity.\n", GetClassname(), GetDebugName(), STRING(m_iszAssociatedFlag));
			}
		}
	}
	else
	{
		Warning("%s(%s) has no associated flag.\n", GetClassname(), GetDebugName());
	}

	/*
	if (!VPhysicsGetObject())
	{
		VPhysicsInitStatic();
	}
	*/

	SetSolid(SOLID_NONE);
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
int CFODiamondHole::OnTakeDamage(const CTakeDamageInfo &info)
{
	if (m_bReachedMax)
		return 0;

	if (m_takedamage == DAMAGE_NO)
		m_takedamage = DAMAGE_YES;

	m_iHealth = 100000000;

	if ((DMG_CLUB & info.GetDamageType()) || (DMG_SLASH & info.GetDamageType())) // only let players dig if they're using melee
	{
		float flDamage = info.GetDamage();

		if (flDamage)
		{
			fo_ditr_diamond_progress.SetValue(RoundFloatToInt((m_flDig / m_flDigMax) * 100.0f));

			if (!m_bFirstTouch)
			{
				SetHealth(m_flDig + flDamage);
				m_flDig += flDamage;

				// Each threshold fires once
				if (m_flDigMax * 0.9 <= m_flDig && !m_b90Done)
				{
					m_On90PercentDig.FireOutput(info.GetAttacker(), this);
					m_b90Done = true;
				}

				if (m_flDigMax * 0.8 <= m_flDig && !m_b80Done)
				{
					m_On80PercentDig.FireOutput(info.GetAttacker(), this);
					m_b80Done = true;
				}

				if (m_flDigMax * 0.7 <= m_flDig && !m_b70Done)
				{
					m_On70PercentDig.FireOutput(info.GetAttacker(), this);
					m_b70Done = true;
				}

				if (m_flDigMax * 0.6 <= m_flDig && !m_b60Done)
				{
					m_On60PercentDig.FireOutput(info.GetAttacker(), this);
					m_b60Done = true;
				}

				if (m_flDigMax * 0.5 <= m_flDig && !m_b50Done)
				{
					m_On50PercentDig.FireOutput(info.GetAttacker(), this);
					m_b50Done = true;
				}

				if (m_flDigMax * 0.4 <= m_flDig && !m_b40Done)
				{
					m_On40PercentDig.FireOutput(info.GetAttacker(), this);
					m_b40Done = true;
				}

				if (m_flDigMax * 0.3 <= m_flDig && !m_b30Done)
				{
					m_On30PercentDig.FireOutput(info.GetAttacker(), this);
					m_b30Done = true;
				}

				if (m_flDigMax * 0.2 <= m_flDig && !m_b20Done)
				{
					m_On20PercentDig.FireOutput(info.GetAttacker(), this);
					m_b20Done = true;
				}

				if (m_flDigMax * 0.1 <= m_flDig && !m_b10Done)
				{
					m_On10PercentDig.FireOutput(info.GetAttacker(), this);
					m_b10Done = true;
				}
			}
			else
			{
				m_OnFirstDig.FireOutput(info.GetAttacker(), this); // diamond revealed - its showtime baby!
				fo_ditr_diamond_digging.SetValue(1);
				m_bFirstTouch = false;
			}
		}

		m_OnDig.FireOutput(info.GetAttacker(), this);

		if (m_flDig >= m_flDigMax)
		{
			m_OnReachDigMax.FireOutput(info.GetAttacker(), this);
			fo_ditr_diamond_digging.SetValue(0);
			m_bReachedMax = true;
		}

		return flDamage;
	}
	else
	{
		return 0;
	}
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CFODiamondHole::ResetAll(inputdata_t &inputData)
{
	fo_ditr_diamond_progress.SetValue(0);
	fo_ditr_diamond_digging.SetValue(0);

	m_flDig = 0;
	SetBodygroup(0, 0);
	m_bFirstTouch = true;
	m_bReachedMax = false;

	m_b90Done = false;
	m_b80Done = false;
	m_b70Done = false;
	m_b60Done = false;
	m_b50Done = false;
	m_b40Done = false;
	m_b30Done = false;
	m_b20Done = false;
	m_b10Done = false;

	AddEffects(EF_NODRAW);

	SetSolid(SOLID_NONE);
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CFODiamondHole::HoleChosen(inputdata_t &inputData)
{
	m_flDig = 0;
	RemoveEffects(EF_NODRAW);
	SetBodygroup(0, 0);

	m_hAssociatedFlag->SetModel(STRING(m_iszFlagModel));

	SetSolid(SOLID_VPHYSICS);
}
