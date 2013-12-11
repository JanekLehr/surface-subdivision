/*
	File:		VLgl.h
	
	Purpose:	Provides some handy wrappers for using svl with
				OpenGL.
 */

#ifndef __VLgl__
#define __VLgl__


#ifdef VL_FLOAT

inline void glVertex(const Vec2 &a) 
{ glVertex2fv(a.Ref()); }

inline void glVertex(const Vec3 &a) 
{ glVertex3fv(a.Ref()); }

inline void glVertex(const Vec4 &a) 
{ glVertex4fv(a.Ref()); }

inline void glColor(const Vec2 &a) 
{ glColor2fv(a.Ref()); }

inline void glColor(const Vec3 &a) 
{ glColor3fv(a.Ref()); }

inline void glColor(const Vec4 &a) 
{ glColor4fv(a.Ref()); }

inline void glNormal(const Vec2 &a) 
{ glNormal2fv(a.Ref()); }

inline void glNormal(const Vec3 &a) 
{ glNormal3fv(a.Ref()); }

inline void glNormal(const Vec4 &a) 
{ glNormal4fv(a.Ref()); }

#else

inline void glVertex(const Vec2 &a) 
{ glVertex2dv(a.Ref()); }

inline void glVertex(const Vec3 &a) 
{ glVertex3dv(a.Ref()); }

inline void glVertex(const Vec4 &a) 
{ glVertex4dv(a.Ref()); }

inline void glColor(const Vec2 &a) 
{ glColor2dv(a.Ref()); }

inline void glColor(const Vec3 &a) 
{ glColor3dv(a.Ref()); }

inline void glColor(const Vec4 &a) 
{ glColor4dv(a.Ref()); }

inline void glNormal(const Vec2 &a) 
{ glNormal2dv(a.Ref()); }

inline void glNormal(const Vec3 &a) 
{ glNormal3dv(a.Ref()); }

inline void glNormal(const Vec4 &a) 
{ glNormal4dv(a.Ref()); }


#endif

#endif
