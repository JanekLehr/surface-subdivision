// -*- C++ -*-
#ifndef VIEWH
#define VIEWH
//////////////////////////////////////////////////////////////////////
// 
// File: view.hh
// 
// Animation Lab, Computer Science Department
// Carnegie Mellon University
// 
// Created:       Sun Mar 10 02:44:54 1996 by Zoran Popovic
// 
// View used for rotation control
// 
//////////////////////////////////////////////////////////////////////

#include "svl/Vec3.h"
#include "qt.hh"

// {{{ ButtonStack

// ======================== ButtonStack ========================

enum ButtonMod { MNone = 0, MShift=1, MControl=2, MMeta=4 };

enum Button { BLeft = 0, BMiddle = 1, BRight = 2, BNone = 3 };

class ButtonStack
{
  public:
    ButtonStack();
    ~ButtonStack() {};
    
    void reset();
    void push(int butt, int mod);
    void pop(int butt);
    int count() { return num; };
    int currentButton() { return button_stack[0]; };
    int currentMod() { return mod_stack[0]; };
    
  protected:
    int num;
    int button_stack[3];
    int mod_stack[3];
  private:
};

// }}}

enum ViewMod { VOrbit = 0, VZoom, VDolly, VNone };

class View
{
 public:
   View(ButtonStack *butt_stack);
   ~View() {};

   int active() { return interaction; };
   void perspective(int w, int h,
		    double fovy, double aspect, double nnear, double ffar);
   void resize( int w, int h );
   void lookAt(double eyex, double eyey, double eyez, 
	       double centerx, double centery, double centerz, double twist);

   void begin(double x, double y);
   void update(double x, double y);
   void end(double x, double y);

   void glSetup();
   void glPushWorldSpace();
   void glRender();

   int glGetRay(double x, double y, Vec3 &origin, Vec3 &dir);
   int glGetMouseWorldPos(double x, double y, Vec3 &pos);
   int glGetPointScreenPos(const Vec3 &pos, double* x, double* y);

   int must_render;

 protected:
   friend class GerbilPole;

   int width, height;
   double fovy, aspect;
   double nnear, ffar;

   Mat4 projection_mat;				  // *_mat are transposed for GL
   Mat4 modelview_mat;				  
   int viewport[4];

   Vec3 center;					  // center of attention
   Quaternion orient;				  // orientation
   double zoom;

   int interaction;				  // user interaction flag
   ButtonStack *button_stack;			  // button press ordering
   ViewMod mod_map[3];				  // buttons to ViewMods
 private:
   Vec3 down;
   Vec3 now;
   Vec3 delta;

   void glComputeModelview();
   void ballPointFromMouse(double x, double y, Vec3& v);
   void qtFromBallPoints(const Vec3& from, const Vec3& to, Quaternion &quat);

   double radius;				  // trackball radius

   Vec3 orig_center;
   Quaternion orig_orient;
   double orig_zoom;
};

#endif /* VIEWH */
