// TURNIP CODE 
// 8/21/2022

#include "cbase.h"

class CFOGamemodeDomination : public CLogicalEntity
{
public:
	DECLARE_CLASS(CFOGamemodeDomination, CLogicalEntity);
	DECLARE_DATADESC();

	// Constructor
	CFOGamemodeDomination()
	{
		m_nRedPointBucket = 0;
		m_nBluePointBucket = 0;
		m_nGreenPointBucket = 0;
		m_nYellowPointBucket = 0;
		m_nPurplePointBucket = 0;
		m_nPinkPointBucket = 0;

		m_nRedPointCounters = 0;
		m_nBluePointCounters = 0;
		m_nGreenPointCounters = 0;
		m_nYellowPointCounters = 0;
		m_nPurplePointCounters = 0;
		m_nPinkPointCounters = 0;

		m_bTimerInHUD = false;
		
		m_bHasWon = false;
	}

	// Input function
	void AddRedCounter(inputdata_t &inputData);
	void AddBlueCounter(inputdata_t &inputData);
	void AddGreenCounter(inputdata_t &inputData);
	void AddYellowCounter(inputdata_t &inputData);
	void AddPurpleCounter(inputdata_t &inputData);
	void AddPinkCounter(inputdata_t &inputData);

	void RemoveRedCounter(inputdata_t &inputData);
	void RemoveBlueCounter(inputdata_t &inputData);
	void RemoveGreenCounter(inputdata_t &inputData);
	void RemoveYellowCounter(inputdata_t &inputData);
	void RemovePurpleCounter(inputdata_t &inputData);
	void RemovePinkCounter(inputdata_t &inputData);

	void UpdatePoints(inputdata_t &inputData);

	int	m_nPointLimit;
	int	m_nRedPointBucket;
	int	m_nBluePointBucket;
	int	m_nGreenPointBucket;
	int	m_nYellowPointBucket;
	int	m_nPurplePointBucket;
	int	m_nPinkPointBucket;

private:

	int	m_nRedPointCounters;
	int	m_nBluePointCounters;
	int	m_nGreenPointCounters;
	int	m_nYellowPointCounters;
	int	m_nPurplePointCounters;
	int	m_nPinkPointCounters;

	bool m_bHasWon;
	bool m_bTimerInHUD;

	COutputEvent	m_OnLimit;

	COutputEvent	m_OnLimitRed;
	COutputEvent	m_OnLimitBlue;
	COutputEvent	m_OnLimitGreen;
	COutputEvent	m_OnLimitYellow;
	COutputEvent	m_OnLimitPurple;
	COutputEvent	m_OnLimitPink;
};

LINK_ENTITY_TO_CLASS(fo_gamemode_dom, CFOGamemodeDomination);


