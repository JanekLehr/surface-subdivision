/*
	File:			Mat.cc

	Function:		Implements Mat.h

	Author(s):		Andrew Willmott

	Copyright:		Copyright (c) 1995-1996, Andrew Willmott

	Notes:			

*/

#include "Mat.h"
#include <ctype.h>
#include <string.h>
#include <stdarg.h>
#include <iomanip>
#ifndef __SVL__
#include "Array.h"
#endif


// --- Mat Constructors & Destructors -----------------------------------------


TMat::TMat(Int rows, Int cols, ZeroOrOne k) : rows(rows), cols(cols)
{
	Assert(rows > 0 && cols > 0, "(Mat) illegal matrix size");
	
	data = new TMReal[rows * cols];
	Assert(data != 0, "(Mat) Out of memory");
	
	MakeDiag(k);
}

TMat::TMat(Int rows, Int cols, Block k) : rows(rows), cols(cols)
{
	Assert(rows > 0 && cols > 0, "(Mat) illegal matrix size");
	
	data = new TMReal[rows * cols];
	Assert(data != 0, "(Mat) Out of memory");
	
	MakeBlock(k);
}

TMat::TMat(Int rows, Int cols, double elt0, ...) : rows(rows), cols(cols)
// The double is hardwired here because it is the only type that will work 
// with var args and C++ real numbers. 
{
	Assert(rows > 0 && cols > 0, "(Mat) illegal matrix size");
	
	va_list ap;
	Int 	i, j;
	
	data = new TMReal[rows * cols];
	Assert(data != 0, "(Mat) Out of memory");
	va_start(ap, elt0);
		
	SetReal(data[0], elt0);
	
	for (i = 1; i < cols; i++)
		SetReal(SELF[0][i], va_arg(ap, double));

	for (i = 1; i < rows; i++)
		for (j = 0; j < cols; j++)
			SetReal(SELF[i][j], va_arg(ap, double));

	va_end(ap);
}

TMat::TMat(const TMat &m) : cols(m.cols)
{
	Assert(m.data != 0, "(Mat) Can't construct from null matrix");
	rows = m.Rows();
	
	UInt	elts = rows * cols;
	
	data = new TMReal[elts];
	Assert(data != 0, "(Mat) Out of memory");
	memcpy(data, m.data, elts * sizeof(TMReal));
}

#ifndef __SVL__
TMat::TMat(const TSubMat &m) : rows(m.Rows()), cols(m.Cols())
{
	data = new TMReal[rows * cols];
	Assert(data != 0, "(Mat) Out of memory");

	for (Int i = 0; i < Rows(); i++) 
		SELF[i] = m[i];
}
#endif

TMat::TMat(const TMat2 &m) : rows(2 | VL_REF_FLAG), cols(2), data(m.Ref())
{
}

TMat::TMat(const TMat3 &m) : rows(3 | VL_REF_FLAG), cols(3), data(m.Ref())
{
}

TMat::TMat(const TMat4 &m) : rows(4 | VL_REF_FLAG), cols(4), data(m.Ref())
{
}


// --- Mat Assignment Operators -----------------------------------------------


TMat &TMat::operator = (const TMat &m)	
{	
	if (!IsRef())
		SetSize(m.Rows(), m.Cols());
	else
		Assert(Rows() == m.Rows(), "(Mat::=) Matrix rows don't match");
	for (Int i = 0; i < Rows(); i++) 
		SELF[i] = m[i];

    return(SELF);
}
	  
#ifndef __SVL__
TMat &TMat::operator = (const TSubMat &m)
{	
	if (!IsRef())
		SetSize(m.Rows(), m.Cols());
	else
		Assert(Rows() == m.Rows(), "(Mat::=) Matrix rows don't match");
	for (Int i = 0; i < Rows(); i++) 
		SELF[i] = m[i];

    return(SELF);
}
#endif
	  
TMat &TMat::operator = (const TMat2 &m)
{	
	if (!IsRef())
		SetSize(m.Rows(), m.Cols());
	else
		Assert(Rows() == m.Rows(), "(Mat::=) Matrix rows don't match");
	for (Int i = 0; i < Rows(); i++) 
		SELF[i] = m[i];

    return(SELF);
}
	  
