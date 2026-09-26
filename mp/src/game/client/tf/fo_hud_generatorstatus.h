//========= Copyright © 1996-2007, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#ifndef FO_HUD_GENERATORSTATUS_H
#define FO_HUD_GENERATORSTATUS_H
#ifdef _WIN32
#pragma once
#endif

#include "tf_controls.h"
#include "tf_imagepanel.h"
#include "GameEventListener.h"

//-----------------------------------------------------------------------------
// Purpose:  
//-----------------------------------------------------------------------------
class CFOHudGeneratorObjectives : public vgui::EditablePanel, public CGameEventListener
{
	DECLARE_CLASS_SIMPLE(CFOHudGeneratorObjectives, vgui::EditablePanel );

public:

	CFOHudGeneratorObjectives( vgui::Panel *parent, const char *name );

	virtual void ApplySchemeSettings( vgui::IScheme *pScheme );
	virtual bool IsVisible( void );
	virtual void Reset();
	void OnTick();

public: // IGameEventListener:
	virtual void FireGameEvent(IGameEvent *event);

private:

	CTFLabel						*m_pPointTimer;
	CTFLabel						*m_pPointTimerShadow;
};

// 3TEAM BELOW

//-----------------------------------------------------------------------------
// Purpose:  
//-----------------------------------------------------------------------------
class CFOHud3GeneratorObjectives : public vgui::EditablePanel, public CGameEventListener
{
	DECLARE_CLASS_SIMPLE(CFOHud3GeneratorObjectives, vgui::EditablePanel);

public:

	CFOHud3GeneratorObjectives(vgui::Panel *parent, const char *name);

	virtual void ApplySchemeSettings(vgui::IScheme *pScheme);
	virtual bool IsVisible(void);
	virtual void Reset();
	void OnTick();

public: // IGameEventListener:
	virtual void FireGameEvent(IGameEvent *event);

private:

	CTFLabel						*m_pPointTimer;
	CTFLabel						*m_pPointTimerShadow;
};

// 6TEAM BELOW

//-----------------------------------------------------------------------------
// Purpose:  
//-----------------------------------------------------------------------------
class CFOHud6GeneratorObjectives : public vgui::EditablePanel, public CGameEventListener
{
	DECLARE_CLASS_SIMPLE(CFOHud6GeneratorObjectives, vgui::EditablePanel);

public:

	CFOHud6GeneratorObjectives(vgui::Panel *parent, const char *name);

	virtual void ApplySchemeSettings(vgui::IScheme *pScheme);
	virtual bool IsVisible(void);
	virtual void Reset();
	void OnTick();

public: // IGameEventListener:
	virtual void FireGameEvent(IGameEvent *event);

private:

	CTFLabel						*m_pPointTimer;
	CTFLabel						*m_pPointTimerShadow;
};
#endif	// FO_HUD_GENERATORSTATUS_H
