//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Renders the metaballs inside a point_blob_container with marching cubes.
//
//=============================================================================//

#include "cbase.h"
#include "c_point_blob_container.h"
#include "c_point_blob_element.h"
#include "metaball.h"
#include "materialsystem/imaterial.h"
#include "materialsystem/imesh.h"
#include "materialsystem/imaterialsystem.h"
#include "recvproxy.h"
#include "debugoverlay_shared.h"
#include "tier1/KeyValues.h"
#include "tier1/utlvector.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

ConVar r_drawblobs( "r_drawblobs", "1", FCVAR_HIDDEN, "Draw metaballs" );
ConVar cl_blobthreshold( "cl_blobthreshold", "1", 0, "Field strength at which the metaball surface is drawn" );
ConVar cl_blobvaluethreshold( "cl_blobvaluethreshold", "0.03", 0, "Contributions weaker than this are ignored" );
ConVar cl_blobs_updatecontainers( "cl_blobs_updatecontainers", "1", 0, "Gather metaballs every frame" );

IMPLEMENT_CLIENTCLASS_DT( C_PointBlobContainer, DT_PointBlobContainer, CPointBlobContainer )
	RecvPropInt( RECVINFO( GridSize ) ),
	RecvPropVector( RECVINFO( GridBounds ) ),
	RecvPropInt( RECVINFO( color ), 0, RecvProxy_IntToColor32 ),
	RecvPropInt( RECVINFO( Ambcolor ), 0, RecvProxy_IntToColor32 ),
	RecvPropFloat( RECVINFO( colorBoost ) ),
	RecvPropString( RECVINFO( BlobMaterialName ) ),
	RecvPropFloat( RECVINFO( attraction ) ),
END_RECV_TABLE()

//-----------------------------------------------------------------------------
C_PointBlobContainer::C_PointBlobContainer()
{
	GridSize = 10;
	GridBounds.Init( 50, 50, 50 );
	colorBoost = 1.0f;
	Ambcolor.r = Ambcolor.g = Ambcolor.b = 0x58;
	Ambcolor.a = 255;
	color.r = color.g = color.b = 100;
	color.a = 255;
	attraction = 1.0f;

	pMesh = NULL;
	parentedContainer = NULL;
	BlobMaterialName[0] = 0;
	kval = NULL;
	CustomMat = NULL;
	first = false;
	m_nLastUpdateFrame = -1;

	cubeGrid.CreateMemory();
}

C_PointBlobContainer::~C_PointBlobContainer()
{
	cubeGrid.FreeMemory();
}

//-----------------------------------------------------------------------------
void C_PointBlobContainer::Spawn( void )
{
	BaseClass::Spawn();

	m_nRenderFX = 3;
}

//-----------------------------------------------------------------------------
// The grid starts at the entity origin and spans GridBounds.
//-----------------------------------------------------------------------------
void C_PointBlobContainer::GetRenderBounds( Vector &mins, Vector &maxs )
{
	mins.Init( 0, 0, 0 );
	maxs = GridBounds;
}

//-----------------------------------------------------------------------------
void C_PointBlobContainer::Simulate( void )
{
	BaseClass::Simulate();

	UpdateContainer();
}

//-----------------------------------------------------------------------------
// Purpose: Gather every metaball in the level and accumulate their field
//			on the grid.
//-----------------------------------------------------------------------------
void C_PointBlobContainer::UpdateContainer( void )
{
	if ( m_nLastUpdateFrame == gpGlobals->framecount )
		return;
	m_nLastUpdateFrame = gpGlobals->framecount;

	// First run: build the grid and the material.
	if ( !first )
	{
		first = true;

		cubeGrid.Init( GridSize, GetAbsOrigin(), GridBounds );

		if ( BlobMaterialName[0] )
		{
			PrecacheMaterial( BlobMaterialName );
			CustomMat = materials->FindMaterial( BlobMaterialName, TEXTURE_GROUP_OTHER );
		}
		else
		{
			char szName[ 64 ];
			Q_snprintf( szName, sizeof( szName ), "blobik_%d.vmt", entindex() );

			kval = new KeyValues( "UnlitGeneric" );
			kval->SetString( "$basetexture", "vgui/white" );
			kval->SetInt( "$vertexcolor", 1 );
			kval->SetInt( "$nocull", 1 );
			CustomMat = materials->CreateMaterial( szName, kval );
		}
	}

	// A container can be parented to another one and then shares its blobs.
	parentedContainer = dynamic_cast<C_PointBlobContainer *>( GetMoveParent() );

	if ( parentedContainer && parentedContainer != this )
	{
		metaballs = parentedContainer->metaballs;
	}
	else if ( cl_blobs_updatecontainers.GetBool() || (metaballs.Count() == 0) )
	{
		metaballs.RemoveAll();
		for ( C_BaseEntity *pEnt = ClientEntityList().FirstBaseEntity(); pEnt; pEnt = ClientEntityList().NextBaseEntity( pEnt ) )
		{
			C_PointBlobElement *pBlob = dynamic_cast<C_PointBlobElement *>( pEnt );
			if ( pBlob )
			{
				metaballs.AddToTail( pBlob );
			}
		}
	}

	if ( !r_drawblobs.GetBool() )
		return;

	// Keep the grid on the entity if it moves.
	if ( cubeGrid.vertices )
	{
		cubeGrid.Init( GridSize, GetAbsOrigin(), GridBounds );
	}

	UpdateMeshData( 0, metaballs.Count() );
}

