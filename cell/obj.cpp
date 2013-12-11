// obj.cc - read Wavefront "obj" file format for 3-D models
// This understands only a subset of the file format, namely
//	# comment
//	v <X> <Y> <Z>
//	f <V1> <V2> ... <VN>
//
// Paul Heckbert	10 Feb 1999


// Most of the code in this file is only relevant
// if you need to modify objReadCell

// If you can think of a more elegant way to do this, that avoids the creation
// of all these temporary data structures, and going through the model twice,
// please let me know.

#include <stdlib.h>
#include <string.h>	// for strcmp
#include <fstream>	// for file I/O
#include <iomanip>	// for file input

// I can't believe these losers generate a million 10 line errors if you
// have the audacity to use the STL.
#pragma warning( disable : 4786 )
#include <map>
#include <vector>

#include <assert.h>
#include <stdio.h>

#include "../svl/SVL.h"

#include "list.hh"
#include "array.hh"
#include "cell.hh"

// ---------------------------- some data structures used by objReadCell only

struct Tvert;

typedef List<Tvert> Vlist;	// linked list of pointers to vertices

struct Tface {			// a (temporary) face
    Vlist vlist;		// the vertices of this face, in ccw order
    int   no;                   // face number
    Face *face;                 // final face in cell, null if not inst. yet
    // need anything else in here??
};

std::ostream &operator << (std::ostream &s, const Tface &f)	// print a Tface
    { return s << 'f' << f.no << " with vertices " << f.vlist; }

class Tsector {
public:

  Tvert *p;     // first ccw vertex
  Tface *f;     // intervening face
  Tvert *q;     // second ccw vertex

  Tsector(Tvert *p, Tface *f, Tvert *q)
    { this->p = p; this->f = f, this->q = q; }

};

typedef List<Tsector> Arc;	// arc of consec. edges emanating from a vertex
				// in counterclockwise order
				// (a linked list of pointers to other vertex)

typedef List<Arc> Arclist;	// unordered collection of arcs about a vertex
				// (a linked list of pointers to Arcs)
				// when done, this (linear) list contains
				// the ccw cycle of edges about a vertex

// For example, for vertex v below,
//
//       c------ b------i
//      / \     /      /
//     /   \   /      /
//    /     \ /      /
//   d------ v -----a--h
//    \     / \        |
//     \   /   \       |
//      \ /     \      |
//       e-------f-----g
//
// some valid Arcs are the lists (a,b), (a,b,c), (b,c), (c,d),
// (f,a), (e,f,a,b), etc. because those are the other endpoints of
// edges emanating from v, in counterclockwise (ccw) order.
// An arc always consists of at least two vertices.
// A valid Arclist is any set of disjoint arcs, in arbitrary order.
// When done, the Arclist for this vertex would be a single Arc.
// It would be a cyclic permutation of (a,b,c,d,e,f).

struct Tvert {			// a (temporary) vertex
    int no;			// ??for debugging
    int done;			// is topology fully set & arclist complete?
    Vec3 p;			// position
    Arclist arclist;		// info about the vertices adjacent to this one
    Vertex *vertex;             // final vertex in cell, null if not id. yet
    int instantiated;           // true if identified and instantiated
};

std::ostream &operator << (std::ostream &s, const Tvert &t)	// print a Tvert
    { return s << t.no; }

std::ostream &operator << (std::ostream &s, const Tsector &sector)	// print a Tsector
    { return s << sector.p->no << "-" << sector.q->no; }

// ---------------------------- obj file input

