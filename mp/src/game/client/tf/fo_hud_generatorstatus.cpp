//========= Copyright © 1996-2007, Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
//=============================================================================//

#include "cbase.h"
#include "hudelement.h"
#include "iclientmode.h"
#include <KeyValues.h>
#include <vgui/IScheme.h>
#include <vgui/ISurface.h>
#include <vgui/ISystem.h>
#include <vgui_controls/AnimationController.h>
#include <vgui_controls/EditablePanel.h>
#include <vgui_controls/ImagePanel.h>
#include <vgui/IVGui.h>
#include <vgui/ISurface.h>
#include <vgui/IImage.h>
#include <vgui_controls/Label.h>

#include "c_playerresource.h"
#include "teamplay_round_timer.h"
#include "utlvector.h"
#include "c_tf_player.h"
#include "c_team.h"
#include "c_tf_team.h"
#include "c_team_objectiveresource.h"
#include "tf_hud_objectivestatus.h"
#include "tf_spectatorgui.h"
#include "teamplayroundbased_gamerules.h"
#include "tf_gamerules.h"
#include "tf_hud_freezepanel.h"
#include "fo_hud_generatorstatus.h"

using namespace vgui;

//DECLARE_BUILD_FACTORY( CFOGeneratorStatus );

extern ConVar fo_gd_red_health;
extern ConVar fo_gd_blue_health;
extern ConVar fo_gd_green_health;
extern ConVar fo_gd_yellow_health;
extern ConVar fo_gd_purple_health;
extern ConVar fo_gd_pink_health;

