//////////////////////////////////////////////////////////////////////
// 
// File: view.cc
// 
// Animation Lab, Computer Science Department
// Carnegie Mellon University
// 
// Created:       Sun Mar 10 02:57:18 1996 by Zoran Popovic
// CVS info:
//     $Source: /afs/cs.cmu.edu/user/andrewb/cvsroot/cs463/p2/cellview/view.cc,v $
// 
// 3-D viewing and interactive rotation/translation
// 
//////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include "view.hh"

View::View(ButtonStack *bstack)
{
    button_stack = bstack;

    mod_map[0] = VOrbit;
    mod_map[1] = VDolly;
    mod_map[2] = VZoom;

    interaction = 0;
    must_render = 0;
}

void
View::lookAt(double eyex, double eyey, double eyez, 
	     double centerx, double centery, double centerz, double twist)
{
    Vec3 eye(eyex, eyey, eyez);
    Vec3 negz(0., 0., -1.); 
    Vec3 dir;
    Quaternion q_dir, q_twist;

    center = Vec3(centerx, centery, centerz);
    dir = center-eye;
    zoom = len(dir);
    normalise(dir);

    q_dir = quatFromUV(negz, dir);
    q_twist = quatFromAxisRot(negz, twist * M_PI / 180);
    orient = conjugate(q_dir*q_twist);		// quaternion multiplication

    glComputeModelview();
}
 
void 
View::begin(double x, double y)
{
    interaction = 1;

    now = Vec3(x, y, 0);
    down = now;
    orig_center = center;
    orig_orient = orient;
    orig_zoom = zoom;
}
 
void 
View::update(double x, double y)
{
    if (button_stack->count() == 0) {
	fprintf(stderr, "View::update: button_stack is empty\n");
	exit(-1);
    }
    now = Vec3(x, y, 0);
    delta = now-down;
    int mod = mod_map[button_stack->currentButton()];

    switch (mod) {
      case VOrbit: 
      {
	  Vec3 from, to;
	  Quaternion q_delta;
	  ballPointFromMouse(down[0], down[1], from);
	  ballPointFromMouse(now[0], now[1], to);
	  qtFromBallPoints(from, to, q_delta); 
	  orient = q_delta*orig_orient;		// quaternion multiplication

	  break;
      }
      case VDolly:
      {
	  Mat4 mat;
	  mat = Mat4FromQuat(orient);
	  Vec3 u = delta[0] * norm(Vec3(mat[0][0], mat[0][1], mat[0][2]));
	  Vec3 v = delta[1] * norm(Vec3(mat[1][0], mat[1][1], mat[1][2]));
	  center = orig_center - zoom/width*(u+v);

	  break;
      }
      case VZoom:
      {
	  zoom = orig_zoom + .1 * delta[1];
	  if (zoom <=nnear) zoom = nnear;
	  if (zoom >= ffar) zoom = ffar;
	  printf("zoom %lf,  nnear %lf\n", zoom, nnear);

	  break;
      }
      default:
	  fprintf(stderr, "View::update: unknown modifier %d\n", 
		  mod_map[button_stack->currentButton()]);
	  exit(-1);
    };	/* switch */

    glComputeModelview();
}

void
View::glComputeModelview()
{
    Mat4 mat;
    mat.MakeHTrans(Vec3(0, 0, -zoom));
    
    Mat4 orient_mat = Mat4FromQuat(orient);
    mat *= orient_mat;
    
    Mat4 t;
    t.MakeHTrans(-center);
    mat *= t;
    modelview_mat = trans(mat);			  // GL uses tranposed mats
}

void 
View::end(double x, double y)
{
    update(x, y);

    down = now;
    orig_center = center;
    orig_orient = orient;
    orig_zoom = zoom;

    if (button_stack->count() == 1)
	interaction = 0;
}

/* Convert window coordinates to sphere coordinates. */
void
View::ballPointFromMouse(double x, double y, Vec3& v)
{
    v = Vec3(x - width*0.5, y - height*0.5, 0) / radius;

    register double mag = dot(v,v);
    if (mag > 1.0) {
        register double scale = 1.0/sqrt(mag);
	v *= scale;
    } else
	v[2] = sqrt(1 - mag);
}

/* Construct a unit quaternion from two points on unit sphere */
void 
View::qtFromBallPoints(const Vec3& from, const Vec3& to, Quaternion &q)
{
    q[0] = from[1]*to[2] - from[2]*to[1];
    q[1] = from[2]*to[0] - from[0]*to[2];
    q[2] = from[0]*to[1] - from[1]*to[0];

    q[3] = from[0]*to[0] + from[1]*to[1] + from[2]*to[2];
}

// ======================== ButtonStack ========================

ButtonStack::ButtonStack()
{
    reset();
}

void
ButtonStack::reset()
{
    button_stack[0]=BNone;
    button_stack[1]=BNone;
    button_stack[2]=BNone;

    mod_stack[0]=MNone;
    mod_stack[1]=MNone;
    mod_stack[2]=MNone;

    num = 0;
}

void
ButtonStack::push(int butt, int mod)
{
    if (button_stack[2] != BNone) {
	fprintf(stderr, "ButtonStack::push: button stack is full.\n");
	return;
    }
    if (button_stack[0] == butt || button_stack[1] == butt) {
	fprintf(stderr, "ButtonStack::push: button %d already active.\n", butt);
	return;
    }
    button_stack[2] = button_stack[1]; mod_stack[2] = mod_stack[1];
    button_stack[1] = button_stack[0]; mod_stack[1] = mod_stack[0];
    button_stack[0] = butt; mod_stack[0] = mod;
    num++;
}

void
ButtonStack::pop(int butt)
{
    // pop the button off the stack
    if (button_stack[0] == butt) {
	button_stack[0] = button_stack[1]; mod_stack[0] = mod_stack[1];
	button_stack[1] = button_stack[2]; mod_stack[1] = mod_stack[2];
	button_stack[2] = BNone; mod_stack[2] = MNone;
    }
    else if (button_stack[1] == butt) {
	button_stack[1] = button_stack[2]; mod_stack[1] = mod_stack[2];
	button_stack[2] = BNone; mod_stack[2] = MNone;
    }
    else if (button_stack[2] == butt) {
	button_stack[2] = BNone; mod_stack[2] = MNone;
    }
    else {
	fprintf(stderr, "ButtonStack::pop: button not active %d\n", butt);
	return;
    }
    num--;
}