static void merge_arc(Tvert *v, Tvert *p, Tvert *q, Tface *f) {
    // Merge the arc (p,q) into the list of arcs around vertex v.
    // Cases:
    //  1. ( bef &&  aft) it connects two existing arcs
    //  2. ( bef && !aft) it goes on the end of an existing arc
    //  3. (!bef &&  aft) it goes on the beginning of an existing arc
    //  4. (!bef && !aft) it does not connect with an existing arc
    // std::cout << "merge_arc " << *v << " " << *p << " " << *q << std::endl;
    // std::cout << "before, arclist=" << v->arclist;
    List_item<Arc> *a, *aft_item;
    Arc *bef = 0, *aft = 0;
    Tsector *sector = new Tsector(p, f, q);
    for (a=v->arclist.first(); a; a=a->next()) {
	// a->obj is an Arc
	if (a->obj->last()->obj->q==p) bef = a->obj;
	if (a->obj->first()->obj->p==q) {aft = a->obj; aft_item = a;}
    }
    // std::cout << "  bef=" << *bef << "  aft=" << *aft;
    // now concatenate the three arcs bef, (p,q), and aft
    // where bef and aft might be null
    if (bef) {
	if (aft) {	// 1. ( bef &&  aft) it connects two existing arcs
	    bef->append(sector);		// insert new sector
	    if (bef==aft) {
		// done with vertex! connecting these would make arc circular
		// std::cout << v->arclist << " done" << std::endl;
		v->done = 1;
		return;
	    }
	    // now we'll merge two arcs in the arclist
	    v->arclist.remove(aft_item);	// remove following arc
	    bef->concat(aft);			// and concat it into previous
	}
	else		// 2. ( bef && !aft) it goes on the end of existing arc
	    bef->append(sector);
    }
    else {
	if (aft)	// 3. (!bef &&  aft) it goes on beg. of existing arc
	    aft->prepend(sector);
	else {		// 4. (!bef && !aft) it doesn't connect w. existing arc
	    Arc *arc = new Arc;
	    assert(arc);
	    arc->append(sector);
	    v->arclist.append(arc);
	}
    }
    // std::cout << "after, arclist=" << v->arclist;
}

static void add_arcs(Vlist &vlist, Tface *f) {
    // std::cout << "add_arcs " << vlist;
    // vlist is not a circular list, but we need to step through all
    // consecutive triples as if it were circular
    List_item<Tvert> *u, *v, *w;
    for (u=vlist.last(), v=vlist.first(), w=v->next(); w; u=v, v=w, w=w->next())
	merge_arc(v->obj, w->obj, u->obj, f);
    merge_arc(v->obj, vlist.first()->obj, u->obj, f);  // one more that we missed
}

/*
 * identified   <=> Tvert has been associated with a particular Vertex
 * instantiated <=> Tface has been associated with a particular Face AND
 *                  all vertices of the face have been identified
 * instantiated <=> Tvert has been identified AND
 *                  all adjacent Tfaces have been instantiated
 */

/*
 * Return true if a given pair of vertices is connected directly by an edge
 * along a given left face.
 * vertex1, vertex2 -> the vertices to check;
 *                     must be nonnull
 * left             -> the left face to check for;
 *                     must be nonnull
 * <- true if there is an edge from _vertex1_ to _vertex2_ with left face
 *    _left_
 */
static int isConnected(Vertex *vertex1, Vertex *vertex2, Face *left)
{
  assert(vertex1!=0);
  assert(vertex2!=0);
  assert(left!=0);

  // check the orbit of vertex1 for an edge to vertex2

  VertexEdgeIterator edges(vertex1);

  Edge *edge;

  while ((edge = edges.next())!=0)
    if (edge->Dest()==vertex2 && edge->Left()==left)
      return 1;

  return 0;
}

/*
 * Return the face to the right of a given face around a given vertex.
 * vertex -> the vertex to look for the face around;
 *           must be nonnull
 * left   -> the left face to return the right face of;
 *           must be nonnull
 * <- the face to the right of _left_ around _vertex_;
 *    null if none
 */
static Face *RightFace(Vertex *vertex, Face *left)
{
  assert(vertex!=0);
  assert(left!=0);

  // check the left face of each edge in the orbit of the vertex

  Edge *start = vertex->getEdge();
  Edge *scan  = start;

  do
  {
    if (scan->Left()==left)
      return scan->Right();

    scan = scan->Onext();
  }
  while (scan!=start);

  return 0;
}

/*
 * Return true if a given vertex is adjacent to a given face.
 * face   -> the face to look for the vertex in;
 *           must be nonnull
 * vertex -> the vertex to look for;
 *           must be nonnull
 * <- true if _vertex_ is on _face_
 */
static int hasVertex(Face *face, Vertex *vertex)
{
  assert(face!=0);
  assert(vertex!=0);

  // check the origin vertex of each edge on the face

  FaceEdgeIterator edges(face);

  Edge *edge;

  while ((edge = edges.next())!=0)
    if (edge->Org()==vertex)
      return 1;

  return 0;
}

/*
 * Return true if a given face includes all the identified vertices on a given
 * Tvert list.
 * face  -> the face to check;
 *          must be nonnull
 * vlist -> the vertex list to check against;
 *          must be nonnull
 * <- true if _face_ is adjacent to all the vertices on _vlist_
 */
