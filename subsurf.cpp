// Helper functions for SubView class
// 
// Zoran Popovic, Mar 1996	first incarnation as GerbilPole test prog.
// Paul Heckbert, Feb 1998	modified for 15-463 assignment P2 "surfed"
// Andrew Bernard, Feb 1999	modified for 15-463 assignment P2 "cellview"
// Paul Heckbert, Mar 1999	modified for 15-463 assignment P3 "subsurf"


// ---------------------------- picking
#include "StdAfx.h"
#include "gl/gl.h"
#include "gl/glu.h"
#include "cell/cell.hh"
#include "view.hh"
#include "glrender.hh"

static const double pick_dist = 30;

Vertex *
control_point_pick( Cell *cell, View *view, int mx, int my )
// Should be member of view, maybe.  (wood)
{
    // Pick the control point closest, in screen projection,
    // to mouse position (mx,my).
    // Returns the closest vertex in cell or NULL if none is close.

    Vertex *best = NULL;
    double dist2, min = HUGE, sx, sy, dx, dy;
    CellVertexIterator vertices( cell );
    Vertex *v;
    while ((v = vertices.next()) != 0)
    {
	// project (center of) each control point to screen space
	view->glGetPointScreenPos(v->pos, &sx, &sy);
	dx = sx-mx;
	dy = sy-my;
	dist2 = dx*dx+dy*dy;	// distance squared from (mx,my) to screen
				// projection of control point number m
	if (dist2<min) {
	    min = dist2;
	    best = v;
	}
    }
    if( sqrt(min) < pick_dist ) {
		std::cout << "picked control point " << best->getID() << std::endl;
		return best;
    }
    return NULL;

    // note: picking could also be done using ray tracing,
    // in which case you'd want to call view->glGetRay(mx, my, orig, dir);
}

// ---------------------------- redisplay scene

void
sphere(double r, double g, double b, Vec3& cen, double radius)
{
    glPushMatrix();
    glTranslated(cen[0], cen[1], cen[2]);
    glScalef((float)radius, (float)radius, (float)radius);
    glColor3f((float)r, (float)g, (float)b);
    draw_sphere();
    glPopMatrix();
}

void drawFaceFilled(Face *face)
{
    FaceEdgeIterator edges(face);
    Edge *edge;

    glBegin(GL_POLYGON);
    while ((edge = edges.next())!=0) {
	glNormal3dv(&edge->Org()->nor[0]);	// vertex normal
	glVertex3dv(&edge->Org()->pos[0]);	// position
    }
    glEnd();
}

void drawFilled( Cell *cell ) {
    CellFaceIterator faces( cell );
    Face *face;
    glColor3f(.8f, .6f, .3f);		// make surface orange
    while ((face = faces.next())!=0)
	drawFaceFilled(face);
}

void drawFaceWireframe(Face *face)
{
    FaceEdgeIterator edges(face);
    Edge *edge;

    // glBegin was done earlier
    while ((edge = edges.next())!=0)
	if (edge->Org() < edge->Dest()) {	// so we don't draw edge twice
	    // we send normals, and leave lighting on, so edges will be shaded
	    glNormal3dv(&edge->Org()->nor[0]);
	    glVertex3dv(&edge->Org()->pos[0]);	// one endpoint
	    glNormal3dv(&edge->Dest()->nor[0]);
	    glVertex3dv(&edge->Dest()->pos[0]);	// other endpoint
	}
}

void drawWireframe(Cell *cell) {
    CellFaceIterator faces( cell );
    Face *face;
    glColor3f(.8f, .8f, .8f);		// make surface white, for now
    glLineWidth(2);
    glBegin(GL_LINES);			// we'll draw line segments
    while ((face = faces.next())!=0)
	drawFaceWireframe(face);
    glEnd();
}
