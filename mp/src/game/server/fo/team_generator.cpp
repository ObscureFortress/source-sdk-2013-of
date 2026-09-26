// TURNIP CODE 
// 8/16/2023

#include "cbase.h"
#include "entity_capture_flag.h"

class CFOTeamGenerator : public CBaseCombatCharacter
{
public:
	DECLARE_CLASS(CFOTeamGenerator, CBaseCombatCharacter);
	DECLARE_DATADESC();

	// Constructor
	CFOTeamGenerator()
	{
	}

	// Input function
	virtual int		OnTakeDamage(const CTakeDamageInfo &info);
	virtual void	Destroyed(const CTakeDamageInfo &info);

	void DestroyThyselfHeathen(inputdata_t &inputData);

	void Activate(void);

private:

	int m_nSetHealth;

	bool m_b90Done;
	bool m_b80Done;
	bool m_b70Done;
	bool m_b60Done;
	bool m_b50Done;
	bool m_b40Done;
	bool m_b30Done;
	bool m_b20Done;
	bool m_b10Done;
	bool m_bDestroyed;

	//float	m_flDig;
	//bool	m_bFirstTouch; // :flushed:
	//bool	m_bReachedMax;

	string_t				m_iszModel;

	//CHandle<CCaptureFlag>	m_hAssociatedFlag;
	//string_t				m_iszAssociatedFlag;

	//string_t				m_iszFlagModel;

	//COutputEvent	m_OnFirstDig;
	//COutputEvent	m_OnDig;
	//COutputEvent	m_OnReachDigMax;

	COutputEvent	m_On10PercentHealth;
	COutputEvent	m_On20PercentHealth;
	COutputEvent	m_On30PercentHealth;
	COutputEvent	m_On40PercentHealth;
	COutputEvent	m_On50PercentHealth;
	COutputEvent	m_On60PercentHealth;
	COutputEvent	m_On70PercentHealth;
	COutputEvent	m_On80PercentHealth;
	COutputEvent	m_On90PercentHealth;

	COutputEvent	m_OnDestroyed;
};

LINK_ENTITY_TO_CLASS(team_generator, CFOTeamGenerator);


BEGIN_DATADESC(CFOTeamGenerator)

	//DEFINE_INPUTFUNC(FIELD_VOID, "ResetAll", ResetAll),
	DEFINE_INPUTFUNC(FIELD_VOID, "DestroyThyselfHeathen", DestroyThyselfHeathen),

	DEFINE_KEYFIELD(m_iszModel, FIELD_STRING, "Model"),

	DEFINE_KEYFIELD(m_nSetHealth, FIELD_INTEGER, "SetHealth"),

	//DEFINE_FIELD(m_hAssociatedFlag, FIELD_EHANDLE),
	//DEFINE_KEYFIELD(m_iszAssociatedFlag, FIELD_STRING, "AssociatedFlag"),

	//DEFINE_KEYFIELD(m_iszFlagModel, FIELD_STRING, "FlagModel"),

	//DEFINE_OUTPUT(m_OnFirstDig, "OnFirstDig"),
	//DEFINE_OUTPUT(m_OnDig, "OnDig"),
	//DEFINE_OUTPUT(m_OnReachDigMax, "OnReachDigMax"),

	DEFINE_OUTPUT(m_On10PercentHealth, "On10PercentHealth"),
	DEFINE_OUTPUT(m_On20PercentHealth, "On20PercentHealth"),
	DEFINE_OUTPUT(m_On30PercentHealth, "On30PercentHealth"),
	DEFINE_OUTPUT(m_On40PercentHealth, "On40PercentHealth"),
	DEFINE_OUTPUT(m_On50PercentHealth, "On50PercentHealth"),
	DEFINE_OUTPUT(m_On60PercentHealth, "On60PercentHealth"),
	DEFINE_OUTPUT(m_On70PercentHealth, "On70PercentHealth"),
	DEFINE_OUTPUT(m_On80PercentHealth, "On80PercentHealth"),
	DEFINE_OUTPUT(m_On90PercentHealth, "On90PercentHealth"),
	DEFINE_OUTPUT(m_OnDestroyed, "OnDestroyed"),

