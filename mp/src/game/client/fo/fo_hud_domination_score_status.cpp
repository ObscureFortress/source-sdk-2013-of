//========= Copyright © 1996-2006, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#include "cbase.h"
#include "hud.h"
#include "hudelement.h"
#include "hud_macros.h"
#include "hud_numericdisplay.h"
#include <KeyValues.h>
#include <vgui/IScheme.h>
#include <vgui/ISurface.h>
#include <vgui/ISystem.h>
#include <vgui_controls/AnimationController.h>
#include "iclientmode.h"
#include "tf_shareddefs.h"
#include <vgui_controls/EditablePanel.h>
#include <vgui_controls/ImagePanel.h>
#include <vgui/ISurface.h>
#include <vgui/IImage.h>
#include <vgui_controls/Label.h>

#include "tf_controls.h"
#include "in_buttons.h"
#include "tf_imagepanel.h"
#include "c_team.h"
#include "c_tf_player.h"
#include "ihudlcd.h"
#include "fo_hud_domination_score_status.h"
#include "tf_gamerules.h"

using namespace vgui;

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"


DECLARE_HUDELEMENT(CFOHudDominationScore);

extern ConVar fo_dom_red_score;
extern ConVar fo_dom_blue_score;
extern ConVar fo_dom_green_score;
extern ConVar fo_dom_yellow_score;
extern ConVar fo_dom_purple_score;
extern ConVar fo_dom_pink_score;
extern ConVar fo_dom_scorelimit;
extern ConVar fo_dom_timerinhud;