TMat &TMat::operator = (const TMat3 &m)
{	
	if (!IsRef())
		SetSize(m.Rows(), m.Cols());
	else
		Assert(Rows() == m.Rows(), "(Mat::=) Matrix rows don't match");
	for (Int i = 0; i < Rows(); i++) 
		SELF[i] = m[i];

    return(SELF);
}
	  
TMat &TMat::operator = (const TMat4 &m)
{	
	if (!IsRef())
		SetSize(m.Rows(), m.Cols());
	else
		Assert(Rows() == m.Rows(), "(Mat::=) Matrix rows don't match");

	for (Int i = 0; i < Rows(); i++) 
		SELF[i] = m[i];

    return(SELF);
}

Void TMat::SetSize(Int nrows, Int ncols)
{
	UInt	elts = nrows * ncols;
	Assert(elts > 0, "(Mat::SetSize) Illegal matrix size.");
	UInt	oldElts = Rows() * Cols();
	Bool	wasRef = IsRef();
	
	rows = nrows;
	cols = ncols;

	if (wasRef)
	{
		// Do we already have enough storage?
		rows |= VL_REF_FLAG;
		if (elts <= oldElts)
			return;

		_Error("(Mat::SetSize) Trying to increase size of a reference vector");

		delete[] data;
	}

	data = new TMReal[elts];
	Assert(data != 0, "(Mat::SetSize) Out of memory");
}

Void TMat::SetSize(const TMat &m)
{
	SetSize(m.Rows(), m.Cols());
}

Void TMat::MakeZero()
{
	Int		i, j;
	
	for (i = 0; i < Rows(); i++)
		for (j = 0; j < Cols(); j++)
			Elt(i,j) = vl_zero;		
}

Void TMat::MakeDiag(TMReal k)
{
	Int		i, j;
	
	for (i = 0; i < Rows(); i++)
		for (j = 0; j < Cols(); j++)
			if (i == j) 
				Elt(i,j) = k;
			else
				Elt(i,j) = vl_zero;		
}

Void TMat::MakeDiag()
{
	Int		i, j;
	
	for (i = 0; i < Rows(); i++)
		for (j = 0; j < Cols(); j++)
			Elt(i,j) = (i == j) ? vl_one : vl_zero;		
}

Void TMat::MakeBlock(TMReal k)
{
	Int		i, j;
	
	for (i = 0; i < Rows(); i++)
		for (j = 0; j < Cols(); j++)
			Elt(i,j) = k;		
}

Void TMat::MakeBlock()
{
	Int		i, j;
	
	for (i = 0; i < Rows(); i++)
		for (j = 0; j < Cols(); j++)
			Elt(i,j) = vl_one;		
}


// --- Mat Assignment Operators -----------------------------------------------


TMat &operator += (TMat &m, const TMat &n)
{
	Assert(n.Rows() == m.Rows(), "(Mat::+=) matrix rows don't match");	
	
	Int		i;
	TMVec 	t;
	
	for (i = 0; i < n.Rows(); i++) 
		m[i] += n[i];
	
	return(m);
}

TMat &operator -= (TMat &m, const TMat &n)
{
	Assert(n.Rows() == m.Rows(), "(Mat::-=) matrix rows don't match");	
	
	Int		i;
	TMVec 	t;
	
	for (i = 0; i < m.Rows(); i++) 
		m[i] -= n[i];
	
	return(m);
}

TMat &operator *= (TMat &m, const TMat &n)
{
	Assert(m.Cols() == n.Cols(), "(Mat::*=) matrix columns don't match");	
	
	Int		i;
	TMVec 	t;
	
	for (i = 0; i < m.Rows(); i++) 
		m[i] *= n;
	
	return(m);
}

TMat &operator *= (TMat &m, TMReal s)
{	
	Int		i;
	TMVec 	t;
	
	for (i = 0; i < m.Rows(); i++) 
		m[i] *= s;
	
	return(m);
}

TMat &operator /= (TMat &m, TMReal s)
{	
	Int		i;
	TMVec 	t;
	
	for (i = 0; i < m.Rows(); i++) 
		m[i] /= s;
	
	return(m);
}


// --- Mat Comparison Operators -----------------------------------------------


Bool operator == (const TMat &m, const TMat &n)
{
	Assert(n.Rows() == m.Rows(), "(Mat::==) matrix rows don't match");	
	
	Int		i;
	
	for (i = 0; i < m.Rows(); i++) 
		if (m[i] != n[i])
			return(0);

	return(1);
}