static int hasVertices(Face *face, Vlist *vlist)
{
  assert(face!=0);
  assert(vlist!=0);

  // check each vertex on the list

  for (List_item<Tvert> *vi = vlist->first(); vi!=0; vi = vi->next())
  {
    Vertex *vertex = vi->obj->vertex;

    if (vertex!=0 && !hasVertex(face, vertex))
      return 0;
  }

  return 1;
}

/*
 * Return a face that can be used to instantiate a given Tface.
 * cell -> the cell to get the face from;
 *         must be nonnull
 * f    -> Tface to get the face for;
 *         must be nonnull
 * <- a face that can be used to instantiate _f_;
 *    null if none are available
 */
static Face *getFace(Cell *cell, Tface *f)
{
  assert(cell!=0);
  assert(f!=0);

  // locate all the unused faces in the cell

  Face       **faces = new Face*[cell->countFaces()];
  unsigned int count = 0;

  {
    CellFaceIterator iterator(cell);

    Face *face;

    while ((face = iterator.next())!=0)
      if (face->data==0)
	faces[count++] = face;
  }

  // discard any faces that don't include all the identified vertices of the
  // Tface

  {
    unsigned int i = 0;

    while (i<count)
    {
      Face *face = faces[i];

      if (hasVertices(face, &f->vlist))
	i++;
      else
	faces[i] = faces[--count];
    }
  }

  Face *face = count>0 ? faces[0] : 0;

  delete[] faces;

  return face;
}

/*
 * Instantiate a given Tface in a given cell by identifying its vertices.
 * cell -> the cell to instantiate the face in;
 *         must be nonnull
 * f    -> the Tface to instantiate;
 *         must be nonnull
 */
static void makeFace(Cell *cell, Tface *f)
{
  assert(cell!=0);
  assert(f!=0);

  // get the face to use for the Tface

  Face *face = getFace(cell, f);

  assert(face!=0);

  // connect all pairs of identified vertices on the face, as necessary

  {
    for (List_item<Tvert> *vi = f->vlist.first(); vi!=0; vi = vi->next())
    {
      Vertex *vertex1 = vi->obj->vertex;
      Vertex *vertex2;

      if (vertex1!=0)
      {
	// find the next identified vertex, even if just itself

	List_item<Tvert> *vj = vi;

	for (;;)
	{
	  vj = vj->next();

	  if (vj==0)
	    vj = f->vlist.first();

	  vertex2 = vj->obj->vertex;

	  if (vertex2!=0)
	    break;
	}

	// connect the vertices, if necessary

	if (!isConnected(vertex1, vertex2, face))
	  (void)cell->makeFaceEdge(face, vertex1, vertex2)->Right();
      }
    }
  }

  // find the first identified vertex

  List_item<Tvert> *vi0 = f->vlist.first();

  while (vi0->obj->vertex==0)
    vi0 = vi0->next();

  // identify all the following and preceding vertices

  List_item<Tvert> *vi     = vi0;
  Vertex           *vertex = vi0->obj->vertex;

  for (;;)
  {
    vi = vi->next();

    if (vi==0)
      vi = f->vlist.first();

    if (vi==vi0)
      break;

    Tvert *v = vi->obj;

    if (v->vertex==0)
    {
      Face *right = RightFace(vertex, face);

      assert(right!=0);

      v->vertex      = cell->makeVertexEdge(vertex, face, right)->Dest();
      v->vertex->pos = v->p;

      v->vertex->setID(v->no);
    }

    vertex = v->vertex;
  }

  // the face is now instantiated

  f->face = face;

  face->setID(f->no);
  face->data = f;
}

/*
 * Instantiate a given identified Tvert in a given cell by instantiating its
 * adjacent faces.
 * cell -> the cell to instantiate the Tvert in;
 *         must be nonnull
 * v    -> the Tvert to instantiate;
 *         must be nonnull
 */
static void makeVertex(Cell *cell, Tvert *v)
{
  assert(cell!=0);
  assert(v!=0);

  // find the first sector with an identified p vertex

  List_item<Tsector> *wi0 = v->arclist.first()->obj->first();

  while (wi0->obj->p->vertex==0)
    wi0 = wi0->next();

  // instantiate all following sectors of the vertex in counterclockwise order

  List_item<Tsector> *wi = wi0;

  do
  {
    Tface *f = wi->obj->f;

    if (f->face==0)
      makeFace(cell, f);

    wi = wi->next();

    if (wi==0)
      wi = v->arclist.first()->obj->first();
  }
  while (wi!=wi0);

  // the vertex is now instantiated

  v->instantiated = 1;
}

