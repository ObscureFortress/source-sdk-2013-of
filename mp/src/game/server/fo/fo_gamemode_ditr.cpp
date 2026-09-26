// TURNIP CODE 
// 10/2/2022

#include "cbase.h"
#include "entity_capture_flag.h"

class CFOGamemodeDiamondInTheRough : public CLogicalEntity
{
public:
	DECLARE_CLASS(CFOGamemodeDiamondInTheRough, CLogicalEntity);
	DECLARE_DATADESC();

	// Constructor
	CFOGamemodeDiamondInTheRough()
	{
		m_nLastDiamondHole = 0;
	}

	// Input function
	void InitiateDiamondHide(inputdata_t &inputData);
	void InitiateDiamondExtracted(inputdata_t &inputData);

	void Activate(void);

private:

	int m_nLastDiamondHole;
	int m_nNumberOfHoles; // bruh name

	CHandle<CCaptureFlag>	m_hAssociatedFlag;
	string_t				m_iszAssociatedFlag;

	CHandle<CBaseCombatCharacter>	m_hDiamondHole1;
	string_t				m_iszDiamondHole1;
	CHandle<CBaseCombatCharacter>	m_hDiamondHole2;
	string_t				m_iszDiamondHole2;
	CHandle<CBaseCombatCharacter>	m_hDiamondHole3;
	string_t				m_iszDiamondHole3;
	CHandle<CBaseCombatCharacter>	m_hDiamondHole4;
	string_t				m_iszDiamondHole4;
	CHandle<CBaseCombatCharacter>	m_hDiamondHole5;
	string_t				m_iszDiamondHole5;
	CHandle<CBaseCombatCharacter>	m_hDiamondHole6;
	string_t				m_iszDiamondHole6;
	CHandle<CBaseCombatCharacter>	m_hDiamondHole7;
	string_t				m_iszDiamondHole7;
	CHandle<CBaseCombatCharacter>	m_hDiamondHole8;
	string_t				m_iszDiamondHole8;
	CHandle<CBaseCombatCharacter>	m_hDiamondHole9;
	string_t				m_iszDiamondHole9;
	CHandle<CBaseCombatCharacter>	m_hDiamondHole10;
	string_t				m_iszDiamondHole10;

	COutputEvent	m_DiamondExtracted;
	COutputEvent	m_DiamondHide;

	COutputEvent	m_Hole1Chosen;
	COutputEvent	m_Hole2Chosen;
	COutputEvent	m_Hole3Chosen;
	COutputEvent	m_Hole4Chosen;
	COutputEvent	m_Hole5Chosen;
	COutputEvent	m_Hole6Chosen;
	COutputEvent	m_Hole7Chosen;
	COutputEvent	m_Hole8Chosen;
	COutputEvent	m_Hole9Chosen;
	COutputEvent	m_Hole10Chosen;
};

LINK_ENTITY_TO_CLASS(fo_gamemode_ditr, CFOGamemodeDiamondInTheRough);


