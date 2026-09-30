// env_skybox_swapper
// Swaps the map's skybox (sv_skyname) when triggered.
// Reconstructed from the Fortress Obscura release build.

#include "cbase.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

//-----------------------------------------------------------------------------
// Purpose: Changes sv_skyname when it receives the "Trigger" input.
//-----------------------------------------------------------------------------
class CSkyboxSwapper : public CServerOnlyPointEntity
{
public:
	DECLARE_CLASS( CSkyboxSwapper, CServerOnlyPointEntity );
	DECLARE_DATADESC();

	virtual void Spawn( void );
	virtual void Precache( void );

	void InputTrigger( inputdata_t &inputdata );

protected:
	string_t m_iszSkyboxName;
};

LINK_ENTITY_TO_CLASS( skybox_swapper, CSkyboxSwapper );

BEGIN_DATADESC( CSkyboxSwapper )
	DEFINE_KEYFIELD( m_iszSkyboxName, FIELD_STRING, "SkyboxName" ),
	DEFINE_INPUTFUNC( FIELD_VOID, "Trigger", InputTrigger ),
END_DATADESC()

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CSkyboxSwapper::Spawn( void )
{
	Precache();
}

//-----------------------------------------------------------------------------
// Purpose: Precache the six faces of the new skybox.
//-----------------------------------------------------------------------------
void CSkyboxSwapper::Precache( void )
{
	if ( m_iszSkyboxName == NULL_STRING )
	{
		Warning( "skybox_swapper (%s) has no skybox specified!\n", STRING( GetEntityName() ) );
		return;
	}

	const char *pszSuffixes[6] = { "rt", "bk", "lf", "ft", "up", "dn" };

	for ( int i = 0; i < 6; i++ )
	{
		char szMaterial[MAX_PATH];
		Q_snprintf( szMaterial, sizeof( szMaterial ), "skybox/%s%s", STRING( m_iszSkyboxName ), pszSuffixes[i] );
		PrecacheMaterial( szMaterial );
	}
}

//-----------------------------------------------------------------------------
// Purpose: Swap the skybox.
//-----------------------------------------------------------------------------
void CSkyboxSwapper::InputTrigger( inputdata_t &inputdata )
{
	static ConVarRef sv_skyname( "sv_skyname", false );

	if ( !sv_skyname.IsValid() )
	{
		Warning( "skybox_swapper (%s) trigger input failed - cannot find 'sv_skyname' convar!\n", STRING( GetEntityName() ) );
		return;
	}

	sv_skyname.SetValue( STRING( m_iszSkyboxName ) );
}
