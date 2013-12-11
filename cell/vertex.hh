
/* ============================================================================
 * p2/cell/vertex.hh
 * ========================================================================= */

#ifndef vertexINCLUDED
#define vertexINCLUDED

#include <iostream>
#include "../svl/Vec3.h"

#include "edge.hh"

class Cell;

/* ----------------------------------------------------------------------------
 * Vertex
 * ------------------------------------------------------------------------- */

/*
 * A vertex of a cell, with an outgoing set of directed edges.
 *
 * pos    the three-dimensional position of the vertex
 * Cell   the cell that the vertex belongs to;
 *        nonnull
 * ID     the ID number assigned to the vertex by its cell (or the client);
 *        positive
 * data   generic data attached to the vertex by the client
 * Edges  the edges in the orbit of the vertex;
 *        all are nonnull
 */
class Vertex
{

  /* -- public class methods ----------------------------------------------- */

  public:

  /*
   * Return a new vertex at the origin with no outgoing edges.
   * cell -> the cell that the vertex belongs to;
   *         must be nonnull
   * <- the new vertex;
   *    will be nonnull
   */
  static Vertex *make(Cell *cell);

  /*
   * Release the storage occupied by a given vertex.
   * vertex -> the vertex to kill;
   *           must be nonnull
   */
  static void kill(Vertex *vertex);

  /* -- public instance variables ------------------------------------------ */

  /*
   * The three-dimensional position and normal of this vertex.
   */
  Vec3 pos, nor, tmp;

  /*
   * The data associated with this edge.
   */
  const void *data;

  int tag;

  /* -- public instance methods -------------------------------------------- */

  public:

  /*
   * Return the cell for this vertex.
   * <- the cell that the vertex belongs to;
   *    will be nonnull
   */
  Cell *getCell();

  /*
   * Return the ID of this vertex.
   * <- the ID of this vertex;
   *    will be positive
   */
  unsigned int getID();

  /*
   * Change the ID of this vertex.
   * id -> the new id for this vertex;
   *       must be positive
   */
  void setID(unsigned int id);

  /*
   * Return an arbitrary outgoing edge from this vertex.
   * <- an edge whose origin is this vertex;
   *    null if isolated
   */
  Edge *getEdge();

  /*
   * Add a given outgoing edge to this vertex.
   * edge -> an edge whose origin is this vertex;
   *         must be nonnull
   */
  void addEdge(Edge *edge);
    
  /*
   * Remove a given outgoing edge from this vertex.
   * edge -> an edge whose origin is no longer at this vertex;
   *         must be nonnull
   */
  void removeEdge(Edge *edge);
    
  /* -- protected instance methods ----------------------------------------- */

  protected:

  /*
   * Initialize this vertex at the origin with no outgoing edges.
   * cell -> the cell that this vertex belongs to;
   *         must be nonnull
   */
  Vertex(Cell *cell);

  /*
   * Release the storage occupied by the contents of this vertex.
   */
  ~Vertex();

  /* -- private instance variables ----------------------------------------- */

  private:

  /*
   * The cell that this vertex belongs to.
   * Nonnull.
   */
  Cell *cell;

  /*
   * The ID of this vertex.
   * Positive.
   */
  unsigned int id;

  /*
   * An arbitrary outgoing edge of this vertex.
   * Null if isolated.
   */
  Edge *edge;
  /* Terrible shame.  Wood. */
  friend Cell *objCloneCell(Cell *cell);
  friend Cell *objReadCell(std::istream &s, const char *streamname);
};

/* -- inline instance methods ---------------------------------------------- */

inline Cell *Vertex::getCell()
{
  return cell;
}

inline unsigned int Vertex::getID()
{
  return id;
}

inline Edge *Vertex::getEdge()
{
  return edge;
}

/* ----------------------------------------------------------------------------
 * VertexEdgeIterator
 * ------------------------------------------------------------------------- */

/*
 * Enumerates the outgoing edges of a given vertex in counterclockwise order.
 */
class VertexEdgeIterator
{

  /* -- public instance methods -------------------------------------------- */

  public:

  /*
   * Initialize this edge iterator over a given vertex.
   * vertex -> the vertex to iterate the edges of;
   *           must be nonnull
   */
  VertexEdgeIterator(Vertex *vertex)
  {
    // pick an arbitrary edge in the vertex orbit

    start = vertex->getEdge();
    edge  = start;
  }

  /*
   * Release the storage occupied by this edge iterator.
   */
  ~VertexEdgeIterator()
  {
  }

  /*
   * Return the next edge of this edge iterator, if any.
   * <- the next edge of this edge iterator;
   *    null if none
   */
  Edge *next()
  {
    // check for degeneracy or exhausted iteration

    Edge *current = edge;

    if (current==0)
	return 0;

    // get the next edge in the counterclockwise orbit of the vertex, but
    // return the current edge
    // reset to null if we've come back to the start

    Edge *next = current->Onext();

    edge = next!=start ? next : 0;

    return current;
  }

  /* -- private instance variables ----------------------------------------- */

  private:

  /*
   * The first edge to be iterated.
   * Nonnull.
   */
  Edge *start;

  /*
   * The next edge to be iterated.
   * Null if exhausted.
   */
  Edge *edge;

};

#endif /* #ifndef vertexINCLUDED */