BEGIN_DATADESC(CFOGamemodeDiamondInTheRough)

	DEFINE_INPUTFUNC(FIELD_VOID, "InitiateDiamondHide", InitiateDiamondHide),
	DEFINE_INPUTFUNC(FIELD_VOID, "InitiateDiamondExtracted", InitiateDiamondExtracted),

	DEFINE_KEYFIELD(m_nNumberOfHoles, FIELD_INTEGER, "HoleCount"),

	DEFINE_FIELD(m_hAssociatedFlag, FIELD_EHANDLE),
	DEFINE_KEYFIELD(m_iszAssociatedFlag, FIELD_STRING, "AssociatedFlag"),

	// this is stupid, but i dont know what else to do
	DEFINE_FIELD(m_hDiamondHole1, FIELD_EHANDLE),
	DEFINE_KEYFIELD(m_iszDiamondHole1, FIELD_STRING, "DiamondHole1"),
	DEFINE_FIELD(m_hDiamondHole2, FIELD_EHANDLE),
	DEFINE_KEYFIELD(m_iszDiamondHole2, FIELD_STRING, "DiamondHole2"),
	DEFINE_FIELD(m_hDiamondHole3, FIELD_EHANDLE),
	DEFINE_KEYFIELD(m_iszDiamondHole3, FIELD_STRING, "DiamondHole3"),
	DEFINE_FIELD(m_hDiamondHole4, FIELD_EHANDLE),
	DEFINE_KEYFIELD(m_iszDiamondHole4, FIELD_STRING, "DiamondHole4"),
	DEFINE_FIELD(m_hDiamondHole5, FIELD_EHANDLE),
	DEFINE_KEYFIELD(m_iszDiamondHole5, FIELD_STRING, "DiamondHole5"),
	DEFINE_FIELD(m_hDiamondHole6, FIELD_EHANDLE),
	DEFINE_KEYFIELD(m_iszDiamondHole6, FIELD_STRING, "DiamondHole6"),
	DEFINE_FIELD(m_hDiamondHole7, FIELD_EHANDLE),
	DEFINE_KEYFIELD(m_iszDiamondHole7, FIELD_STRING, "DiamondHole7"),
	DEFINE_FIELD(m_hDiamondHole8, FIELD_EHANDLE),
	DEFINE_KEYFIELD(m_iszDiamondHole8, FIELD_STRING, "DiamondHole8"),
	DEFINE_FIELD(m_hDiamondHole9, FIELD_EHANDLE),
	DEFINE_KEYFIELD(m_iszDiamondHole9, FIELD_STRING, "DiamondHole9"),
	DEFINE_FIELD(m_hDiamondHole10, FIELD_EHANDLE),
	DEFINE_KEYFIELD(m_iszDiamondHole10, FIELD_STRING, "DiamondHole10"),

	DEFINE_OUTPUT(m_DiamondExtracted, "DiamondExtracted"),
	DEFINE_OUTPUT(m_DiamondHide, "DiamondHide"),

	DEFINE_OUTPUT(m_Hole1Chosen, "Hole1Chosen"),
	DEFINE_OUTPUT(m_Hole2Chosen, "Hole2Chosen"),
	DEFINE_OUTPUT(m_Hole3Chosen, "Hole3Chosen"),
	DEFINE_OUTPUT(m_Hole4Chosen, "Hole4Chosen"),
	DEFINE_OUTPUT(m_Hole5Chosen, "Hole5Chosen"),
	DEFINE_OUTPUT(m_Hole6Chosen, "Hole6Chosen"),
	DEFINE_OUTPUT(m_Hole7Chosen, "Hole7Chosen"),
	DEFINE_OUTPUT(m_Hole8Chosen, "Hole8Chosen"),
	DEFINE_OUTPUT(m_Hole9Chosen, "Hole9Chosen"),
	DEFINE_OUTPUT(m_Hole10Chosen, "Hole10Chosen"),

END_DATADESC()

