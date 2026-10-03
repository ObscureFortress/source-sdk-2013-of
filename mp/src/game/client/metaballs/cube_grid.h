//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Sampling grid used to polygonise metaballs with marching cubes.
//
//=============================================================================//

#ifndef CUBE_GRID_H
#define CUBE_GRID_H
#ifdef _WIN32
#pragma once
#endif

#include "metaball.h"

class CUBE_GRID
{
public:
	CUBE_GRID();
	~CUBE_GRID();

	// Allocates storage for the largest grid allowed by cl_blobs_resolution_max.
	bool CreateMemory();
	void FreeMemory();

	// Builds an n x n x n cell grid starting at origin and spanning extents.
	bool Init( int n, const Vector &origin, const Vector &extents );

	SURFACE_VERTEX	*vertices;
	CUBE			*cubes;
	int				numVertices;
	int				numCubes;
	int				gridSize;
	int				maxGridSize;
};

// Marching cubes lookup tables (classic edge / triangle tables).
extern const int g_BlobEdgeTable[256];
extern const int g_BlobTriTable[256][16];
// Pairs of corner indices joined by each of the 12 cube edges.
extern const unsigned char g_BlobEdgeCorners[12][2];

#endif // CUBE_GRID_H