static void print_quadedge(Array<Tvert> verts, List<Tface> faces) {
    // print vertices around each face and vertex currently

    std::cout << "VERTICES OF EACH FACE:" << std::endl;
    List_item<Tface> *fi;
    for (fi=faces.first(); fi; fi=fi->next()) {
	std::cout << "face:";
	List_item<Tvert> *vi;
	for (vi=fi->obj->vlist.first(); vi; vi=vi->next())
	    std::cout << " " << vi->obj->no;
	std::cout << std::endl;
    }
    std::cout << std::endl;

    std::cout << "VERTICES AROUND EACH VERTEX:" << std::endl;
    int i;
    for (i=0; i<verts.num(); i++) {
	Tvert *v;
	v = &verts[i];
	std::cout << "around vertex " << v->no << ":";
	assert(v->done);
	assert(v->arclist.length()==1);
	// step through the Tverts in the first (and only) arc of arclist
	List_item<Tsector> *wi;
	for (wi=v->arclist.first()->obj->first(); wi; wi=wi->next()) {
	    Tsector *sector = wi->obj;
	    std::cout << " " << *sector;
	}
	std::cout << std::endl;
    }
}

void check_closed(Array<Tvert> verts) {
    // check to see if polyhedron is closed (else we'd crash soon anyway)
    int i;
    for (i=0; i<verts.num(); i++) {
	Arclist &al = verts[i].arclist;
	if (!verts[i].done || al.length()!=1) {
	    if (al.length()==0)
		std::cerr << "\nERROR in OBJ file: unused vertex "
		    << verts[i].no << std::endl;
	    else if (!verts[i].done)
		std::cerr << "\nERROR in OBJ file: vertex " << verts[i].no
		    << " is not surrounded by polygons" << std::endl;
	    else
		std::cerr << "\nERROR in OBJ file: repeated face: " <<
			*al.first()->next()->obj->first()->obj->f << std::endl;
	    exit(1);
	}
    }
}

static Cell *build_quadedge(Array<Tvert> verts, List<Tface> faces) {

  check_closed(verts);

  // create a cell and fetch its initial vertex

  Cell *cell = Cell::make();

  Vertex *vertex1;

  {
    CellVertexIterator vertices(cell);

    vertex1 = vertices.next();
  }

  // instantiate a face of the initial vertex

  {
    Tvert *v = &verts[0];

    v->vertex      = vertex1;
    v->vertex->pos = v->p;

    v->vertex->setID(v->no);

    makeFace(cell, v->arclist.first()->obj->first()->obj->f);
  }

  // instantiate identified vertices until all are instantiated

  for (;;)
  {
    int instantiated = 1;

    for (int i = 0; i<verts.num(); i++)
    {
      Tvert *v = &verts[i];

      if (v->vertex!=0 && !v->instantiated)
	makeVertex(cell, v);

      instantiated &= v->instantiated;
    }

    if (instantiated)
      break;
  }

  // reset the data pointers of all faces

  {
    CellFaceIterator iterator(cell);

    Face *face;

    while ((face = iterator.next())!=0)
      face->data = 0;
  }

  return cell;
}

