//========= Copyright © 1996-2006, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#ifndef FO_HUD_DOMINATION_SCORE_STATUS_H
#define FO_HUD_DOMINATION_SCORE_STATUS_H
#ifdef _WIN32
#pragma once
#endif

#define TF_MAX_GRENADES			4
#define TF_MAX_FILENAME_LENGTH	128

//-----------------------------------------------------------------------------
// Purpose:  Displays weapon ammo data
//-----------------------------------------------------------------------------
class CFOHudDominationScore : public CHudElement, public vgui::EditablePanel
{
	DECLARE_CLASS_SIMPLE(CFOHudDominationScore, vgui::EditablePanel);

public:

	CFOHudDominationScore(const char *pElementName);

	virtual void ApplySchemeSettings(vgui::IScheme *pScheme);
	virtual void Reset();

	virtual void LevelInit();
	virtual void Init();
	virtual void FireGameEvent(IGameEvent * event);

	virtual bool ShouldDraw(void);

protected:

	virtual void OnThink();

private:

	void UpdateTeamPanels(bool bRed, bool bBlue, bool bGreen, bool bYellow, bool bPurple, bool bPink, int nPosition);

private:

	bool							m_bReadyToShift;

	float							m_flNextThink;

	CTFLabel						*m_pRedScore;
	CTFLabel						*m_pRedScoreShadow;
	CTFImagePanel					*m_pRedBG;

	CTFLabel						*m_pBlueScore;
	CTFLabel						*m_pBlueScoreShadow;
	CTFImagePanel					*m_pBlueBG;

	CTFLabel						*m_pGreenScore;
	CTFLabel						*m_pGreenScoreShadow;
	CTFImagePanel					*m_pGreenBG;

	CTFLabel						*m_pYellowScore;
	CTFLabel						*m_pYellowScoreShadow;
	CTFImagePanel					*m_pYellowBG;

	CTFLabel						*m_pPurpleScore;
	CTFLabel						*m_pPurpleScoreShadow;
	CTFImagePanel					*m_pPurpleBG;

	CTFLabel						*m_pPinkScore;
	CTFLabel						*m_pPinkScoreShadow;
	CTFImagePanel					*m_pPinkBG;

	CTFLabel						*m_pPlayingTo;
	CTFImagePanel					*m_pPlayingToBG;
};

#endif	// TF_HUD_AMMOSTATUS_H