Bool operator != (const TMat &m, const TMat &n)
{
	Assert(n.Rows() == m.Rows(), "(Mat::!=) matrix rows don't match");	
	
	Int		i;
	
	for (i = 0; i < m.Rows(); i++) 
		if (m[i] != n[i])
			return(1);

	return(0);
}


// --- Mat Arithmetic Operators -----------------------------------------------


TMat operator + (const TMat &m, const TMat &n)
{
	Assert(n.Rows() == m.Rows(), "(Mat::+) matrix rows don't match");	
	
	TMat	result(m.Rows(), m.Cols());
	Int		i;
	
	for (i = 0; i < m.Rows(); i++) 
		result[i] = m[i] + n[i];
	
	return(result);
}

TMat operator - (const TMat &m, const TMat &n)
{
	Assert(n.Rows() == m.Rows(), "(Mat::-) matrix rows don't match");	
	
	TMat	result(m.Rows(), m.Cols());
	Int		i;
	
	for (i = 0; i < m.Rows(); i++) 
		result[i] = m[i] - n[i];
	
	return(result);
}

TMat operator - (const TMat &m)
{
	TMat	result(m.Rows(), m.Cols());
	Int		i;
	
	for (i = 0; i < m.Rows(); i++) 
		result[i] = -m[i];
	
	return(result);
}

TMat operator * (const TMat &m, const TMat &n)
{
	Assert(m.Cols() == n.Rows(), "(Mat::*m) matrix cols don't match");	
	
	TMat	result(m.Rows(), n.Cols());
	Int		i;
	
	for (i = 0; i < m.Rows(); i++) 
		result[i] = m[i] * n;
	
	return(result);
}

TMVec operator * (const TMat &m, const TMVec &v)
{
	Assert(m.Cols() == v.Elts(), "(Mat::*v) matrix and vector sizes don't match");
	
	Int		i;
	TMVec	result(m.Rows());
	
	for (i = 0; i < m.Rows(); i++) 
		result[i] = dot(v, m[i]);
	
	return(result);
}

TMat operator * (const TMat &m, TMReal s)
{
	Int		i;
	TMat	result(m.Rows(), m.Cols());
	
	for (i = 0; i < m.Rows(); i++) 
		result[i] = m[i] * s;
	
	return(result);
}

TMat operator / (const TMat &m, TMReal s)
{
	Int		i;
	TMat	result(m.Rows(), m.Cols());
	
	for (i = 0; i < m.Rows(); i++) 
		result[i] = m[i] / s;
	
	return(result);
}


// --- Mat Mat-Vec Functions --------------------------------------------------


TMVec operator * (const TMVec &v, const TMat &m)			// v * m
{
	Assert(v.Elts() == m.Rows(), "(Mat::v*m) vector/matrix sizes don't match");
	
	TMVec 	result(m.Cols(), vl_zero);
	Int		i;
	
	for (i = 0; i < m.Rows(); i++) 
		result += m[i] * v[i];
	
	return(result);
}

TMVec &operator *= (TMVec &v, const TMat &m)				// v *= m
{
	v = v * m;		// Can't optimise much here...
	
	return(v);
}


// --- Mat Special Functions --------------------------------------------------


TMat trans(const TMat &m)
{
	Int		i,j;
	TMat	result(m.Cols(), m.Rows());
	
	for (i = 0; i < m.Rows(); i++) 
		for (j = 0; j < m.Cols(); j++)
			result.Elt(j,i) = m.Elt(i,j);
	
	return(result);
}

TMReal trace(const TMat &m)
{
	Int		i;
	TMReal 	result = vl_0;
	
	for (i = 0; i < m.Rows(); i++) 
		result += m.Elt(i,i);
	
	return(result);
}

TMat &clamp(TMat &m, Real fuzz)
//	clamps all values of the matrix with a magnitude
//	smaller than fuzz to zero.
{
	Int i, j;

	for (i = 0; i < m.Rows(); i++)
		for (j = 0; j < m.Cols(); j++)
			if (len(m[i][j]) < fuzz)
				m[i][j] = vl_zero;
			
	return(m);
}

TMat clamped(const TMat &m, Real fuzz)
//	clamps all values of the matrix with a magnitude
//	smaller than fuzz to zero.
{
	Int		i, j;
	TMat	result(m);
	
	for (i = 0; i < result.Rows(); i++)
		for (j = 0; j < result.Cols(); j++)
			if (len(result[i][j]) < fuzz)
				result[i][j] = vl_zero;
			
	return(result);
}