#if 0
static Cell *objReadCell(istream &s, const char *streamname) {

    // warning: this routine does a lousy job of checking for errors
    // (e.g. spaces at the end of input lines)
    // that should be fixed!

    char tok[20];

    Array<Tvert> verts;		// all the vertices
    int nvert = 0;		// current vertex number (counts up)
    int nface = 0;		// current face number (counts up)

    List<Tface> faces;		// all the faces

    while (s >> setw(sizeof tok) >> tok) {
        // std::cout << "(" << tok << ")" << std::endl;
	if (!strcmp(tok, "v")) {		// vertex command
	    nvert++;
	    double x, y, z;
	    s >> x >> y >> z;
	    // std::cout << "verts[" << nvert << "]=" << Vec3(x, y, z) << std::endl;
	    verts[nvert-1].p = Vec3(x, y, z);
	    verts[nvert-1].no = nvert;
	    verts[nvert-1].done = 0;
	    verts[nvert-1].vertex = 0;
	    verts[nvert-1].instantiated = 0;
	    // OBJ vertex numbering starts at 1, but we shift them
	    // all to start at 0 for indexing into vert[]
	}
	else if (!strcmp(tok, "f")) {	// face command
	    nface++;
	    Tface *f = new Tface;
	    f->face = 0;
	    f->no   = nface;
	    assert(f);
	    faces.append(f);
	    int n;
	    for (n=0; s.peek()!='\n'; n++) {
		int v;
		// std::cout << "  peek='" << (char)s.peek() << "' ";
		s >> v;
		// std::cout << "  got " << v << std::endl;
		v--;		// we start numbering at 0, not 1
		f->vlist.append(&verts[v]);
	    }
	    // std::cout << "done gobbling" << std::endl;
	    // std::cout << "f " << f->vlist;
	    add_arcs(f->vlist, f);		// add the topological info in face f
	}
	else if (!strcmp(tok, "#")) {
	    // gobble comment
	    s.ignore(1000, '\n');
	}
	else {
	    std::cerr << "objReadCell: I can't parse this OBJ file, hit token (" << tok
		<< ")" << std::endl;
	    exit(1);
	}
    }
    /*
    std::cout << "obj file " << streamname << " contained "
	<< nvert << " vertices, "
	<< faces.length() << " faces "
	<< std::endl;
    */

    // print_quadedge(verts, faces);

    return build_quadedge(verts, faces);
}
#endif

static Cell *objReadCell(std::istream &s, const char *streamname)
// Reads a limited class of obj files.
// Rewritten by Daniel Wood so as to load models with genus != 0.
// I've realized that if we don't insist that the intermediate data
// structures be valid (e.g. pointer invariants) we can do thsi much easier.
// We have to break the encapsulation of cell,edge,vertex, and face,
// but I think it's probably worth it.
//
// Also note that this procedure doesn't do any error checking.
// A simple no-coding way to fix this would be to hack Heckbert's loader
// just for the checking.  Better would be to write a simple checker.
// (Mesh must be closed, no repeated edges, etc.)
{

    // warning: this routine does a lousy job of checking for errors
    // (e.g. spaces at the end of input lines)
    // that should be fixed!
    char tok[20];

	Cell *cell = new Cell;

	// Load all the vertices and faces into an expandable array.
	std::vector<Vertex*> verts;
	std::vector<Face*> faces;
	// We'll map from an edge to a number by combining its two vertex ids.
	std::map<int,Edge*> numedge;

#define ENO(a,b) ( (a)*(verts.size()+1)+(b) )

	// Here's the plan.
	// 1. First load all vertices.  Let edge pointer dangle.
	// 2. Then load all faces
	// 3. For each face edge:
	//    a. If edge hasn't been made, make it
	//    b. Can fix vertex -> edge pointers now
	//    c. And can do edge -> vertex pointers
	//    d. For edge -> face pointers put only index
	//    e. For each edge (for each face) we can set two of its next ptrs
	//       we get the other two on the other face.
	//       We set the next pointers to ENOs instead of real pointers
	while (s >> std::setw(sizeof tok) >> tok)
	{
		if( !strcmp(tok, "v") )
		{		// vertex command
			if( faces.size() )
			{
				// We must first do all vertices THEN all faces.
				goto cleanup;
			}
			double x, y, z;
			s >> x >> y >> z;
			Vertex *v = new Vertex(cell);
			v->pos = Vec3(x,y,z);
			v->id = verts.size() + 1;
			v->cell = cell;
			verts.push_back( v );
		}
		else if( !strcmp(tok, "f") )
		{
			std::vector<int> ids; // vertex IDs
			for( int n=0; s.peek()!='\n'; n++ )
			{
				int v;
				s >> v;
				ids.push_back(v);
			}
			Face *f = new Face(cell);
			f->id = faces.size()+1;
			f->cell = cell;
			for( int i = 0; i < ids.size(); ++i )
			{
				int id1 = ids[i];
				int id2 = ids[(i+1)%ids.size()]; // two ids of endpoints of edge
				int id3 = ids[(i+2)%ids.size()]; // next one
				int id0 = ids[(i+ids.size()-1)%ids.size()]; // before id1

				std::map<int,Edge*>::iterator ei;
				Edge *e;
				if( (ei = numedge.find(ENO(id1,id2)) ) != numedge.end() )
				{
					e = ei->second;
				}
				else
				{
					e = Edge::make();
					numedge[ENO(id1,id2)] = e;
					numedge[ENO(id2,id1)] = e->Sym();
				}
				
				e->Sym()->face = (Face*) f->id;
				e->vertex = 0;
				e->next = (Edge*) ENO( id2, id3 );
				e->Rot()->face = 0;
				e->Rot()->vertex = verts[id1-1];
				e->Rot()->next = (Edge*) ENO( id1, id0 );
				
				f->edge = e->Rot();
				verts[id1-1]->edge = e->Rot();
				verts[id2-1]->edge = e->InvRot();
			}
			faces.push_back(f);
		}
		else if( !strcmp(tok, "#") )
		{
			s.ignore(1000, '\n');	// gobble comment
		}
		else
		{
			// std::cerr << "objReadCell: I can't parse this OBJ file, hit token (" << tok << ")" << std::endl;
			goto cleanup;
		}
	}

	{
	// OK.  Now have to replace all of the face ids with pointers and all the edge ids with
	// pointers too.
	for( std::map<int,Edge*>::iterator ei = numedge.begin(); ei != numedge.end(); ++ei )
	{
		Edge *e = ei->second;
		e->face = faces[reinterpret_cast<int>(e->face)-1];
		e->next = numedge[reinterpret_cast<int>(e->next)];
		e->Rot()->next = numedge[reinterpret_cast<int>(e->Rot()->next)]->Rot();
	}
	}
	
	return cell;

cleanup:
	{
	for( std::vector<Vertex*>::iterator vi = verts.begin(); vi != verts.end(); ++vi )
		delete *vi;
	for( std::vector<Face*>::iterator fi = faces.begin(); fi != faces.end(); ++fi )
		delete *fi;
	for( std::map<int,Edge*>::iterator ei = numedge.begin(); ei != numedge.end(); ++ei )
		; // do something. TODO.
	}
	return 0;
}