END_DATADESC()

//-----------------------------------------------------------------------------
// Purpose: Precache function for the entity
//-----------------------------------------------------------------------------
void CFOTeamGenerator::Activate(void)
{
	BaseClass::Activate();

	m_b90Done = false;
	m_b80Done = false;
	m_b70Done = false;
	m_b60Done = false;
	m_b50Done = false;
	m_b40Done = false;
	m_b30Done = false;
	m_b20Done = false;
	m_b10Done = false;
	m_bDestroyed = false;

	m_takedamage = 2;
	m_iHealth = m_nSetHealth;

	PrecacheModel(STRING(m_iszModel));

	SetModel(STRING(m_iszModel));

	//SetBodygroup(0, 0);

	SetSolid(SOLID_BBOX);
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
int CFOTeamGenerator::OnTakeDamage(const CTakeDamageInfo &info)
{
	if (m_takedamage == DAMAGE_NO)
		m_takedamage = DAMAGE_YES;

	float flDamage = info.GetDamage();
	int nTeam = info.GetAttacker()->GetTeamNumber();

	//Warning("%d - generator - %d - attacker\n", GetTeamNumber(), nTeam);

	if (GetTeamNumber() != nTeam)
		m_iHealth -= flDamage;

	//Warning("%d\n", m_iHealth);

	if (((m_nSetHealth * 0.9) >= m_iHealth) && !m_b90Done)
	{
		m_On90PercentHealth.FireOutput(info.GetAttacker(), this);
		m_b90Done = true;
	}
	if (((m_nSetHealth * 0.8) >= m_iHealth) && !m_b80Done)
	{
		m_On80PercentHealth.FireOutput(info.GetAttacker(), this);
		m_b80Done = true;
	}
	if (((m_nSetHealth * 0.7) >= m_iHealth) && !m_b70Done)
	{
		m_On70PercentHealth.FireOutput(info.GetAttacker(), this);
		m_b70Done = true;
	}
	if (((m_nSetHealth * 0.6) >= m_iHealth) && !m_b60Done)
	{
		m_On60PercentHealth.FireOutput(info.GetAttacker(), this);
		m_b60Done = true;
	}
	if (((m_nSetHealth * 0.5) >= m_iHealth) && !m_b50Done)
	{
		m_On50PercentHealth.FireOutput(info.GetAttacker(), this);
		m_b50Done = true;
	}
	if (((m_nSetHealth * 0.4) >= m_iHealth) && !m_b40Done)
	{
		m_On40PercentHealth.FireOutput(info.GetAttacker(), this);
		m_b40Done = true;
	}
	if (((m_nSetHealth * 0.3) >= m_iHealth) && !m_b30Done)
	{
		m_On30PercentHealth.FireOutput(info.GetAttacker(), this);
		m_b30Done = true;
	}
	if (((m_nSetHealth * 0.2) >= m_iHealth) && !m_b20Done)
	{
		m_On20PercentHealth.FireOutput(info.GetAttacker(), this);
		m_b20Done = true;
	}
	if (((m_nSetHealth * 0.1) >= m_iHealth) && !m_b10Done)
	{
		m_On10PercentHealth.FireOutput(info.GetAttacker(), this);
		m_b10Done = true;
	}

	if (m_iHealth <= 0 && !m_bDestroyed)
	{
		Destroyed(info);
		m_bDestroyed = true;
	}
	return flDamage;
}

void CFOTeamGenerator::Destroyed(const CTakeDamageInfo &info)
{
	m_OnDestroyed.FireOutput(info.GetAttacker(), this);
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CFOTeamGenerator::DestroyThyselfHeathen(inputdata_t &inputData)
{
	AddEffects(EF_NODRAW);

	SetSolid(SOLID_NONE);
}

