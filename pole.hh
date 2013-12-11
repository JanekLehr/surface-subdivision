// -*- C++ -*-
#ifndef POLEH
#define POLEH
//////////////////////////////////////////////////////////////////////
// 
// File: pole.hh
// 
// Animation Lab, Computer Science Department
// Carnegie Mellon University
// 
// Created:       Sat Mar 16 17:22:12 1996 by Zoran Popovic
// 
// GerbilPole is a class for mousepoles implemented over gl.
//
// The idea of a mousepole is to translate 2D mouse motion into
// movement of a 3D locator, ideally allowing full, usable positioning
// in all 3 dimensions from a single perspective view of the 3D workspace.
//
// The common thread among all mousepoles is the visual representation:
// Starting at the position of the actual 3D locator, we draw thick
// lines down to any or all of the 3 coordinate planes.  Thus if the
// system provides a visual representation of the planes (like a grid for
// the ground plane), we see one or more orthogonal "poles" emerging
// from the planes, converging at the locator, which is seen as the
// "pole tip".  When used in combination with drop shadows in a 3D
// environment, the mousepole allows precise (to the pixel level)
// positioning in all 3 dimensions from a single view.
//
// mousepole idea due to Andy Witkin?
// Copyright (C) 1994  Sebastian Grassia
// Modified November 1995, David Baraff.
// Modified March 1996, Zoran Popovic.
// Converted vector/matrix from libm4 to libsvl, Feb 1997, Paul Heckbert
// 
//////////////////////////////////////////////////////////////////////

#include "svl/Mat3.h"
#include "view.hh"

/* Planes of movement for mousepoles.  Also used to specify
 * the planes we should drop poles onto in 'SetDisplay' */

enum PolePlaneMask { NO_PLANE=0, XZ_PLANE=1, XY_PLANE=2, ZY_PLANE=4, 
		     FLOOR_VIEW_PLANE, UP_VIEW_PLANE, 
		     SCREEN_PLANE=8, LOOKAT_DIR=16 };

class GerbilPole {
public:
    GerbilPole(View *v);
    ~GerbilPole() {};

    int active() { return interaction; };
    void setDropPlanes(int pmask, double xz, double xy, double zy);
    void begin(double x, double y);
    void begin(double x, double y, Vec3 &start_pos);
    void update(double x, double y, Vec3 &new_pos);
    void end(double x, double y);
    void glRender(Vec3 *shadow_dir);
    void glSetMouse(int x, int y, int visible);

    Vec3  pos;			// current position of mousepole tip
    double  scale;		// scale effect of mouse movement

private:
    View *view;
    Mat4 orient_mat;		// camera orientation matrix

    int interaction;

    int drop_plane_mask;	// mask of planes to drop to when display
    Vec3 drop_planes;		// drop values for planes

    Mat3 axis;			// axes vectors projected on the screen
    int coords[3][2];           // coord[Z][X] is a coord[X] when
                                // moving a pole in the XY_PLANE

    Vec3 last, now;		// previous and current mouse coords
    Vec3 world_factor;		// maps screen mouse coords to world mouse plane
    PolePlaneMask plane_map[3];	// mapping of motion planes to mousebuttons

    Vec3 up_plane_normal;	// plane normal for the UP_VIEW_PLANE

    int intersectPlaneRay(Vec3 &porig, Vec3 &n, Vec3 &p, Vec3 &d,
	Vec3 &intersection);

    int findNewFloorDir(Vec3 &dir);
    int findNewUpDir(Vec3 &dir);
};

#endif /* POLEH */
