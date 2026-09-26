//========= Copyright © 1996-2007, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#ifndef TF_HUD_FLAGSTATUS_H
#define TF_HUD_FLAGSTATUS_H
#ifdef _WIN32
#pragma once
#endif

#include "entity_capture_flag.h"
#include "tf_controls.h"
#include "tf_imagepanel.h"
#include "GameEventListener.h"

//-----------------------------------------------------------------------------
// Purpose:  Draws the rotated arrow panels
//-----------------------------------------------------------------------------
class CTFArrowPanel : public CTFImagePanel
{
public:
	DECLARE_CLASS_SIMPLE( CTFArrowPanel, CTFImagePanel );

	CTFArrowPanel( vgui::Panel *parent, const char *name );
	virtual void Paint();
	virtual bool IsVisible( void );
	void SetEntity( EHANDLE hEntity ){ m_hEntity = hEntity; }
	float GetAngleRotation( void );

private:

	EHANDLE				m_hEntity;	

	CMaterialReference	m_RedMaterial;
	CMaterialReference	m_BlueMaterial;
	CMaterialReference	m_GreenMaterial;
	CMaterialReference	m_NeutralMaterial;

	CMaterialReference	m_RedSmallMaterial;
	CMaterialReference	m_BlueSmallMaterial;
	CMaterialReference	m_GreenSmallMaterial;
	CMaterialReference	m_YellowSmallMaterial;
	CMaterialReference	m_PurpleSmallMaterial;
	CMaterialReference	m_PinkSmallMaterial;

	CMaterialReference	m_RedSmallMaterialNoArrow;
	CMaterialReference	m_BlueSmallMaterialNoArrow;
	CMaterialReference	m_GreenSmallMaterialNoArrow;
	CMaterialReference	m_YellowSmallMaterialNoArrow;
	CMaterialReference	m_PurpleSmallMaterialNoArrow;
	CMaterialReference	m_PinkSmallMaterialNoArrow;

	CMaterialReference	m_RedMaterialNoArrow;
	CMaterialReference	m_BlueMaterialNoArrow;
	CMaterialReference	m_GreenMaterialNoArrow;
};

//-----------------------------------------------------------------------------
// Purpose:  
//-----------------------------------------------------------------------------
class CTFFlagStatus : public vgui::EditablePanel
{
public:
	DECLARE_CLASS_SIMPLE( CTFFlagStatus, vgui::EditablePanel );

	CTFFlagStatus( vgui::Panel *parent, const char *name );

	virtual void ApplySchemeSettings( vgui::IScheme *pScheme );
	virtual bool IsVisible( void );
	void UpdateStatus( void );

	void SetEntity( EHANDLE hEntity )
	{ 
		m_hEntity = hEntity;

		if ( m_pArrow )
		{
			m_pArrow->SetEntity( hEntity );
		}
	}

private:

	EHANDLE			m_hEntity;

	CTFArrowPanel	*m_pArrow;
	CTFImagePanel	*m_pStatusIcon;
	CTFImagePanel	*m_pBriefcase;
	CTFImagePanel	*m_p6StatusIcon;
	CTFImagePanel	*m_p6Briefcase;
};

//-----------------------------------------------------------------------------
// Purpose:  
//-----------------------------------------------------------------------------
class CTFHudFlagObjectives : public vgui::EditablePanel, public CGameEventListener
{
	DECLARE_CLASS_SIMPLE( CTFHudFlagObjectives, vgui::EditablePanel );

public:

	CTFHudFlagObjectives( vgui::Panel *parent, const char *name );

	virtual void ApplySchemeSettings( vgui::IScheme *pScheme );
	virtual bool IsVisible( void );
	virtual void Reset();
	void OnTick();

public: // IGameEventListener:
	virtual void FireGameEvent( IGameEvent *event );

private:
	
	void UpdateStatus( void );
	void SetPlayingToLabelVisible( bool bVisible );

private:

	CTFImagePanel			*m_pCarriedImage;

	CTFLabel				*m_pPlayingTo;
	CTFImagePanel			*m_pPlayingToBG;

	CTFFlagStatus			*m_pRedFlag;
	CTFFlagStatus			*m_pBlueFlag;
	CTFArrowPanel			*m_pCapturePoint;

	CTFLabel						*m_pRedTimer;
	CTFLabel						*m_pRedTimerShadow;
	CTFLabel						*m_pBlueTimer;
	CTFLabel						*m_pBlueTimerShadow;

