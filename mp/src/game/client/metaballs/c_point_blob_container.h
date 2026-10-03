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
#include "utlvector.h"

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

	virtual void Spawn( void );
	virtual bool ShouldDraw( void ) { return true; }
	virtual void GetRenderBounds( Vector &mins, Vector &maxs );
	virtual void Simulate( void );
	virtual int DrawModel( int flags );

	void UpdateContainer( void );
	void UpdateMeshData( int iFirst, int iLast );

	CUBE_GRID	cubeGrid;
	int			GridSize;
	float		colorBoost;
	Vector		GridBounds;
	color32		Ambcolor;
	color32		color;
	float		attraction;

	CUtlVector<C_PointBlobElement *> metaballs;
	IMesh		*pMesh;
	C_PointBlobContainer *parentedContainer;
	char		BlobMaterialName[260];
	KeyValues	*kval;
	IMaterial	*CustomMat;

private:
	bool		first;
	int			m_nLastUpdateFrame;
};

#endif // C_POINT_BLOB_CONTAINER_H
