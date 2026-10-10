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
#include "entity_capture_flag.h"
#include "c_tf_player.h"
#include "c_team.h"
#include "c_tf_team.h"
#include "c_team_objectiveresource.h"
#include "tf_hud_objectivestatus.h"
#include "tf_spectatorgui.h"
#include "teamplayroundbased_gamerules.h"
#include "tf_gamerules.h"
#include "tf_hud_freezepanel.h"

using namespace vgui;

CUtlVector<int> g_Flags;
CUtlVector<int> g_CaptureZones;

DECLARE_BUILD_FACTORY( CTFArrowPanel );
DECLARE_BUILD_FACTORY( CTFFlagStatus );

extern ConVar tf_flag_caps_per_round;

extern ConVar fo_ctp_red_score;
extern ConVar fo_ctp_blue_score;
extern ConVar fo_ctp_green_score;
extern ConVar fo_ctp_yellow_score;
extern ConVar fo_ctp_purple_score;
extern ConVar fo_ctp_pink_score;
extern ConVar fo_ctp_scorelimit;

extern ConVar fo_ctp_red_timer;
extern ConVar fo_ctp_blue_timer;
extern ConVar fo_ditr_is_diamond_out;
extern ConVar fo_ditr_diamond_progress;
extern ConVar fo_ditr_diamond_digging;
extern ConVar fo_ctp_green_timer;
extern ConVar fo_ctp_yellow_timer;
extern ConVar fo_ctp_purple_timer;
extern ConVar fo_ctp_pink_timer;

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
CTFArrowPanel::CTFArrowPanel( Panel *parent, const char *name ) : CTFImagePanel( parent, name )
{
	m_RedMaterial.Init( "hud/objectives_flagpanel_compass_red", TEXTURE_GROUP_VGUI ); 
	m_BlueMaterial.Init( "hud/objectives_flagpanel_compass_blue", TEXTURE_GROUP_VGUI ); 
	m_GreenMaterial.Init("hud/objectives_flagpanel_compass_green", TEXTURE_GROUP_VGUI);
	m_NeutralMaterial.Init( "hud/objectives_flagpanel_compass_grey", TEXTURE_GROUP_VGUI ); 

	m_RedSmallMaterial.Init("hud/objectives_6flagpanel_compass_red", TEXTURE_GROUP_VGUI);
	m_BlueSmallMaterial.Init("hud/objectives_6flagpanel_compass_blue", TEXTURE_GROUP_VGUI);
	m_GreenSmallMaterial.Init("hud/objectives_6flagpanel_compass_green", TEXTURE_GROUP_VGUI);
	m_YellowSmallMaterial.Init("hud/objectives_6flagpanel_compass_yellow", TEXTURE_GROUP_VGUI);
	m_PurpleSmallMaterial.Init("hud/objectives_6flagpanel_compass_purple", TEXTURE_GROUP_VGUI);
	m_PinkSmallMaterial.Init("hud/objectives_6flagpanel_compass_pink", TEXTURE_GROUP_VGUI);

	m_RedSmallMaterialNoArrow.Init("hud/objectives_6flagpanel_compass_red_noArrow", TEXTURE_GROUP_VGUI);
	m_BlueSmallMaterialNoArrow.Init("hud/objectives_6flagpanel_compass_blue_noArrow", TEXTURE_GROUP_VGUI);
	m_GreenSmallMaterialNoArrow.Init("hud/objectives_6flagpanel_compass_green_noArrow", TEXTURE_GROUP_VGUI);
	m_YellowSmallMaterialNoArrow.Init("hud/objectives_6flagpanel_compass_yellow_noArrow", TEXTURE_GROUP_VGUI);
	m_PurpleSmallMaterialNoArrow.Init("hud/objectives_6flagpanel_compass_purple_noArrow", TEXTURE_GROUP_VGUI);
	m_PinkSmallMaterialNoArrow.Init("hud/objectives_6flagpanel_compass_pink_noArrow", TEXTURE_GROUP_VGUI);

	m_RedMaterialNoArrow.Init( "hud/objectives_flagpanel_compass_red_noArrow", TEXTURE_GROUP_VGUI ); 
	m_BlueMaterialNoArrow.Init( "hud/objectives_flagpanel_compass_blue_noArrow", TEXTURE_GROUP_VGUI ); 
	m_GreenMaterialNoArrow.Init("hud/objectives_flagpanel_compass_green_noArrow", TEXTURE_GROUP_VGUI);
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
float CTFArrowPanel::GetAngleRotation( void )
{
	float flRetVal = 0.0f;

	C_TFPlayer *pPlayer = ToTFPlayer( C_BasePlayer::GetLocalPlayer() );
	C_BaseEntity *pEnt = m_hEntity.Get();

	if ( pPlayer && pEnt )
	{
		QAngle vangles;
		Vector eyeOrigin;
		float zNear, zFar, fov;

		pPlayer->CalcView( eyeOrigin, vangles, zNear, zFar, fov );

		Vector vecFlag = pEnt->WorldSpaceCenter() - eyeOrigin;
		vecFlag.z = 0;
		vecFlag.NormalizeInPlace();

		Vector forward, right, up;
		AngleVectors( vangles, &forward, &right, &up );
		forward.z = 0;
		right.z = 0;
		forward.NormalizeInPlace();
		right.NormalizeInPlace();

		float dot = DotProduct( vecFlag, forward );
		float angleBetween = acos( dot );

		dot = DotProduct( vecFlag, right );

		if ( dot < 0.0f )
		{
			angleBetween *= -1;
		}

		flRetVal = RAD2DEG( angleBetween );
	}

	return flRetVal;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFArrowPanel::Paint()
{
	if ( !m_hEntity.Get() )
		return;

	C_BaseEntity *pEnt = m_hEntity.Get();
	IMaterial *pMaterial = m_NeutralMaterial;

	C_TFPlayer *pLocalPlayer = C_TFPlayer::GetLocalTFPlayer();

	// figure out what material we need to use
	if ( pEnt->GetTeamNumber() == TF_TEAM_RED )
	{
		if (TFGameRules() && TFGameRules()->ExtraTeamMode() == 2)
		{
			pMaterial = m_RedSmallMaterial;
		}
		else
		{
			pMaterial = m_RedMaterial;
		}

		if ( pLocalPlayer && ( pLocalPlayer->GetObserverMode() == OBS_MODE_IN_EYE ) )
		{
			// is our target a player?
			C_BaseEntity *pTargetEnt = pLocalPlayer->GetObserverTarget();
			if ( pTargetEnt && pTargetEnt->IsPlayer() )
			{
				// does our target have the flag and are they carrying the flag we're currently drawing?
				C_TFPlayer *pTarget = static_cast< C_TFPlayer* >( pTargetEnt );
				if ( pTarget->HasTheFlag() && ( pTarget->GetItem() == pEnt ) )
				{
					if (TFGameRules() && TFGameRules()->ExtraTeamMode() == 2)
					{
						pMaterial = m_RedSmallMaterialNoArrow;
					}
					else
					{
						pMaterial = m_RedMaterialNoArrow;
					}
				}
			}
		}
	}
	else if ( pEnt->GetTeamNumber() == TF_TEAM_BLUE )
	{
		if (TFGameRules() && TFGameRules()->ExtraTeamMode() == 2)
		{
			pMaterial = m_BlueSmallMaterial;
		}
		else
		{
			pMaterial = m_BlueMaterial;
		}

		if ( pLocalPlayer && ( pLocalPlayer->GetObserverMode() == OBS_MODE_IN_EYE ) )
		{
			// is our target a player?
			C_BaseEntity *pTargetEnt = pLocalPlayer->GetObserverTarget();
			if ( pTargetEnt && pTargetEnt->IsPlayer() )
			{
				// does our target have the flag and are they carrying the flag we're currently drawing?
				C_TFPlayer *pTarget = static_cast< C_TFPlayer* >( pTargetEnt );
				if ( pTarget->HasTheFlag() && ( pTarget->GetItem() == pEnt ) )
				{
					if (TFGameRules() && TFGameRules()->ExtraTeamMode() == 2)
					{
						pMaterial = m_BlueSmallMaterialNoArrow;
					}
					else
					{
						pMaterial = m_BlueMaterialNoArrow;
					}
				}
			}
		}
	}
	else if (pEnt->GetTeamNumber() == FO_TEAM_GREEN)
	{
		if (TFGameRules() && TFGameRules()->ExtraTeamMode() == 2)
		{
			pMaterial = m_GreenSmallMaterial;
		}
		else
		{
			pMaterial = m_GreenMaterial;
		}

		if (pLocalPlayer && (pLocalPlayer->GetObserverMode() == OBS_MODE_IN_EYE))
		{
			// is our target a player?
			C_BaseEntity *pTargetEnt = pLocalPlayer->GetObserverTarget();
			if (pTargetEnt && pTargetEnt->IsPlayer())
			{
				// does our target have the flag and are they carrying the flag we're currently drawing?
				C_TFPlayer *pTarget = static_cast<C_TFPlayer*>(pTargetEnt);
				if (pTarget->HasTheFlag() && (pTarget->GetItem() == pEnt))
				{
					if (TFGameRules() && TFGameRules()->ExtraTeamMode() == 2)
					{
						pMaterial = m_GreenSmallMaterialNoArrow;
					}
					else
					{
						pMaterial = m_GreenMaterialNoArrow;
					}
				}
			}
		}
	}
	else if (pEnt->GetTeamNumber() == FO_TEAM_YELLOW)
	{
		pMaterial = m_YellowSmallMaterial;

		if (pLocalPlayer && (pLocalPlayer->GetObserverMode() == OBS_MODE_IN_EYE))
		{
			// is our target a player?
			C_BaseEntity *pTargetEnt = pLocalPlayer->GetObserverTarget();
			if (pTargetEnt && pTargetEnt->IsPlayer())
			{
				// does our target have the flag and are they carrying the flag we're currently drawing?
				C_TFPlayer *pTarget = static_cast<C_TFPlayer*>(pTargetEnt);
				if (pTarget->HasTheFlag() && (pTarget->GetItem() == pEnt))
				{
					pMaterial = m_YellowSmallMaterialNoArrow;
				}
			}
		}
	}
	else if (pEnt->GetTeamNumber() == FO_TEAM_PURPLE)
	{
		pMaterial = m_PurpleSmallMaterial;

		if (pLocalPlayer && (pLocalPlayer->GetObserverMode() == OBS_MODE_IN_EYE))
		{
			// is our target a player?
			C_BaseEntity *pTargetEnt = pLocalPlayer->GetObserverTarget();
			if (pTargetEnt && pTargetEnt->IsPlayer())
			{
				// does our target have the flag and are they carrying the flag we're currently drawing?
				C_TFPlayer *pTarget = static_cast<C_TFPlayer*>(pTargetEnt);
				if (pTarget->HasTheFlag() && (pTarget->GetItem() == pEnt))
				{
					pMaterial = m_PurpleSmallMaterialNoArrow;
				}
			}
		}
	}
	else if (pEnt->GetTeamNumber() == FO_TEAM_PINK)
	{
		pMaterial = m_PinkSmallMaterial;

		if (pLocalPlayer && (pLocalPlayer->GetObserverMode() == OBS_MODE_IN_EYE))
		{
			// is our target a player?
			C_BaseEntity *pTargetEnt = pLocalPlayer->GetObserverTarget();
			if (pTargetEnt && pTargetEnt->IsPlayer())
			{
				// does our target have the flag and are they carrying the flag we're currently drawing?
				C_TFPlayer *pTarget = static_cast<C_TFPlayer*>(pTargetEnt);
				if (pTarget->HasTheFlag() && (pTarget->GetItem() == pEnt))
				{
					pMaterial = m_PinkSmallMaterialNoArrow;
				}
			}
		}
	}
	else if (pEnt->GetTeamNumber() == TEAM_UNASSIGNED) // the diamond
	{
		pMaterial = m_NeutralMaterial;

		if (pLocalPlayer && (pLocalPlayer->GetObserverMode() == OBS_MODE_IN_EYE))
		{
			// is our target a player?
			C_BaseEntity *pTargetEnt = pLocalPlayer->GetObserverTarget();
			if (pTargetEnt && pTargetEnt->IsPlayer())
			{
				// does our target have the flag and are they carrying the flag we're currently drawing?
				C_TFPlayer *pTarget = static_cast<C_TFPlayer*>(pTargetEnt);
				if (pTarget->HasTheFlag() && (pTarget->GetItem() == pEnt))
				{
					pMaterial = m_RedMaterialNoArrow;
				}
			}
		}
	}

	int x = 0;
	int y = 0;
	ipanel()->GetAbsPos( GetVPanel(), x, y );
	int nWidth = GetWide();
	int nHeight = GetTall();

	CMatRenderContextPtr pRenderContext( materials );
	pRenderContext->MatrixMode( MATERIAL_MODEL );
	pRenderContext->PushMatrix(); 

	VMatrix panelRotation;
	panelRotation.Identity();
	MatrixBuildRotationAboutAxis( panelRotation, Vector( 0, 0, 1 ), GetAngleRotation() );
//	MatrixRotate( panelRotation, Vector( 1, 0, 0 ), 5 );
	panelRotation.SetTranslation( Vector( x + nWidth/2, y + nHeight/2, 0 ) );
	pRenderContext->LoadMatrix( panelRotation );

	IMesh *pMesh = pRenderContext->GetDynamicMesh( true, NULL, NULL, pMaterial );

	CMeshBuilder meshBuilder;
	meshBuilder.Begin( pMesh, MATERIAL_QUADS, 1 );

	meshBuilder.TexCoord2f( 0, 0, 0 );
	meshBuilder.Position3f( -nWidth/2, -nHeight/2, 0 );
	meshBuilder.Color4ub( 255, 255, 255, 255 );
	meshBuilder.AdvanceVertex();

	meshBuilder.TexCoord2f( 0, 1, 0 );
	meshBuilder.Position3f( nWidth/2, -nHeight/2, 0 );
	meshBuilder.Color4ub( 255, 255, 255, 255 );
	meshBuilder.AdvanceVertex();

	meshBuilder.TexCoord2f( 0, 1, 1 );
	meshBuilder.Position3f( nWidth/2, nHeight/2, 0 );
	meshBuilder.Color4ub( 255, 255, 255, 255 );
	meshBuilder.AdvanceVertex();

	meshBuilder.TexCoord2f( 0, 0, 1 );
	meshBuilder.Position3f( -nWidth/2, nHeight/2, 0 );
	meshBuilder.Color4ub( 255, 255, 255, 255 );
	meshBuilder.AdvanceVertex();

	meshBuilder.End();

	pMesh->Draw();
	pRenderContext->PopMatrix();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool CTFArrowPanel::IsVisible( void )
{
	if( IsTakingAFreezecamScreenshot() )
		return false;

	return BaseClass::IsVisible();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
CTFFlagStatus::CTFFlagStatus( Panel *parent, const char *name ) : EditablePanel( parent, name )
{
	m_pArrow = NULL;
	m_pStatusIcon = NULL;
	m_pBriefcase = NULL;
	m_p6StatusIcon = NULL;
	m_p6Briefcase = NULL;
	m_hEntity = NULL;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFFlagStatus::ApplySchemeSettings( IScheme *pScheme )
{
	BaseClass::ApplySchemeSettings( pScheme );

	// load control settings...
	LoadControlSettings( "resource/UI/FlagStatus.res" );

	m_pArrow = dynamic_cast<CTFArrowPanel *>( FindChildByName( "Arrow" ) );
	m_pStatusIcon = dynamic_cast<CTFImagePanel *>(FindChildByName("StatusIcon"));
	m_pBriefcase = dynamic_cast<CTFImagePanel *>( FindChildByName( "Briefcase" ) );
	m_p6StatusIcon = dynamic_cast<CTFImagePanel *>(FindChildByName("6StatusIcon"));
	m_p6Briefcase = dynamic_cast<CTFImagePanel *>(FindChildByName("6Briefcase"));
	m_pDiamondStatusIcon = dynamic_cast<CTFImagePanel *>(FindChildByName("DiamondStatusIcon"));
	m_pDiamond = dynamic_cast<CTFImagePanel *>(FindChildByName("Diamond"));
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool CTFFlagStatus::IsVisible( void )
{
	// Only flag game modes use this panel
	if ( !TFGameRules() || ( TFGameRules()->GetGameType() != FO_GAMETYPE_DITR && TFGameRules()->GetGameType() != TF_GAMETYPE_CTF && TFGameRules()->GetGameType() != FO_GAMETYPE_CTP ) )
		return false;

	if (TFGameRules() && TFGameRules()->ExtraTeamMode() == 2)
	{
		if (m_p6Briefcase && m_p6StatusIcon && m_pBriefcase && m_pStatusIcon)
		{
			m_p6Briefcase->SetVisible(true);
			m_p6StatusIcon->SetVisible(true);
			m_pBriefcase->SetVisible(false);
			m_pStatusIcon->SetVisible(false);
		}
	}
	else
	{
		if (m_p6Briefcase && m_p6StatusIcon && m_pBriefcase && m_pStatusIcon)
		{
			m_p6Briefcase->SetVisible(false);
			m_p6StatusIcon->SetVisible(false);
			m_pBriefcase->SetVisible(true);
			m_pStatusIcon->SetVisible(true);
		}
	}

	if (TFGameRules() && TFGameRules()->GetGameType() == FO_GAMETYPE_DITR)
	{
		if (m_p6Briefcase && m_p6StatusIcon && m_pBriefcase && m_pStatusIcon)
		{
			m_p6Briefcase->SetVisible(false);
			m_p6StatusIcon->SetVisible(false);
			m_pBriefcase->SetVisible(false);
			m_pStatusIcon->SetVisible(false);
			m_pDiamond->SetVisible(false);
			m_pDiamondStatusIcon->SetVisible(false);

			if ( m_hEntity.Get() )
			{
				// The diamond is the teamless flag
				CCaptureFlag *pFlag = dynamic_cast<CCaptureFlag *>( m_hEntity.Get() );
				if ( pFlag )
				{
					if ( pFlag->GetTeamNumber() == TEAM_UNASSIGNED )
					{
						m_pDiamond->SetVisible(true);
						m_pDiamondStatusIcon->SetVisible(true);
					}
					else
					{
						m_pDiamond->SetVisible(false);
						m_pDiamondStatusIcon->SetVisible(false);
					}
				}
				else
				{
					m_pDiamond->SetVisible(false);
					m_pDiamondStatusIcon->SetVisible(false);
				}
			}
		}
	}
	else
	{
		if (m_pDiamond && m_pDiamondStatusIcon)
		{
			m_pDiamond->SetVisible(false);
			m_pDiamondStatusIcon->SetVisible(false);
		}
	}

	if( IsTakingAFreezecamScreenshot() )
		return false;

	return BaseClass::IsVisible();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFFlagStatus::UpdateStatus( void )
{
	if ( m_hEntity.Get() )
	{
		CCaptureFlag *pFlag = dynamic_cast<CCaptureFlag *>( m_hEntity.Get() );
	
		if ( pFlag )
		{
			const char *pszImage = "../hud/objectives_flagpanel_ico_flag_home";
			const char *psz6Image = "../hud/objectives_6flagpanel_ico_flag_home";

			if (pFlag->IsDropped())
			{
				pszImage = "../hud/objectives_flagpanel_ico_flag_dropped";
				psz6Image = "../hud/objectives_6flagpanel_ico_flag_dropped";
			}
			else if (pFlag->IsStolen())
			{
				pszImage = "../hud/objectives_flagpanel_ico_flag_moving";
				psz6Image = "../hud/objectives_6flagpanel_ico_flag_moving";
			}

			if (m_pStatusIcon)
			{
				m_pStatusIcon->SetImage(pszImage);
				m_p6StatusIcon->SetImage(psz6Image);
				m_pDiamondStatusIcon->SetImage(pszImage);
			}
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
CTFHudFlagObjectives::CTFHudFlagObjectives( Panel *parent, const char *name ) : EditablePanel( parent, name )
{
	m_pCarriedImage = NULL;
	m_pPlayingTo = NULL;
	m_bFlagAnimationPlayed = false;
	m_bCarryingFlag = false;
	m_pSpecCarriedImage = NULL;

	m_pRedTimer = NULL;
	m_pRedTimerShadow = NULL;
	m_pBlueTimer = NULL;
	m_pBlueTimerShadow = NULL;

	vgui::ivgui()->AddTickSignal( GetVPanel() );

	ListenForGameEvent( "flagstatus_update" );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool CTFHudFlagObjectives::IsVisible( void )
{
	if( IsTakingAFreezecamScreenshot() )
		return false;

	return BaseClass::IsVisible();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFHudFlagObjectives::ApplySchemeSettings( IScheme *pScheme )
{
	BaseClass::ApplySchemeSettings( pScheme );

	// load control settings...
	LoadControlSettings( "resource/UI/HudObjectiveFlagPanel.res" );

	m_pCarriedImage = dynamic_cast<CTFImagePanel *>( FindChildByName( "CarriedImage" ) );
	m_pPlayingTo = dynamic_cast<CTFLabel *>( FindChildByName( "PlayingTo" ) );
	m_pPlayingToBG = dynamic_cast<CTFImagePanel *>( FindChildByName( "PlayingToBG" ) );

	m_pRedFlag = dynamic_cast<CTFFlagStatus *>( FindChildByName( "RedFlag" ) );
	m_pBlueFlag = dynamic_cast<CTFFlagStatus *>( FindChildByName( "BlueFlag" ) );
	m_pDiamondFlag = dynamic_cast<CTFFlagStatus *>( FindChildByName( "DiamondFlag" ) );

	m_pCapturePoint = dynamic_cast<CTFArrowPanel *>( FindChildByName( "CaptureFlag" ) );

	m_pSpecCarriedImage = dynamic_cast<ImagePanel *>( FindChildByName( "SpecCarriedImage" ) );

	m_pRedTimer = dynamic_cast<CTFLabel *>(FindChildByName("RedTimer"));
	m_pRedTimerShadow = dynamic_cast<CTFLabel *>(FindChildByName("RedTimerShadow"));
	m_pBlueTimer = dynamic_cast<CTFLabel *>(FindChildByName("BlueTimer"));
	m_pBlueTimerShadow = dynamic_cast<CTFLabel *>(FindChildByName("BlueTimerShadow"));
	m_pDiamondProgress = dynamic_cast<CTFLabel *>(FindChildByName("DiamondProgress"));
	m_pDiamondProgressShadow = dynamic_cast<CTFLabel *>(FindChildByName("DiamondProgressShadow"));

	// outline is always on, so we need to init the alpha to 0
	CTFImagePanel *pOutline = dynamic_cast<CTFImagePanel *>( FindChildByName( "OutlineImage" ) );
	if ( pOutline )
	{
		pOutline->SetAlpha( 0 );
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFHudFlagObjectives::Reset()
{
	g_pClientMode->GetViewportAnimationController()->StartAnimationSequence( "FlagOutlineHide" );

	if ( m_pCarriedImage && m_pCarriedImage->IsVisible() )
	{
		m_pCarriedImage->SetVisible( false );
	}

	if ( m_pBlueFlag && !m_pBlueFlag->IsVisible() )
	{
		m_pBlueFlag->SetVisible( true );
	}

	if ( m_pRedFlag && !m_pRedFlag->IsVisible() )
	{
		m_pRedFlag->SetVisible( true );
	}

	if ( TFGameRules() && TFGameRules()->GetGameType() == FO_GAMETYPE_DITR && m_pDiamondFlag && !m_pDiamondFlag->IsVisible() )
	{
		m_pDiamondFlag->SetVisible( true );
	}

	if ( m_pSpecCarriedImage && m_pSpecCarriedImage->IsVisible() )
	{
		m_pSpecCarriedImage->SetVisible( false );
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFHudFlagObjectives::SetPlayingToLabelVisible( bool bVisible )
{
	if ( m_pPlayingTo && m_pPlayingToBG )
	{
		if ( m_pPlayingTo->IsVisible() != bVisible )
		{
			m_pPlayingTo->SetVisible( bVisible );
		}

		if ( m_pPlayingToBG->IsVisible() != bVisible )
		{
			m_pPlayingToBG->SetVisible( bVisible );
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFHudFlagObjectives::OnTick()
{
	// iterate through the flags to set their position in our HUD
	for ( int i = 0; i < g_Flags.Count(); i++ )
	{
		CCaptureFlag *pFlag = dynamic_cast< CCaptureFlag* >( ClientEntityList().GetEnt( g_Flags[i] ) );

		if ( pFlag )
		{
			if ( !pFlag->IsDisabled() )
			{
				if ( m_pRedFlag && pFlag->GetTeamNumber() == TF_TEAM_RED )
				{
					m_pRedFlag->SetEntity( pFlag );
				}
				else if ( m_pBlueFlag && pFlag->GetTeamNumber() == TF_TEAM_BLUE )
				{
					m_pBlueFlag->SetEntity( pFlag );
				}
				else if ( m_pDiamondFlag && pFlag->GetTeamNumber() == TEAM_UNASSIGNED )
				{
					m_pDiamondFlag->SetEntity( pFlag );
				}
			}
		}
		else
		{
			// this isn't a valid index for a flag
			g_Flags.Remove( i );
		}
	}

	SetDialogVariable("redtimer", fo_ctp_red_timer.GetInt());
	SetDialogVariable("bluetimer", fo_ctp_blue_timer.GetInt());
	SetDialogVariable("diamondprogress", fo_ditr_diamond_progress.GetInt());

	if (TFGameRules() && TFGameRules()->GetGameType() == FO_GAMETYPE_DITR)
	{
		if (m_pDiamondFlag && !fo_ditr_is_diamond_out.GetBool())
		{
			// Still buried: show the dig progress while someone is digging
			m_pDiamondFlag->SetVisible(false);

			if (fo_ditr_diamond_digging.GetBool())
			{
				m_pDiamondProgress->SetVisible(true);
				m_pDiamondProgressShadow->SetVisible(true);
			}
			else
			{
				m_pDiamondProgress->SetVisible(false);
				m_pDiamondProgressShadow->SetVisible(false);
			}
		}
		else if (m_pDiamondFlag)
		{
			// Dug up: line the capture arrow and carried icon up with the diamond
			m_pDiamondProgress->SetVisible(false);
			m_pDiamondProgressShadow->SetVisible(false);

			m_pCapturePoint->SetPos(m_pCapturePoint->GetXPos(), m_pDiamondFlag->GetYPos());
			m_pCarriedImage->SetPos((double)m_pDiamondFlag->GetXPos() - (ScreenWidth() / 640.0f) * 40.0f, m_pDiamondFlag->GetYPos() + YRES(10));

			if (!m_bCarryingFlag)
				m_pDiamondFlag->SetVisible(true);
			else
				m_pDiamondFlag->SetVisible(false);
		}
	}
	else
	{
		if (m_pDiamondFlag)
			m_pDiamondFlag->SetVisible(false);

		if (m_pDiamondProgress && m_pDiamondProgressShadow)
		{
			m_pDiamondProgress->SetVisible(false);
			m_pDiamondProgressShadow->SetVisible(false);
		}
	}


	if (TFGameRules() && TFGameRules()->GetGameType() == FO_GAMETYPE_CTP)
	{
		if (m_pBlueFlag)
			m_pBlueFlag->SetVisible(false);
		if (m_pRedFlag)
			m_pRedFlag->SetVisible(false);

		if (m_pRedTimer && m_pRedTimerShadow)
		{
			if (fo_ctp_red_timer.GetInt() == 21 || fo_ctp_red_timer.GetInt() == -1)
			{
				m_pRedTimer->SetVisible(false);
				m_pRedTimerShadow->SetVisible(false);
			}
			else
			{
				m_pRedTimer->SetVisible(true);
				m_pRedTimerShadow->SetVisible(true);
			}
		}

		if (m_pBlueTimer && m_pBlueTimerShadow)
		{
			if (fo_ctp_blue_timer.GetInt() == 21 || fo_ctp_blue_timer.GetInt() == -1)
			{
				m_pBlueTimer->SetVisible(false);
				m_pBlueTimerShadow->SetVisible(false);
			}
			else
			{
				m_pBlueTimer->SetVisible(true);
				m_pBlueTimerShadow->SetVisible(true);
			}
		}
	}
	else
	{
		if (m_pBlueFlag && !m_pCarriedImage->IsVisible())
			m_pBlueFlag->SetVisible(true);
		if (m_pRedFlag && !m_pCarriedImage->IsVisible())
			m_pRedFlag->SetVisible(true);

		if (m_pRedTimer)
			m_pRedTimer->SetVisible(false);
		if (m_pRedTimerShadow)
			m_pRedTimerShadow->SetVisible(false);

		if (m_pBlueTimer)
			m_pBlueTimer->SetVisible(false);
		if (m_pBlueTimerShadow)
			m_pBlueTimerShadow->SetVisible(false);
	}

	// are we playing captures for rounds?
	if ( tf_flag_caps_per_round.GetInt() > 0 )
	{
		C_TFTeam *pTeam = GetGlobalTFTeam( TF_TEAM_BLUE );
		if ( pTeam )
		{
			if (TFGameRules() && TFGameRules()->GetGameType() == FO_GAMETYPE_CTP)
			{
				SetDialogVariable("bluescore", fo_ctp_blue_score.GetInt());
			}
			else
			{
				SetDialogVariable("bluescore", pTeam->GetFlagCaptures());
			}
		}

		pTeam = GetGlobalTFTeam( TF_TEAM_RED );
		if ( pTeam )
		{
			if (TFGameRules() && TFGameRules()->GetGameType() == FO_GAMETYPE_CTP)
			{
				SetDialogVariable("redscore", fo_ctp_red_score.GetInt());
			}
			else
			{
				SetDialogVariable("redscore", pTeam->GetFlagCaptures());
			}
		}

		SetPlayingToLabelVisible( true );
		if (TFGameRules() && TFGameRules()->GetGameType() == FO_GAMETYPE_CTP)
		{
			SetDialogVariable("rounds", fo_ctp_scorelimit.GetInt());
		}
		else
		{
			SetDialogVariable("rounds", tf_flag_caps_per_round.GetInt());
		}
	}
	else // we're just playing straight score
	{
		C_TFTeam *pTeam = GetGlobalTFTeam( TF_TEAM_BLUE );
		if ( pTeam )
		{
			if (TFGameRules() && TFGameRules()->GetGameType() == FO_GAMETYPE_CTP)
			{
				SetDialogVariable("bluescore", fo_ctp_blue_score.GetInt());
			}
			else
			{
				SetDialogVariable("bluescore", pTeam->Get_Score());
			}
		}

		pTeam = GetGlobalTFTeam( TF_TEAM_RED );
		if ( pTeam )
		{
			if (TFGameRules() && TFGameRules()->GetGameType() == FO_GAMETYPE_CTP)
			{
				SetDialogVariable("redscore", fo_ctp_red_score.GetInt());
			}
			else
			{
				SetDialogVariable("redscore", pTeam->Get_Score());
			}
		}

		SetPlayingToLabelVisible( false );
	}

	// check the local player to see if they're spectating, OBS_MODE_IN_EYE, and the target entity is carrying the flag
	bool bSpecCarriedImage = false;
	C_TFPlayer *pPlayer = C_TFPlayer::GetLocalTFPlayer();
	if ( pPlayer && ( pPlayer->GetObserverMode() == OBS_MODE_IN_EYE ) )
	{
		// does our target have the flag?
		C_BaseEntity *pEnt = pPlayer->GetObserverTarget();
		if ( pEnt && pEnt->IsPlayer() )
		{
			C_TFPlayer *pTarget = static_cast< C_TFPlayer* >( pEnt );
			if ( pTarget->HasTheFlag() )
			{
				bSpecCarriedImage = true;
				if ( pTarget->GetTeamNumber() == TF_TEAM_RED )
				{
					if ( m_pSpecCarriedImage )
					{
						m_pSpecCarriedImage->SetImage( "../hud/objectives_flagpanel_carried_blue" );
					}
				}
				else
				{
					if ( m_pSpecCarriedImage )
					{
						m_pSpecCarriedImage->SetImage( "../hud/objectives_flagpanel_carried_red" );
					}
				}
			}
		}
	}

	if ( bSpecCarriedImage )
	{
		if ( m_pSpecCarriedImage && !m_pSpecCarriedImage->IsVisible() )
		{
			m_pSpecCarriedImage->SetVisible( true );
		}
	}
	else
	{
		if ( m_pSpecCarriedImage && m_pSpecCarriedImage->IsVisible() )
		{
			m_pSpecCarriedImage->SetVisible( false );
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFHudFlagObjectives::UpdateStatus( void )
{
	C_TFPlayer *pLocalPlayer = ToTFPlayer( C_BasePlayer::GetLocalPlayer() );

	// are we carrying a flag?
	CCaptureFlag *pPlayerFlag = NULL;
	if ( pLocalPlayer && pLocalPlayer->HasItem() && ( pLocalPlayer->GetItem()->GetItemID() == TF_ITEM_CAPTURE_FLAG ) )
	{
		pPlayerFlag = dynamic_cast<CCaptureFlag*>( pLocalPlayer->GetItem() );
	}

	if ( pPlayerFlag )
	{
		m_bCarryingFlag = true;

		// make sure the panels are on, set the initial alpha values, 
		// set the color of the flag we're carrying, and start the animations
		if ( m_pCarriedImage && !m_bFlagAnimationPlayed )
		{
			m_bFlagAnimationPlayed = true;

			if ( m_pBlueFlag && m_pBlueFlag->IsVisible() )
			{
				m_pBlueFlag->SetVisible( false );
			}

			if ( m_pRedFlag && m_pRedFlag->IsVisible() )
			{
				m_pRedFlag->SetVisible( false );
			}

			if ( !m_pCarriedImage->IsVisible() )
			{
				m_pCarriedImage->SetVisible( true );
			}

			if ( m_pDiamondFlag && m_pDiamondFlag->IsVisible() )
			{
				m_pDiamondFlag->SetVisible( false );
			}

			g_pClientMode->GetViewportAnimationController()->StartAnimationSequence( "FlagOutline" );

			if ( m_pCapturePoint )
			{
				if ( !m_pCapturePoint->IsVisible() )
				{
					m_pCapturePoint->SetVisible( true );
				}

				if ( pLocalPlayer )
				{
					// go through all the capture zones and find ours
					for ( int i = 0; i < g_CaptureZones.Count(); i++ )
					{
						C_BaseEntity *pZone = ClientEntityList().GetEnt( g_CaptureZones[i] );

						if ( pZone )
						{
							if ( pZone->GetTeamNumber() == pLocalPlayer->GetTeamNumber() )
							{
								m_pCapturePoint->SetEntity( pZone );
							}
						}
					}
				}
			}
		}
	}
	else
	{
		// were we carrying the flag?
		if ( m_bCarryingFlag )
		{
			m_bCarryingFlag = false;
			g_pClientMode->GetViewportAnimationController()->StartAnimationSequence( "FlagOutline" );
		}

		m_bFlagAnimationPlayed = false;

		if ( m_pCarriedImage && m_pCarriedImage->IsVisible() )
		{
			m_pCarriedImage->SetVisible( false );
		}

		if ( m_pCapturePoint && m_pCapturePoint->IsVisible() )
		{
			m_pCapturePoint->SetVisible( false );
		}

		if ( m_pBlueFlag )
		{
			if ( !m_pBlueFlag->IsVisible() )
			{
				m_pBlueFlag->SetVisible( true );
			}
			
			m_pBlueFlag->UpdateStatus();
		}

		if ( m_pRedFlag )
		{
			if ( !m_pRedFlag->IsVisible() )
			{
				m_pRedFlag->SetVisible( true );
			}

			m_pRedFlag->UpdateStatus();
		}

		if ( m_pDiamondFlag )
		{
			if ( !m_pDiamondFlag->IsVisible() )
			{
				m_pDiamondFlag->SetVisible( true );
			}

			m_pDiamondFlag->UpdateStatus();
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFHudFlagObjectives::FireGameEvent( IGameEvent *event )
{
	const char *eventName = event->GetName();

	if ( !Q_strcmp( eventName, "flagstatus_update" ) )
	{
		UpdateStatus();
	}
}

// 3TEAM BELOW

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
CFOHud3FlagObjectives::CFOHud3FlagObjectives(Panel *parent, const char *name) : EditablePanel(parent, name)
{
	m_pCarriedImage = NULL;
	m_pPlayingTo = NULL;
	m_bFlagAnimationPlayed = false;
	m_bCarryingFlag = false;
	m_pSpecCarriedImage = NULL;

	m_pRedTimer = NULL;
	m_pRedTimerShadow = NULL;
	m_pBlueTimer = NULL;
	m_pBlueTimerShadow = NULL;
	m_pGreenTimer = NULL;
	m_pGreenTimerShadow = NULL;

	m_pGreenScore = NULL;
	m_pGreenScoreShadow = NULL;

	m_pGreenScoreCTP = NULL;
	m_pGreenScoreShadowCTP = NULL;

	vgui::ivgui()->AddTickSignal(GetVPanel());

	ListenForGameEvent("flagstatus_update");
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool CFOHud3FlagObjectives::IsVisible(void)
{
	if (IsTakingAFreezecamScreenshot())
		return false;

	return BaseClass::IsVisible();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CFOHud3FlagObjectives::ApplySchemeSettings(IScheme *pScheme)
{
	BaseClass::ApplySchemeSettings(pScheme);

	// load control settings...
	LoadControlSettings("resource/UI/HudObjective3FlagPanel.res");

	m_pCarriedImage = dynamic_cast<CTFImagePanel *>(FindChildByName("CarriedImage"));
	m_pPlayingTo = dynamic_cast<CTFLabel *>(FindChildByName("PlayingTo"));
	m_pPlayingToBG = dynamic_cast<CTFImagePanel *>(FindChildByName("PlayingToBG"));

	m_pRedFlag = dynamic_cast<CTFFlagStatus *>(FindChildByName("RedFlag"));
	m_pBlueFlag = dynamic_cast<CTFFlagStatus *>(FindChildByName("BlueFlag"));
	m_pGreenFlag = dynamic_cast<CTFFlagStatus *>(FindChildByName("GreenFlag"));

	m_pGreenScore = dynamic_cast<CTFLabel *>(FindChildByName("GreenScore"));
	m_pGreenScoreShadow = dynamic_cast<CTFLabel *>(FindChildByName("GreenScoreShadow"));

	m_pGreenScoreCTP = dynamic_cast<CTFLabel *>(FindChildByName("GreenScoreCTP"));
	m_pGreenScoreShadowCTP = dynamic_cast<CTFLabel *>(FindChildByName("GreenScoreShadowCTP"));

	m_pCapturePoint = dynamic_cast<CTFArrowPanel *>(FindChildByName("CaptureFlag"));

	m_pSpecCarriedImage = dynamic_cast<ImagePanel *>(FindChildByName("SpecCarriedImage"));

	m_pRedTimer = dynamic_cast<CTFLabel *>(FindChildByName("RedTimer"));
	m_pRedTimerShadow = dynamic_cast<CTFLabel *>(FindChildByName("RedTimerShadow"));
	m_pBlueTimer = dynamic_cast<CTFLabel *>(FindChildByName("BlueTimer"));
	m_pBlueTimerShadow = dynamic_cast<CTFLabel *>(FindChildByName("BlueTimerShadow"));
	m_pGreenTimer = dynamic_cast<CTFLabel *>(FindChildByName("GreenTimer"));
	m_pGreenTimerShadow = dynamic_cast<CTFLabel *>(FindChildByName("GreenTimerShadow"));

	// outline is always on, so we need to init the alpha to 0
	CTFImagePanel *pOutline = dynamic_cast<CTFImagePanel *>(FindChildByName("OutlineImage"));
	if (pOutline)
	{
		pOutline->SetAlpha(0);
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CFOHud3FlagObjectives::Reset()
{
	g_pClientMode->GetViewportAnimationController()->StartAnimationSequence("FlagOutlineHide");

	if (m_pCarriedImage && m_pCarriedImage->IsVisible())
	{
		m_pCarriedImage->SetVisible(false);
	}

	if (m_pBlueFlag && !m_pBlueFlag->IsVisible())
	{
		m_pBlueFlag->SetVisible(true);
	}

	if (m_pRedFlag && !m_pRedFlag->IsVisible())
	{
		m_pRedFlag->SetVisible(true);
	}

	if (m_pGreenFlag && !m_pGreenFlag->IsVisible())
	{
		m_pGreenFlag->SetVisible(true);
	}

	if (m_pSpecCarriedImage && m_pSpecCarriedImage->IsVisible())
	{
		m_pSpecCarriedImage->SetVisible(false);
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CFOHud3FlagObjectives::SetPlayingToLabelVisible(bool bVisible)
{
	if (m_pPlayingTo && m_pPlayingToBG)
	{
		if (m_pPlayingTo->IsVisible() != bVisible)
		{
			m_pPlayingTo->SetVisible(bVisible);
		}

		if (m_pPlayingToBG->IsVisible() != bVisible)
		{
			m_pPlayingToBG->SetVisible(bVisible);
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CFOHud3FlagObjectives::OnTick()
{
	// iterate through the flags to set their position in our HUD
	for (int i = 0; i < g_Flags.Count(); i++)
	{
		CCaptureFlag *pFlag = dynamic_cast<CCaptureFlag*>(ClientEntityList().GetEnt(g_Flags[i]));

		if (pFlag)
		{
			if (!pFlag->IsDisabled())
			{
				if (m_pRedFlag && pFlag->GetTeamNumber() == TF_TEAM_RED)
				{
					m_pRedFlag->SetEntity(pFlag);
				}
				else if (m_pBlueFlag && pFlag->GetTeamNumber() == TF_TEAM_BLUE)
				{
					m_pBlueFlag->SetEntity(pFlag);
				}
				else if (m_pGreenFlag && pFlag->GetTeamNumber() == FO_TEAM_GREEN)
				{
					m_pGreenFlag->SetEntity(pFlag);
				}
			}
		}
		else
		{
			// this isn't a valid index for a flag
			g_Flags.Remove(i);
		}
	}

	SetDialogVariable("redtimer", fo_ctp_red_timer.GetInt());
	SetDialogVariable("bluetimer", fo_ctp_blue_timer.GetInt());
	SetDialogVariable("greentimer", fo_ctp_green_timer.GetInt());


	if (TFGameRules() && TFGameRules()->GetGameType() == FO_GAMETYPE_CTP)
	{
		if (m_pBlueFlag)
			m_pBlueFlag->SetVisible(false);
		if (m_pRedFlag)
			m_pRedFlag->SetVisible(false);
		if (m_pGreenFlag)
			m_pGreenFlag->SetVisible(false);

		if (m_pGreenScore)
			m_pGreenScore->SetVisible(false);
		if (m_pGreenScoreShadow)
			m_pGreenScoreShadow->SetVisible(false);

		if (m_pGreenScoreCTP)
			m_pGreenScoreCTP->SetVisible(true);
		if (m_pGreenScoreShadowCTP)
			m_pGreenScoreShadowCTP->SetVisible(true);

		if (m_pRedTimer && m_pRedTimerShadow)
		{
			if (fo_ctp_red_timer.GetInt() == 21 || fo_ctp_red_timer.GetInt() == -1)
			{
				m_pRedTimer->SetVisible(false);
				m_pRedTimerShadow->SetVisible(false);
			}
			else
			{
				m_pRedTimer->SetVisible(true);
				m_pRedTimerShadow->SetVisible(true);
			}
		}

		if (m_pBlueTimer && m_pBlueTimerShadow)
		{
			if (fo_ctp_blue_timer.GetInt() == 21 || fo_ctp_blue_timer.GetInt() == -1)
			{
				m_pBlueTimer->SetVisible(false);
				m_pBlueTimerShadow->SetVisible(false);
			}
			else
			{
				m_pBlueTimer->SetVisible(true);
				m_pBlueTimerShadow->SetVisible(true);
			}
		}

		if (m_pGreenTimer && m_pGreenTimerShadow)
		{
			if (fo_ctp_green_timer.GetInt() == 21 || fo_ctp_green_timer.GetInt() == -1)
			{
				m_pGreenTimer->SetVisible(false);
				m_pGreenTimerShadow->SetVisible(false);
			}
			else
			{
				m_pGreenTimer->SetVisible(true);
				m_pGreenTimerShadow->SetVisible(true);
			}
		}
	}
	else
	{
		if (m_pBlueFlag && !m_pCarriedImage->IsVisible())
			m_pBlueFlag->SetVisible(true);
		if (m_pRedFlag && !m_pCarriedImage->IsVisible())
			m_pRedFlag->SetVisible(true);
		if (m_pGreenFlag && !m_pCarriedImage->IsVisible())
			m_pGreenFlag->SetVisible(true);

		if (m_pGreenScoreCTP)
			m_pGreenScoreCTP->SetVisible(false);
		if (m_pGreenScoreShadowCTP)
			m_pGreenScoreShadowCTP->SetVisible(false);

		if (m_pGreenScore)
			m_pGreenScore->SetVisible(true);
		if (m_pGreenScoreShadow)
			m_pGreenScoreShadow->SetVisible(true);

		if (m_pRedTimer)
			m_pRedTimer->SetVisible(false);
		if (m_pRedTimerShadow)
			m_pRedTimerShadow->SetVisible(false);

		if (m_pBlueTimer)
			m_pBlueTimer->SetVisible(false);
		if (m_pBlueTimerShadow)
			m_pBlueTimerShadow->SetVisible(false);

		if (m_pGreenTimer)
			m_pGreenTimer->SetVisible(false);
		if (m_pGreenTimerShadow)
			m_pGreenTimerShadow->SetVisible(false);
	}

	// are we playing captures for rounds?
	if (tf_flag_caps_per_round.GetInt() > 0)
	{
		C_TFTeam *pTeam = GetGlobalTFTeam(TF_TEAM_BLUE);
		if (pTeam)
		{
			if (TFGameRules() && TFGameRules()->GetGameType() == FO_GAMETYPE_CTP)
			{
				SetDialogVariable("bluescore", fo_ctp_blue_score.GetInt());
			}
			else
			{
				SetDialogVariable("bluescore", pTeam->GetFlagCaptures());
			}
		}

		pTeam = GetGlobalTFTeam(TF_TEAM_RED);
		if (pTeam)
		{
			if (TFGameRules() && TFGameRules()->GetGameType() == FO_GAMETYPE_CTP)
			{
				SetDialogVariable("redscore", fo_ctp_red_score.GetInt());
			}
			else
			{
				SetDialogVariable("redscore", pTeam->GetFlagCaptures());
			}
		}

		pTeam = GetGlobalTFTeam(FO_TEAM_GREEN);
		if (pTeam)
		{
			if (TFGameRules() && TFGameRules()->GetGameType() == FO_GAMETYPE_CTP)
			{
				SetDialogVariable("greenscore", fo_ctp_green_score.GetInt());
			}
			else
			{
				SetDialogVariable("greenscore", pTeam->GetFlagCaptures());
			}
		}

		SetPlayingToLabelVisible(true);
		if (TFGameRules() && TFGameRules()->GetGameType() == FO_GAMETYPE_CTP)
		{
			SetDialogVariable("rounds", fo_ctp_scorelimit.GetInt());
		}
		else
		{
			SetDialogVariable("rounds", tf_flag_caps_per_round.GetInt());
		}
	}
	else // we're just playing straight score
	{
		C_TFTeam *pTeam = GetGlobalTFTeam(TF_TEAM_BLUE);
		if (pTeam)
		{
			if (TFGameRules() && TFGameRules()->GetGameType() == FO_GAMETYPE_CTP)
			{
				SetDialogVariable("bluescore", fo_ctp_blue_score.GetInt());
			}
			else
			{
				SetDialogVariable("bluescore", pTeam->Get_Score());
			}
		}

		pTeam = GetGlobalTFTeam(TF_TEAM_RED);
		if (pTeam)
		{
			if (TFGameRules() && TFGameRules()->GetGameType() == FO_GAMETYPE_CTP)
			{
				SetDialogVariable("redscore", fo_ctp_red_score.GetInt());
			}
			else
			{
				SetDialogVariable("redscore", pTeam->Get_Score());
			}
		}

		pTeam = GetGlobalTFTeam(FO_TEAM_GREEN);
		if (pTeam)
		{
			if (TFGameRules() && TFGameRules()->GetGameType() == FO_GAMETYPE_CTP)
			{
				SetDialogVariable("greenscore", fo_ctp_green_score.GetInt());
			}
			else
			{
				SetDialogVariable("greenscore", pTeam->Get_Score());
			}
		}

		SetPlayingToLabelVisible(false);
	}

	// check the local player to see if they're spectating, OBS_MODE_IN_EYE, and the target entity is carrying the flag
	bool bSpecCarriedImage = false;
	C_TFPlayer *pPlayer = C_TFPlayer::GetLocalTFPlayer();
	if (pPlayer && (pPlayer->GetObserverMode() == OBS_MODE_IN_EYE))
	{
		// does our target have the flag?
		C_BaseEntity *pEnt = pPlayer->GetObserverTarget();
		if (pEnt && pEnt->IsPlayer())
		{
			C_TFPlayer *pTarget = static_cast<C_TFPlayer*>(pEnt);
			if (pTarget->HasTheFlag())
			{
				CCaptureFlag *pPlayerFlag = NULL;
				if (pTarget && pTarget->HasItem() && (pTarget->GetItem()->GetItemID() == TF_ITEM_CAPTURE_FLAG))
				{
					pPlayerFlag = dynamic_cast<CCaptureFlag*>(pTarget->GetItem());
				}

				bSpecCarriedImage = true;
				if (pPlayerFlag->GetTeamNumber() == TF_TEAM_BLUE)
				{
					if (m_pSpecCarriedImage)
					{
						m_pSpecCarriedImage->SetImage("../hud/objectives_flagpanel_carried_blue");
					}
				}
				else if (pPlayerFlag->GetTeamNumber() == TF_TEAM_RED)
				{
					if (m_pSpecCarriedImage)
					{
						m_pSpecCarriedImage->SetImage("../hud/objectives_flagpanel_carried_red");
					}
				}
				else if (pPlayerFlag->GetTeamNumber() == FO_TEAM_GREEN)
				{
					if (m_pSpecCarriedImage)
					{
						m_pSpecCarriedImage->SetImage("../hud/objectives_flagpanel_carried_green");
					}
				}
			}
		}
	}

	if (bSpecCarriedImage)
	{
		if (m_pSpecCarriedImage && !m_pSpecCarriedImage->IsVisible())
		{
			m_pSpecCarriedImage->SetVisible(true);
		}
	}
	else
	{
		if (m_pSpecCarriedImage && m_pSpecCarriedImage->IsVisible())
		{
			m_pSpecCarriedImage->SetVisible(false);
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CFOHud3FlagObjectives::UpdateStatus(void)
{
	C_TFPlayer *pLocalPlayer = ToTFPlayer(C_BasePlayer::GetLocalPlayer());

	// are we carrying a flag?
	CCaptureFlag *pPlayerFlag = NULL;
	if (pLocalPlayer && pLocalPlayer->HasItem() && (pLocalPlayer->GetItem()->GetItemID() == TF_ITEM_CAPTURE_FLAG))
	{
		pPlayerFlag = dynamic_cast<CCaptureFlag*>(pLocalPlayer->GetItem());
	}

	if (pPlayerFlag)
	{
		m_bCarryingFlag = true;

		// make sure the panels are on, set the initial alpha values, 
		// set the color of the flag we're carrying, and start the animations
		if (m_pCarriedImage && !m_bFlagAnimationPlayed)
		{
			m_bFlagAnimationPlayed = true;

			if (m_pBlueFlag && m_pBlueFlag->IsVisible())
			{
				m_pBlueFlag->SetVisible(false);
			}

			if (m_pRedFlag && m_pRedFlag->IsVisible())
			{
				m_pRedFlag->SetVisible(false);
			}

			if (m_pGreenFlag && m_pGreenFlag->IsVisible())
			{
				m_pGreenFlag->SetVisible(false);
			}

			if (!m_pCarriedImage->IsVisible())
			{
				m_pCarriedImage->SetVisible(true);
			}

			if (pPlayerFlag->GetTeamNumber() == TF_TEAM_BLUE)
			{
				m_pCarriedImage->SetImage("../hud/objectives_flagpanel_carried_blue");
			}
			else if (pPlayerFlag->GetTeamNumber() == TF_TEAM_RED)
			{
				m_pCarriedImage->SetImage("../hud/objectives_flagpanel_carried_red");
			}
			else if (pPlayerFlag->GetTeamNumber() == FO_TEAM_GREEN)
			{
				m_pCarriedImage->SetImage("../hud/objectives_flagpanel_carried_green");
			}

			g_pClientMode->GetViewportAnimationController()->StartAnimationSequence("FlagOutline");

			if (m_pCapturePoint)
			{
				if (!m_pCapturePoint->IsVisible())
				{
					m_pCapturePoint->SetVisible(true);
				}

				if (pLocalPlayer)
				{
					// go through all the capture zones and find ours
					for (int i = 0; i < g_CaptureZones.Count(); i++)
					{
						C_BaseEntity *pZone = ClientEntityList().GetEnt(g_CaptureZones[i]);

						if (pZone)
						{
							if (pZone->GetTeamNumber() == pLocalPlayer->GetTeamNumber())
							{
								m_pCapturePoint->SetEntity(pZone);
							}
						}
					}
				}
			}
		}
	}
	else
	{
		// were we carrying the flag?
		if (m_bCarryingFlag)
		{
			m_bCarryingFlag = false;
			g_pClientMode->GetViewportAnimationController()->StartAnimationSequence("FlagOutline");
		}

		m_bFlagAnimationPlayed = false;

		if (m_pCarriedImage && m_pCarriedImage->IsVisible())
		{
			m_pCarriedImage->SetVisible(false);
		}

		if (m_pCapturePoint && m_pCapturePoint->IsVisible())
		{
			m_pCapturePoint->SetVisible(false);
		}

		if (m_pBlueFlag)
		{
			if (!m_pBlueFlag->IsVisible())
			{
				m_pBlueFlag->SetVisible(true);
			}

			m_pBlueFlag->UpdateStatus();
		}

		if (m_pRedFlag)
		{
			if (!m_pRedFlag->IsVisible())
			{
				m_pRedFlag->SetVisible(true);
			}

			m_pRedFlag->UpdateStatus();
		}

		if (m_pGreenFlag)
		{
			if (!m_pGreenFlag->IsVisible())
			{
				m_pGreenFlag->SetVisible(true);
			}

			m_pGreenFlag->UpdateStatus();
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CFOHud3FlagObjectives::FireGameEvent(IGameEvent *event)
{
	const char *eventName = event->GetName();

	if (!Q_strcmp(eventName, "flagstatus_update"))
	{
		UpdateStatus();
	}
}

// 6TEAM BELOW

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
CFOHud6FlagObjectives::CFOHud6FlagObjectives(Panel *parent, const char *name) : EditablePanel(parent, name)
{
	m_pCarriedImage = NULL;
	m_pPlayingTo = NULL;
	m_bFlagAnimationPlayed = false;
	m_bCarryingFlag = false;
	m_pSpecCarriedImage = NULL;

	m_pRedTimer = NULL;
	m_pRedTimerShadow = NULL;
	m_pBlueTimer = NULL;
	m_pBlueTimerShadow = NULL;
	m_pGreenTimer = NULL;
	m_pGreenTimerShadow = NULL;
	m_pYellowTimer = NULL;
	m_pYellowTimerShadow = NULL;
	m_pPurpleTimer = NULL;
	m_pPurpleTimerShadow = NULL;
	m_pPinkTimer = NULL;
	m_pPinkTimerShadow = NULL;

	vgui::ivgui()->AddTickSignal(GetVPanel());

	ListenForGameEvent("flagstatus_update");
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool CFOHud6FlagObjectives::IsVisible(void)
{
	if (IsTakingAFreezecamScreenshot())
		return false;

	return BaseClass::IsVisible();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CFOHud6FlagObjectives::ApplySchemeSettings(IScheme *pScheme)
{
	BaseClass::ApplySchemeSettings(pScheme);

	// load control settings...
	LoadControlSettings("resource/UI/HudObjective6FlagPanel.res");

	m_pCarriedImage = dynamic_cast<CTFImagePanel *>(FindChildByName("CarriedImage"));
	m_pPlayingTo = dynamic_cast<CTFLabel *>(FindChildByName("PlayingTo"));
	m_pPlayingToBG = dynamic_cast<CTFImagePanel *>(FindChildByName("PlayingToBG"));

	m_pRedFlag = dynamic_cast<CTFFlagStatus *>(FindChildByName("RedFlag"));
	m_pBlueFlag = dynamic_cast<CTFFlagStatus *>(FindChildByName("BlueFlag"));
	m_pGreenFlag = dynamic_cast<CTFFlagStatus *>(FindChildByName("GreenFlag"));
	m_pYellowFlag = dynamic_cast<CTFFlagStatus *>(FindChildByName("YellowFlag"));
	m_pPurpleFlag = dynamic_cast<CTFFlagStatus *>(FindChildByName("PurpleFlag"));
	m_pPinkFlag = dynamic_cast<CTFFlagStatus *>(FindChildByName("PinkFlag"));

	m_pCapturePoint = dynamic_cast<CTFArrowPanel *>(FindChildByName("CaptureFlag"));

	m_pSpecCarriedImage = dynamic_cast<ImagePanel *>(FindChildByName("SpecCarriedImage"));

	m_pRedTimer = dynamic_cast<CTFLabel *>(FindChildByName("RedTimer"));
	m_pRedTimerShadow = dynamic_cast<CTFLabel *>(FindChildByName("RedTimerShadow"));
	m_pBlueTimer = dynamic_cast<CTFLabel *>(FindChildByName("BlueTimer"));
	m_pBlueTimerShadow = dynamic_cast<CTFLabel *>(FindChildByName("BlueTimerShadow"));
	m_pGreenTimer = dynamic_cast<CTFLabel *>(FindChildByName("GreenTimer"));
	m_pGreenTimerShadow = dynamic_cast<CTFLabel *>(FindChildByName("GreenTimerShadow"));
	m_pYellowTimer = dynamic_cast<CTFLabel *>(FindChildByName("YellowTimer"));
	m_pYellowTimerShadow = dynamic_cast<CTFLabel *>(FindChildByName("YellowTimerShadow"));
	m_pPurpleTimer = dynamic_cast<CTFLabel *>(FindChildByName("PurpleTimer"));
	m_pPurpleTimerShadow = dynamic_cast<CTFLabel *>(FindChildByName("PurpleTimerShadow"));
	m_pPinkTimer = dynamic_cast<CTFLabel *>(FindChildByName("PinkTimer"));
	m_pPinkTimerShadow = dynamic_cast<CTFLabel *>(FindChildByName("PinkTimerShadow"));

	// outline is always on, so we need to init the alpha to 0
	CTFImagePanel *pOutline = dynamic_cast<CTFImagePanel *>(FindChildByName("OutlineImage"));
	if (pOutline)
	{
		pOutline->SetAlpha(0);
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CFOHud6FlagObjectives::Reset()
{
	g_pClientMode->GetViewportAnimationController()->StartAnimationSequence("FlagOutlineHide");

	if (m_pCarriedImage && m_pCarriedImage->IsVisible())
	{
		m_pCarriedImage->SetVisible(false);
	}

	if (m_pBlueFlag && !m_pBlueFlag->IsVisible())
	{
		m_pBlueFlag->SetVisible(true);
	}

	if (m_pRedFlag && !m_pRedFlag->IsVisible())
	{
		m_pRedFlag->SetVisible(true);
	}

	if (m_pGreenFlag && !m_pGreenFlag->IsVisible())
	{
		m_pGreenFlag->SetVisible(true);
	}

	if (m_pYellowFlag && !m_pYellowFlag->IsVisible())
	{
		m_pYellowFlag->SetVisible(true);
	}

	if (m_pPurpleFlag && !m_pPurpleFlag->IsVisible())
	{
		m_pPurpleFlag->SetVisible(true);
	}

	if (m_pPinkFlag && !m_pPinkFlag->IsVisible())
	{
		m_pPinkFlag->SetVisible(true);
	}

	if (m_pSpecCarriedImage && m_pSpecCarriedImage->IsVisible())
	{
		m_pSpecCarriedImage->SetVisible(false);
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CFOHud6FlagObjectives::SetPlayingToLabelVisible(bool bVisible)
{
	if (m_pPlayingTo && m_pPlayingToBG)
	{
		if (m_pPlayingTo->IsVisible() != bVisible)
		{
			m_pPlayingTo->SetVisible(bVisible);
		}

		if (m_pPlayingToBG->IsVisible() != bVisible)
		{
			m_pPlayingToBG->SetVisible(bVisible);
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CFOHud6FlagObjectives::OnTick()
{
	// iterate through the flags to set their position in our HUD
	for (int i = 0; i < g_Flags.Count(); i++)
	{
		CCaptureFlag *pFlag = dynamic_cast<CCaptureFlag*>(ClientEntityList().GetEnt(g_Flags[i]));

		if (pFlag)
		{
			if (!pFlag->IsDisabled())
			{
				if (m_pRedFlag && pFlag->GetTeamNumber() == TF_TEAM_RED)
				{
					m_pRedFlag->SetEntity(pFlag);
				}
				else if (m_pBlueFlag && pFlag->GetTeamNumber() == TF_TEAM_BLUE)
				{
					m_pBlueFlag->SetEntity(pFlag);
				}
				else if (m_pGreenFlag && pFlag->GetTeamNumber() == FO_TEAM_GREEN)
				{
					m_pGreenFlag->SetEntity(pFlag);
				}
				else if (m_pYellowFlag && pFlag->GetTeamNumber() == FO_TEAM_YELLOW)
				{
					m_pYellowFlag->SetEntity(pFlag);
				}
				else if (m_pPurpleFlag && pFlag->GetTeamNumber() == FO_TEAM_PURPLE)
				{
					m_pPurpleFlag->SetEntity(pFlag);
				}
				else if (m_pPinkFlag && pFlag->GetTeamNumber() == FO_TEAM_PINK)
				{
					m_pPinkFlag->SetEntity(pFlag);
				}
			}
		}
		else
		{
			// this isn't a valid index for a flag
			g_Flags.Remove(i);
		}
	}

	SetDialogVariable("redtimer", fo_ctp_red_timer.GetInt());
	SetDialogVariable("bluetimer", fo_ctp_blue_timer.GetInt());
	SetDialogVariable("greentimer", fo_ctp_green_timer.GetInt());
	SetDialogVariable("yellowtimer", fo_ctp_yellow_timer.GetInt());
	SetDialogVariable("purpletimer", fo_ctp_purple_timer.GetInt());
	SetDialogVariable("pinktimer", fo_ctp_pink_timer.GetInt());


	if (TFGameRules() && TFGameRules()->GetGameType() == FO_GAMETYPE_CTP)
	{
		if (m_pBlueFlag)
			m_pBlueFlag->SetVisible(false);
		if (m_pRedFlag)
			m_pRedFlag->SetVisible(false);
		if (m_pGreenFlag)
			m_pGreenFlag->SetVisible(false);
		if (m_pYellowFlag)
			m_pYellowFlag->SetVisible(false);
		if (m_pPurpleFlag)
			m_pPurpleFlag->SetVisible(false);
		if (m_pPinkFlag)
			m_pPinkFlag->SetVisible(false);

		if (m_pRedTimer && m_pRedTimerShadow)
		{
			if (fo_ctp_red_timer.GetInt() == 21 || fo_ctp_red_timer.GetInt() == -1)
			{
				m_pRedTimer->SetVisible(false);
				m_pRedTimerShadow->SetVisible(false);
			}
			else
			{
				m_pRedTimer->SetVisible(true);
				m_pRedTimerShadow->SetVisible(true);
			}
		}

		if (m_pBlueTimer && m_pBlueTimerShadow)
		{
			if (fo_ctp_blue_timer.GetInt() == 21 || fo_ctp_blue_timer.GetInt() == -1)
			{
				m_pBlueTimer->SetVisible(false);
				m_pBlueTimerShadow->SetVisible(false);
			}
			else
			{
				m_pBlueTimer->SetVisible(true);
				m_pBlueTimerShadow->SetVisible(true);
			}
		}

		if (m_pGreenTimer && m_pGreenTimerShadow)
		{
			if (fo_ctp_green_timer.GetInt() == 21 || fo_ctp_green_timer.GetInt() == -1)
			{
				m_pGreenTimer->SetVisible(false);
				m_pGreenTimerShadow->SetVisible(false);
			}
			else
			{
				m_pGreenTimer->SetVisible(true);
				m_pGreenTimerShadow->SetVisible(true);
			}
		}

		if (m_pYellowTimer && m_pYellowTimerShadow)
		{
			if (fo_ctp_yellow_timer.GetInt() == 21 || fo_ctp_yellow_timer.GetInt() == -1)
			{
				m_pYellowTimer->SetVisible(false);
				m_pYellowTimerShadow->SetVisible(false);
			}
			else
			{
				m_pYellowTimer->SetVisible(true);
				m_pYellowTimerShadow->SetVisible(true);
			}
		}

		if (m_pPurpleTimer && m_pPurpleTimerShadow)
		{
			if (fo_ctp_purple_timer.GetInt() == 21 || fo_ctp_purple_timer.GetInt() == -1)
			{
				m_pPurpleTimer->SetVisible(false);
				m_pPurpleTimerShadow->SetVisible(false);
			}
			else
			{
				m_pPurpleTimer->SetVisible(true);
				m_pPurpleTimerShadow->SetVisible(true);
			}
		}

		if (m_pPinkTimer && m_pPinkTimerShadow)
		{
			if (fo_ctp_pink_timer.GetInt() == 21 || fo_ctp_pink_timer.GetInt() == -1)
			{
				m_pPinkTimer->SetVisible(false);
				m_pPinkTimerShadow->SetVisible(false);
			}
			else
			{
				m_pPinkTimer->SetVisible(true);
				m_pPinkTimerShadow->SetVisible(true);
			}
		}
	}
	else
	{
		if (m_pBlueFlag && !m_pCarriedImage->IsVisible())
			m_pBlueFlag->SetVisible(true);
		if (m_pRedFlag && !m_pCarriedImage->IsVisible())
			m_pRedFlag->SetVisible(true);
		if (m_pGreenFlag && !m_pCarriedImage->IsVisible())
			m_pGreenFlag->SetVisible(true);
		if (m_pYellowFlag && !m_pCarriedImage->IsVisible())
			m_pYellowFlag->SetVisible(true);
		if (m_pPurpleFlag && !m_pCarriedImage->IsVisible())
			m_pPurpleFlag->SetVisible(true);
		if (m_pPinkFlag && !m_pCarriedImage->IsVisible())
			m_pPinkFlag->SetVisible(true);

		if (m_pRedTimer)
			m_pRedTimer->SetVisible(false);
		if (m_pRedTimerShadow)
			m_pRedTimerShadow->SetVisible(false);

		if (m_pBlueTimer)
			m_pBlueTimer->SetVisible(false);
		if (m_pBlueTimerShadow)
			m_pBlueTimerShadow->SetVisible(false);

		if (m_pGreenTimer)
			m_pGreenTimer->SetVisible(false);
		if (m_pGreenTimerShadow)
			m_pGreenTimerShadow->SetVisible(false);

		if (m_pYellowTimer)
			m_pYellowTimer->SetVisible(false);
		if (m_pYellowTimerShadow)
			m_pYellowTimerShadow->SetVisible(false);

		if (m_pPurpleTimer)
			m_pPurpleTimer->SetVisible(false);
		if (m_pPurpleTimerShadow)
			m_pPurpleTimerShadow->SetVisible(false);

		if (m_pPinkTimer)
			m_pPinkTimer->SetVisible(false);
		if (m_pPinkTimerShadow)
			m_pPinkTimerShadow->SetVisible(false);
	}

	// are we playing captures for rounds?
	if (tf_flag_caps_per_round.GetInt() > 0)
	{
		C_TFTeam *pTeam = GetGlobalTFTeam(TF_TEAM_BLUE);
		if (pTeam)
		{
			if (TFGameRules() && TFGameRules()->GetGameType() == FO_GAMETYPE_CTP)
			{
				SetDialogVariable("bluescore", fo_ctp_blue_score.GetInt());
			}
			else
			{
				SetDialogVariable("bluescore", pTeam->GetFlagCaptures());
			}
		}

		pTeam = GetGlobalTFTeam(TF_TEAM_RED);
		if (pTeam)
		{
			if (TFGameRules() && TFGameRules()->GetGameType() == FO_GAMETYPE_CTP)
			{
				SetDialogVariable("redscore", fo_ctp_red_score.GetInt());
			}
			else
			{
				SetDialogVariable("redscore", pTeam->GetFlagCaptures());
			}
		}

		pTeam = GetGlobalTFTeam(FO_TEAM_GREEN);
		if (pTeam)
		{
			if (TFGameRules() && TFGameRules()->GetGameType() == FO_GAMETYPE_CTP)
			{
				SetDialogVariable("greenscore", fo_ctp_green_score.GetInt());
			}
			else
			{
				SetDialogVariable("greenscore", pTeam->GetFlagCaptures());
			}
		}

		pTeam = GetGlobalTFTeam(FO_TEAM_YELLOW);
		if (pTeam)
		{
			if (TFGameRules() && TFGameRules()->GetGameType() == FO_GAMETYPE_CTP)
			{
				SetDialogVariable("yellowscore", fo_ctp_yellow_score.GetInt());
			}
			else
			{
				SetDialogVariable("yellowscore", pTeam->GetFlagCaptures());
			}
		}

		pTeam = GetGlobalTFTeam(FO_TEAM_PURPLE);
		if (pTeam)
		{
			if (TFGameRules() && TFGameRules()->GetGameType() == FO_GAMETYPE_CTP)
			{
				SetDialogVariable("purplescore", fo_ctp_purple_score.GetInt());
			}
			else
			{
				SetDialogVariable("purplescore", pTeam->GetFlagCaptures());
			}
		}

		pTeam = GetGlobalTFTeam(FO_TEAM_PINK);
		if (pTeam)
		{
			if (TFGameRules() && TFGameRules()->GetGameType() == FO_GAMETYPE_CTP)
			{
				SetDialogVariable("pinkscore", fo_ctp_pink_score.GetInt());
			}
			else
			{
				SetDialogVariable("pinkscore", pTeam->GetFlagCaptures());
			}
		}

		SetPlayingToLabelVisible(true);
		if (TFGameRules() && TFGameRules()->GetGameType() == FO_GAMETYPE_CTP)
		{
			SetDialogVariable("rounds", fo_ctp_scorelimit.GetInt());
		}
		else
		{
			SetDialogVariable("rounds", tf_flag_caps_per_round.GetInt());
		}
	}
	else // we're just playing straight score
	{
		C_TFTeam *pTeam = GetGlobalTFTeam(TF_TEAM_BLUE);
		if (pTeam)
		{
			if (TFGameRules() && TFGameRules()->GetGameType() == FO_GAMETYPE_CTP)
			{
				SetDialogVariable("bluescore", fo_ctp_blue_score.GetInt());
			}
			else
			{
				SetDialogVariable("bluescore", pTeam->Get_Score());
			}
		}

		pTeam = GetGlobalTFTeam(TF_TEAM_RED);
		if (pTeam)
		{
			if (TFGameRules() && TFGameRules()->GetGameType() == FO_GAMETYPE_CTP)
			{
				SetDialogVariable("redscore", fo_ctp_red_score.GetInt());
			}
			else
			{
				SetDialogVariable("redscore", pTeam->Get_Score());
			}
		}

		pTeam = GetGlobalTFTeam(FO_TEAM_GREEN);
		if (pTeam)
		{
			if (TFGameRules() && TFGameRules()->GetGameType() == FO_GAMETYPE_CTP)
			{
				SetDialogVariable("greenscore", fo_ctp_green_score.GetInt());
			}
			else
			{
				SetDialogVariable("greenscore", pTeam->Get_Score());
			}
		}

		pTeam = GetGlobalTFTeam(FO_TEAM_YELLOW);
		if (pTeam)
		{
			if (TFGameRules() && TFGameRules()->GetGameType() == FO_GAMETYPE_CTP)
			{
				SetDialogVariable("yellowscore", fo_ctp_yellow_score.GetInt());
			}
			else
			{
				SetDialogVariable("yellowscore", pTeam->Get_Score());
			}
		}

		pTeam = GetGlobalTFTeam(FO_TEAM_PURPLE);
		if (pTeam)
		{
			if (TFGameRules() && TFGameRules()->GetGameType() == FO_GAMETYPE_CTP)
			{
				SetDialogVariable("purplescore", fo_ctp_purple_score.GetInt());
			}
			else
			{
				SetDialogVariable("purplescore", pTeam->Get_Score());
			}
		}

		pTeam = GetGlobalTFTeam(FO_TEAM_PINK);
		if (pTeam)
		{
			if (TFGameRules() && TFGameRules()->GetGameType() == FO_GAMETYPE_CTP)
			{
				SetDialogVariable("pinkscore", fo_ctp_pink_score.GetInt());
			}
			else
			{
				SetDialogVariable("pinkscore", pTeam->Get_Score());
			}
		}

		SetPlayingToLabelVisible(false);
	}

	// check the local player to see if they're spectating, OBS_MODE_IN_EYE, and the target entity is carrying the flag
	bool bSpecCarriedImage = false;
	C_TFPlayer *pPlayer = C_TFPlayer::GetLocalTFPlayer();
	if (pPlayer && (pPlayer->GetObserverMode() == OBS_MODE_IN_EYE))
	{
		// does our target have the flag?
		C_BaseEntity *pEnt = pPlayer->GetObserverTarget();
		if (pEnt && pEnt->IsPlayer())
		{
			C_TFPlayer *pTarget = static_cast<C_TFPlayer*>(pEnt);
			if (pTarget->HasTheFlag())
			{
				CCaptureFlag *pPlayerFlag = NULL;
				if (pTarget && pTarget->HasItem() && (pTarget->GetItem()->GetItemID() == TF_ITEM_CAPTURE_FLAG))
				{
					pPlayerFlag = dynamic_cast<CCaptureFlag*>(pTarget->GetItem());
				}

				bSpecCarriedImage = true;
				if (pPlayerFlag->GetTeamNumber() == TF_TEAM_BLUE)
				{
					if (m_pSpecCarriedImage)
					{
						m_pSpecCarriedImage->SetImage("../hud/objectives_flagpanel_carried_blue");
					}
				}
				else if (pPlayerFlag->GetTeamNumber() == TF_TEAM_RED)
				{
					if (m_pSpecCarriedImage)
					{
						m_pSpecCarriedImage->SetImage("../hud/objectives_flagpanel_carried_red");
					}
				}
				else if (pPlayerFlag->GetTeamNumber() == FO_TEAM_GREEN)
				{
					if (m_pSpecCarriedImage)
					{
						m_pSpecCarriedImage->SetImage("../hud/objectives_flagpanel_carried_green");
					}
				}
				else if (pPlayerFlag->GetTeamNumber() == FO_TEAM_YELLOW)
				{
					if (m_pSpecCarriedImage)
					{
						m_pSpecCarriedImage->SetImage("../hud/objectives_flagpanel_carried_yellow");
					}
				}
				else if (pPlayerFlag->GetTeamNumber() == FO_TEAM_PURPLE)
				{
					if (m_pSpecCarriedImage)
					{
						m_pSpecCarriedImage->SetImage("../hud/objectives_flagpanel_carried_purple");
					}
				}
				else if (pPlayerFlag->GetTeamNumber() == FO_TEAM_PINK)
				{
					if (m_pSpecCarriedImage)
					{
						m_pSpecCarriedImage->SetImage("../hud/objectives_flagpanel_carried_pink");
					}
				}
			}
		}
	}

	if (bSpecCarriedImage)
	{
		if (m_pSpecCarriedImage && !m_pSpecCarriedImage->IsVisible())
		{
			m_pSpecCarriedImage->SetVisible(true);
		}
	}
	else
	{
		if (m_pSpecCarriedImage && m_pSpecCarriedImage->IsVisible())
		{
			m_pSpecCarriedImage->SetVisible(false);
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CFOHud6FlagObjectives::UpdateStatus(void)
{
	C_TFPlayer *pLocalPlayer = ToTFPlayer(C_BasePlayer::GetLocalPlayer());

	// are we carrying a flag?
	CCaptureFlag *pPlayerFlag = NULL;
	if (pLocalPlayer && pLocalPlayer->HasItem() && (pLocalPlayer->GetItem()->GetItemID() == TF_ITEM_CAPTURE_FLAG))
	{
		pPlayerFlag = dynamic_cast<CCaptureFlag*>(pLocalPlayer->GetItem());
	}

	if (pPlayerFlag)
	{
		m_bCarryingFlag = true;

		// make sure the panels are on, set the initial alpha values, 
		// set the color of the flag we're carrying, and start the animations
		if (m_pCarriedImage && !m_bFlagAnimationPlayed)
		{
			m_bFlagAnimationPlayed = true;

			if (m_pBlueFlag && m_pBlueFlag->IsVisible())
			{
				m_pBlueFlag->SetVisible(false);
			}

			if (m_pRedFlag && m_pRedFlag->IsVisible())
			{
				m_pRedFlag->SetVisible(false);
			}

			if (m_pGreenFlag && m_pGreenFlag->IsVisible())
			{
				m_pGreenFlag->SetVisible(false);
			}

			if (m_pYellowFlag && m_pYellowFlag->IsVisible())
			{
				m_pYellowFlag->SetVisible(false);
			}

			if (m_pPurpleFlag && m_pPurpleFlag->IsVisible())
			{
				m_pPurpleFlag->SetVisible(false);
			}

			if (m_pPinkFlag && m_pPinkFlag->IsVisible())
			{
				m_pPinkFlag->SetVisible(false);
			}

			if (!m_pCarriedImage->IsVisible())
			{
				m_pCarriedImage->SetVisible(true);
			}

			if (pPlayerFlag->GetTeamNumber() == TF_TEAM_BLUE)
			{
				m_pCarriedImage->SetImage("../hud/objectives_flagpanel_carried_blue");
			}
			else if (pPlayerFlag->GetTeamNumber() == TF_TEAM_RED)
			{
				m_pCarriedImage->SetImage("../hud/objectives_flagpanel_carried_red");
			}
			else if (pPlayerFlag->GetTeamNumber() == FO_TEAM_GREEN)
			{
				m_pCarriedImage->SetImage("../hud/objectives_flagpanel_carried_green");
			}
			else if (pPlayerFlag->GetTeamNumber() == FO_TEAM_YELLOW)
			{
				m_pCarriedImage->SetImage("../hud/objectives_flagpanel_carried_yellow");
			}
			else if (pPlayerFlag->GetTeamNumber() == FO_TEAM_PURPLE)
			{
				m_pCarriedImage->SetImage("../hud/objectives_flagpanel_carried_purple");
			}
			else if (pPlayerFlag->GetTeamNumber() == FO_TEAM_PINK)
			{
				m_pCarriedImage->SetImage("../hud/objectives_flagpanel_carried_pink");
			}

			g_pClientMode->GetViewportAnimationController()->StartAnimationSequence("FlagOutline");

			if (m_pCapturePoint)
			{
				if (!m_pCapturePoint->IsVisible())
				{
					m_pCapturePoint->SetVisible(true);
				}

				if (pLocalPlayer)
				{
					// go through all the capture zones and find ours
					for (int i = 0; i < g_CaptureZones.Count(); i++)
					{
						C_BaseEntity *pZone = ClientEntityList().GetEnt(g_CaptureZones[i]);

						if (pZone)
						{
							if (pZone->GetTeamNumber() == pLocalPlayer->GetTeamNumber())
							{
								m_pCapturePoint->SetEntity(pZone);
							}
						}
					}
				}
			}
		}
	}
	else
	{
		// were we carrying the flag?
		if (m_bCarryingFlag)
		{
			m_bCarryingFlag = false;
			g_pClientMode->GetViewportAnimationController()->StartAnimationSequence("FlagOutline");
		}

		m_bFlagAnimationPlayed = false;

		if (m_pCarriedImage && m_pCarriedImage->IsVisible())
		{
			m_pCarriedImage->SetVisible(false);
		}

		if (m_pCapturePoint && m_pCapturePoint->IsVisible())
		{
			m_pCapturePoint->SetVisible(false);
		}

		if (m_pBlueFlag)
		{
			if (!m_pBlueFlag->IsVisible())
			{
				m_pBlueFlag->SetVisible(true);
			}

			m_pBlueFlag->UpdateStatus();
		}

		if (m_pRedFlag)
		{
			if (!m_pRedFlag->IsVisible())
			{
				m_pRedFlag->SetVisible(true);
			}

			m_pRedFlag->UpdateStatus();
		}

		if (m_pGreenFlag)
		{
			if (!m_pGreenFlag->IsVisible())
			{
				m_pGreenFlag->SetVisible(true);
			}

			m_pGreenFlag->UpdateStatus();
		}

		if (m_pYellowFlag)
		{
			if (!m_pYellowFlag->IsVisible())
			{
				m_pYellowFlag->SetVisible(true);
			}

			m_pYellowFlag->UpdateStatus();
		}

		if (m_pPurpleFlag)
		{
			if (!m_pPurpleFlag->IsVisible())
			{
				m_pPurpleFlag->SetVisible(true);
			}

			m_pPurpleFlag->UpdateStatus();
		}

		if (m_pPinkFlag)
		{
			if (!m_pPinkFlag->IsVisible())
			{
				m_pPinkFlag->SetVisible(true);
			}

			m_pPinkFlag->UpdateStatus();
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CFOHud6FlagObjectives::FireGameEvent(IGameEvent *event)
{
	const char *eventName = event->GetName();

	if (!Q_strcmp(eventName, "flagstatus_update"))
	{
		UpdateStatus();
	}
}