Cell *objReadCell(const char *file) {
    std::ifstream s(file, std::ios::in);
    if (!s) {
	std::cerr << "objReadCell: can't read " << file << std::endl;
	return 0;
    }
    return objReadCell(s, file);
}

#ifdef OBJ_MAIN	// compile with -DOBJ_MAIN to test objReadCell

void main(int argc, char **argv) {
    if (argc!=2) exit(1);
    objReadCell(argv[1]);
}

#endif

static void objWriteCell(Cell *cell, std::ostream &s, const char *streamname)
{
  // renumber vertices in current order
  // yuk: should really leave ids intact ???

  {
    CellVertexIterator vertices(cell);

    Vertex      *vertex;
    unsigned int id = 1;

    while ((vertex = vertices.next())!=0)
      vertex->setID(id++);
  }

  // write vertices in same order: explicit ids become implicit ids

  s << "# " << cell->countVertices() << " vertices\n";

  {
    CellVertexIterator vertices(cell);

    Vertex *vertex;

    while ((vertex = vertices.next())!=0)
      s << "v " << vertex->pos[0] << " "
	        << vertex->pos[1] << " "
	        << vertex->pos[2] << "\n";
  }

  // write faces in any order

  s << "# " << cell->countFaces() << " faces\n";

  {
    CellFaceIterator faces(cell);

    Face *face;

    while ((face = faces.next())!=0)
    {
      s << "f";

      FaceEdgeIterator edges(face);

      Edge *edge;

      while ((edge = edges.next())!=0)
	s << " " << edge->Org()->getID();

      s << "\n";
    }
  }
}

void objWriteCell(Cell *cell, const char *file)
{
    std::ofstream s(file, std::ios::out);
    if (!s) {
	std::cerr << "objWriteCell: can't write " << file << std::endl;
	return;
    }
    objWriteCell(cell, s, file);
}

#if 0