TMat &clamp(TMat &m)
{
	return(clamp(m, 1e-7));
}

TMat clamped(const TMat &m)
{
	return(clamped(m, 1e-7));
}

TMat oprod(const TMVec &a, const TMVec &b)
// returns outerproduct of a and b:  a * trans(b)
{
	TMat	result;
	Int		i;
	
	result.SetSize(a.Elts(), b.Elts());
	for (i = 0; i < a.Elts(); i++)
		result[i] = a[i] * b;

	return(result);
}


// --- Mat Input & Output -----------------------------------------------------


std::ostream	&operator << (std::ostream &s, const TMat &m)
{
	Int i, w = s.width();
	
	s << '[';
	for (i = 0; i < m.Rows() - 1; i++)
		s << std::setw(w) << m[i] << std::endl;
	s << std::setw(w) << m[i] << ']' << std::endl;
	return(s);
}

#ifndef __SVL__
istream	&operator >> (istream &s, TMat &m)
{
	Array< Array<TMReal> > array;	
    Int		i;
	
	s >> array;						// Read input into array of arrays
	
	m.SetSize(array.NumItems(), array[0].NumItems());
	
	for (i = 0; i < m.Rows(); i++)	// copy the result into m
	{
		Assert(m.Cols() == array[i].NumItems(), "(Mat/>>) different sized matrix rows");
		m[i] = TMVec(m.Cols(), array[i].Ref());
	}
	
    return(s);
}
#endif

//
//	inv: matrix inversion using Gaussian pivoting
//

#if !defined(CL_CHECKING) && !defined(VL_CHECKING)
// we #define away pAssertEps if it is not used, to avoid
// compiler warnings.
#define pAssertEps
#endif

TMat inv(const TMat &m, TMReal *determinant, TMReal pAssertEps)
{
	Assert(m.IsSquare(), "(inv) Matrix not square");

    Int				i, j, k;
    Int				n = m.Rows();
    TMReal			t, pivot, det;
    Real			max;
    TMat			A(m);
    TMat			B(n, n, vl_I);		

    // ---------- Forward elimination ---------- ------------------------------
	
    det = vl_1;
    
    for (i = 0; i < n; i++)				// Eliminate in column i, below diag
    {		
		max = -1.0;
		
		for (k = i; k < n; k++)			// Find a pivot for column i 
		    if (len(A[k][i]) > max)
		    {
				max = len(A[k][i]);
				j = k;
		    }
		    
		Assert(max > pAssertEps, "(inv) Matrix not invertible");
					
		if (j != i)						// Swap rows i and j
		{			
		    for (k = i; k < n; k++)
				Swap(A.Elt(i, k), A.Elt(j, k));
		    for (k = 0; k < n; k++)
				Swap(B.Elt(i, k), B.Elt(j, k));
				
		    det = -det;
		}
		
		pivot = A.Elt(i, i);
		Assert(abs(pivot) > pAssertEps, "(inv) Matrix not invertible");
		det *= pivot;
		
		for (k = i + 1; k < n; k++)		// Only do elements to the right of the pivot 
		    A.Elt(i, k) /= pivot;
		    
		for (k = 0; k < n; k++)
		    B.Elt(i, k) /= pivot;
		    
		// We know that A(i, i) will be set to 1, so don't bother to do it
	
		for (j = i + 1; j < n; j++)
		{								// Eliminate in rows below i 
		    t = A.Elt(j, i);			// We're gonna zero this guy 
		    for (k = i + 1; k < n; k++)	// Subtract scaled row i from row j 
				A.Elt(j, k) -= A.Elt(i, k) * t;	// (Ignore k <= i, we know they're 0) 
		    for (k = 0; k < n; k++)
				B.Elt(j, k) -= B.Elt(i, k) * t;
		}
    }

    // ---------- Backward elimination ---------- -----------------------------

    for (i = n - 1; i > 0; i--)			// Eliminate in column i, above diag 
    {		
		for (j = 0; j < i; j++)			// Eliminate in rows above i 
		{		
	    	t = A.Elt(j, i);			// We're gonna zero this guy 
	    	for (k = 0; k < n; k++)		// Subtract scaled row i from row j 
				B.Elt(j, k) -= B.Elt(i, k) * t;
		}
    }
    if (determinant)
		*determinant = det;
    return(B);
}