//-----------------------------------------------------------------------------
// Purpose: Precache function for the entity
//-----------------------------------------------------------------------------
void CFOGamemodeDiamondInTheRough::Activate(void)
{
	BaseClass::Activate();

	if (m_nNumberOfHoles > 10)
	{
		m_nNumberOfHoles = 1; // you arent very funny, funny man
	}

	if (m_nNumberOfHoles <= 0)
	{
		m_nNumberOfHoles = 1;
	}

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

	if (m_iszDiamondHole1 != NULL_STRING)
	{
		CBaseEntity *pEnt = gEntList.FindEntityByName(NULL, STRING(m_iszDiamondHole1));
		if (!pEnt)
		{
			Warning("%s(%s) unable to find associated entity named '%s'.\n", GetClassname(), GetDebugName(), STRING(m_iszDiamondHole1));
		}
		else
		{
			m_hDiamondHole1 = dynamic_cast<CBaseCombatCharacter*>(pEnt);
			if (!m_hDiamondHole1)
			{
				Warning("%s(%s) tried to use associated entity named '%s', but it isn't a fo_diamond_hole entity. \n", GetClassname(), GetDebugName(), STRING(m_iszDiamondHole1));
			}
		}
	}
	else
	{
		Warning("%s(%s) has no associated hole 1.\n", GetClassname(), GetDebugName());
	}
	if (m_nNumberOfHoles >= 2)
	{
		if (m_iszDiamondHole2 != NULL_STRING)
		{
			CBaseEntity *pEnt = gEntList.FindEntityByName(NULL, STRING(m_iszDiamondHole2));
			if (!pEnt)
			{
				Warning("%s(%s) unable to find associated entity named '%s'.\n", GetClassname(), GetDebugName(), STRING(m_iszDiamondHole2));
			}
			else
			{
				m_hDiamondHole2 = dynamic_cast<CBaseCombatCharacter*>(pEnt);
				if (!m_hDiamondHole2)
				{
					Warning("%s(%s) tried to use associated entity named '%s', but it isn't a fo_diamond_hole entity. \n", GetClassname(), GetDebugName(), STRING(m_iszDiamondHole2));
				}
			}
		}
		else
		{
			Warning("%s(%s) has no associated hole 2.\n", GetClassname(), GetDebugName());
		}
	}
	if (m_nNumberOfHoles >= 3)
	{
		if (m_iszDiamondHole3 != NULL_STRING)
		{
			CBaseEntity *pEnt = gEntList.FindEntityByName(NULL, STRING(m_iszDiamondHole3));
			if (!pEnt)
			{
				Warning("%s(%s) unable to find associated entity named '%s'.\n", GetClassname(), GetDebugName(), STRING(m_iszDiamondHole3));
			}
			else
			{
				m_hDiamondHole3 = dynamic_cast<CBaseCombatCharacter*>(pEnt);
				if (!m_hDiamondHole3)
				{
					Warning("%s(%s) tried to use associated entity named '%s', but it isn't a fo_diamond_hole entity. \n", GetClassname(), GetDebugName(), STRING(m_iszDiamondHole3));
				}
			}
		}
		else
		{
			Warning("%s(%s) has no associated hole 3.\n", GetClassname(), GetDebugName());
		}
	}
	if (m_nNumberOfHoles >= 4)
	{
		if (m_iszDiamondHole4 != NULL_STRING)
		{
			CBaseEntity *pEnt = gEntList.FindEntityByName(NULL, STRING(m_iszDiamondHole4));
			if (!pEnt)
			{
				Warning("%s(%s) unable to find associated entity named '%s'.\n", GetClassname(), GetDebugName(), STRING(m_iszDiamondHole4));
			}
			else
			{
				m_hDiamondHole4 = dynamic_cast<CBaseCombatCharacter*>(pEnt);
				if (!m_hDiamondHole4)
				{
					Warning("%s(%s) tried to use associated entity named '%s', but it isn't a fo_diamond_hole entity. \n", GetClassname(), GetDebugName(), STRING(m_iszDiamondHole4));
				}
			}
		}
		else
		{
			Warning("%s(%s) has no associated hole 4.\n", GetClassname(), GetDebugName());
		}
	}
	if (m_nNumberOfHoles >= 5)
	{
		if (m_iszDiamondHole5 != NULL_STRING)
		{
			CBaseEntity *pEnt = gEntList.FindEntityByName(NULL, STRING(m_iszDiamondHole5));
			if (!pEnt)
			{
				Warning("%s(%s) unable to find associated entity named '%s'.\n", GetClassname(), GetDebugName(), STRING(m_iszDiamondHole5));
			}
			else
			{
				m_hDiamondHole5 = dynamic_cast<CBaseCombatCharacter*>(pEnt);
				if (!m_hDiamondHole5)
				{
					Warning("%s(%s) tried to use associated entity named '%s', but it isn't a fo_diamond_hole entity. \n", GetClassname(), GetDebugName(), STRING(m_iszDiamondHole5));
				}
			}
		}
		else
		{
			Warning("%s(%s) has no associated hole 5.\n", GetClassname(), GetDebugName());
		}
	}
	if (m_nNumberOfHoles >= 6)
	{
		if (m_iszDiamondHole6 != NULL_STRING)
		{
			CBaseEntity *pEnt = gEntList.FindEntityByName(NULL, STRING(m_iszDiamondHole6));
			if (!pEnt)
			{
				Warning("%s(%s) unable to find associated entity named '%s'.\n", GetClassname(), GetDebugName(), STRING(m_iszDiamondHole6));
			}
			else
			{
				m_hDiamondHole6 = dynamic_cast<CBaseCombatCharacter*>(pEnt);
				if (!m_hDiamondHole6)
				{
					Warning("%s(%s) tried to use associated entity named '%s', but it isn't a fo_diamond_hole entity. \n", GetClassname(), GetDebugName(), STRING(m_iszDiamondHole6));
				}
			}
		}
		else
		{
			Warning("%s(%s) has no associated hole 6.\n", GetClassname(), GetDebugName());
		}
	}
	if (m_nNumberOfHoles >= 7)
	{
		if (m_iszDiamondHole7 != NULL_STRING)
		{
			CBaseEntity *pEnt = gEntList.FindEntityByName(NULL, STRING(m_iszDiamondHole7));
			if (!pEnt)
			{
				Warning("%s(%s) unable to find associated entity named '%s'.\n", GetClassname(), GetDebugName(), STRING(m_iszDiamondHole7));
			}
			else
			{
				m_hDiamondHole7 = dynamic_cast<CBaseCombatCharacter*>(pEnt);
				if (!m_hDiamondHole7)
				{
					Warning("%s(%s) tried to use associated entity named '%s', but it isn't a fo_diamond_hole entity. \n", GetClassname(), GetDebugName(), STRING(m_iszDiamondHole7));
				}
			}
		}
		else
		{
			Warning("%s(%s) has no associated hole 7.\n", GetClassname(), GetDebugName());
		}
	}
	if (m_nNumberOfHoles >= 8)
	{
		if (m_iszDiamondHole8 != NULL_STRING)
		{
			CBaseEntity *pEnt = gEntList.FindEntityByName(NULL, STRING(m_iszDiamondHole8));
			if (!pEnt)
			{
				Warning("%s(%s) unable to find associated entity named '%s'.\n", GetClassname(), GetDebugName(), STRING(m_iszDiamondHole8));
			}
			else
			{
				m_hDiamondHole8 = dynamic_cast<CBaseCombatCharacter*>(pEnt);
				if (!m_hDiamondHole8)
				{
					Warning("%s(%s) tried to use associated entity named '%s', but it isn't a fo_diamond_hole entity. \n", GetClassname(), GetDebugName(), STRING(m_iszDiamondHole8));
				}
			}
		}
		else
		{
			Warning("%s(%s) has no associated hole 8.\n", GetClassname(), GetDebugName());
		}
	}
	if (m_nNumberOfHoles >= 9)
	{
		if (m_iszDiamondHole9 != NULL_STRING)
		{
			CBaseEntity *pEnt = gEntList.FindEntityByName(NULL, STRING(m_iszDiamondHole9));
			if (!pEnt)
			{
				Warning("%s(%s) unable to find associated entity named '%s'.\n", GetClassname(), GetDebugName(), STRING(m_iszDiamondHole9));
			}
			else
			{
				m_hDiamondHole9 = dynamic_cast<CBaseCombatCharacter*>(pEnt);
				if (!m_hDiamondHole9)
				{
					Warning("%s(%s) tried to use associated entity named '%s', but it isn't a fo_diamond_hole entity. \n", GetClassname(), GetDebugName(), STRING(m_iszDiamondHole9));
				}
			}
		}
		else
		{
			Warning("%s(%s) has no associated hole 9.\n", GetClassname(), GetDebugName());
		}
	}
	if (m_nNumberOfHoles == 10)
	{
		if (m_iszDiamondHole10 != NULL_STRING)
		{
			CBaseEntity *pEnt = gEntList.FindEntityByName(NULL, STRING(m_iszDiamondHole10));
			if (!pEnt)
			{
				Warning("%s(%s) unable to find associated entity named '%s'.\n", GetClassname(), GetDebugName(), STRING(m_iszDiamondHole10));
			}
			else
			{
				m_hDiamondHole10 = dynamic_cast<CBaseCombatCharacter*>(pEnt);
				if (!m_hDiamondHole10)
				{
					Warning("%s(%s) tried to use associated entity named '%s', but it isn't a fo_diamond_hole entity. \n", GetClassname(), GetDebugName(), STRING(m_iszDiamondHole10));
				}
			}
		}
		else
		{
			Warning("%s(%s) has no associated hole 10.\n", GetClassname(), GetDebugName());
		}
	}
}

