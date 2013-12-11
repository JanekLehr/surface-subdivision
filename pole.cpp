//////////////////////////////////////////////////////////////////////
// 
// File: pole.cc
// 
// Animation Lab, Computer Science Department
// Carnegie Mellon University
// 
// Created:       Sat Mar 16 17:25:19 1996 by Zoran Popovic
// 
// An improved and unbroken mousepole. to wit:
// 
// Pole movement is still performed in one of the 3 coordinate
// planes.  Now, however,  the direction of pole motion is the
// mouse motion itself _mapped along the corresponding cartesian axes_, 
// scaled according to distance from pole to camera. The "corresponding"
// axis is determined by the current camera orientation; the mapping
// is designed to pick the "most natural" pair of axes for the given
// viewing angle.  Some viewing angles work better than others, the most
// pessimal being a view direction of <1, 1, 1> or any reflection
// thereof.  For most likely viewing angles it is fairly intuitive
// to work with, I find.
//
// This mapping of mouse to pole motion means cursor
// position and pole tip will often not coincide on the screen (and
// be a bothersome visual artifact), we simply turn the cursor off
// for the duration of the pole's activation.  When the pole is
// deactivated, we snap the cursor to the pole tip defore turning
// it back on.
//
// Copyright (C) 1995  Sebastian Grassia
// Modified November 1995, David Baraff.
// Finally done right March 1996, Zoran Popovic.
// Converted vector/matrix from libm4 to libsvl, Feb 1998, Paul Heckbert
// 
//////////////////////////////////////////////////////////////////////

// The glSetMouse stuff doesn't seem to work under Windows, and the
// making the cursor invisible stuff definately doesn't work, so for the
// time being, let's just turn off the warping code.

#define DOWARPING 0

#include <stdio.h>
#include <stdlib.h>

#include "svl/SVL.h"
#include "pole.hh"

const int X = 0,
	  Y = 1,
	  Z = 2;

inline double
sgn(double x)
{
    return x > 0 ? 1 : (x < 0 ? -1 : 0);
}

int biggestCoord(Vec3 &uaxis, Vec3 &vaxis)
{
    int retval = X;
    double max_size = -1;
    
    for (int i = X; i <= Y; i++) {
	if (fabs(uaxis[i]) > max_size) {
	    max_size = fabs(uaxis[i]);
	    retval = i;
	}
	if (fabs(vaxis[i]) > max_size) {
	    max_size = fabs(vaxis[i]);
	    retval = i;
	}
    }
    return retval;
}

int
GerbilPole::findNewFloorDir(Vec3 &dir)
{
    Vec3 mouse, orig;
    double t;

    Vec3 delta = now-last;
    double scale = len(delta);
    view->glGetPointScreenPos(pos, &mouse[X], &mouse[Y]);
    mouse[Z] = 0;
    mouse += delta;
    view->glGetRay(mouse[X], mouse[Y], orig, dir);
    if ((t = (pos[Y] - orig[Y]) / dir[Y]) <= 0) {
	mouse -= 2.*delta;
	view->glGetRay(mouse[X], mouse[Y], orig, dir);
	if ((t = (pos[Y] - orig[Y]) / dir[Y]) <= 0) {
	    printf("quitting\n");
	    return 0;
	}
	scale *= -1;
    }
    dir = scale*norm(orig+t*dir-pos);
    // cout << "findnewfloordir: dir=" << dir << endl;
    return 1;
}

int
GerbilPole::intersectPlaneRay(Vec3 &porig, Vec3 &n, Vec3 &p, Vec3 &d,
			      Vec3 &intersection)
{
    double step = fabs(dot(d, n));
    if (step < 1.e-5)
	return 0;
    double dist = fabs(dot(p-porig, n));
    intersection = p+dist/step*d;
    return 1;
}

int
GerbilPole::findNewUpDir(Vec3 &dir)
{
    Vec3 mouse, orig, new_pos;

    Vec3 delta = now-last;
    double scale = len(delta);
    view->glGetPointScreenPos(pos, &mouse[X], &mouse[Y]); mouse[Z] = 0;
    mouse += delta;
    view->glGetRay(mouse[X], mouse[Y], orig, dir);
    intersectPlaneRay(pos, up_plane_normal, orig, dir, new_pos);
    
    if (intersectPlaneRay(pos, up_plane_normal, orig, dir, new_pos) == 0) {
	mouse -= 2.*delta;	// try in the opposite direction
	view->glGetRay(mouse[X], mouse[Y], orig, dir);
	if (intersectPlaneRay(pos, up_plane_normal, orig, dir, new_pos) == 0) {
	    printf("quitting\n");
	    return 0;
	}
	scale *= -1;
    }
    dir = scale*norm(new_pos-pos);
    // cout << "findnewupdir: dir=" << dir << endl;
    return 1;
}

