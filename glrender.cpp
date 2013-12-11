//////////////////////////////////////////////////////////////////////
// 
// File: glrender.cc
// 
// Animation Lab, Computer Science Department
// Carnegie Mellon University
// 
// Created:       Fri Mar 15 02:55:52 1996 by Zoran Popovic
// 
// various GL dependent rendering methods
//
// includes routines to draw cubes, spheres, grids,
// and also drawing code for both View and GerbilPole classes
// 
//////////////////////////////////////////////////////////////////////

// #include <X11/cursorfont.h>
// #include <X11/Xlib.h>

#include "StdAfx.h"
#include <GL/gl.h>
#include <GL/glu.h>

// #include <forms.h>
// #include "ui.h"

#include "pole.hh"
#include "glrender.hh"

// extern FD_main_form* fdf;

inline void
glVertex(Vec3 &v)
{
    glVertex3dv(v.Ref());
}

// Draw various GL primitives

void draw_cube(void)
{
   static GLuint display_tag = 0;
   if(display_tag == 0) {
      display_tag = glGenLists(1);
      glNewList(display_tag, GL_COMPILE_AND_EXECUTE);

      static GLdouble n[6][3] = {
	 {-1.0, 0.0, 0.0}, {0.0, 1.0, 0.0}, {1.0, 0.0, 0.0},
	 {0.0, -1.0, 0.0}, {0.0, 0.0, 1.0}, {0.0, 0.0, -1.0}
      };
      static GLint faces[6][4] = {
	 { 0, 1, 2, 3 }, { 3, 2, 6, 7 }, { 7, 6, 5, 4 },
	 { 4, 5, 1, 0 }, { 5, 6, 2, 1 }, { 7, 4, 0, 3 }
      };
      GLdouble v[8][3];
      GLint i;

      v[0][0] = v[1][0] = v[2][0] = v[3][0] = -1;
      v[4][0] = v[5][0] = v[6][0] = v[7][0] = 1;
      v[0][1] = v[1][1] = v[4][1] = v[5][1] = -1;
      v[2][1] = v[3][1] = v[6][1] = v[7][1] = 1;
      v[0][2] = v[3][2] = v[4][2] = v[7][2] = -1;
      v[1][2] = v[2][2] = v[5][2] = v[6][2] = 1;

      static GLenum type = GL_QUADS; /* GL_LINE_LOOP */

      for (i = 0; i < 6; i++) {
	 glBegin(type);
	 glNormal3dv(&n[i][0]);
	 glVertex3dv(&v[faces[i][0]][0]);
	 glNormal3dv(&n[i][0]);
	 glVertex3dv(&v[faces[i][1]][0]);
	 glNormal3dv(&n[i][0]);
	 glVertex3dv(&v[faces[i][2]][0]);
	 glNormal3dv(&n[i][0]);
	 glVertex3dv(&v[faces[i][3]][0]);
	 glEnd();
      }
      glEndList();
   }
   else {
      glCallList(display_tag);
   }
}

void draw_sphere(void)
{   
   static GLuint display_tag = 0;
   if(display_tag == 0) {
      display_tag = glGenLists(1);
      GLUquadricObj *o = gluNewQuadric();
      // casting to GLenum for quiet SUN compile
      gluQuadricOrientation(o, (GLenum) GLU_OUTSIDE);
      glNewList(display_tag, GL_COMPILE_AND_EXECUTE);
      gluSphere(o, 1, 20, 10);
      glEndList();
      gluDeleteQuadric(o);
   }
   else {
      glCallList(display_tag);
   }
}

static void draw_circle(double x, double y, double radius)
{   
   glPushMatrix();

   glTranslated(x, y, 0);
   glScaled(radius, radius, 1.);

   static GLuint display_tag = 0;
   if(display_tag == 0) {
      display_tag = glGenLists(1);
      glNewList(display_tag, GL_COMPILE_AND_EXECUTE);
      glBegin(GL_LINE_LOOP);
      const double segs = 60;
      for (int i = 0; i < segs; i++) {
	 glVertex2d((cos(i * 2 * M_PI / segs)), 
		    (sin(i * 2 * M_PI / segs)));
      }
      glEnd();
      glEndList();
   }
   else {
      glCallList(display_tag);
   }

   glPopMatrix();
}

static void
draw_grid(int xblocks, int zblocks, double xdim, double zdim)
{
   Vec3 v[2];
   int i;
    
   glPushAttrib(GL_LINE_BIT);
   glLineWidth(2);
   glBegin(GL_LINES);

   v[0] = Vec3(-xdim*0.5, 0, 0);
   v[1] = Vec3( xdim*0.5, 0, 0);
   for (i=0; i<=zblocks; i++){
      v[0][2] = v[1][2] = -zdim*0.5 + i*(zdim/zblocks);
      glVertex(v[0]);
      glVertex(v[1]);
   }
   v[0] = Vec3(0, 0,  zdim*0.5);
   v[1] = Vec3(0, 0, -zdim*0.5);
   for (i=0; i<=xblocks; i++){
      v[0][0] = v[1][0] = -xdim*0.5 + i*(xdim/xblocks);
      glVertex(v[0]);
      glVertex(v[1]);
   }

   glEnd();
   glPopAttrib();
}

