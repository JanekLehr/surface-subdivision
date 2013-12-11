// subdiv.cc - subdivision surfaces code
//
// Just the code to do 1-4 triangle splitting a la Loop
//
// Paul Heckbert	3 March 1999


#include <assert.h>
#include <math.h>

#include "cell/cell.hh"
#include "cell/face.hh"
#include "cell/vertex.hh"

#include "subdiv.hh"

// first some random helpers

int valence(Vertex *v) {
    // return vertex degree
    VertexEdgeIterator edges(v);
    int count = 0;
    while (edges.next())
        count++;
    return count;
}

int nsides(Face *f) {
    // return face degree
    FaceEdgeIterator edges(f);
    int count = 0;
    while (edges.next())
        count++;
    return count;
}
   
//---------------------------------------- subdivision (splitting)

void markAllVerticesEven(Cell *c) {
    // for all vertices, mark as EVEN
    CellVertexIterator cellVertices(c);
    Vertex *v;
    while ((v = cellVertices.next()) != 0)
	v->tag = VEVEN;
}

void printValences(Cell *c, char *str) {
    CellVertexIterator cellVertices(c);
    Vertex *v;
    std::cout << "valences " << str << std::endl;
    while ((v = cellVertices.next()) != 0)
	std::cout << " " << valence(v);
    std::cout << std::endl;
}

Edge *split(Edge *e) {
    // split edge e, putting new vertex at midpoint

    // get Cell pointer from vertex (Edges don't have one)
    Cell *c = e->Org()->getCell();

    // split, creating new edge and vertex (sets topology only)
    Edge *enew = c->makeVertexEdge(e->Org(), e->Left(), e->Right());

    assert(enew->Dest()==e->Org());
    // At this point enew->Dest()==e->Org(),
    // and enew->Dest(), the new vertex, is between enew and e.
    // You might want to check the defn of makeVertexEdge to
    // convince yourself of this.

    // position new vertex at midpoint (note use of Vec3::operator+)
    enew->Dest()->pos = .5*(enew->Org()->pos + e->Dest()->pos);

    return enew;	// return new edge
}

void splitAllEdges(Cell *c) {
    int count = 0;
    {
	// first, set the splitme bits of all original edges
	CellFaceIterator cellFaces(c);
	Face *f;
	while ((f = cellFaces.next()) != 0) {
	    // visit each face of cell c
	    FaceEdgeIterator faceEdges(f);
	    Edge *edge;
	    while ((edge = faceEdges.next()) != 0) {
		// visit each edge of face f
		// mark as unsplit
		edge->tag = EUNSPLIT;	// set bit
	    }
	}
    }
    {
	// go through again, splitting marked edges
	// need to construct a new iterator, hence the {}'s
	CellFaceIterator cellFaces(c);
	Face *f;

	while ((f = cellFaces.next()) != 0) {
	    // visit each face of cell c
	    FaceEdgeIterator faceEdges(f);
	    Edge *edge;
	    while ((edge = faceEdges.next()) != 0) {
		// visit each edge of face f
		// if it is unsplit then split it
		if (edge->tag==EUNSPLIT) {
		    Edge *enew = split(edge);
		    count++;

		    // mark old and new edges and their Syms as split and even
		    // to avoid recursive splitting and to distinguish them
		    // from odd edges, to be created later
		    edge->tag = EEVEN;
		    edge->Sym()->tag = EEVEN;
		    enew->tag= EEVEN;
		    enew->Sym()->tag = EEVEN;

		    // mark new vertex as odd
		    enew->Dest()->tag = VODD;
		}
	    }
	}
    }
    //std::cout << "split " << count << " edges" << std::endl;
}

void splitFace(Face *f) {
    // Assumes that face is triangular and that edges have already
    // been split to have vertices inserted in the middle of each edge.
    //
    //    BEFORE       AFTER
    //      *            *
    //     / \          / \         * = even
    // v1 o   o v3     o---o        o = odd
    //   /     \      / \ / \
    //  *---o---*    *---o---*
    //      v2

    // find the three midpoints

    Edge *e = f->getEdge();
    assert(e);
    if (e->Org()->tag!=VODD)
	e = e->Lnext();
    Edge *estart = e;

    assert(e);
    assert(e->Org()->tag==VODD);
    assert(e->Dest()->tag==VEVEN);
    Vertex *v1 = e->Org();
    e = e->Lnext()->Lnext();

    assert(e);
    assert(e->Org()->tag==VODD);
    assert(e->Dest()->tag==VEVEN);
    Vertex *v2 = e->Org();
    e = e->Lnext()->Lnext();

    assert(e);
    assert(e->Org()->tag==VODD);
    assert(e->Dest()->tag==VEVEN);
    Vertex *v3 = e->Org();
    e = e->Lnext()->Lnext();

    assert(e==estart);	// check that it was a triangle

    // connect 'em up and mark new edges and their Syms as odd
    Cell *c = f->getCell();
    e = c->makeFaceEdge(f, v1, v2);
    e->tag = EODD;
    e->Sym()->tag = EODD;
    e = c->makeFaceEdge(f, v2, v3);
    e->tag = EODD;
    e->Sym()->tag = EODD;
    e = c->makeFaceEdge(f, v3, v1);
    e->tag = EODD;
    e->Sym()->tag = EODD;
}

int splitAllFaces(Cell *c) {
    // Assumes that faces are triangular and that edges have already
    // been split to have vertices inserted in the middle of each edge.

    CellFaceIterator cellFaces(c);
    Face *f;
    int nfaces = 0;
    while ((f = cellFaces.next()) != 0) {
	splitFace(f);
	nfaces++;
    }
    return nfaces;
}

int subdivideOnce(Cell *cell) {
    markAllVerticesEven(cell);
    splitAllEdges(cell);
    int nfaces = splitAllFaces(cell);
    return nfaces;
}


int triangulated(Cell *c) {
    CellFaceIterator faces(c);
    Face *f;
    while ((f = faces.next()) != 0)
	if (nsides(f)!=3)
	    return 0;
    return 1;
}