GerbilPole::GerbilPole(View *v)
{
    view = v;

    scale = .015;
    interaction = 0;

    plane_map[0] = XZ_PLANE;
    plane_map[1] = XY_PLANE;
    plane_map[2] = ZY_PLANE;
    setDropPlanes(XZ_PLANE, -5, 0, -5);
    // setPlanes(XZ_PLANE | XY_PLANE | ZY_PLANE, -5, 0, -5);

    pos.MakeZero();
    // pos = Vec3(10., 10., 10.);
}

/* -----------------------------------------------------------------
 * 'setPlanes' 'pmask' specifies which planes to drop lines to
 * when drawing mousepole. It should be a bitwise OR of PolePlane
 * values. 'xz' is the y coordinate of the XZ coordinate plane (that
 * we drop to), likewise for 'xy' and 'zy'
 * -----------------------------------------------------------------*/
void GerbilPole::setDropPlanes(int pmask, double xz, double xy, double zy)
{
    drop_plane_mask = pmask;
    drop_planes = Vec3(xz, xy, zy);
}

namespace SubMath {
	inline int rint(double d)
	{
		int res = (int)d;
		if (d - res > 0.5)
			++res;
		else if (d - res < -0.5)
			--res;
		return res;
	}
}

/* ARGSUSED */
void GerbilPole::begin(double x, double y)
{
    int mod = view->button_stack->currentMod();
    if (mod & MControl) {
	plane_map[0] = SCREEN_PLANE;
	plane_map[1] = LOOKAT_DIR;
	plane_map[2] = LOOKAT_DIR;
    }
    else {
	plane_map[0] = FLOOR_VIEW_PLANE;	// was XZ
	plane_map[1] = UP_VIEW_PLANE;		// was XY
	plane_map[2] = ZY_PLANE;
    }

    interaction = 1;


    int cx = (int) rint(view->width * .5);
    int cy = (int) rint(view->height * .5);

#if DOWARPING
    // give the mouse more room to move: Since X will still try to
    // respect window boundaries, making the cursor visible (and
    // bound by the physical screen) when its coordinate leaves
    // our window, we'll always yank it back to the middle of our
    // window at the beginning (and after an incremental motion.)
    last = Vec3(cx, cy, 0);
    glSetMouse((int)last[X], (int) last[Y], 0);
#else
	last = Vec3( x,y,0 );
#endif

    // figure out the scaling factors that would map the mouse
    // coords into the relative coords of the screen plane in
    // world space.
    Vec3 orig, center;
    view->glGetMouseWorldPos(0,0, orig);
    view->glGetMouseWorldPos(cx, cy, center);
    double world_diag = len(center-orig);
    double screen_diag = sqrt(cx*cx + cy*cy);
    world_factor = world_diag/screen_diag*Vec3(cx, cy, 0);

    // setup the orientation matrix which is used for SCREEN_PLANE mode
    orient_mat = Mat4FromQuat(view->orient);

    // compute X and Y screen coord indices for each of the plane modifies
    int comp_coord;

    // compute 2D screen vectors for each of the world axis
    view->glGetPointScreenPos(Vec3(1,0,0), &axis[X][X], &axis[X][Y]);
    axis[X][Z] = 0;
    view->glGetPointScreenPos(Vec3(0,1,0), &axis[Y][X], &axis[Y][Y]);
    axis[Y][Z] = 0;
    view->glGetPointScreenPos(Vec3(0,0,1), &axis[Z][X], &axis[Z][Y]);
    axis[Z][Z] = 0;
    Vec3 v;
    view->glGetPointScreenPos(Vec3(0,0,0), &v[X], &v[Y]);
    v[Z] = 0;
    axis[X] -= v;
    axis[Y] -= v;
    axis[Z] -= v;
    
    // XZ_PLANE coord indices == coords[Y]
    comp_coord = biggestCoord(axis[X], axis[Z]);
    coords[Y][X] = (fabs(axis[X][comp_coord]) > fabs(axis[Z][comp_coord])) ?
	comp_coord : 1-comp_coord;
    coords[Y][Y] = 1-coords[Y][X];

    // XY_PLANE coord indices == coords[Z]
    comp_coord = biggestCoord(axis[X], axis[Y]);
    coords[Z][X] = (fabs(axis[X][comp_coord]) > fabs(axis[Y][comp_coord])) ?
	comp_coord : 1-comp_coord;
    coords[Z][Y] = 1-coords[Z][X];

    // ZY_PLANE coord indices == coords[X]
    comp_coord = biggestCoord(axis[Z], axis[Y]);
    coords[X][X] = (fabs(axis[Z][comp_coord]) > fabs(axis[Y][comp_coord])) ?
	comp_coord : 1-comp_coord;
    coords[X][Y] = 1-coords[X][X];

    // UP_VIEW_PLANE
    Vec3 xdir;
    view->glGetMouseWorldPos(cx, cy, center);
    view->glGetMouseWorldPos(cx+10, cy, xdir);
    up_plane_normal = cross(norm(xdir-center), Vec3(0,1,0));
}

