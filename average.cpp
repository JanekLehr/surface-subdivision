// average.cc - class that performs average step of subdivision

#include <assert.h>
#include <math.h>

#include "cell/cell.hh"
#include "cell/face.hh"
#include "cell/vertex.hh"

#include "subdiv.hh"
#include "average.hh"

#define PI 3.14159265

void AvgNOOP::operator()( Cell *cell )
{
	genNormals( cell );
}

void AvgNOOP::genNormal( Vertex *v )
// Generate a normal for Vertex v by averaging together the normals of all of its
// neighboring faces.
{
	Edge *e = v->getEdge();
	Edge *e2 = e->Onext();

	// e,e2 are two successive edges of v.
	Edge *start = e;

	Vec3 vpos = v->pos;
	int cnt = 0;
	Vec3 sumNormal( 0,0,0 );
	do
	{
		++cnt;
					// add into sum the face normal
		sumNormal += norm( cross(e->Dest()->pos-vpos,e2->Dest()->pos-vpos) );
		e = e2;
		e2 = e->Onext();
	} while( e != start );

	v->nor = sumNormal / double(cnt);
}

void AvgNOOP::genNormals( Cell *cell )
// Generate normals for all of the vertices in the cell.
{
	CellVertexIterator verts(cell);
	Vertex *v;
	while( (v = verts.next()) != 0 )
		genNormal( v );
}

void AvgAdHoc::operator()( Cell *cell )
{
	// 1. Generate new positions.  (Put new pos into normal for the time being.)
	{
		CellVertexIterator verts(cell);
		Vertex *v;
		while( (v = verts.next()) != 0 )
			if( !interpolating || v->tag==VODD )
				average( v );
	}
	// 2. Copy positions out of nor into pos.
	{
		CellVertexIterator verts(cell);
		Vertex *v;
		while( (v = verts.next()) != 0 )
			if( !interpolating || v->tag==VODD )
				v->pos = v->nor;
	}
	// 3. Generate new normals.
	AvgNOOP::genNormals( cell );
}

void AvgAdHoc::average( Vertex *v )
// Generate a vertex position for v by averaging the positions of its neighbors.
// Put result in normal field.  (Copy into pos later.)
{
	Edge *start = v->getEdge();
	Edge *e = start;
	v->nor = Vec3(0,0,0);
	int cnt = 0;
	do
	{
		++cnt;
		v->nor += e->Dest()->pos;
		e = e->Onext();
	} while( e != start );

	v->nor /= double(cnt);	
}


void AvgLooping::operator()(Cell *cell)
{
	// 1. Generate new positions.  (Put new pos into normal for the time being.)
	{
		CellVertexIterator verts(cell);
		Vertex *v;
		while ((v = verts.next()) != 0)
			average(v);
	}
	// 2. Copy positions out of nor into pos.
	{
	CellVertexIterator verts(cell);
	Vertex *v;
	while ((v = verts.next()) != 0)
		v->pos = v->nor;
}
	// 3. Generate new normals.
	AvgNOOP::genNormals(cell);
}

void AvgLooping::average(Vertex *v)
// Generate a vertex position for v by averaging the positions of its neighbors.
// Put result in normal field.  (Copy into pos later.)
{
	Edge *start = v->getEdge();
	Edge *e = start;
	 v->nor = Vec3(0, 0, 0);
	// v->nor = v->pos;
	int cnt = 0;
	do
	{
		++cnt;
		v->nor += e->Dest()->pos;
		e = e->Onext();
	} while (e != start);

	// add weighted v to the sum
	v->nor += alpha(++cnt)*v->pos;

	v->nor /= double(cnt) + alpha(cnt);
}

Double AvgLooping::beta(Int numNeighbors)
{
	return  (5.0 / 4.0) - (pow((3 + 2 * cos(2 * PI / numNeighbors)), 2)) / 32;
}

Double AvgLooping::alpha(Int numNeighbors)
{
	return numNeighbors*(1 - beta(numNeighbors)) / beta(numNeighbors);
}

