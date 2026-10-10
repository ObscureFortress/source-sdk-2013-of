//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: A single metaball as seen by the client side renderer.
//
//=============================================================================//

#ifndef METABALL_H
#define METABALL_H
#ifdef _WIN32
#pragma once
#endif

class METABALL
{
public:
	Vector position;
	float squaredRadius;

	void Init( Vector newPosition, float newSquaredRadius )
	{
		position = newPosition;
		squaredRadius = newSquaredRadius;
	}
};

#endif // METABALL_H