BEGIN_DATADESC(CFOGamemodeDomination)

	DEFINE_KEYFIELD(m_nRedPointBucket, FIELD_INTEGER, "redpoints"),
	DEFINE_KEYFIELD(m_nBluePointBucket, FIELD_INTEGER, "bluepoints"),
	DEFINE_KEYFIELD(m_nGreenPointBucket, FIELD_INTEGER, "greenpoints"),
	DEFINE_KEYFIELD(m_nYellowPointBucket, FIELD_INTEGER, "yellowpoints"),
	DEFINE_KEYFIELD(m_nPurplePointBucket, FIELD_INTEGER, "purplepoints"),
	DEFINE_KEYFIELD(m_nPinkPointBucket, FIELD_INTEGER, "pinkpoints"),

	DEFINE_KEYFIELD(m_nPointLimit, FIELD_INTEGER, "pointlimit"),

	DEFINE_KEYFIELD(m_bTimerInHUD, FIELD_BOOLEAN, "timerinhud"),

	DEFINE_INPUTFUNC(FIELD_VOID, "AddRedCounter", AddRedCounter),
	DEFINE_INPUTFUNC(FIELD_VOID, "AddBlueCounter", AddBlueCounter),
	DEFINE_INPUTFUNC(FIELD_VOID, "AddGreenCounter", AddGreenCounter),
	DEFINE_INPUTFUNC(FIELD_VOID, "AddYellowCounter", AddYellowCounter),
	DEFINE_INPUTFUNC(FIELD_VOID, "AddPurpleCounter", AddPurpleCounter),
	DEFINE_INPUTFUNC(FIELD_VOID, "AddPinkCounter", AddPinkCounter),

	DEFINE_INPUTFUNC(FIELD_VOID, "RemoveRedCounter", RemoveRedCounter),
	DEFINE_INPUTFUNC(FIELD_VOID, "RemoveBlueCounter", RemoveBlueCounter),
	DEFINE_INPUTFUNC(FIELD_VOID, "RemoveGreenCounter", RemoveGreenCounter),
	DEFINE_INPUTFUNC(FIELD_VOID, "RemoveYellowCounter", RemoveYellowCounter),
	DEFINE_INPUTFUNC(FIELD_VOID, "RemovePurpleCounter", RemovePurpleCounter),
	DEFINE_INPUTFUNC(FIELD_VOID, "RemovePinkCounter", RemovePinkCounter),

	DEFINE_INPUTFUNC(FIELD_VOID, "UpdatePoints", UpdatePoints),

	DEFINE_OUTPUT(m_OnLimit, "OnLimit"),

	DEFINE_OUTPUT(m_OnLimitRed, "OnLimitRed"),
	DEFINE_OUTPUT(m_OnLimitBlue, "OnLimitBlue"),
	DEFINE_OUTPUT(m_OnLimitGreen, "OnLimitGreen"),
	DEFINE_OUTPUT(m_OnLimitYellow, "OnLimitYellow"),
	DEFINE_OUTPUT(m_OnLimitPurple, "OnLimitPurple"),
	DEFINE_OUTPUT(m_OnLimitPink, "OnLimitPink"),

END_DATADESC()

extern ConVar fo_dom_red_score;
extern ConVar fo_dom_blue_score;
extern ConVar fo_dom_green_score;
extern ConVar fo_dom_yellow_score;
extern ConVar fo_dom_purple_score;
extern ConVar fo_dom_pink_score;
extern ConVar fo_dom_scorelimit;
extern ConVar fo_dom_timerinhud;

