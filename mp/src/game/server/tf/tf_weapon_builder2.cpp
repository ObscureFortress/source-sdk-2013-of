//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose:			The "weapon" used to build objects
//					
//
// $Workfile:     $
// $Date:         $
// $NoKeywords: $
//=============================================================================//
#include "cbase.h"
#include "tf_player.h"
#include "entitylist.h"
#include "in_buttons.h"
#include "tf_obj.h"
#include "sendproxy.h"
#include "tf_weapon_builder2.h"
#include "vguiscreen.h"
#include "tf_gamerules.h"

extern ConVar tf2_object_hard_limits;
extern ConVar tf_fastbuild;

EXTERN_SEND_TABLE(DT_BaseCombatWeapon)

BEGIN_NETWORK_TABLE_NOBASE( CTFWeaponBuilder2, DT_Builder2LocalData )
	SendPropInt( SENDINFO( m_iObjectType ), BUILDER_OBJECT_BITS, SPROP_UNSIGNED ),
	SendPropEHandle( SENDINFO( m_hObjectBeingBuilt ) ),
END_NETWORK_TABLE()

IMPLEMENT_SERVERCLASS_ST(CTFWeaponBuilder2, DT_TFWeaponBuilder2)
	SendPropInt( SENDINFO( m_iBuildState ), 4, SPROP_UNSIGNED ),
	SendPropDataTable( "Builder2LocalData", 0, &REFERENCE_SEND_TABLE( DT_Builder2LocalData ), SendProxy_SendLocalWeaponDataTable ),
END_SEND_TABLE()

