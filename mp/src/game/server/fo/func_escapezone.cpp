//====== Copyright © 1996-2005, Valve Corporation, All rights reserved. =======
//
// Purpose: 
//
//=============================================================================

#include "cbase.h"
#include "func_no_build.h"
#include "tf_team.h"
#include "ndebugoverlay.h"
#include "tf_gamerules.h"
#include "entity_tfstart.h"
#include "modelentities.h"
#include "cbase.h"
#include "triggers.h"

class CFuncEscapeZone : public CBaseTrigger
{
public:
	DECLARE_CLASS(CFuncEscapeZone, CBaseTrigger);
	DECLARE_DATADESC();

	void Spawn();

	void BrushTouch(CBaseEntity *pOther);

private:

	float m_flNextTouch;

	COutputEvent	m_OnCourierTouch;
	COutputEvent	m_OnRedCourierTouch;
	COutputEvent	m_OnBlueCourierTouch;
	COutputEvent	m_OnGreenCourierTouch;
	COutputEvent	m_OnYellowCourierTouch;
	COutputEvent	m_OnPurpleCourierTouch;
	COutputEvent	m_OnPinkCourierTouch;
};

LINK_ENTITY_TO_CLASS(func_escapezone, CFuncEscapeZone);

BEGIN_DATADESC(CFuncEscapeZone)

DEFINE_OUTPUT(m_OnCourierTouch, "OnCourierTouch"),
DEFINE_OUTPUT(m_OnRedCourierTouch, "OnRedCourierTouch"),
DEFINE_OUTPUT(m_OnBlueCourierTouch, "OnBlueCourierTouch"),
DEFINE_OUTPUT(m_OnGreenCourierTouch, "OnGreenCourierTouch"),
DEFINE_OUTPUT(m_OnYellowCourierTouch, "OnYellowCourierTouch"),
DEFINE_OUTPUT(m_OnPurpleCourierTouch, "OnPurpleCourierTouch"),
DEFINE_OUTPUT(m_OnPinkCourierTouch, "OnPinkCourierTouch"),

// Declare this function as being the touch function
DEFINE_ENTITYFUNC(BrushTouch),

END_DATADESC()

//-----------------------------------------------------------------------------
// Purpose: Sets up the entity's initial state
//-----------------------------------------------------------------------------
void CFuncEscapeZone::Spawn()
{
	AddSpawnFlags(SF_TRIGGER_ALLOW_CLIENTS);

	BaseClass::Spawn();
	InitTrigger();

	m_flNextTouch = 0.0;

	SetCollisionGroup(TFCOLLISION_GROUP_RESPAWNROOMS);

	// We want to capture touches from other entities
	SetTouch(&CFuncEscapeZone::BrushTouch);
}

//-----------------------------------------------------------------------------
// Purpose: Move away from an entity that touched us
// Input  : *pOther - the entity we touched
//-----------------------------------------------------------------------------
void CFuncEscapeZone::BrushTouch(CBaseEntity *pOther)
{
	if (PassesTriggerFilters(pOther))
	{
		if (pOther->IsPlayer())
		{
			CTFPlayer *pPlayer = ToTFPlayer(pOther);
			if (pPlayer->GetPlayerClass()->GetClassIndex() == FO_CLASS_COURIER + 1)
			{
				if (gpGlobals->curtime >= m_flNextTouch)
				{
					m_OnCourierTouch.FireOutput(this, this);
					switch (pPlayer->GetTeamNumber())
					{
					case 2:
						m_OnRedCourierTouch.FireOutput(this, this);
						break;
					case 3:
						m_OnBlueCourierTouch.FireOutput(this, this);
						break;
					case 4:
						m_OnGreenCourierTouch.FireOutput(this, this);
						break;
					case 5:
						m_OnYellowCourierTouch.FireOutput(this, this);
						break;
					case 6:
						m_OnPurpleCourierTouch.FireOutput(this, this);
						break;
					case 7:
						m_OnPinkCourierTouch.FireOutput(this, this);
						break;
					}
				}
				m_flNextTouch = gpGlobals->curtime + 0.1f;
			}
		}
	}
}
