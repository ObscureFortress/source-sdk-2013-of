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

// One lattice point of the grid.
class CUBE_GRID_VERTEX
{
public:
	Vector position;
	float value;	// the value of the scalar field at this point
	Vector normal;
};

// One cell of the grid, points at its 8 corner vertices.
class CUBE_GRID_CUBE
{
public:
	CUBE_GRID_VERTEX *vertices[8];
};

class CUBE_GRID
{
public:
	unsigned int numVertices;
	CUBE_GRID_VERTEX *vertices;

	int numCubes;
	CUBE_GRID_CUBE *cubes;

	bool CreateMemory();
	bool Init( int gridSize, Vector Pos, Vector Bounds );
	void DrawSurface( float threshold );
	void FreeMemory();

	CUBE_GRID() : numVertices( 0 ), vertices( NULL ), numCubes( 0 ), cubes( NULL ), numFacesDrawn( 0 )
	{}
	~CUBE_GRID()
	{
		FreeMemory();
	}

	int numFacesDrawn;
};

// A vertex generated on a cube edge.
class SURFACE_VERTEX
{
public:
	Vector position;
	Vector normal;
};

#endif // CUBE_GRID_H
