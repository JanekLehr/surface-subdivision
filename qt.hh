#ifndef QTH
#define QTH

// ======================== Quaternions ========================
// quaternions are used as a representation for rotations

#include "svl/Mat4.h"

class Quaternion : public Vec4		// a Quaternion is a Vec4 plus ...
{
  public:
    // constructors
    Quaternion()
	{}
    Quaternion(double x, double y, double z, double w)
	: Vec4(x, y, z, w) {}
    Quaternion(const Vec3 &xyz, double w)
	: Vec4(xyz, w) {}
    Quaternion(Mat4 &R);		// convert matrix to quaternion

    // operators
    Quaternion operator*(const Quaternion &qR); // this*qR
    /* Return quaternion product out = qL * qR.  Note: order is important!
     * To combine rotations, use the product Mul(qSecond, qFirst),
     * which gives the effect of rotating by qFirst then qSecond. */
};

Mat4 Mat4FromQuat(Quaternion &q);	// convert quaternion to matrix

Quaternion quatFromUV(const Vec3 &u, const Vec3 &v);	// q = (u,v)

Quaternion quatFromAxisRot(const Vec3 &axis, double angle);

Quaternion conjugate(const Quaternion &q);

extern Quaternion qtOne;		// unit quaternion

extern void EulerAnglesFromMatrix(const Mat4 &M,
  double &x, double &y, double &z);
    /* rot(x,y,z) <- R */

#endif
