/*********************************************************************
 * 
 * File: qt.cc
 * 
 * Animation Lab, Computer Science Department
 * Carnegie Mellon University
 * 
 * Created:       Sun Mar 10 02:03:55 1996 by Zoran Popovic
 * 
 * Basic quaternion operations: used as a representation for rotations
 *
 * modified Feb 1998 to be C++; to use SVL, not M4; Paul Heckbert
 * 
 *********************************************************************/

#include <math.h>
#include "float.h"
#include "qt.hh"

Quaternion qt_one(0, 0, 0, 1);

/* Return quaternion product out = qL * qR.  Note: order is important!
 * To combine rotations, use the product Mul(qSecond, qFirst),
 * which gives the effect of rotating by qFirst then qSecond. */
Quaternion Quaternion::operator*(const Quaternion &qR)
{
    // this*qR
    Quaternion out;
    out[3] = (*this)[3]*qR[3] - (*this)[0]*qR[0]
	   - (*this)[1]*qR[1] - (*this)[2]*qR[2];
    out[0] = (*this)[3]*qR[0] + (*this)[0]*qR[3]
	   + (*this)[1]*qR[2] - (*this)[2]*qR[1];
    out[1] = (*this)[3]*qR[1] + (*this)[1]*qR[3]
	   + (*this)[2]*qR[0] - (*this)[0]*qR[2];
    out[2] = (*this)[3]*qR[2] + (*this)[2]*qR[3]
	   + (*this)[0]*qR[1] - (*this)[1]*qR[0];
    return out;
}

/* Construct rotation matrix from (possibly non-unit) quaternion.
 * Assumes matrix is used to multiply column vector on the left:
 * vnew = mat vold.  Works correctly for right-handed coordinate system
 * and right-handed rotations. */
Mat4 Mat4FromQuat(Quaternion &q)
{
    Mat4 out;
    double Nq = q[0]*q[0] + q[1]*q[1] + q[2]*q[2] + q[3]*q[3];
    double s = (Nq > 0.0) ? (2.0 / Nq) : 0.0;
    double xs = q[0]*s,        ys = q[1]*s,         zs = q[2]*s;
    double wx = q[3]*xs,       wy = q[3]*ys,        wz = q[3]*zs;
    double xx = q[0]*xs,       xy = q[0]*ys,        xz = q[0]*zs;
    double yy = q[1]*ys,       yz = q[1]*zs,        zz = q[2]*zs;
    out[0][0] = 1.0 - (yy + zz);
    out[1][0] = xy + wz;
    out[2][0] = xz - wy;
    out[0][1] = xy - wz;
    out[1][1] = 1.0 - (xx + zz);
    out[2][1] = yz + wx;
    out[0][2] = xz + wy;
    out[1][2] = yz - wx;
    out[2][2] = 1.0 - (xx + yy);
    out[0][3] = out[1][3] = out[2][3] = out[3][0] = out[3][1] = out[3][2] = 0.0;
    out[3][3] = 1.0;
    return out;
}

/* Construct a unit quaternion from a 3x3 rotation matrix */
Quaternion::Quaternion(Mat4 &R)
{
  double tr, s ;
  tr = R[0][0] + R[1][1] + R[2][2];
  if (tr >= 0) {
    s = sqrt(tr + 1) ;
    (*this)[3] = s*.5 ;
    s = .5/s ;
    (*this)[0] = (R[2][1] - R[1][2]) * s ;
    (*this)[1] = (R[0][2] - R[2][0]) * s ;
    (*this)[2] = (R[1][0] - R[0][1]) * s ;
  } else {
    int h = 0 ;
    if (R[1][1] > R[0][0]) h =1 ;
    if (R[2][2] > R[h][h]) h =2 ;
    switch(h) {
#define caseMacro(i,j,k) \
    case i: \
      s = sqrt((R[i][i] - (R[j][j]+R[k][k])) + 1) ; \
      (*this)[i] = s*.5 ;\
      s = .5/s ;\
      (*this)[j] = (R[i][j] + R[j][i])*s ;\
      (*this)[k] = (R[i][k] + R[k][i])*s ;\
      (*this)[3] = (R[k][j] - R[j][k])*s ;\
      break 
    caseMacro(0,1,2) ;
    caseMacro(1,2,0) ;
    caseMacro(2,0,1) ;
#undef caseMacro
    }
  }
}

/* Convert matrix to Euler angles (in radians). */
void
EulerAnglesFromMatrix(const Mat4 &M,
  double &x, double &y, double &z)
{
    int i= 0, j= 1, k= 2;

    double cy = sqrt(M[i][i]*M[i][i] + M[j][i]*M[j][i]);
    if (cy > 16*DBL_EPSILON) {
       x = atan2(M[k][j], M[k][k]);
       y = atan2(-M[k][i], cy);
       z = atan2(M[j][i], M[i][i]);
    } else {
       x = atan2(-M[j][k], M[j][j]);
       y = atan2(-M[k][i], cy);
       z = 0;
    }
}

/* compute a quaternion which takes the vector u onto the vector v.
   if unit==TRUE, u and v are already unit vectors. */
#define EpsEq(a,b) (fabs((a)-(b)) < (DBL_EPSILON))

Quaternion
quatFromUV(const Vec3 &u, const Vec3 &v)
{
    Vec3 mid = u+v;
    if (EpsEq(len(mid), 0)) {	   /* check for 180 degree rotation */  
	static Vec3 axis(1, 0, 0);
	/* make sure that arbitrary axis doesn not coincide with from/to */
	if (EpsEq(fabs(u[0]), 1.) || EpsEq(fabs(v[0]), 1.)) {
	    axis[0] = 0.0;
	    axis[1] = 1.0;
	}
	return Quaternion(cross(u, axis), 0.);
    }
    else {
	normalise(mid);
	return Quaternion(cross(u, mid), dot(u, mid));
    }
}
#undef EpsEq

/* compute a unit quaternion which takes the axis angle rotation */
Quaternion
quatFromAxisRot(const Vec3 &axis, double angle)
{
    return Quaternion(sin(.5*angle)*axis, cos(.5*angle));
}

/* Return conjugate of quaternion. */
Quaternion
conjugate(const Quaternion &q)
{
    return Quaternion(-q[0], -q[1], -q[2], q[3]);
}