void GerbilPole::begin(double x, double y, Vec3 &start_pos)
{
    pos = start_pos;
    begin(x, y);
}

void GerbilPole::update(double x, double y, Vec3 &position)
{
    if (view->button_stack->count() == 0) {
	fprintf(stderr, "GerbilPole::update:: button stack empty\n");
	exit(-1);
    }

    // compute the screen delta in world space and scale it
    Vec3 delta;
    now = Vec3(x, y, 0);
    delta = now-last;

    // repositioning the pointer generates an extra event so we
    // must check for it
    if (delta[0] == 0 && delta[1] == 0) {
	position = pos;
	return;
    }

    delta[X] *= world_factor[X] * scale;
    delta[Y] *= world_factor[Y] * scale;

    int c0,c1;					  // coord indices

    switch (plane_map[view->button_stack->currentButton()]){
      case NO_PLANE:
	  break;
		    
      case XZ_PLANE:
	  c0 = coords[Y][X];
	  c1 = coords[Y][Y];
	  pos[X] += delta[c0] * sgn(axis[X][c0]);
	  pos[Z] += delta[c1] * sgn(axis[Z][c1]);
	  break;
	
      case XY_PLANE:
	  c0 = coords[Z][X];
	  c1 = coords[Z][Y];
	  pos[X] += delta[c0] * sgn(axis[X][c0]);
	  pos[Y] += delta[c1] * sgn(axis[Y][c1]);
	  break;
	
      case ZY_PLANE:
	  c0 = coords[X][X];
	  c1 = coords[X][Y];
	  pos[Z] += delta[c0] * sgn(axis[Z][c0]);
	  pos[Y] += delta[c1] * sgn(axis[Y][c1]);
	  break;

      case SCREEN_PLANE:
	  pos[0] += delta[X]*orient_mat[0][0] + delta[Y]*orient_mat[1][0];
	  pos[1] += delta[X]*orient_mat[0][1] + delta[Y]*orient_mat[1][1];
	  pos[2] += delta[X]*orient_mat[0][2] + delta[Y]*orient_mat[1][2];
	  break;

      case LOOKAT_DIR: 
      {
	  Vec3 orig, dir;
	  double mx, my;
	  view->glGetPointScreenPos(pos, &mx, &my); 
	  view->glGetRay(mx, my, orig, dir);
	  pos += delta[Y]*dir;
	  break;
      }
      case FLOOR_VIEW_PLANE:
      {
	  Vec3 dir;
	  if (!findNewFloorDir(dir)) {
	      printf("skipping\n");
	      break;
	  };
	  pos += scale*dir;
	  break;
      }
      case UP_VIEW_PLANE:
      {
	  Vec3 dir;
	  if (!findNewUpDir(dir)) {
	      printf("skipping\n");
	      break;
	  };
	  pos += scale*dir;
	  break;
      }
      default:
	  fprintf(stderr, "GerbilPole::update: Unknown constraint plane %d\n", 
		  plane_map[view->button_stack->currentButton()]);
	  exit(-1);
    }

    // yank mouse back to center of window if near the boundary so
    // that the mouse stays invisible
    last = now;

#if DOWARP
    const double slack = 10;
    if (last[X] < slack || last[X] > view->width-slack ||
	last[Y] < slack || last[Y] > view->height-slack) {
	last = .5*Vec3(view->width, view->height, 0);
	glSetMouse((int)last[X], (int) last[Y], -1);
	printf("repos\n");
    }
#endif

    position = pos;
}

/* -----------------------------------------------------------------
 * 'end'  make the cursor visible again, after yanking it
 * so that it falls right on top of the mousepole tip
 * -----------------------------------------------------------------*/
void GerbilPole::end(double x, double y)
{
    // set to a final position
    Vec3 dummy;
    update(x, y, dummy);

    if (view->button_stack->count() == 1) {
	view->glGetPointScreenPos(pos, &x, &y);
	interaction = 0;
	glSetMouse((int) x, (int) y, 1);
    }
}