static void
draw_translucent_rect(double x0, double y0, double x1, double y1)
{
   /* This stipple is a simple checkerboard of bits, which should
    * make the floor translucent */
   static GLubyte halftone[] = {
      0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55, 
      0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55, 
      0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55,
      0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55, 
      0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55, 
      0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55,
      0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55, 
      0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55, 
      0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55,
      0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55, 
      0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55, 
      0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55,
      0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55, 
      0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55, 
      0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55,
      0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55};
    
   glEnable (GL_POLYGON_STIPPLE);
   //?? glDisable (GL_CULL_FACE);
   glPolygonStipple (halftone);
   glBegin(GL_POLYGON);
   glVertex3d(x0, 0, y0); 
   glVertex3d(x1, 0, y0); 
   glVertex3d(x1, 0, y1); 
   glVertex3d(x0, 0, y1); 
   glEnd();
   glDisable (GL_POLYGON_STIPPLE);
   //?? glEnable (GL_CULL_FACE);
}

#if 0
void pointer_visibility(int arg)
{
   Window win = fl_get_canvas_id(fdf->gl_canvas);
   static int no_cursor = 0;
   char data = 0;
    
   if(! no_cursor) {
      no_cursor = fl_create_bitmap_cursor(&data, &data, 1, 1, 0, 0);
   }

   fl_set_cursor(win, (arg) ? XC_top_left_arrow : no_cursor);
}
#endif

// ============================ View ============================

void
View::perspective(int w, int h, double fov, double asp, double n, double f)
{
   width = w;
   height = h;
   radius = ((height < width) ? height : width) * .5 * .9;

   fovy = fov;
   aspect = asp;
   nnear = n;
   ffar = f;

   glViewport(0,0, (GLint)width, (GLint)height);
   glMatrixMode( GL_PROJECTION );
   glLoadIdentity();
   gluPerspective(fovy, ((double) width) / height, nnear, ffar);
   glGetDoublev(GL_PROJECTION_MATRIX, (double *)projection_mat.Ref());
   glGetIntegerv(GL_VIEWPORT, viewport);
}

void
View::resize( int w, int h )
{
   width = w;
   height = h;
   radius = ((height < width) ? height : width) * .5 * .9;

   aspect = double(width) / height;

   glViewport(0,0, (GLint)width, (GLint)height);
   glMatrixMode( GL_PROJECTION );
   glLoadIdentity();
   gluPerspective(fovy, ((double) width) / height, nnear, ffar);
   glGetDoublev(GL_PROJECTION_MATRIX, (double *)projection_mat.Ref());
   glGetIntegerv(GL_VIEWPORT, viewport);
}

void
View::glSetup()
{
   glMatrixMode( GL_PROJECTION );
   glLoadMatrixd((const GLdouble *) projection_mat.Ref());

   glMatrixMode( GL_MODELVIEW );
   glLoadIdentity();

   // light is up, behind us, a little to left
   // (x points right, y points up, z points toward viewer)
   GLfloat light0_position[] = { -1, 2, 5, 0 }; 
   glLightfv(GL_LIGHT0, GL_POSITION, light0_position);

   //GLfloat mat_specular[] = {1., 1., 1., 1.};
   GLfloat mat_specular[] = {.5, .5, .5, 1.};
   GLfloat mat_shininess[] = {50.};
   glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
   glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);
   glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);

   glLoadMatrixd((const GLdouble *) modelview_mat.Ref());
}

void
View::glPushWorldSpace()
{
   glPushMatrix();
   glLoadMatrixd((const GLdouble *) modelview_mat.Ref());
}

