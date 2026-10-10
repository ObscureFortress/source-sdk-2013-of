//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Sampling grid used to polygonise metaballs with marching cubes.
//
//=============================================================================//

#include "cbase.h"
#include "cube_grid.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

ConVar cl_blobs_resolution_max( "cl_blobs_resolution_max", "20", FCVAR_CLIENTDLL );

//-----------------------------------------------------------------------------
// Purpose: Room for the largest grid cl_blobs_resolution_max allows.
//-----------------------------------------------------------------------------
bool CUBE_GRID::CreateMemory()
{
	vertices = new CUBE_GRID_VERTEX[( cl_blobs_resolution_max.GetInt() + 1 ) * ( cl_blobs_resolution_max.GetInt() + 1 ) * ( cl_blobs_resolution_max.GetInt() + 1 )];
	if ( !vertices )
	{
		Msg( "Unable to allocate memory for %d Grid Vertices", numVertices );
		return false;
	}

	cubes = new CUBE_GRID_CUBE[cl_blobs_resolution_max.GetInt() * cl_blobs_resolution_max.GetInt() * cl_blobs_resolution_max.GetInt()];
	if ( !cubes )
	{
		Msg( "Unable to allocate memory for %d Grid Cubes", numCubes );
		return false;
	}

	return true;
}

//-----------------------------------------------------------------------------
// Purpose: Lay out a gridSize^3 lattice over the box Pos .. Pos + Bounds.
//-----------------------------------------------------------------------------
bool CUBE_GRID::Init( int gridSize, Vector Pos, Vector Bounds )
{
	// VERTICES
	numVertices = ( gridSize + 1 ) * ( gridSize + 1 ) * ( gridSize + 1 );

	int currentVertex = 0;

	for ( int i = 0; i < gridSize + 1; i++ )
	{
		for ( int j = 0; j < gridSize + 1; j++ )
		{
			for ( int k = 0; k < gridSize + 1; k++ )
			{
				vertices[currentVertex].position.x = Bounds.x / gridSize * i + Pos.x;
				vertices[currentVertex].position.y = Bounds.y / gridSize * j + Pos.y;
				vertices[currentVertex].position.z = Bounds.z / gridSize * k + Pos.z;

				currentVertex++;
			}
		}
	}

	// CUBES
	numCubes = gridSize * gridSize * gridSize;

	int currentCube = 0;

	for ( int i = 0; i < gridSize; i++ )
	{
		for ( int j = 0; j < gridSize; j++ )
		{
			for ( int k = 0; k < gridSize; k++ )
			{
				cubes[currentCube].vertices[0] = &vertices[( i * ( gridSize + 1 ) + j ) * ( gridSize + 1 ) + k];
				cubes[currentCube].vertices[1] = &vertices[( i * ( gridSize + 1 ) + j ) * ( gridSize + 1 ) + k + 1];
				cubes[currentCube].vertices[2] = &vertices[( i * ( gridSize + 1 ) + ( j + 1 ) ) * ( gridSize + 1 ) + k + 1];
				cubes[currentCube].vertices[3] = &vertices[( i * ( gridSize + 1 ) + ( j + 1 ) ) * ( gridSize + 1 ) + k];
				cubes[currentCube].vertices[4] = &vertices[( ( i + 1 ) * ( gridSize + 1 ) + j ) * ( gridSize + 1 ) + k];
				cubes[currentCube].vertices[5] = &vertices[( ( i + 1 ) * ( gridSize + 1 ) + j ) * ( gridSize + 1 ) + k + 1];
				cubes[currentCube].vertices[6] = &vertices[( ( i + 1 ) * ( gridSize + 1 ) + ( j + 1 ) ) * ( gridSize + 1 ) + k + 1];
				cubes[currentCube].vertices[7] = &vertices[( ( i + 1 ) * ( gridSize + 1 ) + ( j + 1 ) ) * ( gridSize + 1 ) + k];

				currentCube++;
			}
		}
	}

	return true;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CUBE_GRID::FreeMemory()
{
	if ( vertices )
		delete [] vertices;
	vertices = NULL;
	numVertices = 0;

	if ( cubes )
		delete [] cubes;
	cubes = NULL;
	numCubes = 0;
}