extern ConVar fo_extrateammode;

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CFOHudDominationScore::CFOHudDominationScore(const char *pElementName) : CHudElement(pElementName), BaseClass(NULL, "HudDominationScore")
{
	Panel *pParent = g_pClientMode->GetViewport();
	SetParent(pParent);

	m_pRedScore = NULL;
	m_pRedScoreShadow = NULL;
	m_pRedBG = NULL;

	m_pBlueScore = NULL;
	m_pBlueScoreShadow = NULL;
	m_pBlueBG = NULL;

	m_pGreenScore = NULL;
	m_pGreenScoreShadow = NULL;
	m_pGreenBG = NULL;

	m_pYellowScore = NULL;
	m_pYellowScoreShadow = NULL;
	m_pYellowBG = NULL;

	m_pPurpleScore = NULL;
	m_pPurpleScoreShadow = NULL;
	m_pPurpleBG = NULL;

	m_pPinkScore = NULL;
	m_pPinkScoreShadow = NULL;
	m_pPinkBG = NULL;

	m_flNextThink = 0.0f;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CFOHudDominationScore::LevelInit()
{
	SetVisible(false);
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CFOHudDominationScore::Init()
{
	// listen for events
	ListenForGameEvent("teamplay_waiting_ends");
	ListenForGameEvent("teamplay_waiting_begins");
	ListenForGameEvent("teamplay_round_active");

	SetVisible(false);

	CHudElement::Init();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CFOHudDominationScore::Reset()
{
	m_flNextThink = gpGlobals->curtime + 0.05f;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CFOHudDominationScore::ApplySchemeSettings(IScheme *pScheme)
{
	BaseClass::ApplySchemeSettings(pScheme);

	// load control settings...
	LoadControlSettings("resource/UI/HudDominationScore.res");

	m_pRedScore = dynamic_cast<CTFLabel *>(FindChildByName("RedScore"));
	m_pRedScoreShadow = dynamic_cast<CTFLabel *>(FindChildByName("RedScoreShadow"));
	m_pRedBG = dynamic_cast<CTFImagePanel *>(FindChildByName("RedScoreBG"));

	m_pBlueScore = dynamic_cast<CTFLabel *>(FindChildByName("BlueScore"));
	m_pBlueScoreShadow = dynamic_cast<CTFLabel *>(FindChildByName("BlueScoreShadow"));
	m_pBlueBG = dynamic_cast<CTFImagePanel *>(FindChildByName("BlueScoreBG"));

	m_pGreenScore = dynamic_cast<CTFLabel *>(FindChildByName("GreenScore"));
	m_pGreenScoreShadow = dynamic_cast<CTFLabel *>(FindChildByName("GreenScoreShadow"));
	m_pGreenBG = dynamic_cast<CTFImagePanel *>(FindChildByName("GreenScoreBG"));

	m_pYellowScore = dynamic_cast<CTFLabel *>(FindChildByName("YellowScore"));
	m_pYellowScoreShadow = dynamic_cast<CTFLabel *>(FindChildByName("YellowScoreShadow"));
	m_pYellowBG = dynamic_cast<CTFImagePanel *>(FindChildByName("YellowScoreBG"));

	m_pPurpleScore = dynamic_cast<CTFLabel *>(FindChildByName("PurpleScore"));
	m_pPurpleScoreShadow = dynamic_cast<CTFLabel *>(FindChildByName("PurpleScoreShadow"));
	m_pPurpleBG = dynamic_cast<CTFImagePanel *>(FindChildByName("PurpleScoreBG"));

	m_pPinkScore = dynamic_cast<CTFLabel *>(FindChildByName("PinkScore"));
	m_pPinkScoreShadow = dynamic_cast<CTFLabel *>(FindChildByName("PinkScoreShadow"));
	m_pPinkBG = dynamic_cast<CTFImagePanel *>(FindChildByName("PinkScoreBG"));

	m_pPlayingTo = dynamic_cast<CTFLabel *>(FindChildByName("PlayingTo"));
	m_pPlayingToBG = dynamic_cast<CTFImagePanel *>(FindChildByName("PlayingToBG"));

	m_flNextThink = 0.0f;

	UpdateTeamPanels(true, true, true, true, true, true, 0);
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CFOHudDominationScore::FireGameEvent(IGameEvent * event)
{
	const char *pEventName = event->GetName();

	if (Q_strcmp("teamplay_waiting_ends", pEventName) == 0)
	{
		m_bReadyToShift = true;
		UpdateTeamPanels(true, true, true, true, true, true, 0);
	}
	else if (Q_strcmp("teamplay_waiting_begins", pEventName) == 0)
	{
		m_bReadyToShift = true;
		UpdateTeamPanels(true, true, true, true, true, true, 1);
	}
	else if (Q_strcmp("teamplay_round_active", pEventName) == 0)
	{
		m_bReadyToShift = true;
		UpdateTeamPanels(true, true, true, true, true, true, 0);
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool CFOHudDominationScore::ShouldDraw(void)
{
	return CHudElement::ShouldDraw();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CFOHudDominationScore::UpdateTeamPanels(bool bRed, bool bBlue, bool bGreen, bool bYellow, bool bPurple, bool bPink, int nPosition)
{
	if (m_pRedScore && m_pRedScoreShadow)
	{
		if (m_pRedScore->IsVisible() != bRed)
		{
			m_pRedScore->SetVisible(bRed);
			m_pRedScoreShadow->SetVisible(bRed);
		}
	}
	if (m_pRedBG)
	{
		if (m_pRedBG->IsVisible() != bRed)
		{
			m_pRedBG->SetVisible(bRed);
		}
	}

	if (m_pBlueScore && m_pBlueScoreShadow)
	{
		if (m_pBlueScore->IsVisible() != bBlue)
		{
			m_pBlueScore->SetVisible(bBlue);
			m_pBlueScoreShadow->SetVisible(bBlue);
		}
	}
	if (m_pBlueBG)
	{
		if (m_pBlueBG->IsVisible() != bBlue)
		{
			m_pBlueBG->SetVisible(bBlue);
		}
	}

	if (m_pGreenScore && m_pGreenScoreShadow)
	{
		if (m_pGreenScore->IsVisible() != bGreen)
		{
			m_pGreenScore->SetVisible(bGreen);
			m_pGreenScoreShadow->SetVisible(bGreen);
		}
	}
	if (m_pGreenBG)
	{
		if (m_pGreenBG->IsVisible() != bGreen)
		{
			m_pGreenBG->SetVisible(bGreen);
		}
	}

	if (m_pYellowScore && m_pYellowScoreShadow)
	{
		if (m_pYellowScore->IsVisible() != bYellow)
		{
			m_pYellowScore->SetVisible(bYellow);
			m_pYellowScoreShadow->SetVisible(bYellow);
		}
	}
	if (m_pYellowBG)
	{
		if (m_pYellowBG->IsVisible() != bYellow)
		{
			m_pYellowBG->SetVisible(bYellow);
		}
	}

	if (m_pPurpleScore && m_pPurpleScoreShadow)
	{
		if (m_pPurpleScore->IsVisible() != bPurple)
		{
			m_pPurpleScore->SetVisible(bPurple);
			m_pPurpleScoreShadow->SetVisible(bPurple);
		}
	}
	if (m_pPurpleBG)
	{
		if (m_pPurpleBG->IsVisible() != bPurple)
		{
			m_pPurpleBG->SetVisible(bPurple);
		}
	}

	if (m_pPinkScore && m_pPinkScoreShadow)
	{
		if (m_pPinkScore->IsVisible() != bPink)
		{
			m_pPinkScore->SetVisible(bPink);
			m_pPinkScoreShadow->SetVisible(bPink);
		}
	}
	if (m_pPinkBG)
	{
		if (m_pPinkBG->IsVisible() != bPink)
		{
			m_pPinkBG->SetVisible(bPink);
		}
	}
	if (nPosition == 0)
	{
		SetPos(0, YRES(-40));
	}
	else
	{
		SetPos(0, 0);
	}
}

//-----------------------------------------------------------------------------
// Purpose: Get ammo info from the weapon and update the displays.
//-----------------------------------------------------------------------------
void CFOHudDominationScore::OnThink()
{
	if (TFGameRules() != nullptr)
	{
		if (TFGameRules()->GetGameType() == FO_GAMETYPE_DOM || TFGameRules()->GetGameType() == FO_GAMETYPE_FW)
		{
			if (fo_extrateammode.GetInt() == 1)
			{
				if (fo_dom_timerinhud.GetInt())
				{
					UpdateTeamPanels(true, true, true, false, false, false, 1);
				}
				else
				{
					if (m_bReadyToShift)
					{
						UpdateTeamPanels(true, true, true, false, false, false, 0);
					}
					else
					{
						UpdateTeamPanels(true, true, true, false, false, false, 1);
					}
				}
				m_pPlayingTo->SetPos(m_pPlayingTo->GetXPos(), YRES(90));
				m_pPlayingToBG->SetPos(m_pPlayingTo->GetXPos(), YRES(86));
			}
			else if (fo_extrateammode.GetInt() == 2)
			{
				if (fo_dom_timerinhud.GetInt())
				{
					UpdateTeamPanels(true, true, true, true, true, true, 1);
				}
				else
				{
					if (m_bReadyToShift)
					{
						UpdateTeamPanels(true, true, true, true, true, true, 0);
					}
					else
					{
						UpdateTeamPanels(true, true, true, true, true, true, 1);
					}
				}
				m_pPlayingTo->SetPos(m_pPlayingTo->GetXPos(), YRES(130));
				m_pPlayingToBG->SetPos(m_pPlayingTo->GetXPos(), YRES(126));
			}
			else
			{
				UpdateTeamPanels(true, true, false, false, false, false, 0);
				m_pPlayingTo->SetPos(m_pPlayingTo->GetXPos(), YRES(90));
				m_pPlayingToBG->SetPos(m_pPlayingTo->GetXPos(), YRES(86));
			}
			m_pPlayingTo->SetVisible(true);
			m_pPlayingToBG->SetVisible(true);
		}
		else
		{
			UpdateTeamPanels(false, false, false, false, false, false, 0); // die idiot
			m_pPlayingTo->SetVisible(false);
			m_pPlayingToBG->SetVisible(false);
		}
	}

	if (m_flNextThink < gpGlobals->curtime)
	{
		SetDialogVariable("redscorenum", fo_dom_red_score.GetInt());
		SetDialogVariable("bluescorenum", fo_dom_blue_score.GetInt());
		SetDialogVariable("greenscorenum", fo_dom_green_score.GetInt());
		SetDialogVariable("yellowscorenum", fo_dom_yellow_score.GetInt());
		SetDialogVariable("purplescorenum", fo_dom_purple_score.GetInt());
		SetDialogVariable("pinkscorenum", fo_dom_pink_score.GetInt());
		SetDialogVariable("dominationlimit", fo_dom_scorelimit.GetInt());

		m_flNextThink = gpGlobals->curtime + 0.1f;
	}
}