//-----------------------------------------------------------------------------
// Purpose: Add the field of metaballs [iFirst, iLast) to every grid vertex.
//-----------------------------------------------------------------------------
void C_PointBlobContainer::UpdateMeshData( int iFirst, int iLast )
{
	if ( !cubeGrid.vertices )
		return;

	const float flThreshold = cl_blobvaluethreshold.GetFloat();

	for ( int i = iFirst; i < iLast; i++ )
	{
		C_PointBlobElement *pBall = metaballs[i];
		if ( !pBall )
			continue;

		const Vector &ballPos = pBall->GetAbsOrigin();
		const float flStrength = pBall->GetRadiusSquared() * attraction;

		for ( int v = 0; v < cubeGrid.numVertices; v++ )
		{
			SURFACE_VERTEX &vert = cubeGrid.vertices[v];

			Vector delta = vert.pos - ballPos;
			float d2 = delta.LengthSqr();
			if ( d2 == 0.0f )
				continue;

			float f = flStrength / d2;
			if ( f <= flThreshold )
				f = 0.0f;

			vert.value += f;
			vert.normal += delta * ( flStrength / ( d2 * d2 ) );
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: Polygonise the field with marching cubes and draw it.
//-----------------------------------------------------------------------------
static void InterpolateEdge( EDGE_VERTEX &out, const SURFACE_VERTEX &a, const SURFACE_VERTEX &b, float iso )
{
	float dv = b.value - a.value;
	float t = ( fabsf( dv ) < 0.00001f ) ? 0.5f : ( iso - a.value ) / dv;
	t = clamp( t, 0.0f, 1.0f );

	out.pos = a.pos + ( b.pos - a.pos ) * t;
	out.normal = a.normal + ( b.normal - a.normal ) * t;
	VectorNormalize( out.normal );
}

int C_PointBlobContainer::DrawModel( int flags )
{
	if ( !r_drawblobs.GetBool() )
		return 0;

	UpdateContainer();

	if ( !cubeGrid.vertices || !cubeGrid.cubes || !CustomMat || (metaballs.Count() == 0) )
		return 0;

	const float iso = cl_blobthreshold.GetFloat();

	struct BlobTri_t
	{
		EDGE_VERTEX v[3];
	};
	CUtlVector<BlobTri_t> tris;

	for ( int c = 0; c < cubeGrid.numCubes; c++ )
	{
		const CUBE &cube = cubeGrid.cubes[c];

		int index = 0;
		for ( int k = 0; k < 8; k++ )
		{
			if ( cube.verts[k]->value < iso )
				index |= ( 1 << k );
		}

		int edges = g_BlobEdgeTable[index];
		if ( edges == 0 )
			continue;

		EDGE_VERTEX edgeVerts[12];
		for ( int e = 0; e < 12; e++ )
		{
			if ( edges & ( 1 << e ) )
			{
				InterpolateEdge( edgeVerts[e], *cube.verts[ g_BlobEdgeCorners[e][0] ], *cube.verts[ g_BlobEdgeCorners[e][1] ], iso );
			}
		}

		for ( int t = 0; g_BlobTriTable[index][t] != -1; t += 3 )
		{
			int i = tris.AddToTail();
			tris[i].v[0] = edgeVerts[ g_BlobTriTable[index][t] ];
			tris[i].v[1] = edgeVerts[ g_BlobTriTable[index][t + 1] ];
			tris[i].v[2] = edgeVerts[ g_BlobTriTable[index][t + 2] ];
		}
	}

	if ( tris.Count() == 0 )
		return 0;

	static const float s_flUV[3][2] = { { 1, 1 }, { 1, 0 }, { 0, 1 } };

	CMatRenderContextPtr pRenderContext( materials );
	pRenderContext->Bind( CustomMat );

	const int nBatch = 4000;	// triangles per dynamic mesh (stays under the 32768 vertex limit)
	for ( int start = 0; start < tris.Count(); start += nBatch )
	{
		int n = MIN( nBatch, tris.Count() - start );

		pMesh = pRenderContext->GetDynamicMesh();
		CMeshBuilder meshBuilder;
		meshBuilder.Begin( pMesh, MATERIAL_TRIANGLES, n );

		for ( int t = 0; t < n; t++ )
		{
			for ( int v = 0; v < 3; v++ )
			{
				const EDGE_VERTEX &ev = tris[start + t].v[v];

				// Simple directional shading between the ambient and the main colour.
				float flShade = clamp( 0.5f + 0.5f * ev.normal.z, 0.0f, 1.0f );
				float r = Ambcolor.r + ( color.r * colorBoost - Ambcolor.r ) * flShade;
				float g = Ambcolor.g + ( color.g * colorBoost - Ambcolor.g ) * flShade;
				float b = Ambcolor.b + ( color.b * colorBoost - Ambcolor.b ) * flShade;

				meshBuilder.Position3fv( ev.pos.Base() );
				meshBuilder.Normal3fv( ev.normal.Base() );
				meshBuilder.TexCoord2f( 0, s_flUV[v][0], s_flUV[v][1] );
				meshBuilder.Color4ub( clamp( (int)r, 0, 255 ), clamp( (int)g, 0, 255 ), clamp( (int)b, 0, 255 ), 255 );
				meshBuilder.AdvanceVertex();
			}
		}

		meshBuilder.End();
		pMesh->Draw();
	}

	return 1;
}