void CFOGamemodeDiamondInTheRough::InitiateDiamondHide(inputdata_t &inputData)
{
	m_hAssociatedFlag->SetDisabled(true);

	int m_nHoleChosen = RandomInt(1, m_nNumberOfHoles);

	while (m_nHoleChosen == m_nLastDiamondHole) // dont choose the same hole to hide the diamond
	{
		m_nHoleChosen = RandomInt(1, m_nNumberOfHoles);
	}
	
	m_nLastDiamondHole = m_nHoleChosen;

	//Msg("%d", m_nHoleChosen);

	Vector vecOffset = { 0, 0, 30 };

	switch (m_nHoleChosen)
	{
	case 1:
		if (!m_hDiamondHole1)
			break;
		m_hAssociatedFlag->SetAbsOrigin(m_hDiamondHole1->GetAbsOrigin() + vecOffset);
		m_Hole1Chosen.FireOutput(this, this);
		break;
	case 2:
		if (!m_hDiamondHole2)
			break;
		m_hAssociatedFlag->SetAbsOrigin(m_hDiamondHole2->GetAbsOrigin() + vecOffset);
		m_Hole2Chosen.FireOutput(this, this);
		break;
	case 3:
		if (!m_hDiamondHole3)
			break;
		m_hAssociatedFlag->SetAbsOrigin(m_hDiamondHole3->GetAbsOrigin() + vecOffset);
		m_Hole3Chosen.FireOutput(this, this);
		break;
	case 4:
		if (!m_hDiamondHole4)
			break;
		m_hAssociatedFlag->SetAbsOrigin(m_hDiamondHole4->GetAbsOrigin() + vecOffset);
		m_Hole4Chosen.FireOutput(this, this);
		break;
	case 5:
		if (!m_hDiamondHole5)
			break;
		m_hAssociatedFlag->SetAbsOrigin(m_hDiamondHole5->GetAbsOrigin() + vecOffset);
		m_Hole5Chosen.FireOutput(this, this);
		break;
	case 6:
		if (!m_hDiamondHole6)
			break;
		m_hAssociatedFlag->SetAbsOrigin(m_hDiamondHole6->GetAbsOrigin() + vecOffset);
		m_Hole6Chosen.FireOutput(this, this);
		break;
	case 7:
		if (!m_hDiamondHole7)
			break;
		m_hAssociatedFlag->SetAbsOrigin(m_hDiamondHole7->GetAbsOrigin() + vecOffset);
		m_Hole7Chosen.FireOutput(this, this);
		break;
	case 8:
		if (!m_hDiamondHole8)
			break;
		m_hAssociatedFlag->SetAbsOrigin(m_hDiamondHole8->GetAbsOrigin() + vecOffset);
		m_Hole8Chosen.FireOutput(this, this);
		break;
	case 9:
		if (!m_hDiamondHole9)
			break;
		m_hAssociatedFlag->SetAbsOrigin(m_hDiamondHole9->GetAbsOrigin() + vecOffset);
		m_Hole9Chosen.FireOutput(this, this);
		break;
	case 10:
		if (!m_hDiamondHole10)
			break;
		m_hAssociatedFlag->SetAbsOrigin(m_hDiamondHole10->GetAbsOrigin() + vecOffset);
		m_Hole10Chosen.FireOutput(this, this);
		break;
	}

	m_DiamondHide.FireOutput(this, this);
}

void CFOGamemodeDiamondInTheRough::InitiateDiamondExtracted(inputdata_t &inputData)
{
	m_hAssociatedFlag->SetDisabled(false);
	m_DiamondExtracted.FireOutput(this, this);
}