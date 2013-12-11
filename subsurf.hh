// subsurf - main program for Subdivision Surface editor
// 
// Zoran Popovic, Mar 1996	first incarnation as GerbilPole test prog.
// Paul Heckbert, Feb 1998	modified for 15-463 assignment P2 "surfed"
// Andrew Bernard, Feb 1999	modified for 15-463 assignment P2 "cellview"
// Paul Heckbert, Mar 1999	modified for 15-463 assignment P3 "subsurf"

// sections that you'll need to modify are marked by "//HINT:"

#ifndef SUBSURF_H
#define SUBSURF_H

class Vertex;
class Face;
class Cell;

Vertex *control_point_pick( Cell *, View *, int mx, int my);
void sphere(double r, double g, double b, Vec3& cen, double radius);
void drawFaceFilled(Face *face);
void drawFilled(Cell *cell);
void drawFaceWireframe(Face *face);
void drawWireframe(Cell *cell);

#endif