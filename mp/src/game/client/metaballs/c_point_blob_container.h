//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Renders the metaballs inside a point_blob_container with marching cubes.
//
//=============================================================================//

#ifndef C_POINT_BLOB_CONTAINER_H
#define C_POINT_BLOB_CONTAINER_H
#ifdef _WIN32
#pragma once
#endif

#include "c_baseanimating.h"
#include "cube_grid.h"

#include "tier0/valve_minmax_off.h"
#include <vector>
#include "tier0/valve_minmax_on.h"

class C_PointBlobElement;
class IMesh;
class IMaterial;

class C_PointBlobContainer : public C_BaseAnimating
{
public:
	DECLARE_CLASS( C_PointBlobContainer, C_BaseAnimating );
	DECLARE_CLIENTCLASS();

	C_PointBlobContainer();
	virtual ~C_PointBlobContainer();

	void UpdateContainer( void );
	void UpdateMeshData( unsigned int startpoint, unsigned int nOfIterations );
	void UpdateResolution( void );

	virtual int DrawModel( int flags );
	virtual void GetRenderBounds( Vector &mins, Vector &maxs );
	virtual void Spawn( void );
	virtual void ClientThink( void );
	virtual bool ShouldDraw( void );
	virtual void Simulate( void );
	virtual void OnDataChanged( DataUpdateType_t updateType );

	CUBE_GRID cubeGrid;
	int GridSize;
	float colorBoost;
	Vector GridBounds;
	CNetworkColor32( Ambcolor );
	CNetworkColor32( color );
	float attraction;

	std::vector<C_PointBlobElement *> metaballs;
	IMesh *pMesh;
	C_PointBlobContainer *parentedContainer;
	Vector white[6];
	char BlobMaterialName[260];
	KeyValues *kval;
	IMaterial *CustomMat;
	SURFACE_VERTEX edgeVertices[12];
	bool first;
};

#endif // C_POINT_BLOB_CONTAINER_H