void CFOGamemodeDomination::UpdatePoints(inputdata_t &inputData)
{
	fo_dom_timerinhud.SetValue(m_bTimerInHUD);
	if (m_nRedPointBucket >= m_nPointLimit)
	{
		if (!m_bHasWon)
		{
			m_nRedPointBucket = m_nPointLimit;
			m_OnLimit.FireOutput(this, this); // General output which activates for every team
			m_OnLimitRed.FireOutput(this, this);
			m_bHasWon = true;
		}
		return;
	}

	if (m_nBluePointBucket >= m_nPointLimit)
	{
		if (!m_bHasWon)
		{
			m_nBluePointBucket = m_nPointLimit;
			m_OnLimit.FireOutput(this, this);
			m_OnLimitBlue.FireOutput(this, this);
			m_bHasWon = true;
		}
		return;
	}

	if (m_nGreenPointBucket >= m_nPointLimit)
	{
		if (!m_bHasWon)
		{
			m_nGreenPointBucket = m_nPointLimit;
			m_OnLimit.FireOutput(this, this);
			m_OnLimitGreen.FireOutput(this, this);
			m_bHasWon = true;
		}
		return;
	}

	if (m_nYellowPointBucket >= m_nPointLimit)
	{
		if (!m_bHasWon)
		{
			m_nYellowPointBucket = m_nPointLimit;
			m_OnLimit.FireOutput(this, this);
			m_OnLimitYellow.FireOutput(this, this);
			m_bHasWon = true;
		}
		return;
	}

	if (m_nPurplePointBucket >= m_nPointLimit)
	{
		if (!m_bHasWon)
		{
			m_nPurplePointBucket = m_nPointLimit;
			m_OnLimit.FireOutput(this, this);
			m_OnLimitPurple.FireOutput(this, this);
			m_bHasWon = true;
		}
		return;
	}

	if (m_nPinkPointBucket >= m_nPointLimit)
	{
		if (!m_bHasWon)
		{
			m_nPinkPointBucket = m_nPointLimit;
			m_OnLimit.FireOutput(this, this);
			m_OnLimitPink.FireOutput(this, this);
			m_bHasWon = true;
		}
		return;
	}

	if (!m_bHasWon)
	{
		m_nRedPointBucket += m_nRedPointCounters; // more control points more points added
		m_nBluePointBucket += m_nBluePointCounters;
		m_nGreenPointBucket += m_nGreenPointCounters;
		m_nYellowPointBucket += m_nYellowPointCounters;
		m_nPurplePointBucket += m_nPurplePointCounters;
		m_nPinkPointBucket += m_nPinkPointCounters;
	}

	fo_dom_red_score.SetValue(m_nRedPointBucket); // hud stuff
	fo_dom_blue_score.SetValue(m_nBluePointBucket);
	fo_dom_green_score.SetValue(m_nGreenPointBucket);
	fo_dom_yellow_score.SetValue(m_nYellowPointBucket);
	fo_dom_purple_score.SetValue(m_nPurplePointBucket);
	fo_dom_pink_score.SetValue(m_nPinkPointBucket);

	fo_dom_scorelimit.SetValue(m_nPointLimit);

	/*
	Warning("red team has %d \n", m_nRedPointBucket);
	Warning("blue team has %d \n", m_nBluePointBucket);
	Warning("green team has %d \n", m_nGreenPointBucket);
	Warning("yellow team has %d \n", m_nYellowPointBucket);
	Warning("purple team has %d \n", m_nPurplePointBucket);
	Warning("pink team has %d \n", m_nPinkPointBucket);
	Warning("COUNTERS down______________________\n");
	Warning("red team has %d \n", m_nRedPointCounters);
	Warning("blue team has %d \n", m_nBluePointCounters);
	Warning("green team has %d \n", m_nGreenPointCounters);
	Warning("yellow team has %d \n", m_nYellowPointCounters);
	Warning("purple team has %d \n", m_nPurplePointCounters);
	Warning("pink team has %d \n", m_nPinkPointCounters);
	Warning("POINTS down________________________\n");
	*/
}

// ADD ---------------------------------------

void CFOGamemodeDomination::AddRedCounter(inputdata_t &inputData)
{
	m_nRedPointCounters++;
}

void CFOGamemodeDomination::AddBlueCounter(inputdata_t &inputData)
{
	m_nBluePointCounters++;
}

void CFOGamemodeDomination::AddGreenCounter(inputdata_t &inputData)
{
	m_nGreenPointCounters++;
}

void CFOGamemodeDomination::AddYellowCounter(inputdata_t &inputData)
{
	m_nYellowPointCounters++;
}

void CFOGamemodeDomination::AddPurpleCounter(inputdata_t &inputData)
{
	m_nPurplePointCounters++;
}

void CFOGamemodeDomination::AddPinkCounter(inputdata_t &inputData)
{
	m_nPinkPointCounters++;
}

// REMOVE ---------------------------------------

void CFOGamemodeDomination::RemoveRedCounter(inputdata_t &inputData)
{
	m_nRedPointCounters--;
}

void CFOGamemodeDomination::RemoveBlueCounter(inputdata_t &inputData)
{
	m_nBluePointCounters--;
}

void CFOGamemodeDomination::RemoveGreenCounter(inputdata_t &inputData)
{
	m_nGreenPointCounters--;
}

void CFOGamemodeDomination::RemoveYellowCounter(inputdata_t &inputData)
{
	m_nYellowPointCounters--;
}

void CFOGamemodeDomination::RemovePurpleCounter(inputdata_t &inputData)
{
	m_nPurplePointCounters--;
}

void CFOGamemodeDomination::RemovePinkCounter(inputdata_t &inputData)
{
	m_nPinkPointCounters--;
}