Cell *objCloneCell(Cell *cell)
{
  // renumber vertices in current order
  // yuk: should really leave ids intact ???

  {
    CellVertexIterator vertices(cell);

    Vertex      *vertex;
    unsigned int id = cell->countVertices();

    while ((vertex = vertices.next())!=0)
      vertex->setID(id--);
  }

  Array<Tvert> verts;		// all the vertices
  int nvert = cell->countVertices();		// current vertex number (counts down)

  List<Tface> faces;		// all the faces
  int nface = 0;		// current face number (counts up)

  // make the Tverts

  {
    CellVertexIterator vertices(cell);

    Vertex *vertex;

    while ((vertex = vertices.next())!=0)
    {

      verts[nvert-1].p = vertex->pos;
      verts[nvert-1].no = nvert;
      verts[nvert-1].done = 0;
      verts[nvert-1].vertex = 0;
      verts[nvert-1].instantiated = 0;
      nvert--;
      // OBJ vertex numbering starts at 1, but we shift them
      // all to start at 0 for indexing into vert[]
    }
  }

  // make the Tfaces

  {
    CellFaceIterator iterator(cell);

    Face *face;

    while ((face = iterator.next())!=0)
    {
      nface++;
      Tface *f = new Tface;
      f->face = 0;
      f->no   = nface;
      assert(f);
      faces.append(f);
      {
	FaceEdgeIterator edges(face);

	Edge *edge;

	while ((edge = edges.next())!=0)
	  f->vlist.append(&verts[edge->Org()->getID()-1]);
      }
      add_arcs(f->vlist, f);		// add the topological info in face f
    }
  }

  // go to town
  return build_quadedge(verts, faces);
}
#endif

// Let's try recoding this in an entirely different fashion.
// This function copies one cell to another by delving into its
// internal structure.  Thereby violating the encapsulation.
// (Encapsulation is said here in a falsetto baby-voice.)
// It works, though.

Cell *
objCloneCell(Cell *cell)
{
	Cell *babe = new Cell; // babe in arms . . .
	
	babe->vertexCount = cell->vertexCount;
	babe->vertexSize = cell->vertexSize;
	babe->vertices = new Vertex *[ babe->vertexSize ];
	babe->vertexID = cell->vertexID;

	babe->faceCount = cell->faceCount;
	babe->faceSize = cell->faceSize;
	babe->faces = new Face *[ babe->faceSize ];
	babe->faceID = cell->faceID;

	int i;
	
	typedef std::map<int,Vertex*> IDVertex;
	IDVertex idvertex;
	for( i = 0; i < babe->vertexCount; ++i )
	{
		Vertex *v = new Vertex( *cell->vertices[i] );
		babe->vertices[i] = v;
		idvertex[v->id] = v;
	}

	typedef std::map<int,Face*> IDFace;
	IDFace idface;
	for( i = 0; i < babe->faceCount; ++i )
	{
		Face *f = new  Face( *cell->faces[i] );
		babe->faces[i] = f;
		idface[f->id] = f;
	}

	typedef std::pair<Edge*,Edge*> EP; // old edge, new edge
	typedef std::map<int,EP> IDEP;    // id -> ep
	IDEP idep;

						// Insert all the old edges into the map
						// and new ones too.  Index off of the id of the qe
	CellVertexIterator iter( cell );
	Vertex *v;
	while( v = iter.next() )
	{
		Edge *e = v->getEdge();
		Edge *start = e;
		do
		{
			Edge *eq = e - e->id % 4; // the first edge in the quad edge
			int qeid = eq->id;
			IDEP::iterator i = idep.find(qeid);
			if( i == idep.end() )
				idep[qeid] = std::make_pair( eq, Edge::make() );
			e = e->Onext();
		} while( e != start );
	}

						// OK.  Fix edge and cell vars in vertices.
	for( i = 0; i < babe->vertexCount; ++i )
	{
		babe->vertices[i]->cell = babe;
		int edgeID = babe->vertices[i]->edge->id;
		int quadID = 4 * (edgeID / 4);
		babe->vertices[i]->edge = idep[quadID].second+edgeID%4;
	}

						// OK.  Fix edge and cell vars in faces.
	for( i = 0; i < babe->faceCount; ++i )
	{
		babe->faces[i]->cell = babe;
		int edgeID = babe->faces[i]->edge->id;
		int quadID = 4 * (edgeID / 4);
		babe->faces[i]->edge = idep[quadID].second+edgeID%4;
	}

	{
						// Now do up all of the edge pointers.
	for( IDEP::iterator i = idep.begin(); i != idep.end(); ++i )
	{
		for( int cnt = 0; cnt < 4; ++cnt )
		{
			Edge *olde = i->second.first+cnt;
			Edge *newe = i->second.second+cnt;

			*newe = *olde; // copy generic stuff like sharpness

			newe->face = olde->face ? idface[olde->face->id] : 0;
			newe->vertex = olde->vertex ? idvertex[olde->vertex->id] : 0;
			newe->data = 0;
			int nextid = olde->next->id;
			newe->next = idep[4*(nextid/4)].second+nextid%4;
		}
	}
	}

	return babe;
}