LINK_ENTITY_TO_CLASS( tf_weapon_builder2, CTFWeaponBuilder2 );
PRECACHE_WEAPON_REGISTER( tf_weapon_builder2 );

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
CTFWeaponBuilder2::CTFWeaponBuilder2()
{
	m_iObjectType.Set( BUILDER_INVALID_OBJECT );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
CTFWeaponBuilder2::~CTFWeaponBuilder2()
{
	StopPlacement();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFWeaponBuilder2::SetSubType( int iSubType )
{
	m_iObjectType = iSubType;

	BaseClass::SetSubType( iSubType );
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CTFWeaponBuilder2::Precache( void )
{
	BaseClass::Precache();

	// Precache all the viewmodels we could possibly be building
	for ( int iObj=0; iObj < OBJ_LAST; iObj++ )
	{
		const CObjectInfo *pInfo = GetObjectInfo( iObj );

		if ( pInfo )
		{
			if ( pInfo->m_pViewModel )
			{
				PrecacheModel( pInfo->m_pViewModel );
			}

			if ( pInfo->m_pPlayerModel )
			{
				PrecacheModel( pInfo->m_pPlayerModel );
			}
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool CTFWeaponBuilder2::CanDeploy( void )
{
	CTFPlayer *pPlayer = ToTFPlayer( GetOwner() );
	if (!pPlayer)
		return false;

	if ( pPlayer->CanBuild( m_iObjectType ) != CB_CAN_BUILD )
	{
		return false;
	}

	return BaseClass::CanDeploy();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool CTFWeaponBuilder2::Deploy( void )
{
	m_iViewModelIndex = modelinfo->GetModelIndex( GetViewModel( 0 ) );
	m_iWorldModelIndex = modelinfo->GetModelIndex( GetWorldModel() );

	bool bDeploy = BaseClass::Deploy();

	if ( bDeploy )
	{
		SetCurrentState( BS_PLACING );
		StartPlacement(); 
		m_flNextPrimaryAttack = gpGlobals->curtime + 0.35f;
		m_flNextSecondaryAttack = gpGlobals->curtime;		// asap

		CTFPlayer *pPlayer = ToTFPlayer( GetOwner() );
		if (!pPlayer)
			return false;

		pPlayer->SetNextAttack( gpGlobals->curtime );

		m_flNextDenySound = 0;

		// Set off the hint here, because we don't know until now if our building
		// is rotate-able or not.
		if ( m_hObjectBeingBuilt && !m_hObjectBeingBuilt->MustBeBuiltOnAttachmentPoint() )
		{
			// set the alt-fire hint so it gets removed when we holster
			m_iAltFireHint = HINT_ALTFIRE_ROTATE_BUILDING;
			pPlayer->StartHintTimer( m_iAltFireHint );
		}
	}

	return bDeploy;
}

Activity CTFWeaponBuilder2::GetDrawActivity( void )
{
	// sapper used to call different draw animations , one when invis and one when not.
	// now you can go invis *while* deploying, so let's always use the one-handed deploy.
	if ( GetType() == OBJ_ATTACHMENT_SAPPER )
	{
		return ACT_VM_DRAW_DEPLOYED;
	}
	else
	{
		return BaseClass::GetDrawActivity();
	}
}

//-----------------------------------------------------------------------------
// Purpose: Stop placement when holstering
//-----------------------------------------------------------------------------
bool CTFWeaponBuilder2::Holster( CBaseCombatWeapon *pSwitchingTo )
{
	if ( m_iBuildState == BS_PLACING || m_iBuildState == BS_PLACING_INVALID )
	{
		SetCurrentState( BS_IDLE );
	}
	StopPlacement();

	return BaseClass::Holster(pSwitchingTo);
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFWeaponBuilder2::ItemPostFrame( void )
{
	CTFPlayer *pOwner = ToTFPlayer( GetOwner() );
	if ( !pOwner )
		return;

	// If we're building, and our team has lost, stop placing the object
	if ( m_hObjectBeingBuilt.Get() && 
		TFGameRules()->State_Get() == GR_STATE_TEAM_WIN && 
		pOwner->GetTeamNumber() != TFGameRules()->GetWinningTeam() )
	{
		StopPlacement();
		return;
	}

	// Check that I still have enough resources to build this item
	if ( pOwner->CanBuild( m_iObjectType ) != CB_CAN_BUILD )
	{
		SwitchOwnersWeaponToLast();
	}

	if (( pOwner->m_nButtons & IN_ATTACK ) && (m_flNextPrimaryAttack <= gpGlobals->curtime) )
	{
		PrimaryAttack();
	}

	if ( pOwner->m_nButtons & IN_ATTACK2 )
	{
		if ( m_flNextSecondaryAttack <= gpGlobals->curtime )
		{
			SecondaryAttack();
		}
	}
	else
	{
		m_bInAttack2 = false;
	}

	WeaponIdle();
}

//-----------------------------------------------------------------------------
// Purpose: Start placing or building the currently selected object
//-----------------------------------------------------------------------------
void CTFWeaponBuilder2::PrimaryAttack( void )
{
	CTFPlayer *pOwner = ToTFPlayer( GetOwner() );
	if ( !pOwner )
		return;

	if ( !CanAttack() )
		return;

	// Necessary so that we get the latest building position for the test, otherwise
	// we are one frame behind.
	UpdatePlacementState();

	// What state should we move to?
	switch( m_iBuildState )
	{
	case BS_IDLE:
		{
			// Idle state starts selection
			SetCurrentState( BS_SELECTING );
		}
		break;

	case BS_SELECTING:
		{
			// Do nothing, client handles selection
			return;
		}
		break;

	case BS_PLACING:
		{
			if ( m_hObjectBeingBuilt )
			{
				int iFlags = m_hObjectBeingBuilt->GetObjectFlags();

				// Tricky, because this can re-calc the object position and change whether its a valid 
				// pos or not. Best not to do this only in debug, but we can be pretty sure that this
				// will give the same result as was calculated in UpdatePlacementState() above.
				Assert( IsValidPlacement() );

				// If we're placing an attachment, like a sapper, play a placement animation on the owner
				if ( m_hObjectBeingBuilt->MustBeBuiltOnAttachmentPoint() )
				{
					pOwner->DoAnimationEvent( PLAYERANIMEVENT_ATTACK_GRENADE );
				}

				StartBuilding();

				// Should we switch away?
				if ( iFlags & OF_ALLOW_REPEAT_PLACEMENT )
				{
					// Start placing another
					SetCurrentState( BS_PLACING );
					StartPlacement(); 
				}
				else
				{
					SwitchOwnersWeaponToLast();
				}
			}
		}
		break;

	case BS_PLACING_INVALID:
		{
			if ( m_flNextDenySound < gpGlobals->curtime )
			{
				CSingleUserRecipientFilter filter( pOwner );
				EmitSound( filter, entindex(), "Player.DenyWeaponSelection" );

				m_flNextDenySound = gpGlobals->curtime + 0.5;
			}
		}
		break;
	}

	m_flNextPrimaryAttack = gpGlobals->curtime + 0.2f;
}

void CTFWeaponBuilder2::SecondaryAttack( void )
{
	if ( m_bInAttack2 )
		return;

	// require a re-press
	m_bInAttack2 = true;

	CTFPlayer *pOwner = ToTFPlayer( GetOwner() );
	if ( !pOwner )
		return;

	if ( pOwner->DoClassSpecialSkill() )
	{
		// intentionally blank
	}
	else if ( m_iBuildState == BS_PLACING )
	{
		if ( m_hObjectBeingBuilt )
		{
			pOwner->StopHintTimer( HINT_ALTFIRE_ROTATE_BUILDING );
			m_hObjectBeingBuilt->RotateBuildAngles();
		}
	}

	m_flNextSecondaryAttack = gpGlobals->curtime + 0.2f;
}

//-----------------------------------------------------------------------------
// Purpose: Set the builder to the specified state
//-----------------------------------------------------------------------------
void CTFWeaponBuilder2::SetCurrentState( int iState )
{
	m_iBuildState = iState;
}

//-----------------------------------------------------------------------------
// Purpose: Set the owner's weapon and last weapon appropriately when we need to
//			switch away from the builder weapon.  
//-----------------------------------------------------------------------------
void CTFWeaponBuilder2::SwitchOwnersWeaponToLast()
{
	CTFPlayer *pOwner = ToTFPlayer(GetOwner());
	if (!pOwner)
		return;

	// for engineer, switch to wrench and set last weapon appropriately
	if (pOwner->IsPlayerClass(TF_CLASS_ENGINEER) || pOwner->IsPlayerClass(FO_CLASS_SENTRONIC + 1) || pOwner->IsPlayerClass(FO_CLASS_DISMATIC + 1) || pOwner->IsPlayerClass(TF_CLASS_ROBO + 1) || pOwner->IsPlayerClass(FO_CLASS_WORKERNODE + 1) || pOwner->IsPlayerClass(FO_CLASS_SAPTRAP + 1))
	{
		// Switch to wrench if possible. if not, then best weapon
		CBaseCombatWeapon *pWpn = pOwner->Weapon_GetSlot( 2 );

		// Don't store last weapon when we autoswitch off builder
		CBaseCombatWeapon *pLastWpn = pOwner->GetLastWeapon();

		if ( pWpn )
		{
			pOwner->Weapon_Switch( pWpn );
		}
		else
		{
			pOwner->SwitchToNextBestWeapon( NULL );
		}

		if ( pWpn == pLastWpn )
		{
			// We had the wrench out before we started building. Go ahead and set out last
			// weapon to our primary weapon.
			pWpn = pOwner->Weapon_GetSlot( 0 );
			pOwner->Weapon_SetLast( pWpn );
		}
		else
		{
			pOwner->Weapon_SetLast( pLastWpn );
		}
	}
	else
	{
		// for all other classes, just switch to last weapon used
		pOwner->Weapon_Switch( pOwner->GetLastWeapon() );
	}
}

//-----------------------------------------------------------------------------
// Purpose: updates the building postion and checks the new postion
//-----------------------------------------------------------------------------
void CTFWeaponBuilder2::UpdatePlacementState( void )
{
	// This updates the building position
	bool bValidPos = IsValidPlacement();

	// If we're in placement mode, update the placement model
	switch( m_iBuildState )
	{
	case BS_PLACING:
	case BS_PLACING_INVALID:
		{
			if ( bValidPos )
			{
				SetCurrentState( BS_PLACING );
			}
			else
			{
				SetCurrentState( BS_PLACING_INVALID );
			}
		}
		break;

	default:
		break;
	}
}

//-----------------------------------------------------------------------------
// Purpose: Idle updates the position of the build placement model
//-----------------------------------------------------------------------------
void CTFWeaponBuilder2::WeaponIdle( void )
{
	CTFPlayer *pOwner = ToTFPlayer( GetOwner() );
	if ( !pOwner )
		return;

	if ( HasWeaponIdleTimeElapsed() )
	{
		SendWeaponAnim( ACT_VM_IDLE );
	}
}

//-----------------------------------------------------------------------------
// Purpose: Start placing the object
//-----------------------------------------------------------------------------
void CTFWeaponBuilder2::StartPlacement( void )
{
	StopPlacement();

	// Create the slab
	m_hObjectBeingBuilt = (CBaseObject*)CreateEntityByName( GetObjectInfo( m_iObjectType )->m_pClassName );
	if ( m_hObjectBeingBuilt )
	{
		m_hObjectBeingBuilt->Spawn();
		m_hObjectBeingBuilt->StartPlacement( ToTFPlayer( GetOwner() ) );

		// Stomp this here in the same frame we make the object, so prevent clientside warnings that it's under attack
		m_hObjectBeingBuilt->m_iHealth = OBJECT_CONSTRUCTION_STARTINGHEALTH;
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFWeaponBuilder2::StopPlacement( void )
{
	if ( m_hObjectBeingBuilt )
	{
		m_hObjectBeingBuilt->StopPlacement();
		m_hObjectBeingBuilt = NULL;
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFWeaponBuilder2::WeaponReset( void )
{
	BaseClass::WeaponReset();

	StopPlacement();
}


//-----------------------------------------------------------------------------
// Purpose: Move the placement model to the current position. Return false if it's an invalid position
//-----------------------------------------------------------------------------
bool CTFWeaponBuilder2::IsValidPlacement( void )
{
	if ( !m_hObjectBeingBuilt )
		return false;

	CBaseObject *pObj = m_hObjectBeingBuilt.Get();

	pObj->UpdatePlacement();

	return m_hObjectBeingBuilt->IsValidPlacement();
}

//-----------------------------------------------------------------------------
// Purpose: Player holding this weapon has started building something
// Assumes we are in a valid build position
//-----------------------------------------------------------------------------
void CTFWeaponBuilder2::StartBuilding( void )
{
	CBaseObject *pObj = m_hObjectBeingBuilt.Get();

	Assert( pObj );

	pObj->StartBuilding( GetOwner() );

	m_hObjectBeingBuilt = NULL;

	CTFPlayer *pPlayer = ToTFPlayer( GetOwner() );
	if ( pPlayer )
	{
		pPlayer->RemoveInvisibility();
	}
}

//-----------------------------------------------------------------------------
// Purpose: Return true if this weapon has some ammo
//-----------------------------------------------------------------------------
bool CTFWeaponBuilder2::HasAmmo( void )
{
	CTFPlayer *pOwner = ToTFPlayer( GetOwner() );
	if ( !pOwner )
		return false;

	int iCost = CalculateObjectCost( m_iObjectType );
	return ( pOwner->GetBuildResources() >= iCost );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
int CTFWeaponBuilder2::GetSlot( void ) const
{
	return GetObjectInfo( m_iObjectType )->m_SelectionSlot;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
int CTFWeaponBuilder2::GetPosition( void ) const
{
	return GetObjectInfo( m_iObjectType )->m_SelectionPosition;
}

//-----------------------------------------------------------------------------
// Purpose: 
// Output : char const
//-----------------------------------------------------------------------------
const char *CTFWeaponBuilder2::GetPrintName( void ) const
{
	return GetObjectInfo( m_iObjectType )->m_pStatusName;
}

// -----------------------------------------------------------------------------
// Purpose:
// -----------------------------------------------------------------------------
const char *CTFWeaponBuilder2::GetViewModel( int iViewModel ) const
{
	if ( GetPlayerOwner() == NULL )
	{
		return BaseClass::GetViewModel();
	}

	if ( m_iObjectType != BUILDER_INVALID_OBJECT )
	{
		return GetObjectInfo( m_iObjectType )->m_pViewModel;
	}

	return BaseClass::GetViewModel();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
const char *CTFWeaponBuilder2::GetWorldModel( void ) const
{
	if ( GetPlayerOwner() == NULL )
	{
		return BaseClass::GetWorldModel();
	}

	if ( m_iObjectType != BUILDER_INVALID_OBJECT )
	{
		return GetObjectInfo( m_iObjectType )->m_pPlayerModel;
	}

	return BaseClass::GetWorldModel();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool CTFWeaponBuilder2::AllowsAutoSwitchTo( void ) const
{
	// ask the object we're building
	return GetObjectInfo( m_iObjectType )->m_bAutoSwitchTo;
}