void
View::glRender()
{
   static GLuint floor_tag = 0;
   static GLuint crosshairs_tag = 0;

   // draw the floor
   if (!floor_tag) {
      floor_tag = glGenLists(1);
      glNewList(floor_tag, GL_COMPILE_AND_EXECUTE);
      glDisable (GL_LIGHTING);
//      glDisable (GL_DITHER);
      glColor3f(.25,.25,.25);
      draw_translucent_rect(-5, -5, 5, 5);
      glColor3f(.5,.5,.5);
      draw_grid(6, 6, 10, 10);
//      glEnable (GL_DITHER);
      glEnable (GL_LIGHTING);
      glEndList();
   }
   else {
      glCallList(floor_tag);
   }

   // we don't draw unles in the interaction mode or explicitly
   // ordered to do so.
   if (! active() && !must_render) {
      return;
   }

   // crosshairs
   glPushAttrib(GL_LINE_BIT | GL_LIGHTING_BIT);
   glDisable(GL_LIGHTING);

   glPushMatrix();
   glTranslated(center[0], center[1], center[2]);
   double size = .2;				  // proportion of the screen
   glScaled(zoom * .5 * size, zoom * .5 * size, zoom * .5 * size);
#if 0
   if (!crosshairs_tag) {
      crosshairs_tag = glGenLists(1);
      glNewList(crosshairs_tag, GL_COMPILE_AND_EXECUTE);
      glLineWidth(2);
      glBegin(GL_LINES);
      glColor3f(1,0,0); glVertex3f(-1,0,0); 
      glColor3f(1,.2f,.2f); glVertex3f(1,0,0);
      glColor3f(0,.4f,0); glVertex3f(0,-1,0); 
      glColor3f(0,1,0); glVertex3f(0,1,0);
      glColor3f(0,0,1); glVertex3f(0,0,-1); 
      glColor3f(.2f,.2f,1); glVertex3f(0,0,1);
      glEnd();
      glEndList();
   }
   else {
      glCallList(crosshairs_tag);
   }
#endif
   glPopMatrix();

   // trackball circle in 2D
   glColor3f(1,1,0);
   glMatrixMode( GL_PROJECTION );
   glPushMatrix();
   glLoadIdentity();
   glOrtho(-width*.5, width*.5, -height*.5, height*.5, -1, 1);

   glMatrixMode( GL_MODELVIEW );
   glPushMatrix();
   glLoadIdentity();

   glLineWidth(3);
   draw_circle(0,0, radius);

   glMatrixMode( GL_PROJECTION );
   glPopMatrix();

   glMatrixMode( GL_MODELVIEW );
   glPopMatrix();

   glPopAttrib();
}

int
View::glGetMouseWorldPos(double x, double y, Vec3 &pos)
{
   gluUnProject(x, y, 0, (double*)modelview_mat.Ref(), 
		(double*)projection_mat.Ref(),
		viewport, &pos[0], &pos[1], &pos[2]);
   return 1;
}

int
View::glGetPointScreenPos(const Vec3 &pos, double* x, double* y)
{
   double dummy;
   gluProject(pos[0], pos[1], pos[2], 
	      (double*)modelview_mat.Ref(), 
	      (double*)projection_mat.Ref(), viewport, 
	      x, y, &dummy);
   return 1;
}

int
View::glGetRay(double x, double y, Vec3 &pt, Vec3 &dir)
{
   // pt is the eye point
   Mat4 inv_modelview(adj(modelview_mat));

   // find origin in homogenous coords
   pt = proj(inv_modelview * Vec4(0,0,0,1));	// homogeneous division

   glGetMouseWorldPos(x, y, dir);
   dir = norm(dir-pt);

   return 1;
}

// ============================ Pole ============================

void
GerbilPole::glSetMouse(int x, int y, int visible)
{
//	TODO.  Learn enough Windows programming to duplicate this stuff??

//   Window win = fl_get_canvas_id(fdf->gl_canvas);
//   int origx, origy, w, h;
//   fl_get_wingeometry(win, &origx, &origy, &w, &h);
//   fl_set_mouse(origx + x, origy + (h - y));
//    XWarpPointer(fl_display, None, win, 0, 0, 0, 0, x, h - y);
//   if (visible >= 0) {
//      pointer_visibility(visible);
//   }
}

/* -----------------------------------------------------------------
 * 'DrawPole' uses current gl drawing color and transform stack. if
 * 'shadow_dir' is non-NULL, use it as shine-direction of an
 * infinite light source that will cause the mousepole to cast a
 * black shadow onto the xz plane.
 * -----------------------------------------------------------------*/
void 
GerbilPole::glRender(Vec3 *shadow_dir)
{
   if (! active())
      return;
    
   glPushAttrib(GL_LINE_BIT | GL_LIGHTING_BIT);

   glLineWidth(4);
   glDisable(GL_LIGHTING);
   glColor3d(1, 1, 0);

   Vec3 drop;
    
   if (drop_plane_mask & XZ_PLANE) {
      glBegin(GL_LINES);
      glVertex(pos);
      drop = Vec3(pos[0], drop_planes[1], pos[2]);
      glVertex(drop);
      glEnd();
   }
   if (drop_plane_mask & XY_PLANE) {
      glBegin(GL_LINES);
      glVertex(pos);
      drop = Vec3(pos[0], pos[1], drop_planes[2]);
      glVertex(drop);
      glEnd();
   }
   if (drop_plane_mask & ZY_PLANE) {
      glBegin(GL_LINES);
      glVertex(pos);
      drop = Vec3(drop_planes[0], pos[1], pos[2]);
      glVertex(drop);
      glEnd();
   }
    
   if (shadow_dir){
      Vec3 dir = norm(*shadow_dir);
      double t = -(pos[1]-drop_planes[1])/(double)dir[1];
      glColor3d(.0625, .0625, .0625);
      glBegin(GL_LINES);
      drop = Vec3(pos[0], drop_planes[1] + .05, pos[2]);
      glVertex(drop);
      drop[0] += t * dir[0];
      drop[2] += t * dir[2];
      glVertex(drop);
      glEnd();
   }
   glPopAttrib();
}