	bool					m_bFlagAnimationPlayed;
	bool					m_bCarryingFlag;

	vgui::ImagePanel		*m_pSpecCarriedImage;
};

// 3TEAM BELOW

//-----------------------------------------------------------------------------
// Purpose:  
//-----------------------------------------------------------------------------
class CFOHud3FlagObjectives : public vgui::EditablePanel, public CGameEventListener
{
	DECLARE_CLASS_SIMPLE(CFOHud3FlagObjectives, vgui::EditablePanel);

public:

	CFOHud3FlagObjectives(vgui::Panel *parent, const char *name);

	virtual void ApplySchemeSettings(vgui::IScheme *pScheme);
	virtual bool IsVisible(void);
	virtual void Reset();
	void OnTick();

public: // IGameEventListener:
	virtual void FireGameEvent(IGameEvent *event);

private:

	void UpdateStatus(void);
	void SetPlayingToLabelVisible(bool bVisible);

private:

	CTFImagePanel			*m_pCarriedImage;

	CTFLabel				*m_pPlayingTo;
	CTFImagePanel			*m_pPlayingToBG;

	CTFFlagStatus			*m_pRedFlag;
	CTFFlagStatus			*m_pBlueFlag;
	CTFFlagStatus			*m_pGreenFlag;
	CTFArrowPanel			*m_pCapturePoint;

	CTFLabel						*m_pRedTimer;
	CTFLabel						*m_pRedTimerShadow;
	CTFLabel						*m_pBlueTimer;
	CTFLabel						*m_pBlueTimerShadow;
	CTFLabel						*m_pGreenTimer;
	CTFLabel						*m_pGreenTimerShadow;

	CTFLabel						*m_pGreenScore;
	CTFLabel						*m_pGreenScoreShadow;

	CTFLabel						*m_pGreenScoreCTP;
	CTFLabel						*m_pGreenScoreShadowCTP;

	bool					m_bFlagAnimationPlayed;
	bool					m_bCarryingFlag;

	vgui::ImagePanel		*m_pSpecCarriedImage;
};

// 6TEAM BELOW

//-----------------------------------------------------------------------------
// Purpose:  
//-----------------------------------------------------------------------------
class CFOHud6FlagObjectives : public vgui::EditablePanel, public CGameEventListener
{
	DECLARE_CLASS_SIMPLE(CFOHud6FlagObjectives, vgui::EditablePanel);

public:

	CFOHud6FlagObjectives(vgui::Panel *parent, const char *name);

	virtual void ApplySchemeSettings(vgui::IScheme *pScheme);
	virtual bool IsVisible(void);
	virtual void Reset();
	void OnTick();

public: // IGameEventListener:
	virtual void FireGameEvent(IGameEvent *event);

private:

	void UpdateStatus(void);
	void SetPlayingToLabelVisible(bool bVisible);

private:

	CTFImagePanel			*m_pCarriedImage;

	CTFLabel				*m_pPlayingTo;
	CTFImagePanel			*m_pPlayingToBG;

	CTFFlagStatus			*m_pRedFlag;
	CTFFlagStatus			*m_pBlueFlag;
	CTFFlagStatus			*m_pGreenFlag;
	CTFFlagStatus			*m_pYellowFlag;
	CTFFlagStatus			*m_pPurpleFlag;
	CTFFlagStatus			*m_pPinkFlag;
	CTFArrowPanel			*m_pCapturePoint;

	CTFLabel						*m_pRedTimer;
	CTFLabel						*m_pRedTimerShadow;
	CTFLabel						*m_pBlueTimer;
	CTFLabel						*m_pBlueTimerShadow;
	CTFLabel						*m_pGreenTimer;
	CTFLabel						*m_pGreenTimerShadow;
	CTFLabel						*m_pYellowTimer;
	CTFLabel						*m_pYellowTimerShadow;
	CTFLabel						*m_pPurpleTimer;
	CTFLabel						*m_pPurpleTimerShadow;
	CTFLabel						*m_pPinkTimer;
	CTFLabel						*m_pPinkTimerShadow;

	bool					m_bFlagAnimationPlayed;
	bool					m_bCarryingFlag;

	vgui::ImagePanel		*m_pSpecCarriedImage;
};

#endif	// TF_HUD_FLAGSTATUS_H
