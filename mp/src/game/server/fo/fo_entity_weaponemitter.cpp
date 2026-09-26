//====== Copyright © 1996-2005, Valve Corporation, All rights reserved. =======//
//
// Purpose: CTF AmmoPack.
//
//=============================================================================//
#include "cbase.h"
#include "items.h"
#include "tf_gamerules.h"
#include "tf_shareddefs.h"
#include "tf_weaponbase.h"
#include "tf_player.h"
#include "tf_team.h"
#include "engine/IEngineSound.h"
#include "util.h"
#include "tf_powerup.h"

//=============================================================================
//
// CTF AmmoPack defines.
//

#define FO_WEAPON_PICKUP_SOUND	"AmmoPack.Touch"

class CFOWeaponEmitter : public CTFPowerup
{
public:
	DECLARE_CLASS(CFOWeaponEmitter, CTFPowerup);
	DECLARE_DATADESC();

	CFOWeaponEmitter()
	{
		m_nWeaponID = 0;
		m_sModelName = "models/items/ammopack_large.mdl";
	}

	void	Spawn(void);
	void	Precache(void);
	bool	MyTouch(CBasePlayer *pPlayer);


	const char* m_sModelName;
	const char* m_nWeaponID;


	//void	GetWeaponID(void);

	virtual const char *GetWeaponModel(void) { return m_sModelName; }
};

LINK_ENTITY_TO_CLASS(fo_item_weapon_emitter, CFOWeaponEmitter);

BEGIN_DATADESC(CFOWeaponEmitter)

	DEFINE_KEYFIELD(m_nWeaponID, FIELD_STRING, "weaponid"),
	DEFINE_KEYFIELD(m_sModelName, FIELD_STRING, "weaponmodel"),

END_DATADESC()

//=============================================================================
//
// CTF AmmoPack functions.
//

//-----------------------------------------------------------------------------
// Purpose: Spawn function for the weapon emitter
//-----------------------------------------------------------------------------
void CFOWeaponEmitter::Spawn( void )
{
	Precache();
	SetModel( GetWeaponModel() );

	ResetSequence(LookupSequence("spin"));
	UTIL_SetSize(this, -Vector(25, 25, 12), Vector(25, 25, 12));

	BaseClass::Spawn();
}

//-----------------------------------------------------------------------------
// Purpose: Precache function for the weapon emitter
//-----------------------------------------------------------------------------
void CFOWeaponEmitter::Precache( void )
{
	PrecacheModel( GetWeaponModel() );
	PrecacheScriptSound( FO_WEAPON_PICKUP_SOUND );
}

//-----------------------------------------------------------------------------
// Purpose: MyTouch function for the weapon emitter
//-----------------------------------------------------------------------------
bool CFOWeaponEmitter::MyTouch( CBasePlayer *pPlayer )
{
	bool bSuccess = false;

	if ( ValidTouch( pPlayer ) )
	{
		CTFPlayer *pTFPlayer = ToTFPlayer( pPlayer );
		if ( !pTFPlayer )
			return false;

		//pPlayer->Weapon_Create("tf_weapon_shotgun_primary");


		//pPlayer->Weapon_SlotOccupied

		CBaseCombatWeapon *pWeapon = pPlayer->Weapon_Create(m_nWeaponID); // create the weapon
		pWeapon->ChangeTeam(pPlayer->GetTeamNumber());

		//pPlayer->Weapon_DropSlot(pWeapon->GetSlot());
		if (pPlayer->Weapon_GetSlot(pWeapon->GetSlot()) != NULL)
		{
			pPlayer->Weapon_Drop(pPlayer->Weapon_GetSlot(pWeapon->GetSlot()), NULL, NULL); // delete the previous weapon
			//pPlayer->Weapon_Detach(pPlayer->Weapon_GetSlot(pWeapon->GetSlot()));
			UTIL_Remove(pPlayer->Weapon_GetSlot(pWeapon->GetSlot()));
		}

		pPlayer->Weapon_Equip(pWeapon); // equip the weapon
		pPlayer->Weapon_Switch(pWeapon);

		int iMaxPrimary = pTFPlayer->GetPlayerClass()->GetData()->m_aAmmoMax[TF_AMMO_PRIMARY];
		if (pPlayer->GiveAmmo(ceil(iMaxPrimary * PackRatios[POWERUP_FULL]), TF_AMMO_PRIMARY, true))
		{
			bSuccess = true;
		}

		int iMaxSecondary = pTFPlayer->GetPlayerClass()->GetData()->m_aAmmoMax[TF_AMMO_SECONDARY];
		if (pPlayer->GiveAmmo(ceil(iMaxSecondary * PackRatios[POWERUP_FULL]), TF_AMMO_SECONDARY, true))
		{
			bSuccess = true;
		}

		/*
		CTFPlayerShared *pShared;

		// give that baby some ammo
		if (pShared->GetActiveTFWeapon()->GetTFWpnData().m_iWeaponType == TF_WPN_TYPE_PRIMARY)
		{
			int iMaxPrimary = pTFPlayer->GetPlayerClass()->GetData()->m_aAmmoMax[TF_AMMO_PRIMARY];
			if (pPlayer->GiveAmmo(ceil(iMaxPrimary * PackRatios[POWERUP_FULL]), TF_AMMO_PRIMARY, true))
			{
				bSuccess = true;
			}
		}
		else if (pShared->GetActiveTFWeapon()->GetTFWpnData().m_iWeaponType == TF_WPN_TYPE_SECONDARY)
		{
			int iMaxSecondary = pTFPlayer->GetPlayerClass()->GetData()->m_aAmmoMax[TF_AMMO_SECONDARY];
			if (pPlayer->GiveAmmo(ceil(iMaxSecondary * PackRatios[POWERUP_FULL]), TF_AMMO_SECONDARY, true))
			{
				bSuccess = true;
			}
		}
		*/

		bSuccess = true;

		/*
		pPlayer->Weapon_Detach(pWeapon); // delete the weapon. we dont need it
		UTIL_Remove(pWeapon);

		// give ammo instead
		int iMaxPrimary = pTFPlayer->GetPlayerClass()->GetData()->m_aAmmoMax[TF_AMMO_PRIMARY];
		if (pPlayer->GiveAmmo(ceil(iMaxPrimary * PackRatios[POWERUP_FULL]), TF_AMMO_PRIMARY, true))
		{
			bSuccess = true;
		}
		*/

		// did we give them anything?
		if ( bSuccess )
		{
			CSingleUserRecipientFilter filter( pPlayer );
			EmitSound( filter, entindex(), FO_WEAPON_PICKUP_SOUND );
		}
	}

	return bSuccess;
}