extern ConVar fo_gd_point_timer;

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
CFOHudGeneratorObjectives::CFOHudGeneratorObjectives( Panel *parent, const char *name ) : EditablePanel( parent, name )
{
	m_pPointTimer = NULL;
	m_pPointTimerShadow = NULL;

	vgui::ivgui()->AddTickSignal( GetVPanel() );

	ListenForGameEvent( "flagstatus_update" );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool CFOHudGeneratorObjectives::IsVisible( void )
{
	if( IsTakingAFreezecamScreenshot() )
		return false;

	return BaseClass::IsVisible();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CFOHudGeneratorObjectives::ApplySchemeSettings( IScheme *pScheme )
{
	BaseClass::ApplySchemeSettings( pScheme );

	// load control settings...
	LoadControlSettings( "resource/UI/HudObjectiveGeneratorPanel.res" );

	m_pPointTimer = dynamic_cast<CTFLabel *>(FindChildByName("PointTimer"));
	m_pPointTimerShadow = dynamic_cast<CTFLabel *>(FindChildByName("PointTimerShadow"));
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CFOHudGeneratorObjectives::Reset()
{
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CFOHudGeneratorObjectives::OnTick()
{
	SetDialogVariable("pointtimer", fo_gd_point_timer.GetInt());

	if (m_pPointTimer && m_pPointTimerShadow)
	{
		if (fo_gd_point_timer.GetInt() == 61 || fo_gd_point_timer.GetInt() == -1)
		{
			m_pPointTimer->SetVisible(false);
			m_pPointTimerShadow->SetVisible(false);
		}
		else
		{
			m_pPointTimer->SetVisible(true);
			m_pPointTimerShadow->SetVisible(true);
		}
	}

	SetDialogVariable("redgenhealth", fo_gd_red_health.GetInt());
	SetDialogVariable("bluegenhealth", fo_gd_blue_health.GetInt());
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CFOHudGeneratorObjectives::FireGameEvent(IGameEvent *event)
{
	const char *eventName = event->GetName();

	if (!Q_strcmp(eventName, "flagstatus_update"))
	{
		//UpdateStatus();
	}
}

// 3TEAM BELOW

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
CFOHud3GeneratorObjectives::CFOHud3GeneratorObjectives(Panel *parent, const char *name) : EditablePanel(parent, name)
{
	m_pPointTimer = NULL;
	m_pPointTimerShadow = NULL;

	vgui::ivgui()->AddTickSignal(GetVPanel());

	ListenForGameEvent( "flagstatus_update" );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool CFOHud3GeneratorObjectives::IsVisible(void)
{
	if (IsTakingAFreezecamScreenshot())
		return false;

	return BaseClass::IsVisible();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CFOHud3GeneratorObjectives::ApplySchemeSettings(IScheme *pScheme)
{
	BaseClass::ApplySchemeSettings(pScheme);

	// load control settings...
	LoadControlSettings("resource/UI/HudObjective3GeneratorPanel.res");

	m_pPointTimer = dynamic_cast<CTFLabel *>(FindChildByName("PointTimer"));
	m_pPointTimerShadow = dynamic_cast<CTFLabel *>(FindChildByName("PointTimerShadow"));
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CFOHud3GeneratorObjectives::Reset()
{
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CFOHud3GeneratorObjectives::OnTick()
{
	SetDialogVariable("pointtimer", fo_gd_point_timer.GetInt());

	if (m_pPointTimer && m_pPointTimerShadow)
	{
		if (fo_gd_point_timer.GetInt() == 61 || fo_gd_point_timer.GetInt() == -1)
		{
			m_pPointTimer->SetVisible(false);
			m_pPointTimerShadow->SetVisible(false);
		}
		else
		{
			m_pPointTimer->SetVisible(true);
			m_pPointTimerShadow->SetVisible(true);
		}
	}

	SetDialogVariable("redgenhealth", fo_gd_red_health.GetInt());
	SetDialogVariable("bluegenhealth", fo_gd_blue_health.GetInt());
	SetDialogVariable("greengenhealth", fo_gd_green_health.GetInt());
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CFOHud3GeneratorObjectives::FireGameEvent(IGameEvent *event)
{
	const char *eventName = event->GetName();

	if (!Q_strcmp(eventName, "flagstatus_update"))
	{
		//UpdateStatus();
	}
}

// 6TEAM BELOW

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
CFOHud6GeneratorObjectives::CFOHud6GeneratorObjectives(Panel *parent, const char *name) : EditablePanel(parent, name)
{
	m_pPointTimer = NULL;
	m_pPointTimerShadow = NULL;

	vgui::ivgui()->AddTickSignal(GetVPanel());

	ListenForGameEvent( "flagstatus_update" );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool CFOHud6GeneratorObjectives::IsVisible(void)
{
	if (IsTakingAFreezecamScreenshot())
		return false;

	return BaseClass::IsVisible();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CFOHud6GeneratorObjectives::ApplySchemeSettings(IScheme *pScheme)
{
	BaseClass::ApplySchemeSettings(pScheme);

	// load control settings...
	LoadControlSettings("resource/UI/HudObjective6GeneratorPanel.res");

	m_pPointTimer = dynamic_cast<CTFLabel *>(FindChildByName("PointTimer"));
	m_pPointTimerShadow = dynamic_cast<CTFLabel *>(FindChildByName("PointTimerShadow"));
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CFOHud6GeneratorObjectives::Reset()
{
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CFOHud6GeneratorObjectives::OnTick()
{
	SetDialogVariable("pointtimer", fo_gd_point_timer.GetInt());

	if (m_pPointTimer && m_pPointTimerShadow)
	{
		if (fo_gd_point_timer.GetInt() == 61 || fo_gd_point_timer.GetInt() == -1)
		{
			m_pPointTimer->SetVisible(false);
			m_pPointTimerShadow->SetVisible(false);
		}
		else
		{
			m_pPointTimer->SetVisible(true);
			m_pPointTimerShadow->SetVisible(true);
		}
	}

	SetDialogVariable("redgenhealth", fo_gd_red_health.GetInt());
	SetDialogVariable("bluegenhealth", fo_gd_blue_health.GetInt());
	SetDialogVariable("greengenhealth", fo_gd_green_health.GetInt());
	SetDialogVariable("yellowgenhealth", fo_gd_yellow_health.GetInt());
	SetDialogVariable("purplegenhealth", fo_gd_purple_health.GetInt());
	SetDialogVariable("pinkgenhealth", fo_gd_pink_health.GetInt());
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CFOHud6GeneratorObjectives::FireGameEvent(IGameEvent *event)
{
	const char *eventName = event->GetName();

	if (!Q_strcmp(eventName, "flagstatus_update"))
	{
		//UpdateStatus();
	}
}
