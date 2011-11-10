/*===================================================================================
                                    CarDesigner
                         Copyright Kerry R. Loux 2008-2011

     No requirement for distribution of wxWidgets libraries, source, or binaries.
                             (http://www.wxwidgets.org/)

===================================================================================*/

// File:  triangle.h
// Created:  5/14/2009
// Author:  K. Loux
// Description:  Derived from PRIMITIVE for creating triangular objects.
// History:

#ifndef _TRIANGLE_H_
#define _TRIANGLE_H_

// Local headers
#include "vRenderer/primitives/primitive.h"
#include "vMath/vector.h"

class TRIANGLE : public Primitive
{
public:
	// Constructor
	TRIANGLE(RenderWindow &_RenderWindow);

	// Destructor
	~TRIANGLE();

	// Mandatory overloads from PRIMITIVE - for creating geometry and testing the
	// validity of this object's parameters
	void GenerateGeometry(void);
	bool HasValidParameters(void);

	// Private data accessors
	void SetCorner1(const Vector &_Corner1);
	void SetCorner2(const Vector &_Corner2);
	void SetCorner3(const Vector &_Corner3);

private:
	// The locations of the triangle's corners
	Vector Corner1;
	Vector Corner2;
	Vector Corner3;
};

#endif// _TRIANGLE_H_