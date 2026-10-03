//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Shared types for the client-side metaball renderer.
//
//=============================================================================//

#ifndef METABALL_H
#define METABALL_H
#ifdef _WIN32
#pragma once
#endif

// One lattice point of the sampling grid.
struct SURFACE_VERTEX
{
	Vector	pos;		// world position
	Vector	normal;		// accumulated field gradient
	float	value;		// accumulated field strength
};

// A vertex generated on a cube edge (output of marching cubes).
struct EDGE_VERTEX
{
	Vector	pos;
	Vector	normal;
};

// One cell of the grid, points at its 8 corner vertices.
// Corner order: (0,0,0) (0,0,1) (0,1,1) (0,1,0) (1,0,0) (1,0,1) (1,1,1) (1,1,0)
struct CUBE
{
	SURFACE_VERTEX *verts[8];
};

#endif // METABALL_H
